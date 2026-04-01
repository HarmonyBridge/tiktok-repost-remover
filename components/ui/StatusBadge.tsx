/**
 * @fileoverview Status indicator component
 */

import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { Colors } from '../../theme/colors';
import { Typography } from '../../theme/typography';

interface StatusBadgeProps {
  status: 'idle' | 'loading' | 'running' | 'done' | 'error' | 'rate_limited';
  message?: string;
}

const statusConfig = {
  idle:         { color: Colors.neutral[400], label: 'Ready' },
  loading:      { color: Colors.primary[400], label: 'Loading' },
  running:      { color: Colors.accent[500], label: 'Running' },
  done:         { color: Colors.semantic.success, label: 'Done' },
  error:        { color: Colors.semantic.error, label: 'Error' },
  rate_limited: { color: Colors.semantic.warning, label: 'Paused' },
};

export function StatusBadge({ status, message }: StatusBadgeProps): JSX.Element {
  const config = statusConfig[status];

  return (
    <View style={styles.container}>
      <View style={[styles.dot, { backgroundColor: config.color }]} />
      <Text style={[styles.label, { color: config.color }]}>
        {message || config.label}
      </Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    flexDirection: 'row',
    alignItems: 'center',
    gap: 8,
  },
  dot: {
    width: 8,
    height: 8,
    borderRadius: 4,
  },
  label: {
    fontSize: Typography.fontSize.sm,
    fontWeight: Typography.fontWeight.medium,
  },
});
