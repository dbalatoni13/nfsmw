#include "Speed/Indep/Src/World/WRoadNetwork.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/Src/World/WPathFinder.h"

static const int drivable_lanes[8] = {
    static_cast<int>(0xFFFFDF7F),
    0x00000002,
    static_cast<int>(0xFFFFDF7F),
    static_cast<int>(0xFFFFDF7B),
    static_cast<int>(0xFFFFDF7F),
    0x00000402,
    static_cast<int>(0xFFFFDF7F),
    static_cast<int>(0xFFFFFFFF),
};

static const int selectable_lanes[8] = {
    0x00000402,
    0x00000002,
    static_cast<int>(0xFFFFDF5B),
    0x00000472,
    static_cast<int>(0xFFFFDF7F),
    0x00000402,
    0x00000402,
    static_cast<int>(0xFFFFFFFF),
};

void WRoadNetwork::Shutdown() {
    if (fgRoadNetwork) {
        gFastMem.Free(fgRoadNetwork, sizeof(WRoadNetwork), nullptr);
        fgRoadNetwork = nullptr;
    }
}

void WRoadNetwork::ResetRaceSegments() {
    fValidRaceFilter = false;
    for (int i = 0; i < static_cast<int>(fNumSegments); i++) {
        GetSegmentNonConst(i)->SetInRace(false);
        GetSegmentNonConst(i)->SetRaceRouteForward(false);
    }
}

void WRoadNetwork::ResetBarriers() {
    for (unsigned int segment_number = 0; segment_number < fNumSegments; segment_number++) {
        WRoadSegment *segment = GetSegmentNonConst(segment_number);
        segment->SetCrossesDriveThroughBarrier(false);
        segment->SetCrossesBarrier(false);
    }
}

void WRoadNetwork::GetSegmentNodes(const WRoadSegment &segment, const WRoadNode **node) {
    WRoadNetwork &roadNetwork = Get();
    node[0] = roadNetwork.GetNode(segment.fNodeIndex[0]);
    node[1] = roadNetwork.GetNode(segment.fNodeIndex[1]);
}

const WRoadProfile *WRoadNetwork::GetSegmentProfile(const WRoadSegment &segment, int node_index) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadNode *node = roadNetwork.GetNode(segment.fNodeIndex[node_index]);
    if (node) {
        if (node->fProfileIndex < 0) {
            return &fInvalidProfile;
        }
        return roadNetwork.GetProfile(node->fProfileIndex);
    }
    return &fInvalidProfile;
}

void WRoadNetwork::GetSegmentForwardVector(int segInd, UMath::Vector3 &forwardVector) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);
    roadNetwork.GetSegmentForwardVector(*segment, forwardVector);
}

const WRoadNode *WRoadNetwork::GetSegmentOppNode(int segInd, const WRoadNode *node) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);
    return roadNetwork.GetSegmentOppNode(*segment, node);
}

const WRoadNode *WRoadNetwork::GetSegmentOppNode(const WRoadSegment &segment, const WRoadNode *node) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadNode *nodePtr[2];
    roadNetwork.GetSegmentNodes(segment, nodePtr);
    if (node == nodePtr[0]) {
        return nodePtr[1];
    }
    return nodePtr[0];
}

void WRoadNav::SetCookieTrail(CookieTrail<NavCookie, 32> *p_cookies) {
    pCookieTrail = p_cookies;
    bCookieTrail = (p_cookies != nullptr);
}

void WRoadNav::SetCookieTrail(bool b) {
    if (b && pCookieTrail == nullptr) {
        pCookieTrail = new CookieTrail<NavCookie, 32>();
    }
    bCookieTrail = b;
}

void WRoadNav::ClearCookieTrail() {
    if (pCookieTrail) {
        pCookieTrail->Clear();
    }
    nCookieIndex = 0;
}

void WRoadNav::ResetCookieTrail() {
    ClearCookieTrail();
    UpdateCookieTrail(3.0f);
}

void WRoadNav::MaybeAllocatePathSegments() {
    if (pPathSegments == nullptr) {
        pPathSegments = new unsigned short[0x3FC / sizeof(unsigned short)];
    }
}

void WRoadNav::SetPathType(EPathType type) {
    fPathType = type;
}

void WRoadNav::SetVehicle(AIVehicle *ai_vehicle) {
    pAIVehicle = ai_vehicle;
    DetermineVehicleHalfWidth();
}

bool WRoadNav::IsDrivable(int lane_type) const {
    return (drivable_lanes[fLaneType] >> lane_type) & 1;
}

bool WRoadNav::IsSelectable(int lane_type) const {
    return (selectable_lanes[fLaneType] >> lane_type) & 1;
}

void WRoadNav::SnapToSelectableLane() {
    float offset = SnapToSelectableLane(fLaneOffset);
    ChangeLanes(offset, 0.0f);
}

float WRoadNav::SnapToSelectableLane(float input_offset) {
    return SnapToSelectableLane(input_offset, fSegmentInd, fNodeInd);
}

int WRoadNav::ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *interpolated_cookie) {
    return ClosestCookieAhead(position, nullptr, pCookieTrail->Count(), interpolated_cookie);
}

void WRoadNav::SetStartEndControls(const WRoadSegment &segment) {
    SetControlPos(segment, true);
    SetControlPos(segment, false);
}

bool WRoadNav::FindingPath() {
    PathFinder *path_finder = PathFinder::Get();
    return path_finder != nullptr && path_finder->Pending(this) != nullptr;
}

bool WRoadNav::IsSegmentInPath(int segment_number) {
    if (GetNavType() == kTypePath) {
        int num_segments = GetNumPathSegments();
        for (int i = 0; i < num_segments; i++) {
            if (segment_number == GetPathSegment(i)) {
                return true;
            }
        }
    }
    return false;
}

unsigned char WRoadNetwork::GetSegmentShortcutNumber(const WRoadSegment *segment) {
    if (segment->IsShortcut() && segment->fRoadID != -1) {
        return GetRoad(segment->fRoadID)->nShortcut;
    }
    return 0xFF;
}

void WRoadNav::CancelPathFinding() {
    PathFinder *path_finder = PathFinder::Get();
    if (path_finder != nullptr) {
        path_finder->Cancel(this);
    }
    if (GetNavType() == kTypePath) {
        SetNavType(kTypeDirection);
    }
}

void WRoadNetwork::GetSegmentEndPoints(const WRoadSegment &segment, UMath::Vector3 &start, UMath::Vector3 &end) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadNode *nodePtr[2];
    roadNetwork.GetSegmentNodes(segment, nodePtr);
    start = nodePtr[0]->fPosition;
    end = nodePtr[1]->fPosition;
}

void WRoadNetwork::GetSegmentForwardVector(const WRoadSegment &segment, UMath::Vector3 &forwardVector) {
    const WRoadNode *nodes[2];
    GetSegmentNodes(segment, nodes);
    UMath::Vector3 v = UVector3(nodes[1]->fPosition) - nodes[0]->fPosition;
    UMath::Unit(v, forwardVector);
}

void WRoadNetwork::GetPointOnSegment(const WRoadSegment &segment, float d, UMath::Vector3 &point) {
    WRoadNetwork &roadNetwork = Get();
    if (d > 1.0f) {
        d = 1.0f;
    }
    if (d < 0.0f) {
        d = 0.0f;
    }
    UMath::Vector3 start;
    UMath::Vector3 end;
    roadNetwork.GetSegmentEndPoints(segment, start, end);
    GetPointOnSegment(start, end, segment, d, point);
}

void WRoadNetwork::GetPointOnSegment(const UMath::Vector3 &start, const UMath::Vector3 &end, const WRoadSegment &segment, float d, UMath::Vector3 &point) {
    if (segment.IsCurved()) {
        GetSegmentCurveStep(start, end, segment, d, point);
        return;
    }
    point.x = start.x + (end.x - start.x) * d;
    point.y = start.y + (end.y - start.y) * d;
    point.z = start.z + (end.z - start.z) * d;
}

bool WRoadNetwork::GetSegmentProfiles(const WRoadSegment &segment, const WRoadProfile **profile) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadNode *node[2];
    node[0] = roadNetwork.GetNode(segment.fNodeIndex[0]);
    node[1] = roadNetwork.GetNode(segment.fNodeIndex[1]);
    if (node[0] == nullptr || node[1] == nullptr || node[0]->fProfileIndex < 0 || node[1]->fProfileIndex < 0) {
        profile[0] = &fInvalidProfile;
        profile[1] = &fInvalidProfile;
        return false;
    }
    profile[0] = roadNetwork.GetProfile(node[0]->fProfileIndex);
    profile[1] = roadNetwork.GetProfile(node[1]->fProfileIndex);
    return true;
}

int WRoadNetwork::GetSegmentNumTrafficLanes(const WRoadSegment &segment) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadProfile *profilePtr[2];
    int numTrafficLanes[2];
    numTrafficLanes[0] = 0;
    roadNetwork.GetSegmentProfiles(segment, profilePtr);
    numTrafficLanes[1] = 0;
    for (int i = 0; i < profilePtr[0]->fNumZones; i++) {
        if (profilePtr[0]->GetLaneType(i, false) == 1) {
            numTrafficLanes[0]++;
        }
    }
    for (int i = 0; i < profilePtr[1]->fNumZones; i++) {
        if (profilePtr[1]->GetLaneType(i, false) == 1) {
            numTrafficLanes[1]++;
        }
    }
    return UMath::Max(numTrafficLanes[0], numTrafficLanes[1]);
}

int WRoadNetwork::GetSegmentTrafficLaneInd(const WRoadSegment &segment, int lane_count) {
    const WRoadProfile *profile[2];
    Get().GetSegmentProfiles(segment, profile);
    for (int i = 0; i < profile[0]->fNumZones; i++) {
        if (profile[0]->GetLaneType(i, false) == 1) {
            lane_count--;
            if (lane_count == 0) {
                return i;
            }
        }
    }
    return 0;
}

void WRoadNetwork::FlagSegmentRaceDirection(int FirstSegIndex, int SecondSegIndex) {
    WRoadSegment *FirstSeg = GetSegmentNonConst(FirstSegIndex);
    WRoadSegment *SecondSeg = GetSegmentNonConst(SecondSegIndex);
    if (FirstSeg->fNodeIndex[0] == SecondSeg->fNodeIndex[0] || FirstSeg->fNodeIndex[0] == SecondSeg->fNodeIndex[1]) {
        FirstSeg->SetRaceRouteForward(false);
    } else {
        FirstSeg->SetRaceRouteForward(true);
    }
    if (SecondSeg->fNodeIndex[0] == FirstSeg->fNodeIndex[0] || SecondSeg->fNodeIndex[0] == FirstSeg->fNodeIndex[1]) {
        SecondSeg->SetRaceRouteForward(true);
    } else {
        SecondSeg->SetRaceRouteForward(false);
    }
}

void WRoadNetwork::AddRaceSegments(WRoadNav *road_nav) {
    if (road_nav->GetNavType() != WRoadNav::kTypePath) {
        return;
    }
    int num_segments = road_nav->GetNumPathSegments();
    for (int i = 0; i < num_segments; i++) {
        GetSegmentNonConst(road_nav->GetPathSegment(i))->SetInRace(true);
        if (i + 1 < num_segments) {
            FlagSegmentRaceDirection(road_nav->GetPathSegment(i), road_nav->GetPathSegment(i + 1));
        }
    }
}

void WRoadNetwork::ResetShortcuts() {
    for (unsigned int segment_number = 0; segment_number < fNumSegments; segment_number++) {
        WRoadSegment *segment = GetSegmentNonConst(segment_number);
        segment->SetShortcut(false);
    }
    for (unsigned int road_number = 0; road_number < fNumRoads; road_number++) {
        WRoad *road = GetRoadNonConst(road_number);
        road->nShortcut = 0;
    }
}

bool WRoadNetwork::GetSegmentTrafficLaneRightSide(const WRoadSegment &segment, int laneInd) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadProfile *profilePtr[2];
    roadNetwork.GetSegmentProfiles(segment, profilePtr);
    return laneInd >= profilePtr[0]->fMiddleZone;
}

int WRoadProfile::GetNumTrafficLanes(bool forward) const {
    int num_traffic_lanes = 0;
    int num_lanes = GetNumLanes(forward);
    for (int i = 0; i < num_lanes; i++) {
        if (GetLaneType(i, !forward) == 1) {
            num_traffic_lanes++;
        }
    }
    return num_traffic_lanes;
}

int WRoadProfile::GetNthTrafficLane(int n, bool forward) const {
    int num_traffic_lanes = 0;
    int num_lanes = GetNumLanes(forward);
    int fallback = GetMiddleZone(forward);
    for (int i = 0; i < num_lanes; i++) {
        int real_lane = GetNthLane(i, forward);
        if (GetLaneType(real_lane, false) == 1) {
            if (num_traffic_lanes == n) {
                return real_lane;
            }
            num_traffic_lanes++;
            fallback = real_lane;
        }
    }
    return fallback;
}

unsigned char WRoadNav::FirstShortcutInPath() {
    if (GetNavType() == kTypePath) {
        int num_segments = GetNumPathSegments();
        for (int i = 0; i < num_segments; i++) {
            const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(GetPathSegment(i));
            if (segment->IsShortcut()) {
                const WRoad *road = WRoadNetwork::Get().GetRoad(segment->fRoadID);
                return road->nShortcut;
            }
        }
    }
    return 0xff;
}

const WRoadSegment *GetAttachedDirectionalSegment(const WRoadNode *node, short segment_index) {
    for (int i = 0; i < node->fNumSegments; i++) {
        const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(node->fSegmentIndex[i]);
        if (segment_index != segment->fIndex && !segment->IsDecision()) {
            return segment;
        }
    }
    return nullptr;
}
WRoadNav::WRoadNav() {
    bOccludedFromBehind = false;
    fOccludingTrailSpeed = 0.0f;
    pAIVehicle = nullptr;
    bRaceFilter = false;
    bTrafficFilter = false;
    bCopFilter = false;
    bDecisionFilter = false;
    pCookieTrail = nullptr;
    bCookieTrail = false;
    pPathSegments = nullptr;
    nRoadOcclusion = 0;
    nAvoidableOcclusion = 0;
    Reset();
}

WRoadNav::~WRoadNav() {
    PathFinder *path_finder = PathFinder::Get();
    if (path_finder) {
        path_finder->Cancel(this);
    }
    if (pCookieTrail) {
        delete pCookieTrail;
    }
    if (pPathSegments) {
        delete pPathSegments;
    }
}

bool WRoadNav::IsWrongWay() const {
    bool result = false;
    if (GetRaceFilter() && IsValid()) {
        const WRoadSegment *segment = GetSegment();
        if (segment->IsInRace()) {
            bool seg_foward = segment->RaceRouteForward();
            if (fNodeInd == 1) {
                if (!seg_foward) {
                    return false;
                }
            } else if (seg_foward) {
                return false;
            }
            result = true;
        }
    }
    return result;
}

unsigned int WRoadNav::GetRoadSpeechId() {
    unsigned short segment_index = GetSegmentInd();
    WRoadNetwork &road_network = WRoadNetwork::Get();
    unsigned short num_segments = road_network.GetNumSegments();
    segment_index = bClamp(static_cast<int>(segment_index), 0, static_cast<int>(num_segments) - 1);
    if (GetSegmentInd() != segment_index) {
        return 0;
    }
    const WRoadSegment *segment = road_network.GetSegment(segment_index);
    short road_index = segment->fRoadID;
    short num_roads = road_network.GetNumRoads();
    road_index = bClamp(static_cast<int>(road_index), 0, static_cast<int>(num_roads) - 1);
    if (segment->fRoadID != road_index) {
        return 0;
    }
    const WRoad *road = road_network.GetRoad(road_index);
    return road->nSpeechId;
}

bool WRoadNav::IsOnLegalRoad() {
    if (!IsValid()) {
        return false;
    }
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(segment_number);
    if (segment->IsDecision()) {
        const WRoadNode *node = WRoadNetwork::Get().GetNode(segment->fNodeIndex[GetNodeInd()]);
        segment = GetAttachedDirectionalSegment(node, segment_number);
    }
    if (segment != nullptr && segment->IsTrafficAllowed()) {
        return true;
    }
    return false;
}

bool WRoadNav::FindPath(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed) {
    PathFinder *path_finder = PathFinder::Get();
    if (path_finder != nullptr) {
        MaybeAllocatePathSegments();
        AStarSearch *search = path_finder->Pending(this);
        if (search == nullptr) {
            search = path_finder->Submit(this, goal_position, goal_direction, shortcut_allowed);
        }
        if (search != nullptr) {
            return true;
        }
    }
    return false;
}

bool WRoadNav::FindPathNow(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed) {
    bool ret = FindPath(goal_position, goal_direction, shortcut_allowed);
    if (ret) {
        PathFinder::Get()->ServiceAll();
    }
    return ret && nPathSegments > 0;
}

void WRoadNav::SetStartEndPos(const WRoadSegment &segment, float startOffset, float endOffset) {
    SetBoundPos(segment, startOffset, false);
    SetBoundPos(segment, endOffset, true);
}

void WRoadNav::InitAtPoint(const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane, float dirWeight) {
    UMath::Vector3 closestPoint;
    float time;
    int segment = FindClosestSegmentInd(pos, dir, dirWeight, closestPoint, time);
    if (segment == -1) {
        fValid = false;
        fSegmentInd = 0;
    } else {
        InitAtSegment(static_cast<short>(segment), time, pos, dir, forceCenterLane);
    }
}
