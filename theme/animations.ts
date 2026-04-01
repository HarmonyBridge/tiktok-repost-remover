// ============================================================================
// Animation Presets - TikTok Repost Remover
// ============================================================================

import { withSpring, withTiming, withRepeat, withSequence, withDelay } from 'react-native-reanimated';

// Presets للاستخدام في كل مكان
export const AnimationPresets = {
  // للشعار عندما يعمل
  pulse: () => withRepeat(
    withSequence(
      withTiming(1.05, { duration: 600 }),
      withTiming(1.0,  { duration: 600 })
    ),
    -1, true
  ),

  // عندما يكبر الـ counter
  countBounce: () => withSpring(1, {
    damping: 10,
    stiffness: 200,
    mass: 0.8,
  }),

  // CTA button press
  buttonPress: () => withSpring(0.96, {
    damping: 15,
    stiffness: 300,
  }),

  // fade in عند فتح التطبيق
  fadeIn: (duration = 400) => withTiming(1, { duration }),

  // Background particles/gradient shift
  backgroundShift: () => withRepeat(
    withTiming(1, { duration: 4000 }),
    -1, true
  ),

  // Slide up animation
  slideUp: (delay = 0) => withDelay(
    delay,
    withSpring(0, { damping: 15, stiffness: 150 })
  ),

  // Scale in animation
  scaleIn: () => withSpring(1, { damping: 12, stiffness: 150 }),

  // Dot pulse for loading indicator
  dotPulse: (delay: number) => withDelay(
    delay,
    withRepeat(
      withSequence(
        withTiming(1, { duration: 400 }),
        withTiming(0.3, { duration: 400 })
      ),
      -1
    )
  ),
} as const;
