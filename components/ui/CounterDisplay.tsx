/**
 * @fileoverview Animated number counter component
 */

import React, { useEffect } from 'react';
import { View, Text, StyleSheet } from 'react-native';
import Animated, { useSharedValue, useAnimatedStyle, withSpring, withSequence } from 'react-native-reanimated';
import { Colors } from '../../theme/colors';
import { Typography } from '../../theme/typography';

interface CounterDisplayProps {
  count: number;
  label?: string;
}

export function CounterDisplay({ count, label = 'removed' }: CounterDisplayProps): JSX.Element {
  const scale = useSharedValue(1);

  useEffect(() => {
    if (count > 0) {
      scale.value = withSequence(
        withSpring(1.15, { damping: 10, stiffness: 300 }),
        withSpring(1.0,  { damping: 15, stiffness: 200 })
      );
    }
  }, [count]);

  const animStyle = useAnimatedStyle(() => ({
    transform: [{ scale: scale.value }],
  }));

  return (
    <View style={styles.wrapper}>
      <Animated.Text style={[styles.number, animStyle]}>
        {count}
      </Animated.Text>
      <Text style={styles.label}>{label}</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  wrapper: {
    alignItems: 'center',
    marginBottom: 8,
  },
  number: {
    fontSize: Typography.fontSize['4xl'],
    fontWeight: Typography.fontWeight.extrabold,
    color: Colors.accent[500],
    lineHeight: 52,
  },
  label: {
    fontSize: Typography.fontSize.sm,
    color: Colors.neutral[400],
  },
});
