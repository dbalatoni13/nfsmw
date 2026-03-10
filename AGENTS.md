# Need for Speed Most Wanted Decompilation

Matching decompilation of Need for Speed Most Wanted 2005 (GameCube) targeting the USA Release build (`GOWE69`).
The goal is to produce C++ source that compiles to byte-identical and dwarf-identical object code against the
original retail binary using the ProDG GC 3.9.3 compiler. You're completely autonomous, don't stop until you have tried
every single function for a long time.

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

Gathers source code, objdiff diff, Ghidra decompile, and debug map info:

```sh
python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f AcceptScriptMsg
python tools/decomp-context.py -u main/Speed/Indep/SourceLists/zAnim -f FindIOWin --no-source
```

Flags: `--no-source`, `--no-ghidra` to skip sections.

### dtk (decomp-toolkit)

Dump the dwarf of your own implementation of a function:

```sh
dtk dwarf dump build/GOWE69/src/Speed/Indep/SourceLists/UNITNAME.o -o /tmp/UNITNAME_<random_number>.nothpp
```

Demangle a symbol (you probably won't need this):

```sh
dtk demangle 'AcceptScriptMsg__7CEntityF20EScriptObjectMessage9TUniqueIdR13CStateManager'
```

## Code Conventions

This is a **C++98** codebase compiled with ProDG (GCC under the hood). Key rules:

- No `auto`, range-for, `enum class`, lambdas, or any C++11+
- Enum values use prefix: `enum EFoo { kF_Value1, kF_Value2 }` (not `enum class`)
- Use C++ casts (`static_cast< T >(expr)`) instead of C-style casts
- Header guards: `#ifndef _CLASSNAME` / `#define _CLASSNAME` (not `#pragma once`)
- Constructors use initializer list style with leading `, ` on each line, add empty comments at the end of these lines (except the last) to stop clang-format from putting them all on the same line
- Omit the `this` pointer.
- Use `nullptr` and `override`. If they are missing, you need to include `types.h`.
- Omit `struct` when declaring variables or parameters, we are not in C land.

## Matching Philosophy

You should take the Ghidra decompiler output for the initial translation step, get it to compile, make sure that the dwarf of the function matches and only then look for binary matching problems in the assembly. Be aware Ghidra usually gets the order of branches incorrect in if statements (it inverts the logic and the two bodies are swapped), this needs to be fixed to achieve bytematching status.

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

### PPC EABI calling convention

On PowerPC EABI (as used by GCC), float and integer parameters use **separate** register
files: floats fill f1–f8 sequentially, integers fill r3–r10 sequentially, independently of
each other. This means inserting/removing a `float` parameter shifts all subsequent float
register assignments but does NOT affect integer register assignments (and vice versa).

### Assembly patterns

- `fmuls fX, fX, fY` or sometimes `fmuls fX, fY, fX` often translates to `v *= fY`
- `xoris r0, r0, 0x8000` in int-to-float conversion => field is `int`, not `uint`.
  Unsigned int-to-float uses a different sequence without `xoris`.
