#ifndef WORLD_WROADNETWORK_H
#define WORLD_WROADNETWORK_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/USpline.h"
#include "Speed/Indep/Src/Debug/Debugable.h"
#include "Speed/Indep/Src/Misc/CookieTrail.h"

#include "types.h"

extern class WRoadNetwork *fgRoadNetwork;

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

    static WRoadNetwork &Get() {
        return *fgRoadNetwork;
    }

    // static unsigned int GetTotalMemoryUsage() {}

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

    void *operator new(size_t size) {
        return gFastMem.Alloc(size, nullptr);
    }

    WRoadNav();
    virtual ~WRoadNav();
    void SetCookieTrail(CookieTrail<NavCookie, 32> *p_cookies);
    void SetCookieTrail(bool b);
    void ClearCookieTrail();
    void ResetCookieTrail();
    int ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *interpolated_cookie);
    int ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *cookies, int num_cookies, NavCookie *interpolated_cookie);
    bool CookieCutter(NavCookie &cookie, const UMath::Vector3 &centre, float projection, bool pass_left, unsigned int cut_flags);
    float CookieTrailCurvature(const UMath::Vector3 &car_position, const UMath::Vector3 &car_velocity);

    void MaybeAllocatePathSegments();
    void SetPathType(EPathType type);
    void Reset();
    bool IsPathSegment(unsigned short path_segment) const;
    bool OnPath() const;
    float GetSegmentCentreShift(int segment_number, int which_node);
    short GetNextOffset(const UMath::Vector3 &to, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos);

    void SnapToSelectableLane();
    float SnapToSelectableLane(float input_offset);
    float SnapToSelectableLane(float input_offset, int segment_no, char node_index);

    void InitAtPoint(const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane, float dirWeight);
    void InitFromOtherNav(WRoadNav *other_nav, bool flip_direction);
    void InitAtSegment(short segInd, char laneInd, float timeStep);
    void InitAtSegment(short segInd, float timeStep, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane);
    void InitAtSegment(short segInd, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane);

    bool CanTrafficSpawn();
    void Reverse();
    void IncNavPosition(float dist, const UMath::Vector3 &to, float max_lookahead);
    void PrivateIncNavPosition(float dist, const UMath::Vector3 &to);
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
    bool IsPointInCookieTrail(const UMath::Vector3 &position_3d, float margin);
    bool IsSegmentInCookieTrail(int segment_number, bool use_whole_path);
    bool IsSegmentInPath(int segment_number);
    void PullOver();
    void SetVehicle(class AIVehicle *ai_vehicle);
    void UpdateOccludedPosition(bool occlude_avoidables);

    bool IsValid() {
        return fValid;
    }

    UMath::Vector3 &GetPosition() {
        return fPosition;
    }

    UMath::Vector3 &GetForwardVector() {
        return fForwardVector;
    }

    const NavCookie &GetCurrentCookie() {
        return mCurrentCookie;
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

    void SetLaneType(ELaneType type) {
        fLaneType = type;
    }

    void SetDecisionFilter(bool b) {
        bDecisionFilter = b;
    }

    int IsOccludedByAvoidable() const {
        return bCookieTrail ? nAvoidableOcclusion : 0;
    }

    const UMath::Vector3 &GetOccludedPosition() const {
        return IsOccluded() ? fOccludedPosition : fPosition;
    }

    bool IsOccluded() const {
        return bOccludedFromBehind;
    }

  private:                                      // total size: 0x2F0
    int nCookieIndex;                           // offset 0x0, size 0x4
    CookieTrail<NavCookie, 32> *pCookieTrail;   // offset 0x4, size 0x4
    NavCookie mCurrentCookie;                   // offset 0x8, size 0x40
    float mOutOfBounds;                         // offset 0x48, size 0x4
    bool fValid;                                // offset 0x4C, size 0x1
    bool bRaceFilter;                           // offset 0x50, size 0x1
    bool bTrafficFilter;                        // offset 0x54, size 0x1
    bool bCopFilter;                            // offset 0x58, size 0x1
    bool bDecisionFilter;                       // offset 0x5C, size 0x1
    bool bCookieTrail;                          // offset 0x60, size 0x1
    int nRoadOcclusion;                         // offset 0x64, size 0x4
    int nAvoidableOcclusion;                    // offset 0x68, size 0x4
    bool bOccludedFromBehind;                   // offset 0x6C, size 0x1
    float fOccludingTrailSpeed;                 // offset 0x70, size 0x4
    bVector2 vCookieTrailBoxMin;                // offset 0x74, size 0x8
    bVector2 vCookieTrailBoxMax;                // offset 0x7C, size 0x8
    ENavType fNavType;                          // offset 0x84, size 0x4
    EPathType fPathType;                        // offset 0x88, size 0x4
    ELaneType fLaneType;                        // offset 0x8C, size 0x4
    struct AIVehicle *pAIVehicle;               // offset 0x90, size 0x4
    float fVehicleHalfWidth;                    // offset 0x94, size 0x4
    char fNodeInd;                              // offset 0x98, size 0x1
    short fSegmentInd;                          // offset 0x9A, size 0x2
    float fSegTime;                             // offset 0x9C, size 0x4
    float fCurvature;                           // offset 0xA0, size 0x4
    ALIGN_16 UMath::Vector3 fPosition;          // offset 0xA4, size 0xC
    ALIGN_16 UMath::Vector3 fLeftPosition;      // offset 0xB0, size 0xC
    ALIGN_16 UMath::Vector3 fRightPosition;     // offset 0xBC, size 0xC
    ALIGN_16 UMath::Vector3 fForwardVector;     // offset 0xC8, size 0xC
    ALIGN_16 UMath::Vector3 fEndPos;            // offset 0xD4, size 0xC
    ALIGN_16 UMath::Vector3 fStartPos;          // offset 0xE0, size 0xC
    ALIGN_16 UMath::Vector3 fEndControl;        // offset 0xEC, size 0xC
    ALIGN_16 UMath::Vector3 fStartControl;      // offset 0xF8, size 0xC
    ALIGN_16 UMath::Vector3 fLeftEndPos;        // offset 0x104, size 0xC
    ALIGN_16 UMath::Vector3 fLeftStartPos;      // offset 0x110, size 0xC
    ALIGN_16 UMath::Vector3 fLeftEndControl;    // offset 0x11C, size 0xC
    ALIGN_16 UMath::Vector3 fLeftStartControl;  // offset 0x128, size 0xC
    ALIGN_16 UMath::Vector3 fRightEndPos;       // offset 0x134, size 0xC
    ALIGN_16 UMath::Vector3 fRightStartPos;     // offset 0x140, size 0xC
    ALIGN_16 UMath::Vector3 fRightEndControl;   // offset 0x14C, size 0xC
    ALIGN_16 UMath::Vector3 fRightStartControl; // offset 0x158, size 0xC
    ALIGN_16 UMath::Vector3 fApexPosition;      // offset 0x164, size 0xC
    ALIGN_16 UMath::Vector3 fOccludedPosition;  // offset 0x170, size 0xC
    USpline fRoadSpline;                        // offset 0x17C, size 0x6C
    USpline fLeftSpline;                        // offset 0x1E8, size 0x6C
    USpline fRightSpline;                       // offset 0x254, size 0x6C
    char fDeadEnd;                              // offset 0x2C0, size 0x1
    char fLaneInd;                              // offset 0x2C1, size 0x1
    char fFromLaneInd;                          // offset 0x2C2, size 0x1
    char fToLaneInd;                            // offset 0x2C3, size 0x1
    float fLaneOffset;                          // offset 0x2C4, size 0x4
    float fFromLaneOffset;                      // offset 0x2C8, size 0x4
    float fToLaneOffset;                        // offset 0x2CC, size 0x4
    float fLaneChangeDist;                      // offset 0x2D0, size 0x4
    float fLaneChangeInc;                       // offset 0x2D4, size 0x4
    bool bCrossedPathGoal;                      // offset 0x2D8, size 0x1
    unsigned short nPathGoalSegment;            // offset 0x2DC, size 0x2
    float fPathGoalParam;                       // offset 0x2E0, size 0x4
    int nPathSegments;                          // offset 0x2E4, size 0x4
    unsigned short *pPathSegments;              // offset 0x2E8, size 0x4
};

// TODO right place?
enum RoadNames {
    untagged = 0,
    on_Highway99 = 1,
    thru_Campus_Interchange = 2,
    on_Kilgore_Bridge = 3,
    thru_Camden_Interchange = 4,
    thru_Petersburg_Interchange = 5,
    thru_Stadium_Interchange = 6,
    past_Ironwood_Estates = 7,
    past_Hillcrest = 8,
    on_Hillcrest_Drive = 9,
    on_Union_Row = 10,
    past_Rosewood_Park = 11,
    on_Clubhouse_Road = 12,
    on_Campus_Circle = 13,
    on_Campus_Way = 14,
    on_Chancellor_Way = 15,
    past_Rosewood_College_Hospital = 16,
    past_Rosewood_College_Research_Center = 17,
    past_Diamond_Park = 18,
    on_State_Street = 19,
    on_Hollis_Blvd = 20,
    on_Rockridge_Drive = 21,
    past_Heritage_Heights = 22,
    past_Highlander_Stadium = 23,
    past_Hickley_Field = 24,
    on_Stadium_Blvd = 25,
    past_Forest_Green_Country_Club = 26,
    past_Rosewood_Bus_Station = 27,
    on_Boundary_Road = 28,
    past_Boundary_Mall = 29,
    on_Riverside_Drive = 30,
    on_Bristol_Road = 31,
    on_North_Bay_Road = 32,
    on_Cannery_Way = 33,
    through_Dunwich_Village = 34,
    on_Fisher_Road = 35,
    through_North_Bay = 36,
    past_North_Bay_Cannery = 37,
    over_Seagate_Bridge = 38,
    on_Route_55 = 39,
    on_Chase_Road = 40,
    past_Asylum = 41,
    past_Horn = 42,
    past_Camden_Beach = 43,
    on_Coast_Road = 44,
    past_Ocean_Hills = 45,
    on_Ocean_Hills_Drive = 46,
    thru_Boardwalk = 47,
    over_Seaside_bridge = 48,
    on_Seaside_Highway = 49,
    past_Beacon_Point = 50,
    past_Beacon_Point_Marina = 51,
    on_Harbour_Road = 52,
    past_Shipyard = 53,
    thru_Camden_Tunnel = 54,
    over_Bay_Bridge = 55,
    on_Camden_Tunnel_Road = 56,
    on_Bayshore_Blvd = 57,
    over_Terminal_Bridge = 58,
    past_Omega_Power_Station = 59,
    past_Penitentiary = 60,
    by_Smugglers_Wharf = 61,
    past_Omega_Industries = 62,
    past_Point_Camden_trainyard = 63,
    on_Highway_201 = 64,
    thru_Valley_Interchange = 65,
    thru_Lyons_Tunnel = 66,
    over_Ironhorse_Bridge = 67,
    on_Hastings_Road = 68,
    on_Warrant_Road = 69,
    on_Lennox_Road = 70,
    near_Grand_Terrace = 71,
    past_Projects = 72,
    on_Bond_Blvd = 73,
    past_City_Park = 74,
    past_Fairmount_Bowl = 75,
    past_Riverfront_Stadium = 76,
    through_Little_Italy = 77,
    thru_Financial_District = 78,
    through_Downtown = 79,
    through_Century_Square = 80,
    Seaside_Interchange = 81,
    on_I17 = 82,
    thru_Student_Housing = 83,
    thru_Skyview_Interchange = 84,
    over_Beacon_Bridge = 85,
    past_Cascade_Park = 86,
    on_Waterfront_Road = 87,
    offroad_Gas_Station = 88,
    offroad_Campus = 89,
    offroad_Golf_Course = 90,
    offroad_Hospital = 91,
    offroad_Strip_Mall = 92,
    offroad_Stadium = 93,
    offroad_Park = 94,
    offroad_Trailer_Park = 95,
    offroad_Junkyard = 96,
    offroad_Boatyard = 97,
    offroad_Refinery = 98,
    offroad_Trainyard = 99,
    offroad_Boardwalk = 100,
    offroad_Beach = 101,
    offroad_Subway = 102,
    offroad_Hotel = 103,
    offroad_Museum = 104,
    offroad_Police_Station = 105,
    offroad_Hydro_Plant = 106,
    MAX_ROADNAMES = 107,
};

#endif
