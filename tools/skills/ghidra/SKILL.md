---
name: ghidra
description: Ghidra CLI tool reference
---

# Ghidra CLI

The project contains two programs:

- `mw/GOWE69/NFSMWRELEASE.ELF` — Original GameCube ELF (default). Types and symbols loaded from the DWARF.
- `mw/SLES-53558/NFS.ELF` — Alpha 124 build (PS2). Virtual function calls show up correctly.

Switch between them with `ghidra program open --project NeedForSpeed --program <name>`.

Use `-o compact` for shorter output in the CLI. Use `-o json` for machine-readable output when scripting.

```sh
ghidra set-default project prime        # set default project
ghidra decompile 0x80051324 -o compact  # decompile function at address
ghidra find function "AcceptScriptMsg"  # search by name
ghidra type get "CEntity"               # struct layout
ghidra disasm 0x80051324 -n 30 -o compact  # raw disassembly
```

Note: Ghidra uses short demangled names (e.g. `AcceptScriptMsg` matches all overrides).
Use the address from `config/GOWE69/symbols.txt` for precise lookups.

**Important:** Ghidra has no concept of `const`. All pointers, references, and member
functions appear non-const in Ghidra output. Never infer const-qualification (or lack
thereof) from Ghidra decompilation — use the dwarf info from the lookup skill.

#### Disassembly

Ghidra's disassembly output is fine for quick reference, but it's better to reference decomp-toolkit's
generated disassembly files in `build/GAMEID/asm/` which contain more information (e.g. symbol names,
relocation targets, data values) and are easily grepable.
