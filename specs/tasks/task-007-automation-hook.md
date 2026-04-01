# Task 007: Automation Hook

## Objective
Create the useAutomation hook - the state machine for automation.

## Features Implemented
- State management (idle, loading, running, done, error, rate_limited)
- Message parsing from WebView
- Start/Stop/Reset actions
- Progress tracking

## Checklist
- [x] AutomationState type defined
- [x] Message types defined
- [x] handleMessage parses all message types
- [x] start() injects script
- [x] stop() updates UI state
- [x] reset() clears state
- [x] Progress counter works

## Status: ✅ Completed
