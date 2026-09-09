"""
IDA name importer
=================

Run inside IDA 9.x to restore names from a delink-style model JSON.

Headless:
    idat.exe -A -S"ida_import_names.py <model.json>" <database.i64>

Interactive:
    File -> Script file... and pick this script; it will prompt for the JSON file.

The script:
  * reads the top-level "names" array: [{"addr": ..., "name": ...}, ...]
  * compensates for an image-base difference between the JSON and the open IDB
  * changes only names that differ
  * prints every successful rename to the IDA console
"""

import json

import ida_auto
import ida_kernwin
import ida_name
import ida_nalt
import idaapi
import idc

BADADDR = idaapi.BADADDR


def _msg(text):
    ida_kernwin.msg("import-names: " + text + "\n")


def _get_name(ea):
    """IDA 9.x first, with a small compatibility fallback."""
    fn = getattr(ida_name, "get_ea_name", None)
    if callable(fn):
        try:
            return fn(ea) or ""
        except Exception:
            pass
    try:
        return idc.get_name(ea) or ""
    except Exception:
        return ""


def _parse_ea(value):
    if isinstance(value, str):
        return int(value, 0)
    return int(value)


def _load_model(path):
    with open(path, "r", encoding="utf-8") as fh:
        model = json.load(fh)

    if not isinstance(model, dict):
        raise ValueError("top-level JSON value must be an object")

    names = model.get("names")
    if not isinstance(names, list):
        raise ValueError('JSON does not contain a top-level "names" array')

    return model, names


def _image_base_delta(model):
    meta = model.get("meta")
    if not isinstance(meta, dict) or "image_base" not in meta:
        return 0

    try:
        exported_base = _parse_ea(meta["image_base"])
        current_base = int(idaapi.get_imagebase())
        return current_base - exported_base
    except Exception:
        return 0


def import_names(model):
    names = model["names"]
    delta = _image_base_delta(model)

    changed = 0
    unchanged = 0
    failed = 0

    if delta:
        _msg("image-base delta: %+d (0x%X -> 0x%X)" % (
            delta,
            _parse_ea(model["meta"]["image_base"]),
            int(idaapi.get_imagebase()),
        ))

    # Exported names came from one IDA database, so they should already be
    # syntactically valid and unique. SN_CHECK therefore keeps this importer
    # exact: it will fail rather than silently alter the requested spelling.
    flags = ida_name.SN_CHECK | ida_name.SN_NOWARN

    for entry in names:
        if not isinstance(entry, dict):
            failed += 1
            continue

        try:
            source_ea = _parse_ea(entry["addr"])
            wanted = str(entry["name"])
        except (KeyError, TypeError, ValueError):
            failed += 1
            continue

        if not wanted:
            failed += 1
            continue

        ea = source_ea + delta
        current = _get_name(ea)

        if current == wanted:
            unchanged += 1
            continue

        # Refuse to let IDA make the requested name unique by adding a suffix.
        # If the exact name is already owned by another address, report failure.
        try:
            owner = ida_name.get_name_ea(BADADDR, wanted)
        except Exception:
            owner = BADADDR

        if owner not in (BADADDR, ea):
            _msg(
                "FAILED 0x%X: %r -> %r (name already used at 0x%X)"
                % (ea, current, wanted, owner)
            )
            failed += 1
            continue

        ok = False
        try:
            ok = bool(ida_name.set_name(ea, wanted, flags))
        except Exception:
            ok = False

        actual = _get_name(ea)
        if ok and actual == wanted:
            _msg("0x%X: %r -> %r" % (ea, current, wanted))
            changed += 1
        else:
            _msg("FAILED 0x%X: %r -> %r" % (ea, current, wanted))
            failed += 1

    _msg(
        "done: %d changed, %d unchanged, %d failed"
        % (changed, unchanged, failed)
    )
    return changed, unchanged, failed


def main():
    ida_auto.auto_wait()

    argv = list(idc.ARGV) if idc.ARGV else []
    json_path = argv[1] if len(argv) > 1 else None

    if not json_path and ida_kernwin.is_idaq():
        default = (ida_nalt.get_input_file_path() or "names") + ".delink.json"
        json_path = ida_kernwin.ask_file(False, default, "Import names from delink JSON")

    if not json_path:
        _msg("no input JSON supplied; nothing changed")
        return

    try:
        model, _ = _load_model(json_path)
        import_names(model)
    except Exception as exc:
        _msg("ERROR: %s" % exc)
        if not ida_kernwin.is_idaq():
            idaapi.qexit(1)
        return

    # In headless mode, save the database explicitly before exiting.
    if not ida_kernwin.is_idaq():
        try:
            import ida_loader
            ida_loader.save_database(idc.get_idb_path(), 0)
        except Exception as exc:
            _msg("WARNING: could not explicitly save database: %s" % exc)
        idaapi.qexit(0)


if __name__ == "__main__":
    main()
