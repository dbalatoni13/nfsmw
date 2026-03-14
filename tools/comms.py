#!/usr/bin/env python3
"""
Agent real-time communication tool.

Usage:
    python tools/comms.py start-broker
    python tools/comms.py broker
    python tools/comms.py start-agent <agent>
    python tools/comms.py stop-agent <agent>
    python tools/comms.py agent <agent>
    python tools/comms.py watch <agent>
    python tools/comms.py send <agent> <message>
    python tools/comms.py send <agent> --to <target> --priority high --ack <message>
    python tools/comms.py reply <agent> <message>
    python tools/comms.py inbox <agent>
    python tools/comms.py feedback <agent> --workflow "..." --feature "..." --annoyance "..."
    python tools/comms.py read [--since <N>] | [--new <agent>]
    python tools/comms.py heartbeat <agent>
    python tools/comms.py status
    python tools/comms.py ack <agent> [--seq <N>]

The broker persists structured events to .comms/events.jsonl and mirrors them to
.comms/chat.log for compatibility. Agents should run one persistent background
listener:

    python tools/comms.py start-agent <agent>

That sidecar receives pushed events immediately, updates heartbeats, and replays
missed events after reconnects while the main agent keeps working.
"""

from __future__ import annotations

import argparse
import json
import os
import select
import selectors
import shlex
import signal
import socket
import subprocess
import sys
import time
import uuid
from contextlib import closing
from dataclasses import dataclass
from datetime import datetime
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parent.parent
COMMS_DIR = REPO_ROOT / ".comms"
CHAT_LOG = COMMS_DIR / "chat.log"
EVENTS_LOG = COMMS_DIR / "events.jsonl"
HEARTBEAT_DIR = COMMS_DIR / "heartbeat"
CURSOR_DIR = COMMS_DIR / "cursors"
STATE_DIR = COMMS_DIR / "state"
BUS_SOCKET = COMMS_DIR / "bus.sock"
BROKER_LOG = COMMS_DIR / "broker.log"
PID_DIR = COMMS_DIR / "pids"
LISTENER_LOG_DIR = COMMS_DIR / "logs"
REPLY_HELPER_DIR = COMMS_DIR / "reply_now"
INBOX_DIR = COMMS_DIR / "inbox"
ATTENTION_DIR = COMMS_DIR / "attention"

ONLINE_WINDOW = 15
IDLE_WINDOW = 120
HEARTBEAT_INTERVAL = 5
BROKER_STARTUP_TIMEOUT = 5.0
ALERT_REMINDER_INTERVAL = 10


def native_notifications_enabled():
    return os.environ.get("COMMS_NATIVE_NOTIFY") == "1"

@dataclass
class BrokerClient:
    sock: socket.socket
    buffer: str = ""
    agent: str | None = None
    mode: str = "command"


def ensure_dirs():
    COMMS_DIR.mkdir(exist_ok=True)
    HEARTBEAT_DIR.mkdir(exist_ok=True)
    CURSOR_DIR.mkdir(exist_ok=True)
    STATE_DIR.mkdir(exist_ok=True)
    PID_DIR.mkdir(exist_ok=True)
    LISTENER_LOG_DIR.mkdir(exist_ok=True)
    REPLY_HELPER_DIR.mkdir(exist_ok=True)
    INBOX_DIR.mkdir(exist_ok=True)
    ATTENTION_DIR.mkdir(exist_ok=True)

    if not CHAT_LOG.exists():
        CHAT_LOG.write_text("")

    if not EVENTS_LOG.exists():
        EVENTS_LOG.write_text("")


def now_iso():
    return datetime.now().isoformat(timespec="seconds")


def now_hms():
    return datetime.now().strftime("%H:%M:%S")


def ts_to_hms(ts):
    if not ts:
        return now_hms()

    if "T" in ts:
        return ts.split("T", 1)[1][:8]

    return ts[-8:]


def heartbeat(agent):
    ensure_dirs()
    hb_file = HEARTBEAT_DIR / f"{agent}.alive"
    hb_file.write_text(str(time.time()))


def state_file(agent):
    return STATE_DIR / f"{agent}.json"


def listener_pid_file(agent):
    return PID_DIR / f"{agent}.pid"


def listener_log_file(agent):
    return LISTENER_LOG_DIR / f"{agent}.log"


def reply_helper_file(agent):
    return REPLY_HELPER_DIR / f"{agent}.sh"


def inbox_file(agent):
    return INBOX_DIR / f"{agent}.txt"


def attention_file(agent):
    return ATTENTION_DIR / f"{agent}.txt"


def read_pid(path):
    if not path.exists():
        return None

    try:
        return int(path.read_text().strip())
    except (OSError, ValueError):
        return None


def process_is_running(pid):
    if pid is None or pid <= 0:
        return False

    try:
        os.kill(pid, 0)
        return True
    except OSError:
        return False


def get_listener_status(agent):
    pid = read_pid(listener_pid_file(agent))
    if not process_is_running(pid):
        return "-"

    return "bg(%d)" % pid


def load_agent_state(agent):
    data = {
        "last_seen_seq": 0,
        "last_acked_seq": 0,
    }
    path = state_file(agent)
    if not path.exists():
        return data

    try:
        loaded = json.loads(path.read_text())
    except (OSError, ValueError, json.JSONDecodeError):
        return data

    for key in data:
        try:
            data[key] = int(loaded.get(key, data[key]))
        except (TypeError, ValueError):
            pass

    return data


def save_agent_state(agent, data):
    ensure_dirs()
    path = state_file(agent)
    path.write_text(json.dumps(data, indent=2, sort_keys=True) + "\n")


def note_seen(agent, seq):
    state = load_agent_state(agent)
    if seq > state["last_seen_seq"]:
        state["last_seen_seq"] = seq
    save_agent_state(agent, state)


def note_acked(agent, seq):
    state = load_agent_state(agent)
    if seq > state["last_acked_seq"]:
        state["last_acked_seq"] = seq
    if seq > state["last_seen_seq"]:
        state["last_seen_seq"] = seq
    save_agent_state(agent, state)


def iter_events():
    ensure_dirs()
    with open(EVENTS_LOG, "r") as handle:
        for line in handle:
            line = line.strip()
            if not line:
                continue

            try:
                yield json.loads(line)
            except json.JSONDecodeError:
                continue


def get_last_seq():
    last_seq = 0
    for event in iter_events():
        try:
            last_seq = max(last_seq, int(event.get("seq", 0)))
        except (TypeError, ValueError):
            continue
    return last_seq


def event_targets_agent(event, agent):
    target = event.get("to", "all")
    if target in (None, "", "*", "all"):
        return True

    if isinstance(target, list):
        return "all" in target or "*" in target or agent in target

    return target == agent


def format_event_line(event):
    prefix = "[%s] %s" % (ts_to_hms(event.get("ts")), event.get("from", "unknown"))
    target = event.get("to", "all")
    if target not in (None, "", "*", "all"):
        prefix += " -> %s" % target

    thread = event.get("thread")
    if thread:
        prefix += " #%s" % thread

    flags = []
    kind = event.get("kind", "message")
    if kind != "message":
        flags.append(kind)

    priority = event.get("priority", "normal")
    if priority != "normal":
        flags.append(priority)

    if event.get("requires_ack"):
        flags.append("ack")

    if flags:
        prefix += " [%s]" % " ".join(flags)

    return "%s: %s" % (prefix, event.get("body", ""))


def append_event(event):
    ensure_dirs()
    with open(EVENTS_LOG, "a") as handle:
        handle.write(json.dumps(event, sort_keys=True) + "\n")

    with open(CHAT_LOG, "a") as handle:
        handle.write(format_event_line(event) + "\n")


def send_json(sock, payload):
    data = (json.dumps(payload, sort_keys=True) + "\n").encode("utf-8")
    sock.sendall(data)


def read_json_line(handle):
    line = handle.readline()
    if not line:
        raise RuntimeError("connection closed")
    return json.loads(line)


def broker_is_ready():
    if not BUS_SOCKET.exists():
        return False

    try:
        with closing(socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)) as sock:
            sock.settimeout(0.2)
            sock.connect(str(BUS_SOCKET))
        return True
    except OSError:
        return False


def connect_broker(autostart=False):
    ensure_dirs()
    if autostart:
        ensure_broker_running()

    sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    sock.connect(str(BUS_SOCKET))
    return sock


def ensure_broker_running():
    ensure_dirs()
    if broker_is_ready():
        return

    if BUS_SOCKET.exists():
        try:
            BUS_SOCKET.unlink()
        except OSError:
            pass

    with open(BROKER_LOG, "a") as log_handle:
        subprocess.Popen(
            [sys.executable, str(Path(__file__).resolve()), "broker"],
            cwd=str(REPO_ROOT),
            stdout=log_handle,
            stderr=log_handle,
            start_new_session=True,
        )

    deadline = time.time() + BROKER_STARTUP_TIMEOUT
    while time.time() < deadline:
        if broker_is_ready():
            return
        time.sleep(0.1)

    raise RuntimeError("timed out starting comms broker")


def broker_request(agent, payload, autostart=False):
    with closing(connect_broker(autostart=autostart)) as sock:
        handle = sock.makefile("r", encoding="utf-8")
        try:
            send_json(sock, {"type": "hello", "agent": agent, "mode": "command"})
            read_json_line(handle)
            send_json(sock, payload)
            return read_json_line(handle)
        finally:
            handle.close()


def legacy_send(agent, message):
    ensure_dirs()
    ts = now_hms()
    line = "[%s] %s: %s\n" % (ts, agent, message)
    with open(CHAT_LOG, "a") as handle:
        handle.write(line)
    heartbeat(agent)
    print("Sent: %s" % line.strip())


def send(
    agent,
    message,
    target="all",
    priority="normal",
    kind="message",
    requires_ack=False,
    thread=None,
    correlation_id=None,
):
    ensure_dirs()

    try:
        reply = broker_request(
            agent,
            {
                "type": "publish",
                "from": agent,
                "to": target,
                "kind": kind,
                "priority": priority,
                "requires_ack": requires_ack,
                "body": message,
                "thread": thread,
                "correlation_id": correlation_id,
            },
            autostart=True,
        )
        event = reply["event"]
        print("Sent: %s" % format_event_line(event))
        return
    except Exception:
        legacy_send(agent, message)


def send_direct(
    agent,
    target,
    message,
    priority="normal",
    kind="message",
    requires_ack=False,
    thread=None,
    correlation_id=None,
):
    send(
        agent,
        message,
        target=target,
        priority=priority,
        kind=kind,
        requires_ack=requires_ack,
        thread=thread,
        correlation_id=correlation_id,
    )


def send_feedback(
    agent,
    workflow=None,
    features=None,
    annoyance=None,
    note=None,
    target="commsManager",
    thread="comms-feedback",
    priority="normal",
):
    parts = []
    if workflow:
        parts.append("workflow=%s" % workflow)
    if features:
        parts.append("features=%s" % ", ".join(features))
    if annoyance:
        parts.append("annoyance=%s" % annoyance)
    if note:
        parts.append("note=%s" % note)

    if not parts:
        raise ValueError("feedback requires at least one of --workflow, --feature, --annoyance, or --note")

    send(
        agent,
        "; ".join(parts),
        target=target,
        priority=priority,
        kind="feedback",
        thread=thread,
    )


def find_latest_reply_target(agent):
    events = list(iter_events())
    for event in reversed(events):
        if event.get("from") == agent:
            continue
        if event.get("kind") == "receipt":
            continue
        if event.get("to") == agent or event_mentions_agent(agent, event):
            return {
                "to": event.get("from", "commsManager"),
                "thread": event.get("thread"),
                "correlation_id": event.get("id"),
                "seq": int(event.get("seq", 0)),
            }
    return {
        "to": "commsManager",
        "thread": None,
        "correlation_id": None,
        "seq": None,
    }


def send_reply(agent, message, target=None, thread=None):
    reply_target = find_latest_reply_target(agent)
    send(
        agent,
        message,
        target=target or reply_target["to"],
        thread=thread if thread is not None else reply_target["thread"],
        correlation_id=reply_target["correlation_id"],
    )
    if reply_target["seq"]:
        try:
            broker_request(agent, {"type": "ack", "agent": agent, "seq": reply_target["seq"]}, autostart=False)
        except Exception:
            pass
        note_acked(agent, reply_target["seq"])
    write_agent_inbox(agent)


def actionable_event_for_agent(agent, event):
    if event.get("from") == agent:
        return False
    if event.get("kind") == "receipt":
        return False
    return (
        event.get("to") == agent
        or event_mentions_agent(agent, event)
        or event.get("kind") == "survey"
        or event.get("thread") == "comms-feedback"
    )


def collect_inbox_events(agent):
    acked_seq = load_agent_state(agent)["last_acked_seq"]
    pending = []
    for event in iter_events():
        seq = int(event.get("seq", 0))
        if seq <= acked_seq:
            continue
        if actionable_event_for_agent(agent, event):
            pending.append(event)
    return pending


def write_agent_inbox(agent):
    ensure_dirs()
    pending = collect_inbox_events(agent)
    path = inbox_file(agent)
    if not pending:
        path.write_text("No pending actionable messages.\n")
        return path

    lines = ["Pending actionable messages for %s:" % agent, ""]
    helper = reply_helper_file(agent).relative_to(REPO_ROOT)
    for event in pending[-10:]:
        lines.append("seq=%s from=%s" % (event.get("seq", "?"), event.get("from", "unknown")))
        if event.get("thread"):
            lines.append("thread=%s" % event["thread"])
        lines.append("message=%s" % event.get("body", ""))
        lines.append("reply=%s \"your real reply here\"" % helper)
        lines.append("")

    path.write_text("\n".join(lines).rstrip() + "\n")
    return path


def write_attention_file(agent, event):
    """Overwrite the attention file with the latest unread direct message."""
    ensure_dirs()
    path = attention_file(agent)
    from_agent = event.get("from", "?")
    body = event.get("body", "")
    seq = event.get("seq", "?")
    thread = event.get("thread", "")
    thread_part = (" [#%s]" % thread) if thread else ""
    path.write_text(
        "NEW MESSAGE seq=%s from=%s%s:\n%s\n\nReply: python tools/comms.py reply %s \"your reply\"\n"
        % (seq, from_agent, thread_part, body, agent)
    )


def check_inbox(agent):
    """Print the latest unread direct message in compact form. Returns count."""
    pending = collect_inbox_events(agent)
    if not pending:
        print("(no messages for %s)" % agent)
        return 0

    latest = pending[-1]
    from_agent = latest.get("from", "?")
    body = latest.get("body", "")
    thread = latest.get("thread", "")
    thread_part = (" [#%s]" % thread) if thread else ""
    print("NEW from %s%s: %s" % (from_agent, thread_part, body))
    print("Reply: python tools/comms.py reply %s \"your reply here\"" % agent)
    if len(pending) > 1:
        print("(%d more unread — run: python tools/comms.py inbox %s)" % (len(pending) - 1, agent))
    return len(pending)


def show_inbox(agent):
    path = write_agent_inbox(agent)
    print(path.read_text().rstrip())


def interactive_chat(agent):
    """Interactive chat session: shows pending messages then reads replies from stdin.

    Designed for async bash mode — an agent runs this, sees what's waiting, and
    types a reply. Single empty line or EOF exits.
    """
    print("=== comms chat: %s ===" % agent)
    print("Pending messages shown below. Type reply + Enter. Empty line or Ctrl-C to exit.")
    print("")

    pending = collect_inbox_events(agent)
    if pending:
        print("--- %d pending message(s) ---" % len(pending))
        for event in pending:
            print(format_event_line(event))
        print("")
    else:
        print("(no pending messages for %s)" % agent)
        print("")

    try:
        while True:
            try:
                line = input("[%s]> " % agent)
            except EOFError:
                break
            if not line.strip():
                break
            send_reply(agent, line.strip())
            print("Sent.")
    except KeyboardInterrupt:
        print("\nExiting chat.")


def read_log(n=20):
    events = list(iter_events())
    if events:
        for event in events[-n:]:
            print(format_event_line(event))
        return

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
    events = list(iter_events())
    if events:
        state = load_agent_state(agent)
        new_events = [
            event
            for event in events
            if int(event.get("seq", 0)) > state["last_seen_seq"] and event_targets_agent(event, agent)
        ]
        if not new_events:
            print("(no new messages)")
        else:
            for event in new_events:
                print(format_event_line(event))

            last_seq = int(new_events[-1].get("seq", state["last_seen_seq"]))
            note_acked(agent, last_seq)

        heartbeat(agent)
        return

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

    cursor_file.write_text(str(len(lines)))
    heartbeat(agent)


def count_pending(agent):
    return len(collect_inbox_events(agent))


def status():
    ensure_dirs()
    now = time.time()
    agents = set()

    for hb_file in HEARTBEAT_DIR.glob("*.alive"):
        agents.add(hb_file.stem)

    for state_path in STATE_DIR.glob("*.json"):
        agents.add(state_path.stem)

    broker_state = "ONLINE" if broker_is_ready() else "offline"
    print("Comms Status:")
    print("-" * 52)
    print("  broker           %s  socket=%s  last_seq=%d" % (broker_state, BUS_SOCKET.name, get_last_seq()))

    visible_agents = sorted(agent for agent in agents if agent != "broker")
    if not visible_agents:
        print("  (no agents registered)")
        return

    for agent in visible_agents:
        hb_file = HEARTBEAT_DIR / ("%s.alive" % agent)
        age_text = "unknown"
        last_seen = 0.0
        if hb_file.exists():
            try:
                last_seen = float(hb_file.read_text().strip())
            except (ValueError, OSError):
                last_seen = 0.0

        if last_seen > 0:
            age = now - last_seen
            if age < ONLINE_WINDOW:
                age_text = "ONLINE"
            elif age < IDLE_WINDOW:
                age_text = "idle (%ds ago)" % int(age)
            else:
                age_text = "offline (%dm ago)" % int(age / 60)

        state = load_agent_state(agent)
        pending = count_pending(agent)
        print(
            "  %-15s %-16s listener=%-10s pending=%-3d seen=%-4d ack=%-4d"
            % (
                agent,
                age_text,
                get_listener_status(agent),
                pending,
                state["last_seen_seq"],
                state["last_acked_seq"],
            )
        )


def ack(agent, seq=None):
    ensure_dirs()
    ack_seq = get_last_seq() if seq is None else seq
    note_acked(agent, ack_seq)
    heartbeat(agent)

    try:
        broker_request(agent, {"type": "ack", "agent": agent, "seq": ack_seq}, autostart=False)
    except Exception:
        pass

    write_agent_inbox(agent)
    print("%s: acknowledged through seq %d" % (agent, ack_seq))


def event_mentions_agent(agent, event):
    body = event.get("body", "")
    return ("@%s" % agent) in body


def should_prepare_reply_helper(agent, event):
    if event.get("from") == agent:
        return False

    if event.get("kind") == "receipt":
        return False

    return (
        event.get("to") == agent
        or event_mentions_agent(agent, event)
        or event.get("kind") == "survey"
        or event.get("thread") == "comms-feedback"
    )


def write_reply_helper(agent, event):
    ensure_dirs()
    helper_path = reply_helper_file(agent)
    parts = [
        sys.executable,
        "tools/comms.py",
        "reply",
        agent,
        "--to",
        event.get("from", "commsManager"),
    ]

    if event.get("thread"):
        parts.extend(["--thread", event["thread"]])

    quoted = " ".join(shlex.quote(part) for part in parts)
    lines = [
        "#!/bin/sh",
        'if [ "$#" -eq 0 ]; then',
        '  echo "Usage: %s \\"your real reply here\\""'
        % helper_path.relative_to(REPO_ROOT),
        "  exit 1",
        "fi",
        "cd %s || exit 1" % shlex.quote(str(REPO_ROOT)),
        "%s \"$@\"" % quoted,
        "",
    ]
    helper_path.write_text("\n".join(lines))
    os.chmod(helper_path, 0o755)
    return helper_path


def should_alert(agent, event):
    target = event.get("to", "all")
    priority = event.get("priority", "normal")
    return target == agent or priority in ("high", "critical") or event_mentions_agent(agent, event)


def send_native_notification(agent, event, reminder=False, helper_path=None):
    if sys.platform != "darwin" or not native_notifications_enabled():
        return

    body = event.get("body", "")
    subtitle = "From %s" % event.get("from", "unknown")
    helper_hint = None
    if helper_path is not None:
        helper_hint = "Reply helper: %s" % helper_path.relative_to(REPO_ROOT)

    if reminder:
        body = "Reminder: %s" % body
        subtitle = "Awaiting ack from %s" % event.get("from", "unknown")
        if helper_hint:
            subtitle = "%s | %s" % (subtitle, helper_hint)
    elif helper_hint:
        subtitle = "%s | %s" % (subtitle, helper_hint)

    if event.get("priority") == "critical" and helper_hint and not reminder:
        script = (
            "display dialog %s with title %s buttons {\"OK\"} default button \"OK\" "
            "giving up after 8"
        ) % (
            json.dumps("%s\n\n%s" % (body, helper_hint)),
            json.dumps("Agent comms: %s" % agent),
        )
    else:
        script = "display notification %s with title %s subtitle %s sound name %s" % (
            json.dumps(body),
            json.dumps("Agent comms: %s" % agent),
            json.dumps(subtitle),
            json.dumps("Glass"),
        )

    try:
        subprocess.Popen(
            ["osascript", "-e", script],
            stdout=subprocess.DEVNULL,
            stderr=subprocess.DEVNULL,
        )
    except FileNotFoundError:
        print("osascript not available for local notifications", file=sys.stderr, flush=True)


def notify_event(agent, event):
    print(format_event_line(event), flush=True)
    note_seen(agent, int(event.get("seq", 0)))
    heartbeat(agent)
    helper_path = None

    if should_prepare_reply_helper(agent, event):
        helper_path = write_reply_helper(agent, event)
        write_agent_inbox(agent)
        write_attention_file(agent, event)
        print(
            "Quick manual reply: %s \"your real reply here\""
            % helper_path.relative_to(REPO_ROOT),
            flush=True,
        )
        print("Or: python tools/comms.py reply %s \"your reply\"" % agent, flush=True)

    if should_alert(agent, event):
        if sys.stdout.isatty():
            sys.stdout.write("\a")
            sys.stdout.flush()
        send_native_notification(agent, event, helper_path=helper_path)


def should_auto_receipt(agent, event):
    if event.get("from") == agent:
        return False

    if event.get("kind") == "receipt":
        return False

    target = event.get("to", "all")
    priority = event.get("priority", "normal")
    return target == agent or event.get("requires_ack") or priority in ("high", "critical")


def send_auto_receipt(sock, agent, event):
    send_json(
        sock,
        {
            "type": "publish",
            "from": agent,
            "to": event.get("from", "all"),
            "kind": "receipt",
            "priority": "normal",
            "thread": event.get("thread"),
            "correlation_id": event.get("id"),
            "body": "listener for %s received seq=%s" % (agent, event.get("seq", "?")),
        },
    )


def prune_pending_alerts(agent, pending_alerts):
    acked_seq = load_agent_state(agent)["last_acked_seq"]
    for seq in list(pending_alerts):
        if seq <= acked_seq:
            pending_alerts.pop(seq, None)


def remind_pending_alerts(agent, pending_alerts):
    prune_pending_alerts(agent, pending_alerts)
    now = time.time()

    for seq in sorted(pending_alerts):
        entry = pending_alerts[seq]
        if now - entry["last_alert"] < ALERT_REMINDER_INTERVAL:
            continue

        event = entry["event"]
        print("Reminder pending ack: %s" % format_event_line(event), flush=True)
        helper_path = reply_helper_file(agent)
        if helper_path.exists():
            print(
                "Quick manual reply: %s \"your real reply here\""
                % helper_path.relative_to(REPO_ROOT),
                flush=True,
            )
        if sys.stdout.isatty():
            sys.stdout.write("\a")
            sys.stdout.flush()
        send_native_notification(
            agent,
            event,
            reminder=True,
            helper_path=helper_path if helper_path.exists() else None,
        )
        entry["last_alert"] = now


def agent_loop(agent):
    ensure_broker_running()
    print("Starting realtime listener as %s (Ctrl+C to stop)..." % agent)
    pending_alerts = {}
    sent_receipts = set()

    while True:
        state = load_agent_state(agent)
        try:
            with closing(connect_broker(autostart=False)) as sock:
                sock.setblocking(False)
                send_json(
                    sock,
                    {
                        "type": "hello",
                        "agent": agent,
                        "mode": "listen",
                        "since_seq": state["last_seen_seq"],
                    },
                )

                buffer = ""
                last_heartbeat = 0.0
                while True:
                    now = time.time()
                    if now - last_heartbeat >= HEARTBEAT_INTERVAL:
                        send_json(sock, {"type": "heartbeat", "agent": agent})
                        heartbeat(agent)
                        last_heartbeat = now

                    remind_pending_alerts(agent, pending_alerts)

                    ready, _, _ = select.select([sock], [], [], 1.0)
                    if not ready:
                        continue

                    chunk = sock.recv(4096)
                    if not chunk:
                        raise RuntimeError("broker disconnected")

                    buffer += chunk.decode("utf-8")
                    while "\n" in buffer:
                        line, buffer = buffer.split("\n", 1)
                        if not line.strip():
                            continue

                        msg = json.loads(line)
                        msg_type = msg.get("type")
                        if msg_type == "hello":
                            print("Connected to broker at %s" % BUS_SOCKET, flush=True)
                        elif msg_type == "ready":
                            print(
                                "Listener ready (%d replayed, last_seq=%d)"
                                % (int(msg.get("replayed", 0)), int(msg.get("last_seq", 0))),
                                flush=True,
                            )
                        elif msg_type == "event":
                            event = msg["event"]
                            notify_event(agent, event)
                            event_id = event.get("id")
                            if event_id and event_id not in sent_receipts and should_auto_receipt(agent, event):
                                send_auto_receipt(sock, agent, event)
                                sent_receipts.add(event_id)
                            if not event.get("requires_ack"):
                                seq = int(event.get("seq", 0))
                                send_json(sock, {"type": "ack", "agent": agent, "seq": seq})
                                note_acked(agent, seq)
                            else:
                                seq = int(event.get("seq", 0))
                                pending_alerts[seq] = {
                                    "event": event,
                                    "last_alert": time.time(),
                                }
                        elif msg_type == "ack":
                            seq = int(msg.get("seq", 0))
                            note_acked(agent, seq)
                            prune_pending_alerts(agent, pending_alerts)
                        elif msg_type == "published":
                            pass
        except KeyboardInterrupt:
            print("\nStopped realtime listener.")
            return
        except Exception as exc:
            print("Listener disconnected (%s); reconnecting..." % exc, flush=True)
            time.sleep(1.0)


def watch(agent):
    agent_loop(agent)


def start_broker():
    ensure_broker_running()
    print("Broker is running at %s" % BUS_SOCKET)


def start_agent_listener(agent):
    ensure_dirs()
    ensure_broker_running()

    pid_path = listener_pid_file(agent)
    current_pid = read_pid(pid_path)
    if process_is_running(current_pid):
        print("%s listener already running as pid %d" % (agent, current_pid))
        return

    if pid_path.exists():
        try:
            pid_path.unlink()
        except OSError:
            pass

    log_path = listener_log_file(agent)
    with open(log_path, "a") as log_handle:
        proc = subprocess.Popen(
            [sys.executable, str(Path(__file__).resolve()), "agent", agent],
            cwd=str(REPO_ROOT),
            stdout=log_handle,
            stderr=log_handle,
            start_new_session=True,
        )

    pid_path.write_text(str(proc.pid) + "\n")
    print("%s listener started in background (pid=%d, log=%s)" % (agent, proc.pid, log_path))


def stop_agent_listener(agent):
    ensure_dirs()
    pid_path = listener_pid_file(agent)
    pid = read_pid(pid_path)
    if not process_is_running(pid):
        if pid_path.exists():
            try:
                pid_path.unlink()
            except OSError:
                pass
        print("%s listener is not running" % agent)
        return

    os.kill(pid, signal.SIGTERM)

    deadline = time.time() + 3.0
    while time.time() < deadline:
        if not process_is_running(pid):
            break
        time.sleep(0.1)

    if pid_path.exists():
        try:
            pid_path.unlink()
        except OSError:
            pass

    print("%s listener stopped (pid=%d)" % (agent, pid))


def make_event(from_agent, payload, next_seq):
    event = {
        "seq": next_seq,
        "id": uuid.uuid4().hex,
        "ts": now_iso(),
        "from": from_agent,
        "to": payload.get("to", "all"),
        "kind": payload.get("kind", "message"),
        "priority": payload.get("priority", "normal"),
        "requires_ack": bool(payload.get("requires_ack", False)),
        "body": payload.get("body", ""),
    }

    for key in ("thread", "correlation_id"):
        if payload.get(key):
            event[key] = payload[key]

    return event


def broker_accept(selector, server, clients):
    conn, _ = server.accept()
    conn.setblocking(False)
    client = BrokerClient(sock=conn)
    clients[conn.fileno()] = client
    selector.register(conn, selectors.EVENT_READ, client)


def broker_close_client(selector, clients, client):
    fileno = client.sock.fileno()
    try:
        selector.unregister(client.sock)
    except Exception:
        pass

    try:
        client.sock.close()
    except OSError:
        pass

    clients.pop(fileno, None)


def broker_replay(client, since_seq):
    replayed = 0
    for event in iter_events():
        seq = int(event.get("seq", 0))
        if seq <= since_seq:
            continue
        if event_targets_agent(event, client.agent):
            send_json(client.sock, {"type": "event", "event": event})
            replayed += 1
    return replayed


def broker_publish(clients, next_seq, payload, client_agent):
    sender = payload.get("from") or client_agent or "unknown"
    event = make_event(sender, payload, next_seq)
    append_event(event)
    heartbeat(sender)

    for other_client in list(clients.values()):
        if other_client.mode == "listen" and other_client.agent and event_targets_agent(event, other_client.agent):
            try:
                send_json(other_client.sock, {"type": "event", "event": event})
            except OSError:
                pass

    return event


def broker_handle_message(selector, clients, client, payload, next_seq_ref):
    payload_type = payload.get("type")

    if payload_type == "hello":
        client.agent = payload.get("agent", "unknown")
        client.mode = payload.get("mode", "command")
        heartbeat(client.agent)
        send_json(client.sock, {"type": "hello", "agent": client.agent, "last_seq": next_seq_ref[0] - 1})

        if client.mode == "listen":
            since_seq = int(payload.get("since_seq", 0))
            replayed = broker_replay(client, since_seq)
            send_json(client.sock, {"type": "ready", "replayed": replayed, "last_seq": next_seq_ref[0] - 1})

        return

    if payload_type == "publish":
        event = broker_publish(clients, next_seq_ref[0], payload, client.agent)
        next_seq_ref[0] += 1
        send_json(client.sock, {"type": "published", "event": event})
        return

    if payload_type == "heartbeat":
        agent = payload.get("agent") or client.agent or "unknown"
        heartbeat(agent)
        send_json(client.sock, {"type": "heartbeat", "agent": agent})
        return

    if payload_type == "ack":
        agent = payload.get("agent") or client.agent or "unknown"
        seq = int(payload.get("seq", next_seq_ref[0] - 1))
        note_acked(agent, seq)
        heartbeat(agent)
        send_json(client.sock, {"type": "ack", "agent": agent, "seq": seq})
        return

    send_json(client.sock, {"type": "error", "message": "unknown payload type"})


def run_broker():
    ensure_dirs()

    if BUS_SOCKET.exists():
        if broker_is_ready():
            print("Broker already running at %s" % BUS_SOCKET)
            return

        try:
            BUS_SOCKET.unlink()
        except OSError:
            pass

    server = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
    selector = selectors.DefaultSelector()
    clients = {}
    next_seq_ref = [get_last_seq() + 1]

    try:
        server.bind(str(BUS_SOCKET))
        os.chmod(BUS_SOCKET, 0o600)
        server.listen()
        server.setblocking(False)
        selector.register(server, selectors.EVENT_READ, "server")
        heartbeat("broker")
        print("Broker listening on %s" % BUS_SOCKET)

        while True:
            heartbeat("broker")
            events = selector.select(timeout=1.0)
            for key, _ in events:
                if key.data == "server":
                    broker_accept(selector, server, clients)
                    continue

                client = key.data
                try:
                    chunk = client.sock.recv(4096)
                except OSError:
                    broker_close_client(selector, clients, client)
                    continue

                if not chunk:
                    broker_close_client(selector, clients, client)
                    continue

                client.buffer += chunk.decode("utf-8")
                while "\n" in client.buffer:
                    line, client.buffer = client.buffer.split("\n", 1)
                    line = line.strip()
                    if not line:
                        continue

                    try:
                        payload = json.loads(line)
                    except json.JSONDecodeError:
                        send_json(client.sock, {"type": "error", "message": "invalid json"})
                        continue

                    try:
                        broker_handle_message(selector, clients, client, payload, next_seq_ref)
                    except OSError:
                        broker_close_client(selector, clients, client)
                        break
    except KeyboardInterrupt:
        print("\nBroker stopped.")
    finally:
        try:
            selector.close()
        except Exception:
            pass

        for client in list(clients.values()):
            broker_close_client(selector, clients, client)

        try:
            server.close()
        except OSError:
            pass

        if BUS_SOCKET.exists():
            try:
                BUS_SOCKET.unlink()
            except OSError:
                pass


def build_parser():
    parser = argparse.ArgumentParser(description="Agent realtime communication tool.")
    subparsers = parser.add_subparsers(dest="command")

    subparsers.add_parser("start-broker")
    subparsers.add_parser("broker")

    start_agent_parser = subparsers.add_parser("start-agent")
    start_agent_parser.add_argument("agent")

    stop_agent_parser = subparsers.add_parser("stop-agent")
    stop_agent_parser.add_argument("agent")

    agent_parser = subparsers.add_parser("agent")
    agent_parser.add_argument("agent")

    watch_parser = subparsers.add_parser("watch")
    watch_parser.add_argument("agent")

    send_parser = subparsers.add_parser(
        "send",
        help="Send a broadcast by default. Use --to or prefer dm for direct messages.",
    )
    send_parser.add_argument("agent")
    send_parser.add_argument("--to", default="all")
    send_parser.add_argument("--priority", choices=("normal", "high", "critical"), default="normal")
    send_parser.add_argument("--kind", default="message")
    send_parser.add_argument("--ack", action="store_true")
    send_parser.add_argument("--thread")
    send_parser.add_argument("--correlation-id")
    send_parser.add_argument("message", nargs="+")

    dm_parser = subparsers.add_parser("dm", help="Send a direct message without remembering --to")
    dm_parser.add_argument("agent")
    dm_parser.add_argument("target")
    dm_parser.add_argument("--priority", choices=("normal", "high", "critical"), default="normal")
    dm_parser.add_argument("--kind", default="message")
    dm_parser.add_argument("--ack", action="store_true")
    dm_parser.add_argument("--thread")
    dm_parser.add_argument("--correlation-id")
    dm_parser.add_argument("message", nargs="+")

    reply_parser = subparsers.add_parser("reply")
    reply_parser.add_argument("agent")
    reply_parser.add_argument("--to")
    reply_parser.add_argument("--thread")
    reply_parser.add_argument("message", nargs="+")

    feedback_parser = subparsers.add_parser("feedback")
    feedback_parser.add_argument("agent")
    feedback_parser.add_argument("--to", default="commsManager")
    feedback_parser.add_argument("--thread", default="comms-feedback")
    feedback_parser.add_argument("--priority", choices=("normal", "high", "critical"), default="normal")
    feedback_parser.add_argument("--workflow")
    feedback_parser.add_argument("--feature", action="append", default=[])
    feedback_parser.add_argument("--annoyance")
    feedback_parser.add_argument("--note")

    inbox_parser = subparsers.add_parser("inbox", help="Show all pending actionable messages")
    inbox_parser.add_argument("agent")

    check_parser = subparsers.add_parser(
        "check",
        help="Quick 1-2 line latest message + reply command. Use at start of each work turn.",
    )
    check_parser.add_argument("agent")

    chat_parser = subparsers.add_parser(
        "chat",
        help="Interactive chat: shows pending messages then reads replies from stdin (async bash mode).",
    )
    chat_parser.add_argument("agent")

    read_parser = subparsers.add_parser("read")
    read_group = read_parser.add_mutually_exclusive_group()
    read_group.add_argument("--since", type=int, default=20)
    read_group.add_argument("--new", metavar="AGENT")

    heartbeat_parser = subparsers.add_parser("heartbeat")
    heartbeat_parser.add_argument("agent")

    subparsers.add_parser("status")

    ack_parser = subparsers.add_parser("ack")
    ack_parser.add_argument("agent")
    ack_parser.add_argument("--seq", type=int)

    return parser


def main():
    parser = build_parser()
    args = parser.parse_args()

    if args.command == "start-broker":
        start_broker()
    elif args.command == "broker":
        run_broker()
    elif args.command == "start-agent":
        start_agent_listener(args.agent)
    elif args.command == "stop-agent":
        stop_agent_listener(args.agent)
    elif args.command == "agent":
        agent_loop(args.agent)
    elif args.command == "watch":
        watch(args.agent)
    elif args.command == "send":
        send(
            args.agent,
            " ".join(args.message),
            target=args.to,
            priority=args.priority,
            kind=args.kind,
            requires_ack=args.ack,
            thread=args.thread,
            correlation_id=args.correlation_id,
        )
    elif args.command == "dm":
        send_direct(
            args.agent,
            args.target,
            " ".join(args.message),
            priority=args.priority,
            kind=args.kind,
            requires_ack=args.ack,
            thread=args.thread,
            correlation_id=args.correlation_id,
        )
    elif args.command == "feedback":
        try:
            send_feedback(
                args.agent,
                workflow=args.workflow,
                features=args.feature,
                annoyance=args.annoyance,
                note=args.note,
                target=args.to,
                thread=args.thread,
                priority=args.priority,
            )
        except ValueError as exc:
            parser.error(str(exc))
    elif args.command == "reply":
        send_reply(
            args.agent,
            " ".join(args.message),
            target=args.to,
            thread=args.thread,
        )
    elif args.command == "inbox":
        show_inbox(args.agent)
    elif args.command == "check":
        count = check_inbox(args.agent)
        sys.exit(0 if count == 0 else 1)
    elif args.command == "chat":
        interactive_chat(args.agent)
    elif args.command == "read":
        if args.new:
            read_new(args.new)
        else:
            read_log(args.since)
    elif args.command == "heartbeat":
        heartbeat(args.agent)
        print("%s: heartbeat updated" % args.agent)
    elif args.command == "status":
        status()
    elif args.command == "ack":
        ack(args.agent, args.seq)
    else:
        parser.print_help()
        sys.exit(1)


if __name__ == "__main__":
    main()
