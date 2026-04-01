# Privacy Policy — TikTok Repost Remover

**Last Updated:** March 22, 2026

## 1. Introduction

TikTok Repost Remover ("the App") is designed to help users bulk-delete reposts from their TikTok profile. This Privacy Policy explains how we handle your data.

**Key Principle:** We do not collect, store, or transmit your personal data to external servers.

---

## 2. What Data We Collect

### On Your Device (Local Only)
- **TikTok Session Cookies:** Stored in the WebView to keep you logged in between app sessions.
- **Automation Progress:** Saved locally to allow resuming interrupted removal sessions.
- **Selector Cache:** Downloaded TikTok DOM selectors cached locally for faster automation.

### What We Do NOT Collect
- ❌ Your TikTok username or password
- ❌ Your video content or metadata
- ❌ Your followers or engagement data
- ❌ Analytics or crash reports
- ❌ Device identifiers (IDFA, Android ID, etc.)
- ❌ Location data
- ❌ Any personally identifiable information (PII)

---

## 3. How We Use Your Data

All data is used **exclusively on your device** to:
1. Maintain your TikTok login session
2. Track removal progress for session resumption
3. Optimize DOM selectors for TikTok's current UI

---

## 4. Third-Party Services

### TikTok
- The App communicates directly with **tiktok.com** via an embedded WebView.
- Your interaction with TikTok is governed by [TikTok's Privacy Policy](https://www.tiktok.com/legal/page/us/privacy-policy/).
- We do not act as an intermediary or proxy.

### GitHub (Optional)
- If enabled, the App may fetch updated DOM selectors from GitHub.
- This is a direct fetch; we do not log or track these requests.

### No Other Third Parties
- We do not use analytics SDKs (Firebase, Mixpanel, etc.).
- We do not use advertising networks.
- We do not use crash reporting services.

---

## 5. Data Storage & Security

| Data | Location | Retention | Security |
|------|----------|-----------|----------|
| TikTok Cookies | Device WebView | Until logout | Encrypted by OS |
| Session Progress | Device Storage | Until completion | Encrypted by OS |
| Selector Cache | Device Storage | 6 hours | Encrypted by OS |

All data is encrypted by your device's operating system. We do not have access to it.

---

## 6. Your Rights

You have full control:
- **Access:** All your data is on your device; you can inspect it via Android's file system.
- **Delete:** Uninstalling the app deletes all data. You can also manually clear app data in Settings.
- **Opt-out:** You can disable remote selector updates by not granting network permissions.

---

## 7. Children's Privacy

The App is not intended for users under 13. We do not knowingly collect data from children.

---

## 8. Changes to This Policy

We may update this policy. Changes will be reflected in the app's release notes.

---

## 9. Contact

For privacy questions, please contact:
- **GitHub Issues:** [abbn7/tiktok-repost-remover-v2](https://github.com/abbn7/tiktok-repost-remover-v2/issues)

---

## 10. Compliance

This app complies with:
- **Google Play Store Privacy Policy Requirements**
- **GDPR** (EU users)
- **CCPA** (California users)
- **Android Privacy & Security Best Practices**

---

**Summary:** We don't collect your data. Period. Everything stays on your device.
