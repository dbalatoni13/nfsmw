---
name: realtime-comms
description: Workflow for broker-backed agent communication, background listeners, and fast structured feedback.
---

# Realtime Agent Comms Workflow

Use this skill when coordinating multiple agents in the same worktree and you need
messages to be noticed without manual polling.

The system is built around `tools/comms.py`, a local Unix-socket broker, durable
event storage in `.comms/events.jsonl`, and one detached listener sidecar per agent.

## Core Workflow

Start the broker once:

```sh
python tools/comms.py start-broker
```

Start one nonblocking listener per agent:

```sh
python tools/comms.py start-agent zFEng
python tools/comms.py start-agent zFe2
python tools/comms.py start-agent zFeOverlay
```

Check whether the broker and sidecars are really up:

```sh
python tools/comms.py status
```

Look for `listener=bg(<pid>)` in the output. That means the agent has a detached
listener sidecar and should receive pushed events while continuing other work.

## Messaging Patterns

### Broadcast an update

```sh
python tools/comms.py send commsManager "Build fix landed. Pull latest before continuing."
```

### Direct an urgent message at one agent

```sh
python tools/comms.py send commsManager --to zFe2 --priority high --ack "Please re-test the HUD build."
```

Use `--ack` when you need the receiver to explicitly confirm they saw it.

### Group related discussion into a thread

```sh
python tools/comms.py send commsManager --thread build-break "Tracking the current header regression here."
python tools/comms.py send zFEng --thread shared-headers "Claiming FEManager.hpp for the next edit."
```

Prefer threads for recurring topics such as:

- `build-break`
- `shared-headers`
- `ownership`
- `comms-feedback`

## Fast Reply Pattern

When you want feedback quickly, do not ask for a freeform essay first. Use a
low-friction structured reply.

The fastest built-in path is:

```sh
python tools/comms.py feedback zFEng \
  --workflow "direct alerts for build breaks" \
  --feature "shared-header locks" \
  --feature "digest summary" \
  --annoyance "too much broadcast noise"
```

This sends a compact reply into the `comms-feedback` thread aimed at `commsManager`
by default.

Use the same command shape for any agent:

```sh
python tools/comms.py feedback zFe2 --workflow "background while coding HUD" --feature "direct mentions" --annoyance "easy to miss ownership changes"
python tools/comms.py feedback zFeOverlay --workflow "interrupt only for blockers" --feature "build-break alerts" --feature "header ownership claims"
```

## How To Get Faster Responses

If agents are slow to answer, these tactics work better than repeating the same
open-ended question:

1. Use direct messages plus `--priority high` for the person who needs to answer.
2. Put the request in a named `--thread` so it is visually grouped.
3. Give a one-line reply template or the `feedback` command, not a blank prompt.
4. Ask for only 1-3 concrete items, not a broad design memo.
5. Reserve `--ack` for “I saw this” confirmation; use `feedback` for actual content.

## Notice Latency Guidance

If the goal is to make an agent *notice* a message as fast as possible, prefer:

- a direct `--to <agent>` message
- `--priority high` or `--priority critical`
- `--ack` when the message is important enough to keep reminding about
- an `@agent` mention in the body if the message is broadcast

The detached listener treats direct, high-priority, critical, and `@agent`-mention
messages as alerts. Ack-required urgent messages are re-notified until acknowledged,
which is the main anti-miss mechanism for blockers.
For direct or urgent messages, the target listener also emits an automatic `receipt`
event back to the sender, which is the fastest proof that the sidecar noticed the event.

## Logs and Recovery

Detached listener logs live under:

```sh
.comms/logs/<agent>.log
```

If an agent disconnects and reconnects, the broker replays missed events from the
last seen sequence automatically.

Stop a listener when needed:

```sh
python tools/comms.py stop-agent zFe2
```

For debugging in the foreground, you can still use:

```sh
python tools/comms.py agent zFe2
```
