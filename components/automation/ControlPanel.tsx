/**
 * @fileoverview Start/Stop + progress UI
 */

import React from 'react';
import { View, Text, StyleSheet, TouchableOpacity } from 'react-native';
import { LinearGradient } from 'expo-linear-gradient';
import * as Haptics from 'expo-haptics';
import { Colors } from '../../theme/colors';
import { Typography } from '../../theme/typography';
import { CounterDisplay } from '../ui/CounterDisplay';

interface ControlPanelProps {
  status: 'idle' | 'loading' | 'running' | 'done' | 'error' | 'rate_limited';
  count: number;
  message: string;
  onStart: () => void;
  onStop: () => void;
  onDone: () => void;
}

export function ControlPanel({
  status,
  count,
  message,
  onStart,
  onStop,
  onDone,
}: ControlPanelProps): JSX.Element {
  const showStart = ['ready', 'error', 'rate_limited'].includes(status);
  const showStop = status === 'running';
  const showDone = status === 'done';

  const handleStart = () => {
    Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Heavy);
    onStart();
  };

  const handleStop = () => {
    Haptics.impactAsync(Haptics.ImpactFeedbackStyle.Medium);
    onStop();
  };

  return (
    <View style={styles.container}>
      {count > 0 && <CounterDisplay count={count} />}

      <Text style={styles.message} numberOfLines={2}>
        {message}
      </Text>

      <View style={styles.actions}>
        {showStart && (
          <TouchableOpacity onPress={handleStart} activeOpacity={0.85} style={styles.btnWrapper}>
            <LinearGradient
              colors={[Colors.accent[500], Colors.accent[700]]}
              start={{ x: 0, y: 0 }}
              end={{ x: 1, y: 0 }}
              style={styles.startBtn}
            >
              <Text style={styles.btnText}>
                {status === 'rate_limited' ? '▶ Resume' : '▶ Start Removing'}
              </Text>
            </LinearGradient>
          </TouchableOpacity>
        )}

        {showStop && (
          <TouchableOpacity onPress={handleStop} style={styles.stopBtn}>
            <Text style={styles.stopText}>Stop</Text>
          </TouchableOpacity>
        )}

        {showDone && (
          <TouchableOpacity onPress={onDone} activeOpacity={0.85} style={styles.btnWrapper}>
            <LinearGradient
              colors={['#10B981', '#059669']}
              start={{ x: 0, y: 0 }}
              end={{ x: 1, y: 0 }}
              style={styles.doneBtn}
            >
              <Text style={styles.btnText}>View Results →</Text>
            </LinearGradient>
          </TouchableOpacity>
        )}
      </View>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    backgroundColor: Colors.background,
    paddingHorizontal: 16,
    paddingTop: 12,
    paddingBottom: 24,
    borderTopWidth: 1,
    borderTopColor: Colors.border,
    minHeight: 130,
    maxHeight: 220,
  },
  message: {
    fontSize: Typography.fontSize.sm,
    color: Colors.neutral[500],
    textAlign: 'center',
    marginBottom: 12,
    lineHeight: 18,
  },
  actions: {
    alignItems: 'center',
    gap: 10,
  },
  btnWrapper: {
    width: '100%',
    borderRadius: 14,
    overflow: 'hidden',
  },
  startBtn: {
    paddingVertical: 16,
    alignItems: 'center',
    borderRadius: 14,
  },
  doneBtn: {
    paddingVertical: 16,
    alignItems: 'center',
    borderRadius: 14,
  },
  btnText: {
    color: '#fff',
    fontSize: Typography.fontSize.md,
    fontWeight: Typography.fontWeight.bold,
  },
  stopBtn: {
    paddingVertical: 12,
    paddingHorizontal: 32,
    borderRadius: 12,
    borderWidth: 1,
    borderColor: Colors.border,
  },
  stopText: {
    color: Colors.neutral[500],
    fontSize: Typography.fontSize.base,
    fontWeight: Typography.fontWeight.medium,
  },
});
