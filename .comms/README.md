# Agent Communication Protocol

Three agents share this branch. This directory prevents conflicts and data loss.

## IMPORTANT: Use .comms/ only (not .com/)

All communication goes in `.comms/`. The `.com/` folder was created by mistake.
If you have files there, move them here.

## Golden Rule

**Never modify files owned by another agent.** If you need a change in their
file, leave a message. If you accidentally stage/commit their files, revert
immediately.

## File Ownership

Each agent owns specific directories and files. Check status files before
touching anything outside your own area.

| Agent      | Owned paths |
|------------|-------------|
| zFEng      | `src/Speed/Indep/Src/FEng/*`, `src/Speed/Indep/SourceLists/zFEng.cpp` |
| zFe2       | `src/Speed/Indep/Src/Frontend/HUD/*`, `src/Speed/Indep/SourceLists/zFe2.cpp`, `src/Speed/Indep/Src/FEng/FEMath.h` |
| zFeOverlay | `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/*`, `src/Speed/Indep/Src/Frontend/FECarLoader.hpp`, `src/Speed/Indep/SourceLists/zFeOverlay.cpp` |

When ownership is ambiguous (shared headers like `FEManager.hpp`),
coordinate via a message before editing.

## Required Workflow

### Before every commit
```bash
git diff --staged --name-only   # verify you only touch YOUR files
# Check .comms/ for new messages addressed to you
# Update your .comms/<unit>.status.md
```

### Before modifying a shared header
```bash
# 1. Check .comms/shared-headers.log for recent edits
# 2. Append your intent to shared-headers.log
# 3. If another agent recently touched it, leave a message first
```

### After every commit
```bash
# Update .comms/<unit>.status.md with new match %, current target
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

## The DWARF Budget Problem

All three TUs are jumbo builds that hit the wibo/NgcAs assembly pipe buffer limit
(~5.83MB). Every struct/class definition with inline bodies in a shared header
adds DWARF entries that count against EVERY TU that transitively includes it.

**Rules for shared headers:**
1. Prefer forward declarations over full struct definitions
2. Move inline function bodies to .cpp files when possible
3. If a struct must be in a header, keep it minimal (no inline bodies)
4. Before adding a new struct to a shared header, estimate DWARF impact
5. If only your TU needs a type, put it in YOUR header, not a shared one

## Conflict Resolution

1. **Staged but not committed**: `git restore --staged <file>` to unstage
2. **Already committed**: revert the specific file: `git checkout HEAD~1 -- <file>`
3. **Merge conflict**: the file owner resolves it; the other agent rebases
