---
name: scaffold
description: Workflow for creating an accurate header for an unimplemented class.
---

# Class Initialization Workflow

Your goal is to copy the struct and header definitions from the dwarf that capture the exact class layout (vtable, members, sizes) using `find-symbol.py`, the `lookup` and the `line-lookup` skills.

## Phase 1: Gather Information

Collect data from **all** of these sources in parallel where possible:

### 1a. Codebase search

- Check if headers already exist (`include/` and `src/`)
- Find the classes in `config/GOWE69/symbols.txt` — list all member functions
- Find the classes in `config/GOWE69/splits.txt` — note .text, .rodata, .data ranges
- Read the assembly file in `build/GOWE69/asm/` — especially the vtable at the end

### 1b. Reference

- Use the `lookup` skill using the `symbols/Dwarf` folder to get the struct and enum bodies from the dwarf and using the `symbols/PS2/PS2_types.nothpp` file to get the correct order of function declarations inside structs and the order of virtual functions.
- Use the `line-lookup` skill and read `symbols/file_names.txt` to determine in which headers the structs should go.

### 1c. Parent class

- Read the parent class header to understand inherited virtual methods and field offsets

## Phase 2: Setup class

Copy the header/type body that you got from running the `lookup` skill using the
`symbols/Dwarf` folder into the canonical owner header first. Do not retype or
reconstruct the layout from memory, from scattered callsites, or from guessed
semantics.

Then do the minimum cleanup backed by evidence: fix visibility, function order and
vtable related things based on using `lookup` on the PS2 types, and clean up duplicated
inline copies when the DWARF emitted both versions.

For formatting and local cleanup while writing the header, consult
`.github/skills/code_style/SKILL.md`. Use it for member-comment alignment, declaration
grouping, TODO placement, and other repo-specific style decisions.

Preserve the real `class` / `struct` kind while scaffolding. Check existing headers first,
then use Dwarf plus PS2 visibility / vtable info to decide the type kind. Even temporary
forward declarations should match the known original kind.

Keep the header prologue in repo order: header guard, `EA_PRAGMA_ONCE_SUPPORTED` block,
then includes. Do not drop project includes ahead of `#pragma once`.

If the repo already has a header for a type you need, include that header instead of
adding a new local forward declaration. Only forward-declare when no canonical repo header
exists yet and you have verified that the ownership is still unresolved.

Preserve real member names, types, order, and offset comments while scaffolding. Do not
fill gaps with invented `pad`, `unk`, or `field_XXXX` members for game types; verify the
layout from Dwarf / PS2 data and leave a TODO over the type if a field is still uncertain.

Preserve the declaration order from the dumped type body as well, not just the member
order. Do not regroup methods, fields, enums, or helper declarations for readability
unless an existing repo header or PS2 evidence proves the original owner header used a
different order.

Keep the `// total size: 0x...` comment above the recovered type declaration. When the
recovered type is a `class`, keep explicit access sections and prefer putting methods /
accessors before the member layout block unless existing repo evidence says otherwise.

When a recovered field width is known, prefer explicit-width aliases such as `uint8` /
`uint16` over raw `unsigned char` / `unsigned short`. Use plain `char` for string or byte
buffers and `signed char` when the field is a signed 8-bit counter.

If a recovered type repeatedly walks neighbors, boundaries, or in-object offsets, prefer
small named helpers such as `GetTop`, `GetBot`, `GetNext`, `GetPrev`, or boundary getters
instead of repeating raw pointer arithmetic at each call site.

When those helpers operate on addresses or byte offsets, prefer `intptr_t` / `uintptr_t`
or explicit byte-pointer arithmetic instead of plain `int` address parameters.

Only create headers if it's really necessary (the struct doesn't have inlines so you can't determine in which header file it goes and it's thematically very different from the other structs that use it), otherwise put it into the one you determined to be correct.

The dwarf often has duplicated inlines, clean those up according to the order in the PS2 info.

Write a TODO comment over the struct/class if you aren't 100% sure that it belongs to the correct header, and say why (ownership uncertainty, circular dependency, dwarf caveat, etc.) instead of leaving a bare marker.

## Phase 3: Add needed files to jumbo file and compile

Since the project uses jumbo builds, you'll need to add the real source files to the jumbo
build using `#include` statements before your changes can be picked up by the build system.

**Finding the correct jumbo unit:** Use the `line-lookup` skill on any function address from
the class you are scaffolding. The outermost `.cpp` file in the result (the one spanning
the widest sequential address range) is the source file being compiled. Its corresponding
`SourceLists/z*.cpp` jumbo unit is where the new `#include` belongs. For example, if
`line_lookup.py` shows `src/Speed/Indep/Src/zWorld2/CWorldObject.cpp` dominating the range,
the new file goes into `src/Speed/Indep/SourceLists/zWorld2.cpp`.

Try to build afterwards and see if there are any compile errors.

## Phase 4: Report

Summarize:

- Match status of implemented functions
- List of remaining unimplemented (missing) functions
- Any extra symbols that need attention
