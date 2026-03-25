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

## Quick Tooling

Use the repo-local helper before doing a style pass:

```sh
python tools/code_style.py audit --base origin/main
```

- `audit` classifies changed files into default-format vs match-sensitive buckets and reports repo-specific findings.
- `audit` also checks touched `class` / `struct` declarations against known header declarations and, when no header exists, against the PS2 visibility rule.
- `audit` warns on touched local forward declarations when the repo already has a header for that type.
- `audit` warns on touched type members that look like invented padding or placeholder names such as `pad`, `unk`, or `field_1234`.
- `audit` also checks touched style-guide rules that clang-format cannot enforce for you, such as cast spacing, `using namespace`, `NULL`, bare `#if MACRO` presence checks, recovered layout members that still use raw `unsigned char` / `unsigned short`, and missing or misordered `EA_PRAGMA_ONCE_SUPPORTED` guard blocks when a header's prologue is touched.
- `audit` groups repeated findings by file so branch-wide output stays readable.
- Use `audit --category safe-cpp` when you want a smaller Frontend/FEng-focused subset and `audit --category match-sensitive-cpp` when you want a conservative review queue for decomp code.
- `format --check` is an opt-in wrapper around the repo's `.clang-format`, and by default it targets eligible changed C/C++ files, including match-sensitive code.
- Use `format --check --base origin/main` for a branch-wide formatter pass over all changed C/C++ files.
- Use `format --check --base origin/main --category safe-cpp` when you want a branch-level formatter probe instead of spelling every file path out.
- `format --check` labels whitespace-only formatter output separately from other non-whitespace changes.
- `format` also accepts `SourceLists/z*.cpp` and other repo C/C++ files; if a formatting pass touches match-sensitive code, verify the affected unit afterwards.
- Files that use initializer-list guard comments (`//`) are still formatter targets; if a formatting pass touches match-sensitive code, verify the affected unit afterwards.
- `clang-format` itself is optional. If it is not on `PATH`, install it locally or point the helper at it with `CLANG_FORMAT=/path/to/clang-format`.
- Do not assume a formatting-only change is automatically byte-stable; verify affected units when the formatter touches match-sensitive code.

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
- When a helper is doing address arithmetic, prefer `intptr_t` / `uintptr_t` or byte-pointer (`reinterpret_cast<char *>`) math over plain `int` parameters or integerized pointer subtraction.
- Inline assembly is acceptable when it is needed to preserve dead-code compares, ordering, or other compiler behavior that source alone cannot reproduce.
- In low-level list / node / allocator code, prefer existing helper methods such as `AddBefore`, `AddAfter`, `Remove`, `GetPrev`, `GetNext`, or typed accessors over open-coding link rewiring once the helper exists.

### Header prologues and preprocessor checks

- In headers, keep the guard / `EA_PRAGMA_ONCE_SUPPORTED` block before any project `#include`; do not place includes ahead of `#pragma once`.
- Use `#ifdef MACRO` / `#ifndef MACRO` for presence checks. Reserve bare `#if MACRO` for cases where you really need the macro's numeric value.

### Forward declarations and local prototypes

- Prefer including the owning repo header over adding a local forward declaration for a project type.
- If the repo already has a header declaration/definition for a type, include that header instead of redeclaring the type locally.
- If the repo only has an empty or stub owner header, and line info / surrounding source clearly points at that header's subsystem, prefer populating that owner header over leaving a recovered project type declaration inside a `.cpp`.
- Do not create a duplicate micro-header that re-declares one type already owned by a broader subsystem header just to dodge that include. Include the real owner header and rewrite the caller back toward the original API shape instead of inventing one-off constructors or wrappers in the duplicate header.
- Only keep a local forward declaration when no canonical repo header exists yet and you have verified that the ownership is still unresolved.
- Likewise for project free functions: if a declaration is shared across translation units, move it into the owning header instead of leaving ad-hoc local prototypes in `.cpp` files.
- For COM-style interfaces that declare `static HINTERFACE _IHandle();`, preserve the original ownership. If the interface wants an out-of-line handle, define the real `Type::_IHandle()` member in the owning TU (or use the header's outline hook macro) instead of inventing a free `extern "C"` shim with an `asm("...")` alias.
- In constructors for out-of-line `_IHandle` owners, check local precedent before calling `_IHandle()`. In this repo, several matching interfaces want `(HINTERFACE)_IHandle` passed straight to `UTL::COM::IUnknown`, and using the call form can perturb both codegen and DWARF ownership.
- If DWARF says a tiny virtual destructor was inline, do not assume "move it into the header" is automatically safe. In jumbo TUs that can improve one destructor callsite while also making objdiff report the standalone deleting-dtor symbol as missing from the owner TU, so always rebuild and check both the caller and the destructor symbol before keeping that rewrite.
- Prefer moving helper template declarations next to their real use site instead of leaving them in an unrelated file.

### Pointer style

- Prefer `Type *name` over `Type* name`.
- Do not do broad pointer-style sweeps in match-sensitive files; change a small batch and verify the affected unit.

### Header layout and data carriers

- Use the repo's header guard form when writing headers: `#ifndef` / `#define` plus the `#ifdef EA_PRAGMA_ONCE_SUPPORTED` / `#pragma once` block.
- Keep member layout comments aligned and intact in decomp headers.
- When writing a recovered layout, start from a pasted GC DWARF dump instead of hand-reconstructing a cleaner version. Treat the dump as source-of-truth data entry, then make only small verified fixes from PS2 or existing headers.
- Preserve the original `class` / `struct` kind from existing headers or Dwarf / PS2 evidence; do not treat it as a cosmetic style choice.
- Treat header declarations as the repo source of truth. If the repo only has local `.cpp` partial declarations, verify the kind with the PS2 dump instead of copying them blindly.
- Even forward declarations and local partial declarations should use the accurate keyword when known.
- If Dwarf and PS2 disagree on `struct` vs `class`, treat PS2 visibility sections as the tie-breaker for the real owner kind and then rebuild the affected TU to confirm the shape is byte-stable.
- Keep the `// total size: 0x...` comment above the recovered type declaration instead of burying it inside the body.
- When a recovered type is a `class`, keep explicit access sections and put the method/accessor block before the member layout block unless existing repo evidence shows otherwise.
- Preserve the member naming style that DWARF shows. Some types use `mMember`, others use `m_member`; do not normalize them.
- Preserve recovered member names, types, order, and offset comments. Do not invent placeholder members named `pad`, `unk`, `unknown`, or `field_XXXX` for game code just to make a layout compile.
- Preserve the dumped declaration order too. Do not regroup methods, helpers, enums, or fields for readability unless an existing repo header or PS2 evidence proves the original order differs.
- If a member is genuinely unknown, stop and verify it with `find-symbol.py`, GC Dwarf, and PS2 data. If the layout is still incomplete, add a short TODO above the type instead of burying uncertainty in fake member names.
- Add offset / size comments when you are writing recovered type layouts from DWARF.
- In recovered layouts, prefer explicit-width aliases such as `uint8` / `uint16` when the field width is known. Use plain `char` for text / byte buffers and `signed char` when the field is a signed 8-bit counter.
- Define inline member functions in headers only when DWARF shows that they are genuinely inlined in the binary.
- Use `struct` for POD-like data carriers with public fields; use `class` for behavior-heavy types only when that matches the recovered type information.
- Keep tiny placeholder methods as concise inline bodies when that is already the local pattern.

### Namespaces and container aliases

- Do not add `using` directives.
- Keep namespace-qualified types explicit at point of use.
- When introducing `UTL::Std::list` / `UTL::Std::vector` aliases that rely on a `_type_` helper, pair them with `DECLARE_CONTAINER_TYPE`.

### Dense local code

- Expand dense one-line helper structs, declaration blocks, and function bodies in non-match-sensitive files into normal multiline formatting.
- In low-level headers, prefer normal multi-line bodies for touched inline operators and accessors instead of stacking `{ return ...; }` on one line, unless the surrounding file clearly uses intentional placeholder one-liners.
- Prefer readable blocks over stacked one-line statements when behavior does not depend on exact source shape.
- In touched validation/parsing code, prefer explicit min/max or boundary checks over equivalent magic-constant arithmetic when the clearer form still compiles to the verified result.
- In parser/state-table code, prefer named enums and enum-typed state variables over anonymous integer state codes when that rewrite is verified safe.

### Recovery markers

- Remove stale recovery markers such as `// TODO`, `// UNSOLVED`, or `// STRIPPED` when the touched code is now implemented or understood.
- If a marker still needs to stay, give it short context such as ownership uncertainty, a Dwarf caveat, a platform/config note, or a scratch/link reference. Avoid bare marker-only comments.
- Do not leave `// TODO` hanging off a declaration or helper you just implemented; either finish the thought or remove the marker.

### Uncertain ownership

- If a declaration or global clearly compiles but its original home is uncertain, add a short TODO comment instead of inventing structure you cannot justify yet.
- When ownership matters, verify it with `decomp-workflow.py`, `decomp-context.py`, and `line-lookup` before moving code.

### Readable helper extraction

- When touched recovered code repeats the same pointer/boundary arithmetic, prefer a short named helper or accessor such as `GetTop`, `GetBot`, `GetNext`, `GetPrev`, `GetStringTableStart`, or `GetStringTableEnd` if that shape is already supported by Dwarf/inlining evidence.
- Prefer call sites that use those helpers or existing container APIs over re-encoding the same arithmetic or link manipulation inline.
- If a touched caller is using a local `*Access` shim with placeholder padding just to peek at another type's private members, prefer a narrow inline accessor on the real type over keeping the fake layout struct in the caller.

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
- Header prologues should keep the `EA_PRAGMA_ONCE_SUPPORTED` block ahead of includes, not after them.
- Bare `#if MACRO` presence checks are review bait; use `#ifdef` / `#ifndef` unless you are intentionally testing a numeric config value.
- Reviewed recovered headers tend to keep total-size comments above the type, methods before fields, explicit access sections, and fixed-width aliases for width-known narrow integer members.
- Recent `zMisc` review cleanup also showed that hand-reconstructed structs and reordered declarations create avoidable churn; copy recovered layouts from DWARF into the owner header first and keep the dumped order unless PS2/header evidence proves a correction.
- Reviewed fixups also remove stale bare recovery markers or replace them with context, and prefer existing list/node helpers over hand-written pointer/link rewiring.
- Some reviewed fixups improved readability without losing match by replacing opaque range-check arithmetic with explicit bounds and by moving repeated pointer/boundary math behind short named helpers.
- Other recurring review churn came from plain-`int` address helpers, stray local `.cpp` prototypes for shared functions, and integer-coded parser states where named enums were clearer but still matched.
