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

Copy and cleanup the header that you got from running the `lookup` skill using the `symbols/Dwarf` folder. Fix visibility, function order and vtable related things based on using `lookup` on the PS2 types.

For formatting and local cleanup while writing the header, consult
`.github/skills/code_style/SKILL.md`. Use it for member-comment alignment, declaration
grouping, TODO placement, and other repo-specific style decisions.

Preserve the real `class` / `struct` kind while scaffolding. Check existing headers first,
then use Dwarf plus PS2 visibility / vtable info to decide the type kind. Even temporary
forward declarations should match the known original kind.

If the repo already has a header for a type you need, include that header instead of
adding a new local forward declaration. Only forward-declare when no canonical repo header
exists yet and you have verified that the ownership is still unresolved.

Preserve real member names, types, order, and offset comments while scaffolding. Do not
fill gaps with invented `pad`, `unk`, or `field_XXXX` members for game types; verify the
layout from Dwarf / PS2 data and leave a TODO over the type if a field is still uncertain.

Only create headers if it's really necessary (the struct doesn't have inlines so you can't determine in which header file it goes and it's thematically very different from the other structs that use it), otherwise put it into the one you determined to be correct.

The dwarf often has duplicated inlines, clean those up according to the order in the PS2 info.

Write a TODO comment over the struct/class if you aren't 100% sure that it belongs to the correct header.

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
