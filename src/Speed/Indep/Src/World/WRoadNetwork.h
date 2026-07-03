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
#include "WRoadElem.h"

#include "types.h"

extern class WRoadNetwork *fgRoadNetwork;

class WRoadNav;
struct TrackPathBarrier;
class IBody;

// total size: 0x1
class WRoadNetwork : public Debugable {
  public:
    USE_FASTALLOC(WRoadNetwork);

    static WRoadNetwork &Get() {
        return *fgRoadNetwork;
    }

    WRoadNetwork() {}

    ~WRoadNetwork() {}

    static void Init();
    static void Shutdown();
    static void Restart();
    void ResetRaceSegments();
    void ResetBarriers();
    bool WillCrossBarrier(int segment_number, int which_node, bool respect_drive_through_barriers);
    void GetSegmentNodes(const WRoadSegment &segment, const WRoadNode **node);
    const WRoadProfile *GetSegmentProfile(const WRoadSegment &segment, int node_index);
    void GetSegmentForwardVector(int segInd, UMath::Vector3 &forwardVector);
    void GetSegmentForwardVector(const WRoadSegment &segment, UMath::Vector3 &forwardVector);
    const WRoadNode *GetSegmentOppNode(int segInd, const WRoadNode *node);
    const WRoadNode *GetSegmentOppNode(const WRoadSegment &segment, const WRoadNode *node);
    uint8 GetSegmentShortcutNumber(const WRoadSegment *segment);
    bool GetSegmentTrafficLaneRightSide(const WRoadSegment &segment, int laneInd);
    int GetRightMostTrafficEntrance(int node_number, int onto_segment);
    bool GetSegmentProfiles(const WRoadSegment &segment, const WRoadProfile **profile);
    int GetSegmentNumTrafficLanes(const WRoadSegment &segment);
    int GetSegmentTrafficLaneInd(const WRoadSegment &segment, int lane_count);
    void GetSegmentEndPoints(const WRoadSegment &segment, UMath::Vector3 &start, UMath::Vector3 &end);
    void GetPointOnSegment(const WRoadSegment &segment, float d, UMath::Vector3 &point);
    void GetPointOnSegment(const UMath::Vector3 &start, const UMath::Vector3 &end, const WRoadSegment &segment, float d, UMath::Vector3 &point);
    void GetSegmentCurveStep(const UMath::Vector3 &start, const UMath::Vector3 &end, const WRoadSegment &segment, float d, UMath::Vector3 &point);
    void FlagSegmentRaceDirection(int FirstSegIndex, int SecondSegIndex);
    void AddRaceSegments(WRoadNav *road_nav);
    void ResetShortcuts();
    void ResolveShortcuts();
    void ResolveBarriers();
    void GetPointAndVecOnSegment(const WRoadSegment &segment, float d, UMath::Vector3 &point, UMath::Vector3 &vec);
    float GetSegmentPointIntersect(const WRoadSegment &segment, const UMath::Vector3 &pt, UMath::Vector3 &intersect, bool checkBound);
    float GetLinePointIntersect(const UMath::Vector3 &start, const UMath::Vector3 &end, const UMath::Vector3 &pt, UMath::Vector3 &intersect,
                                bool checkBound);
    void BuildSegmentSpline(const WRoadSegment &segment, USpline &spline);
    bool SegmentCrossesBarrier(WRoadSegment *segment, TrackPathBarrier *barrier);

    // void SetRaceFilterValid(bool b) {}

    bool IsRaceFilterValid() {
        return fValidRaceFilter;
    }

    // bool IsValid() {}

    // bool HasValidTrafficRoads() {}

    const WRoadNode *GetNode(int index) {
        return &fNodes[index];
    }

    const WRoad *GetRoad(int index) {
        return &fRoads[index];
    }

    const WRoadProfile *GetProfile(int index) {
        return &fProfiles[index];
    }

    const WRoadSegment *GetSegment(int index) {
        return &fSegments[index];
    }

    // const WRoad *GetSegmentRoad(int segment_index) {}

    WRoad *GetRoadNonConst(int index) {
        return &fRoads[index];
    }

    WRoadSegment *GetSegmentNonConst(int index) {
        return &fSegments[index];
    }

    unsigned int GetNumSegments() {
        return fNumSegments;
    }

    unsigned int GetNumRoads() {
        return fNumRoads;
    }

    // unsigned int GetNumNodes() {}

    short GetSegRoadInd(int index) {
        return fSegments[index].fRoadID;
    }

    // void IncSegmentStamp() {}

    // unsigned long GetSegmentStamp() {}

    friend class WRoadNav; // it accesses fNodes without the getter

  private:
    static WRoad *fRoads;                         // size: 0x4, address: 0x80438FDC
    static WRoadNode *fNodes;                     // size: 0x4, address: 0x80438FD0
    static WRoadProfile *fProfiles;               // size: 0x4, address: 0x80438FCC
    static WRoadSegment *fSegments;               // size: 0x4, address: 0x80438FD4
    static WRoadIntersection *fIntersections;     // size: 0x4, address: 0xFFFFFFFF
    static unsigned int fNumRoads;                // size: 0x4, address: 0x804FFF0C
    static unsigned int fNumNodes;                // size: 0x4, address: 0x804FFF10
    static unsigned int fNumProfiles;             // size: 0x4, address: 0x804FFF14
    static unsigned int fNumSegments;             // size: 0x4, address: 0x804FFEF8
    static unsigned int fNumIntersections;        // size: 0x4, address: 0x804FFF18
    static WRoadProfile fInvalidProfile;          // size: 0x40, address: 0x804F4248
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

// total size: 0x2F0
class WRoadNav {
  public:
    enum ENavType {
        kTypeNone = 0,
        kTypeTraffic = 1,
        kTypeDirection = 2,
        kTypePath = 3,
    };

    enum EPathType {
        kPathCop = 0,
        kPathNone = 1,
        kPathRacer = 2,
        kPathGPS = 3,
        kPathPlayer = 4,
        kPathChopper = 5,
        kPathRaceRoute = 6,
    };

    enum ELaneType {
        kLaneRacing = 0,
        kLaneTraffic = 1,
        kLaneDrag = 2,
        kLaneCop = 3,
        kLaneCopReckless = 4,
        kLaneReset = 5,
        kLaneStartingGrid = 6,
        kLaneAny = 7,
        kLaneMax = 8,
    };

    USE_FASTALLOC(WRoadNav);

    WRoadNav();
    virtual ~WRoadNav();
    void SetCookieTrail(CookieTrail<NavCookie, 32> *p_cookies);
    void SetCookieTrail(bool b);
    float CookieTrailCurvature(const UMath::Vector3 &car_position, const UMath::Vector3 &car_velocity);

    void SetPathType(EPathType type);
    void Reset();
    bool IsPathSegment(uint16 path_segment) const;
    bool OnPath() const;
    float GetSegmentCentreShift(int segment_number, int which_node);
    short GetNextOffset(const UMath::Vector3 &to, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos);
    short GetNextTraffic(const UMath::Vector3 &to, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos);

    void SnapToSelectableLane();
    float SnapToSelectableLane(float input_offset);
    float SnapToSelectableLane(float input_offset, int segment_no, char node_index);

    void InitAtPoint(const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane, float dirWeight);
    void InitFromOtherNav(WRoadNav *other_nav, bool flip_direction);
    void InitLaneOffset(const UMath::Vector3 &vehicle_pos);
    void InitAtSegment(short segInd, char laneInd, float timeStep);
    void InitAtSegment(short segInd, float timeStep, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane);
    void InitAtSegment(short segInd, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane);

    bool CanTrafficSpawn();
    void IncNavPosition(float dist, const UMath::Vector3 &to, float max_lookahead);
    bool IsOnShortcut();
    uint8 GetShortcutNumber();
    bool IsOnLegalRoad();
    bool MakeShortcutDecision(int shortcut_number, uint32 *cached, uint32 *allowed);

    void PullOver();
    void SetVehicle(class AIVehicle *ai_vehicle);
    void UpdateOccludedPosition(bool occlude_avoidables);
    void ChangeDragLanes(int left_right);
    void SetStartEndControls(const WRoadSegment &segment);
    void SetControlPos(const WRoadSegment &segment, bool is_start);

    bool IsValid() const {
        return fValid;
    }

    UMath::Vector3 &GetPosition() {
        return fPosition;
    }

    const UMath::Vector3 &GetRightPosition() const {
        return fRightPosition;
    }

    const UMath::Vector3 &GetLeftPosition() const {
        return fLeftPosition;
    }

    UMath::Vector3 &GetForwardVector() {
        return fForwardVector;
    }

    float GetCurvature() {
        return fCurvature;
    }

    const NavCookie &GetCurrentCookie() {
        return mCurrentCookie;
    }

    bool GetRaceFilter() const {
        return bRaceFilter && WRoadNetwork::Get().IsRaceFilterValid();
    }

    void SetRaceFilter(bool b) {
        bRaceFilter = b && WRoadNetwork::Get().IsRaceFilterValid();
    }

    bool GetTrafficFilter() const {
        return bTrafficFilter;
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

    EPathType GetPathType() const {
        return fPathType;
    }

    bool RespectFullBarriers() {
        return fPathType != kPathCop && fPathType != kPathChopper;
    }

    bool RespectDriveThroughBarriers() {
        return fPathType == kPathRacer || fPathType == kPathPlayer || fPathType == kPathGPS || fPathType == kPathRaceRoute;
    }

    ENavType GetNavType() const {
        return fNavType;
    }

    void SetNavType(ENavType type) {
        fNavType = type;
    }

    ELaneType GetLaneType() const {
        return fLaneType;
    }

    void SetLaneType(ELaneType type) {
        fLaneType = type;
    }

    bool GetDecisionFilter() {
        return bDecisionFilter;
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
        return bCookieTrail && (nRoadOcclusion != 0 || nAvoidableOcclusion != 0);
    }

    bool IsOccludedFromBehind() const {
        return IsOccludedByAvoidable() != 0 && bOccludedFromBehind;
    }

    const WRoadSegment *GetSegment() const {
        return WRoadNetwork::Get().GetSegment(fSegmentInd);
    }

    short GetSegmentInd() const {
        return fSegmentInd;
    }

    short GetRoadInd() const {
        return WRoadNetwork::Get().GetSegRoadInd(fSegmentInd);
    }

    char GetNodeInd() const {
        return fNodeInd;
    }

    char HitDeadEnd() const {
        return fDeadEnd;
    }

    float GetSegmentTime() const {
        return fSegTime;
    }

    char GetLaneInd() const {
        return fLaneInd;
    }

    void SetLaneInd(char ind) {
        fToLaneInd = ind;
        fLaneInd = ind;
    }

    float GetLaneOffset() const {
        return fLaneOffset;
    }

    void SetLaneOffset(float offset) {
        fLaneOffset = offset;
        fToLaneOffset = offset;
        fFromLaneOffset = offset;
    }

    float GetOutOfBounds() {
        return mOutOfBounds;
    }

    void DetermineDragLane() {
        ChangeDragLanes(0);
    }

    bool HasCookieTrail() const {
        return pCookieTrail != nullptr;
    }

    void ResetCookieTrail();

    AIVehicle *GetVehicle() {
        return pAIVehicle;
    }

    float GetVehicleHalfWidth() {
        return fVehicleHalfWidth;
    }

    void ChangeLanes(float newOffset, float dist);
    bool IncLane(int direction);
    bool UpdateLaneChange(float distance);
    void InitAtPath(const UMath::Vector3 &position, bool forceCenterLane);
    float FindClosestOnSpline(const UMath::Vector3 &point, UMath::Vector3 &intersectPoint, float &timeStep, float incStep, int segInd);

    void DetermineVehicleHalfWidth();
    void SetBoundPos(const WRoadSegment &segment, float offset, bool start);
    void SetStartEndPos(const WRoadSegment &segment, float startOffset, float endOffset);

    void SetStartEndPos(const WRoadSegment &segment, float offset) {
        SetStartEndPos(segment, offset, offset);
    }

    int FindClosestSegmentInd(const UMath::Vector3 &point, const UMath::Vector3 &dir, float dirWeight, UMath::Vector3 &closestPoint, float &time);
    unsigned int GetRoadSpeechId();

    bool FindPath(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed);
    bool FindPathNow(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed);
    bool FindingPath();
    bool FindClosestOnPath(const UMath::Vector3 &position, UMath::Vector3 *found_position, UMath::Vector3 *found_direction,
                           unsigned short *found_segment, float *found_interval) const;
    void CancelPathFinding();
    bool IsSegmentInPath(int segment_number);
    bool IsSegmentInCookieTrail(int segment_number, bool use_whole_path);
    uint8 FirstShortcutInPath();
    void Reverse();

    bool HasPath() const {
        if (pPathSegments == nullptr) {
            return false;
        }
        return nPathSegments > 0;
    }

    unsigned short GetPathSegment(int n) {
        return pPathSegments[n];
    }

    unsigned short *GetPathSegments() {
        return pPathSegments;
    }

    void SetNumPathSegments(int n) {
        nPathSegments = n;
    }

    int GetNumPathSegments() {
        return nPathSegments;
    }

    int GetMaxPathSegments() {
        return 510;
    }

    bool IsWrongWay() const;

    float GetPathDistanceRemaining();

    void SetPathGoal(unsigned short segment_number, float param) {
        bCrossedPathGoal = false;
        nPathGoalSegment = segment_number;
        fPathGoalParam = param;
    }

    bool IsGoalInCookieTrail() {}

    bool IsPointInCookieTrail(const UMath::Vector3 &position_3d, float margin);
    void RebuildSplines(const WRoadSegment *segment);
    void EvaluateSplines(const WRoadSegment *segment);

  private:
    bool IsDrivable(int lane_type) const;
    bool IsSelectable(int lane_type) const;
    void ClearCookieTrail();
    void UpdateCookieTrail(float time);
    void MaybeAllocatePathSegments();
    bool ChangeDragDecision(int left_right);
    void PrivateIncNavPosition(float dist, const UMath::Vector3 &to);
    void ClampCookieCentres(NavCookie *cookies, int num_cookies);
    int FetchAvoidables(IBody **avoidables, const int listsize) const;
    void HolePunchAvoidables(NavCookie *cookies, int num_cookies, float current_offset, float delta_offset);
    bool CookieCutter(NavCookie &cookie, const UMath::Vector3 &centre, float projection, bool pass_left, uint32 cut_flags);
    int ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *interpolated_cookie);
    int ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *cookies, int num_cookies, NavCookie *interpolated_cookie);

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
    AIVehicle *pAIVehicle;                      // offset 0x90, size 0x4
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
    uint16 nPathGoalSegment;                    // offset 0x2DC, size 0x2
    float fPathGoalParam;                       // offset 0x2E0, size 0x4
    int nPathSegments;                          // offset 0x2E4, size 0x4
    uint16 *pPathSegments;                      // offset 0x2E8, size 0x4
};

// total size: 0xB00
class WRoadNavWithCookies : public WRoadNav {
  public:
    WRoadNavWithCookies() {
        SetCookieTrail(&mCookieTrail);
    }

    // Overrides: WRoadNav
    ~WRoadNavWithCookies() override {
        SetCookieTrail((CookieTrail<NavCookie, 32> *)nullptr);
    }

  private:
    CookieTrail<NavCookie, 32> mCookieTrail; // offset 0x2F0, size 0x810
};

const WRoadSegment *GetAttachedDirectionalSegment(const WRoadNode *node, short segInd);

#endif
