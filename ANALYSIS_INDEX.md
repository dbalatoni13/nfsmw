# NFS Most Wanted 2005 (GameCube) - zWorld2 Decompilation Analysis
## WTriggerManager Function Mismatches

### 📁 Generated Analysis Files

#### 1. **MISMATCH_SUMMARY.md** (Comprehensive)
Detailed breakdown of all 5 functions with:
- Register allocation mismatches
- Floating point register issues
- Stack memory organization differences
- Missing/reordered instructions
- Pattern analysis across all functions
- Root cause analysis

#### 2. **CRITICAL_MISMATCHES.txt** (Executive Summary)
Visual presentation of most critical instruction differences:
- Side-by-side comparison of key mismatches
- Register cascade analysis
- Floating point pressure visualization
- Branch condition semantics differences
- Missing instruction analysis
- Categorization of 6 mismatch types

#### 3. **ALL_DIFFS_COMPLETE.txt** (Overview)
Quick reference document with:
- File locations of full diffs
- Summary of key findings per function
- Pattern analysis
- Root cause hypotheses

#### 4. **Full Diff Output Files**
- `ProcessRB_full.txt` (341 lines) - 95.3% match, 62B unmatched
- `ProcessSRB.txt` (366 lines) - 95.2% match, 68B unmatched  
- `CheckCollideRB.txt` (253 lines) - 94.6% match
- `CheckCollideSRB.txt` (271 lines) - 94.5% match
- `WorldEffectConnUpdate.txt` (545 lines) - 95.1% match, 104B unmatched

### 🎯 Quick Reference

**5 Functions Analyzed:**
1. ✓ ProcessRB
2. ✓ ProcessSRB
3. ✓ WorldEffectConn::Update
4. ✓ CheckCollideRB
5. ✓ CheckCollideSRB

### 🔴 Critical Issues Found

#### Issue #1: Register Allocation Cascade
- **Severity**: High
- **Affected Functions**: ProcessRB, ProcessSRB, CheckCollideSRB
- **Description**: Initial register choice cascades through 15+ subsequent instructions
- **Example**: r27 vs r28 parameter allocation in ProcessRB affects entire function

#### Issue #2: Floating Point Register Pressure
- **Severity**: Medium
- **Affected Functions**: ProcessRB, ProcessSRB, WorldEffectConn::Update
- **Description**: Different FP register allocation (f2-f13 range) based on compiler pressure
- **Pattern**: f30/f31 swaps, different storage locations

#### Issue #3: Branch Condition Semantics
- **Severity**: CRITICAL
- **Affected Functions**: CheckCollideRB
- **Description**: Same fcmpu instruction but different branch types (blt vs bso)
- **Root Cause**: Different compiler optimization interpreting comparison differently

#### Issue #4: Missing Instructions
- **Severity**: CRITICAL
- **Affected Functions**: WorldEffectConn::Update, CheckCollideRB
- **Description**: Function calls or setup instructions removed/moved
- **Example**: VU0_sqrt call present in LEFT but missing in RIGHT

#### Issue #5: Stack Layout Reorganization
- **Severity**: Medium
- **Affected Functions**: CheckCollideSRB, WorldEffectConn::Update
- **Description**: Temporary storage allocated at different stack offsets
- **Impact**: Cascades all subsequent FP loads/stores

### 📊 Match Statistics

| Function | Match % | Unmatched | Instructions |
|----------|---------|-----------|--------------|
| ProcessRB | 95.3% | 62B | 337 |
| ProcessSRB | 95.2% | 68B | 361 |
| WorldEffectConn::Update | 95.1% | 104B | 540 |
| CheckCollideRB | 94.6% | - | 248 |
| CheckCollideSRB | 94.5% | - | 256 |

### 🔍 Analysis Methodology

Each function was analyzed using the decomp-diff tool with flags:
```bash
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zWorld2 \
  -d '<function_name>' -C 0 --no-collapse 2>&1
```

Output includes:
- OFFSET: Memory address of instruction
- LEFT: Binary compilation (our reference)
- LINE: Source line number (when available)  
- RIGHT: Expected/decompiled version
- ~ (tilde): Difference indicator
- < (less): Extra in LEFT
- > (greater): Extra in RIGHT

### 💡 Root Causes (Hypothesis)

1. **Compiler Version Difference**
   - Different versions of Metrowerks CodeWarrior compiler
   - Different optimization levels/flags

2. **Register Allocator Differences**
   - Live register set management differs
   - Parameter passing convention interpretation

3. **Inlining Decisions**
   - Different functions inlined (VU0_sqrt)
   - Affects instruction scheduling

4. **FP Unit Scheduling**
   - Different delays/scheduling for floating point ops
   - Stack frame optimization strategies

5. **Dead Code Elimination**
   - Different detection of unused branches
   - Affects instruction reordering

### 📋 Next Steps for Decompilation

1. **Priority Fixes** (to improve match %):
   - Address register allocation in ProcessRB/ProcessSRB
   - Fix branch condition in CheckCollideRB
   - Restore VU0_sqrt call in WorldEffectConn::Update

2. **Investigation Needed**:
   - Check compiler flags used in original build
   - Verify calling convention implementation
   - Validate FP register usage patterns

3. **Validation**:
   - Create test cases for each function
   - Verify floating point accuracy
   - Test edge cases in collision detection

### 📚 Related Commands

Generate all diffs at once:
```bash
cd /Users/johannberger/nfsmw-zWorld2

for func in "ProcessRB" "ProcessSRB" "WorldEffectConn::Update" "CheckCollideRB" "CheckCollideSRB"; do
  python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zWorld2 \
    -d "$func" -C 0 --no-collapse 2>&1
done
```

View specific mismatch lines:
```bash
grep "^[<>~]" ProcessRB_full.txt | head -50
```

Count mismatches by type:
```bash
echo "Register mismatches:" && grep "{" *.txt | wc -l
echo "Missing instructions:" && grep "^<" *.txt | wc -l
echo "Extra instructions:" && grep "^>" *.txt | wc -l
```

---
**Last Updated**: 2024
**Analysis Tool**: decomp-diff
**Target**: NFS Most Wanted 2005 (GameCube)
**Translation Unit**: zWorld2
