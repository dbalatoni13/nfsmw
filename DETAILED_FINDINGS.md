# DETAILED TECHNICAL FINDINGS - zCamera Structural Analysis

## KEY FINDINGS BY PATTERN

### Pattern 1: Pure Register Allocation (Easiest Fixes)

**ReplayPowerSlideScore (68.0%, 38B unmatched)**
- **Root Cause**: Different floating-point register usage for temporaries
  - Left:  f1, f13, f12, f0 assignments
  - Right: f10, f0, f13, f12 assignments (different order)
- **Key Observation**: ALL arithmetic operations are IDENTICAL
  - `fmsubs f0, f0, f11, f12` vs `fmsubs f0, f0, f11, f13` (just register names)
  - `fmuls f13, f0, f13` vs `fmuls f0, f0, f12` (semantically equivalent reordering)
- **Branch Pattern Change**: `bso` (branch if summary overflow) → `bgt` + `fmr f1, f10`
  - This achieves the same result but uses different registers
- **Fix Strategy**: This is likely a compiler preference for register reuse
  - Try: Declaring temporaries in different order might help compiler preference
  - Or: Using explicit register constraints with `__attribute__((register(r3)))`

**ReplayJumpScore (75.6%, 46B unmatched)**
- **Root Cause**: Stack frame shrinking + one register saved removed
  - Left:  Stack -0x20, saves r30
  - Right: Stack -0x18, no r30 saved (r30 truly not needed)
  - r30 used for loading constant in left, but directly loaded in right
- **Key Pattern**: `stmw r30, 0x10(r1)` removed in optimized version
  - This means one fewer register to preserve
  - Entire function can fit in fewer callee-saved registers
- **Fix Strategy**: 
  - Remove unnecessary register usage
  - Let compiler optimize register pressure

### Pattern 2: Stack Frame Optimization (Medium Difficulty)

**FixWorldHeight (72.9%, 62B unmatched)**
- **Left Stack Setup**: 
  ```
  stwu r1, {-0x30}(r1)    # Frame size 0x30
  stmw {r30}, {0x28}(r1)  # Save from 0x28
  stw r0, {0x34}(r1)      # Stack too small for this offset!
  ```
- **Right Stack Setup**:
  ```
  stwu r1, {-0x38}(r1)    # Frame size 0x38 (8 bytes more)
  stmw {r29}, {0x2c}(r1)  # Save from 0x2c (more conservative)
  stw r0, {0x3c}(r1)      # LR save at end
  ```
- **Key Difference**: Compiler made different frame size choice (0x30 vs 0x38)
  - Left tries to be minimal but has register pressure issues
  - Right uses more space but cleaner register allocation
- **Register Pressure**: r30→r29 swap due to different saved set
- **Fix Impact**: ~62B - mostly instructions, register assignment

**GetGroundElevation (71.1%, 57B unmatched)**
- **Similar pattern** to FixWorldHeight
- Setup phase has instructions reordered but achieve same initialization
- Stack frame differences: tempt vector setups in different stack locations

### Pattern 3: Complex Loop Scheduling (Hard)

**ChooseGoodSceneCameraTrackIndex (70.8%, 233B unmatched)**
- **Initialization Loop Issue**:
  - Left: 13 sequential `stfs f31, offset(r1)` instructions (redundant init)
  - Right: Compiler eliminated loop-invariant stores
  - Savings: ~52B from removing redundant setup
  
- **Major Register Swaps Throughout**:
  ```
  r23 ↔ r18  (player car transform matrix reference)
  r24 ↔ r23  (temp position)
  r30 ↔ f31  (best distance tracking - FLOAT vs INT register!)
  ```
  
- **Vector Operations Reordering**: 
  - Same `eMulVector()` calls but different stack offsets
  - Left stores at 0x48-0x54, Right stores at 0x68-0x74
  - Compiler made different stack allocation choices

- **Why This Matters**: This function has a loop over camera tracks
  - Each iteration does similar math
  - Compiler optimized variable reuse aggressively
  - Left version saves fewer registers but more stack accesses
  - Right version uses more registers but cleaner

### Pattern 4: Extreme Reordering (Very Hard)

**DebugWorldCameraMover::Update (87.5%, 257B unmatched)**
- **Widespread Register Swaps**:
  - r26 ↔ r25 (static global references to Eye/Look)
  - f30 ↔ f31 (temporary float calculations)
  - r29 ↔ r30 (object pointers)

- **Large Instruction Reordering**:
  ```
  Left sequence:
  - stfs f0, 0x28(r1)
  - stfs f13, 0x20(r1)
  - stfs f12, 0x24(r1)
  - bl bVector3 constructor
  - (then uses values from stack)
  
  Right sequence:
  - All the stfs mixed with loads/calculations
  - No separate bVector3 constructor call
  - Direct inline operations
  ```

- **Missing Constructor Call**: 
  - Left: `bl bVector3::bVector3(bVector3 const &)`
  - Right: Eliminated (inline copy)
  - This is 4 instructions saved

- **Stack Offset Shifting**:
  - Left uses 0x80-0x88 region
  - Right uses 0x40-0x48 region
  - Completely different stack layout for same data

- **Why It's Hard**: 
  - This isn't just register allocation
  - Compiler inlined a constructor and reordered everything
  - Would need source code refactoring or explicit inline attributes


## CODE GENERATION DIFFERENCES

### Mismatch Categories:

1. **Register Allocation Mismatch**: 20-30 instructions typically
   - Different saved register set
   - Different temp register usage
   - Different immediate loading strategies

2. **Stack Frame Mismatch**: 8-16 bytes typically
   - Different padding strategies
   - Different alignment choices
   - Different local variable stack layouts

3. **Instruction Reordering**: Variable, 10-50 instructions
   - Load/store interleaving
   - Due to compiler scheduler
   - Doesn't change semantics

4. **Optimization Differences**: 20-100+ instructions
   - Loop-invariant code motion
   - Dead code elimination
   - Constant folding

5. **Inlining Decisions**: 4-40 instructions
   - Constructor inlining
   - Small function inlining
   - Wrapper function elimination


## ACTIONABLE RECOMMENDATIONS

### Tier 1: ~100B Potential (Easy Fixes)
- **ReplayPowerSlideScore**: Focus on register naming conventions
  - Add `#pragma clang attribute push (__attribute__((preserve_most)))`
  - Or reorder temp variable declarations
  
- **ReplayJumpScore**: Remove unused register from saved set
  - May require refactoring to not use r30
  
- **FixWorldHeight**: Stack frame sizing
  - Try adjusting local variable types or alignment

### Tier 2: ~200B Potential (Medium Difficulty)
- **ChooseGoodSceneCameraTrackIndex**: 
  - Add loop initialization guards to prevent optimization
  - Restructure vector operation storage pattern
  - May need `#pragma unroll(1)` to prevent loop optimization
  
- **GetGroundElevation**: 
  - Similar to FixWorldHeight

### Tier 3: Defer (Very High Effort)
- **DebugWorldCameraMover::Update**:
  - Would require significant refactoring
  - Consider focusing on other functions first
  - This is likely due to more aggressive -O3 vs -O2 settings


## COMPILER FLAGS HYPOTHESIS

The differences suggest:
- Left compiled with: `-O2` or `-O1` (conservative)
- Right compiled with: `-O3` or `-O4` (aggressive) + link-time optimization (LTO)
- Possibly different `-march=` or `-mtune=` flags

Evidence:
- Right eliminates loop-invariant code more aggressively
- Right inlines constructors
- Right makes bolder register allocation choices
- Right sometimes uses fewer stack bytes despite same logic

