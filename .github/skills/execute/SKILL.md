---
name: execute
description: Workflow for decompiling an entire translation unit end-to-end.
---

# Translation Unit Execution Workflow

Your goal is to orchestrate reverse engineering, class scaffolding, and function-by-function
matching to produce C++ source that compiles to byte-identical object code against the
original retail binary.

## Overview

This skill coordinates several agent types:

1. **reverse-engineer** — Update Ghidra with accurate data types for the class
2. **scaffolder** — Create header/source if the class is not yet in the project
3. **implementer** — Match each function one at a time until the TU is complete.

All non-read-only work is done **sequentially** — never spawn multiple writing agents at
the same time, as they will interfere with each other.

**Avoid** doing deep dives into Ghidra or the assembly yourself — instead, rely on the agents to gather and analyze that context. Your context window is precious, so focus on high-level orchestration, monitoring progress, and providing agents the necessary information they need to do their work.

## Phase 0: Establish Baseline

Before any work begins, establish a regression baseline:

```sh
ninja           # ensure clean build
ninja baseline  # snapshot current match state
```

All agents that modify code should check `ninja changes` after modifying shared headers
to verify no regressions were introduced. An empty changeset means no regressions. If
regressions appear, the shared header change must be reverted.

## Phase 1: Reconnaissance

Before spawning any implementation agents, understand the current state of the TU.

### 1a. Identify the file path

Determine the file path (e.g. `src/Speed/Indep/SourceLists/zWorld2`). The game uses unity builds, so such a file includes multiple source files from `src/Speed/Indep/Src/`.

### 1c. Get the full function list

```sh
python tools/decomp-diff.py -u main/Path/To/TU
```

This shows all symbols with their match status. Note the total count of missing,
nonmatching, and matching functions.

## Phase 2: Scaffold (if needed)

A jump file contains many files and classes. Spawn a `scaffolder`
agent to create each class whose definition does not yet exist in the project. The scaffolder will:

- Create the structs in the correct header files with accurate class layouts from the dwarf

Wait for this agent to complete before proceeding.

## Phase 3: Implement Functions

### 3a. Get the updated function list

After scaffolding, rebuild and re-check the function list:

```sh
ninja
python tools/decomp-diff.py -u main/Path/To/TU -s nonmatching -t function
python tools/decomp-diff.py -u main/Path/To/TU -s missing -t function
```

### 3c. Implement each function sequentially

For each missing or nonmatching function, spawn an `implementer` agent. Provide:

- The class name and function name
- The TU path
- Any context from previous iterations (e.g. patterns discovered, field types clarified)
- Accumulated matching tips from previous agents (see below)

**Important considerations:**

- **One at a time.** Never spawn multiple implementer agents concurrently.
- **Balance new vs fixing.** Don't get stuck on one stubborn function — sometimes
  implementing the next function reveals patterns that make the previous one click.
  But don't leave too many functions nonmatching, as agents may copy incorrect patterns.
- **Mismatch triage:**
  - `@stringBase0` offset mismatches often resolve as more string literals are added
  - Register swaps and stack layout issues require direct intervention
  - Branch structure mismatches indicate wrong control flow (if/switch/loop)
- **Match percentage is misleading.** The last few percent are often the hardest.
  Agents may assume a 95% match is "close enough" — remind them that the goal is 100%.

### 3d. Collect and propagate matching tips

Every implementer agent prompt should include:

- All matching tips accumulated so far from previous agents in this session
- A request to **report any new assembly patterns or matching tips** discovered

After each agent completes, evaluate its reported tips:

- **Generalizable patterns** (e.g. `fmuls fX, fX, fY` == `*=`) should be added to
  AGENTS.md's "Assembly patterns" section so all future sessions benefit.
- **TU-specific patterns** (e.g. "this class uses `const char*` cast for bool array
  access") should be kept in the session context and passed to subsequent agents but
  not added to AGENTS.md.

### 3f. Regression checking

Remind agents in their prompts:

> After modifying any shared headers, run `ninja changes` to check for regressions.
> Empty changeset = no regressions. If regressions appear, revert the shared change
> and use a local workaround instead.

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

For any remaining nonmatching functions, make one final pass with the implementer agent,
providing all context accumulated during the session.

## Phase 5: Report

Summarize the session:

- **Functions matched** — count and list
- **Functions remaining** — count, list, and notes on what's blocking each
- **Key discoveries** — field types, patterns, or conventions learned
- **Matching tips** — new assembly patterns discovered (note which are generalizable)
- **Adjacent classes touched** — any scaffolding/RE done on related classes
- **Recommendations** — what to tackle next, dependencies on other TUs

## Agent Prompt Template

When spawning implementer agents, always include these standard instructions:

```
Source file: src/[PathToClass].cpp
Header: include/[PathToClass].hpp
ASM: build/GOWE69/asm/[PathToClass].s

Implement the function [ClassName]::[FunctionName]

**Standard agent instructions:**
- Use the lookup and line-lookup skills for dwarf info.
- After modifying shared headers, run `ninja changes` to check for regressions (empty = good).
- Report any new general assembly patterns or matching tips you discover.

**Matching tips from this session:**
[accumulated tips here]
```
