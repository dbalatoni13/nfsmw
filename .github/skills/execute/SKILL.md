---
name: execute
description: Workflow for decompiling an entire translation unit end-to-end.
---

# Translation Unit Execution Workflow

Your goal is to decompile a full translation unit: understand the current state,
scaffold any missing classes if needed, then match the unit function by function until
the produced C++ compiles to byte-identical object code against the original retail binary.

For each function, "done" means both objdiff and normalized DWARF are exact.

Human review is not a substitute for running `dwarf compare`. Each function should hit
its own `verify` gate before you treat it as ready to hand off, commit, or move past.

## Overview

This workflow combines several smaller workflows:

1. **Scaffold** missing classes or headers when the TU depends on types that do not yet exist (see `.github/skills/scaffold/SKILL.md`).
2. **Implement** each missing or nonmatching function one at a time (see `.github/skills/implement/SKILL.md`).
3. **Refine** stubborn 80–99% functions after the obvious implementation has already been tried (see `.github/skills/refiner/SKILL.md`).

Work through the TU **sequentially** and keep one coherent state in the source tree.

You may use sub-agents for **read-only reconnaissance only**: symbol searches, Ghidra
inspection, dump lookups, line mapping, assembly review, or summarizing the current
state of a TU. Sub-agents must **not** write or edit repository files. All scaffolding,
implementation, refactoring, and other persistent file changes must be done directly by
the main worker after reviewing the read-only findings.

## Phase 0: Establish Baseline

Before any work begins, establish a regression baseline:

```sh
python tools/decomp-workflow.py health --full main/Path/To/TU
ninja           # ensure clean build
ninja baseline  # snapshot current match state
```

Add `--timings` to the `health --full` command when you are investigating slow worktrees
or unexpectedly expensive build/tool startup.

After modifying shared headers, check `ninja changes` to verify no regressions were
introduced. An empty changeset means no regressions. If regressions appear, the shared
header change must be reverted.

## Phase 1: Reconnaissance

Before making changes, understand the current state of the TU.

This phase is a good fit for read-only sub-agents. They can gather function lists, inspect
Ghidra output, trace line mappings, and summarize missing/nonmatching areas, but they must
not edit files or apply code changes.

### 1a. Identify the file path

Determine the file path (e.g. `src/Speed/Indep/SourceLists/zWorld2`). The game uses unity builds, so such a file includes multiple source files from `src/Speed/Indep/Src/`.

### 1b. Get the full function list

Preferred shortcut:

```sh
python tools/decomp-workflow.py next --unit main/Path/To/TU --limit 10
python tools/decomp-workflow.py unit -u main/Path/To/TU --limit 20
```

Use `next` first when you want the wrapper to rank the most useful targets instead of
following raw objdiff order. `--strategy balanced` is the default and is usually the best
starting point because it now favors large remaining gains and penalizes near-finished
cleanup work. Use `--strategy impact` when you only care about the biggest unmatched-byte
wins. Use `--strategy quick-wins` when you want lower-match functions where the first big
chunk of progress is likely to come faster than late cleanup.

Stay in the wrapper flow by default. Only drop to raw `decomp-status.py` / `decomp-diff.py`
when you need an option the wrapper does not expose yet.

If the shared unit object is missing, the wrapper now rebuilds it automatically before
running `next --unit` / `unit`.

If you need the raw tools instead of the wrapper, run `decomp-status.py` and
`decomp-diff.py` directly against the shared build output as a fallback, not the default.

## Phase 2: Scaffold (if needed)

A jumbo file contains many files and classes. If the TU depends on a type whose
definition does not yet exist in the project, follow the scaffold workflow in
`.github/skills/scaffold/SKILL.md` to create the needed header/source definitions
before moving on.

Treat recovered types here as copied reference data, not as hand-designed headers. Copy
the GC DWARF type body into the canonical owner header first and preserve its declaration
order unless PS2 or existing repo-header evidence proves a specific correction.

## Phase 3: Implement Functions

### 3a. Get the updated function list

After scaffolding, rebuild and re-check the function list:

Preferred shortcut:

```sh
python tools/decomp-workflow.py unit -u main/Path/To/TU
```

If you need the raw tools, rebuild normally and then run `decomp-diff.py`
directly on the unit only as a fallback.

### 3c. Implement each function sequentially

For each missing or nonmatching function, follow the implementation workflow in
`.github/skills/implement/SKILL.md`.

**Important considerations:**

- **One at a time.** Keep the tree in a coherent state as you work through the list.
- **Balance new vs fixing.** Don't get stuck on one stubborn function — sometimes
  implementing the next function reveals patterns that make the previous one click.
- **Recovered types are not freeform.** If a function forces you to add or fix a type,
  copy the DWARF layout into the owner header first. Do not sketch structs/classes from
  use sites or reorder declarations just to make the header look nicer.
- **Mismatch triage:**
  - `@stringBase0` offset mismatches often resolve as more string literals are added
    - If you need to inspect the original string or rodata at a virtual address, use `python tools/elf_lookup.py 0xADDR`
  - Register swaps and stack layout issues require direct intervention
  - Branch structure mismatches indicate wrong control flow (if/switch/loop)
- **Match percentage is misleading.** The last few percent are often the hardest.
  Treat 95% as unfinished; the goal is 100%.
- **DWARF is equally mandatory.** A 100% objdiff function with a DWARF mismatch is still unfinished.

### 3d. Collect and propagate matching tips

Keep notes on useful patterns you discover while working through the TU.
After each useful result, evaluate whether the pattern is generalizable:

- **Generalizable patterns** (e.g. `fmuls fX, fX, fY` == `*=`) should be added to
  AGENTS.md's "Assembly patterns" section so all future sessions benefit.
- **TU-specific patterns** (e.g. "this class uses `const char*` cast for bool array
  access") should be kept in the session context and applied to subsequent functions but
  not added to AGENTS.md.

### 3f. Regression checking

After modifying any shared headers, run `ninja changes` to check for regressions.
Empty changeset = no regressions. If regressions appear, revert the shared change
and use a local workaround instead.

Use `python tools/decomp-workflow.py function ...` or
`python tools/decomp-workflow.py diff ...` when you want a shorter, wrapper-first
view for one function.

After each function-level edit pass, run:

```sh
python tools/decomp-workflow.py verify -u main/Path/To/TU -f FunctionName
```

If it fails, follow up with `decomp-workflow.py diff` and `decomp-workflow.py dwarf`
until both checks pass.

Do not queue up several "probably done" functions and leave the DWARF check for later.
Close the `verify` gate per function before moving on whenever feasible; otherwise the
reviewer ends up doing avoidable DWARF triage.

### 3g. Periodic reassessment

After every few functions, re-run the full status check:

```sh
python tools/decomp-workflow.py unit -u main/Path/To/TU
python tools/decomp-workflow.py next --unit main/Path/To/TU --limit 10
```

Review progress and decide whether to:

- Continue with new functions
- Revisit nonmatching functions with fresh context
- Scaffold adjacent classes if they're blocking progress

## Phase 4: Final Verification

When all functions have been attempted:

```sh
# Wrapper-first unit summary
python tools/decomp-workflow.py unit -u main/Path/To/TU

# Focused remaining mismatches
python tools/decomp-workflow.py diff -u main/Path/To/TU -s nonmatching -t function

# Verify no regressions
ninja changes
```

If you need a raw full-symbol dump beyond that, use `decomp-diff.py` only as a final
fallback.

For any remaining nonmatching functions, make one final pass using the implementation
or refiner workflow with all context accumulated during the session.

Do not report a function as complete unless its per-function `verify` check also passes.
Do not hand a function to review as "done except maybe DWARF" — either resolve the DWARF
failure yourself or explicitly call out the blocker and why it remains.

## Phase 5: Report

Summarize the session:

- **Functions matched** — count and list
- **Functions remaining** — count, list, and notes on what's blocking each
- **Key discoveries** — field types, patterns, or conventions learned
- **Matching tips** — new assembly patterns discovered (note which are generalizable)
- **Adjacent classes touched** — any scaffolding/RE done on related classes
- **Recommendations** — what to tackle next, dependencies on other TUs
