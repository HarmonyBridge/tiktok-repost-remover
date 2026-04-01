# Task 010: Testing Checklist

## Objective
Verify all functionality before build.

## Pre-Build Checklist
- [x] All TypeScript errors: `npx tsc --noEmit` (zero errors required)
- [x] `true;` at end of injectedScript
- [x] domStorageEnabled={true} in WebView
- [x] userAgent overridden in WebView
- [x] All selectors have 3+ fallbacks
- [x] No hardcoded colors outside theme/colors.ts
- [x] No any types in TypeScript
- [x] constitution.md, spec.md, plan.md up to date

## Bug Fixes Checklist
- [x] Bug #1 fixed: __TRR_RUNNING__ guard in injectedScript
- [x] Bug #2 fixed: Navigation race condition in useAutomation
- [x] Bug #3 fixed: JSON parse crash in handleMessage

## Reliability Upgrades Checklist
- [x] Hot-reload selectors implemented (remoteSelectors.ts)
- [x] selectors.json hosted on remote
- [x] Exponential backoff implemented in delays
- [x] Session persistence saves count (sessionStorage.ts)
- [x] Pre-scroll loads all reposts before starting
- [x] Login detection enhanced (5 methods)
- [x] DRY_RUN mode available for testing

## Defensive Coding Checklist
- [x] Selector health check implemented
- [x] Memory leak prevention in useEffect cleanup
- [x] WebView crash recovery implemented
- [x] TypeScript strict mode enabled
- [x] All catch blocks show error message to UI
- [x] All async functions have try/catch

## Device Testing (Manual)
- [ ] Login flow works normally
- [ ] Repost tab detected correctly
- [ ] Removal loop works end-to-end
- [ ] Progress counter updates in real-time
- [ ] Rate limit detection works
- [ ] Empty state (no reposts) handled correctly
- [ ] Back button during process shows confirmation
- [ ] App survives screen rotation
- [ ] App resumes from background correctly

## Status: ✅ Code Complete - Ready for Testing
