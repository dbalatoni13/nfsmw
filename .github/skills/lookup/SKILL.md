---
name: game-lookup
description: Queries structs, enums, functions, globals, and typedefs from pre-extracted game dump files using lookup.py. Use this during decompilation to get exact type layouts, field offsets, enum values, and function bodies without loading entire files into context. Also queries the PS2 dump for vtable layout and member visibility to determine struct vs class.
---

# SKILL: Using lookup.py for Game Decompilation

## Purpose

`lookup.py` is a precision query tool for game decompilation. It extracts exactly one symbol from pre-extracted game dump files and prints it — nothing else. Use it constantly during decompilation to avoid guessing types, sizes, field offsets, function signatures, and enum values. **Never assume you know a type layout or field offset. Always look it up.**

---

## The Two Data Sources

You always have access to two independent data sources. Use both.

### 1. `./symbols/Dwarf` — Primary source (folder mode)

This is your main reference for decompiling code. It contains:

```
./symbols/Dwarf/
  globals.nothpp    — structs, enums, typedefs, global variable declarations
  functions.nothpp  — function bodies with address annotations
```

Use this for everything: function bodies, struct layouts, enums, globals, typedefs.

```bash
python tools/lookup.py ./symbols/Dwarf struct AITarget
python tools/lookup.py ./symbols/Dwarf function 0x801DE9AC
python tools/lookup.py ./symbols/Dwarf enum EPerfectLaunch::State
python tools/lookup.py ./symbols/Dwarf global TheAnimCandidateData
python tools/lookup.py ./symbols/Dwarf typedef HHANDLER
```

### 2. `./symbols/PS2/PS2_types.nothpp` — PS2 dump (single-file mode)

Use this as a secondary reference for two specific purposes:

**a) Vtable layout** — the PS2 dump often contains more complete vtable information. When you need to know the virtual method order or which methods a class overrides, look it up here.

**b) Visibility** of struct members (functions and variables)

**c) `struct` vs `class` determination** — Look up the type in the PS2 dump and apply this rule:

- If the PS2 definition has **no visibility modifiers** (`public:`, `private:`, `protected:`) anywhere in the body → declare it as `struct`
- If the PS2 definition has **any visibility modifiers** → declare it as `class`

**d) The correct declaration order of functions inside a struct/class.**

```bash
python tools/lookup.py --file ./symbols/PS2/PS2_types.nothpp struct RollingBlockFormation
```

Always check the PS2 dump for every non-trivial type you write, even if the Dwarf dump already has it.

---

## Command Syntax

```bash
# Folder mode (primary — Dwarf dump)
python tools/lookup.py <folder> <kind> <query>

# Single-file mode (PS2 dump or any combined file)
python tools/lookup.py --file <path> <kind> <query>
```

Valid kinds: `struct`, `enum`, `function`, `global`, `typedef`

---

## Lookup Reference

### struct

Look up a type's field layout. Always do this before writing any code that accesses struct members.

```bash
python tools/lookup.py ./symbols/Dwarf struct AITarget
python tools/lookup.py --file ./symbols/PS2/PS2_types.nothpp struct AITarget
```

Output includes field names, types, byte offsets, and sizes. When a struct appears multiple times with different members (e.g. a forward declaration vs full definition), all variants are printed separated by a blank line — use the one that you determine to be the correct one.

### enum

Look up an enum's values. Use this when you encounter an integer that is clearly used as a state/flag/type discriminator.

```bash
# Top-level enum
python tools/lookup.py ./symbols/Dwarf enum ELaunchType

# Enum nested inside a struct — use StructName::EnumName
python tools/lookup.py ./symbols/Dwarf enum AIGoal::State
```

### function

Look up a function's decompiled body by its **start or end address**.

```bash
python tools/lookup.py ./symbols/Dwarf function 0x801DE9AC
```

The address can match either the start or end of the `// Range:` annotation. Case-insensitive.

### global

Look up a global variable declaration by name.

```bash
python tools/lookup.py ./symbols/Dwarf global TheAnimCandidateData
```

### typedef

Look up a typedef by its alias name.

```bash
python tools/lookup.py ./symbols/Dwarf typedef HHANDLER
```

---

## Checking for Existing Definitions

**Before declaring any new symbol** (struct, class, enum, global variable, typedef, or any other type), use clangd to search for an existing definition in the source tree. Only declare it yourself if clangd finds nothing.

Use clangd's workspace symbol search to look up the name:

```
clangd: workspace/symbol { "query": "AIGoal" }
```

Or equivalently via any LSP-capable tool:

```
clangd: textDocument/definition  (on any usage of the symbol)
```

If clangd returns a definition inside `./src/`:

- **Do not redeclare it.** Include the header that contains it instead.
- Note the file path and `#include` it in your translation unit.

If clangd finds nothing in `./src/`:

- Declare it yourself, using the Dwarf and PS2 dumps as the source of truth for layout and `struct` vs `class`.

This applies to **all** of the following before writing them:

- `struct` / `class` definitions
- `enum` / `enum class` definitions
- Global variable declarations (`extern` or otherwise)
- `typedef` declarations
- Any other named type

**Never assume a symbol doesn't exist elsewhere in the project just because you haven't seen it yet.**

---

## Key Rules

- **Always use `./symbols/Dwarf` as the primary source** for all decompilation work.
- **Always check `./symbols/PS2/PS2_types.nothpp`** for every type to determine `struct` vs `class` and vtable layout.
- **`struct` vs `class` rule:** no visibility modifiers in PS2 dump → `struct`; any visibility modifier present → `class`.
- **Never declare a symbol without first searching for it with clangd.** If it exists in `./src/`, include that header instead.
- **Never guess field offsets or sizes.** Look them up.
- **Nested enums** must be queried as `StructName::EnumName`, not just `EnumName`.
- **Function addresses** are hex. Always include the `0x` prefix.
- **Do not load entire files into context.** Fetch only the one symbol you need right now.

---

## Error Handling

| Error message                        | What to do                                                                   |
| ------------------------------------ | ---------------------------------------------------------------------------- |
| `Error: struct 'Foo' not found.`     | Try alternate spellings: `_Foo`, `CFoo`, `IFoo`, or check if it is a typedef |
| `Error: function '0x...' not found.` | Confirm the address is correct; try the end address instead of the start     |
| `Error: enum 'Bar' not found.`       | Try `SomeStruct::Bar` if it may be nested                                    |
| `Error: '...' is not a directory.`   | You are in folder mode but the path is a file — add `--file`                 |
| `Error: a folder path is required`   | You forgot the folder argument and did not pass `--file`                     |
