// ============================================================================
// Typography Scale - TikTok Repost Remover
// ============================================================================

export const Typography = {
  // Font families
  fontFamily: {
    // استخدام system fonts - أسرع للتحميل ومناسب للعربي
    regular:  'System',
    medium:   'System',
    bold:     'System',
  },

  // Scale
  fontSize: {
    xs:   11,
    sm:   13,
    base: 15,
    md:   17,
    lg:   20,
    xl:   24,
    '2xl': 28,
    '3xl': 34,
    '4xl': 44,
  },

  fontWeight: {
    regular:     '400' as const,
    medium:      '500' as const,
    semibold:    '600' as const,
    bold:        '700' as const,
    extrabold:   '800' as const,
  },

  lineHeight: {
    tight:  1.2,
    normal: 1.5,
    loose:  1.8,
  },
} as const;
