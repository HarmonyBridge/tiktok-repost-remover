# Feature Specification — TikTok Repost Remover

## 1. Problem Statement
TikTok users want to bulk-delete all their reposts.
TikTok's native app has no bulk-delete feature.
The only competitor (ClearTok) is iOS-only + Desktop Chrome Extension.
Android market is EMPTY — this is the opportunity.

## 2. User Journeys

### Journey 1: First-Time User
1. User downloads app from Google Play
2. Opens app → sees Home Screen with clear instructions
3. Taps "Open TikTok" button
4. TikTok web loads in the app's WebView
5. User logs into their TikTok account normally
6. User taps "Start Removing Reposts"
7. App automatically:
   - Navigates to user profile
   - Clicks Reposts tab
   - Removes each repost one by one
   - Shows real-time counter
8. User sees "Done! X reposts removed" screen

### Journey 2: Rate-Limited User
1. User starts removal process
2. After ~20 reposts, TikTok rate-limits
3. App detects failure, shows warning
4. App pauses and shows "Wait 1 hour, then tap Resume"
5. User resumes → continues from where it stopped

### Journey 3: No Reposts User
1. User starts process
2. App finds no reposts tab or empty list
3. App shows "You have no reposts! You're all clean ✓"

## 3. Functional Requirements

### F-01: WebView TikTok Loader
- MUST load tiktok.com in embedded WebView
- MUST use Chrome Mobile User-Agent (not WebView UA)
- MUST persist cookies between sessions
- MUST handle TikTok login normally

### F-02: Repost Detection
- MUST find Reposts tab using multi-selector strategy
- MUST detect empty state (no reposts)
- MUST count total reposts before starting

### F-03: Automated Removal Loop
- MUST remove reposts one by one
- MUST use random human-like delays
- MUST report progress in real-time
- MUST detect completion automatically
- MUST detect and handle rate limiting

### F-04: Progress UI
- MUST show real-time counter of removed reposts
- MUST show current status message
- MUST have Stop button to halt process
- MUST show final summary when done

### F-05: Session Persistence
- MUST save TikTok login session between app launches
- MUST NOT require re-login on every use

## 4. Non-Functional Requirements

### NFR-01: Performance
- App startup: < 2 seconds
- WebView load: depends on internet (not our control)
- UI responsiveness: 60fps minimum

### NFR-02: Reliability
- Must handle TikTok UI changes gracefully
- Must not crash on unexpected DOM structure
- Must provide clear error messages

### NFR-03: Security
- No credentials stored outside device
- No analytics SDK that collects personal data
- APK must pass Google Play security review

## 5. Out of Scope (explicitly excluded)
- iOS version (future phase)
- Backend/API server
- User accounts or registration
- Analytics or crash reporting (optional, privacy-first)
- Multiple social platforms
