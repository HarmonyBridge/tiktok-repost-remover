/**
 * @fileoverview Card with status + counter
 */

import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { Colors } from '../../theme/colors';
import { Typography } from '../../theme/typography';
import { CounterDisplay } from './CounterDisplay';
import { StatusBadge } from './StatusBadge';

interface ProgressCardProps {
  count: number;
  status: 'idle' | 'loading' | 'running' | 'done' | 'error' | 'rate_limited';
  message?: string;
}

export function ProgressCard({ count, status, message }: ProgressCardProps): JSX.Element {
  return (
    <View style={styles.container}>
      {count > 0 && <CounterDisplay count={count} />}
      <StatusBadge status={status} message={message} />
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    backgroundColor: Colors.surface,
    borderRadius: 16,
    borderWidth: 1,
    borderColor: Colors.border,
    padding: 20,
    alignItems: 'center',
    gap: 12,
  },
});
