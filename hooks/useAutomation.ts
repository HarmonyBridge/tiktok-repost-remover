/**
 * @fileoverview Core automation state machine.
 * Manages all states of the repost removal process.
 * Single source of truth for automation state.
 */

import { useState, useRef, useCallback, RefObject, useEffect } from 'react';
import WebView from 'react-native-webview';
import { buildInjectedScript } from '../utils/injectedScript';
import { TIKTOK } from '../constants/config';
import { saveSession, clearSession, hasResumableSession, resumeSession } from '../utils/sessionStorage';

// ============================================================
// Types
// ============================================================
export type AutomationStatus =
  | 'idle'         // App just opened
  | 'loading'      // WebView loading
  | 'login_needed' // Not logged in
  | 'ready'        // Logged in, ready to start
  | 'running'      // Actively removing
  | 'navigating'   // Going to profile
  | 'rate_limited' // TikTok rate limit hit
  | 'done'         // Finished successfully
  | 'error';       // Fatal error

export type MessageType =
  | 'STATUS'
  | 'STARTED'
  | 'PROGRESS'
  | 'DONE'
  | 'ERROR'
  | 'WARNING'
  | 'RATE_LIMITED'
  | 'NAVIGATING'
  | 'HEALTH_CHECK';

export interface AutomationMessage {
  type:       MessageType;
  message?:   string;
  count?:     number;
  code?:      string;
  timestamp?: number;
  passed?:    boolean;
  results?:   Array<{ name: string; found: boolean }>;
}

export interface AutomationState {
  status:         AutomationStatus;
  statusMessage:  string;
  removedCount:   number;
  isRunning:      boolean;
  canStart:       boolean;
  error:          string | null;
}

// ============================================================
// Hook
// ============================================================
export function useAutomation(webViewRef: RefObject<WebView>) {
  const [state, setState] = useState<AutomationState>({
    status:        'idle',
    statusMessage: 'Log into TikTok, then tap "Start Removing"',
    removedCount:  0,
    isRunning:     false,
    canStart:      false,
    error:         null,
  });

  const sessionCount = useRef(0);
  
  // Bug #2 Fix: Navigation race condition flags
  const isNavigating = useRef(false);
  const pendingInject = useRef(false);
  const hasAutoReloaded = useRef(false);

  // ---- Derived helpers ----
  function update(partial: Partial<AutomationState>) {
    setState(prev => ({ ...prev, ...partial }));
  }

  // ---- Initialize session on mount ----
  useEffect(() => {
    (async () => {
      try {
        const hasResumable = await hasResumableSession();
        if (hasResumable) {
          const previousCount = await resumeSession();
          sessionCount.current = previousCount;
          update({
            removedCount: previousCount,
            statusMessage: `Resume previous session (${previousCount} removed)?`,
            canStart: true,
            status: 'ready',
          });
        }
      } catch (e) {
        console.warn('[TRR] Failed to check resumable session:', e);
      }
    })();
  }, []);

  // ---- Handle messages from WebView ----
  // Bug #3 Fix: JSON parse crash protection
  const handleMessage = useCallback((raw: string) => {
    // Guard: Check if message is from our script
    if (!raw || typeof raw !== 'string' || !raw.includes('"type"')) {
      return; // Ignore messages not from our script (e.g., TikTok's own messages)
    }

    let msg: AutomationMessage;
    try {
      msg = JSON.parse(raw);
    } catch {
      // Invalid JSON — ignore
      return;
    }

    // Validate message type
    const validTypes: MessageType[] = ['STATUS', 'STARTED', 'PROGRESS', 'DONE', 'ERROR', 'WARNING', 'RATE_LIMITED', 'NAVIGATING', 'HEALTH_CHECK'];
    if (!msg.type || !validTypes.includes(msg.type)) {
      return; // Unknown message type
    }

    switch (msg.type) {
      case 'STATUS':
        update({ statusMessage: msg.message || '' });
        break;

      case 'NAVIGATING':
        // Bug #2 Fix: Set navigation flag
        isNavigating.current = true;
        pendingInject.current = true;
        update({
          status:        'navigating',
          statusMessage: msg.message || 'Navigating...',
        });
        break;

      case 'STARTED':
        update({
          status:        'running',
          isRunning:     true,
          statusMessage: msg.message || 'Removing reposts...',
        });
        // Save session as active
        saveSession({
          removedCount: state.removedCount,
          status: 'running',
          isActive: true,
        });
        break;

      case 'PROGRESS':
        const count = msg.count ?? state.removedCount;
        sessionCount.current = count;
        update({
          removedCount:  count,
          statusMessage: msg.message || '',
        });
        // Save session progress
        saveSession({
          removedCount: count,
          status: 'running',
          isActive: true,
        });
        break;

      case 'DONE':
        update({
          status:        'done',
          isRunning:     false,
          removedCount:  msg.count ?? sessionCount.current,
          statusMessage: msg.message || 'Done!',
          canStart:      false,
        });
        // Clear session on completion
        clearSession();
        break;

      case 'RATE_LIMITED':
        update({
          status:        'rate_limited',
          isRunning:     false,
          removedCount:  msg.count ?? sessionCount.current,
          statusMessage: msg.message || 'Rate limited. Wait 1 hour.',
          canStart:      true,  // Can resume
        });
        // Save session for resume
        saveSession({
          removedCount: msg.count ?? sessionCount.current,
          status: 'rate_limited',
          isActive: true,
        });
        break;

      case 'ERROR':
        update({
          status:        'error',
          isRunning:     false,
          statusMessage: msg.message || 'An error occurred',
          error:         msg.code || 'UNKNOWN',
          canStart:      msg.code !== 'NOT_LOGGED_IN',
        });
        break;

      case 'WARNING':
        update({ statusMessage: msg.message || '' });
        break;

      case 'HEALTH_CHECK':
        // Log health check results in development
        if (__DEV__ && msg.results) {
          console.log('[TRR] Health check:', msg.passed ? 'PASSED' : 'FAILED', msg.results);
        }
        break;
    }
  }, [state.removedCount]);

  // ---- WebView events ----
  const handleWebViewLoad = useCallback((url: string) => {
    // Bug #2 Fix: Handle pending injection after navigation
    if (isNavigating.current) {
      isNavigating.current = false;
      // If we were waiting to inject, do it now after a short delay
      if (pendingInject.current && webViewRef.current) {
        pendingInject.current = false;
        setTimeout(() => {
          webViewRef.current?.injectJavaScript(buildInjectedScript());
        }, 500); // Wait half second after load
        return;
      }
    }

    update({ status: 'loading' });

    // Auto-detect if on profile page
    if (url.match(/tiktok\.com\/@[^/]+\/?$/) || url.includes('/profile')) {
      update({
        status:        'ready',
        canStart:      true,
        statusMessage: 'Ready! Tap "Start Removing" to begin.',
      });
    }
  }, []);

  const handleWebViewLoadEnd = useCallback(() => {
    if (state.status === 'loading') {
      update({
        status:        'ready',
        canStart:      true,
        statusMessage: 'Log into TikTok, then tap "Start Removing"',
      });
    }
  }, [state.status]);

  // ---- User actions ----
  const start = useCallback(() => {
    if (!webViewRef.current) return;

    sessionCount.current = state.removedCount;
    update({
      status:        'running',
      isRunning:     true,
      statusMessage: 'Starting...',
      error:         null,
    });

    webViewRef.current.injectJavaScript(buildInjectedScript());
  }, [state.removedCount]);

  const stop = useCallback(() => {
    // Can't truly stop injected script, but update UI
    update({
      status:        'ready',
      isRunning:     false,
      statusMessage: 'Stopped. Tap Start to continue.',
    });
    // Save session state
    saveSession({
      removedCount: state.removedCount,
      status: 'paused',
      isActive: true,
    });
  }, [state.removedCount]);

  const reset = useCallback(() => {
    sessionCount.current = 0;
    update({
      status:        'ready',
      statusMessage: 'Ready to start again.',
      removedCount:  0,
      isRunning:     false,
      error:         null,
      canStart:      true,
    });
    // Clear session
    clearSession();
  }, []);

  // Defense #2: Memory leak prevention - cleanup on unmount
  useEffect(() => {
    return () => {
      // Reset navigation flags
      isNavigating.current = false;
      pendingInject.current = false;
      // Save any pending session state
      if (state.isRunning) {
        saveSession({
          removedCount: state.removedCount,
          status: 'paused',
          isActive: true,
        });
      }
    };
  }, [state.isRunning, state.removedCount]);

  return {
    state,
    handleMessage,
    handleWebViewLoad,
    handleWebViewLoadEnd,
    start,
    stop,
    reset,
    // Expose flags for debugging
    isNavigating: isNavigating.current,
    pendingInject: pendingInject.current,
    hasAutoReloaded,
  };
}
