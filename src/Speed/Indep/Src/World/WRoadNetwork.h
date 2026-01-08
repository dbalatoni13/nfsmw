#ifndef WORLD_WROADNETWORK_H
#define WORLD_WROADNETWORK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/USpline.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Misc/CookieTrail.h"

#include "types.h"

// total size: 0x1
class WRoadNetwork : public Debugable {
  public:
    // static inline void *operator new(unsigned int size, void *ptr) {}

    // static inline void operator delete(void *mem, void *ptr) {}

    // static inline void *operator new(unsigned int size) {}

    // static inline void operator delete(void *mem, unsigned int size) {}

    // static inline void *operator new(unsigned int size, const char *name) {}

    // static inline void operator delete(void *mem, const char *name) {}

    // static inline void operator delete(void *mem, unsigned int size, const char *name) {}

    static WRoadNetwork &Get() {}

    static unsigned int GetTotalMemoryUsage() {}

    WRoadNetwork() {}

    ~WRoadNetwork() {}

    // void SetRaceFilterValid(bool b) {}

    bool IsRaceFilterValid() {
        return fValidRaceFilter;
    }

    // bool IsValid() {}

    // bool HasValidTrafficRoads() {}

    // const struct WRoadNode *GetNode(int index) {}

    // const struct WRoad *GetRoad(int index) {}

    // const struct WRoadProfile *GetProfile(int index) {}

    // const struct WRoadSegment *GetSegment(int index) {}

    // const struct WRoad *GetSegmentRoad(int segment_index) {}

    // struct WRoad *GetRoadNonConst(int index) {}

    // struct WRoadSegment *GetSegmentNonConst(int index) {}

    // unsigned int GetNumRoads() {}

    // unsigned int GetNumNodes() {}

    // unsigned int GetNumSegments() {}

    // short GetSegRoadInd(int index) {}

    // void IncSegmentStamp() {}

    // unsigned long GetSegmentStamp() {}

  private:
    static struct WRoad *fRoads;                     // size: 0x4, address: 0x80438FDC
    static struct WRoadNode *fNodes;                 // size: 0x4, address: 0x80438FD0
    static struct WRoadProfile *fProfiles;           // size: 0x4, address: 0x80438FCC
    static struct WRoadSegment *fSegments;           // size: 0x4, address: 0x80438FD4
    static struct WRoadIntersection *fIntersections; // size: 0x4, address: 0xFFFFFFFF
    static unsigned int fNumRoads;                   // size: 0x4, address: 0x804FFF0C
    static unsigned int fNumNodes;                   // size: 0x4, address: 0x804FFF10
    static unsigned int fNumProfiles;                // size: 0x4, address: 0x804FFF14
    static unsigned int fNumSegments;                // size: 0x4, address: 0x804FFEF8
    static unsigned int fNumIntersections;           // size: 0x4, address: 0x804FFF18
    // static WRoadProfile fInvalidProfile;             // size: 0x40, address: 0x804F4248
    static bool fValid;                           // size: 0x1, address: 0x804FFF1C
    static bool fValidRaceFilter;                 // size: 0x1, address: 0x804FFEF4
    static bool fValidTrafficRoads;               // size: 0x1, address: 0x804FFF00
    static unsigned long fSegmentStamp;           // size: 0x4, address: 0x80438FE0
    static WRoadNetwork *fgRoadNetwork;           // size: 0x4, address: 0x80438FC8
    static unsigned int nRoadMemoryUsage;         // size: 0x4, address: 0x80438FE4
    static unsigned int nNodeMemoryUsage;         // size: 0x4, address: 0x80438FE8
    static unsigned int nProfileMemoryUsage;      // size: 0x4, address: 0x80438FEC
    static unsigned int nSegmentMemoryUsage;      // size: 0x4, address: 0x80438FF0
    static unsigned int nIntersectionMemoryUsage; // size: 0x4, address: 0x80438FF4
    static unsigned int nTotalMemoryUsage;        // size: 0x4, address: 0x80438FF8
};

class WRoadNav {
  public:
    enum ENavType {
        kTypeNone = 0x0000,
        kTypeTraffic = 0x0001,
        kTypeDirection = 0x0002,
        kTypePath = 0x0003,
    };

    enum EPathType {
        kPathCop = 0x0000,
        kPathNone = 0x0001,
        kPathRacer = 0x0002,
        kPathGPS = 0x0003,
        kPathPlayer = 0x0004,
        kPathPathy = 0x0005,
        kPathChopper = 0x0006,
        kPathRaceRoute = 0x0007,
    };

    enum ELaneType {
        kLaneRacing = 0x0000,
        kLaneTraffic = 0x0001,
        kLaneDrag = 0x0002,
        kLaneCop = 0x0003,
        kLaneCopReckless = 0x0004,
        kLaneReset = 0x0005,
        kLaneStartingGrid = 0x0006,
        kLaneAny = 0x0007,
        kLaneMax = 0x0008,
    };

    WRoadNav();
    virtual ~WRoadNav();
    void SetCookieTrail(CookieTrail<NavCookie, 32> *p_cookies);
    void SetCookieTrail(bool b);
    void ClearCookieTrail();
    void ResetCookieTrail();
    void MaybeAllocatePathSegments();
    // void WRoadNav::SetPathType(EPathType type);
    void Reset();
    bool IsPathSegment(unsigned short path_segment) const;
    bool OnPath() const;
    float GetSegmentCentreShift(int segment_number, int which_node);
    short GetNextOffset(const UMath::Vector3 &to, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos);
    void SnapToSelectableLane();
    float SnapToSelectableLane(float input_offset);
    float SnapToSelectableLane(float input_offset, int segment_no, char node_index);
    void Reverse();
    void InitAtPoint(const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane, float dirWeight);
    bool CanTrafficSpawn();
    void SetPathType(EPathType type);
    void IncNavPosition(float dist, const UMath::Vector3 &to, float max_lookahead);
    void PrivateIncNavPosition(float dist, const UMath::Vector3 &to);
    int ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *interpolated_cookie);
    int ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *cookies, int num_cookies, NavCookie *interpolated_cookie);
    bool CookieCutter(NavCookie &cookie, const UMath::Vector3 &centre, float projection, bool pass_left, unsigned int cut_flags);
    void ClampCookieCentres(NavCookie *cookies, int num_cookies);
    bool IsWrongWay() const;
    bool IsOnShortcut();
    unsigned char GetShortcutNumber();
    bool IsOnLegalRoad();
    bool MakeShortcutDecision(int shortcut_number, unsigned int *cached, unsigned int *allowed);
    void CancelPathFinding();
    bool FindPath(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed);
    bool FindPathNow(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed);
    bool FindingPath();
    float GetPathDistanceRemaining();
    float CookieTrailCurvature(const UMath::Vector3 &car_position, const UMath::Vector3 &car_velocity);
    bool IsPointInCookieTrail(const UMath::Vector3 &position_3d, float margin);
    bool IsSegmentInCookieTrail(int segment_number, bool use_whole_path);
    bool IsSegmentInPath(int segment_number);

    bool IsValid() {
        return fValid;
    }

    UMath::Vector3 &GetPosition() {
        return fPosition;
    }

    UMath::Vector3 &GetForwardVector() {
        return fForwardVector;
    }

    void SetRaceFilter(bool b) {
        bRaceFilter = b && WRoadNetwork::Get().IsRaceFilterValid();
    }

    void SetTrafficFilter(bool b) {
        bTrafficFilter = b;
    }

    bool GetCopFilter() const {
        return bCopFilter;
    }

    void SetCopFilter(bool b) {
        bCopFilter = b;
    }

    void SetNavType(ENavType type) {
        fNavType = type;
    }

  private:                                           // total size: 0x2F0
    int nCookieIndex;                                // offset 0x0, size 0x4
    CookieTrail<NavCookie, 32> *pCookieTrail;        // offset 0x4, size 0x4
    NavCookie mCurrentCookie;                        // offset 0x8, size 0x40
    float mOutOfBounds;                              // offset 0x48, size 0x4
    bool fValid;                                     // offset 0x4C, size 0x1
    bool bRaceFilter;                                // offset 0x50, size 0x1
    bool bTrafficFilter;                             // offset 0x54, size 0x1
    bool bCopFilter;                                 // offset 0x58, size 0x1
    bool bDecisionFilter;                            // offset 0x5C, size 0x1
    bool bCookieTrail;                               // offset 0x60, size 0x1
    int nRoadOcclusion;                              // offset 0x64, size 0x4
    int nAvoidableOcclusion;                         // offset 0x68, size 0x4
    bool bOccludedFromBehind;                        // offset 0x6C, size 0x1
    float fOccludingTrailSpeed;                      // offset 0x70, size 0x4
    bVector2 vCookieTrailBoxMin;                     // offset 0x74, size 0x8
    bVector2 vCookieTrailBoxMax;                     // offset 0x7C, size 0x8
    ENavType fNavType;                               // offset 0x84, size 0x4
    EPathType fPathType;                             // offset 0x88, size 0x4
    ELaneType fLaneType;                             // offset 0x8C, size 0x4
    struct AIVehicle *pAIVehicle;                    // offset 0x90, size 0x4
    float fVehicleHalfWidth;                         // offset 0x94, size 0x4
    char fNodeInd;                                   // offset 0x98, size 0x1
    short fSegmentInd;                               // offset 0x9A, size 0x2
    float fSegTime;                                  // offset 0x9C, size 0x4
    float fCurvature;                                // offset 0xA0, size 0x4
    ALIGN_PS2(16) UMath::Vector3 fPosition;          // offset 0xA4, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fLeftPosition;      // offset 0xB0, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fRightPosition;     // offset 0xBC, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fForwardVector;     // offset 0xC8, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fEndPos;            // offset 0xD4, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fStartPos;          // offset 0xE0, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fEndControl;        // offset 0xEC, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fStartControl;      // offset 0xF8, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fLeftEndPos;        // offset 0x104, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fLeftStartPos;      // offset 0x110, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fLeftEndControl;    // offset 0x11C, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fLeftStartControl;  // offset 0x128, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fRightEndPos;       // offset 0x134, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fRightStartPos;     // offset 0x140, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fRightEndControl;   // offset 0x14C, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fRightStartControl; // offset 0x158, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fApexPosition;      // offset 0x164, size 0xC
    ALIGN_PS2(16) UMath::Vector3 fOccludedPosition;  // offset 0x170, size 0xC
    USpline fRoadSpline;                             // offset 0x17C, size 0x6C
    USpline fLeftSpline;                             // offset 0x1E8, size 0x6C
    USpline fRightSpline;                            // offset 0x254, size 0x6C
    char fDeadEnd;                                   // offset 0x2C0, size 0x1
    char fLaneInd;                                   // offset 0x2C1, size 0x1
    char fFromLaneInd;                               // offset 0x2C2, size 0x1
    char fToLaneInd;                                 // offset 0x2C3, size 0x1
    float fLaneOffset;                               // offset 0x2C4, size 0x4
    float fFromLaneOffset;                           // offset 0x2C8, size 0x4
    float fToLaneOffset;                             // offset 0x2CC, size 0x4
    float fLaneChangeDist;                           // offset 0x2D0, size 0x4
    float fLaneChangeInc;                            // offset 0x2D4, size 0x4
    bool bCrossedPathGoal;                           // offset 0x2D8, size 0x1
    unsigned short nPathGoalSegment;                 // offset 0x2DC, size 0x2
    float fPathGoalParam;                            // offset 0x2E0, size 0x4
    int nPathSegments;                               // offset 0x2E4, size 0x4
    unsigned short *pPathSegments;                   // offset 0x2E8, size 0x4
};

#endif
