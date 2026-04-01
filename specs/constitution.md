# Project Constitution — TikTok Repost Remover

## 1. Project Identity
- **Name:** TikTok Repost Remover
- **Type:** Android Mobile Application
- **Platform:** React Native + Expo SDK 51
- **Target:** Android 7.0+ (API 24+)
- **Distribution:** Google Play Store

## 2. Core Principles (لا يمكن انتهاكها)

### 2.1 Privacy First
- NEVER store user credentials (sessionid, password, token)
- NEVER send personal data to any server
- ALL automation happens on-device only
- NO backend required for core functionality

### 2.2 Resilience Over Perfection
- Always use multi-selector fallback strategy
- Every DOM selector must have 3+ fallback options
- Graceful degradation when TikTok updates its UI
- Never crash silently — always report status to user

### 2.3 Human-Like Behavior
- All TikTok interactions must include random delays
- Delays range: base_delay + (Math.random() * extra_delay)
- Never use fixed delays — always randomize
- Max 30 actions per session to avoid rate limiting

### 2.4 Code Quality
- TypeScript strict mode — NO any types
- Every component must have clear Props interface
- Every utility function must have JSDoc comment
- No magic numbers — use constants file

### 2.5 Design System Compliance
- **Primary Color:** #8B2D50 (burgundy from logo)
- **Accent Color:** #C41E6A (crimson accent)
- **Background:** #FFFFFF (white mode only)
- **Surface:** #F8FAFC
- ZERO exceptions to this color system
- All animations must use react-native-reanimated v3

## 3. Amendment Process
Changes to this constitution require explicit justification and documentation of the rationale.
