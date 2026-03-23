---
name: refiner
description: Workflow for resolving stubborn instruction mismatches in a function that the standard implementer has already attempted. Use when a function is stuck at 80–99% match and the obvious approaches have already been tried. Assumes the function compiles cleanly, a diff exists, and the implementer has already made one or more passes.
---

# Refiner Workflow

Your goal is to close the remaining instruction mismatches in a function that is partially
matching. The implementer has already made a pass. You must **not** repeat the same
approaches that were tried before — instead, apply systematic lateral analysis.

## Starting assumptions

- The function already compiles.
- A diff is available (`decomp-diff.py -u <TU> -d <func>`).
- The "obvious" translation from Ghidra has been attempted.
- You have been given the current source code and the diff.
- You have already run the per-function `verify` gate and know whether the remaining work
  is still structural DWARF cleanup or true late-stage instruction cleanup.

Refiner is not the place to dump unresolved DWARF debt on a reviewer. If `verify` or
`dwarf` is still showing obvious structural mismatches (missing locals, wrong types,
wrong inline ownership, wrong helper/header owner), fix those first or drop back to the
implementer workflow before doing late instruction polish.

## Phase 1: Read the full diff without collapsing

Before you start a refiner pass, confirm the gate status:

```sh
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

If the combined gate is failing for reasons that are still clearly visible in the DWARF
diff, address those first instead of treating them as reviewer follow-up.

Preferred shortcut:

```sh
python tools/decomp-workflow.py diff -u main/Path/To/TU -d FunctionName --no-collapse
```

If the shared unit object is missing, the wrapper now rebuilds it automatically before
running `diff`.

Stay in the wrapper flow for refiner passes unless you hit a wrapper limitation and need a
backend-only option.

If you need the raw backend form instead of the wrapper, rebuild the unit and then run:

```sh
python tools/decomp-workflow.py build -u main/Path/To/TU
python tools/decomp-diff.py -u main/Path/To/TU -d FunctionName --no-collapse
```

Read every instruction pair. Categorize each mismatch:

| Category | Symptom | Strategy |
|---|---|---|
| **Branch inversion** | Entire blocks swapped, branch condition inverted | Invert the `if` condition and swap the two bodies |
| **Register pressure** | Same ops, different register allocation | Reorder source expressions; introduce/remove a named temp |
| **Stack frame size** | Wrong frame size in prologue | Count locals in DWARF; remove temporaries not in DWARF |
| **Float vs int sequence** | `xoris` present → field is `int`; absent → `uint` | Check field type in DWARF; change cast |
| **`fmuls` operand order** | `fmuls fX, fX, fY` or `fmuls fX, fY, fX` | Try `v *= fY` vs `fY * v` explicitly |
| **Relocation offset** | `@stringBase0` or data offset differs | More string literals will shift this; add them in order. Use `python tools/elf_lookup.py 0xADDR` when you need to confirm the original string/rodata at a virtual address |
| **Virtual vs direct call** | `bl` vs indirect through vtable | Check const-qualifier; use `GetFoo()` vs `Foo()` |
| **Inline vs outlined** | Extra call to helper vs inlined sequence | Force inline by rewriting the expression without calling the helper |
| **Loop structure** | Guarded `do/while` from Ghidra or mismatched loop branches | Rewrite to the natural source form suggested by the control flow; in particular, a guarded `do/while` often needs to become a plain `for` loop |

## Phase 2: Systematic permutation strategies

Try these **in order**, rebuilding and diffing after each:

### 2a. Temporaries

Remove any named temporary that is **not** in the DWARF, or add one that **is**.
Temporaries affect register allocation significantly.

```bash
python tools/lookup.py ./symbols/Dwarf function 0xADDR   # check for temps in DWARF
```

### 2b. Expression decomposition

Split or merge compound expressions. GCC often schedules arithmetic differently when
sub-expressions are named:

```cpp
// Try decomposed:
float a = foo->x * bar;
float b = a + baz->y;
result = b;

// vs composed:
result = foo->x * bar + baz->y;
```

### 2c. Const-correctness

Check every method call in the diff against the DWARF. A const method resolves to a
different symbol than its non-const overload. Even one wrong const qualifier causes
a `bl` mismatch.

```bash
python tools/lookup.py ./symbols/Dwarf struct ClassName
```

### 2d. Inline math

The game uses `UMath` and `bMath` inlines. If the diff shows a hand-rolled float
sequence, look up whether an inline covers the same operation:

```bash
python tools/lookup.py ./symbols/Dwarf struct UMath
python tools/lookup.py ./symbols/Dwarf struct bMath
```

Replace hand-rolled sequences with the correct inline call.

### 2e. Constructor initialization placement

Only do this for constructors. Compare which members are initialized in the
initializer list versus the function body, and in what order. Initializer-list use
often stabilizes store order, but forcing every member into the initializer list can
also make the match worse.

### 2f. Cast type

`static_cast<int>` vs `static_cast<unsigned int>` produces different assembly
sequences on PPC (see `xoris` pattern in AGENTS.md). Check all casts.

## Phase 3: DWARF verification

After any instruction match, verify the DWARF also matches. The function is not done
until both objdiff and normalized DWARF are exact.

Preferred shortcut:

```bash
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

If the combined gate fails because of DWARF, inspect the DWARF diff directly with:

```bash
python tools/decomp-workflow.py dwarf -u main/Path/To/TU -f FunctionName
```

Manual fallback:

Use the rebuilt shared object from Phase 1 (or rebuild again if you've changed the source):

```bash
# Rebuild the unit, then dump its DWARF (ignore dwarf specific errors)
python tools/decomp-workflow.py build -u main/Path/To/TU
build/tools/dtk dwarf dump build/GOWE69/src/Path/To/TU.o -o /tmp/refiner_<func>_check.nothpp

# Compare your function's DWARF against the original
python tools/lookup.py --file /tmp/refiner_<func>_check.nothpp function "ClassName::FunctionName(void)"
python tools/lookup.py ./symbols/Dwarf function 0xADDR
```

DWARF mismatches to watch for:

- Extra or missing local variables (temporaries in DWARF = they must exist in source)
- Wrong parameter types or qualifiers
- Wrong return type
- Missing inlined function records (means an inline call was outlined)

If these mismatches are still present, you are not in pure refiner territory yet. Resolve
them before you ask a reviewer to spend time on the function.

## Phase 4: Report

Summarize:

- Final match % and instruction count
- What was blocking the match (the root cause category from Phase 1)
- The specific source change that resolved it
- Any new generalizable assembly pattern discovered (add to AGENTS.md if so)
- DWARF match status and whether `verify` passes
- If still not matching: the exact diff lines that remain and your best theory
