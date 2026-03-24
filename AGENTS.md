# Need for Speed Most Wanted Decompilation

Matching decompilation of Need for Speed Most Wanted 2005 (GameCube) targeting the USA Release build (`GOWE69`).
The goal is to produce C++ source that compiles to byte-identical and dwarf-identical object code against the
original retail binary using the ProDG GC 3.9.3 compiler, which is GCC 2.95-based under the hood.

## Build & Verify

```sh
python configure.py    # generate build.ninja + objdiff.json (run after config changes)
ninja all_source       # build all objects
ninja                  # build all objects, hash check and progress report
ninja baseline         # generates baseline report for regression checking
ninja changes          # check for regressions after code changes (empty = no regressions)
python tools/build_matrix.py              # sequential full `ninja` across GC/Xbox/PS2, then restore GOWE69
python tools/build_matrix.py --all-source # sequential compile-only smoke check across GC/Xbox/PS2
```

Use `python tools/build_matrix.py` when you want one command that verifies the current
worktree across all supported platforms. It runs `configure.py --version ...` and the
selected ninja target sequentially, writes per-platform logs under `build/<VERSION>/logs/`,
prints failure tails with the exact failing command, and restores the worktree to
`GOWE69` by default when it finishes.

## Project Layout

```
src/                   C++ source and headers
include/               glibc headers
config/GOWE69/         Symbol addresses, section splits
  symbols.txt          Mangled symbol names -> addresses
  splits.txt           Memory layout / section boundaries
orig/                  Original game files
tools/                 Build system, agent tooling and other scripts
objdiff.json           Generated build/diff configuration
```

## Agent Tooling

## Sub-Agent Usage

Sub-agents are **strictly prohibited**. Do not use sub-agents for any tasks (whether read-only exploration or editing). All work must be performed by the main worker directly.

## Forbidden Changes

Do **not** edit or otherwise touch the comparison and configuration inputs that define the
project's match metrics:

- `config/GOWE69/symbols.txt`
- `config/GOWE69/splits.txt`
- `configure.py`

Treat these files as read-only unless the user explicitly asks for a task that is specifically
about maintaining that infrastructure.

Do **not** try to cheat objdiff, progress, or match metrics in any way. The goal is to improve
the real decompilation output, not to manipulate the comparison setup, hide mismatches, or make
progress numbers look better without actually matching the original code.

**Never** copy, overwrite, or symlink a compiled source `.o` file into `build/GOWE69/obj/`.
The `obj/` directory contains the **original reference objects** extracted from the retail
binary by `dtk dol split`. Replacing them with your own compiled output will make objdiff
compare your code against itself, producing a false 100% match. If the `obj/` file is
accidentally corrupted, regenerate it with:

```sh
rm build/GOWE69/config.json
ninja build/GOWE69/config.json   # re-splits from the original ELF
```

### lookup.py — Symbol lookup from the debug dump

Query structs, enums, functions, globals, and typedefs directly from the pre-extracted
Dwarf dump.

See `.github/skills/lookup/SKILL.md` for the full workflow.

### lookup_address.py — Locate classes and inlines via debug line mapping

When you have a function's address and want to know which source file a class or inline
originates from, use this script against the compiler-generated debug line mapping:

See `.github/skills/line_lookup/SKILL.md` for the full workflow.

`line_lookup.py` now accepts both the original `0xADDR:` debug-line format and rebuilt
object exports written as bare `ADDR:` lines, so you can point it at
`symbols/debug_lines.txt` or at a rebuilt `debug_lines.txt` from
`tools/dwarf1_gcc_line_info.py`.

### elf_lookup.py — Resolve strings / rodata by virtual address

When you have a virtual address inside the original ELF and need to know which string or
rodata bytes live there, use:

```sh
python tools/elf_lookup.py 0x803E58F4
python tools/elf_lookup.py 0x803E58F4 --mode bytes --length 32
python tools/elf_lookup.py 0x002F1234 --game ps2
```

This is the preferred replacement for ad-hoc Python snippets that manually parse the ELF
to chase `@stringBase0` or other rodata/data references.

### code-style — Repo-local style guidance

When you are writing code, polishing code you already touched, or doing a style-review pass,
consult `.github/skills/code_style/SKILL.md` first. It captures repo-specific formatting and
cleanup rules, including jumbo include spacing, initializer-list comment markers, declaration
placement, pointer style, and how to keep style work safe in match-sensitive code.

Use `python tools/code_style.py audit --base origin/main` before a branch-wide style pass.
It classifies changed files, reports repo-specific findings, and can run clang-format
across eligible changed C/C++ files by default.

### decomp-diff.py — Diff & symbol overview

Overview mode lists all symbols in a translation unit with match status:

```sh
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -s nonmatching -t function
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -s missing -t function
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim --search RemoveIOWin
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -d FindIOWin --reloc-diffs all
```

Filters: `-t function,object` (type), `-s missing|matching|nonmatching|extra` (status),
`--section .text`, `--search <pattern>` (fuzzy name match), `--reloc-diffs none|name_address|data_value|all`
(surface relocation-only mismatches when needed; default: `none`).

Diff mode shows side-by-side instruction comparison:

```sh
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -d DistributeOneMessage
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -d FindIOWin -C 5
```

Mismatched args are wrapped in `{}`. Matching runs are collapsed (control with `-C <n>` context lines, `--no-collapse`). Left = original, right = decomp.

### decomp-status.py — Project-wide progress

```sh
python tools/decomp-status.py                    # all categories
python tools/decomp-status.py --category game    # filter to game code
python tools/decomp-status.py --unit main/Speed/Indep/SourceLists/zAnim
python tools/decomp-status.py --json             # machine-readable
```

### decomp-context.py — Function context for matching work

Gathers a scoped source excerpt, objdiff diff, and Ghidra decompile for a specific function:

```sh
python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f AcceptScriptMsg
python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --no-source
python tools/decomp-context.py --ghidra-check   # verify Ghidra CLI is set up correctly
```

Flags: `--no-source`, `--no-ghidra` to skip sections. Source output is automatically scoped
to the function's line range (with a few lines of context) instead of dumping the whole file.

### decomp-workflow.py — Wrapper for common agent workflows

Prefer this wrapper for routine agent-driven flows instead of manually chaining
`decomp-context.py`, `decomp-diff.py`, and `decomp-status.py`:

```sh
python tools/decomp-workflow.py health
python tools/decomp-workflow.py health --full main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py health --full main/Speed/Indep/SourceLists/zAnim --timings
python tools/decomp-workflow.py health --smoke-build main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py health --smoke-dtk main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py next --category game --limit 10
python tools/decomp-workflow.py next --unit main/Speed/Indep/SourceLists/zAnim --limit 5
python tools/decomp-workflow.py build -u main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py diff -u main/Speed/Indep/SourceLists/zAnim -d FindIOWin
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --brief
python tools/decomp-workflow.py verify -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --ghidra-version gc
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --lookup-mode full
python tools/decomp-workflow.py unit -u main/Speed/Indep/SourceLists/zAnim --search FindIOWin --limit 20
```

The wrapper keeps the existing tools as the source of truth. It is intended to reduce
repeated command chaining and to standardize routine worktree preflight checks for agents.
`next --unit`, `function`, `unit`, and `diff` now also auto-build the unit's shared `.o`
once when that output is missing, so wrapper-first inspection works more often on
half-prepared worktrees.

Use `health --full <unit>` when you want one end-to-end tooling smoke test for a worktree.
It reuses a single shared build for the build smoke, DTK dump, rebuilt debug-line export,
and rebuilt `line_lookup.py` check, so it is faster and more representative than chaining
`--smoke-build` and `--smoke-dtk` separately. Add `--timings` when you are diagnosing a
slow worktree or compiler/tool startup.

In normal agent work, use the wrapper commands first. Drop to the raw backend tools only
when you specifically need a backend-only flag, are debugging a wrapper/backend discrepancy,
or are doing a final exhaustive check that the wrapper does not expose directly.

When you do not already have a specific target in mind, start with `next` or `unit`
instead of picking functions in raw objdiff order. `next` is the fastest way to answer
"what should I work on now?":

- `--strategy balanced` favors functions with large remaining gains, penalizes
  high-match cleanup work, de-prioritizes obvious init/setup sinkholes, and prefers
  targets with usable source context.
- `--strategy impact` is the blunt "largest unmatched byte loss first" view.
- `--strategy quick-wins` biases toward low-match functions where getting the first
  40-60% tends to be much faster than squeezing a polished function from 95% to 100%.
  It should not be treated as a cleanup/polish mode.

When choosing what to work on next, bias toward low-match, high-remaining functions.
As a rule of thumb, getting a function from 0% to 80% is usually much faster and higher
leverage than pushing a function from 90% to 100%.
Leave 85%+ cleanup and refiner-style polish for deliberate cleanup passes unless the
user explicitly wants that work or the function is directly blocking something else.

`function` is the preferred context-gathering entrypoint: it bundles source excerpt,
objdiff status/diff, compact GC DWARF function lookup, and Ghidra output in one run.
If the unit metadata points at an empty or otherwise useless source-list file, it also
falls back to the GC debug-line-mapped repo source file when that file exists and has
real content.
Add `--brief` when you want to keep the helper sections compact; it trims suggested
commands and related-source hints without hiding the core status/diff/source data.

For every function you touch, treat DWARF as a first-class completion gate, not a
secondary polish pass. After each meaningful code/build iteration, run the wrapper's
combined verification flow:

```sh
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

`verify` fails unless **both** checks are exact for that function:

- objdiff instruction match is 100%
- normalized DWARF block match is exact

Pass the normal demangled function name to `verify`. The objdiff side matches against both
the demangled and symbol-name fields, and the DWARF side reuses the demangled-name lookup
matching that also tolerates omitted leading namespaces when that information is inconsistent.

If the combined check fails, then inspect the DWARF diff directly with:

```sh
python tools/decomp-workflow.py dwarf -u main/Path/To/TU -f FunctionName
```

It compares the original and rebuilt DWARF blocks for one function, prints a normalized
DWARF match percentage, and shows a diff-like view of what still differs. Use it
whenever `verify` says the function is still failing the DWARF gate. This is the
fastest way to see whether you are still missing locals, have the wrong inline body, or
changed signature/type details even when the instruction diff already looks good.

It also compares the debug-line ownership of each `// Range:` block. Treat the
`Range source ownership` summary as the fast inline-placement check: file mismatches are
strong evidence that an inline body came from the wrong header or owner file. The exact
file+line count is stricter and mainly useful as a secondary hint, not as the main gate.

### prodg_dump.py — ProDG compiler-state dump helper

When you need the exact ProDG compiler state for one unit, prefer this helper over
reconstructing long `ngccc` / `cc1plus` command lines by hand. It recovers the real
`ngccc.exe` invocation from `ninja -t commands`, derives the matching preprocess step,
runs `cc1plus.exe -da`, and can extract or diff one function across dump sets.

```sh
python tools/prodg_dump.py command -u main/Speed/Indep/SourceLists/zAttribSys
python tools/prodg_dump.py dump -u main/Speed/Indep/SourceLists/zAttribSys -o /tmp/zattrib_base
python tools/prodg_dump.py extract /tmp/zattrib_base --stage lreg \
    -f 'VecHashMap<unsigned int,Attrib::Class,Attrib::Class::TablePolicy,false,16>::UpdateSearchLength'
python tools/prodg_dump.py diff /tmp/zattrib_dumps /tmp/zattrib_dumps \
    --left-base-name base --right-base-name preinc --stages lreg,greg,rtl \
    -f 'VecHashMap<unsigned int,Attrib::Class,Attrib::Class::TablePolicy,false,16>::UpdateSearchLength'
```

Use `extract --grep ... -C <n>` when you only want a few interesting lines inside a
function block, such as stack-slot references or one pseudo register family. `diff`
prints a unified diff for each requested stage, and for `lreg` it also summarizes
register-preference and final hard-register assignment changes so allocator/regclass
shifts stand out immediately.

When working with these tools, do not just work around recurring friction silently. If you
notice a clear, safe workflow or tooling improvement that would make future decomp work
faster, shorter, or more reliable, prefer implementing that improvement as part of the task
instead of leaving the paper cut in place. Favor small, surgical tuning to wrappers, shared
helpers, error messages, output shaping, and context-gathering defaults when they remove
repeated manual steps for future agents.

On a newly updated or unusual worktree, run `python tools/decomp-workflow.py health` first.
If it reports missing generated files such as `objdiff.json` or `build.ninja`, run
`python configure.py` in that worktree before using the decomp wrappers. `health` also
checks the debug-symbol side of the setup now, plus the wrapper binaries themselves:
`objdiff-cli`, `dtk`, GC/PS2 `symbols.txt`, GC DWARF lookup, PS2 type lookup, and the
GC debug line mapping.

### find-symbol.py — Check for existing definitions before declaring new types

Before declaring any new struct, class, enum, global, or typedef, run this to check whether
it already exists in `src/`. This is the CLI alternative to clangd workspace/symbol search.

```sh
python tools/find-symbol.py AITarget
python tools/find-symbol.py CEntity --type class
python tools/find-symbol.py EState --type enum
```

If it prints "Not found: ... Safe to declare", you can proceed to define the symbol.
If it finds a match, include that header instead of redeclaring.

### find-symbol.py — Check for existing definitions before declaring new types

Before declaring any new struct, class, enum, global, or typedef, run this to check whether
it already exists in `src/`. This is the CLI alternative to clangd workspace/symbol search.

```sh
python tools/find-symbol.py AITarget
python tools/find-symbol.py CEntity --type class
python tools/find-symbol.py EState --type enum
```

If it prints "Not found: ... Safe to declare", you can proceed to define the symbol.
If it finds a match, include that header instead of redeclaring.

### dtk (decomp-toolkit)

Dump the dwarf of your own implementation of a function after rebuilding the unit normally (ignore dwarf specific errors):

```sh
python tools/decomp-workflow.py build -u main/Speed/Indep/SourceLists/zAnim
build/tools/dtk dwarf dump build/GOWE69/src/Speed/Indep/SourceLists/zAnim.o -o /tmp/zAnim_check.nothpp
```

Demangle a symbol (you probably won't need this):

```sh
dtk demangle 'AcceptScriptMsg__7CEntityF20EScriptObjectMessage9TUniqueIdR13CStateManager'
```

### share_worktree_assets.py — Share stable assets across git worktrees

Deduplicate immutable debug inputs and downloaded tool binaries across all git
worktrees while keeping per-worktree generated build files local:

```sh
python tools/share_worktree_assets.py link --all
python tools/share_worktree_assets.py status --all
```

This shares extracted `orig/*` contents, `symbols/*`, root ELF / MAP files, and
downloaded tool binaries under `build/`. It does **not** share `build.ninja`,
`objdiff.json`, `compile_commands.json`, or per-worktree object outputs, so run
`python configure.py` inside each worktree after linking.

## Code Conventions

This is a **C++98** codebase compiled with ProDG GC 3.9.3 (GCC 2.95 under the hood). Key rules:

- No `auto`, range-for, `enum class`, lambdas, or any C++11+
- Enum values use prefix: `enum EFoo { kF_Value1, kF_Value2 }` (not `enum class`)
- Use C++ casts (`static_cast<T>(expr)`) instead of C-style casts
- Header guards should use `#ifndef` / `#define` together with the `EA_PRAGMA_ONCE_SUPPORTED` block when writing repo headers
- Constructors use initializer list style with leading `, ` on each line, add empty comments at the end of these lines (except the last) to stop clang-format from putting them all on the same line
- Inline assembly is acceptable when needed to reproduce dead code or compiler scheduling that source alone cannot express cleanly
- Preserve the original `class` vs `struct` kind. Check existing headers first, then Dwarf / PS2 info when needed. Even forward declarations and local partial declarations should use the accurate keyword when known.
- Prefer including the real repo header over introducing a local forward declaration for a project type. If a type already has a header in `src/`, include it instead of redeclaring it locally.
- If a subsystem already has a stub or umbrella owner header and the debug line info points back at that subsystem, fill the owner header instead of keeping a recovered project type declaration in a `.cpp` or spinning up a one-off micro-header just for that type.
- Apply the same owner-header rule to shared enums, globals, callback typedefs, and free functions. If multiple TUs need the declaration, put it in the canonical owner header once and include that header instead of duplicating enum bodies or `extern` blocks across `.cpp`s.
- Preserve original member names, types, order, and proven layout comments. Do not invent `pad`, `unk`, or `field_XXXX` members just to satisfy a guessed size or offset; verify the real members with `find-symbol.py`, GC Dwarf, and PS2 data, and leave a short TODO if a layout detail is still uncertain.
- When recovering a type, start by copying the GC DWARF struct/class body into the canonical owner header. Treat that dump as the source of truth for declaration order too; only apply targeted fixes that are backed by existing repo headers or PS2 data, such as visibility, virtual/function order, duplicate-inline cleanup, or owner-header placement.
- Do not hand-reconstruct recovered layouts from scattered field accesses, guessed semantics, or a "cleaned up" reordering. If you do not have enough evidence to paste the type confidently, stop and gather more DWARF / PS2 info first.
- Preserve the original scope and nesting of recovered declarations too. Keep class-owned enums/types nested when the original did, and move subsystem/global enums into their real owner header instead of flattening or duplicating them near one caller.
- Use the narrowest correct home for recovered declarations: shared project-facing types in headers, TU-private helper structs/classes and allocator metadata in the `.cpp`. Do not dump implementation-only helpers into public headers just because they were convenient to write there.
- Prefer real subsystem or vendor headers over ad-hoc local typedef/prototype blocks. If an external API is shared and the project is missing the proper header, add that header in the correct subtree instead of stashing declarations in an unrelated gameplay file.
- Do not leave repeated `// TODO move`, `// TODO where should this go`, or "I just made this up" markers around declarations. Either move the declaration to its owner now or leave one short targeted TODO above the owner declaration if ownership is still genuinely unresolved.
- Follow DWARF member naming exactly (`mMember` vs `m_member`) instead of normalizing names
- Omit the `this` pointer.
- Use `nullptr` and `override`. If they are missing, you need to include `types.h`.
- Prefer `if (ptr)` / `if (!ptr)` over explicit `nullptr` comparisons. In match-sensitive translation units, if you choose to normalize many of them, do it as one mechanical TU-wide pass and then rebuild / re-check that unit instead of assuming a piecemeal cleanup is free.
- Omit `struct` when declaring variables or parameters, we are not in C land.
- Avoid using `using` directives at all cost. Since the game uses jumbo builds, they leak through files.

## Committing Progress

After each meaningful percentage-point improvement in objdiff match score, commit your changes. Check the current unit match percentage with:

```sh
python tools/decomp-status.py --unit main/Path/To/TU
```

Commit whenever the match percentage increases (e.g. you matched a new function). Use this format for the commit message:

```
n.n[n]%: short description of what was matched or changed
```

Examples:

- `42.1%: match UpdateCamera`
- `78.56%: match PlayerController constructor and destructor`
- `100.0%: full match for zAnim`

Do not batch up multiple percentage milestones into one commit — commit as each improvement lands.

## Matching Philosophy

You should take the Ghidra decompiler output for the initial translation step, get it to compile, make sure that the dwarf of the function matches and only then look for binary matching problems in the assembly. Be aware Ghidra usually gets the order of branches incorrect in if statements (it inverts the logic and the two bodies are swapped), this needs to be fixed to achieve bytematching status.

A function is only done when both objdiff and normalized DWARF are exact. Treat a
100% instruction match with a DWARF mismatch as unfinished work, not a near-complete
result.

The DWARF of your structs does not always compare cleanly in every detail, but the recovery process still starts by copying the dumped layout correctly. Do not freehand-reconstruct a struct from call sites or guessed semantics; paste the DWARF body into the real owner header first, then make only the minimal PS2/header-backed fixes such as visibility, function order, vtable order, or duplicate-inline cleanup.

Never dismiss a diff as "close enough" or "just register allocation." Every mismatched
instruction is a signal that the source doesn't perfectly represent the original. Even
the most stubborn mismatches can be resolved through careful analysis, lateral thinking, and
creative source-level permutations. It may take 10 iterations or 100, but the ultimate goal
is to perfectly match the original code.

Matching can be tricky — be patient and methodical. Try many different ways of writing the same thing. Look for patterns elsewhere in the codebase. Only move on when you've exhausted concrete ideas, not when the match percentage is "high enough."

Make sure that the dwarf also matches 100%, this is usually necessary to achieve byte matching status. If you really really struggle to do it, make sure to at least add a comment to the function that the dwarf is not matching.

Some functions on the bottom of a translation unit are inlines which are emitted by the compiler by the way they are used. Make sure to treat them differently than regular functions.

## Matching Tips

Collected patterns for getting GCC to generate matching code.

### Always **humanize** code

Most importantly: write code that a human would write. The original code was written by humans, so the closer your source is
to natural C++, the more likely it is to match. Don't use any temporaries that aren't found in the dwarf.

GCC's instruction scheduling will often reorder instructions (_especially_ float math).
Attempt to write in the most natural way to start, and only restructure if it does not match.

Use `DECLARE_CONTAINER_TYPE` from `UStandard.h` when you encounter a `_type_` container that's not found in the project yet.

### Respect the dwarf but know its limits

The dwarf (lookup skill using symbols/Dwarf) is your main source of information, but do know that the inlines are duplicated in the DWARF on the bottom of a struct, decide whether to use the top or bottom once based on your intuition or on the PS2 debug info if available.

Virtual table layout is also missing from the dwarf but there on PS2. Be aware that the PS2 version might be missing things because it's an alpha build.

The inline information in the dwarf is incredibly useful. When you encounter one, you should look up its body in the project. If it doesn't exist yet, deduce how the code should look like and add it to the correct header (you can use your address lookup skill or if that doesn't succeed and the inline is a member function, just find the corresponding class in the project).

For recovered structs and classes, treat DWARF as copied source material rather than a loose sketch. Paste the dumped type into the owner header first and keep its declaration/member order unless PS2 or an existing repo header proves a specific correction.

It's very important that you use math inlines from bMath and UMath as shown in the dwarf. UVector inlines use temporaries that the compiler couldn't optimize out. You can see in the dwarf on which stack address they are and deduce final destination they are copied to.

### Store instruction order hints

- GCC likes to reorder store instructions, so try multiple combinations instead of strictly
  using the order from the assembly. When there are lots of store instructions after each other,
  the first one of the source code often ends up being the last in the assembly.
- The developers usually initialized members using initializer lists. This is great because the order
  of stores becomes deterministic that way. However if you put all possible variables into the initializer list
  and the order is wrong, you might have to initialize some or all variables in the function body instead.

### Relocation diffs

- When you have to use a constant that looks like an address, it's possible that the splitter thought it was
  an allocation and it shows up as a diff because the left side has a symbol and the right side has a constant.
  In this case you need to figure out the virtual address of the instruction and block the relocation in config.yml.
- When you need to confirm what lives at a rodata/data address from the original ELF, use
  `python tools/elf_lookup.py 0xADDR` instead of writing a one-off Python script.

### PPC EABI calling convention

On PowerPC EABI (as used by GCC), float and integer parameters use **separate** register
files: floats fill f1–f8 sequentially, integers fill r3–r10 sequentially, independently of
each other. This means inserting/removing a `float` parameter shifts all subsequent float
register assignments but does NOT affect integer register assignments (and vice versa).

### Assembly patterns

- `fmuls fX, fX, fY` or sometimes `fmuls fX, fY, fX` often translates to `v *= fY`
- `xoris r0, r0, 0x8000` in int-to-float conversion => field is `int`, not `uint`.
  Unsigned int-to-float uses a different sequence without `xoris`.

### Branch structure

- Ghidra almost always **inverts** `if`-statement branch logic: the true and false bodies
  are swapped in its output. Fix by inverting the condition and swapping the two code paths.
- A `do { ... } while (i < upperBound)` with a leading `if (upperBound > 0)` guard should
  be written as a plain `for` loop — GCC emits the same code.

### Stack frame and locals

- Frame size (`stwu r1, -0xNNN(r1)`) is determined by the number and types of locals.
  Every local that is NOT in the DWARF is a spurious temporary — remove it.
- Every local that IS in the DWARF must exist in the source, even if you don't use the name.
  Name it exactly as the DWARF shows.
- When objdiff is already exact but a local only differs by lexical scope, try an equivalent
  loop form that keeps the temporary inside the same block as the original DWARF. In practice,
  changing a `for (...; ...; x = next)` into a `while (...) { T *next = ...; ...; x = next; }`
  can fix DWARF-only scope mismatches without changing codegen.

### Slot-pooled delete paths

- If a recovered local/project type participates in `delete` paths or container/list teardown,
  check whether the original type exposed inline `operator new` / `operator delete`. Missing
  slot-pool-backed operators often makes GCC emit `__builtin_delete` instead of the original
  allocator/free path and can also move destructor/delete DWARF ownership out of the TU.
- This applies even when the TU mostly allocates the type manually through `bOMalloc` or a
  pool helper. Restoring the inline operators can still be necessary so `delete` expressions
  and synthesized cleanup paths match the original code and DWARF.

### Virtual vs direct calls

- A `bl` to a specific address = direct (non-virtual) call.
- An `lwz + mtctr + bctrl` sequence = virtual dispatch through vtable.
- If the diff shows a virtual call where you have a direct call (or vice versa), the
  const-qualifier of the method or the object pointer is wrong. Check the DWARF.

### Register allocation hints

- GCC is sensitive to expression decomposition. Splitting a compound expression into
  named sub-expressions often produces different (matching) register allocation.
- Conversely, merging sub-expressions into one can collapse intermediate registers.
- If two adjacent float ops are swapped, try commuting the operands or using a temp.

### Inlines

- Inlines at the bottom of a TU are emitted by usage, not by definition. Do not write
  them as normal function bodies; their presence in source is controlled by `#include`.
- If an inline appears in the DWARF but does not exist in `src/`, deduce its body and add
  it to the correct header (use `line-lookup` skill to find the header file).

---

## Discovered Matching Patterns

This section accumulates session-specific patterns discovered during decompilation.
Generalizable entries are promoted here; TU-specific ones stay in session context only.

**Format for new entries:**

```
### <ShortDescription>
TU: <translation-unit-name> | Function: <FunctionName>
<Description of the source pattern that achieved the match>
```

<!-- Add new entries below this line -->

### ExplicitInlineSpecialMembersForSTLElements

TU: zAttribSys | Function: \_STL::\_Rb_tree<Attrib::TypeDesc, ...>::\_M_insert
If an STL node insertion path refuses to match, check whether the element type is missing explicit inline special members that the original source exposed. Adding the Dwarf-backed `operator new`, `operator delete`, placement `new`, copy constructor, and tiny accessors to `TypeDesc` made the tree node creation/insertion path match exactly.

### WrapperOwnedVecHashMapHelpers

TU: zAttribSys | Function: Class::SetTableBuffer / Class::AddCollection / Database::AddClass
For `VecHashMap`-backed wrapper tables, keep `VecHashMap::Clear()` as a named helper, keep the `CollectionHashMap` constructor and scan helpers on the thin wrapper in `AttribPrivate.h`, and rely on the global placement `operator new` from SN's `<new>` instead of adding a `Node::operator new` member. That combination restored `Class::SetTableBuffer`, `Class::AddCollection`, and `Database::AddClass` to PASS/PASS without changing objdiff.

### FirstFunctionAnchorsKeyedConstructors

TU: zAttribSys | Function: global constructors keyed to Attrib::Class::Class / ClassPrivate::CollectionHashMap::~CollectionHashMap
In a jumbo TU, do not move a newly restored out-of-line wrapper special member above the file's first real top-level function without rechecking symbol order. In `AttribClass.cpp`, restoring `CollectionHashMap::~CollectionHashMap()` was necessary to emit the destructor and keep `Class::Delete` matched, but placing it above `Class::Class` renamed the 44-byte `global constructors keyed to ...` helper. Keeping `Class::Class` first and moving the wrapper dtor below it restored the helper name and the TU's `99.91143%` floor.

### SharedInlineParameterNamesMatter

TU: zAttribSys | Function: Collection::Contains / Collection::NextKey / Collection::GetNode / HashMap::UpdateSearchLength
When a byte-exact mismatch fans out through the same shared inline helper, verify the helper's recovered parameter names before restructuring callers. Renaming `HashMapTablePolicy::WrapIndex`'s first parameter from `k` back to DWARF's `index` cleared four matched-function DWARF mismatches at once without changing objdiff.

### TernaryNodeGetClearsWrapperDWARFNoise

TU: zAttribSys | Function: Class::RemoveCollection / VecHashMap::RemoveIndex
When `VecHashMap::Node::Get()` is written as `return IsValid() ? mPtr : nullptr;` instead of an `if (IsValid()) return mPtr; return nullptr;` ladder, `Class::RemoveCollection` drops the stray `RemoveIndex::result // r26` DWARF mismatch while leaving objdiff and `Database::RemoveClass` unchanged. This is a safe retained cleanup, but it does not touch the remaining `Database::RemoveClass` first-inline `newMaxSearch r31 -> r30` DWARF debt.

### MaxSearchGreaterFixesRemoveCollectionDwarf

TU: zAttribSys | Function: Class::RemoveCollection / VecHashMap::UpdateSearchLength
Writing the second `UpdateSearchLength` search loop as `for (unsigned int searchLen = 1; maxSearch > searchLen; searchLen++)` instead of `searchLen < maxSearch` is a real retained partial win. On `zAttribSys` it makes `Class::RemoveCollection` DWARF-exact, improves that wrapper's objdiff slightly, and nudges the unit text floor from `99.91143%` to `99.9116%`, but it does not help `Database::RemoveClass`.

### SearchFirstHoistIsFalseFriend

TU: zAttribSys | Function: Database::RemoveClass / VecHashMap::UpdateSearchLength
Hoisting the second-loop `searchLen` declaration above `newMaxSearch` (`unsigned int searchLen = 1; unsigned int newMaxSearch = 0; for (; maxSearch > searchLen; searchLen++)`) is a false friend. On the current endgame floor it improves the **second** `UpdateSearchLength` inline in `Database::RemoveClass` and nudges objdiff up to `98.6%`, but it also hoists `searchLen` out of the anonymous loop block in **both** inlined `UpdateSearchLength` bodies and drops normalized DWARF to about `86.6%`, so do not keep or build on that variant.

### BlockScopedSearchLenTradesOneDebtForAnother

TU: zAttribSys | Function: Class::RemoveCollection / Database::RemoveClass / VecHashMap::UpdateSearchLength
The block-scoped variant `unsigned int newMaxSearch; { unsigned int searchLen = 1; newMaxSearch = 0; for (; maxSearch > searchLen; searchLen++) ... }` is another false friend. On the current floor it fixes `Database::RemoveClass`'s **second-inline** `newMaxSearch // r31` placement and raises that wrapper to `98.6%`, but it simultaneously reintroduces the old second-inline `searchLen // r7 -> r6` mismatch in **both** wrappers and drops `Class::RemoveCollection` back to `98.6% / 99.6%`, so it is not a real endgame win.
On that higher-text plateau (`99.91307%`, `42B` remaining), a whole batch of "safe" follow-up nudges stayed completely inert: empty `if (Unk2)` / `if (!Unk2)` hooks inside the late loop, `searchLen = searchLen + 0`, `newMaxSearch = newMaxSearch + 0`, `register` hints on `searchLen` / `newMaxSearch`, typed-pointer `IsValid()`, `Move`'s C-style cast, `Key()`'s zero spellings, local `asm("r7")` register variables, and even an empty `asm volatile("" : "+r"(searchLen))` constraint. If you revisit this plateau, skip those low-risk hooks and look for a genuinely different source shape.

### PreincrementLessThanFixesHeaderButMovesGlobalRegs

TU: zAttribSys | Function: Class::RemoveCollection / Database::RemoveClass / VecHashMap::UpdateSearchLength
Writing the late scan as `for (unsigned int searchLen = 0; ++searchLen < maxSearch; )` is the first condition-only rewrite that makes the second `UpdateSearchLength` loop header itself line up: it fixes the `cmplwi r4, 1` fold, restores the `li r7, 1` / `cmplw r7, r4` shape, and also fixes `Database::RemoveClass`'s `newMaxSearch // r31` placement in that late inline. But it is still a false friend overall: it moves both wrappers onto a broader `r6`/`r7` swap family, drops `Class::RemoveCollection` to `98.6% / 99.6%`, leaves `Database::RemoveClass` at `98.5% / 99.6%`, and changes the remaining mismatches rather than removing them.

### Unk2SelectiveLoopBodyHooksAreRealButUnsafe

TU: zAttribSys | Function: Class::RemoveCollection / Database::RemoveClass / VecHashMap::RemoveIndex
The second empty `for` body in `VecHashMap::RemoveIndex` is a real per-instantiation regalloc lever via the `Unk2` template boolean, but the obvious source shapes are false friends. In this endgame, baseline `if (Unk2) {}` inside that empty body perturbs only `Database::RemoveClass` while leaving `Class::RemoveCollection` at the retained baseline, and baseline `if (!Unk2) {}` perturbs only `Class::RemoveCollection` while leaving `Database::RemoveClass` at the retained baseline. On top of the block-scoped `UpdateSearchLength` near-miss, `if (!Unk2) {}` still perturbs only `Class::RemoveCollection` while leaving `Database::RemoveClass` at the block state. That proves the `Unk2` polarity and the body-CFG selectivity are real, but the tested body forms (`{}`, expression statements, `switch (0)`, `continue`) all regressed, so do not assume an `Unk2`-guarded loop-body tweak is a safe hybrid fix by itself. By contrast, wrapping an empty body as `if (Unk2) { do {} while (0); }` or `if (!Unk2) { do {} while (0); }` was fully inert on the retained floor and is not a useful perturbation.

### HeaderOnlyZAttribSweepsNeedForcedRebuild

TU: zAttribSys | Function: any header-defined inline in `VecHashMap64.h`
Header-only sweeps against `VecHashMap64.h` can silently compare stale code unless you force a rebuild of the rebuilt jumbo object first. The generated `build.ninja` rule for `build/GOWE69/src/Speed/Indep/SourceLists/zAttribSys.o` tracks only `src/Speed/Indep/SourceLists/zAttribSys.cpp`, so after editing included headers you must delete that rebuilt `.o` (and, if needed for tooling, refresh the `.ctx`) before trusting `verify` or `diff` output.

### SizedDeletePathBeatsStrayUnsizedDelete

TU: zAttribSys | Function: Collection::~Collection / Class::Delete / HashMap::PreFlightAdd
If delete-path DWARF keeps collapsing to an empty unsized `operator delete()` helper, re-check whether the type still has a stray unsized `operator delete(void *)` overload that the original source never exposed. In `AttribHashMap.h`, removing the unsized `HashMap::operator delete(void *)` let ProDG reuse the sized delete path again and made `Collection::~Collection` and `Class::Delete` DWARF-exact without moving objdiff.

### RegisterAllocatorTieBreakDeadEnd

TU: zAttribSys | Function: Class::RemoveCollection / Database::RemoveClass
If two near-matching functions differ only because the same inlined helper chain lands long-lived locals in different caller-saved registers, treat it as a likely ProDG/GCC 2.95 register-allocation tie-break, not a normal source mismatch. In `zAttribSys`, the retained `maxSearch > searchLen` rewrite moved the live endgame debt into the **second** `UpdateSearchLength` inline reached from `RemoveIndex`'s empty follow-up loop: `Class::RemoveCollection` is left with the shared late-loop compare/branch shape, while `Database::RemoveClass` also wants `newMaxSearch // r31` where the rebuild keeps it in `r30`. This survived 300+ source experiments plus later focused sweeps of `RemoveIndex` statement order, `FindIndex` local declaration order, wrapper-result liveness, first-call shapes, second-loop declaration/scope variants, second-call expression sugar, `UpdateSearchLength` prelude condition forms, `register` hints on params/locals, per-instantiation `UpdateSearchLength` specializations with identical bodies, and combination searches across individually neutral source toggles. Temporary compiler-side probing was no better: single-flag ProDG toggles around `gcse`/scheduling/CSE/regmove/caller-saves/thread-jumps/delayed-branch either regressed or produced no change, local register variables did not reserve hard registers, and explicit `UpdateSearchLength` specialization was catastrophic even with an identical body. One real exception is the retained `maxSearch > searchLen` loop-header rewrite above: it makes `Class::RemoveCollection` DWARF-exact and bumps the unit floor slightly, but the remaining `RemoveCollection` / `RemoveClass` debt still behaves like the same allocator tie-break family. Once the remaining diff has collapsed to this kind of isolated register swap and DWARF locals/inlining already match, stop attacking each caller separately. Document the functions as `NON_MATCHING`, note the shared inlined root cause, and only revisit the area if you have a genuinely new source shape or stronger compiler evidence than the dead ends above.

### NamedRodataForInlinedAllocatorStrings
TU: zAttribSys | Function: DatabaseExportPolicy::Initialize
When an inlined allocator path must reference a specific rodata symbol, replace a repeated string literal with a named `static const char[]` so the compiler preserves the expected rodata label and relocation pattern.
