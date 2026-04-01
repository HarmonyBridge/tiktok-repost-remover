// ============================================================================
// Color Tokens - TikTok Repost Remover
// ============================================================================
// NO colors outside this file - this is the law
// Primary: #8B2D50 (burgundy from logo background)
// Accent: #C41E6A (crimson accent)
// ============================================================================

export const Colors = {
  // ===== Primary Palette (من اللوجو - خلفية الأيقونة) =====
  primary: {
    50:  '#FDF2F5',   // أفتح درجة
    100: '#F9E6EC',
    200: '#F0C2D2',
    300: '#E08CA8',
    400: '#C4557A',   // ← لون متوسط
    500: '#8B2D50',   // ← اللون الأساسي (خلفية اللوجو)
    600: '#7A2444',
    700: '#691C39',
    800: '#58152F',
    900: '#470F25',
  },

  // ===== Accent Palette (لون مكمل) =====
  accent: {
    100: '#F9D0E0',
    200: '#F2A1C1',
    300: '#E86B96',
    400: '#D94480',
    500: '#C41E6A',   // ← اللون الثانوي
    600: '#A31759',
    700: '#831247',
    800: '#630D36',
    900: '#430824',
  },

  // ===== Neutrals (White Mode) =====
  neutral: {
    0:   '#FFFFFF',   // خلفية التطبيق الأساسية
    50:  '#F8FAFC',   // Surface
    100: '#F1F5F9',   // Card background
    200: '#E2E8F0',   // Divider
    300: '#CBD5E1',   // Border
    400: '#94A3B8',   // Placeholder
    500: '#64748B',   // Subtle text
    600: '#475569',   // Secondary text
    700: '#334155',   // Body text
    800: '#1E293B',   // Primary text
    900: '#0F172A',   // Headings
  },

  // ===== Semantic Colors =====
  semantic: {
    success: '#10B981',
    warning: '#F59E0B',
    error:   '#EF4444',
    info:    '#8B2D50',  // = primary.500
  },

  // ===== App Semantic Aliases =====
  background:  '#FFFFFF',
  surface:     '#FDF2F5',   // خلفية وردية فاتحة
  text:        '#1E293B',
  textMuted:   '#64748B',
  border:      '#F0C2D2',   // حدود وردية
  ctaPrimary:  '#8B2D50',   // ← الأزرار الأساسية (برغندي)
  ctaAccent:   '#C41E6A',   // ← الأزرار الثانوية / CTA القوي
} as const;

// Type-safe color tokens
export type ColorToken = typeof Colors;
