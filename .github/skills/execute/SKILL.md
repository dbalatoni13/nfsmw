---
name: execute
description: Workflow for decompiling an entire translation unit end-to-end.
---

# Translation Unit Execution Workflow

Your goal is to decompile a full translation unit: understand the current state,
scaffold any missing classes if needed, then match the unit function by function until
the produced C++ compiles to byte-identical object code against the original retail binary.

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
ninja           # ensure clean build
ninja baseline  # snapshot current match state
```

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
python tools/decomp-workflow.py unit -u main/Path/To/TU
```

Manual equivalent:

```sh
python tools/decomp-diff.py -u main/Path/To/TU
```

This shows all symbols with their match status. Note the total count of missing,
nonmatching, and matching functions.

## Phase 2: Scaffold (if needed)

A jump file contains many files and classes. If the TU depends on a type whose
definition does not yet exist in the project, follow the scaffold workflow in
`.github/skills/scaffold/SKILL.md` to create the needed header/source definitions
before moving on.

## Phase 3: Implement Functions

### 3a. Get the updated function list

After scaffolding, rebuild and re-check the function list.
Use `build-unit.py` to compile to a private temp `.o` so the status check isn't
polluted by another concurrent temp build:

Preferred shortcut:

```sh
python tools/decomp-workflow.py unit -u main/Path/To/TU
```

Manual equivalent:

```sh
ninja                  # full build to update shared state (progress, sha1)
TEMPOBJ=$(python tools/build-unit.py -u main/Path/To/TU)
python tools/decomp-diff.py -u main/Path/To/TU -s nonmatching -t function --base-obj "$TEMPOBJ"
python tools/decomp-diff.py -u main/Path/To/TU -s missing -t function --base-obj "$TEMPOBJ"
```

### 3c. Implement each function sequentially

For each missing or nonmatching function, follow the implementation workflow in
`.github/skills/implement/SKILL.md`.

**Important considerations:**

- **One at a time.** Keep the tree in a coherent state as you work through the list.
- **Balance new vs fixing.** Don't get stuck on one stubborn function — sometimes
  implementing the next function reveals patterns that make the previous one click.
- **Mismatch triage:**
  - `@stringBase0` offset mismatches often resolve as more string literals are added
  - Register swaps and stack layout issues require direct intervention
  - Branch structure mismatches indicate wrong control flow (if/switch/loop)
- **Match percentage is misleading.** The last few percent are often the hardest.
  Treat 95% as unfinished; the goal is 100%.

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

Use `build-unit.py` + `--base-obj` for diff and context commands when you want
results isolated from other concurrent builds of the same TU.

### 3g. Periodic reassessment

After every few functions, re-run the full status check:

```sh
python tools/decomp-diff.py -u main/Path/To/TU
```

Review progress and decide whether to:

- Continue with new functions
- Revisit nonmatching functions with fresh context
- Scaffold adjacent classes if they're blocking progress

## Phase 4: Final Verification

When all functions have been attempted:

```sh
# Full status
python tools/decomp-diff.py -u main/Path/To/TU

# Check for any remaining mismatches
python tools/decomp-diff.py -u main/Path/To/TU -s nonmatching

# Verify no regressions
ninja changes
```

For any remaining nonmatching functions, make one final pass using the implementation
or refiner workflow with all context accumulated during the session.

## Phase 5: Report

Summarize the session:

- **Functions matched** — count and list
- **Functions remaining** — count, list, and notes on what's blocking each
- **Key discoveries** — field types, patterns, or conventions learned
- **Matching tips** — new assembly patterns discovered (note which are generalizable)
- **Adjacent classes touched** — any scaffolding/RE done on related classes
- **Recommendations** — what to tackle next, dependencies on other TUs
