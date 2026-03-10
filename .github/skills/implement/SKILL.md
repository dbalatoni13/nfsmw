---
name: implement
description: Workflow for decompiling and iterating on a function.
---

# Function Implementation Workflow

Your goal is to decompile a specific function: writing C++ source that compiles to byte-identical object code against the original retail binary, verified via `decomp-diff.py`.

## Phase 1: Gather Context

Collect data from **all** of these sources in parallel where possible.

### 1a. decomp-context.py

```sh
python tools/decomp-context.py -u main/Path/To/TU -f FunctionName
```

This provides in one shot:

- Current source code (if any exists)
- objdiff status and instruction-level diff
- Ghidra decompilation of the original

### 1b. Lookup skill

Reference the skill for the usage. It gives info based on the virtual address of the function.

### 1c. Existing source and header

- Read the headers for class layout, member types, field offsets and the source files for existing implementations and includes (both are in `src/.../*.cpp`).
- Check parent class headers for inherited members/methods used in the function

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

Don't add any comments.

Don't use any temporary local variables that don't exist in the dwarf.

Always use the f suffix because the game doesn't use doubles.

Replace if (upperBound > 0) {do {...} while(i < upperBound);} with a simple for loop if it comes up.

Be aware that namespace info might be inconsistent between ghidra and the dwarf, the dwarf often omits it (for example std, or UMath for math inlines or vectors/matrices that don't start with a lowercase b).

Don't be confused by the local variables of inlines seen in the dwarf dump.

The game uses stlport, so you'll often encounter \_STL, but in the code it must be std.

## Phase 4: Build, Diff, and Iterate

### Initial build

```sh
ninja
```

If the build fails, fix compilation errors first.

### Check the diff

```sh
# Quick status
python tools/decomp-diff.py -u main/Path/To/TU --search FunctionName

# Full instruction diff
python tools/decomp-diff.py -u main/Path/To/TU -d FunctionName
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

After writing your code, occasionally run the dwarf dump on the compiled output and then query your output dump with lookup.py to compare your decompiled functions against the originals. Since the address of the function you're working on can keep changing
due to work on other functions, query the unmangled name instead.

```bash
# Dump your compiled unit (see dwarf dump tool)
# Then look up the same function in your output:
python lookup.py --file /tmp/my_unit_dump.nothpp function EPerfectLaunch::~EPerfectLaunch(void)
# Compare with the original:
python lookup.py ./symbols/Dwarf function 0x801DE9AC
```

If you can't figure out the source address using objdiff, find the function in the temporary file manually.

### Iterate

Repeat the build-diff cycle until the diff shows 100% match with no `~` lines.
Every mismatched instruction is a signal — don't settle for "close enough".
Reaching 100% matching status is not enough, also make sure that the dwarf of the function matches the original.

## Phase 5: Report

Summarize:

- Final match status (percentage, instruction count)
- What the function does (brief description)
- Key decisions or tricky patterns used to achieve the match
- If not fully matching, document remaining mismatches and theories
