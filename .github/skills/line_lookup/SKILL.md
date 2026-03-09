---
name: line-lookup
description: Use this skill when decompiling a game and trying to determine which source file a function, inline, or class belongs to. When the user has a function address and wants to know where it's defined, which file a class lives in, or which source file an inline expands from, use this skill to run line_lookup.py against the map file. Trigger whenever the user mentions a hex address, asks "where is this function defined", "which file is this class in", "what file does this inline come from", or is trying to match a decompiled function to its original source location.
---

# Debug Address-Line Lookup for Decompilation

When decompiling a game, functions compile down to a contiguous block of addresses. Inlined functions expand at the call site, meaning the map file records the _original source file_ of the inline — not just the outer function. This makes the map file a powerful tool for locating:

- **Which file a class is defined in** — inlines are typically defined inside class bodies in headers, so an inline at a given address points directly to the class's header file
- **Which file a free function or method originates from**
- **Confirmation that two addresses belong to the same translation unit**

## The Key Insight

If you are decompiling a function at address `0x801AE814` and you see entries like:

```
0x801AE814: d:/mw/speed/gamecube/libs/stl/stlport-4.5/stlport/stl/_tree.h (line 278)
0x801AE814: d:/mw/speed/gamecube/libs/stl/stlport-4.5/stlport/stl/_function_base.h (line 152)
```

...this tells you that the code at that address was inlined from `_tree.h` and `_function_base.h`. These inlines were almost certainly defined inside a class body in those headers. So the **class you're decompiling lives in one of those files**.

Look at the cluster of files around the target address — the most frequently appearing file, or the one that spans the widest address range, is the most likely home of the class.

## Workflow

### 1. Identify the target address

Get the start address of the function you're decompiling from your disassembler (e.g. Ghidra, IDA, Decomp Toolkit).

### 2. Run the lookup

```bash
python tools/line_lookup.py <mapfile> <address>
```

This returns 50 entries before and 50 entries after the closest match. If the address isn't in the file exactly, the script finds the nearest one and reports the offset.

### 3. Read the output

The matched line is marked with `>>>`. Scan the surrounding entries and look for:

- **Repeated file paths** near the target address — a file appearing many times across a range of addresses strongly suggests the class is defined there
- **File paths that span the widest address range** around the target — the "parent" file containing the class will typically have entries spread across the entire function body
- **The specific file/line combo** for entries right at the target address — these are the inlines that expanded at that exact point

### 4. Cross-reference with source context

Once you have candidate files, use the line numbers in the map entries to look up the exact location in the source (if available). Inline definitions inside class bodies will confirm the class's home file.

## Example Interpretation

Given these entries around a target address:

```
0x801AE61C: src/Gameplay/GManager.cpp (line 2264)
0x801AE624: src/Gameplay/GManager.cpp (line 2265)
0x801AE624: src/Gameplay/GManager.cpp (line 2266)
0x801AE624: libs/support/utility/UCrc.h (line 27)
0x801AE628: src/Gameplay/GManager.cpp (line 2266)
0x801AE62C: libs/support/utility/UCrc.h (line 27)
0x801AE630: src/Generated/Messages/MNotifyMilestoneProgress.h (line 26)
0x801AE634: libs/support/utility/UCrc.h (line 27)
0x801AE63C: src/Generated/Messages/MNotifyMilestoneProgress.h (line 26)
0x801AE640: libs/support/utility/UCrc.h (line 26)
0x801AE644: src/misc/Hermes.h (line 150)
0x801AE648: src/Generated/Messages/MNotifyMilestoneProgress.h (line 23)
0x801AE64C: src/misc/Hermes.h (line 150)
0x801AE650: libs/support/utility/UCrc.h (line 26)
0x801AE654: src/Gameplay/GManager.cpp (line 2267)
0x801AE658: src/misc/Hermes.h (line 150)
0x801AE65C: src/Gameplay/GManager.cpp (line 2267)
0x801AE660: src/Generated/Messages/MNotifyMilestoneProgress.h (line 29)
0x801AE664: libs/support/utility/UCrc.h (line 22)
0x801AE668: src/Generated/Messages/MNotifyMilestoneProgress.h (line 29)
0x801AE66C: src/Gameplay/GManager.cpp (line 2267)
0x801AE670: src/Gameplay/GRaceStatus.h (line 354)
0x801AE688: src/Gameplay/GManager.cpp (line 2271)
0x801AE690: src/Gameplay/GRaceStatus.h (line 353)
0x801AE690: src/Gameplay/GRaceStatus.h (line 362)
0x801AE694: src/Gameplay/GManager.cpp (line 2271)
0x801AE69C: src/Gameplay/GManager.cpp (line 2272)
```

**Analysis:**

- **`GManager.cpp`** spans the entire address range at sequential source lines (2264–2272) — this is the outer function itself, not an inline. The function being decompiled belongs to `GManager`.
- **`GRaceStatus.h`** appears at lines 353, 354, and 362 — a tight cluster of line numbers, characteristic of a short inline method defined inside a class body. **`GRaceStatus` is defined in `GRaceStatus.h`**.
- **`MNotifyMilestoneProgress.h`** appears at lines 23, 26, and 29 — another inline class, likely a small generated message struct. **`MNotifyMilestoneProgress` is defined in that generated header**.
- **`UCrc.h`** appears repeatedly at lines 26–27 across several addresses — a utility inline called multiple times (likely a CRC/hash function). **`UCrc` is defined in `UCrc.h`**.
- **`Hermes.h`** appears three times at line 150 — the same line each time, meaning the same inline is called in a loop or multiple times. **The relevant `Hermes` class method is defined at line 150 of `Hermes.h`**.

The tight line-number clustering (`GRaceStatus.h` lines 353/354/362, `MNotifyMilestoneProgress.h` lines 23/26/29) is the key signal that these are inline methods defined inside class bodies — not free functions pulled in from elsewhere.

## Script Reference

**Usage:** `python tools/line_lookup.py <mapfile> <address>`  
**Address format:** hex, with or without `0x` prefix

The script:

1. Finds the exact or closest address in the map file
2. Prints 50 entries before and 50 entries after, with the match marked `>>>`
3. Reports if the match was approximate and by how much

## Tips

- **Multiple entries at the same address are normal** — they represent different inlines that all expanded at the same point
- **STL or library files appearing heavily** usually means the function operates on a container type; the _game_ class is likely in a nearby non-library file
- **If the address range is tight** (many entries clustered at very close addresses), you're likely looking at a small leaf function — the file attribution is more direct
- **If no game-specific files appear**, try looking up a nearby address (e.g. the next function) — sometimes the first few bytes of a function have no map entries
