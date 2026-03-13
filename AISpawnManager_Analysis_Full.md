# AISpawnManager DWARF and Ghidra Decompilation Analysis

This document contains comprehensive DWARF debug information and Ghidra decompilation output for all AISpawnManager functions in the NFS: Most Wanted codebase.

---

## AISpawnManager Struct Definition (PS2 Version)

From `symbols/PS2/PS2_types.nothpp`:

```c
struct AISpawnManager { // 0xc
private:
/* 0x0 */ float mMinSpawnDist;
/* 0x4 */ float mMaxSpawnDist;
static int kMaxSpawnSegments;
static float mMaxGatherDist;
static int mSpawnSegment[50];
static int mNumSpawnSegments;
public:
/* 0x8 */ __vtbl_ptr_type *$vf9405;

AISpawnManager& operator=();
AISpawnManager();
protected:
void GetBasePosition();
void GetBaseForwardVector();
bool RespawnAvailable();
bool GetSpawnLocation();
bool CheckSpawnPosition();
AISpawnManager();
/* vtable[1] */ virtual AISpawnManager(AISpawnManager*, int, void);
private:
bool GetSpawnPointOnSegment();
void RefreshSpawnData();
};
```

**Struct Size:** 0xc bytes
**Static Members:** 
- mMaxGatherDist (float)
- mSpawnSegment (int[50])
- mNumSpawnSegments (int)
- kMaxSpawnSegments (int)

---

## AISpawnManager Functions

### 1. Constructor: AISpawnManager::AISpawnManager(float, float)

**Mangled Name:** `__14AISpawnManagerff`
**GC Address:** 0x80038830
**PS2 Address:** 0x132b78
**Size:** 92 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x80038830):
```
// Range: 0x80038830 -> 0x8003888C
// this: r3
AISpawnManager::AISpawnManager(float minSpawnDist /* f1 */, float maxSpawnDist /* f2 */) {
    // Range: 0x80038830 -> 0x80038860
    inline float UMath::Max(const float a, const float b) {
        // Range: 0x80038830 -> 0x80038860
        inline float VU0_floatmax(const float a, const float b) {}
    }

    /* anonymous block */ {
        // Range: 0x80038860 -> 0x80038888
        int i; // r11
    }
}
```

#### Ghidra Decompile (GC - 0x80038830):
```c
void __14AISpawnManagerff(double param_1, double param_2, float *param_3)
{
  int iVar1;
  int iVar2;
  double dVar3;
  
  param_3[2] = (float)_vt_14AISpawnManager;
  *param_3 = (float)param_1;
  param_3[1] = (float)param_2;
  dVar3 = (double)_14AISpawnManager_mMaxGatherDist;
  if ((double)_14AISpawnManager_mMaxGatherDist < param_2) {
    dVar3 = param_2;
  }
  _14AISpawnManager_mMaxGatherDist = (float)dVar3;
  iVar2 = 0;
  do {
    iVar1 = iVar2 * 4;
    iVar2 = iVar2 + 1;
    *(undefined4 *)(_14AISpawnManager_mSpawnSegment + iVar1) = 0xffffffff;
  } while (iVar2 < 0x32);
  return;
}
```

#### Ghidra Decompile (PS2 - 0x132b78):
```c
undefined8 __14AISpawnManagerff(undefined4 param_1, float param_2, undefined8 param_3)
{
  undefined4 *puVar1;
  int iVar2;
  
  puVar1 = (undefined4 *)param_3;
  *puVar1 = param_1;
  puVar1[2] = _vt_14AISpawnManager;
  puVar1[1] = param_2;
  _14AISpawnManager_mMaxGatherDist =
       (float)((int)param_2 * (uint)(_14AISpawnManager_mMaxGatherDist < param_2) |
              (int)_14AISpawnManager_mMaxGatherDist *
              (uint)(_14AISpawnManager_mMaxGatherDist >= param_2));
  puVar1 = (undefined4 *)(_14AISpawnManager_mSpawnSegment + 0xc4);
  iVar2 = 0x31;
  do {
    *puVar1 = 0xffffffff;
    iVar2 = iVar2 + -1;
    puVar1 = puVar1 + -1;
  } while (iVar2 > -1);
  return param_3;
}
```

---

### 2. Destructor: AISpawnManager::~AISpawnManager(void)

**Mangled Name:** `_._14AISpawnManager`
**GC Address:** 0x8003888C
**Size:** 52 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x8003888C):
```
// Range: 0x8003888C -> 0x800388C0
// this: r3
virtual AISpawnManager::~AISpawnManager() {}
```

#### Ghidra Decompile (GC - 0x8003888C):
```c
void ___14AISpawnManager(int param_1, uint param_2)
{
  *(undefined1 **)(param_1 + 8) = _vt_14AISpawnManager;
  if ((param_2 & 1) != 0) {
    __builtin_delete();
  }
  return;
}
```

**Note:** PS2 version not available in symbols.txt

---

### 3. GetBasePosition: AISpawnManager::GetBasePosition(UMath::Vector3 &)

**Mangled Name:** `GetBasePosition__14AISpawnManagerRQ25UMath7Vector3`
**GC Address:** 0x800388C0
**PS2 Address:** 0x132c00
**Size:** 136 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x800388C0):
```
// Range: 0x800388C0 -> 0x80038948
// this: r3
void AISpawnManager::GetBasePosition(struct UMath::Vector3 & basePos /* r4 */) {
    // Local variables
    struct eView * view;
    struct bVector3 cPos; // r1+0x8
    struct bVector3 posV3; // r1+0x18

    // Range: 0x800388C4 -> 0x800388C4
    inline struct eView * eGetView(int view_id, bool doAssert) {}

    // Range: 0x800388C4 -> 0x800388C4
    inline int eView::IsActive() const {}

    // Range: 0x800388F8 -> 0x800388F8
    inline struct Camera * eView::GetCamera() {}

    // Range: 0x800388F8 -> 0x800388F8
    inline struct bVector3 * Camera::GetPosition() {}

    // Range: 0x800388F8 -> 0x800388F8
    inline bVector3::bVector3(const struct bVector3 & v) {
        // Range: 0x800388F8 -> 0x800388F8
        inline struct bVector3 * bCopy(struct bVector3 * dest, const struct bVector3 * v) {
            // Local variables
            float x; // f13
            float y; // f0
            float z; // f12

            // Range: 0x800388F8 -> 0x800388F8
            inline struct bVector3 * bFill(struct bVector3 * dest, float x, float y, float z) {}
        }
    }

    // Range: 0x800388F8 -> 0x800388F8
    inline bVector3::bVector3() {}

    // Range: 0x800388F8 -> 0x800388F8
    inline void eUnSwizzleWorldVector(const struct bVector3 & inVec, struct bVector3 & outVec) {
        // Range: 0x800388F8 -> 0x800388F8
```

**Purpose:** Retrieves the base spawn position from the camera view

---

### 4. GetBaseForwardVector: AISpawnManager::GetBaseForwardVector(UMath::Vector3 &)

**Mangled Name:** `GetBaseForwardVector__14AISpawnManagerRQ25UMath7Vector3`
**GC Address:** 0x80038948
**PS2 Address:** 0x132c88
**Size:** 136 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x80038948):
```
// Range: 0x80038948 -> 0x800389D0
// this: r3
void AISpawnManager::GetBaseForwardVector(struct UMath::Vector3 & baseForwardVec /* r4 */) {
    // Local variables
    struct eView * view;
    struct bVector3 cPos; // r1+0x8
    struct bVector3 posV3; // r1+0x18

    // Range: 0x8003894C -> 0x8003894C
    inline struct eView * eGetView(int view_id, bool doAssert) {}

    // Range: 0x8003894C -> 0x8003894C
    inline int eView::IsActive() const {}

    // Range: 0x80038980 -> 0x80038980
    inline struct Camera * eView::GetCamera() {}

    // Range: 0x80038980 -> 0x80038980
    inline struct bVector3 * Camera::GetDirection() {}

    // Range: 0x80038980 -> 0x80038980
    inline bVector3::bVector3(const struct bVector3 & v) {
        // Range: 0x80038980 -> 0x80038980
        inline struct bVector3 * bCopy(struct bVector3 * dest, const struct bVector3 * v) {
            // Local variables
            float x; // f13
            float y; // f0
            float z; // f12

            // Range: 0x80038980 -> 0x80038980
            inline struct bVector3 * bFill(struct bVector3 * dest, float x, float y, float z) {}
        }
    }

    // Range: 0x80038980 -> 0x80038980
    inline bVector3::bVector3() {}

    // Range: 0x80038980 -> 0x80038980
    inline void eUnSwizzleWorldVector(const struct bVector3 & inVec, struct bVector3 & outVec) {
        // Range: 0x80038980 -> 0x80038980
```

**Purpose:** Retrieves the forward direction vector from the camera

---

### 5. RespawnAvailable: AISpawnManager::RespawnAvailable(UMath::Vector3 const &, float)

**Mangled Name:** `RespawnAvailable__14AISpawnManagerRCQ25UMath7Vector3f`
**GC Address:** 0x800389D0
**PS2 Address:** 0x132d10
**Size:** 284 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x800389D0):
```
// Range: 0x800389D0 -> 0x80038AEC
// this: r31
bool AISpawnManager::RespawnAvailable(const struct UMath::Vector3 & position /* r29 */, float radius /* f1 */) {
    // Local variables
    struct UMath::Vector3 basePos; // r1+0x8
    float distToBase;

    // Range: 0x800389E0 -> 0x800389E0
    inline float UMath::Distancexz(const struct UMath::Vector3 & a, const struct UMath::Vector3 & b) {
        // Range: 0x800389E0 -> 0x800389E0
        inline float VU0_v3distancexz(const struct UMath::Vector3 & p1, const struct UMath::Vector3 & p2) {}
    }

    /* anonymous block */ {
        // Range: 0x80038A58 -> 0x80038AD4
        struct eView * view;

        // Range: 0x80038A58 -> 0x80038A58
        inline struct eView * eGetView(int view_id, bool doAssert) {}

        // Range: 0x80038A58 -> 0x80038A58
        inline int eView::IsActive() const {}
        /* anonymous block */ {
            // Range: 0x80038A68 -> 0x80038AD4
            struct UMath::Vector3 baseForwardVec; // r1+0x18
            struct UMath::Vector3 baseToPos; // r1+0x28
            float dot;

            // Range: 0x80038A68 -> 0x80038A68
            inline void UMath::Sub(const struct UMath::Vector3 & a, const struct UMath::Vector3 & b, struct UMath::Vector3 & r) {}

            // Range: 0x80038A68 -> 0x80038A68
            inline void UMath::Unit(const struct UMath::Vector3 & a, struct UMath::Vector3 & r) {
                // Range: 0x80038A68 -> 0x80038A68
                inline void VU0_v3unit(const struct UMath::Vector3 & a, struct UMath::Vector3 & result) {
                    // Local variables
                    float rlen;
                }
            }

            // Range: 0x80038A68 -> 0x80038A68
            inline float UMath::Dot(const struct UMath::Vector3 & a, const struct UMath::Vector3 & b) {}
        }
    }
}
```

#### Ghidra Decompile (GC - 0x800389D0):
```c
undefined4 RespawnAvailable__14AISpawnManagerRCQ25UMath7Vector3f(float *param_1, float *param_2)
{
  double dVar1;
  double dVar2;
  float afStack_40 [2];
  float fStack_38;
  undefined1 auStack_30 [16];
  undefined1 auStack_20 [16];
  
  GetBasePosition__14AISpawnManagerRQ25UMath7Vector3(param_1, afStack_40);
  dVar1 = (double)VU0_sqrt__Ff((double)((afStack_40[0] - *param_2) * (afStack_40[0] - *param_2) +
                                       (fStack_38 - param_2[2]) * (fStack_38 - param_2[2])));
  dVar2 = (double)*param_1;
  if (dVar2 < dVar1) {
    if ((double)((float)((double)param_1[1] + dVar2) * _UNK_803caaa4) < dVar1) {
      return 1;
    }
    if ((dVar2 < dVar1) && (cRam80461fcc != '\0')) {
      GetBaseForwardVector__14AISpawnManagerRQ25UMath7Vector3(param_1, auStack_30);
      VU0_v3sub__FRCQ25UMath7Vector3T0RQ25UMath7Vector3(param_2, afStack_40, auStack_20);
      VU0_v3lengthsquare__FRCQ25UMath7Vector3(auStack_20);
      VU0_rsqrt__Ff();
      VU0_v3scale__FRCQ25UMath7Vector3fRQ25UMath7Vector3(auStack_20, auStack_20);
      dVar1 = (double)VU0_v3dotprod__FRCQ25UMath7Vector3T0(auStack_20, auStack_30);
      if ((dVar1 <= (double)_UNK_803caaa8) && ((double)_UNK_803caaac <= dVar1)) {
        return 1;
      }
    }
  }
  return 0;
}
```

**Purpose:** Checks if a position is available for respawning within the given radius

---

### 6. GetSpawnPointOnSegment: AISpawnManager::GetSpawnPointOnSegment(short &, char &, float &)

**Mangled Name:** `GetSpawnPointOnSegment__14AISpawnManagerRsRcRf`
**GC Address:** 0x80038AEC
**PS2 Address:** 0x132e80
**Size:** 200 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x80038AEC):
```
// Range: 0x80038AEC -> 0x80038BB4
// this: r3
bool AISpawnManager::GetSpawnPointOnSegment(short & segInd /* r4 */, char & laneInd /* r28 */, float & timeStep /* r30 */) {
    // Local variables
    const struct WRoadSegment * segment; // r31
    unsigned int lastSpawnLane; // @ 0x80415484
    int lane;

    // Range: 0x80038AFC -> 0x80038AFC
    static inline struct WRoadNetwork & WRoadNetwork::Get() {}

    // Range: 0x80038AFC -> 0x80038AFC
    inline const struct WRoadSegment * WRoadNetwork::GetSegment(int index) {}

    // Range: 0x80038AFC -> 0x80038AFC
    inline float SimRandom::_SimRandom_Float() {
        // Local variables
        int r;
    }

    // Range: 0x80038AFC -> 0x80038AFC
    static inline struct WRoadNetwork & WRoadNetwork::Get() {}

    // Range: 0x80038AFC -> 0x80038B70
    inline int Max(const int a, const int b) {}

    // Range: 0x80038B70 -> 0x80038B70
    static inline struct WRoadNetwork & WRoadNetwork::Get() {}
}
```

**Purpose:** Retrieves a random spawn point on a road segment

---

### 7. GetSpawnLocation: AISpawnManager::GetSpawnLocation(short &, char &, float &)

**Mangled Name:** `GetSpawnLocation__14AISpawnManagerRsRcRf`
**GC Address:** 0x80038BB4
**PS2 Address:** 0x132f48
**Size:** 360 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x80038BB4):
```
// Range: 0x80038BB4 -> 0x80038D1C
// this: r28
bool AISpawnManager::GetSpawnLocation(short & segInd /* r31 */, char & laneInd /* r29 */, float & timeStep /* r27 */) {
    // Local variables
    int roadInd;
    struct WRoadNav testNav; // r1+0x8
    struct UMath::Vector3 checkPos; // r1+0x2F8

    // Range: 0x80038BE8 -> 0x80038BE8
    inline int SimRandom::_SimRandom_IntRange(const int range) {
        // Local variables
        int r;
    }

    // Range: 0x80038C20 -> 0x80038C20
    static inline struct WRoadNetwork & WRoadNetwork::Get() {}

    // Range: 0x80038C20 -> 0x80038C20
    inline short WRoadNetwork::GetSegRoadInd(int index) {}

    // Range: 0x80038C60 -> 0x80038C60
    inline void WRoadNav::SetNavType(enum ENavType type) {}

    // Range: 0x80038C60 -> 0x80038C60
    inline void WRoadNav::SetLaneType(enum ELaneType type) {}

    // Range: 0x80038C60 -> 0x80038C60
    inline struct UMath::Vector3 & WRoadNav::GetPosition() {}

    // Range: 0x80038C60 -> 0x80038C60
    inline bool WRoadNav::IsValid() const {}

    // Range: 0x80038CBC -> 0x80038CBC
    inline char WRoadNav::GetNodeInd() const {}
}
```

**Purpose:** Determines a valid spawn location with road navigation checks

---

### 8. CheckSpawnPosition: AISpawnManager::CheckSpawnPosition(UMath::Vector3 const &, bool, int, int, bool)

**Mangled Name:** `CheckSpawnPosition__14AISpawnManagerRCQ25UMath7Vector3biiT2`
**GC Address:** 0x80038D1C
**Size:** 660 bytes
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x80038D1C):
```
// Range: 0x80038D1C -> 0x80038FB0
// this: r31
bool AISpawnManager::CheckSpawnPosition(const struct UMath::Vector3 & checkPos /* r27 */, bool checkLane /* r29 */, int laneInd /* r26 */, int nodeInd /* r24 */, bool bCheckDist /* r30 */) {
    // Local variables
    struct UMath::Vector3 basePos; // r1+0x8
    float dist;
    struct bVector2 position2d; // r1+0x18
    struct TrackPathZone * zone;

    // Range: 0x80038D44 -> 0x80038D44
    inline float UMath::Distancexz(const struct UMath::Vector3 & a, const struct UMath::Vector3 & b) {
        // Range: 0x80038D44 -> 0x80038D44
        inline float VU0_v3distancexz(const struct UMath::Vector3 & p1, const struct UMath::Vector3 & p2) {}
    }

    // Range: 0x80038DA8 -> 0x80038DA8
    inline bVector2::bVector2(float _x, float _y) {
        // Range: 0x80038DA8 -> 0x80038DA8
        inline struct bVector2 * bFill(struct bVector2 * dest, float x, float y) {}
    }

    /* anonymous block */ {
        // Range: 0x80038DE8 -> 0x80038F80
        struct IVehicle * const * iter; // r28

        // Range: 0x80038DE8 -> 0x80038DE8
        static inline const struct List & ListableSet<IVehicle,10,eVehicleList,10>::GetList(enum eVehicleList idx) {}

        // Range: 0x80038DE8 -> 0x80038DE8
        inline struct IVehicle * const * Vector<IVehicle *,16>::begin() const {}

        // Range: 0x80038E14 -> 0x80038E14
        static inline const struct List & ListableSet<IVehicle,10,eVehicleList,10>::GetList(enum eVehicleList idx) {}

        // Range: 0x80038E14 -> 0x80038E14
        inline struct IVehicle * const * Vector<IVehicle *,16>::end() const {}
        /* anonymous block */ {
            // Range: 0x80038E2C -> 0x80038F78
            struct IVehicle * ivehicle; // r30
            struct ISimable * isimable; // r31
            struct IVehicleAI * ivehicleAI; // r11
            struct WRoadNav * otherNav; // r29
            /* anonymous block */ {
                // Range: 0x80038EB8 -> 0x80038F78
                float distance; // f31

                // Range: 0x80038EB8 -> 0x80038EB8
                inline float UMath::DistanceSquare(const struct UMath::Vector3 & a, const struct UMath::Vector3 & b) {}

                // Range: 0x80038EFC -> 0x80038EFC
                inline enum ENavType WRoadNav::GetNavType() const {}
                /* anonymous block */ {
                    // Range: 0x80038F08 -> 0x80038F70
                    const float sameLaneZoneDist;
                    const float diffLaneZoneDist;

                    // Range: 0x80038F28 -> 0x80038F28
                    inline char WRoadNav::GetLaneInd() const {}

                    // Range: 0x80038F38 -> 0x80038F38
                    inline char WRoadNav::GetToLaneInd() const {}

                    // Range: 0x80038F48 -> 0x80038F48
                    inline char WRoadNav::GetNodeInd() const {}
                }
                /* anonymous block */ {
                    // Range: 0x80038F70 -> 0x80038F78
                    const float otherCarZoneDist;
                }
            }
        }
    }
}
```

#### Key Ghidra Decompile Sections (GC - 0x80038D1C):
- Validates spawn position against vehicles and track zones
- Checks distance constraints based on parameters
- Uses TrackPathZone lookups for position validation
- Iterates through IVehicle list to check for conflicts

**Purpose:** Comprehensive position validation for spawn attempts, checking vehicle proximity and track zones

---

### 9. RefreshSpawnData: AISpawnManager::RefreshSpawnData

**Mangled Name:** `RefreshSpawnData__14AISpawnManager`
**GC Address:** 0x80038FB0
**PS2 Address:** 0x133388
**Size:** 1120 bytes (largest function)
**Status:** Missing in decomp output

#### DWARF Info (GC - 0x80038FB0):
```
// Range: 0x80038FB0 -> 0x80039410
// this: r3
void AISpawnManager::RefreshSpawnData() {
    // Local variables
    int currentSegmentIndex; // @ 0x80415488
    int spawnSegmentIndex; // @ 0x8041548C
    struct WRoadNetwork & roadNetwork;
    int nextSegmentIndex;
    float maxDistSpawn; // f30
    struct UMath::Vector3 basePos; // r1+0x8
    struct IPlayer * player; // r31
    struct UMath::Vector3 linearVelocity; // r1+0x18
    int currentSpawnIndex; // @ 0x80415490
    int nextSpawnIndex;

    // Range: 0x80038FE0 -> 0x80038FE0
    static inline struct WRoadNetwork & WRoadNetwork::Get() {}

    // Range: 0x80038FE0 -> 0x80038FE0
    inline bool WRoadNetwork::HasValidTrafficRoads() {}

    // Range: 0x80038FF0 -> 0x80038FF0
    inline void WRoadNetwork::IncSegmentStamp() {}

    // Range: 0x80038FF0 -> 0x80038FF0
    inline unsigned int WRoadNetwork::GetNumSegments() {}

    // Range: 0x80038FF0 -> 0x80039020
    inline int Min(const int a, const int b) {}

    // Range: 0x800390B4 -> 0x800390B4
    inline void UMath::Add(const struct UMath::Vector3 & a, const struct UMath::Vector3 & b, struct UMath::Vector3 & r) {}

    /* Main loop logic:
    - Iterate through road segments
    - Calculate distances to nodes
    - Populate spawn segment array
    - Track currentSegmentIndex, spawnSegmentIndex
    - Maintain mSpawnSegment array with segment indices
    */
}
```

**Purpose:** Main spawn data refresh function that updates the spawn segment list based on road network and player position

---

## Summary Statistics

| Function | Size (bytes) | Status | GC Addr | PS2 Addr |
|----------|-------------|--------|---------|----------|
| Constructor | 92 | Missing | 0x80038830 | 0x132b78 |
| Destructor | 52 | Missing | 0x8003888C | N/A |
| GetBasePosition | 136 | Missing | 0x800388C0 | 0x132c00 |
| GetBaseForwardVector | 136 | Missing | 0x80038948 | 0x132c88 |
| RespawnAvailable | 284 | Missing | 0x800389D0 | 0x132d10 |
| GetSpawnPointOnSegment | 200 | Missing | 0x80038AEC | 0x132e80 |
| GetSpawnLocation | 360 | Missing | 0x80038BB4 | 0x132f48 |
| CheckSpawnPosition | 660 | Missing | 0x80038D1C | N/A |
| RefreshSpawnData | 1120 | Missing | 0x80038FB0 | 0x133388 |
| **TOTAL** | **3,040** | All Missing | | |

---

## Key Observations

1. **Constructor Logic:** Initializes spawn points to -1, updates mMaxGatherDist, and sets up virtual table
2. **Two Main Vectors:** Uses BasePosition and BaseForwardVector retrieved from active camera view
3. **Spawn Validation:** RespawnAvailable, GetSpawnPointOnSegment, and CheckSpawnPosition work together to validate spawn points
4. **Road Network Integration:** Heavy reliance on WRoadNetwork and WRoadSegment for spawn point calculation
5. **Dynamic Spawn Management:** RefreshSpawnData continuously updates valid spawn segments based on player position
6. **Vehicle Avoidance:** CheckSpawnPosition checks proximity to all IVehicles to prevent spawning near other cars
7. **Track Zones:** Uses TrackPathZone lookups for additional spawn validation

---

## Source File Locations

The implementation file is currently empty in the repository:
- **Primary Source:** `src/Speed/Indep/Src/AI/Common/AISpawnManager.cpp` (empty)
- **Header File:** `src/Speed/Indep/Src/AI/AISpawnManager.h`
- **Related Headers:** `src/Speed/Indep/Src/AI/Common/` directory

GC Debug Line Mappings:
- Line 28: Constructor (0x80038830)
- Line 39: Destructor (0x8003888C)
- Line 43: GetBasePosition (0x800388C0)
- Line 61: GetBaseForwardVector (0x80038948)
- Line 80: RespawnAvailable (0x800389D0)
- Line 118: GetSpawnPointOnSegment (0x80038AEC)
- Line 134: GetSpawnLocation (0x80038BB4)
- Line 182: CheckSpawnPosition (0x80038D1C)
- Line 253: RefreshSpawnData (0x80038FB0)

