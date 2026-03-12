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
```

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

Sub-agents are allowed only for **read-only exploration** tasks such as:

- searching the codebase for symbols, call sites, or include relationships
- inspecting decomp output, assembly, DWARF, PS2 dumps, or line mappings
- gathering context from Ghidra, `tools/decomp-workflow.py`, `lookup.py`, `decomp-diff.py`, or similar tools
- summarizing findings that help the main worker decide what to change

Sub-agents must **not** write or edit code files, headers, configs, or other repository files.
All persistent file changes, decomp implementations, scaffolding, and follow-up fixes must be
done by the main worker after reviewing the read-only findings.

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

### decomp-diff.py — Diff & symbol overview

Overview mode lists all symbols in a translation unit with match status:

```sh
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -s nonmatching -t function
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim -s missing -t function
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAnim --search RemoveIOWin
```

Filters: `-t function,object` (type), `-s missing|matching|nonmatching|extra` (status),
`--section .text`, `--search <pattern>` (fuzzy name match).

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
python tools/decomp-workflow.py health --smoke-build main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py health --smoke-dtk main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py next --category game --limit 10
python tools/decomp-workflow.py next --unit main/Speed/Indep/SourceLists/zAnim --strategy quick-wins --limit 5
python tools/decomp-workflow.py build -u main/Speed/Indep/SourceLists/zAnim
python tools/decomp-workflow.py diff -u main/Speed/Indep/SourceLists/zAnim -d FindIOWin
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --brief
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --ghidra-version gc
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --lookup-mode full
python tools/decomp-workflow.py unit -u main/Speed/Indep/SourceLists/zAnim --search FindIOWin --limit 20
```

The wrapper keeps the existing tools as the source of truth. It is intended to reduce
repeated command chaining and to standardize routine worktree preflight checks for agents.
`next --unit`, `function`, `unit`, and `diff` now also auto-build the unit's shared `.o`
once when that output is missing, so wrapper-first inspection works more often on
half-prepared worktrees.

When you do not already have a specific target in mind, start with `next` or `unit`
instead of picking functions in raw objdiff order. `next` is the fastest way to answer
"what should I work on now?":

- `--strategy balanced` favors high-impact functions while de-prioritizing obvious
  init/setup sinkholes and preferring targets with usable source context.
- `--strategy impact` is the blunt "largest unmatched byte loss first" view.
- `--strategy quick-wins` favors mature units and existing implementations where agents
  are more likely to land progress quickly.

`function` is the preferred context-gathering entrypoint: it bundles source excerpt,
objdiff status/diff, compact GC DWARF function lookup, and Ghidra output in one run.
If the unit metadata points at an empty or otherwise useless source-list file, it also
falls back to the GC debug-line-mapped repo source file when that file exists and has
real content.
Add `--brief` when you want to keep the helper sections compact; it trims suggested
commands and related-source hints without hiding the core status/diff/source data.

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

Dump the dwarf of your own implementation of a function after rebuilding the unit normally:

```sh
python tools/decomp-workflow.py build -u main/Speed/Indep/SourceLists/zAnim
dtk dwarf dump build/GOWE69/src/Speed/Indep/SourceLists/zAnim.o -o /tmp/zAnim_check.nothpp
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
- Use C++ casts (`static_cast< T >(expr)`) instead of C-style casts
- Header guards: `#ifndef _CLASSNAME` / `#define _CLASSNAME` (not `#pragma once`)
- Constructors use initializer list style with leading `, ` on each line, add empty comments at the end of these lines (except the last) to stop clang-format from putting them all on the same line
- Omit the `this` pointer.
- Use `nullptr` and `override`. If they are missing, you need to include `types.h`.
- Omit `struct` when declaring variables or parameters, we are not in C land.
- Avoid using `using namespace` at all cost. Since the game uses jumbo builds, they leak through files.

## Committing Progress

After each meaningful percentage-point improvement in objdiff match score, commit your changes. Check the current unit match percentage with:

```sh
python tools/decomp-status.py --unit main/Path/To/TU
```

Commit whenever the match percentage increases (e.g. you matched a new function). Use this format for the commit message:

```
n.n%: short description of what was matched or changed
```

Examples:

- `42.1%: match UpdateCamera`
- `78.5%: match PlayerController constructor and destructor`
- `100.0%: full match for zAnim`

Do not batch up multiple percentage milestones into one commit — commit as each improvement lands.

## Parallel Sub-Agent Matching

When working on a translation unit with multiple non-matching functions, use sub-agents selectively for **read-only exploration** around individual functions. Each sub-agent should focus on **exactly one function** — do not assign a sub-agent more than one function at a time.

**Limit: never run more than 5 sub-agents concurrently.** Spawning too many at once causes resource contention and makes it harder to reason about progress.

Guidelines:

- Prefer solving difficult matching work in the main worker. Use sub-agents to inspect one function's context, diff, DWARF, or related call paths without editing files.
- Spawn a sub-agent per function only when the functions are independent (no shared edits to the same source lines).
- Sub-agents stay read-only. Let them inspect existing diff/context output rather than compiling or rebuilding.
- Do not sit idle waiting for sub-agents to finish. Continue with other independent investigation while they run.
- After a useful result lands and you make a real improvement, check the updated match percentage and commit if it improved.

## Matching Philosophy

You should take the Ghidra decompiler output for the initial translation step, get it to compile, make sure that the dwarf of the function matches and only then look for binary matching problems in the assembly. Be aware Ghidra usually gets the order of branches incorrect in if statements (it inverts the logic and the two bodies are swapped), this needs to be fixed to achieve bytematching status.

You may use sub-agents to gather read-only context during this process, but they must not
edit files. Treat their output as analysis input for the main worker, not as a path to
delegate source changes.

The dwarf of your structs doesn't have to neccessarily match the original due to various reasons, just make sure that you copied everything correctly.

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

### RegisterAllocatorTieBreakDeadEnd

TU: zAttribSys | Function: Class::RemoveCollection / Database::RemoveClass
If two near-matching functions differ only because the same inlined helper chain lands `mTableSize` in `r6` in the original but `r7` in the rebuild, treat it as a likely ProDG/GCC 2.95 register-allocation tie-break, not a normal source mismatch. In `zAttribSys`, `VecHashMap::FindIndex` inlined through `Remove -> RemoveIndex -> UpdateSearchLength` produced a stable `lwz r6, 4(r3)` vs `lwz r7, 4(r3)` split, which then propagated into later `UpdateSearchLength` control-flow differences. This survived 300+ source experiments: loop-form changes, adding/removing temporaries, splitting/merging expressions, helper inline/outline changes, declaration-order tweaks, member type changes, access-control changes, template method reorderings, and inline vs out-of-line ctor/dtor placement. Once the diff has collapsed to this kind of isolated register swap and DWARF locals/inlining already match, stop attacking each caller separately. Document the functions as `NON_MATCHING`, note the shared inlined root cause, and only consider flag permutation or compiler-level investigation as a last resort.
