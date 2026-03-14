# AIVehicle::UpdateRoads - Complete Implementation Context

## Overview
This collection contains all necessary context gathered from the nfsmw-zAI repository to implement `AIVehicle::UpdateRoads()`.

**Function Details:**
- **Binary Address:** 0x80021A28
- **Address Range:** 0x80021A28 → 0x800220EC
- **Size:** 1732 bytes (433 instructions)
- **Stack Frame:** -0x470 (1136 bytes)
- **Status:** Missing in decomp output (needs implementation)

## Files in this Collection

### 1. **AIVehicle_UpdateRoads_QUICK_REFERENCE.txt** ⭐ START HERE
**Best for:** Quick overview and implementation planning
- Algorithm overview with ASCII flowchart
- Key member variables with offsets
- Local variables and stack layout
- Register allocation summary
- External functions called
- Control flow diagram

### 2. **AIVehicle_UpdateRoads_IMPLEMENTATION_CONTEXT.txt**
**Best for:** Detailed understanding and reference
- Complete command outputs summary (7 sections)
- Ghidra decompilation analysis
- Function signature and local variables
- AIVehicle struct definition (1876 bytes total)
- Source code context (GetSeekAheadPosition caller)
- Header file declarations
- Implementation notes and responsibilities
- Critical constants and register allocations

### 3. **AIVehicle_UpdateRoads_RAW_COMMAND_OUTPUT.txt**
**Best for:** Verification and debugging
- Verbatim output from all 4 commands
- Complete Ghidra decompilation
- Full DWARF function signature
- Full struct definition
- Can be used to cross-reference with tool outputs

## Key Context for Implementation

### Function Signature
```c
void AIVehicle::UpdateRoads();
```

### Purpose
Updates the AI vehicle's path through the road network:
- Maintains current and future road navigation (WRoadNav objects)
- Updates lookahead position and direction for steering
- Handles lane selection and transitions
- Manages update timing with two throttle timers

### Core Algorithm
1. **Profiling & Proximity Check** - Query collision body position
2. **Update Gating** - Time-based throttling (2 levels)
3. **Velocity & Race Filter Setup** - Get speed and racing status
4. **Road Walker Initialization** - Set up path traversal
5. **Path Walking** - Traverse road network segments
6. **Lane Navigation** - Select and transition lanes
7. **Far Future Calculation** - Update lookahead position/direction

### Critical Member Variables

| Offset | Variable | Type | Size | Purpose |
|--------|----------|------|------|---------|
| 0x114  | mRoadUpdateTimer | float | 4 | Major update throttle |
| 0x118  | mRoadIncrementTimer | float | 4 | Incremental update throttle |
| 0x148  | mCurrentRoad | WRoadNav | 784 | Current segment navigation |
| 0x438  | mFutureRoad | WRoadNav | 784 | Lookahead segment navigation |
| 0x12C  | mFarFutureDirection | Vector3 | 12 | Direction ahead |
| 0x138  | mFarFuturePosition | Vector3 | 12 | Position ahead |
| 0x144  | mLastFutureSegment | short | 2 | Last segment index |
| 0x146  | mLastFutureNodeInd | short | 2 | Last node index |

### Key Global Constants
These control UpdateRoads behavior:
- `_UNK_803ca034` - Proximity threshold
- `_UNK_803ca038` - Road update timer threshold
- `_UNK_803ca03c` - Road increment timer threshold
- `_UNK_803ca040` - Min distance parameter
- `_UNK_803ca044` - Normalized distance parameter
- `_UNK_803ca048` - Lane change threshold
- `_UNK_803ca04c` - Far future distance
- `_UNK_803ca050` - Max future distance
- `_12WRoadNetwork_fValidRaceFilter` - Race filter validation

### Register Allocation
- **r31:** `this` pointer (AIVehicle*)
- **r27:** `isvalid` flag (proximity check)
- **r26:** `bRaceRouteOnly` flag (race filter)
- **f31:** `speed` (velocity magnitude)

### Stack Layout
- **-0x470:** Frame start
- **r1+0x8:** currentoff (vector)
- **r1+0x18:** velocity (vector)
- **r1+0x28:** position (calculations)
- **r1+0x38:** direction (calculations)
- **r1+0x88:** walker (road_walker struct)

## Command Execution History

All following commands were executed and their outputs are documented:

```bash
# Command 1 - Ghidra decompilation
python tools/decomp-workflow.py function -u main/Speed/Indep/SourceLists/zAI \
    -f 'AIVehicle::UpdateRoads' --ghidra-version gc

# Command 2 - Find correct address
python tools/decomp-diff.py -u main/Speed/Indep/SourceLists/zAI --search UpdateRoads

# Command 3 - DWARF function info
python tools/lookup.py ./symbols/Dwarf function 0x80021A28

# Command 4 - AIVehicle struct
python tools/lookup.py ./symbols/Dwarf struct AIVehicle
```

## External Function Dependencies

### Interface Lookups
- `ICollisionBody::Find()` - Get collision interface
- `IPerpetrator::Find()` - Get pursuit interface

### Time
- `Sim::GetTime()` - Get simulation time

### Path Traversal
- `road_walker::constructor()`
- `walk_road__11road_walker()` - Main path traversal

### Road Navigation
- `WRoadNav::SnapToSelectableLane()` - Lane selection
- `WRoadNav::ChangeLanes()` - Lane transition
- `WRoadNav::IncNavPosition()` - Move along lane
- `WRoadNav::GetPosition()` - Get current position
- `WRoadNav::GetForwardVector()` - Get direction vector

### Vector Math (VU0)
- `VU0_v3sub()` - Subtraction
- `VU0_v3length()` - Magnitude
- `VU0_v3unit()` - Normalization
- `VU0_v3scale()` - Scaling
- `VU0_v3dotprod()` - Dot product
- `VU0_v3distance()` - Distance
- `VU0_rsqrt()` - Reciprocal square root

## Related Source Files

- **Main Implementation:** `src/Speed/Indep/Src/AI/Common/AIVehicle.cpp`
- **Header File:** `src/Speed/Indep/Src/AI/AIVehicle.h`
- **Caller Context:** `GetSeekAheadPosition()` (lines 1137-1177)
- **Lookup Tool:** `tools/lookup.py`
- **Decomp Workflow:** `tools/decomp-workflow.py`

## Usage Recommendations

### For Understanding the Algorithm
1. Start with **QUICK_REFERENCE.txt** - Algorithm overview
2. Review the control flow diagram
3. Examine local variables and stack layout

### For Implementation Details
1. Read **IMPLEMENTATION_CONTEXT.txt** - Complete analysis
2. Study the Ghidra decompilation
3. Reference external function signatures
4. Check member variable offsets

### For Verification
1. Cross-reference with **RAW_COMMAND_OUTPUT.txt**
2. Verify addresses and sizes
3. Check DWARF function range boundaries

## Notes for Implementation

- Function has complex VU0 (Vector Unit 0) math operations
- Two-level timing throttling with separate timers
- Race filter affects road network navigation
- Handles both full path recalculation and incremental updates
- Large stack frame (1136 bytes) used for temporary structures
- Proximity check suggests special handling when vehicle is stopped/slow

---

**Generated:** March 13, 2025
**Status:** All context gathered - Ready for implementation
**Tools Used:** decomp-workflow.py, decomp-diff.py, lookup.py
