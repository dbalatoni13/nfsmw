---
name: implement
description: Workflow for decompiling and iterating on a function.
---

# Function Implementation Workflow

Your goal is to decompile a specific function: writing C++ source that compiles to byte-identical object code against the original retail binary, verified via `decomp-diff.py`.

A function is not done until it is exact in both objdiff and normalized DWARF.

Reviewers should not be spending their time rediscovering DWARF mismatches. Before you
report progress, ask for review, hand the function off, or switch to another target, you
must run the per-function verification gate yourself and treat any DWARF failure as your
next task, not as review debt.

## Phase 1: Gather Context

Collect data from **all** of these sources in parallel where possible.

If the function was not already chosen for you, pick it with the ranking wrapper first:

```sh
python tools/decomp-workflow.py next --unit main/Path/To/TU --limit 10
python tools/decomp-workflow.py next --category game --limit 10
```

Prefer low-match, high-remaining targets here. Do not default to near-finished cleanup
functions unless the user explicitly wants a cleanup/refiner pass.

Use the wrapper flow first throughout this skill. Drop to raw `decomp-context.py` or
`decomp-diff.py` only when the wrapper is missing a specific flag or you are debugging.

On a new, suspicious, or recently updated worktree, start with:

```sh
python tools/decomp-workflow.py health --full main/Path/To/TU
```

Add `--timings` when you need to understand why wrapper/tool startup or the shared build
smoke is slow.

### 1a. decomp-context.py

Preferred shortcut:

```sh
python tools/decomp-workflow.py function -u main/Path/To/TU -f FunctionName
python tools/decomp-workflow.py function -u main/Path/To/TU -f FunctionName --brief
python tools/decomp-workflow.py diff -u main/Path/To/TU -d FunctionName
```

If the shared unit object is missing, the wrapper now rebuilds it automatically before
running `next --unit` / `function` / `diff`.

If you only need one Ghidra view, add `--ghidra-version gc` or `--ghidra-version ps2`
to keep the context run faster and shorter.

The wrapper defaults to compact GC DWARF signatures. Add `--lookup-mode full` when you
need the full DWARF body with locals and nested inline info.

Add `--brief` when you want a shorter helper view; it trims suggested commands and
related-source hints while keeping the core source/status/diff context.

Equivalent manual fallback:

```sh
python tools/decomp-context.py -u main/Path/To/TU -f FunctionName
```

This provides in one shot:

- Current source code (if any exists)
- A fallback source excerpt from the GC debug-line-mapped repo file when the metadata
  source path is empty or otherwise unhelpful
- Related source-file hints when the unit metadata source is empty or unhelpful
- Compact GC DWARF signature by default, or full DWARF body with `--lookup-mode full`
- objdiff status and instruction-level diff
- Ghidra decompilation of the original

### 1b. Lookup skill

Reference the skill for the usage. It gives info based on the virtual address of the function.

### 1c. Existing source and header

- Read the headers for class layout, member types, field offsets and the source files for existing implementations and includes (both are in `src/.../*.cpp`).
- Check parent class headers for inherited members/methods used in the function
- Before adding any new declaration, partial declaration, or forward declaration, check whether the type already exists with `python tools/find-symbol.py <TypeName>`.
- If a repo header already exists for the type, include that header instead of introducing a local forward declaration.
- Preserve the original `class` vs `struct` kind. If the existing header is missing or incomplete, verify the type kind from GC Dwarf and PS2 info before writing a local declaration.
- Preserve real member names and field types too. Do not introduce `pad`, `unk`, or `field_XXXX` members as placeholders for guessed layout; verify the member list from GC Dwarf / PS2 data and leave a TODO when something is still uncertain.
- When a type is missing or incomplete, dump the full class/struct body from GC DWARF and paste that as the starting point. Do not reconstruct the layout from one function's field accesses or from guessed semantics.
- Preserve the dumped declaration order as well as the member order. Do not re-sort methods, group fields by guessed meaning, or otherwise "clean up" the layout unless an existing repo header or PS2 evidence proves a specific correction.

### 1e. Assembly reference

If these don't provide enough detail, check the generated assembly. Use the Read tool
to open the relevant `.s` file (prefer this over dumping the whole file):

```
build/GOWE69/asm/Path/To/TU.s
```

Search for the function label (mangled name) to navigate directly to its section.

### 1f. Related functions

If the function calls helpers or accesses types you're unfamiliar with, check their
declarations and any existing implementations for usage patterns.

## Phase 2: Analyze the Original

Before writing any code, understand what the original does by studying the Ghidra decompile
and assembly:

1. **Control flow** — identify switch/if-else structure, loops, early returns
2. **Function calls** — note which methods are called (bl instructions), whether they're
   virtual (indirect through vtable) or direct
3. **Field access patterns** — trace `lfs`/`lwz`/`lbz` at `offset(rN)` to identify which
   class members are read/written
4. **Stack frame** — `stwu r1, -0xNN(r1)` gives the frame size; count locals
5. **Calling convention** — r3=this, r4-r10=int args, f1-f8=float args; struct returns
   use r3 as hidden result pointer
6. **Bitfield operations** — `rlwimi` sets/clears specific bits in flag bytes
7. **Const vs non-const** — `GetFoo` (const) vs `Foo` (non-const) affect which overload
   the linker resolves, visible as different `bl` targets in the diff

## Phase 3: Write the Implementation

### Placement in source file

Utilize the dwarf information that you get from the lookup skill heavily.

For any recovered type you touch while implementing the function, treat the DWARF body as source material to copy, not prose to paraphrase. Start from the dumped layout in the canonical owner header, then make only the minimal verified fixes.

Don't add explanatory comments during implementation unless you need to document a remaining DWARF mismatch.

Don't use any temporary local variables that don't exist in the dwarf.

Always use the f suffix because the game doesn't use doubles.

Replace if (upperBound > 0) {do {...} while(i < upperBound);} with a simple for loop if it comes up.

Be aware that namespace info might be inconsistent between ghidra and the dwarf, the dwarf often omits it (for example std, or UMath for math inlines or vectors/matrices that don't start with a lowercase b).

Don't be confused by the local variables of inlines seen in the dwarf dump.

The game uses stlport, so you'll often encounter \_STL, but in the code it must be std.

## Phase 4: Build, Diff, and Iterate

### Initial build

Rebuild the shared object the normal way before diffing. If you just need the
standard context flow, prefer
`python tools/decomp-workflow.py function -u main/Path/To/TU -f FunctionName`.
For a rebuild plus a standardized diff run, use:

```sh
python tools/decomp-workflow.py build -u main/Path/To/TU
python tools/decomp-workflow.py diff -u main/Path/To/TU -d FunctionName
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

If the build fails, fix compilation errors first.

As soon as you have a compiling draft, run the combined verification gate immediately:

```sh
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

Do this before you spend a long time polishing late instruction mismatches. If `verify`
already shows a DWARF failure, fix that first so you are not polishing code the reviewer
will bounce anyway.

### Check the diff

```sh
# Quick status
python tools/decomp-workflow.py diff -u main/Path/To/TU --search FunctionName --limit 20

# Full instruction diff
python tools/decomp-workflow.py diff -u main/Path/To/TU -d FunctionName
```

### Interpreting the diff

- Left column = original binary, Right column = your decomp
- `~` prefix = mismatched instruction (args in `{}` show the specific mismatch)
- Matching runs are collapsed by default; use `--no-collapse` to see everything
- Branch target differences (relative vs absolute) are cosmetic, not real mismatches

### Fixing mismatches

Refer to the **Matching Tips** section in
AGENTS.md for detailed patterns on resolving instruction mismatches, register allocation
issues, stack frame differences, and symbol naming.

After each meaningful edit/build iteration, run the combined verification gate first:

Preferred shortcut:

```sh
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

This fails unless both the instruction diff and normalized DWARF are exact.

If the verify gate fails because of DWARF, inspect the DWARF block diff directly:

```sh
python tools/decomp-workflow.py dwarf -u main/Path/To/TU -f FunctionName
```

This gives you a normalized DWARF match percentage plus a diff-like report of what still
differs between the original and rebuilt DWARF blocks for that function.

Pay attention to the `Range source ownership` summary there as well. It compares the
debug-line owner files for each DWARF `// Range:` block, which makes it much easier to
spot inlines that are coming from the wrong header or owner file. Exact line-number
agreement is a useful secondary hint, but file ownership is the first thing to check.

Use this as the default loop when the function compiles but `verify` is still failing:

1. Run `verify`.
2. If DWARF fails, run `dwarf`.
3. Fix the structural issue the DWARF diff is pointing at first: missing/extra locals,
   wrong qualifiers or parameter types, wrong inline ownership, wrong helper/header owner,
   or a source shape that outlined something that should be inlined.
4. Rebuild and rerun `verify`.
5. Only return to instruction-by-instruction cleanup once the remaining failures are no
   longer obvious DWARF-compare issues.

Manual fallback:

After writing your code, you can also run the dwarf dump on the compiled output and then query your output dump with lookup.py to compare your decompiled functions against the originals. Since the address of the function you're working on can keep changing
due to work on other functions, query the unmangled name instead.

```bash
# Rebuild the unit, then dump the shared object file's DWARF (ignore dwarf specific errors):
python tools/decomp-workflow.py build -u main/Path/To/TU
build/tools/dtk dwarf dump build/GOWE69/src/Path/To/TU.o -o /tmp/my_unit_dump.nothpp
# Then look up the same function in your output:
python tools/lookup.py --file /tmp/my_unit_dump.nothpp function "EPerfectLaunch::~EPerfectLaunch(void)"
# Compare with the original:
python tools/lookup.py ./symbols/Dwarf function 0x801DE9AC
```

If you can't figure out the source address using objdiff, find the function in the rebuilt object file manually.

### Iterate

Repeat the build-diff cycle until the diff shows 100% match with no `~` lines:

```sh
python tools/decomp-workflow.py build -u main/Path/To/TU
python tools/decomp-workflow.py diff -u main/Path/To/TU -d FunctionName
```

Every mismatched instruction is a signal — don't settle for "close enough".
Reaching 100% instruction matching status is not enough. Stay in the loop until `verify`
passes, which means the DWARF of the function also matches after normalization.

Do not leave a function in a "review-ready" or "good enough for now" state with a known
DWARF failure unless you are explicitly blocked and you document that blocker clearly.

## Phase 5: Report

Summarize:

- Final match status (percentage, instruction count)
- Final DWARF status (exact or remaining mismatch summary)
- What the function does (brief description)
- Key decisions or tricky patterns used to achieve the match
- If not fully matching, document remaining mismatches and theories
