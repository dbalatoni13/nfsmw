# Agent Communication Protocol

Three agents share this branch (`zFE2`). All coordination goes through `.comms/`.

## Quick Start

```bash
python tools/comms.py start-broker          # start the local event broker once
python tools/comms.py start-agent <agent>   # start your nonblocking listener sidecar
python tools/comms.py send <agent> "msg"    # broadcast message through the broker
python tools/comms.py dm <agent> zFe2 --priority high --ack "msg"
python tools/comms.py send <agent> --thread shared-headers "claiming FEManager.hpp"
python tools/comms.py feedback <agent> --workflow "..." --feature "..." --annoyance "..."
python tools/comms.py inbox <agent>         # actionable items that still need attention
python tools/comms.py status                # broker, listeners, actionable inbox backlog
python tools/comms.py ack <agent>           # clear pending ack-required events
python tools/comms.py stop-agent <agent>    # stop your detached listener sidecar
```

`start-agent` is the normal workflow now. It runs the listener as a detached background
task, writes logs to `.comms/logs/<agent>.log`, and keeps receiving events while the main
agent continues compiling, diffing, or editing in the foreground.

For debugging, `python tools/comms.py agent <agent>` still runs the listener in the
foreground.

High-priority or direct events trigger an alert path from the listener. In a foreground
TTY that means a terminal bell. Native macOS notifications are now **off by default**
to avoid flooding the user OS; enable them only for focused testing with
`COMMS_NATIVE_NOTIFY=1`.
Messages that contain `@<agent-name>` also count as alerts, and ack-required urgent
messages now re-notify until they are acknowledged.
Direct or urgent messages also trigger an automatic `receipt` event from the target
listener, so the sender can see broker-to-sidecar delivery immediately in chat.

Use `--thread <name>` for ongoing topics like `shared-headers`, `build-break`, or
`comms-feedback` so related messages stay visually grouped in the log.

Use `feedback` when you want a quick human reply with less typing. It sends a
structured message into `#comms-feedback` for `commsManager` by default:

```bash
python tools/comms.py feedback zFe2 \
  --workflow "background while iterating on HUD" \
  --feature "direct mentions" \
  --feature "build-break alerts" \
  --annoyance "broadcast noise"
```

Use `dm` for person-to-person pings. It avoids the easy-to-misremember
`send --to <agent>` syntax and makes direct routing explicit.

## Agreed Rules (confirmed by all 3 agents)

1. **Only git-add files you own** — never `git add .`
2. **Update `.comms/<unit>.status.md`** after each commit
3. **Run `python tools/comms.py start-agent <name>` once** so you no longer have to remember to poll
4. **Post match % in chat** after each commit
5. **Announce shared header changes in chat** before editing

## File Ownership

| Agent      | Owned paths |
|------------|-------------|
| zFEng      | `src/Speed/Indep/Src/FEng/*`, `src/Speed/Indep/SourceLists/zFEng.cpp` |
| zFe2       | `src/Speed/Indep/Src/Frontend/HUD/*`, `src/Speed/Indep/Src/FEng/FEMath.h`, `src/Speed/Indep/SourceLists/zFe2.cpp` |
| zFeOverlay | `src/Speed/Indep/Src/Frontend/MenuScreens/Safehouse/*`, `src/Speed/Indep/Src/Frontend/FECarLoader.hpp`, `src/Speed/Indep/SourceLists/zFeOverlay.cpp` |

**Shared headers** (e.g. `FEManager.hpp`, `FEMenuScreen.hpp`): announce in chat before editing.

## Communication Files

| File | Purpose |
|------|---------|
| `events.jsonl` | Durable structured event stream used by the broker |
| `chat.log` | Human-readable mirror of events for compatibility / quick inspection |
| `<unit>.status.md` | Agent state, match %, owned files |
| `heartbeat/<agent>.alive` | Timestamp proving agent is active |
| `logs/<agent>.log` | Output from the detached listener sidecar |
| `pids/<agent>.pid` | PID file for a detached listener sidecar |
| `shared-headers.log` | Log of shared header modifications |

## DWARF Budget Warning

All TUs hit the wibo/NgcAs ~5.83MB assembly pipe limit. Rules:
- No struct with inline bodies in headers included by multiple TUs
- Forward-declare types in shared headers, define in `.cpp` or `_impl.h`
- Before adding types to a shared header, estimate DWARF impact
