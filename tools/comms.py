#!/usr/bin/env python3
"""
Agent real-time communication tool.

Usage:
    python tools/comms.py send <agent> <message>
    python tools/comms.py read [--since <N>]     # read last N lines (default: 20)
    python tools/comms.py read --new              # only unread messages
    python tools/comms.py heartbeat <agent>       # signal you're alive
    python tools/comms.py status                  # who's online?
    python tools/comms.py ack <agent>             # acknowledge you've read messages
    python tools/comms.py watch <agent>           # block and tail new messages

All agents share .comms/chat.log as a single append-only chat stream.
Each agent calls 'heartbeat' periodically to show they're alive.
"""

import sys
import os
import time
import json
from pathlib import Path
from datetime import datetime

REPO_ROOT = Path(__file__).resolve().parent.parent
COMMS_DIR = REPO_ROOT / ".comms"
CHAT_LOG = COMMS_DIR / "chat.log"
HEARTBEAT_DIR = COMMS_DIR / "heartbeat"
CURSOR_DIR = COMMS_DIR / "cursors"


def ensure_dirs():
    COMMS_DIR.mkdir(exist_ok=True)
    HEARTBEAT_DIR.mkdir(exist_ok=True)
    CURSOR_DIR.mkdir(exist_ok=True)
    if not CHAT_LOG.exists():
        CHAT_LOG.write_text("")


def send(agent, message):
    ensure_dirs()
    ts = datetime.now().strftime("%H:%M:%S")
    line = f"[{ts}] {agent}: {message}\n"
    with open(CHAT_LOG, "a") as f:
        f.write(line)
    # Also update heartbeat
    heartbeat(agent)
    print(f"Sent: {line.strip()}")


def read_log(n=20):
    ensure_dirs()
    if not CHAT_LOG.exists():
        print("(no messages yet)")
        return
    lines = CHAT_LOG.read_text().splitlines()
    if not lines:
        print("(no messages yet)")
        return
    for line in lines[-n:]:
        print(line)


def read_new(agent):
    ensure_dirs()
    cursor_file = CURSOR_DIR / f"{agent}.cursor"
    cursor = 0
    if cursor_file.exists():
        try:
            cursor = int(cursor_file.read_text().strip())
        except ValueError:
            cursor = 0

    if not CHAT_LOG.exists():
        print("(no messages yet)")
        return

    lines = CHAT_LOG.read_text().splitlines()
    new_lines = lines[cursor:]
    if not new_lines:
        print("(no new messages)")
    else:
        for line in new_lines:
            print(line)

    # Update cursor
    cursor_file.write_text(str(len(lines)))
    heartbeat(agent)


def heartbeat(agent):
    ensure_dirs()
    hb_file = HEARTBEAT_DIR / f"{agent}.alive"
    hb_file.write_text(str(time.time()))


def status():
    ensure_dirs()
    now = time.time()
    print("Agent Status:")
    print("-" * 40)
    any_alive = False
    for hb_file in sorted(HEARTBEAT_DIR.glob("*.alive")):
        agent = hb_file.stem
        try:
            last = float(hb_file.read_text().strip())
            age = now - last
            if age < 120:
                state = "ONLINE"
            elif age < 600:
                state = f"idle ({int(age)}s ago)"
            else:
                state = f"offline ({int(age/60)}m ago)"
            print(f"  {agent:15s} {state}")
            any_alive = True
        except (ValueError, OSError):
            print(f"  {agent:15s} unknown")
    if not any_alive:
        print("  (no agents registered)")


def ack(agent):
    ensure_dirs()
    if not CHAT_LOG.exists():
        return
    lines = CHAT_LOG.read_text().splitlines()
    cursor_file = CURSOR_DIR / f"{agent}.cursor"
    cursor_file.write_text(str(len(lines)))
    heartbeat(agent)
    print(f"{agent}: acknowledged all {len(lines)} messages")


def watch(agent):
    """Tail the chat log, blocking until new messages appear."""
    ensure_dirs()
    heartbeat(agent)
    if not CHAT_LOG.exists():
        CHAT_LOG.write_text("")

    lines = CHAT_LOG.read_text().splitlines()
    pos = len(lines)
    print(f"Watching chat as {agent} (Ctrl+C to stop)...")
    print(f"({pos} historical messages, showing new only)")
    try:
        while True:
            time.sleep(2)
            heartbeat(agent)
            current = CHAT_LOG.read_text().splitlines()
            if len(current) > pos:
                for line in current[pos:]:
                    print(line)
                pos = len(current)
    except KeyboardInterrupt:
        print("\nStopped watching.")


def main():
    if len(sys.argv) < 2:
        print(__doc__)
        sys.exit(1)

    cmd = sys.argv[1]

    if cmd == "send":
        if len(sys.argv) < 4:
            print("Usage: python tools/comms.py send <agent> <message>")
            sys.exit(1)
        send(sys.argv[2], " ".join(sys.argv[3:]))

    elif cmd == "read":
        if "--new" in sys.argv:
            if len(sys.argv) < 4:
                print("Usage: python tools/comms.py read --new <agent>")
                sys.exit(1)
            idx = sys.argv.index("--new")
            agent = sys.argv[idx + 1] if idx + 1 < len(sys.argv) else "unknown"
            read_new(agent)
        elif "--since" in sys.argv:
            idx = sys.argv.index("--since")
            n = int(sys.argv[idx + 1]) if idx + 1 < len(sys.argv) else 20
            read_log(n)
        else:
            read_log()

    elif cmd == "heartbeat":
        if len(sys.argv) < 3:
            print("Usage: python tools/comms.py heartbeat <agent>")
            sys.exit(1)
        heartbeat(sys.argv[2])
        print(f"{sys.argv[2]}: heartbeat updated")

    elif cmd == "status":
        status()

    elif cmd == "ack":
        if len(sys.argv) < 3:
            print("Usage: python tools/comms.py ack <agent>")
            sys.exit(1)
        ack(sys.argv[2])

    elif cmd == "watch":
        if len(sys.argv) < 3:
            print("Usage: python tools/comms.py watch <agent>")
            sys.exit(1)
        watch(sys.argv[2])

    else:
        print(f"Unknown command: {cmd}")
        print(__doc__)
        sys.exit(1)


if __name__ == "__main__":
    main()
