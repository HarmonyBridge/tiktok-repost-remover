# Technical Plan — TikTok Repost Remover

## 1. Architecture Overview

```
┌─────────────────────────────────────────────────────────┐
│                    React Native App (Expo)              │
│  ┌─────────────┐    ┌─────────────┐    ┌─────────────┐  │
│  │  App Shell  │    │  WebView    │    │   Control   │  │
│  │             │◄──►│   Engine    │    │   Panel     │  │
│  │ Home Screen │    │             │    │             │  │
│  │             │    │  tiktok.com │    │ Start/Stop  │  │
│  └─────────────┘    │  (mobile)   │    │  Progress   │  │
│                     └──────┬──────┘    └─────────────┘  │
│                            ↑ injectJavaScript()          │
│                            ↓ onMessage()                 │
│  ┌─────────────────────────────────────────────────────┐ │
│  │         Core Automation Engine                      │ │
│  │  injectedScript.ts → runs inside TikTok DOM         │ │
│  │  Multi-selector strategy + random delays            │ │
│  └─────────────────────────────────────────────────────┘ │
│  ┌─────────────┐  ┌──────────────┐  ┌────────────┐       │
│  │   Design    │  │    Hooks     │  │ Constants  │       │
│  │   System    │  │ useAutomation│  │selectors,  │       │
│  │(colors,     │  │  useSession  │  │ delays, UA │       │
│  │  fonts)     │  │              │  │            │       │
│  └─────────────┘  └──────────────┘  └────────────┘       │
└─────────────────────────────────────────────────────────┘
```

## 2. Technology Stack & Rationale

| Technology | Version | Why |
|------------|---------|-----|
| React Native | 0.74.x | Cross-platform native apps |
| Expo SDK | 51 | Simplifies build + native APIs |
| expo-router | 3.5.x | File-based routing, simple |
| react-native-webview | 13.10.x | WebView with JS injection |
| expo-linear-gradient | 13.x | Gradient backgrounds |
| react-native-reanimated | 3.x | 60fps animations |
| expo-haptics | 13.x | Haptic feedback |
| AsyncStorage | latest | Persist session preferences |
| EAS Build | latest | Free cloud builds |

**Why NOT:**
- No Redux → overkill for this app size
- No Firebase → no backend needed
- No Supabase → no server data
- No REST API → everything local
- No TypeORM → no database needed

## 3. Module Breakdown

### Module 1: WebView Engine
- Loads tiktok.com with Chrome UA
- Manages cookie persistence
- Handles JS injection and message receiving
- Files: `screens/WebViewScreen.tsx`, `hooks/useWebView.ts`

### Module 2: Automation Engine
- Core JavaScript that runs inside TikTok
- Multi-selector fallback system
- Human-like delay generator
- Rate limit detection
- Files: `utils/injectedScript.ts`, `utils/selectors.ts`, `utils/delays.ts`

### Module 3: UI Layer
- Home screen (instructions + CTA)
- Control panel (progress + status + stop)
- Result screen (final summary)
- Files: `screens/`, `components/`

### Module 4: Design System
- Color tokens
- Typography scale
- Animation presets
- Files: `theme/colors.ts`, `theme/typography.ts`, `theme/animations.ts`

## 4. Data Flow

```
User taps "Start"
    ↓
useAutomation.start()
    ↓
WebView.injectJavaScript(buildScript())
    ↓
[Inside TikTok DOM] → find repost tab → click → find video → click → find share → click → find remove → click → send message to RN
    ↓
WebView.onMessage → parse JSON
    ↓
setCount() / setStatus()
    ↓
UI re-renders with new data
```

## 5. Critical Technical Decisions

### Decision 1: User-Agent Override
TikTok detects WebView via `wv` token in default UA.
Override with full Chrome Mobile UA to avoid detection.

### Decision 2: Multi-Selector Strategy
TikTok changes CSS classes frequently (confirmed by ClearTok changelog).
Every element must have 3-5 fallback selectors.
Selector priority: data-e2e → aria-label → text content → class pattern

### Decision 3: Message Protocol
All communication from injected script to RN via:
```javascript
window.ReactNativeWebView.postMessage(JSON.stringify({
  type: 'PROGRESS' | 'DONE' | 'ERROR' | 'WARNING' | 'STATUS',
  count?: number,
  message: string
}))
```

### Decision 4: Random Delays
Based on ClearTok research: fixed delays cause rate limiting.
All delays = base + Math.random() * variance.
