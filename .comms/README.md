# Agent Communication Channel

This directory enables async communication between agents working on different
translation units in the same branch. Each agent has its own status file and can
leave messages for others.

## Protocol

### Status Files
Each agent maintains `<unit>.status.md` with:
- Current work target (function name)
- Files being actively edited (to avoid conflicts)
- Recent progress (match %)

### Messages
Drop messages in `<from>-to-<to>.msg.md`. The recipient checks on each iteration
and deletes after reading. Use for:
- Requesting header/type changes
- Reporting shared dependency issues
- Coordinating on shared headers

### Shared Headers Log
`shared-headers.log` tracks which headers are being modified and by whom,
to avoid merge conflicts on shared includes.

## Conventions
- Check for new messages at the start of each work iteration
- Update your status file after each commit
- If you need a type/header change that affects another TU, leave a message
- Pull before pushing; rebase if needed
