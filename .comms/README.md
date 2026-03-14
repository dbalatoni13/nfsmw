# Agent Communication Protocol

Three agents share this branch. This directory prevents conflicts and data loss.

## Golden Rule

**Never modify files owned by another agent.** If you need a change in their
file, leave a message. If you accidentally stage/commit their files, revert
immediately.

## File Ownership

Each agent owns specific directories and files. Check status files before
touching anything outside your own area.

| Agent | Owned paths |
|-------|-------------|
| zFEng | `src/Speed/Indep/Src/FEng/*`, `src/Speed/Indep/SourceLists/zFEng.cpp` |
| zFe2 | `src/Speed/Indep/Src/Frontend/*` (except Safehouse/), `src/Speed/Indep/SourceLists/zFe2.cpp` |
| zFeOverlay | `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/*`, `src/Speed/Indep/SourceLists/zFeOverlay.cpp` |

When ownership is ambiguous (shared headers like `FEManager.hpp`, `types.h`),
coordinate via a message before editing.

## Required Workflow

### Before every commit
```
1. git diff --staged --name-only   # verify you only touch YOUR files
2. Check .comms/ for new messages addressed to you
3. Update your .comms/<unit>.status.md
```

### Before modifying a shared header
```
1. Check .comms/shared-headers.log for recent edits
2. Append your intent to shared-headers.log
3. If another agent recently touched it, leave a message first
```

## File Naming

| File | Purpose |
|------|---------|
| `<unit>.status.md` | Your current state, match %, files you own. **Update after each commit.** |
| `<from>-to-<to>.msg.md` | One-shot message. Recipient deletes after reading. |
| `shared-headers.log` | Append-only log of shared header modifications. |

## Status File Template

```markdown
# <Unit> Agent Status

## Current State
- **Match**: X% (N/M functions)
- **Working on**: current target

## Files Being Edited (DO NOT MODIFY)
- path/to/file1
- path/to/file2

## Recent Progress
- milestone notes
```

## Message File Template

```markdown
# Message: <from> -> <to>

## Subject: one-line summary
**Priority**: LOW | MEDIUM | HIGH | BLOCKING

### What I need
Describe the change or coordination request.

### What I'll do if no response
Describe your workaround plan.
```

## Conflict Resolution

1. **Staged but not committed**: `git restore --staged <file>` to unstage
2. **Already committed**: revert the specific file: `git checkout HEAD~1 -- <file>`
3. **Merge conflict**: the file owner resolves it; the other agent rebases

## Shared Header Rules

Shared headers (`FEList.h`, `FEObject.h`, `FETypes.h`, `fengine.h`, etc.) are
owned by the agent whose TU they primarily serve. For FEng headers, that's zFEng.

If you need a type from a shared header:
- **Preferred**: `#include` the existing header as-is
- **If header is too heavy**: forward-declare the type locally
- **Never**: copy the header, simplify it, or empty the original

If you need a new member/method added to a shared header:
- Leave a message in `.comms/<you>-to-<owner>.msg.md`
- The owner agent adds it on their next iteration

