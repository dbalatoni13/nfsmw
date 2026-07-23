# NFSMW Match Annotations

This repository-local VS Code extension adds CodeLens annotations above C and C++ functions showing their objdiff and normalized DWARF match percentages. For PS2 projects it skips DWARF, keeps objdiff function annotations, and annotates struct/class definitions using ccc stdump output compared with `symbols/PS2/PS2_types.nothpp`.

The extension maps the current source file through `objdiff.json`, invokes `tools/decomp-diff.py` and `tools/dwarf-compare.py`, and only annotates symbols with the info. Click a mismatching percentage annotation to open the normalized DWARF diff beside the source.

## Running locally

Open this directory in VS Code and start an Extension Development Host, or package it with `npx @vscode/vsce package` and install the resulting VSIX. Open the NFSMW repository as the workspace in the development host.

Use **NFSMW: Refresh Match Annotations for Current File** from the Command Palette after rebuilding a unit. Cached annotations are restored when a source file is reopened. The cache is LRU-pruned and cannot exceed 500 MB; `nfsmwMatch.cacheMaxMB` can lower the limit.

Running refresh again for the same file cancels its in-progress comparison and immediately starts a new one. Use **NFSMW: Disable objdiff Metrics** when objdiff is too slow; refreshes then retain DWARF annotations on non-PS2 versions and PS2 type annotations on PS2 versions. **NFSMW: Enable objdiff Metrics** restores the function metric. Any successfully fetched metric below 100% is also published as an editor warning, so it appears on the source line and in the scrollbar overview ruler.

Annotations are suspended while a source file has unsaved edits so CodeLens rows do not move the editor viewport while typing. Saving only remaps the last known results to the updated symbol positions; it does not rerun comparison tools or recalculate percentages. Use the refresh command when new metrics are wanted.

On the first PS2 refresh, select the ccc `stdump.exe` when prompted. The selected path is stored in the global `nfsmwMatch.stdumpPath` setting. Use **NFSMW: Set PS2 stdump Path** to change it later. Clicking a PS2 type percentage opens its normalized struct-definition diff in the remembered diff editor group.

The extension expects clangd or another C/C++ language server that supplies document symbols. Set `nfsmwMatch.pythonPath` if `python` is not the correct interpreter command.
