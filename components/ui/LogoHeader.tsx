/**
 * @fileoverview App logo + title header component
 */

import React from 'react';
import { View, Text, StyleSheet } from 'react-native';
import { LinearGradient } from 'expo-linear-gradient';
import { Colors } from '../../theme/colors';
import { Typography } from '../../theme/typography';

interface LogoHeaderProps {
  size?: 'sm' | 'md' | 'lg';
}

const sizes = {
  sm: { container: 48, icon: 20, fontSize: Typography.fontSize.lg },
  md: { container: 64, icon: 28, fontSize: Typography.fontSize.xl },
  lg: { container: 80, icon: 36, fontSize: Typography.fontSize['2xl'] },
};

export function LogoHeader({ size = 'md' }: LogoHeaderProps): JSX.Element {
  const s = sizes[size];

  return (
    <View style={styles.container}>
      <View style={[styles.logo, { width: s.container, height: s.container, borderRadius: s.container * 0.25 }]}>
        <LinearGradient
          colors={[Colors.primary[400], Colors.accent[500]]}
          start={{ x: 0, y: 0 }}
          end={{ x: 1, y: 1 }}
          style={styles.gradient}
        >
          <Text style={[styles.icon, { fontSize: s.icon }]}>✦</Text>
        </LinearGradient>
      </View>
      <Text style={[styles.title, { fontSize: s.fontSize }]}>TikTok Repost Remover</Text>
    </View>
  );
}

const styles = StyleSheet.create({
  container: {
    alignItems: 'center',
    gap: 12,
  },
  logo: {
    overflow: 'hidden',
  },
  gradient: {
    flex: 1,
    alignItems: 'center',
    justifyContent: 'center',
  },
  icon: {
    color: '#fff',
  },
  title: {
    fontWeight: Typography.fontWeight.extrabold,
    color: Colors.neutral[900],
    textAlign: 'center',
  },
});
