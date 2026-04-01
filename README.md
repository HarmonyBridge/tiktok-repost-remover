# TikTok Repost Remover

An Android app that automatically removes all your TikTok reposts in one tap.

## Features

- **100% Free** - No subscriptions, no ads
- **Private** - Everything happens on your device only
- **Fast** - Automated removal with human-like delays
- **Safe** - Your credentials are never stored or transmitted
- **Resumable** - Pause and resume sessions anytime
- **Hot-Reload** - Automatic selector updates without app reinstall

## How It Works

1. **Open the app** → TikTok loads inside the app
2. **Log in** → Your TikTok account (credentials stay on device)
3. **Start Removing** → Tap the button to begin automation
4. **Watch it work** → All reposts disappear automatically
5. **Resume anytime** → If interrupted, resume from where you left off

### What Happens Behind the Scenes
- The app injects a JavaScript automation script into the TikTok WebView
- The script navigates to your profile, finds the Reposts tab
- For each repost, it opens the share menu and clicks "Remove Repost"
- Human-like delays prevent TikTok from rate-limiting
- Progress is saved locally for session resumption

## Tech Stack

- React Native + Expo SDK 51
- TypeScript (strict mode)
- react-native-webview for TikTok automation
- react-native-reanimated for smooth animations
- AsyncStorage for session persistence
- expo-router for navigation

## Project Structure

```
tiktok-repost-remover/
├── app/                    # expo-router screens
│   ├── _layout.tsx
│   ├── index.tsx          # Home screen
│   ├── webview.tsx        # WebView + Control panel
│   └── result.tsx         # Final results screen
├── components/
│   ├── ui/                # Reusable UI components
│   └── automation/        # Automation components
├── hooks/
│   ├── useAutomation.ts   # Core automation state machine
│   └── useWebView.ts      # WebView helpers
├── utils/
│   ├── injectedScript.ts  # JS injected into TikTok
│   ├── selectors.ts       # TikTok DOM selectors
│   ├── remoteSelectors.ts # Hot-reload selectors system
│   ├── sessionStorage.ts  # Session persistence
│   ├── delays.ts          # Delay generator
│   └── messageParser.ts   # Parse WebView messages
├── theme/
│   ├── colors.ts          # Color tokens
│   ├── typography.ts      # Font scale
│   └── animations.ts      # Reanimated presets
├── constants/
│   ├── config.ts          # App config
│   └── strings.ts         # User-facing strings
├── selectors.json         # Remote selectors config
└── specs/                 # spec-kit documentation
    ├── constitution.md
    ├── spec.md
    ├── plan.md
    ├── improvement_plan.md
    └── tasks/
```

## Development

### Prerequisites

- Node.js 18+
- npm or yarn
- Android device or emulator

### Setup

```bash
# Install dependencies
npm install

# Start development server
npx expo start

# Scan QR code with Expo Go app on Android
```

### Build

```bash
# Install EAS CLI
npm install -g eas-cli

# Login to Expo
eas login

# Build preview APK
npm run build:preview

# Build production AAB
npm run build:production
```

## Design System

- **Primary Color:** #7DBDE8 (blue from logo)
- **Accent Color:** #C41E6A (red/crimson from logo)
- **Background:** #FFFFFF (white mode only)
- **Surface:** #F8FAFC

## Privacy & Compliance

This app does not collect, store, or transmit any personal data.
All operations occur locally on your device.
Your TikTok credentials are never accessed by this app.

**Full Privacy Policy:** See [PRIVACY_POLICY.md](./PRIVACY_POLICY.md)

### What We Don't Collect
- ❌ Your TikTok username or password
- ❌ Your video content or metadata
- ❌ Analytics or crash reports
- ❌ Device identifiers or location data
- ❌ Any personally identifiable information

## Recent Improvements (v2.0)

- ✅ Session resume functionality
- ✅ Hot-reload DOM selectors system
- ✅ Enhanced login detection (5 methods)
- ✅ Exponential backoff for rate limiting
- ✅ Memory leak prevention
- ✅ WebView crash recovery
- ✅ TypeScript strict mode compliance
- ✅ Privacy-first architecture
- ✅ Fixed result screen count display
- ✅ Improved error handling and HTTP status codes

## Disclaimer

Not affiliated with TikTok or ByteDance.

## License

MIT
