/**
 * @fileoverview WebView ref + injection helpers
 */

import { useRef, useCallback } from 'react';
import WebView from 'react-native-webview';

export function useWebView() {
  const webViewRef = useRef<WebView>(null);

  const injectJavaScript = useCallback((script: string) => {
    if (webViewRef.current) {
      webViewRef.current.injectJavaScript(script);
    }
  }, []);

  const reload = useCallback(() => {
    if (webViewRef.current) {
      webViewRef.current.reload();
    }
  }, []);

  const goBack = useCallback(() => {
    if (webViewRef.current) {
      webViewRef.current.goBack();
    }
  }, []);

  return {
    webViewRef,
    injectJavaScript,
    reload,
    goBack,
  };
}
