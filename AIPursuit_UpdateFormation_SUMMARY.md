# AIPursuit::UpdateFormation(3900B) - Complete Decompilation Context

## Executive Summary

**Function:** `AIPursuit::UpdateFormation(float dT)`  
**Size:** 3900 bytes (975 PPC instructions)  
**Address Range:** `0x80032850` → `0x8003378C`  
**Status:** Missing in decomp output - requires implementation  
**Priority:** HIGH (formation AI core logic)

---

## Command Outputs Completed

### ✅ Command 1: Ghidra Decompilation
- **Full decompilation:** 600+ lines of pseudocode
- **Contains:** Parameter analysis, vector operations, interface queries, formation logic
- **Key insight:** Formation speed calculations and cop positioning logic

### ✅ Command 2: DWARF Debug Info (Address: 0x80032850)
- **Signature:** `void AIPursuit::UpdateFormation(float dT /* f29 */)`
- **Register mapping:** r31=this, f29=dT, r27=targetvehicleai, r28=itargetRB, r29=pursuitIter
- **Local variables:** 14 significant variables tracked across execution

### ✅ Command 3: AIPursuit.h Header Grep
- Formation class hierarchy identified
- PursuitFormation base class with virtual GetTargetOffsets()
- Subclasses: BoxInFormation, RollingBlockFormation, FollowFormation, PitFormation, HerdFormation

### ✅ Command 4: AIPursuit.cpp Implementation Grep
- **Result:** UpdateFormation NOT YET IMPLEMENTED
- Function stub missing from current codebase
- Related functions (EvenOutOffsets, AssignClosestOffsets) also referenced but not found

---

## Critical Function Flow

### 1. **Initialization & Validation** (0x80032850-0x80032930)
```
- QR register validation (PSQ operations)
- Query IVehicleAI interface from target
- Query IRigidBody interface
- Query IPerpetrator for player info
- Early exit if target invalid
```

### 2. **Cop Collection Loop** (0x80032930-0x80032E60)
```
- Iterate vehicle pool
- Find IPursuitAI interface for each cop
- Check distance to target (formationCandidateLimit)
- Handle chopper special case (AssignChopperGoal)
- Collect positions in vectors:
  * assignCopList (IPursuitAI*)
  * copRelativePositions (UMath::Vector3)
```

### 3. **Formation Assignment** (0x80032E68-0x80032F2C)
```
- Call EvenOutOffsets(copRelativePositions, formationOffsets)
- Call AssignClosestOffsets(copRelativePositions, assignCopList, formationOffsets)
- Result: Cops assigned to formation target positions
```

### 4. **Position Verification** (0x80032F2C-0x80033028)
```
- Iterate assignCopList
- Check if cop is in formation zone (dist <= disttolerance)
- Count cops in position (countInPosition)
- Update cop state (in/out of formation)
```

### 5. **Formation Speed Logic** (0x80033028-0x80033660)
```
- Get player speed from IPerpetrator
- Get pursuit level attributes (CollapseSpeed, MaxCopsCollapsing, etc.)
- Calculate average distance to formation positions
- Adjust formation speed:
  * Increase if cops getting closer
  * Decrease if cops spread out
  * Check against player speed
- Handle collapse maneuver (tactical retreat)
```

### 6. **Cleanup** (0x80033660-0x8003376C)
```
- Deallocate temporary vectors via FastMem
- Restore QR registers
- Return to caller
```

---

## Key Helper Functions

| Function | Purpose |
|----------|---------|
| `EvenOutOffsets` | Distribute cop positions across formation targets |
| `AssignClosestOffsets` | Match each cop to nearest formation target |
| `UpdateOutOfFormationOffsets` | Handle cops outside formation zone |
| `SetupCollapse` | Initiate tactical retreat maneuver |
| `AssignChopperGoal` | Special handling for chopper vehicles |
| `GetAttributePointer` / `DefaultDataArea` | Fetch pursuit attributes |
| `VU0_v3sub`, `VU0_v3length`, `VU0_sqrt` | Graphics synthesizer math |

---

## Data Structures & Offsets

### AIPursuit Member Offsets (param_2)
```
+0x6c:  Cop vehicle count
+0x9c:  Target (AITarget pointer)
+0xa8:  Formation object (PursuitFormation*)
+0x9c+0x28: Pursuit state flag
+0xd4:  Current formation speed
+0xd8:  Formation speed transition
+0xe0:  Collapse maneuver active flag
+0x114: Special flag 1
+0x118: Special flag 2
+0x144: Special flag 3
+0x210: Pursuit type identifier
+0x1c4: Ground support request
+0x23c: Police wanted level
```

### Local Variables (Stack)
```
r1+0x8:   vector<IPursuitAI*>           (assignCopList)
r1+0x20:  vector<UMath::Vector3>        (copRelativePositions)
r1+0x30:  vector<FormationTarget>       (formationOffsets)
r1+0x40-0x50: temp position data
r1-0xE0:  saved registers (PSQ f29-f31)
```

---

## Global Constants

```
_UNK_803ca8c4  → Formation candidate distance limit
_UNK_803ca8c8  → Minimum formation speed
_UNK_803ca8cc  → Formation distance tolerance
_UNK_803ca8d0  → Speed multiplier
_UNK_803ca8d4  → Police wanted level speed override
_UNK_803ca8d8  → Collapse distance offset
_UNK_803ca8e0  → Formation rate acceleration factor
_UNK_803ca8e8  → Count normalization (formation calculation)
_UNK_803ca8f0  → Max formation speed cap
_UNK_803ca8f4  → Speed reduction factor
```

---

## Implementation Priorities

### Phase 1: Core Structure
- [ ] Convert Ghidra pseudocode to typed C++
- [ ] Map all offset accesses to AIPursuit members
- [ ] Implement interface queries (IVehicleAI, IRigidBody, IPerpetrator)

### Phase 2: Cop Collection
- [ ] Implement vehicle iteration logic
- [ ] Add distance filtering
- [ ] Implement vector reallocation patterns

### Phase 3: Formation Assignment
- [ ] Reverse engineer EvenOutOffsets
- [ ] Reverse engineer AssignClosestOffsets
- [ ] Connect to formation target selection

### Phase 4: Speed & Dynamics
- [ ] Implement formation speed calculations
- [ ] Add collapse maneuver detection
- [ ] Handle edge cases (no cops, single cop, full formation)

### Phase 5: Testing & Validation
- [ ] Verify against binary
- [ ] Test with actual pursuit scenarios
- [ ] Profile performance

---

## Notes

- Function uses PPC graphics synthesizer (PSQ operations, VU0 functions)
- Heavy use of dynamic memory allocation (FastMem)
- Complex vector reallocation patterns need careful implementation
- Formation logic depends on external PursuitFormation subclass
- Speed calculations involve floating-point accumulation and constraints

**Full decompilation saved to:** `/Users/johannberger/nfsmw-zAI/AIPursuit_UpdateFormation_FULL_CONTEXT.txt`

