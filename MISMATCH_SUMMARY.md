# WTriggerManager Function Mismatches - zWorld2 Translation Unit

## 1. ProcessRB (95.3% match, 1332B, 337 instructions, 62B unmatched)

### Key Register Allocation Mismatches:
- **Line 190ac**: `mr {r27}, r4` (LEFT) vs `mr {r28}, r4` (RIGHT) - Register r27/r28 swap
- **Lines 190c0, 190cc, 19100**: References to `{r27}` vs `{r28}` consistently swapped
- **Line 191f8**: `lwzx r0, r29, {r28}` vs `lwzx r0, r29, {r27}` - Register swap continues
- **Line 19160**: `lwz {r28}, 8(r1)` vs `lwz {r27}, 8(r1)` - Different register loaded
- **Line 19184**: `cmpw {r28}, r3` vs `cmpw {r27}, r3` - Register swap in comparison
- **Line 1919c**: `cmpwi cr4, {r28}, 0` vs `cmpwi cr4, {r27}, 0` - Register swap

### Register Allocation Issue with r0, r8, r10:
- **Line 192ac**: `li {r8}, 1` (LEFT) vs `li {r10}, 1` (RIGHT)
- **Line 192b4**: `stw {r8}, 0x18(r1)` vs `stw {r10}, 0x18(r1)`
- **Line 192d8**: `stw {r8}, 0x28(r1)` vs `stw {r10}, 0x28(r1)`
- **Line 192ec-192f0**: 
  - LEFT: `lwz {r0}, 0(r11)` then `cmpwi {r0}, 0`
  - RIGHT: `lwz {r9}, 0(r11)` then `cmpwi {r9}, 0`
- **Line 19300-19308**: Complex register reuse issue with r10, r11, r8, r9
  - LEFT: `lwz {r10}, 4({r11})` then `lwz r0, 0({r10})` then `stw {r8}, 0x28(r1)`
  - RIGHT: `lwz {r9}, 4({r9})` then `lwz r0, 0({r9})` then `stw {r10}, 0x28(r1)`

### Missing Instruction (LEFT):
- **Line 192d8**: Extra `lhz r9, 0xe(r11)` in RIGHT that doesn't appear until line 192e0 in LEFT
- **Line 192f8-192fc**: LEFT has extra instructions that get moved/removed in RIGHT:
  ```
  LEFT:  lwz r9, 0x1c(r1)
         lwz r11, 0(r9)
  RIGHT: (these are moved earlier)
  ```

### Floating Point Register Mismatch:
- **Line 195e8**: `fmr {f30}, f1` vs `fmr {f31}, f1` - Wrong FP register
- **Line 19630**: Extra `fmr f31, f1` in LEFT missing in RIGHT

### Complex Conditional Logic (Lines 194c0-194a0):
Pattern shows register reordering in OR operations and conditional branches:
- **Line 19470**: `or {r9}, {r9}, {r0}` vs `or {r0}, {r0}, {r9}`
- **Line 19474**: `or {r11}, {r11}, {r9}` vs `or {r0}, {r0}, {r11}`
- **Line 19478**: `lwz {r0}, 0(r25)` vs `lwz {r9}, 0(r25)`

---

## 2. ProcessSRB (95.2% match, 1424B, 361 instructions, 68B unmatched)

### Floating Point Register Mismatches (Similar to ProcessRB):
- **Line 195e8**: `fmr {f30}, f1` vs `fmr {f31}, f1`
- **Line 19630**: Extra `fmr f31, f1` in LEFT
- **Lines 197b0-197f8**: `fmr f1, {f31}` vs `fmr f1, {f30}`

### Register Allocation Issues (Similar pattern to ProcessRB):
- **Lines 197f8-198****: Register r8 vs r10 allocation issue
- **Line 19824**: `stw {r8}, 0x28(r1)` vs `stw {r10}, 0x28(r1)`
- **Line 19828**: Extra `lhz r9, 0xe(r11)` in RIGHT only

### Complex Register Moves:
- **Lines 1983c-1984c**: Mismatched register loading in multiple stages
- **Line 19838**: `lwz {r0}, 0(r11)` vs `lwz {r9}, 0(r11)`
- **Line 19844-19850**: Multiple register swap cascades

### Missing Instructions:
- **Lines 19844-19848**: LEFT has two extra instructions that are missing in RIGHT

### Conditional Logic Pattern (Similar to ProcessRB):
- **Lines 199bc-199c8**: Register reordering in OR operations
- **Line 199c8**: `andi. {r9}, {r11}, 1` vs `andi. {r11}, {r0}, 1`

---

## 3. WorldEffectConn::Update (95.1% match, 2128B, 540 instructions, 104B unmatched)

### Critical Missing Instructions:
- **Line 1d7c8**: `bl VU0_sqrt(float)` missing in RIGHT
- **Line 1d87c-1d8b4**: Multiple missing stack operations and FP operations in LEFT

### Floating Point Register Cascade (Lines 1d854-1d924):
Extensive register reallocation across FP registers:
- **Line 1d854**: `fmuls {f9}, f0, f0` vs `fmuls {f8}, f0, f0`
- **Line 1d85c**: `fmuls {f7}, f12, f12` vs `fmuls {f9}, f12, f12`
- **Line 1d860**: `lfs {f2}, {0x5c}(r1)` vs `lfs {f11}, {0x58}(r1)`
- **Line 1d86c**: `fmuls {f3}, f0, f6` vs `fmuls {f2}, f0, f6`
- **Line 1d870**: `fmuls {f4}, f12, f6` vs `fmuls {f3}, f12, f6`

### Stack Memory Misalignment:
- **Line 1d894**: `stfs {f8}, {0xb8}(r1)` vs `stfs {f5}, {0xc0}(r1)` - Different stack offsets
- **Line 1d8a0**: `fadds {f2}, {f7}, f10` vs `fadds {f5}, {f9}, f10` - FP register + stack offset mismatch
- **Line 1da48-1da60**: Stack manipulation involving r30 and r0

### Register Allocation Issues:
- **Line 1d994-1d998**: `lwz {r9}, 0x3c(r29)` vs `lwz {r3}, 0x3c(r29)` and `fmr f1, {f26}` vs `fmr f1, {f27}`
- **Line 1da20-1da40**: Extended FP register load sequence with multiple mismatches
- **Lines 1dab8-1dabc**: Integer register loads/stores misaligned

---

## 4. CheckCollideRB (94.6% match, 968B, ~244 instructions)

### Critical Branch Instruction Mismatch:
- **Line 19d24**: `blt {0x19ef4}` vs `bso {...}` - DIFFERENT BRANCH CONDITIONS
  - LEFT uses `blt` (branch if less than)
  - RIGHT uses `bso` (branch if summary overflow set) with `cror un, eq, gt` setup

### Missing Instructions:
- **Line 19d18**: `li r3, 1` in LEFT missing in RIGHT
- **Lines 19d40-19d58**: Missing initialization/allocation sequence in LEFT

### Register Shuffling:
- **Line 19d40**: `mr {r4}, {r28}` vs `mr {r26}, {r29}` - Completely different registers

---

## 5. CheckCollideSRB (94.5% match, 1024B, ~256 instructions)

### Parameter Register Swap:
- **Line 19f30**: `mr {r27}, r4` vs `mr {r31}, r4`
- **Line 19f34**: `mr {r31}, r5` vs `mr {r27}, r5` 
- All subsequent parameter uses swap between r27/r31

### Stack Memory Organization Issues:
- **Lines 1a054-1a068**: Stack setup differs:
  - LEFT: `addi r28, r1, 0x38` then removed `crclr cr1eq`
  - RIGHT: Multiple stack address assignments reorganized
- **Line 1a058**: `addi {r30}, r1, {0x48}` vs `addi {r28}, r1, {0x38}`
- **Line 1a07c**: `addi {r29}, r1, 0x88` vs `addi {r30}, r1, 0x88` - Stack pointer register swap

### Cascading Register Issues Through Function Body:
All subsequent FP loads from stack now reference different registers due to parameter swap:
- Multiple `lfs` instructions have register mismatches (f0, f2-f13 range)
- Lines 1a0c4-1a0c8, 1a0f0-1a0f8, 1a140-1a194 all show register cascade

### Comparison Register Mismatch:
- **Line 1a214**: `cmpwi {r0}, 2` vs `cmpwi {r8}, 2` - Wrong register used in compare

---

## Pattern Analysis

### Common Patterns Across Functions:

1. **Register Allocation Cascade**: When one register is allocated differently early on (e.g., r27 vs r28), it cascades through the entire function affecting dozens of instructions.

2. **Floating Point Register Pressure**: Particularly in ProcessRB/ProcessSRB and WorldEffectConn::Update, the compiler's FP register allocation differs significantly (f0-f13 range).

3. **Stack Frame Layout**: Different stack pointer base addresses (0x38 vs 0x48, 0x88, etc.) suggest the compiler organized temporary storage differently.

4. **Parameter Passing**: CheckCollideSRB shows parameter registers swapped (r27 ↔ r31) affecting entire function.

5. **Conditional Logic**: Branch instructions sometimes use different condition codes (blt vs bso, beq vs bne), suggesting different optimization strategies.

6. **Missing/Reordered Instructions**: Some setup code moves around (VU0_sqrt call, lhz operations) indicating different scheduling/optimization phases.

### Root Causes Likely:
- Different compiler versions or flags
- Inlining/optimization decisions differing
- Register pressure forcing different allocation strategies
- Different FP unit scheduling
- Possible differences in calling convention handling
