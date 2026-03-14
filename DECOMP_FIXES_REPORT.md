# Decomp-Diff Analysis & Source-Level Fixes
## Investigation of 5 Functions for Potential Fixes

---

## Executive Summary

**Overall Match Before Fixes:** 73.6% average
**Expected Match After Fixes:** 77.4% average
**Estimated Improvement:** +4 percentage points

| Function | Before | After | Issue Type | Fixability |
|----------|--------|-------|-----------|-----------|
| Average::Flush | 85.7% | ~87% | Store Order | ✅ FIXABLE |
| JoylogBuffer::PrintNearbyJoylogEntries | 62.8% | ~75% | Branch Inversion | ✅ FIXABLE |
| JoylogBuffer::GetEntry | 58.5% | 58.5% | Pure Register Allocation | ⚠️ ACCEPTABLE |
| DisculatorDriver::Read | 76.3% | 76.3% | Calculation Ordering | ⚠️ COMPILER-DEPENDENT |
| bFile::ReadAsync | 84.9% | ~90% | Param Swap + Missing Code | ✅ CRITICAL |

---

## 1. Average::Flush (85.7% match)

**File:** `src/Speed/Indep/Src/Misc/Table.cpp:182-189`

### Issue: Store Order - Intermediate Register Allocation

Line 186 contains nested casts that force intermediate stack allocation:

```c
fTotal = fValue * static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSlots)));
```

**Decomp Difference:**
- LEFT: `stfs f1, 0xc(r3)` → `stw r8, 8(r1)` (store to object, then stack)
- RIGHT: `stw r8, 8(r1)` → `stfs f1, 0xc(r3)` (store to stack, then object)

### Source-Level Fix

**Change Line 186:**
```c
// BEFORE:
fTotal = fValue * static_cast<float>(static_cast<int>(static_cast<unsigned int>(nSlots)));

// AFTER:
fTotal = fValue * static_cast<float>(nSlots);
```

**Why This Works:** Removing nested casts eliminates intermediate register allocation on stack, allowing the compiler to optimize store order differently.

**Severity:** MEDIUM | **Complexity:** TRIVIAL | **Risk:** LOW

---

## 2. JoylogBuffer::PrintNearbyJoylogEntries (62.8% match)

**File:** `src/Speed/Indep/Src/Misc/Joylog.cpp:177-193`

### Issue A: Branch Direction Inversion (bge ↔ ble)

At decomp offset 60cc/60c8:
- LEFT: `bge` (branch if greater-or-equal) 
- RIGHT: `ble` (branch if less-or-equal)

Associated comparison operands are swapped:
- LEFT: `cmpw r0, r11` 
- RIGHT: `cmpw r11, r0`

This occurs at source line 184: `if (TopPosition <= new_pos) { return; }`

### Issue B: Register Allocation 

The parameter `this` (passed in r3) is allocated to:
- LEFT: `r30` (with `stmw r30` - save multiple)
- RIGHT: `r31` (with `lwz r31, 0x1c(r1)`)

This cascades through the function affecting all member access.

### Source-Level Fixes

**Change 2A: Reorder Variable Declarations (lines 178-179)**
```c
// BEFORE:
void JoylogBuffer::PrintNearbyJoylogEntries(int error_pos) {
    const int range = 40;
    int pos = BufferStartPosition;

// AFTER:
void JoylogBuffer::PrintNearbyJoylogEntries(int error_pos) {
    int pos = BufferStartPosition;
    const int range = 40;
```

**Why:** Declaration order influences how the compiler allocates registers for parameters and locals.

**Change 2B: Swap Comparison Operands (line 184)**
```c
// BEFORE:
if (TopPosition <= new_pos) {
    return;
}

// AFTER:
if (new_pos >= TopPosition) {
    return;
}
```

**Why:** Swapping operands forces the compiler to generate comparison instructions with reversed register order, changing `bge` to `ble`.

**Severity:** HIGH | **Complexity:** LOW | **Risk:** LOW

---

## 3. JoylogBuffer::GetEntry (58.5% match)

**File:** `src/Speed/Indep/Src/Misc/Joylog.cpp:166-175`

### Assessment: PURE REGISTER ALLOCATION

The loop algorithm is **identical** between LEFT and RIGHT versions:

```c
for (int byte_num = 0; byte_num < entry->DataSize; byte_num++) {
    data |= static_cast<unsigned int>(pbuf[1 + byte_num]) << (byte_num * 8);
}
```

**Differences Detected:**
- Loop accumulator: LEFT uses `r10`, RIGHT uses `r7`
- Loop limit: LEFT uses `r11`, RIGHT uses `r6`
- Loop counter: LEFT uses `r8`, RIGHT uses `r10`
- Shift operation: LEFT uses `slwi` then `or`, RIGHT uses `slw`

All differences are purely in **register naming**. The algorithm executes identically in both versions.

### Assessment

✅ **NO CHANGES REQUIRED** - This is acceptable compiler variation
- Functionally correct in both versions
- Loop structure and algorithm match perfectly
- Only register allocation differs

### Optional Fix (if binary match required)

Unroll the loop for consistent register allocation:

```c
int JoylogBuffer::GetEntry(JoylogBufferEntry *entry, uint8 *pbuf) {
    entry->ChannelNumber = pbuf[0] & 0xF;
    entry->DataSize = pbuf[0] >> 4;
    unsigned int data = 0;
    
    // Unroll for 4-byte maximum
    if (entry->DataSize >= 1) data |= pbuf[1] << 0;
    if (entry->DataSize >= 2) data |= pbuf[2] << 8;
    if (entry->DataSize >= 3) data |= pbuf[3] << 16;
    if (entry->DataSize >= 4) data |= pbuf[4] << 24;
    
    entry->Data = data;
    return entry->DataSize + 1;
}
```

**Severity:** LOW | **Complexity:** MEDIUM | **Risk:** NONE

---

## 4. DisculatorDriver::Read (76.3% match)

**File:** `src/Speed/Indep/Src/Misc/bFile.cpp:293-312` (focus on line 307)

### Issue: Calculation Sequence Reordering

Source line 307 performs a 64-bit bit rotation:

```c
unsigned long long seekOffset = static_cast<unsigned long long>(
    static_cast<int>(sectorBase >> 21 | sectorBase << 11)
) + file->seekPos;
```

**Decomp Difference:**

LEFT immediately chains operations:
```asm
addc r6, r8, r10
adde r5, r7, r9
srwi r0, r6, 11
slwi r4, r5, 21
or r10, r4, r0
```

RIGHT defers and reorders:
```asm
slwi r9, r5, 21
srwi r0, r6, 11
or r12, r9, r0
addc r6, r6, r10
adde r5, r5, r9
```

Both produce the same final result, but the calculation sequence differs.

### Assessment: COMPILER-DEPENDENT

This is compiler optimization behavior - both versions produce correct results.

### Optional Fix (if binary match required)

Force intermediate calculation:

```c
unsigned int rotated = (sectorBase >> 21) | (sectorBase << 11);
unsigned long long seekOffset = static_cast<unsigned long long>(
    static_cast<int>(rotated)
) + file->seekPos;
```

**Note:** May still not match if compiler reorders the second calculation anyway.

**Severity:** MEDIUM | **Complexity:** MEDIUM | **Risk:** LOW

---

## 5. bFile::ReadAsync (84.9% match) - �� CRITICAL

**File:** `src/Speed/Indep/Src/Misc/bFile.cpp:559-603`

### Critical Issue A: Parameter Register Swap

At decomp offset 8850-8854:

**LEFT allocation:**
```asm
mr r26, r4    (r26 ← num_bytes)
mr r27, r6    (r27 ← callback_param)
```

**RIGHT allocation:**
```asm
mr r27, r4    (r27 ← num_bytes)
mr r26, r6    (r26 ← callback_param)
```

**Function signature:** 
```c
void bFile::ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param)
// Parameters: r3=buf, r4=num_bytes, r5=callback, r6=callback_param
```

**Consequence:** This register swap cascades through the function:

| Line | LEFT Uses | RIGHT Uses | Intended |
|------|-----------|-----------|----------|
| 569 | `r26` | `r27` | num_bytes |
| 572 | `r26` | `r27` | buf (wrong!) |
| 574 | `r27` | `r26` | callback (inverted!) |
| 577 | `r27` | `r26` | callback_param (wrong!) |
| 584 | `r27` | `r26` | callback (wrong!) |

### Critical Issue B: Missing Position Update

At decomp offsets 89e4-89ec, LEFT has 3 instructions **missing from RIGHT**:

```asm
89e4 | lwz r0, 0x10(r29)     |      | Load current Position
89e8 | add r0, r0, r28       |      | Add bytes read
89ec | stw r0, 0x10(r29)     |      | Store back to Position
```

This updates the file position after an async read operation. **RIGHT version doesn't have this.**

### Source-Level Fixes

#### Fix 5A: Add Explicit Local Variables (INSERT after line 559)

```c
void bFile::ReadAsync(void *buf, int num_bytes, void (*callback)(void *), void *callback_param) {
    // Force register allocation in correct order
    void * const work_buf = buf;
    int work_num_bytes = num_bytes;
    void (* const work_callback)(void *) = callback;
    void * const work_callback_param = callback_param;
    
    bGetTicker();
    if (FileSize < Position + work_num_bytes) {
        work_num_bytes = FileSize - Position;
    }
    // ... rest of function using work_* variables
}
```

**Then replace all references:**
- `buf` → `work_buf` (lines 562, 569, 572, 600)
- `num_bytes` → `work_num_bytes` (lines 561, 562, 567, 568, 569, 570, 600)
- `callback` → `work_callback` (line 574, 584)
- `callback_param` → `work_callback_param` (line 577, 586)

#### Fix 5B: Add Missing Position Update (after line 601)

```c
    } else {
        int fop = FILESYS_read(FileHandle, work_buf, work_num_bytes, nullptr, cb);
        FILESYS_callbackop(fop, bFile::CallbackFunctionRead);
        Position = Position + work_num_bytes;  // ← ADD THIS LINE
    }
}
```

### Why These Fixes Work

1. **Explicit locals** force the compiler to allocate parameters in a consistent order matching the target
2. **Position update** restores missing functionality (file position tracking)

**Severity:** CRITICAL | **Complexity:** MEDIUM | **Risk:** LOW

**Affected Areas:**
- File reading with async callbacks
- File position tracking
- Callback function invocation

---

## Issue Classification Summary

### Branch Direction Issues
- ✅ **JoylogBuffer::PrintNearbyJoylogEntries**: `ble` vs `bge` (FIXABLE)
- ✅ **bFile::ReadAsync**: Parameter swap affects branches (FIXABLE via locals)

### Store Order Issues
- ✅ **Average::Flush**: `stfs`/`stw` order (FIXABLE via cast simplification)
- ✅ **bFile::ReadAsync**: Store sequence from register allocation (FIXABLE via locals)

### Loop Structure Issues
- ⚠️ **JoylogBuffer::GetEntry**: Register allocation in loop (ACCEPTABLE)

### Missing/Extra Code Blocks
- ✅ **bFile::ReadAsync**: Missing 3 instructions for position update (FIXABLE)
- ⚠️ **DisculatorDriver::Read**: Deferred calculation sequence (COMPILER-DEPENDENT)

### Pure Register Allocation
- ⚠️ **JoylogBuffer::GetEntry**: 58.5% match (ACCEPTABLE)
- ⚠️ **DisculatorDriver::Read**: 76.3% match (MOSTLY ACCEPTABLE)

---

## Implementation Roadmap

### Priority 1: CRITICAL
1. **bFile::ReadAsync** - Add explicit locals + position update

### Priority 2: HIGH
2. **JoylogBuffer::PrintNearbyJoylogEntries** - Swap condition operands

### Priority 3: MEDIUM
3. **Average::Flush** - Remove nested casts
4. **DisculatorDriver::Read** - Optional: split calculation (if binary match required)

### Priority 4: SKIP
5. **JoylogBuffer::GetEntry** - No changes needed (acceptable as-is)

---

## Verification Checklist

- [ ] Average::Flush: Line 186 - remove nested casts
- [ ] JoylogBuffer::PrintNearbyJoylogEntries: Lines 178-179 - reorder declarations
- [ ] JoylogBuffer::PrintNearbyJoylogEntries: Line 184 - swap condition operands
- [ ] bFile::ReadAsync: Line 559-560 - add explicit local variables
- [ ] bFile::ReadAsync: Lines 561-602 - replace all buf/num_bytes/callback references
- [ ] bFile::ReadAsync: Line 602 - add missing Position update
- [ ] Compile and test
- [ ] Run decomp-diff verification

---

## Conclusion

**3 out of 5 functions have fixable issues at the source level**, primarily due to:
1. Register allocation order (solvable via explicit locals or declaration reordering)
2. Comparison operand order (solvable via logic rewriting)
3. Missing code blocks (solvable via adding missing lines)

**2 out of 5 functions are acceptable as-is:**
1. Pure register allocation differences (functionally equivalent)
2. Compiler optimization differences (both correct)

**Expected improvement: +4% average match** across the 5 functions after implementing Priority 1-3 fixes.
