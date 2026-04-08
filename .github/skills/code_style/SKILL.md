---
name: code-style
description: Repo-specific code style and match-safe cleanup guidance for code-writing tasks.
---

# Code Style Workflow

Use this skill when writing new code, polishing code you already touched, or doing a style review of a branch.

This skill is about **code style only**: formatting, declaration placement, header layout, local readability, and repo-specific conventions. It is **not** a PR-response workflow and it is **not** a license to do broad cleanup sweeps.

It also tracks the repo's written `STYLE_GUIDE.md` rules where they fit the decomp workflow.

## Core Principle

In this repo, style cleanup must preserve decomp progress.

- In match-sensitive code, prefer the smallest local cleanup that keeps the code readable.
- If a style tweak changes codegen or match status, revert it.
- Extend this skill only from patterns you actually verified in the repo.

## Phase 1: Classify the File Before Cleaning

Decide which bucket the file belongs to:

### 1a. Match-sensitive decomp code

Examples:

- gameplay / camera / physics / world translation-unit source
- utility headers and templates included by matched code
- headers with layout-sensitive inlines or emitted virtual methods

For these files, style cleanup must be conservative and verified.

### 1b. Default-format support / frontend / tooling code

Examples:

- frontend interface shims
- scripts, tooling, and agent docs
- non-match-critical glue code

These files can take normal readability cleanup, but still follow repo conventions.

## Phase 2: Apply Repo-Specific Style Rules

### Jumbo source-list files

- Keep deliberate blank lines between `#include` entries when they help prevent clang-format from collapsing or reshuffling the jumbo list.
- Do not leave stray helper declarations in a `SourceLists/z*.cpp` file when they really belong near a use site in the underlying implementation file.

### Constructors and initializer lists

- Preserve the repo's multiline initializer-list style.
- Keep the trailing `//` markers on each initializer line except the last when that pattern is already being used to keep clang-format from collapsing the list.

Example:

```cpp
Foo::Foo()
    : a(0), //
      b(1), //
      c(2) {}
```

### Casts, nulls, and low-level code

- Use C++ casts instead of C-style casts.
- Spell casts without spaces inside the angle brackets: `static_cast<Type *>(expr)`, not `static_cast< Type * >(expr)`.
- Use `nullptr` exclusively for null pointers.
- Prefer `if (ptr)` / `if (!ptr)` over explicit null comparisons when the change is local and verified safe.
- When a match-sensitive TU has many explicit `nullptr` checks and you decide to normalize them, prefer one mechanical full-TU pass over piecemeal cleanup. Rebuild the unit and re-check its status before keeping the rewrite.
- Inline assembly is strictly only allowed in math functions.

### Forward declarations and local prototypes

- Prefer including the owning repo header over adding a local forward declaration for a project type.
- If the repo already has a header declaration/definition for a type, include that header instead of redeclaring the type locally.
- If the repo only has an empty or stub owner header, and line info / surrounding source clearly points at that header's subsystem, prefer populating that owner header over leaving a recovered project type declaration inside a `.cpp`.
- Only keep a local forward declaration when no canonical repo header exists yet and you have verified that the ownership is still unresolved.
- Prefer moving helper template declarations next to their real use site instead of leaving them in an unrelated file.

### Pointer style

- Prefer `Type *name` over `Type* name`.
- Do not do broad pointer-style sweeps in match-sensitive files; change a small batch and verify the affected unit.

### Header layout and data carriers

- Use the repo's header guard form when writing headers: `#ifndef` / `#define` plus the `#ifdef EA_PRAGMA_ONCE_SUPPORTED` / `#pragma once` block.
- Keep member layout comments aligned and intact in decomp headers.
- Treat header declarations as the repo source of truth. If the repo only has local `.cpp` partial declarations, verify the kind with the PS2 dump instead of copying them blindly.
- Preserve the member naming style that DWARF shows. Some types use `mMember`, others use `m_member`; do not normalize them.
- Preserve recovered member names, types, order, and offset comments. Do not invent placeholder members named `pad`, `unk`, `unknown`, or `field_XXXX` for game code just to make a layout compile.
- If a member is genuinely unknown, stop and verify it with `find-symbol.py`, GC Dwarf, and PS2 data. If the layout is still incomplete, add a short TODO above the type instead of burying uncertainty in fake member names.
- Add offset / size comments when you are writing recovered type layouts from DWARF.
- Define inline member functions in headers only when DWARF shows that they are genuinely inlined in the binary.

### Namespaces and container aliases

- Do not add `using` directives.
- Keep namespace-qualified types explicit at point of use.
- When introducing `UTL::Std::list` / `UTL::Std::vector` aliases that rely on a `_type_` helper, pair them with `DECLARE_CONTAINER_TYPE`.

### Dense local code

- Expand dense one-line helper structs, declaration blocks, and function bodies in non-match-sensitive files into normal multiline formatting.
- Prefer readable blocks over stacked one-line statements when behavior does not depend on exact source shape.

### Uncertain ownership

- If a declaration or global clearly compiles but its original home is uncertain, add a short TODO comment instead of inventing structure you cannot justify yet.
- When ownership matters, verify it with `decomp-workflow.py`, `decomp-context.py`, and `line-lookup` before moving code.

## Phase 3: Things Not To "Clean Up" Blindly

- Do not move an inline method out of a header just because it looks cleaner.
- Do not broad-format utility templates or virtual interfaces without checking who includes them.
- Do not rewrite expression structure in a near-matching function just to satisfy a style preference.
- Do not replace repo-specific formatting conventions with generic modern C++ preferences.

## Phase 4: Verify Risky Style Changes

For match-sensitive translation units:

```sh
python tools/decomp-workflow.py build -u main/Path/To/TU
python tools/decomp-status.py --unit main/Path/To/TU
```

For safer but still compiled code:

```sh
python tools/decomp-workflow.py build -u main/Path/To/OtherTU
```

Keep the cleanup only if the build succeeds and the relevant match status is unchanged.

## Branch Patterns Confirmed So Far

- Blank-line spacing in jumbo source-list include blocks is intentional and worth preserving.
- Helper template declarations should live near the file that actually uses them, not in the jumbo source-list file.
- The trailing `//` initializer-list markers are an intentional repo convention, not noise to remove.
- Small `if (ptr)` cleanup batches can be kept in match-sensitive code, but only after rebuilding the affected unit.
- Dense frontend shim files benefit from multiline struct/prototype/function formatting.

The Ghidra output of boolean logic is messy, use `bFadingOut = false;` instead of `*reinterpret_cast<int *>(&bFadingOut) = 0;`

Use implicit conversion: Directly pass strings instead of `UCrc32(string)`

You are strictly not allowed to use made up local names which aren't in the DWARF, especially ones straight from the Ghidra output (e.g. `p_Var4`).
