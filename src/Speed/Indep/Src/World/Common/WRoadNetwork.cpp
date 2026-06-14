#include "Speed/Indep/Src/World/WRoadNetwork.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"
#include "Speed/Indep/Libs/Support/Utility/UVector.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Gameplay/GMarker.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/IBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IArticulatedVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Physics/Common/VehicleSystem.h"
#include "Speed/Indep/Src/AI/AITarget.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/World/Common/WGrid.h"
#include "Speed/Indep/Src/World/WPathFinder.h"
#include "Speed/Indep/Src/World/WWorld.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/Sim/SimSubSystem.h"

WRoadNetwork *WRoadNetwork::fgRoadNetwork = nullptr;       // size: 0x4, address: 0x80438FC8
WRoadProfile *WRoadNetwork::fProfiles = nullptr;           // size: 0x4, address: 0x80438FCC
WRoadNode *WRoadNetwork::fNodes = nullptr;                 // size: 0x4, address: 0x80438FD0
WRoadSegment *WRoadNetwork::fSegments = nullptr;           // size: 0x4, address: 0x80438FD4
WRoad *WRoadNetwork::fRoads = nullptr;                     // size: 0x4, address: 0x80438FDC
unsigned long WRoadNetwork::fSegmentStamp = 0;             // size: 0x4, address: 0x80438FE0
unsigned int WRoadNetwork::nRoadMemoryUsage = 0;           // size: 0x4, address: 0x80438FE4
unsigned int WRoadNetwork::nNodeMemoryUsage = 0;           // size: 0x4, address: 0x80438FE8
unsigned int WRoadNetwork::nProfileMemoryUsage = 0;        // size: 0x4, address: 0x80438FEC
unsigned int WRoadNetwork::nSegmentMemoryUsage = 0;        // size: 0x4, address: 0x80438FF0
unsigned int WRoadNetwork::nIntersectionMemoryUsage = 0;   // size: 0x4, address: 0x80438FF4
unsigned int WRoadNetwork::nTotalMemoryUsage = 0;          // size: 0x4, address: 0x80438FF8
WRoadIntersection *WRoadNetwork::fIntersections = nullptr; // size: 0x4, address: 0xFFFFFFFF
unsigned int WRoadNetwork::fNumRoads;                      // size: 0x4, address: 0x804FFF0C
unsigned int WRoadNetwork::fNumNodes;                      // size: 0x4, address: 0x804FFF10
unsigned int WRoadNetwork::fNumProfiles;                   // size: 0x4, address: 0x804FFF14
unsigned int WRoadNetwork::fNumSegments;                   // size: 0x4, address: 0x804FFEF8
unsigned int WRoadNetwork::fNumIntersections;              // size: 0x4, address: 0x804FFF18
WRoadProfile WRoadNetwork::fInvalidProfile;                // size: 0x40, address: 0x804F4248
bool WRoadNetwork::fValid;                                 // size: 0x1, address: 0x804FFF1C
bool WRoadNetwork::fValidRaceFilter;                       // size: 0x1, address: 0x804FFEF4
bool WRoadNetwork::fValidTrafficRoads;                     // size: 0x1, address: 0x804FFF00

static const int drivable_lanes[8] = {
    0xFFFFDF7F, 0x00000002, 0xFFFFDF7F, 0xFFFFDF7B, 0xFFFFDF7F, 0x00000402, 0xFFFFDF7F, 0xFFFFFFFF,
};

static const int selectable_lanes[8] = {
    0x00000402, 0x00000002, 0xFFFFDF5B, 0x00000472, 0xFFFFDF7F, 0x00000402, 0x00000402, 0xFFFFFFFF,
};

BIND_SIM_SUBSYSTEM(WRoadNetwork, WRoadNetwork::Init, WRoadNetwork::Shutdown);

void WRoadNetwork::Init() {
    if (fgRoadNetwork != nullptr) {
        return;
    }
    fgRoadNetwork = new WRoadNetwork();
    fgRoadNetwork->MakeDebugable(DBG_WROADNETWORK);
    fValid = false;
    fValidRaceFilter = false;
    fValidTrafficRoads = true;

    fNumRoads = 0;
    fNumIntersections = 0;
    fNumSegments = 0;
    fNumNodes = 0;
    nTotalMemoryUsage = 0;
    nIntersectionMemoryUsage = 0;
    nSegmentMemoryUsage = 0;
    nProfileMemoryUsage = 0;
    nNodeMemoryUsage = 0;
    nRoadMemoryUsage = 0;

    if (WWorld::Get().GetMapGroup()) {
        const UGroup *networkGroup = WWorld::Get().GetMapGroup()->GroupLocate(MAKE_UDATA_TYPE('RN'), 'gp');
        if (networkGroup != WWorld::Get().GetMapGroup()->GroupEnd()) {
            const UData *headerData = networkGroup->DataLocate(MAKE_UDATA_TYPE('RN'), 'hd');
            const WRoadNetworkInfo *roadInfo = static_cast<const WRoadNetworkInfo *>(headerData->GetDataConst());

            fNumProfiles = roadInfo->fNumProfiles;
            fNumNodes = roadInfo->fNumNodes;
            fNumSegments = roadInfo->fNumSegments;
            fNumIntersections = roadInfo->fNumIntersections;
            fNumRoads = roadInfo->fNumRoads;
            nRoadMemoryUsage = fNumRoads * sizeof(WRoad);
            nNodeMemoryUsage = fNumNodes * sizeof(WRoadNode);
            nProfileMemoryUsage = fNumProfiles * sizeof(WRoadProfile);
            nSegmentMemoryUsage = fNumSegments * sizeof(WRoadSegment);
            nIntersectionMemoryUsage = fNumIntersections * sizeof(WRoadIntersection);
            nTotalMemoryUsage = nRoadMemoryUsage + nNodeMemoryUsage + nProfileMemoryUsage + nSegmentMemoryUsage + nIntersectionMemoryUsage;

            if (fNumNodes == 0) {
                fValid = false;
                return;
            }

            const UData *data = networkGroup->DataLocate(MAKE_UDATA_TYPE('RN'), 'pf');
            void *nonConstData = const_cast<void *>(data->GetDataConst());
            if (data != networkGroup->DataEnd()) {
                fProfiles = static_cast<WRoadProfile *>(nonConstData);
            }

            data = networkGroup->DataLocate(MAKE_UDATA_TYPE('RN'), 'nd');
            nonConstData = const_cast<void *>(data->GetDataConst());
            if (data != networkGroup->DataEnd()) {
                fNodes = static_cast<WRoadNode *>(nonConstData);
            }

            data = networkGroup->DataLocate(MAKE_UDATA_TYPE('RN'), 'sg');
            nonConstData = const_cast<void *>(data->GetDataConst());
            if (data != networkGroup->DataEnd()) {
                fSegments = static_cast<WRoadSegment *>(nonConstData);
            }

            data = networkGroup->DataLocate(MAKE_UDATA_TYPE('RN'), 'rd');
            nonConstData = const_cast<void *>(data->GetDataConst());
            if (data != networkGroup->DataEnd()) {
                fRoads = static_cast<WRoad *>(nonConstData);
            }

            fValid = true;
        }
    }
    fgRoadNetwork->ResolveBarriers();
    fgRoadNetwork->ResolveShortcuts();
    fgRoadNetwork->ResetRaceSegments();
}

void WRoadNetwork::Shutdown() {
    if (fgRoadNetwork != nullptr) {
        delete fgRoadNetwork;
        fgRoadNetwork = nullptr;
    }
}

// STRIPPED
void WRoadNetwork::Restart() {}

// STRIPPED
bool WRoadNetwork::WillCrossBarrier(int segment_number, int which_node, bool respect_drive_through_barriers) {}

bool WRoadNetwork::SegmentCrossesBarrier(WRoadSegment *segment, TrackPathBarrier *barrier) {
    USpline spline;
    bVector2 points[2];

    BuildSegmentSpline(*segment, spline);
    int num_pieces = bMax(static_cast<int>(bCeil(segment->GetLength() * 0.1f)), 4);
    float inc = 1.0f / static_cast<float>(num_pieces);

    for (int i = 0; i <= num_pieces; i++) {
        int which_point = i & 1;
        UMath::Vector4 v4;
        spline.EvaluateSpline(static_cast<float>(i) * inc, v4);

        points[which_point] = bVector2(v4.z, -v4.x);
        if (i > 0) {
            if (barrier->Intersects(&points[which_point], &points[which_point ^ 1])) {
                return true;
            }
        }
    }
    return false;
}

void WRoadNetwork::ResetRaceSegments() {
    fValidRaceFilter = false;
    for (int i = 0; i < static_cast<int>(fNumSegments); i++) {
        GetSegmentNonConst(i)->SetInRace(false);
        GetSegmentNonConst(i)->SetRaceRouteForward(false);
    }
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
    if (road_nav->GetNavType() == WRoadNav::kTypePath) {
        int num_segments = road_nav->GetNumPathSegments();
        for (int i = 0; i < num_segments; i++) {
            GetSegmentNonConst(road_nav->GetPathSegment(i))->SetInRace(true);
            if (i < num_segments - 1) {
                FlagSegmentRaceDirection(road_nav->GetPathSegment(i), road_nav->GetPathSegment(i + 1));
            }
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
        road->nShortcut = 0xFF;
    }
}

unsigned char WRoadNav::FirstShortcutInPath() {
    if (GetNavType() == kTypePath) {
        int num_segments = GetNumPathSegments();
        WRoadNetwork &rn = WRoadNetwork::Get();
        for (int i = 0; i < num_segments; i++) {
            const WRoadSegment *segment = rn.GetSegment(GetPathSegment(i));
            if (segment->IsShortcut()) {
                int road_number = segment->fRoadID;
                const WRoad *road = rn.GetRoad(road_number);
                return road->nShortcut;
            }
        }
    }
    return 0xff;
}

void WRoadNetwork::ResolveShortcuts() {
    ResetShortcuts();
    if (GRaceStatus::Exists()) {
        GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();
        if (race_parameters) {
            WRoadNav nav;
            const float dir_weight = 1.0f;
            const bool force_centre_lane = true;

            nav.SetDecisionFilter(true);
            nav.SetNavType(WRoadNav::kTypeDirection);

            int num_shortcuts = race_parameters->GetNumShortcuts();
            for (int i = 0; i < num_shortcuts; i++) {
                GMarker *shortcut = race_parameters->GetShortcut(i);
                if (shortcut) {
                    nav.InitAtPoint(shortcut->GetPosition(), shortcut->GetDirection(), true, dir_weight);
                    if (nav.IsValid()) {
                        WRoad *road = Get().GetRoadNonConst(nav.GetRoadInd());
                        road->nShortcut = static_cast<char>(i);
                    }
                }
            }
            for (unsigned int segment_number = 0; segment_number < fNumSegments; segment_number++) {
                WRoadSegment *segment = GetSegmentNonConst(segment_number);
                int road_number = segment->fRoadID;
                if (road_number != -1) {
                    if (fgRoadNetwork->GetRoad(road_number)->nShortcut != 0xFF) {
                        segment->SetShortcut(true);
                    }
                }
            }
        }
    }
}

void WRoadNetwork::ResetBarriers() {
    for (unsigned int segment_number = 0; segment_number < fNumSegments; segment_number++) {
        WRoadSegment *segment = GetSegmentNonConst(segment_number);
        segment->SetCrossesDriveThroughBarrier(false);
        segment->SetCrossesBarrier(false);
    }
}

void WRoadNetwork::ResolveBarriers() {
    int num_exemptions = 0;
    short exempted_roads[4];

    ResetBarriers();

    for (int i = 0; i < 4; i++) {
        exempted_roads[i] = -1;
    }

    if (GRaceStatus::Exists()) {
        WRoadNav nav;
        const float dir_weight = 1.0f;
        const bool force_centre_lane = true;
        nav.SetDecisionFilter(true);
        nav.SetNavType(WRoadNav::kTypeDirection);
        GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();

        if (race_parameters) {
            num_exemptions = race_parameters->GetNumBarrierExemptions();
            if (num_exemptions > 0) {
                for (int i = 0; i < num_exemptions; i++) {
                    GMarker *exemption = race_parameters->GetBarrierExemption(i);
                    nav.InitAtPoint(exemption->GetPosition(), exemption->GetDirection(), force_centre_lane, dir_weight);
                    if (nav.IsValid()) {
                        exempted_roads[i] = nav.GetRoadInd();
                    }
                }
            }
        } else {
            UMath::Vector3 hack_direction = UMath::Vector3Make(-0.7f, 0.0f, 0.7f);
            UMath::Vector3 hack_position = UMath::Vector3Make(-2511.0f, 147.8f, 1783.0f);
            nav.InitAtPoint(hack_position, hack_direction, force_centre_lane, dir_weight);
            if (nav.IsValid()) {
                num_exemptions = 1;
                exempted_roads[0] = nav.GetRoadInd();
            }
        }
    }

    WGrid &grid = WGrid::Get();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    int num_barriers = TheTrackPathManager.GetNumBarriers();

    for (int barrier_number = 0; barrier_number < num_barriers; barrier_number++) {
        TrackPathBarrier *barrier = TheTrackPathManager.GetBarrier(barrier_number);
        if (barrier->IsEnabled()) {
            typedef UTL::Std::set<short, _type_set> SEGMENT_SET;
            UMath::Vector4 barrier_points[2];
            barrier_points[0] = UMath::Vector4Make(-barrier->Points[0].y, 0.0f, barrier->Points[0].x, 1.0f);
            barrier_points[1] = UMath::Vector4Make(-barrier->Points[1].y, 0.0f, barrier->Points[1].x, 1.0f);

            UTL::FastVector<unsigned int, 16> node_list;
            grid.FindNodes(barrier_points, node_list);

            SEGMENT_SET segment_set;

            for (unsigned int *iter = node_list.begin(); iter != node_list.end(); ++iter) {
                WGridNode *grid_node = grid.fNodes[*iter];
                if (grid_node) {
                    unsigned int numSegments = grid_node->GetElemTypeCount(WGrid_kRoadSegment);
                    for (unsigned int i = 0; i < numSegments; i++) {
                        segment_set.insert(static_cast<short>(grid_node->GetElemType(i, WGrid_kRoadSegment)));
                    }
                }
            }

            for (SEGMENT_SET::const_iterator it = segment_set.begin(); it != segment_set.end(); ++it) {
                short segment_number = *it;
                WRoadSegment *segment = roadNetwork.GetSegmentNonConst(segment_number);
                if (SegmentCrossesBarrier(segment, barrier)) {
                    bool exempt = false;
                    short road_number = segment->fRoadID;
                    if (num_exemptions > 0 && road_number != -1) {
                        for (int j = 0; j < num_exemptions; j++) {
                            exempt = exempt | (road_number == exempted_roads[j]);
                        }
                    }
                    if (!exempt) {
                        if (barrier->IsPlayerBarrier()) {
                            segment->SetCrossesDriveThroughBarrier(true);
                        } else {
                            segment->SetCrossesBarrier(true);
                        }
                    }
                }
            }
        }
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
    if (!node || node->fProfileIndex < 0) {
        return &fInvalidProfile;
    }
    return roadNetwork.GetProfile(node->fProfileIndex);
}

bool WRoadNetwork::GetSegmentProfiles(const WRoadSegment &segment, const WRoadProfile **profile) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadNode *node[2];
    node[0] = roadNetwork.GetNode(segment.fNodeIndex[0]);
    node[1] = roadNetwork.GetNode(segment.fNodeIndex[1]);
    if (!node[0] || !node[1] || node[0]->fProfileIndex < 0 || node[1]->fProfileIndex < 0) {
        profile[0] = &fInvalidProfile;
        profile[1] = &fInvalidProfile;
        return false;
    }
    profile[0] = roadNetwork.GetProfile(node[0]->fProfileIndex);
    profile[1] = roadNetwork.GetProfile(node[1]->fProfileIndex);
    return true;
}

bool WRoadNetwork::GetSegmentTrafficLaneRightSide(const WRoadSegment &segment, int laneInd) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadProfile *profilePtr[2];
    roadNetwork.GetSegmentProfiles(segment, profilePtr);
    return laneInd >= profilePtr[0]->fMiddleZone;
}

int WRoadNetwork::GetSegmentTrafficLaneInd(const WRoadSegment &segment, int lane_count) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadProfile *profilePtr[2];
    int decLaneCount = lane_count;

    roadNetwork.GetSegmentProfiles(segment, profilePtr);
    for (int i = 0; i < profilePtr[0]->fNumZones; i++) {
        if (profilePtr[0]->GetLaneType(i, false) == WRoadNav::kLaneTraffic) {
            if (decLaneCount <= 0) {
                return i;
            }
            decLaneCount--;
        }
    }
    return 0;
}

int WRoadNetwork::GetSegmentNumTrafficLanes(const WRoadSegment &segment) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadProfile *profilePtr[2];
    int numTrafficLanes[2] = {0};

    roadNetwork.GetSegmentProfiles(segment, profilePtr);
    for (int i = 0; i < profilePtr[0]->fNumZones; i++) {
        if (profilePtr[0]->GetLaneType(i, false) == WRoadNav::kLaneTraffic) {
            numTrafficLanes[0]++;
        }
    }
    for (int i = 0; i < profilePtr[1]->fNumZones; i++) {
        if (profilePtr[1]->GetLaneType(i, false) == WRoadNav::kLaneTraffic) {
            numTrafficLanes[1]++;
        }
    }
    return UMath::Max(numTrafficLanes[1], numTrafficLanes[0]);
}

void WRoadNetwork::GetSegmentForwardVector(int segInd, UMath::Vector3 &forwardVector) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);
    roadNetwork.GetSegmentForwardVector(*segment, forwardVector);
}

void WRoadNetwork::GetSegmentForwardVector(const WRoadSegment &segment, UMath::Vector3 &forwardVector) {
    const WRoadNode *nodes[2];
    GetSegmentNodes(segment, nodes);
    UMath::Vector3 v = UVector3(nodes[1]->fPosition) - nodes[0]->fPosition;
    UMath::Unit(v, forwardVector);
}

void WRoadNetwork::GetSegmentEndPoints(const WRoadSegment &segment, UMath::Vector3 &start, UMath::Vector3 &end) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadNode *nodePtr[2];
    roadNetwork.GetSegmentNodes(segment, nodePtr);
    start = nodePtr[0]->fPosition;
    end = nodePtr[1]->fPosition;
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

void WRoadNetwork::GetPointOnSegment(const UMath::Vector3 &start, const UMath::Vector3 &end, const WRoadSegment &segment, float d,
                                     UMath::Vector3 &point) {
    if (segment.fFlags & kRoadSegmentCurved) {
        GetSegmentCurveStep(start, end, segment, d, point);
        return;
    }
    UMath::Lerp(start, end, d, point);
}

void WRoadNetwork::BuildSegmentSpline(const WRoadSegment &segment, USpline &spline) {
    const WRoadNode *nodePtr[2];
    UMath::Vector3 end_control;

    segment.GetEndControl(end_control);

    UMath::Vector3 start_control;
    segment.GetStartControl(start_control);

    GetSegmentNodes(segment, nodePtr);

    const UMath::Vector3 &end = nodePtr[1]->fPosition;
    const UMath::Vector3 &start = nodePtr[0]->fPosition;
    spline.BuildSplineEx(start, UVector3(start) + UVector3(start_control), end, UVector3(end) + UVector3(end_control));
}

void WRoadNetwork::GetPointAndVecOnSegment(const WRoadSegment &segment, float d, UMath::Vector3 &point, UMath::Vector3 &vec) {
    WRoadNetwork &roadNetwork = Get();
    GetPointOnSegment(segment, d, point);
    if (segment.fFlags & kRoadSegmentCurved) {
        static USpline roadSpline;
        roadNetwork.BuildSegmentSpline(segment, roadSpline);
        UMath::Vector4 tangent;
        roadSpline.EvaluateTangent(d, tangent);
        vec = UMath::Vector4To3(tangent);
    } else {
        roadNetwork.GetSegmentForwardVector(segment, vec);
    }
}

float WRoadNetwork::GetSegmentPointIntersect(const WRoadSegment &segment, const UMath::Vector3 &pt, UMath::Vector3 &intersect, bool checkBound) {
    WRoadNetwork &roadNetwork = Get();
    UMath::Vector3 pos = GetNode(segment.fNodeIndex[0])->fPosition;
    UMath::Vector3 pos2 = GetNode(segment.fNodeIndex[1])->fPosition;
    return GetLinePointIntersect(pos, pos2, pt, intersect, checkBound);
}

float WRoadNetwork::GetLinePointIntersect(const UMath::Vector3 &start, const UMath::Vector3 &end, const UMath::Vector3 &pt, UMath::Vector3 &intersect,
                                          bool checkBound) {
    UMath::Vector3 segVec;
    UMath::Vector3 posVec;

    UMath::Sub(end, start, segVec);
    UMath::Unit(segVec, segVec);
    UMath::Sub(pt, start, posVec);
    float length = UMath::Distance(pt, start);

    UMath::Unit(posVec, posVec);
    float dist = length * UMath::Dot(segVec, posVec);
    float segDist = UMath::Distance(start, end);

    if (checkBound) {
        if (dist >= segDist) {
            intersect = end;
            return 1.0f;
        } else if (dist <= 0.0f) {
            intersect = start;
            return 0.0f;
        }
    }

    UMath::Unit(segVec, segVec);
    UMath::Scale(segVec, dist, segVec);
    UMath::Add(start, segVec, intersect);
    return dist / segDist;
}

void WRoadNetwork::GetSegmentCurveStep(const UMath::Vector3 &start, const UMath::Vector3 &end, const WRoadSegment &segment, float u,
                                       UMath::Vector3 &point) {
    WRoadNetwork &roadNetwork = Get();
    static USpline roadSpline;
    UMath::Vector4 tempPos;

    UMath::Vector3 end_control;
    segment.GetEndControl(end_control);
    UMath::Vector3 start_control;
    segment.GetStartControl(start_control);

    roadSpline.BuildSplineEx(start, UVector3(start) + UVector3(start_control), end, UVector3(end) + UVector3(end_control));
    roadSpline.EvaluateSpline(u, tempPos);
    point = UMath::Vector4To3(tempPos);
}

WRoadNav::WRoadNav() {
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
    bOccludedFromBehind = false;
    Reset();
}

WRoadNav::~WRoadNav() {
    PathFinder *path_finder = PathFinder::Get();
    if (path_finder) {
        path_finder->Cancel(this);
    }
    if (pCookieTrail != nullptr) {
        delete pCookieTrail;
    }
    if (pPathSegments != nullptr) {
        delete pPathSegments;
    }
}

void WRoadNav::SetCookieTrail(CookieTrail<NavCookie, 32> *p_cookies) {
    pCookieTrail = p_cookies;
    bCookieTrail = p_cookies != nullptr;
}

void WRoadNav::SetCookieTrail(bool b) {
    if (b && !pCookieTrail) {
        pCookieTrail = new ("WRoadNav CookieTrail", 0) CookieTrail<NavCookie, 32>();
    }
    bCookieTrail = b;
}

void WRoadNav::ClearCookieTrail() {
    if (pCookieTrail != nullptr) {
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
        pPathSegments = new ("WRoadNav Path Buffer", 0) unsigned short[510];
    }
}

void WRoadNav::SetPathType(EPathType type) {
    fPathType = type;
}

void WRoadNav::Reset() {
    fValid = false;
    ClearCookieTrail();
    DetermineVehicleHalfWidth();
    fPathType = kPathNone;
    nPathGoalSegment = 0xFFFF;
    fNavType = kTypeNone;
    fLaneType = kLaneRacing;
    nPathSegments = 0;
    bCrossedPathGoal = false;
    fPosition = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    fForwardVector = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    fNodeInd = 0;
    fSegmentInd = 0;
    fSegTime = 0.0f;
    fCurvature = 0.0f;
    fStartPos = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    fEndPos = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    fStartControl = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    fEndControl = UMath::Vector3Make(0.0f, 0.0f, 0.0f);
    fDeadEnd = 0;
    fLaneInd = 0;
    fFromLaneInd = 0;
    fToLaneInd = 0;
    fLaneOffset = 0.0f;
    fFromLaneOffset = 0.0f;
    fToLaneOffset = 0.0f;
    fLaneChangeDist = 0.0f;
    fLaneChangeInc = 0.0f;
    mOutOfBounds = 0.0f;
}

bool WRoadNav::OnPath() const {
    if (fNavType == kTypePath && IsValid() && pPathSegments && nPathSegments > 0) {
        int i;
        WRoadNetwork &roadNetwork = WRoadNetwork::Get();
        const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());
        const WRoadNode *node = &roadNetwork.fNodes[segment->fNodeIndex[GetNodeInd()]];
        bool found;
        for (i = 0; i < nPathSegments; i++) {
            if (fSegmentInd == pPathSegments[i]) {
                break;
            }
        }
        if (++i < nPathSegments) {
            int new_segment_index = pPathSegments[i];
            const WRoadSegment *new_segment = roadNetwork.GetSegment(new_segment_index);
            const WRoadNode *new_nodes[2];
            roadNetwork.GetSegmentNodes(*new_segment, new_nodes);

            bool match_first = node == new_nodes[0];
            bool match_second = node == new_nodes[1];
            if (!match_first && !match_second) {
                return false;
            }
            return true;
        }
    }

    return false;
}

bool bAiRandomTurns = false;

// UNSOLVED https://decomp.me/scratch/Beczf
// PS: https://decomp.me/scratch/rJkvP
short WRoadNav::GetNextOffset(const UMath::Vector3 &to, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos) {
    useOldStartPos = true;
    bool end_of_path = false;
    short newSegInd = GetSegmentInd();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(newSegInd);
    const WRoadNode *node = &roadNetwork.fNodes[segment->fNodeIndex[GetNodeInd()]];

    if (fNavType == kTypePath) {
        int i;
        bool found = false;
        if (pPathSegments != nullptr) {
            for (i = 0; i < nPathSegments; i++) {
                if (fSegmentInd == pPathSegments[i]) {
                    break;
                }
            }
            if (++i < nPathSegments) {
                const WRoadNode *new_nodes[2];
                int new_segment_index = pPathSegments[i];
                const WRoadSegment *new_segment = roadNetwork.GetSegment(new_segment_index);
                roadNetwork.GetSegmentNodes(*new_segment, new_nodes);

                bool match_first = (node == new_nodes[0]);
                bool match_second = (node == new_nodes[1]);
                if (!match_first && !match_second) {
                    nodeInd ^= 1;
                    useOldStartPos = false;
                } else {
                    nodeInd = match_first;
                    newSegInd = static_cast<short>(new_segment_index);
                }
                found = true;
            } else {
                end_of_path = true;
            }
        }
        if (!found) {
            if (fPathType == kPathGPS) {
                newSegInd = fSegmentInd;
                return newSegInd;
            }
            SetNavType(kTypeDirection);
        }
    }

    if (fNavType == kTypeDirection) {
        UMath::Vector3 toVec = to;
        UMath::Unit(toVec, toVec);
        const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(node, fSegmentInd);

        if (node->fNumSegments > 1) {
            if (checkSegment || (segment->fFlags & kRoadSegmentDecision)) { // TODO
                newSegInd = checkSegment->fIndex;
                nodeInd = node == &roadNetwork.fNodes[checkSegment->fNodeIndex[0]];
            } else {
                unsigned int shortcut_cached = 0;
                unsigned int shortcut_allowed = 0;
                unsigned char shortcut_number = GetShortcutNumber();

                if (shortcut_number != 0xFF) {
                    int mask = 1 << shortcut_number;
                    shortcut_cached |= mask;
                    shortcut_allowed |= mask;
                }

                float closest_to_target = 2.0f;
                float target_dot = bAiRandomTurns ? bRandom(1.0f) : 1.0f;

                for (int i = 0; i < static_cast<int>(node->fNumSegments); i++) {
                    if (node->fSegmentIndex[i] == fSegmentInd)
                        continue;

                    const WRoadSegment *newRoadSegment = roadNetwork.GetSegment(node->fSegmentIndex[i]);
                    bool respect_full_barriers = RespectFullBarriers();
                    bool respect_drive_through_barriers = RespectDriveThroughBarriers();

                    float worst_gap_to_target = 0.0f;
                    const int kMaxWalkSegments = 19;
                    const float kMaxWalkDistance = 100.0f;
                    float distance = kMaxWalkDistance;
                    const WRoadNode *walkRoadNode = node;
                    const WRoadSegment *walkRoadSegment = newRoadSegment;

                    for (int w = 0; w <= kMaxWalkSegments; w++) {
                        if ((respect_full_barriers || respect_drive_through_barriers) &&
                            walkRoadSegment->CrossesBarrier(respect_drive_through_barriers)) {
                            walkRoadSegment = nullptr;
                            break;
                        }

                        bool walk_segment_forward = (walkRoadNode == &roadNetwork.fNodes[walkRoadSegment->fNodeIndex[0]]);

                        if (bRaceFilter) {
                            if (!walkRoadSegment->IsInRace()) {
                                walkRoadSegment = nullptr;
                                break;
                            }
                            if (walkRoadSegment->RaceRouteForward() ^ walk_segment_forward) {
                                walkRoadSegment = nullptr;
                                break;
                            }
                        }

                        if (bTrafficFilter && !walkRoadSegment->IsTrafficAllowed()) {
                            walkRoadSegment = nullptr;
                            break;
                        }

                        if (bCopFilter && !walkRoadSegment->ShouldCopsConsider()) {
                            walkRoadSegment = nullptr;
                            break;
                        }

                        if (walkRoadSegment->IsShortcut()) {
                            const WRoad *road = roadNetwork.GetRoad(walkRoadSegment->fRoadID);
                            if (!MakeShortcutDecision(road->nShortcut, &shortcut_cached, &shortcut_allowed)) {
                                walkRoadSegment = nullptr;
                                break;
                            }
                        }

                        UMath::Vector3 vec;
                        roadNetwork.GetSegmentForwardVector(*walkRoadSegment, vec);
                        if (!walk_segment_forward) {
                            UMath::Negate(vec);
                        }
                        UMath::Unit(vec, vec);
                        float dot = UMath::Dot(vec, toVec);
                        float gap_to_target = bAbs(dot - target_dot);
                        if (gap_to_target > worst_gap_to_target) {
                            worst_gap_to_target = gap_to_target;
                        }
                        if (worst_gap_to_target >= closest_to_target) {
                            walkRoadSegment = nullptr;
                            break;
                        }

                        distance -= walkRoadSegment->GetLength();
                        if (w > 0 && distance <= 0.0f)
                            break;

                        const WRoadNode *oppNode = roadNetwork.GetSegmentOppNode(*walkRoadSegment, walkRoadNode);
                        const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(oppNode, walkRoadSegment->fIndex);
                        if (checkSegment == nullptr) {
                            if (w == 0) {
                                walkRoadSegment = nullptr;
                            }
                            break;
                        }
                        if (oppNode != &roadNetwork.fNodes[checkSegment->fNodeIndex[0]] && (checkSegment->fFlags & kRoadSegmentOneWay)) {
                            char towards;
                            walkRoadSegment = nullptr;
                            break;
                        }
                        if (end_of_path)
                            break;
                        walkRoadSegment = checkSegment;
                        walkRoadNode = oppNode;
                    }

                    if (walkRoadSegment && closest_to_target > worst_gap_to_target) {
                        newSegInd = node->fSegmentIndex[i];
                        const WRoadSegment *newSegment = roadNetwork.GetSegment(newSegInd);
                        nodeInd = (node == &roadNetwork.fNodes[newSegment->fNodeIndex[0]]);
                        closest_to_target = worst_gap_to_target;
                    }
                }

                if (newSegInd == GetSegmentInd()) {
                    nodeInd ^= 1;
                    useOldStartPos = false;
                }
            }
        } else {
            nodeInd = nodeInd ^ 1;
            useOldStartPos = false;
        }
    }

    bool drag;
    GetLaneType();
    const WRoadSegment *new_segment = roadNetwork.GetSegment(0);
    bool do_centre_shift;
    float centre_shift;

    nextLaneOffset = SnapToSelectableLane(fLaneOffset, newSegInd, nodeInd);
    return newSegInd;
}

void WRoadNav::SnapToSelectableLane() {
    float offset = SnapToSelectableLane(fLaneOffset);
    ChangeLanes(offset, 0.0f);
}

float WRoadNav::SnapToSelectableLane(float input_offset) {
    return SnapToSelectableLane(input_offset, fSegmentInd, fNodeInd);
}

float WRoadNav::SnapToSelectableLane(float input_offset, int segment_no, char node_index) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    bool cop_lane = fLaneType == kLaneCop || fLaneType == kLaneCopReckless;
    bool drag_lane = fLaneType == kLaneDrag;
    bool grid_lane = fLaneType == kLaneStartingGrid;
    bool racing_lane = fLaneType == kLaneRacing;
    const WRoadSegment *segment = roadNetwork.GetSegment(segment_no);
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, node_index);
    bool inverted = segment->IsProfileInverted(node_index);
    bool forward = node_index > 0;

    int best_lane = -1;
    bool found_lane = false;
    bool best_backward = false;
    float next_offset = 0.0f;
    float offset_difference = 1000.0f;

    int num_forward_lanes = profile->GetNumLanes(forward, inverted);

    for (int n = 0; n < num_forward_lanes; n++) {
        int lane = profile->GetNthLane(n, forward, inverted);
        unsigned char lane_type = profile->GetLaneType(lane, false);
        if (IsSelectable(lane_type)) {
            float offset = profile->GetRawLaneOffset(lane);
            float difference = offset - input_offset;
            if (difference == bClamp(difference, -offset_difference, offset_difference)) {
                found_lane = true;
                best_lane = lane;
                next_offset = offset;
                offset_difference = bAbs(offset - input_offset);
            }
        }
    }

    int num_backward_lanes = profile->GetNumLanes(!forward, inverted);

    if (cop_lane || grid_lane || racing_lane || drag_lane || !found_lane) {
        for (int n = 0; n < num_backward_lanes; n++) {
            int lane = profile->GetNthLane(n, !forward, inverted);
            unsigned char lane_type = profile->GetLaneType(lane, false);
            if (IsSelectable(lane_type)) {
                float offset = -profile->GetRawLaneOffset(lane);
                float difference = offset - input_offset;
                if (difference == bClamp(difference, -offset_difference, offset_difference)) {
                    best_backward = true;
                    best_lane = lane;
                    next_offset = offset;
                    offset_difference = bAbs(offset - input_offset);
                }
            }
        }
    }

    float output_offset;

    if ((cop_lane || racing_lane) && best_lane >= 0) {
        float offset = profile->GetRawLaneOffset(best_lane);
        if (best_backward) {
            offset = -offset;
        }
        float width = profile->GetRawLaneWidth(best_lane);
        float difference = input_offset - offset;
        difference = bClamp(difference, width * -0.5f, width * 0.5f);

        bool inverted_xor_backward = inverted != !forward;

        int left_lane = profile->GetLaneNumber(best_lane, inverted_xor_backward);
        int right_lane = left_lane;
        next_offset = offset + difference;

        while (left_lane > 0) {
            if (!IsDrivable(profile->GetLaneType(left_lane - 1, inverted_xor_backward)))
                break;
            left_lane--;
        }

        while (right_lane < profile->fNumZones - 1) {
            if (!IsDrivable(profile->GetLaneType(right_lane + 1, inverted_xor_backward)))
                break;
            right_lane++;
        }

        float right =
            profile->GetRelativeLaneOffset(right_lane, inverted_xor_backward) + profile->GetLaneWidth(right_lane, inverted_xor_backward) * 0.5f;
        float left =
            profile->GetRelativeLaneOffset(left_lane, inverted_xor_backward) - profile->GetLaneWidth(left_lane, inverted_xor_backward) * 0.5f;

        float safety_margin = fVehicleHalfWidth + 1.5f;
        output_offset = bClamp(next_offset, left + safety_margin, right - safety_margin);
    } else {
        output_offset = next_offset;
    }

    return output_offset;
}

void WRoadNav::Reverse() {
    if (GetRaceFilter() && !IsWrongWay() && (fPathType == kPathRacer || fPathType == kPathPlayer)) {
        return;
    }
    fNodeInd = fNodeInd ^ 1;
    fSegTime = 1.0f - fSegTime;

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());
    SetStartEndPos(*segment, fLaneOffset);
    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
    ResetCookieTrail();
}

int WRoadProfile::GetNumTrafficLanes(bool forward) const {
    int num_traffic_lanes = 0;
    int num_lanes = GetNumLanes(forward);
    for (int i = 0; i < num_lanes; i++) {
        if (GetLaneType(i, !forward) == WRoadNav::kLaneTraffic) {
            num_traffic_lanes++;
        }
    }
    return num_traffic_lanes;
}

int WRoadProfile::GetNthTrafficLane(int n, bool forward) const {
    int num_traffic_lanes = 0;
    int num_lanes = GetNumLanes(forward);
    int fallback = GetMiddleZone(!forward);
    for (int i = 0; i < num_lanes; i++) {
        int real_lane = GetNthLane(i, forward);
        if (GetLaneType(real_lane, false) == WRoadNav::kLaneTraffic) {
            if (n == num_traffic_lanes++) {
                return real_lane;
            }
            fallback = real_lane;
        }
    }
    return fallback;
}

int WRoadProfile::GetNthTrafficLaneFromCurb(int n, bool forward) const {
    int num_traffic_lanes = 0;
    int num_lanes = GetNumLanes(forward);
    int fallback = GetMiddleZone(!forward);
    for (int i = num_lanes - 1; i >= 0; i--) {
        int real_lane = GetNthLane(i, forward);
        if (GetLaneType(real_lane, false) == WRoadNav::kLaneTraffic) {
            if (n == num_traffic_lanes++) {
                return real_lane;
            }
            fallback = real_lane;
        }
    }
    return fallback;
}

// UNSOLVED MW: https://decomp.me/scratch/0PjWL
// PS: https://decomp.me/scratch/FzlHK
// A124: https://decomp.me/scratch/pVEP3
int WRoadNetwork::GetRightMostTrafficEntrance(int node_number, int onto_segment) {
    int ret = -1;
    const WRoadNode *node = GetNode(node_number);
    int num_segments = node->fNumSegments;

    if (num_segments > 2) {
        const WRoadSegment *segment = GetSegment(onto_segment);
        int which_node = node_number != segment->fNodeIndex[0];
        bool inverted = segment->IsProfileInverted(which_node);
        bool forward = which_node == 0;
        const WRoadProfile *profile = GetProfile(node->fProfileIndex);

        if (profile->GetNumTrafficLanes(forward, inverted) > 0) {
            UMath::Vector2 onto_forward;

            segment->GetForwardVec(which_node, onto_forward);

            if (!forward) {
                UMath::Scale(onto_forward, -1.0f, onto_forward);
            }

            float best_cross;
            bool first = true;
            UMath::Vector2 best_vector;
            for (int i = 0; i < num_segments; i++) {
                int segment_number = node->fSegmentIndex[i];
                if (segment_number == onto_segment) {
                    continue;
                }

                const WRoadSegment *from_segment = GetSegment(segment_number);
                const WRoadNode *from_node = GetSegmentOppNode(segment_number, node);
                int from_which_node = node_number != from_segment->fNodeIndex[1];
                bool from_inverted = from_segment->IsProfileInverted(from_which_node);
                bool from_forward = from_which_node == 0;
                const WRoadProfile *from_profile = GetProfile(from_node->fProfileIndex);

                if (from_profile->GetNumTrafficLanes(from_forward, from_inverted) == 0) {
                    continue;
                }
                UMath::Vector3 vector_3d = UVector3(from_node->fPosition) - node->fPosition;
                UMath::Vector2 vector = UMath::Vector2Make(vector_3d.x, vector_3d.z);
                float cross = UMath::Cross(vector, onto_forward);

                bool is_best = false;
                if (!first) {
                    float old_cross = UMath::Cross(best_vector, vector);
                    bool right_of_onto = cross >= 0.0f;
                    bool right_of_best = old_cross >= 0.0f;
                    // is_best = (best_cross < 0.0f) ? right_of_onto || right_of_best : right_of_onto && right_of_best;
                    if (best_cross < 0.0f) {
                        is_best = right_of_onto || right_of_best;
                    } else {
                        is_best = right_of_onto && right_of_best;
                    }
                    if (!is_best) {
                        continue;
                    }
                }
                first = false;
                best_cross = cross;
                best_vector = vector;
                ret = segment_number;
            }
        }
    }

    return ret;
}

void WRoadNav::PullOver() {
    ClearCookieTrail();

    int which_node = GetNodeInd();
    WRoadNetwork &rn = WRoadNetwork::Get();
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = rn.GetSegment(segment_number);
    const WRoadProfile *profile = rn.GetSegmentProfile(*segment, which_node);
    int num_lanes = profile->fNumZones;
    bool inverted = segment->IsProfileInverted(which_node) ^ (which_node == 0);

    int lane = profile->GetLaneNumber(GetLaneInd(), inverted);

    bool is_barrier = false;
    bool last_lane;
    while (lane < num_lanes - 1) {
        int next_lane_type = profile->GetLaneType(lane + 1, inverted);
        if (next_lane_type == kLaneAny) {
            is_barrier = true;
        }
        if (next_lane_type != kLaneTraffic)
            break;
        lane++;
    }

    float extra = fVehicleHalfWidth;
    if ((lane == num_lanes - 1) || is_barrier) {
        extra = -extra;
    }

    float offset = profile->GetLaneOffset(lane, inverted) + profile->GetLaneWidth(lane, inverted) * 0.5f + extra;

    const UMath::Vector3 &nav_forward = GetForwardVector();
    UMath::Vector3 nav_right = UMath::Vector3Make(nav_forward.z, 0.0f, -nav_forward.x);
    UMath::Normalize(nav_right);

    float offset_change = offset - GetLaneOffset();
    UMath::ScaleAdd(nav_right, offset_change, GetPosition(), GetPosition());
}

// UNSOLVED
short WRoadNav::GetNextTraffic(const UMath::Vector3 &toVec, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    int which_node = GetNodeInd();
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = roadNetwork.GetSegment(segment_number);
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, which_node);
    bool forward = (which_node == 1);
    bool inverted = segment->IsProfileInverted(which_node);
    int nth_lane = 0;
    int num_traffic_lanes = 0;
    int current_lane = GetLaneInd();

    int num_lanes = profile->GetNumLanes(forward, inverted);
    for (int i = 0; i < num_lanes; i++) {
        int real_lane = profile->GetNthLane(i, forward, inverted);
        if (profile->GetLaneType(real_lane, false) == kLaneTraffic) {
            if (current_lane == real_lane) {
                nth_lane = num_traffic_lanes;
            }
            num_traffic_lanes++;
        }
    }

    WRoadNode *node = &roadNetwork.fNodes[segment->fNodeIndex[which_node]];

    char newLaneInd = fLaneInd;
    short oldSegInd; // TODO
    short newSegInd = fSegmentInd;
    const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(node, segment_number);

    if (node->fNumSegments < 2) {
        newSegInd = segment->fIndex;
    } else if (checkSegment != nullptr) {
        newSegInd = checkSegment->fIndex;
        bool new_forward = (node == &roadNetwork.fNodes[checkSegment->fNodeIndex[0]]); // TODO
        nodeInd = new_forward;
        bool new_inverted = checkSegment->IsProfileInverted(nodeInd);
        const WRoadProfile *new_profile = roadNetwork.GetSegmentProfile(*checkSegment, static_cast<int>(nodeInd));
        newLaneInd = static_cast<char>(new_profile->GetNthTrafficLane(nth_lane, new_forward, new_inverted));
    } else {
        if (UMath::LengthSquare(toVec) > 0.01f) {
            float bestDot = -1.0f;
            float check_segment_number = bestDot;
            for (int onSeg = 0; onSeg < static_cast<int>(node->fNumSegments); onSeg++) {
                const WRoadSegment *intersectionSegment = roadNetwork.GetSegment(node->fSegmentIndex[onSeg]);
                if (intersectionSegment->fIndex == fSegmentInd)
                    continue;
                if (!intersectionSegment->IsTrafficAllowed())
                    continue;

                const WRoadNode *oppNode = roadNetwork.GetSegmentOppNode(*intersectionSegment, node);
                const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(oppNode, -1);
                if (checkSegment && checkSegment->IsTrafficAllowed()) {
                    UMath::Vector3 vec;
                    bool reverse = (oppNode != &roadNetwork.fNodes[checkSegment->fNodeIndex[0]]);
                    roadNetwork.GetSegmentForwardVector(*checkSegment, vec);
                    if (reverse) {
                        vec = UMath::Vector3Make(-vec.x, -vec.y, -vec.z);
                    }
                    UMath::Unit(vec, vec);
                    float dot = UMath::Dot(vec, toVec);

                    if (dot >= bestDot) {
                        newSegInd = node->fSegmentIndex[onSeg];
                        const WRoadSegment *newSegment = roadNetwork.GetSegment(newSegInd);
                        check_segment_number = static_cast<float>(checkSegment->fIndex);
                        nodeInd = (node == &roadNetwork.fNodes[newSegment->fNodeIndex[0]]);
                        bestDot = dot;
                    }
                }
            }

            if (check_segment_number != -1.0f) {
                bool new_forward = (nodeInd == 1);
                const WRoadSegment *newSegment = roadNetwork.GetSegment(newSegInd);
                const WRoadProfile *new_profile = roadNetwork.GetSegmentProfile(*newSegment, static_cast<int>(nodeInd));
                bool new_inverted = newSegment->IsProfileInverted(static_cast<int>(nodeInd));

                if (newSegInd == roadNetwork.GetRightMostTrafficEntrance(newSegment->fNodeIndex[nodeInd], static_cast<int>(check_segment_number))) {
                    int nth_from_curb = num_traffic_lanes - nth_lane - 1;
                    newLaneInd = static_cast<char>(new_profile->GetNthTrafficLaneFromCurb(nth_from_curb, new_forward, new_inverted));
                } else {
                    newLaneInd = static_cast<char>(new_profile->GetNthTrafficLane(nth_lane, new_forward, new_inverted));
                }
            }
        } else {
            struct Candidate {
                int Lane;
                int WhichNode;
                int SegmentNumber;
                bool LastResort;
            };
            int num_candidates = 0;
            Candidate candidates[7];

            for (int i = 0; i < node->fNumSegments; i++) {
                int new_segment_number = node->fSegmentIndex[i];
                if (new_segment_number == fSegmentInd)
                    continue;

                const WRoadSegment *decision_segment = roadNetwork.GetSegment(new_segment_number);
                if (!decision_segment->IsTrafficAllowed())
                    continue;

                const WRoadNode *nodePtr[2];
                roadNetwork.GetSegmentNodes(*decision_segment, nodePtr);
                const WRoadNode *oppNode = node != nodePtr[0] ? nodePtr[0] : nodePtr[1];

                const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(oppNode, -1);
                if (checkSegment == nullptr)
                    continue;
                if (!checkSegment->IsTrafficAllowed())
                    continue;
                if (oppNode != &roadNetwork.fNodes[checkSegment->fNodeIndex[0]] && (checkSegment->fFlags & kRoadSegmentOneWay))
                    continue;

                char towards; // TODO
                // const bool respect_drive_through_barriers; // TODO
                int new_which_node = (node->fIndex != decision_segment->fNodeIndex[1]);
                bool new_forward = new_which_node == 1;
                bool new_inverted = decision_segment->IsProfileInverted(new_which_node);
                const WRoadProfile *new_profile = roadNetwork.GetSegmentProfile(*decision_segment, new_which_node);
                int num_traffic_lanes = new_profile->GetNumTrafficLanes(new_forward, new_inverted);

                if (num_traffic_lanes == 0)
                    continue;

                int rightmost_traffic_entrance = roadNetwork.GetRightMostTrafficEntrance(oppNode->fIndex, checkSegment->fIndex);

                if (new_segment_number == rightmost_traffic_entrance) {
                    int nth_from_curb = num_traffic_lanes - nth_lane - 1;
                    candidates[num_candidates].LastResort = (nth_from_curb > 0);
                    candidates[num_candidates].Lane = new_profile->GetNthTrafficLaneFromCurb(nth_from_curb, new_forward, new_inverted);
                } else {
                    candidates[num_candidates].LastResort = false;
                    candidates[num_candidates].Lane = new_profile->GetNthTrafficLane(nth_lane, new_forward, new_inverted);
                }
                candidates[num_candidates].WhichNode = new_which_node;
                candidates[num_candidates].SegmentNumber = new_segment_number;
                num_candidates++;
            }

            if (num_candidates > 0) {
                int selection = 0;
                if (num_candidates > 1) {
                    selection = bRandom(num_candidates);
                    if (candidates[selection].LastResort) {
                        selection = (selection + 1) % num_candidates;
                    }
                }
                newLaneInd = static_cast<char>(candidates[selection].Lane);
                nodeInd = static_cast<char>(candidates[selection].WhichNode);
                newSegInd = static_cast<short>(candidates[selection].SegmentNumber);
            }
        }
    }

    if (newSegInd != GetSegmentInd()) {
        if (newLaneInd < 0) {
            newLaneInd = 0;
        }
        SetLaneInd(newLaneInd);

        const WRoadSegment *nextSegment = roadNetwork.GetSegment(newSegInd);
        const WRoadNode *oppNode = roadNetwork.GetSegmentOppNode(*nextSegment, node);
        const WRoadProfile *new_profile = roadNetwork.GetProfile(oppNode->fProfileIndex);
        nextLaneOffset = new_profile->GetLaneOffset(newLaneInd, false);
    }

    useOldStartPos = true;
    return newSegInd;
}

void WRoadNav::RebuildSplines(const WRoadSegment *segment) {
    if (segment->fFlags & kRoadSegmentCurved) {
        fRoadSpline.BuildSplineEx(fStartPos, fStartControl, fEndPos, fEndControl);
        if (bCookieTrail) {
            fLeftSpline.BuildSplineEx(fLeftStartPos, fLeftStartControl, fLeftEndPos, fLeftEndControl);
            fRightSpline.BuildSplineEx(fRightStartPos, fRightStartControl, fRightEndPos, fRightEndControl);
        }
    }
}

void WRoadNav::EvaluateSplines(const WRoadSegment *segment) {
    if (segment->fFlags & kRoadSegmentCurved) {
        UMath::Vector4 tempPos;
        fRoadSpline.EvaluateSpline(fSegTime, tempPos);
        fPosition = UMath::Vector4To3(tempPos);
        fRoadSpline.EvaluateTangent(fSegTime, tempPos);
        fForwardVector = UMath::Vector4To3(tempPos);
        fCurvature = fRoadSpline.EvaluateCurvatureXZ(fSegTime);
        if (bCookieTrail) {
            UMath::Vector4 left_position;
            UMath::Vector4 right_position;
            fLeftSpline.EvaluateSpline(fSegTime, left_position);
            fRightSpline.EvaluateSpline(fSegTime, right_position);
            fLeftPosition = UMath::Vector4To3(left_position);
            fRightPosition = UMath::Vector4To3(right_position);
        }
    } else {
        fCurvature = 0.0f;
        WRoadNetwork &roadNetwork = WRoadNetwork::Get();
        UMath::Sub(fEndPos, fStartPos, fForwardVector);
        roadNetwork.GetPointOnSegment(fStartPos, fEndPos, *segment, fSegTime, fPosition);
        if (bCookieTrail) {
            roadNetwork.GetPointOnSegment(fLeftStartPos, fLeftEndPos, *segment, fSegTime, fLeftPosition);
            roadNetwork.GetPointOnSegment(fRightStartPos, fRightEndPos, *segment, fSegTime, fRightPosition);
        }
    }
}

void WRoadNav::UpdateCookieTrail(float cookie_gap) {
    if (IsValid() && bCookieTrail && pCookieTrail) {
        float cookie_length = 0.0f;
        int num_cookies = pCookieTrail->Count();
        bool add_new_cookie = (num_cookies == 0);

        if (!add_new_cookie) {
            const NavCookie &newest_cookie = pCookieTrail->Newest();
            UMath::Vector3 current_cookie_ray = UVector3(GetPosition()) - newest_cookie.Centre;
            float current_ray_length = UMath::Length(current_cookie_ray);

            if (current_ray_length >= cookie_gap) {
                add_new_cookie = true;
                cookie_length = current_ray_length;
                bVector2 cookie_ray_2d(current_cookie_ray.x, current_cookie_ray.z);
                if (bDot(&cookie_ray_2d, reinterpret_cast<const bVector2 *>(&newest_cookie.Forward)) < -0.99f) {
                    ClearCookieTrail();
                }
            }
        }

        if (add_new_cookie) {
            NavCookie cookie;
            cookie.Length = cookie_length;
            cookie.Flags = 0;
            cookie.Centre = GetPosition();
            cookie.Curvature = GetCurvature();
            cookie.SegmentNodeInd = GetNodeInd();
            cookie.SegmentNumber = GetSegmentInd();
            cookie.SetSegmentParameter(GetSegmentTime());

            bVector2 centre(cookie.Centre.x, cookie.Centre.z);

            cookie.Left = UMath::Vector2Make(GetLeftPosition().x, GetLeftPosition().z);
            cookie.Right = UMath::Vector2Make(GetRightPosition().x, GetRightPosition().z);

            bVector2 centre_to_left = *reinterpret_cast<bVector2 *>(&cookie.Left) - centre;
            bVector2 centre_to_right = *reinterpret_cast<bVector2 *>(&cookie.Right) - centre;
            bVector2 right = centre_to_right - centre_to_left;
            bVector2 forward(-right.y, right.x);

            bNormalize(reinterpret_cast<bVector2 *>(&cookie.Forward), &forward);

            cookie.LeftOffset = bCross(&centre_to_left, reinterpret_cast<bVector2 *>(&cookie.Forward));
            cookie.RightOffset = bCross(&centre_to_right, reinterpret_cast<bVector2 *>(&cookie.Forward));

            if (num_cookies == 0) {
                mCurrentCookie = cookie;
            }

            if (pCookieTrail->Count() == pCookieTrail->Capacity()) {
                nCookieIndex = bMax(0, nCookieIndex - 1);
            }

            pCookieTrail->AddNew(cookie);
        }
    }
}

void WRoadNav::IncNavPosition(float dist, const UMath::Vector3 &to, float max_lookahead) {
    if (!fValid)
        return;

    float cookie_gap = 3.0f;
    if (max_lookahead > 0.0f) {
        cookie_gap = UMath::Clamp(max_lookahead / 26.0f, 1.0f, cookie_gap);
    }

    if (bCookieTrail && pCookieTrail) {
        while (dist > 0.0f) {
            float incdist = bMin(cookie_gap * 1.1f, dist);
            PrivateIncNavPosition(incdist, to);
            UpdateCookieTrail(cookie_gap);
            dist -= incdist;
        }
    } else {
        PrivateIncNavPosition(dist, to);
    }
}

void WRoadNav::PrivateIncNavPosition(float dist, const UMath::Vector3 &to) {
    while (true) {
        WRoadNetwork &roadNetwork = WRoadNetwork::Get();
        const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());
        float segmentLength = UMath::Max(0.01f, UMath::Distance(fStartPos, fEndPos));
        float distFraction = dist / segmentLength;
        float toLength = UMath::Length(to);

        if (fSegTime + distFraction <= 1.0f) {
            fSegTime = fSegTime + distFraction;
            fSegTime = UMath::Min(UMath::Max(fSegTime, 0.0f), 1.0f);

            if (UpdateLaneChange(dist)) {
                SetStartEndPos(*segment, fLaneOffset);
                SetStartEndControls(*segment);
                RebuildSplines(segment);
            }

            EvaluateSplines(segment);
            break;
        } else {
            short newSegInd = fSegmentInd;

            UpdateLaneChange((1.0f - fSegTime) * segmentLength);

            float nextLaneOffset = fLaneOffset;
            bool useOldStartPos = false;
            char old_node_ind = fNodeInd;
            const WRoadSegment *newSegment;

            if (fNavType == kTypeDirection && toLength == 0.0f) {
                UMath::Vector3 endTo;
                segment->GetForwardVec(old_node_ind, endTo);
                if (fNodeInd == 0) {
                    UMath::Negate(endTo);
                }
                newSegInd = GetNextOffset(endTo, nextLaneOffset, fNodeInd, useOldStartPos);
            } else if (fNavType == kTypeDirection || fNavType == kTypePath) {
                newSegInd = GetNextOffset(to, nextLaneOffset, fNodeInd, useOldStartPos);
            } else if (fNavType == kTypeTraffic) {
                newSegInd = GetNextTraffic(to, nextLaneOffset, fNodeInd, useOldStartPos);
            }

            if (fSegmentInd == newSegInd && fNodeInd == old_node_ind) {
                fDeadEnd = 1;
                if (fNavType == kTypeTraffic) {
                    break;
                }
            }

            fSegmentInd = static_cast<short>(newSegInd);
            newSegment = roadNetwork.GetSegment(GetSegmentInd());

            if (useOldStartPos) {
                fStartPos = fEndPos;
                if (bCookieTrail) {
                    fLeftStartPos = fLeftEndPos;
                    fRightStartPos = fRightEndPos;
                }
                SetBoundPos(*newSegment, nextLaneOffset, false);
                SetLaneOffset(nextLaneOffset);
            } else {
                SetStartEndPos(*newSegment, fLaneOffset);
            }

            SetStartEndControls(*newSegment);
            RebuildSplines(newSegment);
            fPosition = fStartPos;
            float distRemaining; // TODO
            dist = (distFraction - (1.0f - fSegTime)) * segmentLength;
            fSegTime = 0.0f;
        }
    }
}

int WRoadNav::ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *interpolated_cookie) {
    return ClosestCookieAhead(position, nullptr, pCookieTrail->Count(), interpolated_cookie);
}

int WRoadNav::ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *cookies, int num_cookies, NavCookie *interpolated_cookie) {
    int ret = -1;
    float closest = -1.0f;
    if (num_cookies > 0) {
        float previous_dot = 0.0f;
        bVector2 car_position(position.x, position.z);
        for (int n = 0; n < num_cookies; n++) {
            const NavCookie &cookie = cookies ? cookies[n] : pCookieTrail->NthOldest(n);
            bVector2 cookie_to_car = car_position - bVector2(cookie.Centre.x, cookie.Centre.z);
            float current_dot = bDot(reinterpret_cast<const bVector2 *>(&cookie.Forward), &cookie_to_car);
            float distance_squared = bDot(cookie_to_car, cookie_to_car);
            if (n == 0) {
                if (current_dot < 0.0f) {
                    ret = n;
                    closest = distance_squared;
                }
            } else {
                bool between = current_dot * previous_dot < 0.0f;
                if (between && (ret < 0 || distance_squared < closest)) {
                    ret = n;
                    closest = distance_squared;
                }
            }
            previous_dot = current_dot;
        }
    }
    return ret;
}

bool WRoadNav::CookieCutter(NavCookie &cookie, const UMath::Vector3 &centre, float projection, bool pass_left, unsigned int cut_flags) {
    bVector2 cookie_to_centre(centre.x - cookie.Centre.x, centre.z - cookie.Centre.z);
    float left_offset = cookie.LeftOffset;
    float right_offset = cookie.RightOffset;
    float l = bCross(&cookie_to_centre, reinterpret_cast<const bVector2 *>(&cookie.Forward));

    if (l != bClamp(l, left_offset - projection, right_offset + projection)) {
        return false;
    }

    cookie.Flags |= 1 | cut_flags;

    bVector2 cookie_centre(cookie.Centre.x, cookie.Centre.z);
    bVector2 cookie_right(cookie.Forward.y, -cookie.Forward.x);

    float minimum_width = GetNavType() == kTypeTraffic ? 0.1f : 1.0f;

    if (pass_left) {
        right_offset = bMax(left_offset + minimum_width, l - projection);
        bScaleAdd(reinterpret_cast<bVector2 *>(&cookie.Right), &cookie_centre, &cookie_right, right_offset);
        cookie.RightOffset = right_offset;
    } else {
        left_offset = bMin(right_offset - minimum_width, l + projection);
        bScaleAdd(reinterpret_cast<bVector2 *>(&cookie.Left), &cookie_centre, &cookie_right, left_offset);
        cookie.LeftOffset = left_offset;
    }

    return true;
}

void WRoadNav::ClampCookieCentres(NavCookie *cookies, int num_cookies) {
    for (int i = 0; i < num_cookies; i++) {
        NavCookie &cookie = cookies[i];
        if (cookie.Flags & 1) {
            float size = (cookie.RightOffset - cookie.LeftOffset) / 2.0f;
            cookie.Centre.x = (cookie.Left.x + cookie.Right.x) / 2.0f;
            cookie.Centre.z = (cookie.Left.y + cookie.Right.y) / 2.0f;
            cookie.RightOffset = size;
            cookie.LeftOffset = -size;
        }
    }
}

static float TimeToClosestApproach(const UMath::Vector3 &p0, const UMath::Vector3 &v0, const UMath::Vector3 &p1, const UMath::Vector3 &v1,
                                   float *closing_speed) {
    UMath::Vector3 p = UVector3(p1) - p0;
    UMath::Vector3 v = UVector3(v1) - v0;
    UMath::Vector3 dir;
    UMath::Unit(p, dir);

    float a = UMath::Dot(dir, v);
    *closing_speed = -a;
    float b = UMath::Dot(v, v);

    a = UMath::Dot(p, v);

    return b < 0.0001f ? 3.0f : -a / b;
}

int WRoadNav::FetchAvoidables(IBody **avoidables, const int listsize) const {
    IVehicleAI *my_ai = pAIVehicle;
    if (!my_ai) {
        return 0;
    }

    IPursuit *my_pursuit = my_ai->GetPursuit();

    IPursuitAI *my_pursuitai;
    my_ai->QueryInterface(&my_pursuitai);
    bool is_formation_cop = false;
    if (my_pursuitai && my_pursuitai->GetInFormation()) {
        is_formation_cop = true;
    }

    ISimable *my_pursuit_target = nullptr;
    if (my_pursuit && is_formation_cop) {
        AITarget *target = my_pursuit->GetTarget();
        if (target != nullptr) {
            my_pursuit_target = target->GetSimable();
        }
    }

    IArticulatedVehicle *my_hitch;
    my_ai->QueryInterface(&my_hitch);
    int num_avoidables = 0;

    const AvoidableList &avoidable_list = my_ai->GetAvoidableList();

    for (AvoidableList::const_iterator iter = avoidable_list.begin(); iter != avoidable_list.end() && num_avoidables < listsize; iter++) {
        AIAvoidable *av = *iter;
        IBody *avoidable_body;
        if (!av->QueryInterface(&avoidable_body)) {
            continue;
        }

        if (my_hitch) {
            if (ComparePtr(avoidable_body, my_hitch->GetTrailer()) && my_hitch->IsHitched()) {
                continue;
            }
        }

        if (is_formation_cop && my_pursuit) {
            IVehicleAI *his_ai;
            if (avoidable_body->QueryInterface(&his_ai)) {
                IPursuit *his_pursuit = his_ai->GetPursuit();
                if (my_pursuit == his_pursuit) {
                    IPursuitAI *his_pursuitai;
                    if (ComparePtr(my_pursuit_target, his_ai)) {
                        continue;
                    }
                    his_ai->QueryInterface(&his_pursuitai);
                    if (his_pursuitai && his_pursuitai->GetInFormation()) {
                        continue;
                    }
                }
            }
        }

        avoidables[num_avoidables] = avoidable_body;
        num_avoidables++;
    }

    return num_avoidables;
}

// TODO pretty unfinished
// https://decomp.me/scratch/MXqTq
void WRoadNav::HolePunchAvoidables(NavCookie *cookies, int num_cookies, float current_offset, float delta_offset) {
    if (num_cookies == 0)
        return;

    IVehicleAI *my_ai = pAIVehicle;
    if (!my_ai)
        return;

    IBody *avoidables[32];
    int num_avoidables = FetchAvoidables(avoidables, sizeof(avoidables) / sizeof(IBody *));
    if (num_avoidables == 0)
        return;

    IRigidBody *my_body;
    my_ai->QueryInterface(&my_body);

    const UMath::Vector3 &my_position = my_body->GetPosition();
    const int my_cookie_index = ClosestCookieAhead(my_position, cookies, num_cookies, nullptr);
    if (my_cookie_index < 0)
        return;

    const NavCookie &my_cookie = cookies[my_cookie_index];
    const UMath::Vector3 &my_velocity = my_body->GetLinearVelocity();

    UMath::Vector3 my_right;
    my_body->GetRightVector(my_right);
    UMath::Vector3 my_forward;
    my_body->GetForwardVector(my_forward);
    UMath::Vector3 my_dimension;
    my_body->GetDimension(my_dimension);

    const float my_speed = my_body->GetSpeed();
    const UMath::Vector3 &nav_forward_3d = GetForwardVector();
    bVector2 nav_forward(nav_forward_3d.x, nav_forward_3d.z);
    bNormalize(&nav_forward, &nav_forward);

    int closest_avoidable = num_cookies;
    const bool is_racer = GetPathType() == kPathRacer || GetPathType() == kPathPlayer;
    const bool is_traffic = GetNavType() == kTypeTraffic;
    const bool is_drag = GRaceStatus::IsDragRace();

    for (int i = 0; i < num_avoidables; i++) {
        IBody *avoidable_body = avoidables[i];

        UMath::Matrix4 tranform;
        avoidable_body->GetTransform(tranform);

        UMath::Vector3 avoidable_position = UMath::Vector4To3(tranform.v3);
        const UMath::Vector3 &avoidable_right = UMath::Vector4To3(tranform.v0);
        const UMath::Vector3 &avoidable_forward = UMath::Vector4To3(tranform.v2);

        float elevation = avoidable_position.y - my_cookie.Centre.y;
        if (elevation != bClamp(elevation, -5.0f, 5.0f))
            continue;

        UMath::Vector3 avoidable_dimension;
        avoidable_body->GetDimension(avoidable_dimension);

        IVehicle *his_vehicle;
        avoidable_body->QueryInterface(&his_vehicle);
        const DriverClass his_class = his_vehicle ? his_vehicle->GetDriverClass() : DRIVER_NONE;
        // const bool he_is_player;
        // TODO const bool
        int he_is_traffic;
        if (!his_vehicle)
            he_is_traffic = 0;
        else
            he_is_traffic = his_class == DRIVER_TRAFFIC || his_class == DRIVER_NONE;
        // const bool he_is_airacer;

        if (is_racer && he_is_traffic && UMath::Abs(avoidable_right.x * my_cookie.Forward.x + avoidable_right.z * my_cookie.Forward.y) > 0.707f &&
            his_vehicle && VehicleClass::TRAILER == his_vehicle->GetVehicleClass()) {
            UMath::ScaleAdd(avoidable_forward, -6.0f, avoidable_position, avoidable_position);
            avoidable_dimension.x = 1.8f;
            avoidable_dimension.z = 1.8f;
        }

        UMath::Vector3 avoidable_to_me;
        UMath::Sub(avoidable_position, my_position, avoidable_to_me);

        // TODO which one?
        // const float his_extent =
        //             UMath::Abs(my_cookie.Forward.x * avoidable_right.x + my_cookie.Forward.y * avoidable_right.z) * avoidable_dimension.z +
        //             (avoidable_dimension.x + 0.5f) +
        //             UMath::Abs(my_cookie.Forward.x * avoidable_forward.x + my_cookie.Forward.y * avoidable_forward.z) * avoidable_dimension.x;

        const float his_extent =
            avoidable_dimension.x + 0.5f +
            UMath::Abs(my_cookie.Forward.x * avoidable_forward.x + my_cookie.Forward.y * avoidable_forward.z) * avoidable_dimension.z +
            UMath::Abs(my_cookie.Forward.x * avoidable_right.x + my_cookie.Forward.y * avoidable_right.z) * avoidable_dimension.x;

        const float my_extent = my_dimension.x + 0.5f + my_dimension.z;
        const float dist_ahead = avoidable_to_me.x * my_cookie.Forward.x + avoidable_to_me.z * my_cookie.Forward.y;
        const float extent_side = UMath::Abs(UMath::Dot(my_right, avoidable_forward)) * avoidable_dimension.z + my_dimension.x +
                                  UMath::Abs(UMath::Dot(my_right, avoidable_right)) * avoidable_dimension.x;
        const float dist_side = UMath::Abs(UMath::Dot(avoidable_to_me, my_right));

        unsigned int cut_flags = 0;
        if (is_traffic || my_speed < 20.0f) {
            if (dist_ahead + his_extent < my_extent)
                cut_flags = 2;
        } else if (dist_ahead - his_extent <= my_extent) {
            cut_flags = 2;
        }

        if (dist_ahead < -(my_extent + his_extent))
            continue;
        if (dist_ahead + his_extent < 0.0f && dist_side < extent_side)
            continue;

        UMath::Vector3 his_velocity;
        avoidable_body->GetLinearVelocity(his_velocity);

        const float gap_ahead = UMath::Max(dist_ahead - (my_extent + his_extent), 0.0f);

        UMath::Vector3 my_nose = my_position;
        const float my_nose_ahead = UMath::Min(my_extent, gap_ahead);
        my_nose.x += my_cookie.Forward.x * my_nose_ahead;
        my_nose.z += my_cookie.Forward.y * my_nose_ahead;

        UMath::Vector3 his_nose = avoidable_position;
        const float his_nose_ahead = UMath::Min(his_extent, gap_ahead);
        his_nose.x -= my_cookie.Forward.x * his_nose_ahead;
        his_nose.z -= my_cookie.Forward.y * his_nose_ahead;

        float closing_speed = 0.0f;
        float approach_time = TimeToClosestApproach(my_nose, my_velocity, his_nose, his_velocity, &closing_speed);

        bool blocked_traffic = false;
        if (cut_flags == 0) {
            if (closing_speed <= 0.0f) {
                float safe_distahead = my_extent + his_extent;
                if (is_traffic) {
                    safe_distahead += my_speed * 0.5f + 2 * my_extent;
                }
                if (dist_ahead > safe_distahead)
                    continue;
                if (is_traffic && dist_side > (extent_side + 1.0f))
                    continue;
            }
            blocked_traffic = is_traffic;
        }
        if (!blocked_traffic && approach_time >= 3.0f)
            continue;

        const float trailing_speed = his_velocity.x * my_cookie.Forward.x + his_velocity.z * my_cookie.Forward.y;
        const float time_offset = trailing_speed * approach_time - (my_extent + his_extent);
        UMath::Vector3 point_of_impact = avoidable_position;
        point_of_impact.x += my_cookie.Forward.x * time_offset;
        point_of_impact.z += my_cookie.Forward.y * time_offset;

        const int closest_cookie = ClosestCookieAhead(point_of_impact, cookies, num_cookies, nullptr);
        if (closest_cookie > -1) {
            const NavCookie &cookie = cookies[closest_cookie];

            UMath::Vector3 right;
            UMath::Scale(avoidable_right, avoidable_dimension.x, right);
            UMath::Vector3 forward;
            UMath::Scale(avoidable_forward, avoidable_dimension.z, forward);

            bVector2 left_diagonal(forward.x - right.x, forward.z - right.z);
            bVector2 right_diagonal(right.x + forward.x, forward.z + right.z);
            bVector2 avoidable_velocity(his_velocity.x, his_velocity.z);
            float avoidable_delta_offset = bCross(&avoidable_velocity, reinterpret_cast<const bVector2 *>(&cookie.Forward));

            if (closest_cookie < closest_avoidable && dist_ahead > (my_extent + his_extent)) {
                fOccludingTrailSpeed = trailing_speed;
                closest_avoidable = closest_cookie;
            }

            UMath::Vector3 cut_to_position = point_of_impact;
            float offset_change = avoidable_delta_offset * bClamp(approach_time, 0.0f, 1.0f);
            cut_to_position.x += offset_change * 0.8f * cookie.Forward.y;
            cut_to_position.z -= offset_change * 0.8f * cookie.Forward.x;

            bVector2 cookie_to_avoidable(cut_to_position.x - cookie.Centre.x, cut_to_position.z - cookie.Centre.z);
            bVector2 cookie_to_me(my_position.x - cookie.Centre.x, my_position.z - cookie.Centre.z);
            // float TODO = offset_change * 0.19999999f; // this constant is used somewhere, but where?
            float avoidable_d = bDot(&cookie_to_avoidable, reinterpret_cast<const bVector2 *>(&cookie.Forward));
            float my_d = bDot(&cookie_to_me, reinterpret_cast<const bVector2 *>(&cookie.Forward));
            float avoidable_ahead = avoidable_d - my_d;
            float close_factor = UMath::Ramp(avoidable_ahead, -6.0f, 6.0f);
            float extra_width = offset_change * 0.2f;
            float avoidable_offset = bCross(reinterpret_cast<const bVector2 *>(&cookie.Forward), &cookie_to_avoidable);
            float nav_cross = bCross(&nav_forward, reinterpret_cast<const bVector2 *>(&cookie.Forward));
            float right_projection = bCross(&right_diagonal, reinterpret_cast<const bVector2 *>(&cookie.Forward));
            float left_projection = bCross(&left_diagonal, reinterpret_cast<const bVector2 *>(&cookie.Forward));
            float lateral_projection = bClamp(approach_time, 0.0f, 1.0f);
            float new_current_offset = lateral_projection * close_factor * delta_offset * 0.2f + current_offset;
            new_current_offset += nav_cross;
            new_current_offset += nav_cross;
            float avoidable_half_width = bAbs(right_projection);
            avoidable_half_width = bMax(avoidable_half_width, bAbs(left_projection));
            // this var doesn't exist
            float adjusted_width = extra_width * close_factor + avoidable_half_width;
            float hole_punch_safety_margin = close_factor;
            if (is_drag) {
                hole_punch_safety_margin = close_factor * 0.8f;
            }
            float gap_right = cookie.RightOffset - avoidable_offset - adjusted_width;
            float gap_left = avoidable_offset - adjusted_width - cookie.LeftOffset;
            float gap_required = hole_punch_safety_margin + fVehicleHalfWidth;
            bool fit_right = gap_right > gap_required;
            bool fit_left = gap_left > gap_required;
            bool pass_left = new_current_offset < avoidable_offset;
            pass_left = fit_left ^ fit_right ? fit_left : pass_left;
            // TODO is this lateral_projection?
            float total_width = adjusted_width + fVehicleHalfWidth + hole_punch_safety_margin;

            int i = closest_cookie;
            for (; i < num_cookies; i++) {
                NavCookie &this_cookie = cookies[i];
                if (!CookieCutter(this_cookie, cut_to_position, total_width, pass_left, cut_flags) && i == closest_cookie)
                    break;

                UMath::Vector2 delta;
                delta.x = point_of_impact.x - this_cookie.Centre.x;
                delta.y = point_of_impact.y - this_cookie.Centre.y;
                float dist_to_tail = 2 * (my_extent + his_extent) + delta.x * this_cookie.Forward.x + delta.y * this_cookie.Forward.y;
                if (dist_to_tail < 0.0f)
                    break;
            }
        }
    }

    ClampCookieCentres(cookies, num_cookies);
}

// UNSOLVED, but should be functionally matching
void WRoadNav::UpdateOccludedPosition(bool occlude_avoidables) {
    if (!bCookieTrail)
        return;
    HasCookieTrail(); // unused?

    nRoadOcclusion = 0;
    nAvoidableOcclusion = 0;
    fOccludingTrailSpeed = 0.0f;
    bOccludedFromBehind = false;

    ISimable *simable = pAIVehicle ? pAIVehicle->GetSimable() : nullptr;
    IRigidBody *car = simable ? simable->GetRigidBody() : nullptr;
    if (car == nullptr)
        return;

    int num_cookies = pCookieTrail->Count();
    if (num_cookies <= 0)
        return;

    UMath::Vector3 car_forward_3d;
    car->GetForwardVector(car_forward_3d);
    UMath::Vector3 car_position_3d;
    const UMath::Vector3 &car_velocity_3d = car->GetLinearVelocity();
    UMath::ScaleAdd(car_forward_3d, 0.0f, car->GetPosition(), car_position_3d);

    bVector2 car_position(car_position_3d.x, car_position_3d.z);

    bool traffic = (GetNavType() == kTypeTraffic);
    float look_min = 2.0f;
    float look_max = traffic ? 4.0f : 8.0f;
    float out_scale = 2.0f;
    float out_bounds = traffic ? 1.5f : 1.0f;

    int n = nCookieIndex;
    float current_dot = 0.0f;
    float look_ahead = look_min;
    for (; n < num_cookies; n++) {
        const NavCookie &cookie = pCookieTrail->NthOldest(n);
        bVector2 cookie_to_car = car_position - bVector2(cookie.Centre.x, cookie.Centre.z);
        float dot = bDot(reinterpret_cast<const bVector2 *>(&cookie.Forward), &cookie_to_car);
        if (dot >= 0.0f) {
            nCookieIndex = n;
            current_dot = dot;
            float offset = bCross(&cookie_to_car, reinterpret_cast<const bVector2 *>(&cookie.Forward));
            float out_of_bounds = out_bounds + bMax(offset - cookie.RightOffset, cookie.LeftOffset - offset);
            look_ahead = bMin(look_max, out_scale * bMax(0.0f, out_of_bounds) + look_ahead);
        }
        if (dot < -look_ahead)
            break;
    }

    const NavCookie &current_cookie = pCookieTrail->NthOldest(nCookieIndex);
    int next_cookie_index = nCookieIndex + 1;

    if (next_cookie_index < num_cookies) {
        const NavCookie &next_cookie = pCookieTrail->NthOldest(next_cookie_index);
        bVector2 cookie_to_car = car_position - bVector2(next_cookie.Centre.x, next_cookie.Centre.z);
        float sum = current_dot + bAbs(bDot(reinterpret_cast<const bVector2 *>(&current_cookie.Forward), &cookie_to_car));
        float current_blend = sum > 1e-3f ? (1.0f - current_dot / sum) : 1.0f;
        float next_blend = 1.0f - current_blend;

        UMath::Lerp(current_cookie.Left, next_cookie.Left, current_blend, mCurrentCookie.Left);
        UMath::Lerp(current_cookie.Right, next_cookie.Right, current_blend, mCurrentCookie.Right);
        UMath::Lerp(current_cookie.Forward, next_cookie.Forward, current_blend, mCurrentCookie.Forward);

        UMath::Normalize(mCurrentCookie.Forward);

        mCurrentCookie.Centre.x = (current_cookie.Centre.x * current_blend) + (next_cookie.Centre.x * next_blend);
        mCurrentCookie.Centre.y = (current_cookie.Centre.y * current_blend) + (next_cookie.Centre.y * next_blend);
        mCurrentCookie.Centre.z = (current_cookie.Centre.z * current_blend) + (next_cookie.Centre.z * next_blend);

        mCurrentCookie.LeftOffset = UMath::Lerp(current_cookie.LeftOffset, next_cookie.LeftOffset, current_blend);
        mCurrentCookie.RightOffset = UMath::Lerp(current_cookie.RightOffset, next_cookie.RightOffset, current_blend);

        int next_segment_number = next_cookie.SegmentNumber;
        int current_segment_number = current_cookie.SegmentNumber;

        if (next_segment_number == current_segment_number) {
            mCurrentCookie.SegmentNumber = next_segment_number;
            mCurrentCookie.SegmentNodeInd = current_cookie.SegmentNodeInd;
            mCurrentCookie.SetSegmentParameter(UMath::Lerp(current_cookie.GetSegmentParameter(), next_cookie.GetSegmentParameter(), current_blend));
        } else {
            WRoadNetwork &rn = WRoadNetwork::Get();
            const WRoadSegment *next_segment = rn.GetSegment(next_segment_number);
            const WRoadSegment *current_segment = rn.GetSegment(current_segment_number);
            float next_segment_length = next_segment->GetLength();
            float current_segment_length = current_segment->GetLength();
            float next_dist = next_segment_length * next_cookie.GetSegmentParameter();
            float current_dist = current_segment_length * (current_cookie.GetSegmentParameter() - 1.0f);
            float distance = UMath::Lerp(current_dist, next_dist, current_blend);
            if (distance < 0.0f) {
                mCurrentCookie.SegmentNumber = current_segment_number;
                mCurrentCookie.SegmentNodeInd = current_cookie.SegmentNodeInd;
                mCurrentCookie.SetSegmentParameter((distance + current_segment_length) / current_segment_length);
            } else {
                mCurrentCookie.SegmentNumber = next_segment_number;
                mCurrentCookie.SegmentNodeInd = next_cookie.SegmentNodeInd;
                mCurrentCookie.SetSegmentParameter(distance / next_segment_length);
            }
        }
    } else {
        mCurrentCookie.Forward = current_cookie.Forward;
        mCurrentCookie.LeftOffset = current_cookie.LeftOffset;
        mCurrentCookie.RightOffset = current_cookie.RightOffset;
        mCurrentCookie.SegmentNumber = current_cookie.SegmentNumber;
        mCurrentCookie.SegmentNodeInd = current_cookie.SegmentNodeInd;
        UMath::ScaleAdd(current_cookie.Left, current_dot, current_cookie.Forward, mCurrentCookie.Left);
        UMath::ScaleAdd(current_cookie.Right, current_dot, current_cookie.Forward, mCurrentCookie.Right);
        mCurrentCookie.Centre.x = current_cookie.Centre.x + current_dot * current_cookie.Forward.x;
        mCurrentCookie.Centre.z = current_cookie.Centre.z + current_dot * current_cookie.Forward.y;
        mCurrentCookie.Centre.y = current_cookie.Centre.y;
    }

    bVector2 cookie_to_car = car_position - bVector2(mCurrentCookie.Centre.x, mCurrentCookie.Centre.z);
    float current_offset = bCross(&cookie_to_car, reinterpret_cast<const bVector2 *>(&mCurrentCookie.Forward));
    mOutOfBounds = fVehicleHalfWidth + bMax(current_offset - mCurrentCookie.RightOffset, mCurrentCookie.LeftOffset - current_offset);

    if (n < num_cookies) {
        int first_index = n;
        int last_visible = n;
        int occluding_index = n;
        int left_limit_index = n;
        int right_limit_index = n;
        bVector2 *occluder = nullptr;
        bool left_occlusion = false;
        bVector2 left_limit;
        bVector2 right_limit;
        NavCookie cookies[33];
        bMemSet(&cookies[num_cookies], 0, sizeof(NavCookie));

        for (int i = n; i < num_cookies; i++) {
            cookies[i] = pCookieTrail->NthOldest(i);
        }

        bVector2 car_velocity(car_velocity_3d.x, car_velocity_3d.z);
        float delta_offset = bCross(&car_velocity, reinterpret_cast<bVector2 *>(&mCurrentCookie.Forward));

        if (occlude_avoidables) {
            HolePunchAvoidables(&cookies[n], num_cookies - n, current_offset, delta_offset);
        }

        UMath::ScaleAdd(car_velocity_3d,
#ifdef EA_BUILD_A124
                        0.06f,
#else
                        0.0f,
#endif
                        car_position_3d, car_position_3d);
        car_position = bVector2(car_position_3d.x, car_position_3d.z);

        bInitializeBoundingBox(&vCookieTrailBoxMin, &vCookieTrailBoxMax);

        for (; n <= num_cookies; n++) {
            const NavCookie &cookie = cookies[n];
            bVector2 left(cookie.Left.x, cookie.Left.y);
            bVector2 right(cookie.Right.x, cookie.Right.y);
            bVector2 car_to_left = left - car_position;
            bVector2 car_to_right = right - car_position;

            if (n < num_cookies) {
                bExpandBoundingBox(&vCookieTrailBoxMin, &vCookieTrailBoxMax, &left);
                bExpandBoundingBox(&vCookieTrailBoxMin, &vCookieTrailBoxMax, &right);
            }

            if (n == first_index) {
                left_limit = car_to_left;
                right_limit = car_to_right;
            } else {
                bool use_nav = (n == num_cookies);
                const UMath::Vector3 &centre_3d = use_nav ? GetPosition() : cookie.Centre;
                bVector2 centre(centre_3d.x, centre_3d.z);
                bVector2 car_to_centre = centre - car_position;

                if (!use_nav) {
                    if (bCross(&left_limit, &car_to_left) < 0.0f) {
                        left_limit = car_to_left;
                        left_limit_index = n;
                    }
                    if (bCross(&right_limit, &car_to_right) > 0.0f) {
                        right_limit = car_to_right;
                        right_limit_index = n;
                    }
                }

                if (bCross(&car_to_centre, &left_limit) < 0.0f) {
                    occluder = &left_limit;
                    occluding_index = left_limit_index;
                    left_occlusion = true;
                } else {
                    if (bCross(&car_to_centre, &right_limit) > 0.0f) {
                        occluder = &right_limit;
                        occluding_index = right_limit_index;
                        last_visible = last_visible;
                        left_occlusion = false;
                    } else {
                        last_visible = n;
                    }
                }
            }

            if (0.0f < bCross(&left_limit, &right_limit))
                break;
        }

        if (last_visible < num_cookies && occluder) {
            const NavCookie &apex_cookie = cookies[occluding_index];
            fApexPosition = UVector3(car_position_3d) + UMath::Vector3Make(occluder->x, apex_cookie.Centre.y - car_position_3d.y, occluder->y);

            nRoadOcclusion = (cookies[occluding_index].Flags & 1) ? 0 : (left_occlusion ? -1 : 1);
            nAvoidableOcclusion = (cookies[occluding_index].Flags & 1) ? (left_occlusion ? -1 : 1) : 0;
            bOccludedFromBehind = nAvoidableOcclusion && (cookies[occluding_index].Flags & 2);

            if (IsOccluded()) {
                float apex_width = bAbs(apex_cookie.RightOffset - apex_cookie.LeftOffset);
                bVector2 apex_2d(fApexPosition.x, fApexPosition.z);
                bVector2 apex_to_car = bNormalize(apex_2d - car_position);
                bVector2 apex_to_nav = bNormalize(apex_2d - bVector2(fPosition.x, fPosition.z));

                float dist_to_apex = bLength(apex_2d - car_position);

                bVector2 desired_position = apex_2d + apex_to_nav * dist_to_apex;
                bVector2 perp = bNormalize(apex_to_car + apex_to_nav);

                float projection = bDot(desired_position - apex_2d, perp);
                projection = bMin(projection, apex_width);

                if (nAvoidableOcclusion != 0) {
                    float my_trailingspeed = bDot(car_velocity, apex_to_car);
                    float closing_speed = my_trailingspeed - fOccludingTrailSpeed;
                    float ratio = UMath::Ramp(closing_speed, 0.0f, 2 * my_trailingspeed);
                    projection *= 2 * ratio;
                    projection = bMin(projection, apex_width);
                }

                perp *= projection;
                fOccludedPosition.x = perp.x + fApexPosition.x;
                fOccludedPosition.y = fApexPosition.y;
                fOccludedPosition.z = perp.y + fApexPosition.z;
            } else {
                fOccludedPosition = fPosition;
            }
        }
        if (nAvoidableOcclusion == 0) {
            bOccludedFromBehind = false;
            fOccludingTrailSpeed = 0.0f;
        }
    }
}

float WRoadNav::FindClosestOnSpline(const UMath::Vector3 &point, UMath::Vector3 &intersectPoint, float &timeStep, float incStep, int segInd) {
    UMath::Vector3 newIntersectPoint;
    UMath::Vector3 pointToIntersect;
    UMath::Sub(intersectPoint, point, pointToIntersect);

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    WRoadSegment *segment = const_cast<WRoadSegment *>(roadNetwork.GetSegment(segInd));

    float timeStepInc = incStep / UMath::Max(1.0f, segment->GetLength());

    UMath::Vector3 segmentForwardVec;
    roadNetwork.GetSegmentForwardVector(segment->fIndex, segmentForwardVec);

    if (UMath::Dot(segmentForwardVec, pointToIntersect) > 0.0f) {
        timeStepInc = -timeStepInc;
    }

    float currDistance = UMath::Distance(intersectPoint, point);

    float oldTimeStep;
    {
        float newTimeStep = timeStep + timeStepInc;
        if (newTimeStep > 0.0f && newTimeStep < 1.0f) {
            while (true) {
                roadNetwork.GetPointOnSegment(*segment, newTimeStep, newIntersectPoint);

                float distToNewIntersect = UMath::Distance(newIntersectPoint, point);

                if (distToNewIntersect > currDistance)
                    break;

                intersectPoint = newIntersectPoint;
                timeStep = newTimeStep;
                newTimeStep += timeStepInc;
                currDistance = distToNewIntersect;

                if (newTimeStep <= 0.0f || newTimeStep >= 1.0f)
                    break;
            }
        }
    }

    return currDistance;
}

int WRoadNav::FindClosestSegmentInd(const UMath::Vector3 &point, const UMath::Vector3 &dir, float dirWeight, UMath::Vector3 &closestPoint,
                                    float &time) {
    typedef UTL::Std::set<short, _type_set> SEGMENT_SET;

    const WGrid &grid = WGrid::Get();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    short segInd = -1;
    bool found = false;
    bool some_segments;
    float foundScore = 20000.0f;
    UTL::FastVector<unsigned int, 16> node_indices;
    UMath::Vector3 ndir;

    node_indices.reserve(64);
    WGrid::Get().FindNodes(point, 32.0f, node_indices);

    ndir = dir;
    UMath::Normalize(ndir);

    SEGMENT_SET segment_set;
    for (UTL::FastVector<unsigned int, 16>::iterator iter = node_indices.begin(); iter != node_indices.end(); ++iter) {
        WGridNode *grid_node = grid.fNodes[*iter];
        if (grid_node) {
            unsigned int numSegments = grid_node->GetElemTypeCount(WGrid_kRoadSegment);
            for (unsigned int i = 0; i < numSegments; ++i) {
                segment_set.insert(static_cast<short>(grid_node->GetElemType(i, WGrid_kRoadSegment)));
            }
        }
    }

    for (SEGMENT_SET::const_iterator it = segment_set.begin(); it != segment_set.end(); ++it) {
        short index = *it;
        if (index >= static_cast<int>(roadNetwork.GetNumSegments()))
            continue;
        WRoadSegment *segment = const_cast<WRoadSegment *>(roadNetwork.GetSegment(index));

        if (bDecisionFilter && segment->IsDecision())
            continue;
        if (bRaceFilter && !segment->IsInRace())
            continue;
        if (bTrafficFilter && !segment->IsTrafficAllowed())
            continue;
        if (bCopFilter && !segment->ShouldCopsConsider())
            continue;

        UMath::Vector3 intersectPoint;
        float timeStep = WRoadNetwork::Get().GetSegmentPointIntersect(*segment, point, intersectPoint, true);

        float currDistance;
        if (segment->fFlags & (1 << 8)) {
            WRoadNetwork::Get().GetPointOnSegment(*segment, timeStep, intersectPoint);
            FindClosestOnSpline(point, intersectPoint, timeStep, 1.0f, static_cast<int>(segment->fIndex));
            currDistance = FindClosestOnSpline(point, intersectPoint, timeStep, 0.25f, static_cast<int>(segment->fIndex));
        } else {
            currDistance = UMath::Distance(point, intersectPoint);
        }

        float currScore = currDistance;
        if (dirWeight > 0.0f) {
            UMath::Vector3 intersectDir;
            roadNetwork.GetPointAndVecOnSegment(*segment, timeStep, intersectPoint, intersectDir);

            UMath::Vector3 intersectRight = UMath::Vector3Make(intersectDir.z, 0.0f, -intersectDir.x);
            UMath::Normalize(intersectRight);

            UMath::Vector3 offset;
            UMath::Sub(point, intersectPoint, offset);
            float sideDistance = UMath::Dot(intersectRight, offset);
            float rightedge[2];
            float leftedge[2];

            for (int i = 0; i < 2; i++) {
                bool inverted = segment->IsProfileInverted(i);
                const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, i);

                leftedge[i] = profile->GetRelativeLaneOffset(0, inverted) - profile->GetLaneWidth(0, inverted) * 0.5f;
                rightedge[i] =
                    profile->GetRelativeLaneOffset(profile->fNumZones - 1, inverted) + profile->GetLaneWidth(profile->fNumZones - 1, inverted) * 0.5f;
            }

            float right = rightedge[0] + timeStep * (rightedge[1] - rightedge[0]);
            float left = leftedge[0] + timeStep * (leftedge[1] - leftedge[0]);

            float oldSideDistance = sideDistance;
            if (sideDistance > right) {
                sideDistance -= right;
            } else if (sideDistance < left) {
                sideDistance -= left;
            } else {
                sideDistance = 0.0f;
            }

            UMath::ScaleAdd(intersectRight, sideDistance - oldSideDistance, offset, offset);
            currDistance = UMath::Length(offset);

            UMath::Vector3 start = roadNetwork.fNodes[segment->fNodeIndex[0]].fPosition;
            UMath::Vector3 end = roadNetwork.fNodes[segment->fNodeIndex[1]].fPosition;
            UMath::Vector3 segdir;
            UMath::Sub(end, start, segdir);
            UMath::Normalize(segdir);

            float dot = UMath::Dot(ndir, segdir);
            if (!(segment->fFlags & kRoadSegmentOneWay)) {
                dot = UMath::Abs(dot);
            }

            float currDirScore = (1.0f - dot) * 10.0f;
            currScore = currDistance + dirWeight * currDirScore;
        }

        if (!found || currScore < foundScore) {
            found = true;
            closestPoint = intersectPoint;
            time = timeStep;
            foundScore = currScore;
            segInd = index;
        }
    }

    return segInd;
}

void WRoadNav::InitFromOtherNav(WRoadNav *other_nav, bool flip_direction) {
    if (other_nav == this) {
        return;
    }
    fSegmentInd = other_nav->GetSegmentInd();
    fNodeInd = other_nav->GetNodeInd();
    fSegTime = other_nav->GetSegmentTime();
    SetLaneInd(other_nav->GetLaneInd());
    SetLaneOffset(other_nav->GetLaneOffset());

    fValid = other_nav->fValid;
    if (flip_direction) {
        fNodeInd ^= 1;
        fSegTime = 1.0f - fSegTime;
    }
    WRoadNetwork &road_network = WRoadNetwork::Get();
    const WRoadSegment *segment = road_network.GetSegment(fSegmentInd);
    SetStartEndPos(*segment, GetLaneOffset());
    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
    ResetCookieTrail();
}

void WRoadNav::InitLaneOffset(const UMath::Vector3 &vehicle_pos) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());

    UMath::Vector3 nav_forward;
    UMath::Unit(GetForwardVector(), nav_forward);

    const UMath::Vector3 &nav_position = GetPosition();
    UMath::Vector3 nav_to_vehicle = UVector3(vehicle_pos) - nav_position;
    UMath::Vector2 nav_forward_2d = UMath::Vector2Make(nav_forward.x, nav_forward.z);
    UMath::Vector2 nav_to_vehicle_2d = UMath::Vector2Make(nav_to_vehicle.x, nav_to_vehicle.z);

    float current_offset = UMath::Cross(nav_to_vehicle_2d, nav_forward_2d);
    SetStartEndPos(*segment, current_offset);
    SetLaneOffset(current_offset);

    if (fNavType == kTypeTraffic) {
        int end = GetNodeInd();
        bool forward = end == 1;
        int start = end ^ 1;
        bool end_inverted = segment->IsProfileInverted(end);
        bool start_inverted = segment->IsProfileInverted(start);

        const WRoadProfile *end_profile = roadNetwork.GetSegmentProfile(*segment, end);
        const WRoadProfile *start_profile = roadNetwork.GetSegmentProfile(*segment, start);

        int num_start_traffic_lanes = start_profile->GetNumTrafficLanes(forward, start_inverted);
        int num_end_traffic_lanes = end_profile->GetNumTrafficLanes(forward, end_inverted);
        int num_traffic_lanes = bMin(num_start_traffic_lanes, num_end_traffic_lanes);

        if (num_traffic_lanes > 0) {
            bool foundClosest = false;
            float closestDist = 0.0f;
            float startOffset = 0.0f;
            float endOffset = 0.0f;
            float current_offset = GetLaneOffset();

            for (int i = 0; i < num_traffic_lanes; i++) {
                int end_lane = end_profile->GetNthTrafficLaneFromCurb(i, forward, end_inverted);
                int start_lane = start_profile->GetNthTrafficLaneFromCurb(i, forward, start_inverted);

                float parameter = GetSegmentTime();

                float end_offset = end_profile->GetLaneOffset(end_lane, false);
                float start_offset = start_profile->GetLaneOffset(start_lane, false);

                float lane_offset = UMath::Lerp(start_offset, end_offset, parameter);
                float distance = bAbs(current_offset - lane_offset);
                if (!foundClosest || distance < closestDist) {
                    SetLaneOffset(lane_offset);
                    SetLaneInd(static_cast<char>(end_lane));
                    foundClosest = true;
                    startOffset = start_offset;
                    endOffset = end_offset;
                    closestDist = distance;
                }
            }
            SetStartEndPos(*segment, startOffset, endOffset);
        }
    }
    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
}

// UNSOLVED, stack problems
void WRoadNav::InitAtSegment(short segInd, char laneInd, float timeStep) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();

    fValid = true;
    fSegmentInd = segInd;
    fDeadEnd = 0;

    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);

    UMath::Vector3 vec;
    roadNetwork.GetSegmentForwardVector(segInd, vec);

    if (!roadNetwork.GetSegmentTrafficLaneRightSide(*segment, laneInd) && !(segment->fFlags & 0x40)) {
        fNodeInd = 0;
        fForwardVector = UMath::Vector3Make(-vec.x, -vec.y, -vec.z);
        fSegTime = fabsf(1.0f - timeStep);
    } else {
        fNodeInd = 1;
        fForwardVector = UMath::Vector3Make(vec.x, vec.y, vec.z);
        fSegTime = timeStep;
    }

    fStartPos = roadNetwork.GetNode(segment->fNodeIndex[fNodeInd == 0])->fPosition;
    fEndPos = roadNetwork.GetNode(segment->fNodeIndex[fNodeInd])->fPosition;

    SetLaneInd(laneInd);
    SetLaneOffset(0.0f);

    {
        SetLaneInd(laneInd);
        const WRoadNode *nodePtr[2];
        roadNetwork.GetSegmentNodes(*segment, nodePtr);

        const WRoadProfile *profile = roadNetwork.GetProfile(nodePtr[fNodeInd == 0]->fProfileIndex);
        float startOffset = profile->GetRawLaneOffset(laneInd);

        profile = roadNetwork.GetProfile(nodePtr[fNodeInd]->fProfileIndex);
        float endOffset = profile->GetRawLaneOffset(laneInd);

        float laneOffset = startOffset + (endOffset - startOffset) * fSegTime;
        SetLaneOffset(laneOffset);

        SetStartEndPos(*segment, startOffset, endOffset);
    }

    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
    ResetCookieTrail();
}

void WRoadNav::InitAtSegment(short segInd, float timeStep, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    UMath::Vector3 segmentForwardVector;

    fValid = true;
    fSegmentInd = segInd;
    fDeadEnd = 0;

    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);

    roadNetwork.GetSegmentForwardVector(*segment, segmentForwardVector);
    float facingDot = UMath::Dot(dir, segmentForwardVector);
    bool backward = facingDot < 0.0f;
    if (bRaceFilter) {
        backward = !segment->RaceRouteForward();
    }

    if (backward) {
        fNodeInd = 0;
        UMath::Scale(segmentForwardVector, -1.0f, fForwardVector);
        fSegTime = UMath::Abs(1.0f - timeStep);
        fStartPos = roadNetwork.GetNode(segment->fNodeIndex[1])->fPosition;
        fEndPos = roadNetwork.GetNode(segment->fNodeIndex[0])->fPosition;
    } else {
        fNodeInd = 1;
        fForwardVector = segmentForwardVector;
        fSegTime = timeStep;
        fStartPos = roadNetwork.GetNode(segment->fNodeIndex[0])->fPosition;
        fEndPos = roadNetwork.GetNode(segment->fNodeIndex[1])->fPosition;
    }

    SetLaneOffset(0.0f);
    SetStartEndPos(*segment, 0.0f);

    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);

    if (!forceCenterLane) {
        InitLaneOffset(pos);
    }
    ResetCookieTrail();
}

void WRoadNav::InitAtSegment(short segInd, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane) {
    WRoadNetwork &rn = WRoadNetwork::Get();
    const WRoadSegment *segment = rn.GetSegment(segInd);
    float timeStep = rn.GetSegmentPointIntersect(*segment, pos, fPosition, true);
    if (segment->fFlags & (1 << 8)) {
        rn.GetPointOnSegment(*segment, timeStep, fPosition);
        FindClosestOnSpline(pos, fPosition, timeStep, 1.0f, static_cast<int>(segment->fIndex));
        FindClosestOnSpline(pos, fPosition, timeStep, 0.25f, static_cast<int>(segment->fIndex));
    }
    InitAtSegment(segInd, timeStep, pos, dir, forceCenterLane);
}

bool WRoadNav::IsWrongWay() const {
    if (!GetRaceFilter()) {
        return false;
    }
    if (!IsValid()) {
        return false;
    }
    bool seg_forward = (fNodeInd == 1);
    const WRoadSegment *segment = GetSegment();

    return segment->IsInRace() && (segment->RaceRouteForward() ^ seg_forward);
}

bool WRoadNav::FindClosestOnPath(const UMath::Vector3 &position, UMath::Vector3 *found_position, UMath::Vector3 *found_direction,
                                 unsigned short *found_segment, float *found_interval) const {
    if (pPathSegments && nPathSegments > 0) {
        float min_dist_sq = -1.0f;
        int found_segment_index = -1;
        float best_interval = 0.0f;
        UMath::Vector3 best_position;
        WRoadNetwork &roadNetwork = WRoadNetwork::Get();
        int i;
        for (i = 0; i < nPathSegments; i++) {
            const WRoadSegment *segment = roadNetwork.GetSegment(pPathSegments[i]);
            UMath::Vector3 intersect;
            float d = roadNetwork.GetSegmentPointIntersect(*segment, position, intersect, true);
            float dist_square = UMath::DistanceSquare(intersect, position);
            if (min_dist_sq < 0.0f || dist_square < min_dist_sq) {
                best_position = intersect;
                found_segment_index = i;
                min_dist_sq = dist_square;
                best_interval = d;
            }
        }
        if (found_segment_index >= 0) {
            unsigned short segment = pPathSegments[found_segment_index];
            const WRoadNode *this_nodes[2];
            const WRoadSegment *this_segment = roadNetwork.GetSegment(segment);
            int tail;

            roadNetwork.GetSegmentNodes(*this_segment, this_nodes);

            unsigned short next_segment =
                found_segment_index < nPathSegments - 1 ? pPathSegments[found_segment_index + 1] : static_cast<unsigned short>(-1);
            unsigned short prev_segment = found_segment_index >= 1 ? pPathSegments[found_segment_index - 1] : static_cast<unsigned short>(-1);

            USpline roadSpline;
            roadNetwork.BuildSegmentSpline(*this_segment, roadSpline);
            UMath::Vector4 tangent;
            roadSpline.EvaluateTangent(best_interval, tangent);

            if (found_direction) {
                *found_direction = UMath::Vector4To3(tangent);
                if (this_nodes[0]->IsSegment(next_segment) || this_nodes[1]->IsSegment(prev_segment)) {
                    UMath::Negate(*found_direction);
                }
            }

            if (found_segment) {
                *found_segment = segment;
            }
            if (found_position) {
                *found_position = best_position;
            }
            if (found_interval) {
                *found_interval = best_interval;
            }
            return true;
        }
    }
    return false;
}

void WRoadNav::InitAtPath(const UMath::Vector3 &position, bool forceCenterLane) {
    UMath::Vector3 found_position;
    float found_interval = 0.0f;
    unsigned short found_segment;
    UMath::Vector3 found_direction;
    if (!FindClosestOnPath(position, &found_position, &found_direction, &found_segment, &found_interval)) {
        fSegmentInd = 0;
        fValid = false;
    } else {
        InitAtSegment(static_cast<short>(found_segment), found_interval, found_position, found_direction, forceCenterLane);
    }
}

void WRoadNav::InitAtPoint(const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane, float dirWeight) {
    float segment_parameter;
    int segment_number = FindClosestSegmentInd(pos, dir, dirWeight, fPosition, segment_parameter);
    if (segment_number == -1) {
        fSegmentInd = 0;
        fValid = false;
    } else {
        InitAtSegment(static_cast<short>(segment_number), segment_parameter, pos, dir, forceCenterLane);
    }
}

void WRoadNav::SetControlPos(const WRoadSegment &segment, bool startControl) {
    if (segment.fFlags & 0x100) {
        bool forward = fNodeInd != 0;
        bool which_end = startControl ^ forward;
        WRoadNetwork &road_network = WRoadNetwork::Get();
        const UMath::Vector3 &nodePos = road_network.GetNode(segment.fNodeIndex[which_end])->fPosition;
        const UMath::Vector3 &otherPos = road_network.GetNode(segment.fNodeIndex[which_end ^ 1])->fPosition;

        UMath::Vector3 handle;
        segment.GetControl(which_end, handle);
        UMath::Vector3 controlPos = UVector3(nodePos) + UVector3(handle);

        float original_distance = UMath::Max(0.01f, UMath::Distance(nodePos, otherPos));
        float new_distance = UMath::Max(0.01f, UMath::Distance(fStartPos, fEndPos));
        float scale = new_distance / original_distance;

        UMath::ScaleAdd(handle, scale, startControl ? fStartPos : fEndPos, startControl ? fStartControl : fEndControl);

        if (bCookieTrail) {
            float left_scale = UMath::Max(0.01f, UMath::Distance(fLeftStartPos, fLeftEndPos)) / original_distance;
            float right_scale = UMath::Max(0.01f, UMath::Distance(fRightStartPos, fRightEndPos)) / original_distance;

            UMath::ScaleAdd(handle, left_scale, startControl ? fLeftStartPos : fLeftEndPos, startControl ? fLeftStartControl : fLeftEndControl);
            UMath::ScaleAdd(handle, right_scale, startControl ? fRightStartPos : fRightEndPos, startControl ? fRightStartControl : fRightEndControl);
        }
    }
}

void WRoadNav::SetStartEndControls(const WRoadSegment &segment) {
    SetControlPos(segment, true);
    SetControlPos(segment, false);
}

bool WRoadNav::IsDrivable(int lane_type) const {
    return (drivable_lanes[fLaneType] >> lane_type) & 1;
}

bool WRoadNav::IsSelectable(int lane_type) const {
    return (selectable_lanes[fLaneType] >> lane_type) & 1;
}

void WRoadNav::DetermineVehicleHalfWidth() {
    fVehicleHalfWidth = 1.0f;
    if (pAIVehicle != nullptr) {
        IBody *body;
        if (pAIVehicle->GetOwner()->QueryInterface(&body)) {
            UMath::Vector3 dimension;
            body->GetDimension(dimension);
            fVehicleHalfWidth = dimension.x;
        }

        if (GRaceStatus::IsDragRace()) {
            IVehicle *ivehicle;
            if (pAIVehicle->GetOwner()->QueryInterface(&ivehicle)) {
                if (VehicleClass::TRACTOR == ivehicle->GetVehicleClass()) {
                    fVehicleHalfWidth += 2.0f;
                }
            }
        }
    }
}

void WRoadNav::SetVehicle(AIVehicle *ai_vehicle) {
    pAIVehicle = ai_vehicle;
    DetermineVehicleHalfWidth();
}

void WRoadNav::SetBoundPos(const WRoadSegment &segment, float offset, bool start) {
    bool forward = fNodeInd != 0;
    bool which_end = start ^ forward;
    WRoadNetwork &road_network = WRoadNetwork::Get();
    const WRoadNode *node = road_network.GetNode(segment.fNodeIndex[which_end]);
    const UMath::Vector3 &nodePos = node->fPosition;
    UMath::Vector3 rightVec;
    float sign = forward ? 1.0f : -1.0f;

    segment.GetRightVec(which_end, rightVec);

    if (bCookieTrail) {
        float vehicle_half_width = fVehicleHalfWidth;
        float left_offset = offset - vehicle_half_width * 0.5f;
        float right_offset = offset + vehicle_half_width * 0.5f;
        int nav_type = GetNavType();

        if (nav_type != kTypeTraffic) {
            left_offset = offset - 2.0f;
            right_offset = offset + 2.0f;

            const WRoadProfile *profile = road_network.GetSegmentProfile(segment, which_end);
            int num_lanes = profile->fNumZones;

            if (num_lanes > 0) {
                int closest_drivable = -1;
                float closest_offset = 0.0f;
                bool inverted = !forward ^ segment.IsProfileInverted(fNodeInd);
                int middle_lane = profile->GetMiddleZone(inverted);

                for (int lane_index = 0; lane_index < num_lanes; lane_index++) {
                    if (IsDrivable(profile->GetLaneType(lane_index, inverted))) {
                        float lane_offset = profile->GetLaneOffset(lane_index, inverted);
                        if (lane_index < middle_lane) {
                            lane_offset = -lane_offset;
                        }
                        float lane_distance = bAbs(offset - lane_offset);
                        if (closest_drivable < 0 || lane_distance < closest_offset) {
                            closest_drivable = lane_index;
                            closest_offset = lane_distance;
                        }
                    }
                }

                if (closest_drivable > -1) {
                    int left_lane = closest_drivable;
                    int right_lane = closest_drivable;

                    while (left_lane > 0) {
                        if (!IsDrivable(profile->GetLaneType(left_lane - 1, inverted)))
                            break;
                        left_lane--;
                    }

                    while (right_lane < num_lanes - 1) {
                        if (!IsDrivable(profile->GetLaneType(right_lane + 1, inverted)))
                            break;
                        right_lane++;
                    }

                    float left_lane_offset = profile->GetRelativeLaneOffset(left_lane, inverted);
                    float right_lane_offset = profile->GetRelativeLaneOffset(right_lane, inverted);
                    float safety_margin = vehicle_half_width + 1.5f;

                    left_offset = left_lane_offset - profile->GetLaneWidth(left_lane, inverted) * 0.5f + safety_margin;
                    right_offset = right_lane_offset + profile->GetLaneWidth(right_lane, inverted) * 0.5f - safety_margin;

                    float how_unsafe = bMax(0.0f, 2.0f * safety_margin - (right_offset - left_offset)) * 0.5f;

                    right_offset += how_unsafe;
                    left_offset -= how_unsafe;
                }

                offset = bClamp(offset, left_offset + 0.1f, right_offset - 0.1f);
            }
        }

        UMath::ScaleAdd(rightVec, sign * left_offset, nodePos, start ? fLeftStartPos : fLeftEndPos);
        UMath::ScaleAdd(rightVec, sign * right_offset, nodePos, start ? fRightStartPos : fRightEndPos);
    }

    UMath::ScaleAdd(rightVec, sign * offset, nodePos, start ? fStartPos : fEndPos);
}

void WRoadNav::SetStartEndPos(const WRoadSegment &segment, float startOffset, float endOffset) {
    // const bool end; // TODO
    // const bool start;
    SetBoundPos(segment, endOffset, false);
    SetBoundPos(segment, startOffset, true);
}

void WRoadNav::ChangeLanes(float new_lane_offset, float dist) {
    if (dist > 0.0f) {
        SetLaneOffset(fToLaneOffset);
        fToLaneOffset = new_lane_offset;
        fLaneChangeDist = dist;
        fLaneChangeInc = 0.0f;
    } else {
        float old_lane_offset = fLaneOffset;
        if (old_lane_offset != new_lane_offset) {
            SetLaneOffset(new_lane_offset);
            fLaneChangeDist = 0.0f;
            const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(fSegmentInd);
            if (segment->IsDecision() && fLaneType != kLaneStartingGrid) {
                SetBoundPos(*segment, new_lane_offset, false);
                SetControlPos(*segment, false);
            } else {
                SetStartEndPos(*segment, new_lane_offset, new_lane_offset);
                SetStartEndControls(*segment);
            }
            RebuildSplines(segment);
            EvaluateSplines(segment);
        }
    }
}

bool WRoadNav::ChangeDragDecision(register int left_right) {
    WRoadNetwork &rn = WRoadNetwork::Get();
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = rn.GetSegment(segment_number);

    if (segment->IsDecision()) {
        UMath::Vector2 ray;
        const WRoadNode *nodes[2];
        rn.GetSegmentNodes(*segment, nodes);
        int from_which_node = GetNodeInd() ^ 1;
        const WRoadNode *from_node = nodes[from_which_node];

        segment->GetForwardVec(from_which_node ^ 1, ray);
        if (from_which_node == 1) {
            UMath::Scale(ray, -1.0f);
        }
        UMath::Normalize(ray);

        int new_which_node = 0;
        float best = 0.7f;
        const WRoadSegment *new_segment = nullptr;
        float sign = left_right < 0 ? 1.0f : -1.0f;

        int num_segments = from_node->fNumSegments;
        for (int i = 0; i < num_segments; i++) {
            int departing_segment_number = from_node->fSegmentIndex[i];
            if (departing_segment_number == segment_number) {
                continue;
            }

            const WRoadSegment *departing_segment = rn.GetSegment(departing_segment_number);
            if (!departing_segment->IsDecision() || !departing_segment->IsInRace()) {
                continue;
            }
            UMath::Vector2 departing_ray;
            int to_which_node = from_node->fIndex == departing_segment->fNodeIndex[0];

            departing_segment->GetForwardVec(to_which_node, departing_ray);
            if (to_which_node == 0) {
                UMath::Scale(departing_ray, -1.0f);
            }
            UMath::Normalize(departing_ray);

            if (sign * Cross(ray, departing_ray) >= 0.0f) {
                float dot = UMath::Dot(ray, departing_ray);
                if (best < dot) {
                    best = dot;
                    new_which_node = to_which_node;
                    new_segment = departing_segment;
                }
            }
        }

        if (new_segment) {
            bool inverted = new_which_node == 1;
            int new_segment_index = new_segment->fIndex;
            inverted = (new_segment->IsProfileInverted(new_which_node) != false) != inverted;
            const WRoadProfile *new_profile = rn.GetSegmentProfile(*new_segment, new_which_node);
            int num_lanes = new_profile->fNumZones;
            int end = left_right >= 0 ? num_lanes : -1;
            int start = left_right >= 0 ? 0 : num_lanes - 1;
            int new_lane = new_profile->GetMiddleZone(inverted);

            for (int lane = start; lane != end; lane += left_right) {
                int lane_type = new_profile->GetLaneType(lane, inverted);
                if (IsDrivable(lane_type)) {
                    new_lane = lane;
                    break;
                }
            }

            float new_lane_offset = new_profile->GetRelativeLaneOffset(new_lane, inverted);
            SetLaneOffset(new_lane_offset);

            fNodeInd = new_which_node;
            fSegmentInd = new_segment_index;
            SetControlPos(*new_segment, false);
            SetBoundPos(*new_segment, new_lane_offset, false);
            RebuildSplines(new_segment);
            UMath::Vector3 new_spline_point;
            FindClosestOnSpline(fPosition, new_spline_point, fSegTime, 0.1f, new_segment_index);
            EvaluateSplines(new_segment);
            return true;
        }
    }
    return false;
}

bool WRoadNav::IncLane(int direction) {
    if (!IsValid()) {
        return false;
    }

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, GetNodeInd());

    bool backward = GetNodeInd() == 0;
    bool inverted = segment->IsProfileInverted(GetNodeInd());
    bool inverted_xor_backward = inverted ^ backward;

    int current_lane = 0;
    for (int n = 0; n < profile->fNumZones - 1; n++) {
        float width = profile->GetLaneWidth(n, inverted_xor_backward);
        float offset = profile->GetRelativeLaneOffset(n, inverted_xor_backward);
        if (fLaneOffset > width * 0.5f + offset) {
            current_lane++;
        }
    }

    do {
        if (direction > 0) {
            current_lane++;
        } else {
            current_lane--;
        }
        if (current_lane < 0 || current_lane >= profile->fNumZones) {
            return false;
        }
    } while (!IsSelectable(profile->GetLaneType(current_lane, inverted_xor_backward)));

    float new_offset = profile->GetRelativeLaneOffset(current_lane, inverted_xor_backward);
    float new_width = profile->GetLaneWidth(current_lane, inverted_xor_backward);

    if (UMath::Abs(new_offset - fLaneOffset) >= new_width * 0.5f) {
        ChangeLanes(new_offset, 0.0f);
        return true;
    }

    return false;
}

void WRoadNav::ChangeDragLanes(int left_right) {
    char node_ind = GetNodeInd();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, node_ind);

    bool backward = node_ind == 0;
    bool inverted = segment->IsProfileInverted(node_ind);
    float current_offset = fLaneOffset;

    if (left_right == 0) {
        ISimable *simable = pAIVehicle ? pAIVehicle->GetSimable() : nullptr;
        IRigidBody *rigid_body = simable ? simable->GetRigidBody() : nullptr;
        if (rigid_body) {
            WRoadNav temp_nav;
            temp_nav.SetRaceFilter(true);
            const UMath::Vector3 &car_position = rigid_body->GetPosition();
            temp_nav.InitAtPoint(car_position, GetForwardVector(), false, 1.0f);
            if (temp_nav.fValid) {
                current_offset = temp_nav.GetLaneOffset();
            }
        }
    }

    bool inverted_xor_backward = inverted ^ backward;
    int current_lane = profile->GetMiddleZone(inverted_xor_backward);

    while (!IsSelectable(profile->GetLaneType(current_lane, inverted_xor_backward))) {
        current_lane++;
    }

    float offset_difference = bAbs(profile->GetRelativeLaneOffset(current_lane, inverted_xor_backward) - current_offset);
    int num_lanes = profile->fNumZones;

    for (int lane = 0; lane < num_lanes; lane++) {
        if (!IsSelectable(profile->GetLaneType(lane, inverted_xor_backward))) {
            continue;
        }
        float offset = profile->GetRelativeLaneOffset(lane, inverted_xor_backward);
        float difference = offset - current_offset;
        if (difference == bClamp(difference, -offset_difference, offset_difference)) {
            offset_difference = bAbs(difference);
            current_lane = lane;
        }
    }

    if (left_right != 0) {
        int last_lane = current_lane;
        int next_lane = current_lane + left_right;

        while (next_lane < num_lanes && next_lane >= 0) {
            int next_lane_type = profile->GetLaneType(next_lane, inverted_xor_backward);
            if (IsDrivable(next_lane_type)) {
                if (IsSelectable(next_lane_type)) {
                    current_lane = next_lane;
                    break;
                }
                next_lane += left_right;
            } else {
                break;
            }
        }

        if (current_lane == last_lane && ChangeDragDecision(left_right)) {
            return;
        }
    }

    ChangeLanes(profile->GetRelativeLaneOffset(current_lane, inverted_xor_backward), 0.0f);
}

bool WRoadNav::UpdateLaneChange(float distance) {
    if (fLaneChangeDist <= 0.0f) {
        return false;
    }
    float laneChangeLerp = (fLaneChangeInc + distance) / fLaneChangeDist;
    fLaneChangeInc += distance;
    if (laneChangeLerp < 1.0f) {
        fLaneOffset = fFromLaneOffset + (fToLaneOffset - fFromLaneOffset) * laneChangeLerp;
    } else {
        SetLaneOffset(fToLaneOffset);
        fLaneChangeDist = 0.0f;
        fLaneChangeInc = 0.0f;
    }
    return true;
}

unsigned int WRoadNav::GetRoadSpeechId() {
    unsigned int ret = 0;
    unsigned short segment_index = GetSegmentInd();
    WRoadNetwork &road_network = WRoadNetwork::Get();
    unsigned short num_segments = road_network.GetNumSegments();
    if (segment_index == bClamp(segment_index, 0, num_segments - 1)) {
        const WRoadSegment *segment = road_network.GetSegment(segment_index);
        short road_index = segment->fRoadID;
        short num_roads = road_network.GetNumRoads();
        if (road_index == bClamp(road_index, 0, num_roads - 1)) {
            const WRoad *road = road_network.GetRoad(road_index);
            ret = road->nSpeechId;
        }
    }
    return ret;
}

unsigned char WRoadNav::GetShortcutNumber() {
    if (IsValid()) {
        WRoadNetwork &rn = WRoadNetwork::Get();
        int segment_number = GetSegmentInd();
        const WRoadSegment *segment = rn.GetSegment(segment_number);
        if (segment->IsDecision()) {
            const WRoadNode *nodes[2];
            int which_node = GetNodeInd();

            rn.GetSegmentNodes(*segment, nodes);
            segment = GetAttachedDirectionalSegment(nodes[which_node], segment_number);
            if (!segment || !segment->IsShortcut()) {
                segment = GetAttachedDirectionalSegment(nodes[which_node ^ 1], segment_number);
            }
        }
        if (segment && segment->IsShortcut()) {
            int road_number = segment->fRoadID;
            if (road_number != -1) {
                const WRoad *road = rn.GetRoad(road_number);
                return road->nShortcut;
            }
        }
    }
    return 0xFF;
}

bool WRoadNav::IsOnLegalRoad() {
    if (IsValid()) {
        WRoadNetwork &rn = WRoadNetwork::Get();
        int segment_number = GetSegmentInd();
        const WRoadSegment *segment = rn.GetSegment(segment_number);
        if (segment->IsDecision()) {
            const WRoadNode *node = rn.GetNode(segment->fNodeIndex[GetNodeInd()]);
            segment = GetAttachedDirectionalSegment(node, segment_number);
        }
        return segment && segment->IsTrafficAllowed();
    }
    return false;
}

unsigned char WRoadNetwork::GetSegmentShortcutNumber(const WRoadSegment *segment) {
    if (segment->IsShortcut()) {
        int road_number = segment->fRoadID;
        if (road_number != -1) {
            return GetRoad(road_number)->nShortcut;
        }
    }
    return 0xFF;
}

bool WRoadNav::MakeShortcutDecision(int shortcut_number, unsigned int *cached, unsigned int *allowed) {
    if (shortcut_number != 0xff) {
        if (GetPathType() == kPathPlayer) {
            return shortcut_number == GetShortcutNumber();
        } else if (GetPathType() == kPathRacer) {
            int mask = 1 << shortcut_number;
            if (cached && (mask & *cached) != 0) {
                return (mask & *allowed) != 0;
            }
            GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();
            if (race_parameters) {
                AIVehicle *vehicle = GetVehicle();
                float skill = vehicle ? vehicle->GetShortcutSkill() : 0.0f;
                GMarker *marker = race_parameters->GetShortcut(shortcut_number);
                float min = marker->ShortcutMinChance();
                float max = marker->ShortcutMaxChance();
                if (min > 1.0f) {
                    min *= 0.01f;
                }
                if (max > 1.0f) {
                    max *= 0.01f;
                }
                float chance = min + skill * (max - min);
                bool shortcut_allowed = bRandom(1.0f) < chance;
                if (shortcut_allowed && allowed) {
                    *allowed |= mask;
                }
                if (cached != nullptr) {
                    *cached |= mask;
                }
                return shortcut_allowed;
            }
        }
    }
    return true;
}

void WRoadNav::CancelPathFinding() {
    PathFinder *path_finder = PathFinder::Get();
    if (path_finder) {
        path_finder->Cancel(this);
    }
    if (GetNavType() == kTypePath) {
        SetNavType(kTypeDirection);
    }
}

bool WRoadNav::FindPath(const UMath::Vector3 *goal_position, const UMath::Vector3 *goal_direction, char *shortcut_allowed) {
    PathFinder *path_finder = PathFinder::Get();
    if (path_finder) {
        MaybeAllocatePathSegments();
        AStarSearch *search = path_finder->Pending(this);
        if (search == nullptr) {
            search = path_finder->Submit(this, goal_position, goal_direction, shortcut_allowed);
        }
        return search;
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

bool WRoadNav::FindingPath() {
    PathFinder *path_finder = PathFinder::Get();
    return path_finder && path_finder->Pending(this);
}

float WRoadNav::GetPathDistanceRemaining() {
    float distance = 0.0f;
    WRoadNetwork &rn = WRoadNetwork::Get();
    if (GetNavType() == kTypePath && pPathSegments) {
        bool accumulate = false;
        for (int i = 0; i < nPathSegments; i++) {
            float min_param = 0.0f;
            float max_param = 1.0f;
            unsigned short segment_number = pPathSegments[i];
            if (segment_number == GetSegmentInd()) {
                min_param = GetSegmentTime();
                accumulate = true;
            }
            bool break_out = false;
            if (segment_number == nPathGoalSegment) {
                max_param = fPathGoalParam;
                break_out = true;
            }
            if (accumulate) {
                float coef = bMax(0.0f, max_param - min_param);
                const WRoadSegment *segment = rn.GetSegment(segment_number);
                float segment_length = coef * segment->GetLength();
                if (segment->IsShortcut()) {
                    const WRoad *road = rn.GetRoad(segment->fRoadID);
                    segment_length *= road->GetScale();
                }
                distance += segment_length;
            }
            if (break_out) {
                break;
            }
        }
    }
    return distance;
}

// TODO player_or_racer
bool WRoadNav::CanTrafficSpawn() {
    if (!IsValid()) {
        return false;
    }

    WRoadNetwork &road_network = WRoadNetwork::Get();
    const WRoadSegment *segment = road_network.GetSegment(GetSegmentInd());
    int which_node = GetNodeInd();

    if (segment->IsDecision()) {
        return false;
    }
    if (!segment->IsTrafficAllowed()) {
        return false;
    }
    if (segment->IsOneWay() && which_node == 0) {
        return false;
    }

    // const bool player_or_racer;
    bool forward = (which_node == 1);
    bool inverted = segment->IsProfileInverted(which_node);

    const WRoadProfile *profile = road_network.GetSegmentProfile(*segment, which_node);

    int num_traffic_lanes = profile->GetNumTrafficLanes(forward, inverted);
    if (!num_traffic_lanes) {
        return false;
    }

    int random_lane = profile->GetNthTrafficLane(bRandom(num_traffic_lanes), forward, inverted);
    ChangeLanes(profile->GetLaneOffset(random_lane, false), 0.0f);

    SetLaneInd(static_cast<char>(random_lane));

    return IsValid();
}

// UNSOLVED
float WRoadNav::CookieTrailCurvature(const UMath::Vector3 &car_position, const UMath::Vector3 &car_velocity) {
    if (pCookieTrail != nullptr) {
        float road_curvature = 0.0f;
        float apex = 0.0f;

        if (pCookieTrail->Count() > 2) {
            if (IsOccluded() && !IsOccludedFromBehind()) {
                UMath::Vector3 current_to_apex;
                const UMath::Vector3 &nav_position = fPosition;
                const UMath::Vector3 &apex_position = fApexPosition;
                const UMath::Vector3 &current_position = fPosition;
                const UMath::Vector3 &occluded_position = fOccludedPosition;

                UMath::Sub(occluded_position, car_position, current_to_apex);
                current_to_apex.y = 0.0f;
                float dist_to_apex = UMath::Normalize(current_to_apex);

                int apex_cookie_index = ClosestCookieAhead(apex_position, nullptr);
                if (dist_to_apex > 1.0f && apex_cookie_index >= 0) {
                    const NavCookie &apex_cookie = pCookieTrail->NthOldest(apex_cookie_index);
                    float apex_width = UMath::Max(dist_to_apex, bAbs(apex_cookie.RightOffset - apex_cookie.LeftOffset));

                    UMath::Vector3 apex_to_nav;
                    UMath::Sub(nav_position, apex_position, apex_to_nav);
                    apex_to_nav.y = 0.0f;
                    UMath::Normalize(apex_to_nav);

                    float sina =
                        UMath::Abs(UMath::ASinr(UMath::Clamp(apex_to_nav.x * current_to_apex.z - current_to_apex.x * apex_to_nav.z, -1.0f, 1.0f)));

                    if (current_to_apex.x * apex_to_nav.x + current_to_apex.z * apex_to_nav.z < 0.0f) {
                        sina = static_cast<float>(M_PI) - sina;
                    }

                    float div = UMath::Max(1.0f, apex_width);
                    apex = sina * UMath::Sinr(UMath::Min(sina, static_cast<float>(M_PI_2)));

                    if (nAvoidableOcclusion != 0) {
                        float my_trailingspeed = UMath::Dot(car_velocity, current_to_apex);
                        float closing_speed = (my_trailingspeed - fOccludingTrailSpeed);
                        float ratio = 0.0f;
                        if (my_trailingspeed > 1e-6f) {
                            ratio = UMath::Ramp(closing_speed / my_trailingspeed, 0.0f, 1.0f);
                        }
                        apex *= ratio * ratio;
                    }

                    apex = UMath::Clamp(apex, 0.0f, static_cast<float>(M_PI)) / div;
                }
            }
        }
        {
            float distance = 0.0f;
            float total_curvature = 0.0f;
            float previous_curvature = 0.0f;
            int num_cookies = pCookieTrail->Count();

            for (int i = nCookieIndex; i < num_cookies; i++) {
                const NavCookie &cookie = pCookieTrail->NthOldest(i);
                float current_curvature = UMath::Clamp(cookie.Curvature, -0.01f, 0.01f);
                if (i > nCookieIndex) {
                    float length = cookie.Length;
                    float avg_curvature = (current_curvature + previous_curvature) * 0.5f;
                    total_curvature += length * avg_curvature;
                    distance += length;
                }
                previous_curvature = current_curvature;
            }

            if (distance > 0.0f) {
                road_curvature = bAbs(total_curvature / distance);
            }
        }

        return UMath::Max(apex, road_curvature);
    }
    return 0.0f;
}

bool WRoadNav::IsPointInCookieTrail(const UMath::Vector3 &position_3d, float margin) {
    if (pCookieTrail != nullptr) {
        bVector2 position(position_3d.x, position_3d.z);
        if (bBoundingBoxIsInside(&vCookieTrailBoxMin, &vCookieTrailBoxMax, &position, margin)) {
            int closest_cookie = ClosestCookieAhead(position_3d, nullptr);
            if (closest_cookie >= nCookieIndex) {
                const NavCookie &cookie = pCookieTrail->NthOldest(closest_cookie);
                if (position_3d.y == bClamp(position_3d.y, cookie.Centre.y - 5.0f, cookie.Centre.y + 5.0f)) {
                    float min_offset = cookie.LeftOffset - margin;
                    float max_offset = cookie.RightOffset + margin;
                    bVector2 cookie_to_position = position - bVector2(cookie.Centre.x, cookie.Centre.z);
                    float offset = bCross(&cookie_to_position, reinterpret_cast<const bVector2 *>(&cookie.Forward));
                    return offset == bClamp(offset, min_offset, max_offset);
                }
            }
        }
    }
    return false;
}

bool WRoadNav::IsSegmentInCookieTrail(int segment_number, bool use_whole_path) {
    if (pCookieTrail != nullptr) {
        int num_cookies = pCookieTrail->Count();
        for (int i = use_whole_path ? 0 : nCookieIndex; i < num_cookies; i++) {
            const NavCookie &cookie = pCookieTrail->NthOldest(i);
            if (segment_number == cookie.SegmentNumber) {
                return true;
            }
        }
    }
    return false;
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

const WRoadSegment *GetAttachedDirectionalSegment(const WRoadNode *node, short segInd) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *newRoadSegment = roadNetwork.GetSegment(node->fSegmentIndex[0]);
    for (int i = 0; i < node->fNumSegments; i++) {
        newRoadSegment = roadNetwork.GetSegment(node->fSegmentIndex[i]);
        if (segInd != newRoadSegment->fIndex && (newRoadSegment->fFlags ^ 1) & 1) {
            return newRoadSegment;
        }
    }
    return nullptr;
}
