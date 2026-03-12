#include "Speed/Indep/Src/World/WRoadNetwork.h"

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
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

extern BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width);
extern void bInitializeBoundingBox(bVector2 *min, bVector2 *max);
extern void bExpandBoundingBox(bVector2 *min, bVector2 *max, const bVector2 *point);
extern bool bAiRandomTurns;

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

Sim::SubSystem _Physics_System_WRoadNetwork("WRoadNetwork", WRoadNetwork::Init, WRoadNetwork::Shutdown);

void WRoadNetwork::Init() {
    if (fgRoadNetwork == nullptr) {
        fgRoadNetwork = new WRoadNetwork();
        fValid = false;
        fValidRaceFilter = false;
        fValidTrafficRoads = true;
        fNumNodes = 0;
        fNumSegments = 0;
        fNumIntersections = 0;
        fNumRoads = 0;
        nRoadMemoryUsage = 0;
        nNodeMemoryUsage = 0;
        nProfileMemoryUsage = 0;
        nSegmentMemoryUsage = 0;
        nIntersectionMemoryUsage = 0;
        nTotalMemoryUsage = 0;

        if (WWorld::Get().GetMapGroup()) {
            const UGroup *networkGroup = WWorld::Get().GetMapGroup()->GroupLocate('RN', 'gp');
            if (networkGroup != WWorld::Get().GetMapGroup()->GroupEnd()) {
                const UData *headerData = networkGroup->DataLocate('RN', 'hd');
                const WRoadNetworkInfo *roadInfo =
                    static_cast< const WRoadNetworkInfo * >(headerData->GetDataConst());

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
                nTotalMemoryUsage = nRoadMemoryUsage + nNodeMemoryUsage +
                                    nProfileMemoryUsage + nSegmentMemoryUsage +
                                    nIntersectionMemoryUsage;

                if (fNumNodes == 0) {
                    fValid = false;
                    return;
                }

                const UData *data;
                void *nonConstData;

                data = networkGroup->DataLocate('RN', 'pf');
                nonConstData = const_cast< void * >(data->GetDataConst());
                if (data != networkGroup->DataEnd()) {
                    fProfiles = static_cast< WRoadProfile * >(nonConstData);
                }

                data = networkGroup->DataLocate('RN', 'nd');
                nonConstData = const_cast< void * >(data->GetDataConst());
                if (data != networkGroup->DataEnd()) {
                    fNodes = static_cast< WRoadNode * >(nonConstData);
                }

                data = networkGroup->DataLocate('RN', 'sg');
                nonConstData = const_cast< void * >(data->GetDataConst());
                if (data != networkGroup->DataEnd()) {
                    fSegments = static_cast< WRoadSegment * >(nonConstData);
                }

                data = networkGroup->DataLocate('RN', 'rd');
                nonConstData = const_cast< void * >(data->GetDataConst());
                if (data != networkGroup->DataEnd()) {
                    fRoads = static_cast< WRoad * >(nonConstData);
                }

                fValid = true;
            }
        }
        fgRoadNetwork->ResolveBarriers();
        fgRoadNetwork->ResolveShortcuts();
        fgRoadNetwork->ResetRaceSegments();
    }
}

void WRoadNetwork::Shutdown() {
    if (fgRoadNetwork) {
        gFastMem.Free(fgRoadNetwork, sizeof(WRoadNetwork), nullptr);
        fgRoadNetwork = nullptr;
    }
}

bool WRoadNetwork::SegmentCrossesBarrier(WRoadSegment *segment, TrackPathBarrier *barrier) {
    USpline spline;
    bVector2 points[2];
    BuildSegmentSpline(*segment, spline);
    int num_pieces = bMax(static_cast< int >(bCeil(segment->GetLength() * 0.1f)), 4);
    float inc = 1.0f / static_cast< float >(num_pieces);
    for (int i = 0; i <= num_pieces; i++) {
        int which_point = i & 1;
        UMath::Vector4 v4;
        spline.EvaluateSpline(static_cast< float >(i) * inc, v4);
        bVector2 temp(v4.z, -v4.x);
        points[which_point] = temp;
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

        if (race_parameters != nullptr) {
            num_exemptions = race_parameters->GetNumBarrierExemptions();
            if (num_exemptions > 0) {
                for (int i = 0; i < num_exemptions; i++) {
                    GMarker *exemption = race_parameters->GetBarrierExemption(i);
                    nav.InitAtPoint(exemption->GetPosition(), exemption->GetDirection(),
                                    force_centre_lane, dir_weight);
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
            typedef UTL::Std::set<unsigned short, _type_set> SEGMENT_SET;
            UMath::Vector4 barrier_points[2];
            barrier_points[0] = UMath::Vector4Make(-barrier->Points[0].y, 0.0f,
                                                   barrier->Points[0].x, 1.0f);
            barrier_points[1] = UMath::Vector4Make(-barrier->Points[1].y, 0.0f,
                                                   barrier->Points[1].x, 1.0f);

            UTL::FastVector<unsigned int, 16> node_list;
            grid.FindNodes(barrier_points, node_list);

            SEGMENT_SET segment_set;

            for (unsigned int *iter = node_list.begin(); iter != node_list.end(); ++iter) {
                WGridNode *grid_node = grid.fNodes[*iter];
                if (grid_node != nullptr) {
                    unsigned int numSegments = grid_node->GetElemTypeCount(WGrid_kRoadSegment);
                    for (unsigned int i = 0; i < numSegments; i++) {
                        segment_set.insert(
                            static_cast<short>(grid_node->GetElemType(i, WGrid_kRoadSegment)));
                    }
                }
            }

            for (SEGMENT_SET::const_iterator it = segment_set.begin();
                 it != segment_set.end(); ++it) {
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
        unsigned char lane_type = profile->GetLaneType(lane, inverted);
        if (IsSelectable(lane_type)) {
            float offset = profile->GetRawLaneOffset(lane);
            float difference = bClamp(offset - input_offset, -offset_difference, offset_difference);
            if (offset - input_offset == difference) {
                offset_difference = bAbs(offset - input_offset);
                found_lane = true;
                best_lane = lane;
                next_offset = offset;
            }
        }
    }

    int num_backward_lanes = profile->GetNumLanes(!forward, inverted);

    if ((cop_lane || drag_lane || grid_lane || racing_lane || !found_lane) && num_backward_lanes > 0) {
        for (int n = 0; n < num_backward_lanes; n++) {
            int lane = profile->GetNthLane(n, !forward, inverted);
            unsigned char lane_type = profile->GetLaneType(lane, inverted);
            if (IsSelectable(lane_type)) {
                float offset = -profile->GetRawLaneOffset(lane);
                float difference = bClamp(offset - input_offset, -offset_difference, offset_difference);
                if (offset - input_offset == difference) {
                    best_backward = true;
                    offset_difference = bAbs(offset - input_offset);
                    next_offset = offset;
                    best_lane = lane;
                }
            }
        }
    }

    float output_offset = next_offset;

    if ((cop_lane || racing_lane) && best_lane > -1) {
        float offset = profile->GetRawLaneOffset(best_lane);
        float width = profile->GetRawLaneWidth(best_lane);
        float difference = bClamp(input_offset - next_offset, -width * 0.5f, width * 0.5f);

        bool inverted_xor_backward = (profile->GetLaneNumber(best_lane, inverted) < profile->GetMiddleZone(inverted)) != best_backward;

        int left_lane;
        int right_lane;
        if (inverted_xor_backward) {
            left_lane = profile->fNumZones - 1;
            right_lane = 0;
        } else {
            left_lane = 0;
            right_lane = profile->fNumZones - 1;
        }

        float right = profile->GetRelativeLaneOffset(right_lane, inverted);
        float right_width = profile->GetLaneWidth(right_lane, inverted);

        float left = profile->GetRelativeLaneOffset(left_lane, inverted);
        float left_width = profile->GetLaneWidth(left_lane, inverted);

        output_offset = next_offset + difference;
        float safety_margin = right + right_width - 0.5f;
        float left_limit = left - left_width + 0.5f;
        output_offset = bClamp(output_offset, left_limit, safety_margin);
    }

    return output_offset;
}

int WRoadNav::ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *interpolated_cookie) {
    return ClosestCookieAhead(position, nullptr, pCookieTrail->Count(), interpolated_cookie);
}

int WRoadNav::ClosestCookieAhead(const UMath::Vector3 &position, NavCookie *cookies, int num_cookies,
                                  NavCookie *interpolated_cookie) {
    int ret = -1;
    float closest = -1.0f;
    if (num_cookies > 0) {
        float previous_dot = 0.0f;
        bVector2 car_position(position.x, position.z);
        for (int n = 0; n < num_cookies; n++) {
            const NavCookie &cookie = cookies ? cookies[n] : pCookieTrail->NthOldest(n);
            bVector2 cookie_to_car = car_position - bVector2(cookie.Centre.x, cookie.Centre.z);
            float distance_squared = bDot(&cookie_to_car, &cookie_to_car);
            float current_dot = bDot(reinterpret_cast<const bVector2 &>(cookie.Forward), cookie_to_car);
            if (n == 0) {
                if (current_dot < 0.0f) {
                    ret = 0;
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

void WRoadNav::SetStartEndControls(const WRoadSegment &segment) {
    SetControlPos(segment, true);
    SetControlPos(segment, false);
}

bool WRoadNav::FindingPath() {
    PathFinder *path_finder = PathFinder::Get();
    return path_finder != nullptr && path_finder->Pending(this) != nullptr;
}

float WRoadNav::GetPathDistanceRemaining() {
    WRoadNetwork &rn = WRoadNetwork::Get();
    float distance = 0.0f;
    if (GetNavType() == kTypePath && pPathSegments != nullptr) {
        bool accumulate = false;
        for (int i = 0; i < nPathSegments; i++) {
            unsigned short segment_number = pPathSegments[i];
            float min_param = 0.0f;
            float max_param = 1.0f;
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

    int player_or_racer = (which_node == 1);
    bool inverted = segment->IsProfileInverted(which_node);

    const WRoadProfile *profile = road_network.GetSegmentProfile(*segment, which_node);

    int num_traffic_lanes = profile->GetNumTrafficLanes(player_or_racer, inverted);
    if (!num_traffic_lanes) {
        return false;
    }

    int random_lane = bRandom(num_traffic_lanes);
    int lane = profile->GetNthTrafficLane(random_lane, player_or_racer, inverted);
    float offset = profile->GetLaneOffset(lane, false);
    ChangeLanes(offset, 0.0f);

    SetLaneInd(static_cast< char >(lane));

    return IsValid();
}

float WRoadNav::CookieTrailCurvature(const UMath::Vector3 &car_position, const UMath::Vector3 &car_velocity) {
    if (pCookieTrail == nullptr) {
        return 0.0f;
    }

    float road_curvature = 0.0f;
    float apex = 0.0f;

    if (pCookieTrail->Count() > 2) {
        if (IsOccluded() && !IsOccludedFromBehind()) {
            UMath::Vector3 current_to_apex;
            const UMath::Vector3 &nav_position = fPosition;
            const UMath::Vector3 &apex_position = fApexPosition;
            const UMath::Vector3 &occluded_position = fOccludedPosition;

            UMath::Sub(occluded_position, car_position, current_to_apex);
            current_to_apex.y = 0.0f;
            float dist_to_apex = UMath::Normalize(current_to_apex);

            if (dist_to_apex > 1.0f) {
                int apex_cookie_index = ClosestCookieAhead(fApexPosition, nullptr);

                if (apex_cookie_index >= 0) {
                    const NavCookie &apex_cookie = pCookieTrail->NthOldest(apex_cookie_index);
                    float apex_width = bAbs(apex_cookie.LeftOffset - apex_cookie.RightOffset);
                    apex_width = UMath::Max(apex_width, dist_to_apex);

                    UMath::Vector3 apex_to_nav;
                    UMath::Sub(nav_position, apex_position, apex_to_nav);
                    apex_to_nav.y = 0.0f;
                    UMath::Normalize(apex_to_nav);

                    float sina = UMath::Clamp(current_to_apex.x * apex_to_nav.z - current_to_apex.z * apex_to_nav.x, -1.0f, 1.0f);
                    float angle = UMath::Abs(UMath::ASinr(sina));

                    if (current_to_apex.x * apex_to_nav.x + current_to_apex.z * apex_to_nav.z < 0.0f) {
                        angle = static_cast< float >(M_PI) - angle;
                    }

                    float div = UMath::Max(1.0f, apex_width);
                    apex = angle * UMath::Sinr(UMath::Min(angle, static_cast< float >(M_PI_2)));

                    if (nAvoidableOcclusion != 0) {
                        float my_trailingspeed = UMath::Dot(car_velocity, current_to_apex);
                        float ratio = 0.0f;
                        if (my_trailingspeed > 0.5f) {
                            float closing_speed = (my_trailingspeed - fOccludingTrailSpeed) / my_trailingspeed;
                            ratio = UMath::Ramp(closing_speed, 0.0f, 1.0f);
                        }
                        apex *= ratio * ratio;
                    }

                    apex = UMath::Clamp(apex, 0.0f, static_cast< float >(M_PI));
                    apex = apex / div;
                }
            }
        }
    }

    float distance = 0.0f;
    float total_curvature = 0.0f;
    float previous_curvature = 0.0f;
    int num_cookies = pCookieTrail->Count();

    for (int i = nCookieIndex; i < num_cookies; i++) {
        const NavCookie &cookie = pCookieTrail->NthOldest(i);
        float current_curvature = UMath::Clamp(cookie.Curvature, -0.01f, 0.01f);
        if (nCookieIndex < i) {
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

    return UMath::Max(apex, road_curvature);
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

void WRoadNetwork::GetSegmentCurveStep(const UMath::Vector3 &start, const UMath::Vector3 &end, const WRoadSegment &segment, float u, UMath::Vector3 &point) {
    WRoadNetwork &roadNetwork = Get();
    static USpline roadSpline;
    UMath::Vector4 tempPos;
    UMath::Vector3 end_control;
    UMath::Vector3 start_control;
    segment.GetEndControl(end_control);
    segment.GetStartControl(start_control);
    roadSpline.BuildSplineEx(start, UVector3(start) + UVector3(start_control), end, UVector3(end) + UVector3(end_control));
    roadSpline.EvaluateSpline(u, tempPos);
    point = UMath::Vector4To3(tempPos);
}

void WRoadNetwork::BuildSegmentSpline(const WRoadSegment &segment, USpline &spline) {
    const WRoadNode *nodePtr[2];
    UMath::Vector3 end_control;
    segment.GetEndControl(end_control);
    UMath::Vector3 start_control;
    segment.GetStartControl(start_control);
    GetSegmentNodes(segment, nodePtr);
    const UMath::Vector3 &start = nodePtr[0]->fPosition;
    const UMath::Vector3 &end = nodePtr[1]->fPosition;
    spline.BuildSplineEx(start, UVector3(start) + UVector3(start_control), end, UVector3(end) + UVector3(end_control));
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
    int numTrafficLanes[2] = {0};
    roadNetwork.GetSegmentProfiles(segment, profilePtr);
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
    return UMath::Max(numTrafficLanes[1], numTrafficLanes[0]);
}

int WRoadNetwork::GetSegmentTrafficLaneInd(const WRoadSegment &segment, int lane_count) {
    const WRoadProfile *profile[2];
    Get().GetSegmentProfiles(segment, profile);
    for (int i = 0; i < profile[0]->fNumZones; i++) {
        if (profile[0]->GetLaneType(i, false) == 1) {
            if (lane_count <= 0) {
                return i;
            }
            lane_count--;
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
        if (i < num_segments - 1) {
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
        road->nShortcut = 0xFF;
    }
}

void WRoadNetwork::ResolveShortcuts() {
    ResetShortcuts();
    if (GRaceStatus::Exists()) {
        GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();
        if (race_parameters != nullptr) {
            WRoadNav nav;
            nav.SetDecisionFilter(true);
            nav.SetNavType(WRoadNav::kTypeDirection);
            int num_shortcuts = race_parameters->GetNumShortcuts();
            for (int i = 0; i < num_shortcuts; i++) {
                GMarker *shortcut = race_parameters->GetShortcut(i);
                if (shortcut != nullptr) {
                    nav.InitAtPoint(shortcut->GetPosition(), shortcut->GetDirection(), true, 0.7f);
                    if (nav.IsValid()) {
                        WRoad *road = Get().GetRoadNonConst(nav.GetRoadInd());
                        road->nShortcut = static_cast<char>(i);
                    }
                }
            }
            for (unsigned int segment_number = 0; segment_number < fNumSegments; segment_number++) {
                WRoadSegment *segment = GetSegmentNonConst(segment_number);
                if (segment->fRoadID != -1) {
                    if (Get().GetRoad(segment->fRoadID)->nShortcut != 0xFF) {
                        segment->SetShortcut(true);
                    }
                }
            }
        }
    }
}

bool WRoadNetwork::GetSegmentTrafficLaneRightSide(const WRoadSegment &segment, int laneInd) {
    WRoadNetwork &roadNetwork = Get();
    const WRoadProfile *profilePtr[2];
    roadNetwork.GetSegmentProfiles(segment, profilePtr);
    return laneInd >= profilePtr[0]->fMiddleZone;
}

int WRoadNetwork::GetRightMostTrafficEntrance(int node_number, int onto_segment) {
    int ret = -1;
    const WRoadNode *node = GetNode(node_number);
    int num_segments = node->fNumSegments;

    if (num_segments < 3) {
        return ret;
    }

    const WRoadSegment *segment = GetSegment(onto_segment);
    int which_node = node_number != segment->fNodeIndex[0];
    bool inverted = segment->IsProfileInverted(which_node);
    bool forward = static_cast< bool >(which_node) == inverted;
    const WRoadProfile *profile = GetProfile(node->fProfileIndex);

    if (profile->GetNumTrafficLanes(forward, inverted) > 0) {
        UMath::Vector2 onto_forward;
        float best_cross;
        bool first = true;
        UMath::Vector2 best_vector;

        segment->GetForwardVec(which_node, onto_forward);

        if (which_node) {
            UMath::Scale(onto_forward, -1.0f, onto_forward);
        }

        for (int i = 0; i < num_segments; i++) {
            int segment_number = node->fSegmentIndex[i];
            if (segment_number != onto_segment) {
                const WRoadSegment *from_segment = GetSegment(segment_number);
                const WRoadNode *from_node = GetSegmentOppNode(segment_number, node);
                int from_which_node = node_number != from_segment->fNodeIndex[0];
                bool from_inverted = from_segment->IsProfileInverted(from_which_node);
                bool from_forward = from_inverted != static_cast< bool >(from_which_node);
                const WRoadProfile *from_profile = GetProfile(from_node->fProfileIndex);

                if (from_profile->GetNumTrafficLanes(from_forward, from_inverted) != 0) {
                    UMath::Vector3 vector_3d;
                    UMath::Sub(from_node->fPosition, node->fPosition, vector_3d);
                    UMath::Vector2 vector = UMath::Vector2Make(vector_3d.x, vector_3d.z);
                    float cross = UMath::Cross(vector, onto_forward);

                    if (first) {
                        first = false;
                        best_cross = cross;
                        best_vector = vector;
                        ret = segment_number;
                    } else {
                        float old_cross = UMath::Cross(best_vector, vector);
                        bool right_of_onto = cross >= 0.0f;
                        bool right_of_best = old_cross >= 0.0f;
                        bool is_best;
                        if (best_cross < 0.0f) {
                            is_best = right_of_onto || right_of_best;
                        } else {
                            is_best = right_of_onto && right_of_best;
                        }
                        if (is_best) {
                            best_cross = cross;
                            best_vector = vector;
                            ret = segment_number;
                        }
                    }
                }
            }
        }
    }

    return ret;
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

int WRoadProfile::GetNthTrafficLaneFromCurb(int n, bool forward) const {
    int num_traffic_lanes = 0;
    int num_lanes = GetNumLanes(forward);
    int fallback = GetMiddleZone(!forward);
    for (int i = num_lanes - 1; i >= 0; i--) {
        int real_lane = GetNthLane(i, !forward);
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
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    for (int i = 0; i < node->fNumSegments; i++) {
        const WRoadSegment *newRoadSegment = roadNetwork.GetSegment(node->fSegmentIndex[i]);
        if (segment_index != newRoadSegment->fIndex && (newRoadSegment->fFlags ^ 1) & 1) {
            return newRoadSegment;
        }
    }
    return nullptr;
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
    if (pCookieTrail) {
        delete pCookieTrail;
    }
    if (pPathSegments) {
        delete pPathSegments;
    }
}

bool WRoadNav::IsWrongWay() const {
    if (!GetRaceFilter()) {
        return false;
    }
    if (!IsValid()) {
        return false;
    }
    bool result = false;
    bool is_node_one = (fNodeInd == 1);
    const WRoadSegment *segment = GetSegment();
    if (segment->IsInRace()) {
        bool seg_foward = true;
        if (!segment->RaceRouteForward()) {
            seg_foward = false;
        }
        if (is_node_one) {
            if (!seg_foward) {
                result = true;
            }
        } else if (seg_foward) {
            result = true;
        }
    }
    return result;
}

unsigned int WRoadNav::GetRoadSpeechId() {
    unsigned short segment_index = GetSegmentInd();
    WRoadNetwork &road_network = WRoadNetwork::Get();
    unsigned short num_segments = road_network.GetNumSegments();
    if (segment_index != bClamp(segment_index, 0, num_segments - 1)) {
        return 0;
    }
    const WRoadSegment *segment = road_network.GetSegment(segment_index);
    short road_index = segment->fRoadID;
    short num_roads = road_network.GetNumRoads();
    if (road_index != bClamp(road_index, 0, num_roads - 1)) {
        return 0;
    }
    const WRoad *road = road_network.GetRoad(road_index);
    return road->nSpeechId;
}

unsigned char WRoadNav::GetShortcutNumber() {
    if (!IsValid()) {
        return 0xFF;
    }
    WRoadNetwork &rn = WRoadNetwork::Get();
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = rn.GetSegment(segment_number);
    if (segment->IsDecision()) {
        const WRoadNode *nodes[2];
        int which_node = GetNodeInd();
        rn.GetSegmentNodes(*segment, nodes);
        segment = GetAttachedDirectionalSegment(nodes[which_node], segment_number);
        if (segment == nullptr || !segment->IsShortcut()) {
            segment = GetAttachedDirectionalSegment(nodes[which_node ^ 1], segment_number);
        }
    }
    if (segment != nullptr && segment->IsShortcut()) {
        int road_number = segment->fRoadID;
        if (road_number != -1) {
            const WRoad *road = rn.GetRoad(road_number);
            return road->nShortcut;
        }
    }
    return 0xFF;
}

bool WRoadNav::IsOnLegalRoad() {
    if (IsValid()) {
        int segment_number = GetSegmentInd();
        WRoadNetwork &rn = WRoadNetwork::Get();
        const WRoadSegment *segment = rn.GetSegment(segment_number);
        if (segment->IsDecision()) {
            const WRoadNode *node = rn.GetNode(segment->fNodeIndex[GetNodeInd()]);
            segment = GetAttachedDirectionalSegment(node, segment_number);
        }
        return segment != nullptr && segment->IsTrafficAllowed();
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
        return search != nullptr;
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
    SetBoundPos(segment, endOffset, false);
    SetBoundPos(segment, startOffset, true);
}

void WRoadNav::InitAtPoint(const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane, float dirWeight) {
    float time;
    int segment = FindClosestSegmentInd(pos, dir, dirWeight, fPosition, time);
    if (segment == -1) {
        fSegmentInd = 0;
        fValid = false;
    } else {
        InitAtSegment(static_cast<short>(segment), time, pos, dir, forceCenterLane);
    }
}

void WRoadNav::InitAtPath(const UMath::Vector3 &position, bool forceCenterLane) {
    UMath::Vector3 found_position;
    UMath::Vector3 found_direction;
    unsigned short found_segment;
    float found_interval = -1.0f;
    int result = FindClosestOnPath(position, &found_position, &found_direction, &found_segment, &found_interval);
    if (result == 0) {
        fSegmentInd = 0;
        fValid = false;
    } else {
        InitAtSegment(static_cast<short>(found_segment), found_interval, found_position, found_direction, forceCenterLane);
    }
}

bool WRoadNav::UpdateLaneChange(float distance) {
    if (fLaneChangeDist <= 0.0f) {
        return false;
    }
    float laneChangeLerp = (fLaneChangeInc + distance) / fLaneChangeDist;
    fLaneChangeInc = fLaneChangeInc + distance;
    if (laneChangeLerp < 1.0f) {
        fLaneOffset = (fToLaneOffset - fFromLaneOffset) * laneChangeLerp + fFromLaneOffset;
    } else {
        fLaneOffset = fToLaneOffset;
        fLaneChangeDist = 0.0f;
        fFromLaneOffset = fToLaneOffset;
        fLaneChangeInc = 0.0f;
    }
    return true;
}

void WRoadNav::RebuildSplines(const WRoadSegment *segment) {
    if (segment->IsCurved()) {
        fRoadSpline.BuildSplineEx(fStartPos, fStartControl, fEndPos, fEndControl);
        if (bCookieTrail) {
            fLeftSpline.BuildSplineEx(fLeftStartPos, fLeftStartControl, fLeftEndPos, fLeftEndControl);
            fRightSpline.BuildSplineEx(fRightStartPos, fRightStartControl, fRightEndPos, fRightEndControl);
        }
    }
}

void WRoadNav::InitFromOtherNav(WRoadNav *other_nav, bool flip_direction) {
    if (other_nav != this) {
        fSegmentInd = other_nav->GetSegmentInd();
        fNodeInd = other_nav->GetNodeInd();
        fSegTime = other_nav->GetSegmentTime();
        SetLaneInd(other_nav->GetLaneInd());
        SetLaneOffset(other_nav->GetLaneOffset());
        fValid = other_nav->fValid;
        if (flip_direction) {
            fNodeInd = fNodeInd ^ 1;
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
}

void WRoadNav::Reverse() {
    if (GetRaceFilter() && !IsWrongWay() && (fPathType == kPathRacer || fPathType == kPathPlayer)) {
        return;
    }
    fNodeInd = fNodeInd ^ 1;
    fSegTime = 1.0f - fSegTime;
    const WRoadSegment *segment = WRoadNetwork::Get().GetSegment(fSegmentInd);
    SetStartEndPos(*segment, fLaneOffset);
    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
    ResetCookieTrail();
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
        if (next_lane_type != kLaneTraffic) break;
        lane++;
    }

    float extra = fVehicleHalfWidth;
    if (lane == num_lanes - 1 || is_barrier) {
        extra = -extra;
    }

    float offset = profile->GetLaneOffset(lane, inverted) + profile->GetLaneWidth(lane, inverted) * 0.5f + extra;

    const UMath::Vector3 &nav_forward = GetForwardVector();
    UMath::Vector3 nav_right = UMath::Vector3Make(nav_forward.z, 0.0f, -nav_forward.x);
    UMath::Normalize(nav_right);

    float offset_change = offset - GetLaneOffset();
    UMath::ScaleAdd(nav_right, offset_change, GetPosition(), GetPosition());
}

bool WRoadNav::IsPointInCookieTrail(const UMath::Vector3 &position_3d, float margin) {
    if (pCookieTrail != nullptr) {
        bVector2 position(position_3d.x, position_3d.z);
        if (bBoundingBoxIsInside(&vCookieTrailBoxMin, &vCookieTrailBoxMax, &position, margin)) {
            int closest_cookie = ClosestCookieAhead(position_3d, nullptr);
            if (closest_cookie >= nCookieIndex) {
                const NavCookie &cookie = pCookieTrail->NthOldest(closest_cookie);
                float y = bClamp(position_3d.y, cookie.Centre.y - 5.0f, cookie.Centre.y + 5.0f);
                if (position_3d.y == y) {
                    float min_offset = cookie.LeftOffset - margin;
                    float max_offset = cookie.RightOffset + margin;
                    bVector2 centre_2d(cookie.Centre.x, cookie.Centre.z);
                    bVector2 cookie_to_position = position - centre_2d;
                    float offset = bCross(&cookie_to_position, reinterpret_cast<const bVector2 *>(&cookie.Forward));
                    float clamped_offset = bClamp(offset, min_offset, max_offset);
                    return offset == clamped_offset;
                }
            }
        }
    }
    return false;
}

bool WRoadNav::IsSegmentInCookieTrail(int segment_number, bool use_whole_path) {
    if (pCookieTrail != nullptr) {
        int num_cookies = pCookieTrail->Count();
        int i = 0;
        if (!use_whole_path) {
            i = nCookieIndex;
        }
        for (; i < num_cookies; i++) {
            const NavCookie &cookie = pCookieTrail->NthOldest(i);
            if (segment_number == cookie.SegmentNumber) {
                return true;
            }
        }
    }
    return false;
}

bool WRoadNav::CookieCutter(NavCookie &cookie, const UMath::Vector3 &centre, float projection, bool pass_left,
                            unsigned int cut_flags) {
    bVector2 cookie_to_centre(centre.x - cookie.Centre.x, centre.z - cookie.Centre.z);
    float l = bCross(&cookie_to_centre, reinterpret_cast<const bVector2 *>(&cookie.Forward));
    float left_offset = cookie.LeftOffset;
    float right_offset = cookie.RightOffset;

    if (l != bClamp(l, left_offset - projection, right_offset + projection)) {
        return false;
    }

    cookie.Flags |= 1 | cut_flags;

    bVector2 cookie_centre(cookie.Centre.x, cookie.Centre.z);
    bVector2 cookie_right(cookie.Forward.y, -cookie.Forward.x);

    float minimum_width;
    if (GetNavType() == kTypeTraffic) {
        minimum_width = 0.1f;
    } else {
        minimum_width = 1.0f;
    }

    if (pass_left) {
        right_offset = bMax(left_offset + minimum_width, l - projection);
        bScaleAdd(reinterpret_cast<bVector2 *>(&cookie.Right), &cookie_centre, &cookie_right, right_offset);
        cookie.RightOffset = right_offset;
    } else {
        left_offset = bMin(l + projection, right_offset - minimum_width);
        bScaleAdd(reinterpret_cast<bVector2 *>(&cookie.Left), &cookie_centre, &cookie_right, left_offset);
        cookie.LeftOffset = left_offset;
    }

    return true;
}

void WRoadNav::ClampCookieCentres(NavCookie *cookies, int num_cookies) {
    for (int i = 0; i < num_cookies; i++) {
        NavCookie &cookie = cookies[i];
        if (cookie.Flags & 1) {
            float size = (cookie.RightOffset - cookie.LeftOffset) * 0.5f;
            float cx = (cookie.Left.x + cookie.Right.x) * 0.5f;
            float cz = (cookie.Left.y + cookie.Right.y) * 0.5f;
            cookie.RightOffset = size;
            cookie.Centre.x = cx;
            cookie.Centre.z = cz;
            cookie.LeftOffset = -size;
        }
    }
}

static float TimeToClosestApproach(const UMath::Vector3 &p0, const UMath::Vector3 &v0, const UMath::Vector3 &p1, const UMath::Vector3 &v1, float *closing_speed) {
    UMath::Vector3 p;
    UMath::Vector3 v;
    UMath::Vector3 dir;
    p = UVector3(p1) - p0;
    v = UVector3(v1) - v0;
    UMath::Unit(p, dir);
    float a = UMath::Dot(dir, v);
    *closing_speed = -a;
    float b = UMath::Dot(v, v);
    a = UMath::Dot(p, v);
    if (b < 0.001f) {
        return 1000.0f;
    }
    return -a / b;
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
        if (target) {
            my_pursuit_target = target->GetSimable();
        }
    }

    int num_avoidables = 0;

    IArticulatedVehicle *my_hitch;
    my_ai->QueryInterface(&my_hitch);

    const AvoidableList &avoidable_list = my_ai->GetAvoidableList();

    for (AvoidableList::const_iterator iter = avoidable_list.begin();
         iter != avoidable_list.end() && num_avoidables < listsize;
         iter++) {
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

void WRoadNav::HolePunchAvoidables(NavCookie *cookies, int num_cookies, float current_offset, float delta_offset) {
    if (num_cookies == 0) return;

    IVehicleAI *my_ai = pAIVehicle;
    if (!my_ai) return;

    IBody *avoidables[32];
    int num_avoidables = FetchAvoidables(avoidables, 32);
    if (num_avoidables == 0) return;

    IRigidBody *my_body;
    my_ai->QueryInterface(&my_body);

    const UMath::Vector3 &my_position = my_body->GetPosition();

    int my_cookie_index = ClosestCookieAhead(my_position, cookies, num_cookies, nullptr);
    if (my_cookie_index < 0) return;

    const NavCookie &my_cookie = cookies[my_cookie_index];

    bool is_racer = false;
    int closest_avoidable = num_cookies;

    const UMath::Vector3 &my_velocity = my_body->GetLinearVelocity();

    UMath::Vector3 my_right;
    my_body->GetRightVector(my_right);
    UMath::Vector3 my_forward;
    my_body->GetForwardVector(my_forward);
    UMath::Vector3 my_dimension;
    my_body->GetDimension(my_dimension);

    float my_speed = my_body->GetSpeed();

    bVector2 nav_forward(fForwardVector.x, fForwardVector.z);
    bNormalize(&nav_forward, &nav_forward);

    if (GetPathType() == kPathRacer || GetPathType() == kPathPlayer) {
        is_racer = true;
    }

    bool is_traffic = GetNavType() == kTypeTraffic;
    bool is_drag = GRaceStatus::IsDragRace();

    for (int i = 0; i < num_avoidables; i++) {
        IBody *avoidable_body = avoidables[i];

        UMath::Matrix4 tranform;
        avoidable_body->GetTransform(tranform);

        const UMath::Vector3 &avoidable_forward = UMath::Vector4To3(tranform.v2);
        const UMath::Vector3 &avoidable_right = UMath::Vector4To3(tranform.v0);

        UMath::Vector3 avoidable_position;
        avoidable_position.y = tranform.v3.y;
        avoidable_position.z = tranform.v3.z;
        avoidable_position.x = tranform.v3.x;

        float elevation = avoidable_position.y - my_cookie.Centre.y;
        if (bClamp(elevation, -5.0f, 5.0f) != elevation) continue;

        UMath::Vector3 avoidable_dimension;
        avoidable_body->GetDimension(avoidable_dimension);

        IVehicle *his_vehicle;
        DriverClass his_class;
        if (!avoidable_body->QueryInterface(&his_vehicle)) {
            his_class = DRIVER_NONE;
        } else {
            his_class = his_vehicle->GetDriverClass();
        }

        bool he_is_traffic = false;
        if (his_vehicle && (his_class == DRIVER_TRAFFIC || his_class == DRIVER_NONE)) {
            he_is_traffic = true;
        }

        if (is_racer && he_is_traffic) {
            float facing = avoidable_right.x * my_cookie.Forward.x + avoidable_right.z * my_cookie.Forward.y;
            if (facing < 0.0f) {
                facing = -facing;
            }
            if (facing > 0.707f && his_vehicle && his_vehicle->GetVehicleClass() == VehicleClass::TRAILER) {
                UMath::ScaleAdd(avoidable_forward, -6.0f, avoidable_position, avoidable_position);
                avoidable_dimension.x = 1.8f;
                avoidable_dimension.z = 1.8f;
            }
        }

        UMath::Vector3 avoidable_to_me;
        UMath::Sub(avoidable_position, my_position, avoidable_to_me);

        float dot_fwd = avoidable_forward.x * my_cookie.Forward.x + avoidable_forward.z * my_cookie.Forward.y;
        if (dot_fwd < 0.0f) dot_fwd = -dot_fwd;
        float dot_right = avoidable_right.x * my_cookie.Forward.x + avoidable_right.z * my_cookie.Forward.y;
        if (dot_right < 0.0f) dot_right = -dot_right;
        float his_extent = dot_right * avoidable_dimension.x + dot_fwd * avoidable_dimension.z + avoidable_dimension.x + 0.5f;

        float dist_ahead = avoidable_to_me.x * my_cookie.Forward.x + avoidable_to_me.z * my_cookie.Forward.y;

        float my_extent = my_dimension.x + 0.5f + my_dimension.z;

        float dot_my_fwd = UMath::Abs(UMath::Dot(my_right, avoidable_forward));
        float dot_my_right = UMath::Abs(UMath::Dot(my_right, avoidable_right));
        float extent_side = dot_my_right * avoidable_dimension.x + dot_my_fwd * avoidable_dimension.z + my_dimension.x;

        float dist_side = UMath::Abs(UMath::Dot(avoidable_to_me, my_right));

        unsigned int cut_flags = 0;

        if (is_traffic || my_speed < 20.0f) {
            if (dist_ahead + his_extent < my_extent) {
                cut_flags = 2;
            }
        } else {
            if (dist_ahead - his_extent <= my_extent) {
                cut_flags = 2;
            }
        }

        float combined_extent = my_extent + his_extent;

        if (dist_ahead < -combined_extent) continue;
        if (dist_ahead + his_extent < 0.0f && dist_side < extent_side) continue;

        UMath::Vector3 his_velocity;
        avoidable_body->GetLinearVelocity(his_velocity);

        float gap_ahead = UMath::Max(0.0f, dist_ahead - combined_extent);

        UMath::Vector3 my_nose;
        my_nose.x = my_position.x;
        my_nose.y = my_position.y;
        my_nose.z = my_position.z;

        float my_nose_ahead = UMath::Min(my_extent, gap_ahead);
        my_nose.x += my_cookie.Forward.x * my_nose_ahead;
        my_nose.z += my_cookie.Forward.y * my_nose_ahead;

        float his_nose_ahead = UMath::Min(his_extent, gap_ahead);

        UMath::Vector3 his_nose;
        his_nose.y = avoidable_position.y;
        his_nose.x = avoidable_position.x - my_cookie.Forward.x * his_nose_ahead;
        his_nose.z = avoidable_position.z - my_cookie.Forward.y * his_nose_ahead;

        float closing_speed = 0.0f;
        float approach_time = TimeToClosestApproach(my_nose, my_velocity, his_nose, his_velocity, &closing_speed);

        bool blocked_traffic = false;
        if (cut_flags == 0) {
            if (closing_speed <= 0.0f) {
                float trailing_speed = combined_extent;
                if (is_traffic) {
                    trailing_speed = combined_extent + my_speed * 0.5f + my_extent + my_extent;
                }
                if (dist_ahead > trailing_speed) continue;
                if (is_traffic && dist_side > extent_side + 1.0f) continue;
            }
            blocked_traffic = is_traffic;
        }

        if (blocked_traffic || approach_time < 3.0f) {
            UMath::Vector3 point_of_impact;
            point_of_impact.x = avoidable_position.x;
            point_of_impact.y = avoidable_position.y;
            point_of_impact.z = avoidable_position.z;

            float closing_along = his_velocity.x * my_cookie.Forward.x + his_velocity.z * my_cookie.Forward.y;
            float time_offset = approach_time * closing_along - combined_extent;
            point_of_impact.z += my_cookie.Forward.y * time_offset;
            point_of_impact.x += my_cookie.Forward.x * time_offset;

            int closest_cookie = ClosestCookieAhead(point_of_impact, cookies, num_cookies, nullptr);
            if (closest_cookie > -1) {
                const NavCookie &cookie = cookies[closest_cookie];

                UMath::Vector3 right;
                UMath::Scale(avoidable_right, avoidable_dimension.x, right);
                UMath::Vector3 forward;
                UMath::Scale(avoidable_forward, avoidable_dimension.z, forward);

                bVector2 left_diagonal(forward.x - right.x, forward.z - right.z);
                bVector2 right_diagonal(forward.x + right.x, forward.z + right.z);
                bVector2 avoidable_velocity(his_velocity.x, his_velocity.z);

                float avoidable_delta_offset = bCross(&avoidable_velocity, reinterpret_cast<const bVector2 *>(&cookie.Forward));

                if (closest_cookie < closest_avoidable && dist_ahead > combined_extent) {
                    fOccludingTrailSpeed = closing_along;
                    closest_avoidable = closest_cookie;
                }

                UMath::Vector3 cut_to_position;
                cut_to_position.x = point_of_impact.x;
                cut_to_position.y = point_of_impact.y;
                cut_to_position.z = point_of_impact.z;

                float lateral_projection = bClamp(approach_time, 0.0f, 1.0f);
                float offset_change = avoidable_delta_offset * lateral_projection;

                cut_to_position.x += offset_change * cookie.Forward.y * 0.8f;
                cut_to_position.z -= offset_change * cookie.Forward.x * 0.8f;

                bVector2 cookie_to_avoidable(cut_to_position.x - cookie.Centre.x, cut_to_position.z - cookie.Centre.z);

                float extra_width = offset_change * 0.2f;

                bVector2 cookie_to_me(my_position.x - cookie.Centre.x, my_position.z - cookie.Centre.z);

                float my_d = bDot(&cookie_to_me, reinterpret_cast<const bVector2 *>(&cookie.Forward));
                float avoidable_d = bDot(&cookie_to_avoidable, reinterpret_cast<const bVector2 *>(&cookie.Forward));

                float close_factor = UMath::Ramp(avoidable_d - my_d, -6.0f, 12.0f);

                float right_projection = bCross(&right_diagonal, reinterpret_cast<const bVector2 *>(&cookie.Forward));
                float left_projection = bCross(&left_diagonal, reinterpret_cast<const bVector2 *>(&cookie.Forward));
                float avoidable_half_width = bAbs(right_projection);
                float tmp = bAbs(left_projection);
                avoidable_half_width = bMax(avoidable_half_width, tmp);

                float adjusted_width = extra_width * close_factor + avoidable_half_width;

                float nav_cross = bCross(reinterpret_cast<const bVector2 *>(&nav_forward), reinterpret_cast<const bVector2 *>(&cookie.Forward));
                float new_current_offset = lateral_projection * close_factor * delta_offset * 0.2f + current_offset + nav_cross + nav_cross;

                float hole_punch_safety_margin = close_factor;
                if (is_drag) {
                    hole_punch_safety_margin = close_factor * 0.8f;
                }

                float avoidable_offset = bCross(&cookie_to_avoidable, reinterpret_cast<const bVector2 *>(&cookie.Forward));

                float gap_left = avoidable_offset - adjusted_width - cookie.LeftOffset;
                float gap_right = cookie.RightOffset - avoidable_offset - adjusted_width;
                float gap_required = hole_punch_safety_margin + fVehicleHalfWidth;

                bool fit_left = gap_left > gap_required;
                bool fit_right = gap_right > gap_required;

                bool pass_left = new_current_offset < avoidable_offset;
                if (fit_right && !fit_left) {
                    pass_left = false;
                } else if (!fit_right && fit_left) {
                    pass_left = true;
                }

                float total_width = adjusted_width + fVehicleHalfWidth + hole_punch_safety_margin;

                int i = closest_cookie;
                if (closest_cookie < num_cookies) {
                    while (true) {
                        NavCookie &this_cookie = cookies[i];
                        int result = CookieCutter(this_cookie, cut_to_position, total_width, pass_left, cut_flags);
                        if (result == 0 && i == closest_cookie) break;

                        UMath::Vector2 delta;
                        delta.x = point_of_impact.x - this_cookie.Centre.x;
                        delta.y = point_of_impact.y - this_cookie.Centre.y;
                        float dist_to_tail = bDot(reinterpret_cast<const bVector2 *>(&delta), reinterpret_cast<const bVector2 *>(&this_cookie.Forward)) + combined_extent + combined_extent;
                        if (dist_to_tail < 0.0f) break;
                        i++;
                        if (i >= num_cookies) break;
                    }
                }
            }
        }
    }

    ClampCookieCentres(cookies, num_cookies);
}

void WRoadNetwork::GetPointAndVecOnSegment(const WRoadSegment &segment, float d, UMath::Vector3 &point, UMath::Vector3 &vec) {
    WRoadNetwork &roadNetwork = Get();
    GetPointOnSegment(segment, d, point);
    if (segment.IsCurved()) {
        static USpline roadSpline;
        roadNetwork.BuildSegmentSpline(segment, roadSpline);
        UMath::Vector4 tangent;
        roadSpline.EvaluateTangent(d, tangent);
        vec = UMath::Vector4To3(tangent);
    } else {
        roadNetwork.GetSegmentForwardVector(segment, vec);
    }
}

float WRoadNetwork::GetLinePointIntersect(const UMath::Vector3 &start, const UMath::Vector3 &end, const UMath::Vector3 &pt, UMath::Vector3 &intersect, bool checkBound) {
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
        }
        if (dist <= 0.0f) {
            intersect = start;
            return 0.0f;
        }
    }

    UMath::Unit(segVec, segVec);
    UMath::Scale(segVec, dist, segVec);
    UMath::Add(start, segVec, intersect);
    return dist / segDist;
}

float WRoadNetwork::GetSegmentPointIntersect(const WRoadSegment &segment, const UMath::Vector3 &pt, UMath::Vector3 &intersect, bool checkBound) {
    UMath::Vector3 pos;
    UMath::Vector3 pos2;
    const WRoadNode *node0 = GetNode(segment.fNodeIndex[0]);
    const WRoadNode *node1 = GetNode(segment.fNodeIndex[1]);
    pos = node0->fPosition;
    pos2 = node1->fPosition;
    return GetLinePointIntersect(pos, pos2, pt, intersect, checkBound);
}

bool WRoadNav::OnPath() const {
    if (fNavType != kTypePath || !IsValid() || pPathSegments == nullptr || nPathSegments <= 0) {
        return false;
    }
    int i;
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(fSegmentInd);
    const WRoadNode *node = roadNetwork.GetNode(segment->fNodeIndex[static_cast<int>(fNodeInd)]);
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
    return false;
}

short WRoadNav::GetNextOffset(const UMath::Vector3 &to, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos) {
    useOldStartPos = true;
    short newSegInd = GetSegmentInd();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(newSegInd);
    const WRoadNode *node = roadNetwork.GetNode(segment->fNodeIndex[static_cast< int >(GetNodeInd())]);
    bool end_of_path = false;

    if (fNavType == kTypePath) {
        bool found = false;
        if (pPathSegments != nullptr) {
            int i;
            for (i = 0; i < nPathSegments; i++) {
                if (fSegmentInd == pPathSegments[i]) {
                    break;
                }
            }
            if (++i < nPathSegments) {
                int new_segment_index = pPathSegments[i];
                const WRoadNode *new_nodes[2];
                const WRoadSegment *new_segment = roadNetwork.GetSegment(new_segment_index);
                roadNetwork.GetSegmentNodes(*new_segment, new_nodes);
                bool match_first = (node == new_nodes[0]);
                bool match_second = (node == new_nodes[1]);
                if (match_first || match_second) {
                    nodeInd = match_first;
                    newSegInd = static_cast< short >(new_segment_index);
                } else {
                    nodeInd = nodeInd ^ 1;
                    useOldStartPos = false;
                }
                found = true;
            } else {
                end_of_path = true;
            }
        }
        if (!found) {
            if (fPathType == kPathGPS) {
                newSegInd = GetSegmentInd();
                return newSegInd;
            }
            SetNavType(kTypeDirection);
        }
    }

    if (fNavType == kTypeDirection) {
        UMath::Vector3 toVec = to;
        UMath::Unit(toVec, toVec);
        const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(node, GetSegmentInd());

        if (node->fNumSegments > 1) {
            if (checkSegment != nullptr) {
                newSegInd = checkSegment->fIndex;
                nodeInd = (node == roadNetwork.GetNode(checkSegment->fNodeIndex[0]));
            } else if (!segment->IsDecision()) {
                unsigned int shortcut_cached = 0;
                unsigned int shortcut_allowed = 0;
                unsigned char shortcut_number = GetShortcutNumber();
                float closest_to_target = 2.0f;

                if (shortcut_number != 0xFF) {
                    int mask = 1 << shortcut_number;
                    shortcut_cached |= mask;
                    shortcut_allowed |= mask;
                }

                float target_dot;
                if (bAiRandomTurns) {
                    target_dot = bRandom(1.0f);
                } else {
                    target_dot = 1.0f;
                }

                for (int i = 0; i < static_cast< int >(node->fNumSegments); i++) {
                    if (node->fSegmentIndex[i] == GetSegmentInd()) continue;

                    bool respect_full_barriers = RespectFullBarriers();
                    bool respect_drive_through_barriers = RespectDriveThroughBarriers();

                    const WRoadSegment *newRoadSegment = roadNetwork.GetSegment(node->fSegmentIndex[i]);
                    float worst_gap_to_target = 0.0f;
                    const int kMaxWalkSegments = 19;
                    const float kMaxWalkDistance = 100.0f;
                    float distance = kMaxWalkDistance;
                    const WRoadNode *walkRoadNode = node;
                    const WRoadSegment *walkRoadSegment = newRoadSegment;

                    for (int w = 0; ; ) {
                        if (respect_full_barriers && walkRoadSegment->CrossesBarrier(respect_drive_through_barriers)) {
                            walkRoadSegment = nullptr;
                            break;
                        }

                        bool walk_segment_forward = (walkRoadNode == roadNetwork.GetNode(walkRoadSegment->fNodeIndex[0]));

                        if (bRaceFilter) {
                            if (!walkRoadSegment->IsInRace()) {
                                walkRoadSegment = nullptr;
                                break;
                            }
                            bool race_route_forward = walkRoadSegment->RaceRouteForward();
                            if (walk_segment_forward) {
                                if (!race_route_forward) {
                                    walkRoadSegment = nullptr;
                                    break;
                                }
                            } else if (race_route_forward) {
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
                        if (w > 0 && distance <= 0.0f) break;

                        const WRoadNode *oppNode = roadNetwork.GetSegmentOppNode(*walkRoadSegment, walkRoadNode);
                        const WRoadSegment *nextSeg = GetAttachedDirectionalSegment(oppNode, walkRoadSegment->fIndex);
                        if (nextSeg == nullptr) {
                            if (w == 0) {
                                walkRoadSegment = nullptr;
                            }
                            break;
                        }
                        if (oppNode != roadNetwork.GetNode(nextSeg->fNodeIndex[0]) && nextSeg->IsOneWay()) {
                            walkRoadSegment = nullptr;
                            break;
                        }
                        if (end_of_path) break;
                        w++;
                        walkRoadSegment = nextSeg;
                        walkRoadNode = oppNode;
                        if (w > kMaxWalkSegments) break;
                    }

                    if (walkRoadSegment != nullptr && closest_to_target > worst_gap_to_target) {
                        newSegInd = node->fSegmentIndex[i];
                        char towards = (node == roadNetwork.GetNode(roadNetwork.GetSegment(newSegInd)->fNodeIndex[0]));
                        nodeInd = towards;
                        closest_to_target = worst_gap_to_target;
                    }
                }

                if (newSegInd == GetSegmentInd()) {
                    nodeInd = nodeInd ^ 1;
                    useOldStartPos = false;
                }
            } else {
                nodeInd = nodeInd ^ 1;
                useOldStartPos = false;
            }
        } else {
            nodeInd = nodeInd ^ 1;
            useOldStartPos = false;
        }
    }

    nextLaneOffset = SnapToSelectableLane(fLaneOffset, newSegInd, nodeInd);
    return newSegInd;
}

void WRoadNav::ChangeLanes(float new_lane_offset, float dist) {
    if (dist > 0.0f) {
        float old_lane_offset = fToLaneOffset;
        fLaneChangeInc = 0.0f;
        fLaneOffset = old_lane_offset;
        fToLaneOffset = new_lane_offset;
        fLaneChangeDist = dist;
        fFromLaneOffset = old_lane_offset;
        return;
    }
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

bool WRoadNav::IncLane(int direction) {
    if (!IsValid()) {
        return false;
    }

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(GetSegmentInd());
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, GetNodeInd());

    bool backward = (GetNodeInd() == 0);
    bool inverted = segment->IsProfileInverted(GetNodeInd());
    bool inverted_xor_backward = inverted ^ backward;

    int current_lane = 0;
    {
        for (int n = 0; n < profile->fNumZones - 1; n++) {
            float width = profile->GetLaneWidth(n, inverted_xor_backward);
            float offset = profile->GetRelativeLaneOffset(n, inverted_xor_backward);
            if (fLaneOffset > width * 0.5f + offset) {
                current_lane++;
            }
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

void WRoadNav::EvaluateSplines(const WRoadSegment *segment) {
    if (segment->IsCurved()) {
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
    if (!IsValid()) return;
    if (!bCookieTrail || pCookieTrail == nullptr) return;

    int num_cookies = pCookieTrail->Count();
    float cookie_length = 0.0f;
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
        if (!add_new_cookie) return;
    }

    NavCookie cookie;
    cookie.SetSegmentParameter(GetSegmentTime());
    cookie.Centre = GetPosition();
    cookie.Flags = 0;
    cookie.Length = cookie_length;
    cookie.Curvature = GetCurvature();
    cookie.SegmentNumber = GetSegmentInd() & 0x7FFF;
    cookie.SegmentNodeInd = GetNodeInd() & 1;

    bVector2 centre(GetPosition().x, GetPosition().z);

    cookie.Left = UMath::Vector2Make(GetLeftPosition().x, GetLeftPosition().z);
    cookie.Right = UMath::Vector2Make(GetRightPosition().x, GetRightPosition().z);

    bVector2 centre_to_left = *reinterpret_cast<bVector2 *>(&cookie.Left) - centre;
    bVector2 centre_to_right = *reinterpret_cast<bVector2 *>(&cookie.Right) - centre;
    bVector2 right = centre_to_right - centre_to_left;
    bVector2 forward(-right.y, right.x);

    bNormalize(reinterpret_cast<bVector2 *>(&cookie.Forward), &forward);

    cookie.LeftOffset = bCross(&centre_to_left, reinterpret_cast<const bVector2 *>(&cookie.Forward));
    cookie.RightOffset = bCross(&centre_to_right, reinterpret_cast<const bVector2 *>(&cookie.Forward));

    if (num_cookies == 0) {
        mCurrentCookie = cookie;
    }

    if (pCookieTrail->Count() == pCookieTrail->Capacity()) {
        nCookieIndex = bMax(nCookieIndex - 1, 0);
    }

    pCookieTrail->AddNew(cookie);
}

void WRoadNav::IncNavPosition(float dist, const UMath::Vector3 &to, float max_lookahead) {
    if (!fValid) return;

    float cookie_gap = 3.0f;
    if (max_lookahead > 0.0f) {
        cookie_gap = UMath::Clamp(max_lookahead * (1.0f / 26.0f), 1.0f, cookie_gap);
    }

    if (bCookieTrail && pCookieTrail != nullptr) {
        while (dist > 0.0f) {
            float incdist = bMin(cookie_gap * 1.1f, dist);
            PrivateIncNavPosition(incdist, to);
            dist -= incdist;
            UpdateCookieTrail(cookie_gap);
        }
    } else {
        PrivateIncNavPosition(dist, to);
    }
}

void WRoadNav::PrivateIncNavPosition(float dist, const UMath::Vector3 &to) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment;
    float segmentLength;
    float distFraction;
    float toLength;

    for (;;) {
        segment = roadNetwork.GetSegment(GetSegmentInd());
        segmentLength = UMath::Max(0.01f, UMath::Distance(fStartPos, fEndPos));
        distFraction = dist / segmentLength;
        toLength = UMath::Length(to);

        if (fSegTime + distFraction <= 1.0f) {
            fSegTime = fSegTime + distFraction;
            fSegTime = UMath::Min(UMath::Max(fSegTime, 0.0f), 1.0f);

            if (UpdateLaneChange(dist)) {
                SetStartEndPos(*segment, fLaneOffset);
                SetStartEndControls(*segment);
                RebuildSplines(segment);
            }

            EvaluateSplines(segment);
            return;
        }

        {
            short newSegInd = GetSegmentInd();

            UpdateLaneChange((1.0f - fSegTime) * segmentLength);

            char old_node_ind = fNodeInd;
            float nextLaneOffset = fLaneOffset;
            bool useOldStartPos = false;
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

            if (GetSegmentInd() == newSegInd && fNodeInd == old_node_ind) {
                fDeadEnd = 1;
                if (fNavType == kTypeTraffic) return;
            }

            fSegmentInd = static_cast< short >(newSegInd);
            newSegment = roadNetwork.GetSegment(GetSegmentInd());

            if (useOldStartPos) {
                fStartPos = fEndPos;
                if (bCookieTrail) {
                    fLeftStartPos = fLeftEndPos;
                    fRightStartPos = fRightEndPos;
                }
                SetBoundPos(*newSegment, nextLaneOffset, false);
                fLaneOffset = nextLaneOffset;
                fToLaneOffset = nextLaneOffset;
                fFromLaneOffset = nextLaneOffset;
            } else {
                SetStartEndPos(*newSegment, fLaneOffset);
            }

            SetStartEndControls(*newSegment);
            RebuildSplines(newSegment);
            fPosition = fStartPos;
            dist = (distFraction - (1.0f - fSegTime)) * segmentLength;
            fSegTime = 0.0f;
        }
    }
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

void WRoadNav::UpdateOccludedPosition(bool occlude_avoidables) {
    if (!bCookieTrail) return;

    nRoadOcclusion = 0;
    nAvoidableOcclusion = 0;
    fOccludingTrailSpeed = 0.0f;
    bOccludedFromBehind = false;

    ISimable *simable = pAIVehicle ? pAIVehicle->GetSimable() : nullptr;
    IRigidBody *car = simable ? simable->GetRigidBody() : nullptr;
    if (!car) return;

    int num_cookies = pCookieTrail->Count();
    if (num_cookies <= 0) return;

    UMath::Vector3 car_forward_3d;
    car->GetForwardVector(car_forward_3d);
    const UMath::Vector3 &car_velocity_3d = car->GetLinearVelocity();
    UMath::Vector3 car_position_3d;
    UMath::ScaleAdd(car_forward_3d, 0.0f, car->GetPosition(), car_position_3d);

    bVector2 car_position(car_position_3d.x, car_position_3d.z);

    bool traffic = (fNavType == kTypeTraffic);
    float look_min = 0.0f;
    float look_max;
    float out_scale;
    float out_bounds;
    if (traffic) {
        look_max = 80.0f;
        out_bounds = 6.0f;
    } else {
        look_max = 200.0f;
        out_bounds = 10.0f;
    }
    out_scale = 4.0f;

    int n = nCookieIndex;
    float current_dot = 0.0f;
    float look_ahead = look_min;
    if (n < num_cookies) {
        for (; n < num_cookies; n++) {
            const NavCookie &cookie = pCookieTrail->NthOldest(n);
            bVector2 cookie_to_car(car_position.x - cookie.Centre.x, car_position.y - cookie.Centre.z);
            float dot = bDot(reinterpret_cast< const bVector2 * >(&cookie.Forward), &cookie_to_car);
            if (dot >= current_dot) {
                nCookieIndex = n;
                float offset = bCross(&cookie_to_car, reinterpret_cast< const bVector2 * >(&cookie.Forward));
                float out_of_bounds = bMax(cookie.LeftOffset - offset, offset - cookie.RightOffset);
                look_ahead = out_scale * bMax(0.0f, -(out_bounds + out_of_bounds)) + look_ahead;
                current_dot = dot;
                if (look_max - look_ahead < 0.0f) {
                    look_ahead = look_max;
                }
            }
            if (-look_ahead > dot) break;
        }
    }

    const NavCookie &current_cookie = pCookieTrail->NthOldest(nCookieIndex);
    int next_cookie_index = nCookieIndex + 1;

    if (next_cookie_index < num_cookies) {
        const NavCookie &next_cookie = pCookieTrail->NthOldest(next_cookie_index);
        bVector2 cookie_to_car(car_position.x - next_cookie.Centre.x, car_position.y - next_cookie.Centre.z);
        float sum = current_dot + bAbs(bDot(reinterpret_cast< const bVector2 * >(&current_cookie.Forward), &cookie_to_car));
        float current_blend;
        float next_blend;
        if (sum > 1e-4f) {
            current_blend = 1.0f - current_dot / sum;
        } else {
            current_blend = 1.0f;
        }
        next_blend = 1.0f - current_blend;

        UMath::Lerp(current_cookie.Left, next_cookie.Left, current_blend, mCurrentCookie.Left);
        UMath::Lerp(current_cookie.Right, next_cookie.Right, current_blend, mCurrentCookie.Right);
        UMath::Lerp(current_cookie.Forward, next_cookie.Forward, current_blend, mCurrentCookie.Forward);

        float forward_len = bLength(reinterpret_cast< bVector2 * >(&mCurrentCookie.Forward));
        if (forward_len > 1e-6f) {
            float inv_len = 1.0f / forward_len;
            mCurrentCookie.Forward.x *= inv_len;
            mCurrentCookie.Forward.y *= inv_len;
        }

        mCurrentCookie.Centre.x = UMath::Lerp(current_cookie.Centre.x, next_cookie.Centre.x, current_blend);
        mCurrentCookie.Centre.y = UMath::Lerp(current_cookie.Centre.y, next_cookie.Centre.y, current_blend);
        mCurrentCookie.Centre.z = UMath::Lerp(current_cookie.Centre.z, next_cookie.Centre.z, current_blend);

        mCurrentCookie.LeftOffset = UMath::Lerp(current_cookie.LeftOffset, next_cookie.LeftOffset, current_blend);
        mCurrentCookie.RightOffset = UMath::Lerp(current_cookie.RightOffset, next_cookie.RightOffset, current_blend);

        int next_segment_number = next_cookie.SegmentNumber;
        int current_segment_number = current_cookie.SegmentNumber;

        if (current_segment_number == next_segment_number) {
            mCurrentCookie.SegmentNumber = next_segment_number;
            mCurrentCookie.SegmentNodeInd = current_cookie.SegmentNodeInd;
            float current_param = current_cookie.GetSegmentParameter();
            float next_param = next_cookie.GetSegmentParameter();
            float t = UMath::Lerp(current_param, next_param, current_blend);
            mCurrentCookie.SetSegmentParameter(t);
        } else {
            WRoadNetwork &rn = WRoadNetwork::Get();
            const WRoadSegment *next_segment = rn.GetSegment(next_segment_number);
            const WRoadSegment *current_segment = rn.GetSegment(current_segment_number);
            float next_segment_length = next_segment->GetLength();
            float current_segment_length = current_segment->GetLength();
            float next_dist = next_segment_length * next_cookie.GetSegmentParameter();
            float current_dist = current_segment_length * (1.0f - current_cookie.GetSegmentParameter());
            float distance = current_dist * current_blend + next_dist;
            if (distance >= 0.0f) {
                distance = distance / next_segment_length;
                if (distance < 0.0f) {
                    distance = 0.0f;
                }
                mCurrentCookie.SegmentNumber = next_segment_number;
            } else {
                distance = (distance + current_segment_length) / current_segment_length;
                mCurrentCookie.SegmentNumber = current_segment_number;
                if (distance < 0.0f) {
                    distance = 0.0f;
                }
            }
            mCurrentCookie.SegmentNodeInd = (distance >= 0.0f) ? next_cookie.SegmentNodeInd : current_cookie.SegmentNodeInd;
            if (1.0f - distance < 0.0f) {
                distance = 1.0f;
            }
            mCurrentCookie.SetSegmentParameter(distance);
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

    bVector2 occ_centre(mCurrentCookie.Centre.x, mCurrentCookie.Centre.z);
    bVector2 occ_to_car = car_position - occ_centre;
    float cross = bCross(&occ_to_car, reinterpret_cast< const bVector2 * >(&mCurrentCookie.Forward));
    float out = bMax(cross - mCurrentCookie.RightOffset, mCurrentCookie.LeftOffset - cross);
    mOutOfBounds = fVehicleHalfWidth + out;

    if (n < num_cookies) {
        NavCookie cookies[33];
        bMemSet(&cookies[num_cookies], 0, static_cast< unsigned int >(0x40));

        int i = nCookieIndex;
        for (; i < num_cookies; i++) {
            cookies[i] = pCookieTrail->NthOldest(i);
        }

        bVector2 car_velocity(car_velocity_3d.x, car_velocity_3d.z);

        if (occlude_avoidables) {
            float delta_offset = car_velocity.x * mCurrentCookie.Forward.y - mCurrentCookie.Forward.x * car_velocity.y;
            HolePunchAvoidables(&cookies[nCookieIndex], num_cookies - nCookieIndex, cross, delta_offset);
        }

        UMath::ScaleAdd(car_velocity_3d, 0.0f, car_position_3d, car_position_3d);
        bVector2 car_pos_updated(car_position_3d.x, car_position_3d.z);
        car_position = car_pos_updated;

        bInitializeBoundingBox(&vCookieTrailBoxMin, &vCookieTrailBoxMax);
        int first_index = nCookieIndex;
        int last_visible = nCookieIndex;
        int occluding_index = nCookieIndex;
        int left_limit_index = nCookieIndex;
        int right_limit_index = nCookieIndex;
        bVector2 *occluder = nullptr;
        bool left_occlusion = false;
        bVector2 left_limit;
        bVector2 right_limit;

        for (int j = nCookieIndex; j <= num_cookies; j++) {
            const NavCookie &cookie = (j < num_cookies) ? cookies[j] : *(reinterpret_cast<const NavCookie *>(&fPosition));
            bVector2 left(cookie.Left.x, cookie.Left.y);
            bVector2 right(cookie.Right.x, cookie.Right.y);
            bVector2 car_to_left = left - car_position;
            bVector2 car_to_right = right - car_position;

            if (n < num_cookies) {
                bExpandBoundingBox(&vCookieTrailBoxMin, &vCookieTrailBoxMax, &left);
                bExpandBoundingBox(&vCookieTrailBoxMin, &vCookieTrailBoxMax, &right);
            }

            if (j == first_index) {
                left_limit = car_to_left;
                right_limit = car_to_right;
            } else {
                const UMath::Vector3 &centre_3d = (j < num_cookies) ? cookies[j].Centre : fPosition;
                bool use_nav = (j != num_cookies);
                bVector2 centre(centre_3d.x, centre_3d.z);
                bVector2 car_to_centre = centre - car_position;

                if (j != num_cookies) {
                    if (left_limit.x * car_to_left.y - car_to_left.x * left_limit.y < 0.0f) {
                        left_limit = car_to_left;
                        left_limit_index = j;
                    }
                    if (0.0f < right_limit.x * car_to_right.y - car_to_right.x * right_limit.y) {
                        right_limit = car_to_right;
                        right_limit_index = j;
                    }
                }

                if (0.0f <= car_to_centre.x * left_limit.y - left_limit.x * car_to_centre.y) {
                    last_visible = j;
                    if (0.0f < car_to_centre.x * right_limit.y - right_limit.x * car_to_centre.y) {
                        left_occlusion = false;
                        last_visible = last_visible;
                        occluder = &right_limit;
                        occluding_index = right_limit_index;
                    }
                } else {
                    left_occlusion = true;
                    occluder = &left_limit;
                    occluding_index = left_limit_index;
                }
            }

            if (0.0f < left_limit.x * right_limit.y - right_limit.x * left_limit.y) break;
        }

        if (last_visible < num_cookies && occluder != nullptr) {
            const NavCookie &apex_cookie = cookies[occluding_index];
            UMath::Vector3 c;
            c.x = occluder->x;
            c.y = cookies[occluding_index].Centre.y - car_position_3d.y;
            c.z = occluder->y;
            UMath::Vector3 result;
            UMath::Add(car_position_3d, c, result);

            fApexPosition.x = result.x;
            fApexPosition.z = result.z;
            fApexPosition.y = result.y;

            if ((apex_cookie.Flags & 1) == 0) {
                nRoadOcclusion = left_occlusion ? -1 : 1;
            } else {
                nRoadOcclusion = 0;
            }

            if ((apex_cookie.Flags & 1) != 0) {
                nAvoidableOcclusion = left_occlusion ? -1 : 1;
            } else {
                nAvoidableOcclusion = 0;
            }

            int behind = 0;
            if (nAvoidableOcclusion != 0) {
                behind = (apex_cookie.Flags & 2) != 0 ? 1 : 0;
            }
            bOccludedFromBehind = behind;

            if (IsOccluded()) {
                float apex_width = bAbs(apex_cookie.RightOffset - apex_cookie.LeftOffset);
                bVector2 apex_2d(fApexPosition.x, fApexPosition.z);
                bVector2 apex_to_car = car_position - apex_2d;

                bVector2 to_left(cookies[occluding_index].Left.x - apex_2d.x, cookies[occluding_index].Left.y - apex_2d.y);
                bVector2 left_dir = bNormalize(to_left);

                bVector2 to_right(cookies[occluding_index].Right.x - apex_2d.x, cookies[occluding_index].Right.y - apex_2d.y);
                bVector2 right_dir = bNormalize(to_right);

                float dist_to_apex = bLength(&apex_to_car);

                bVector2 apex_to_nav = right_dir * dist_to_apex;
                bVector2 desired_position = apex_2d + apex_to_nav;

                bVector2 bisect = left_dir + right_dir;
                bVector2 perp = bNormalize(bisect);

                bVector2 desired_to_apex = desired_position - apex_2d;
                float projection = bDot(desired_to_apex, perp);
                projection = bMin(projection, apex_width);

                if (nAvoidableOcclusion != 0) {
                    float my_trailingspeed = bDot(car_velocity, perp);
                    float closing_speed = my_trailingspeed + my_trailingspeed;
                    if (closing_speed > 1e-6f) {
                        float ratio = UMath::Ramp(my_trailingspeed - fOccludingTrailSpeed, 0.0f, closing_speed);
                        ratio = ratio + ratio;
                    } else {
                        projection = 0.0f;
                    }
                    projection = projection * projection;
                    projection = bMin(projection, apex_width);
                }

                perp *= projection;
                fOccludedPosition.x = perp.x + fApexPosition.x;
                fOccludedPosition.z = perp.y + fApexPosition.z;
                fOccludedPosition.y = fApexPosition.y;
            } else {
                fOccludedPosition = fPosition;
            }
        }
    }

    if (nAvoidableOcclusion == 0) {
        bOccludedFromBehind = false;
        fOccludingTrailSpeed = 0.0f;
    }
}

float WRoadNav::FindClosestOnSpline(const UMath::Vector3 &point, UMath::Vector3 &intersectPoint, float &timeStep, float incStep, int segInd) {
    UMath::Vector3 newIntersectPoint;
    UMath::Vector3 pointToIntersect;
    UMath::Sub(intersectPoint, point, pointToIntersect);

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);

    float timeStepInc = incStep / UMath::Max(1.0f, segment->GetLength());

    UMath::Vector3 segmentForwardVec;
    roadNetwork.GetSegmentForwardVector(segment->fIndex, segmentForwardVec);

    if (UMath::Dot(segmentForwardVec, pointToIntersect) > 0.0f) {
        timeStepInc = -timeStepInc;
    }

    float currDistance = UMath::Distance(intersectPoint, point);

    float newTimeStep = timeStep + timeStepInc;
    if (newTimeStep > 0.0f && newTimeStep < 1.0f) {
        while (true) {
            roadNetwork.GetPointOnSegment(*segment, newTimeStep, newIntersectPoint);

            float distToNewIntersect = UMath::Distance(newIntersectPoint, point);

            if (distToNewIntersect > currDistance) break;

            intersectPoint = newIntersectPoint;
            timeStep = newTimeStep;
            newTimeStep += timeStepInc;
            currDistance = distToNewIntersect;

            if (newTimeStep <= 0.0f || newTimeStep >= 1.0f) break;
        }
    }

    return currDistance;
}

int WRoadNav::FindClosestSegmentInd(const UMath::Vector3 &point, const UMath::Vector3 &dir, float dirWeight, UMath::Vector3 &closestPoint, float &time) {
    typedef UTL::Std::set<unsigned short, _type_set> SEGMENT_SET;
    const WGrid &grid = WGrid::Get();
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    short segInd = -1;
    bool found = false;
    float foundScore = 0.0f;
    UTL::FastVector<unsigned int, 16> node_indices;
    UMath::Vector3 ndir;
    SEGMENT_SET segment_set;
    UMath::Vector3 intersectPoint;
    UMath::Vector3 intersectDir;
    UMath::Vector3 intersectRight;
    UMath::Vector3 offset;
    UMath::Vector3 start;
    UMath::Vector3 end;
    UMath::Vector3 segdir;
    float timeStep;
    float rightedge[2];
    float leftedge[2];

    node_indices.reserve(0x40);
    grid.FindNodes(point, 500.0f, node_indices);

    ndir = dir;
    UMath::Normalize(ndir);

    for (unsigned int *iter = node_indices.begin(); iter != node_indices.end(); ++iter) {
        WGridNode *grid_node = grid.fNodes[*iter];
        if (grid_node != nullptr) {
            unsigned int numSegments = grid_node->GetElemTypeCount(WGrid_kRoadSegment);
            for (unsigned int i = 0; i < numSegments; ++i) {
                short seg = static_cast< short >(grid_node->GetElemType(i, WGrid_kRoadSegment));
                segment_set.insert(seg);
            }
        }
    }

    for (SEGMENT_SET::const_iterator it = segment_set.begin(); it != segment_set.end(); ++it) {
        short index = *it;
        if (index >= static_cast< int >(roadNetwork.GetNumSegments())) continue;
        const WRoadSegment *segment = roadNetwork.GetSegment(index);

        if (bDecisionFilter && segment->IsDecision()) continue;
        if (bRaceFilter && !segment->IsInRace()) continue;
        if (bTrafficFilter && !segment->IsTrafficAllowed()) continue;
        if (bCopFilter && !segment->ShouldCopsConsider()) continue;

        timeStep = roadNetwork.GetSegmentPointIntersect(*segment, point, intersectPoint, true);

        if (segment->IsCurved()) {
            roadNetwork.GetPointOnSegment(*segment, timeStep, intersectPoint);
            FindClosestOnSpline(point, intersectPoint, timeStep, 1.0f, static_cast< int >(segment->fIndex));
            FindClosestOnSpline(point, intersectPoint, timeStep, 0.25f, static_cast< int >(segment->fIndex));
        }

        VU0_v3sub(point, intersectPoint, intersectDir);
        float currDistance = VU0_sqrt(VU0_v3lengthsquare(intersectDir));
        float currScore;

        if (dirWeight > 0.0f) {
            roadNetwork.GetPointAndVecOnSegment(*segment, timeStep, intersectPoint, intersectDir);

            offset.x = intersectDir.z;
            offset.y = 0.0f;
            offset.z = -intersectDir.x;
            intersectRight = offset;
            UMath::Normalize(intersectRight);

            UMath::Sub(point, intersectPoint, offset);
            float sideDistance = UMath::Dot(intersectRight, offset);

            for (int i = 0; i < 2; i++) {
                bool inverted = segment->IsProfileInverted(i);
                const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, i);

                float relOffset = profile->GetRelativeLaneOffset(inverted ? profile->fNumZones - 1 : 0, inverted);
                float laneWidth = profile->GetLaneWidth(inverted ? profile->fNumZones - 1 : 0, inverted);
                rightedge[i] = relOffset - laneWidth * 0.5f;

                relOffset = profile->GetRelativeLaneOffset(inverted ? 0 : profile->fNumZones - 1, inverted);
                laneWidth = profile->GetLaneWidth(inverted ? 0 : profile->fNumZones - 1, inverted);
                leftedge[i] = relOffset + laneWidth * 0.5f;
            }

            float right = rightedge[0] + (rightedge[1] - rightedge[0]) * timeStep;
            float left = leftedge[0] + (leftedge[1] - leftedge[0]) * timeStep;

            float oldSideDistance;
            if (sideDistance > right) {
                oldSideDistance = sideDistance - right;
            } else if (sideDistance < left) {
                oldSideDistance = sideDistance - left;
            } else {
                oldSideDistance = 0.0f;
            }

            UMath::ScaleAdd(intersectRight, oldSideDistance - sideDistance, offset, offset);
            currDistance = UMath::Length(offset);

            start = roadNetwork.GetNode(segment->fNodeIndex[0])->fPosition;
            end = roadNetwork.GetNode(segment->fNodeIndex[1])->fPosition;
            UMath::Sub(end, start, segdir);
            UMath::Normalize(segdir);

            float dot = UMath::Dot(ndir, segdir);
            if (!segment->IsOneWay()) {
                dot = UMath::Abs(dot);
            }

            currScore = currDistance + dirWeight * (1.0f - dot) * 100.0f;
        } else {
            currScore = currDistance;
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

void WRoadNav::InitAtSegment(short segInd, char laneInd, float timeStep) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);
    UMath::Vector3 vec;

    fDeadEnd = 0;
    fValid = true;
    fSegmentInd = segInd;

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
        const WRoadProfile *profile;
        float startOffset;
        float endOffset;
        roadNetwork.GetSegmentNodes(*segment, nodePtr);

        profile = roadNetwork.GetProfile(nodePtr[fNodeInd == 0]->fProfileIndex);
        startOffset = profile->GetRawLaneOffset(static_cast< int >(laneInd));

        profile = roadNetwork.GetProfile(nodePtr[fNodeInd]->fProfileIndex);
        endOffset = profile->GetRawLaneOffset(static_cast< int >(laneInd));

        float laneOffset = (endOffset - startOffset) * fSegTime + startOffset;
        SetLaneOffset(laneOffset);

        SetStartEndPos(*segment, startOffset, endOffset);
    }

    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
    ResetCookieTrail();
}

void WRoadNav::InitAtSegment(short segInd, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane) {
    WRoadNetwork &rn = WRoadNetwork::Get();
    const WRoadSegment *segment = rn.GetSegment(segInd);
    float timeStep = rn.GetSegmentPointIntersect(*segment, pos, fPosition, true);
    if (segment->IsCurved()) {
        rn.GetPointOnSegment(*segment, timeStep, fPosition);
        FindClosestOnSpline(pos, fPosition, timeStep, 1.0f, static_cast<int>(segment->fIndex));
        FindClosestOnSpline(pos, fPosition, timeStep, 0.25f, static_cast<int>(segment->fIndex));
    }
    InitAtSegment(segInd, timeStep, pos, dir, forceCenterLane);
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
    float current_offset = UMath::Cross(nav_forward_2d, nav_to_vehicle_2d);
    SetStartEndPos(*segment, current_offset);
    SetLaneOffset(current_offset);

    if (fNavType != kTypeTraffic) return;

    {
        bool forward = GetNodeInd() == 1;
        int start = forward ? 0 : 1;
        int end = forward ? 1 : 0;
        bool end_inverted = segment->IsProfileInverted(end);
        bool start_inverted = segment->IsProfileInverted(start);
        const WRoadProfile *end_profile = roadNetwork.GetSegmentProfile(*segment, end);
        const WRoadProfile *start_profile = roadNetwork.GetSegmentProfile(*segment, start);
        int num_start_traffic_lanes = start_profile->GetNumTrafficLanes(forward, start_inverted);
        int num_end_traffic_lanes = end_profile->GetNumTrafficLanes(forward, end_inverted);
        int num_traffic_lanes = bMin(num_start_traffic_lanes, num_end_traffic_lanes);
        if (num_traffic_lanes > 0) {
            bool foundClosest = false;
            float closestDist;
            float startOffset;
            float endOffset;
            float current_offset = GetLaneOffset();
            for (int i = 0; i < num_traffic_lanes; i++) {
                int end_lane = start_profile->GetNthTrafficLaneFromCurb(i, forward, start_inverted);
                int start_lane = end_profile->GetNthTrafficLaneFromCurb(i, forward, end_inverted);
                float parameter = GetSegmentTime();
                float end_offset = start_profile->GetLaneOffset(end_lane, start_inverted);
                float start_offset = end_profile->GetLaneOffset(start_lane, end_inverted);
                float lane_offset = UMath::Lerp(start_offset, end_offset, parameter);
                float distance = bAbs(current_offset - lane_offset);
                if (!foundClosest || distance < closestDist) {
                    SetLaneOffset(lane_offset);
                    SetLaneInd(static_cast< char >(end_lane));
                    foundClosest = true;
                    startOffset = end_offset;
                    closestDist = distance;
                    endOffset = start_offset;
                }
            }
            SetStartEndPos(*segment, endOffset, startOffset);
        }
    }

    SetStartEndControls(*segment);
    RebuildSplines(segment);
    EvaluateSplines(segment);
}

void WRoadNav::InitAtSegment(short segInd, float timeStep, const UMath::Vector3 &pos, const UMath::Vector3 &dir, bool forceCenterLane) {
    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    const WRoadSegment *segment = roadNetwork.GetSegment(segInd);
    UMath::Vector3 segmentForwardVector;

    fDeadEnd = 0;
    fValid = true;
    fSegmentInd = segInd;

    roadNetwork.GetSegmentForwardVector(*segment, segmentForwardVector);
    float facingDot = UMath::Dot(dir, segmentForwardVector);
    bool backward;
    if (bRaceFilter) {
        backward = !segment->RaceRouteForward();
    } else {
        backward = facingDot < 0.0f;
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

int WRoadNav::FindClosestOnPath(const UMath::Vector3 &position, UMath::Vector3 *found_position, UMath::Vector3 *found_direction, unsigned short *found_segment, float *found_interval) const {
    if (pPathSegments != nullptr && nPathSegments > 0) {
        float min_dist_sq = 0.0f;
        int found_segment_index = -1;
        float best_interval = -1.0f;
        UMath::Vector3 best_position;
        WRoadNetwork &roadNetwork = WRoadNetwork::Get();
        for (int i = 0; i < nPathSegments; i++) {
            const WRoadSegment *segment = roadNetwork.GetSegment(pPathSegments[i]);
            UMath::Vector3 intersect;
            float d = roadNetwork.GetSegmentPointIntersect(*segment, position, intersect, true);
            float dist_square = UMath::DistanceSquare(intersect, position);
            if (min_dist_sq > 0.0f && dist_square >= min_dist_sq) continue;
            best_position = intersect;
            found_segment_index = i;
            best_interval = d;
            min_dist_sq = dist_square;
        }
        if (found_segment_index > -1) {
            unsigned short segment = pPathSegments[found_segment_index];
            const WRoadNode *this_nodes[2];
            const WRoadSegment *this_segment = roadNetwork.GetSegment(segment);
            int tail;
            unsigned short next_segment;
            unsigned short prev_segment;
            USpline roadSpline;
            UMath::Vector4 tangent;

            roadNetwork.GetSegmentNodes(*this_segment, this_nodes);
            if (found_segment_index < nPathSegments - 1) {
                next_segment = pPathSegments[found_segment_index + 1];
            } else {
                next_segment = static_cast< unsigned short >(-1);
            }
            if (found_segment_index >= 1) {
                prev_segment = pPathSegments[found_segment_index - 1];
            } else {
                prev_segment = static_cast< unsigned short >(-1);
            }

            roadNetwork.BuildSegmentSpline(*this_segment, roadSpline);
            roadSpline.EvaluateTangent(best_interval, tangent);

            if (found_direction != nullptr) {
                *found_direction = UMath::Vector4To3(tangent);
                if (!this_nodes[0]->IsSegment(next_segment)) {
                    if (this_nodes[1]->IsSegment(prev_segment)) {
                        UMath::Negate(*found_direction);
                    }
                }
            }

            if (found_segment != nullptr) {
                *found_segment = segment;
            }
            if (found_position != nullptr) {
                *found_position = best_position;
            }
            if (found_interval != nullptr) {
                *found_interval = best_interval;
            }
            return 1;
        }
    }
    return 0;
}

void WRoadNav::SetControlPos(const WRoadSegment &segment, bool startControl) {
    if (!(segment.fFlags & 0x100)) return;

    {
        bool forward = fNodeInd != 0;
        bool which_end;
        if (fNodeInd == 0) {
            which_end = startControl;
        } else {
            which_end = !startControl;
        }
        WRoadNetwork &road_network = WRoadNetwork::Get();
        const UMath::Vector3 &nodePos = road_network.GetNode(segment.fNodeIndex[which_end])->fPosition;
        const UMath::Vector3 &otherPos = road_network.GetNode(segment.fNodeIndex[which_end ^ 1])->fPosition;
        UMath::Vector3 handle;
        UMath::Vector3 controlPos;

        segment.GetControl(which_end, handle);
        controlPos = UVector3(nodePos) + UVector3(handle);

        float original_distance = UMath::Distance(nodePos, otherPos);
        original_distance = UMath::Max(original_distance, 0.001f);
        float new_distance = UMath::Distance(fStartPos, fEndPos);
        new_distance = UMath::Max(new_distance, 0.001f);
        float scale = new_distance / original_distance;

        const UMath::Vector3 &nodeRef = startControl ? fEndPos : fStartPos;
        UMath::Vector3 &controlRef = startControl ? fEndControl : fStartControl;
        UMath::ScaleAdd(handle, scale, nodeRef, controlRef);

        if (bCookieTrail) {
            float left_scale = UMath::Distance(fLeftStartPos, fLeftEndPos);
            left_scale = UMath::Max(left_scale, 0.001f);
            float right_scale = UMath::Distance(fRightStartPos, fRightEndPos);
            right_scale = UMath::Max(right_scale, 0.001f);

            const UMath::Vector3 &leftNodeRef = startControl ? fLeftEndPos : fLeftStartPos;
            UMath::Vector3 &leftControlRef = startControl ? fLeftEndControl : fLeftStartControl;
            UMath::ScaleAdd(handle, left_scale / original_distance, leftNodeRef, leftControlRef);

            const UMath::Vector3 &rightNodeRef = startControl ? fRightEndPos : fRightStartPos;
            UMath::Vector3 &rightControlRef = startControl ? fRightEndControl : fRightStartControl;
            UMath::ScaleAdd(handle, right_scale / original_distance, rightNodeRef, rightControlRef);
        }
    }
}

void WRoadNav::SetBoundPos(const WRoadSegment &segment, float offset, bool start) {
    bool forward = fNodeInd != 0;
    bool which_end;
    if (fNodeInd == 0) {
        which_end = start;
    } else {
        which_end = !start;
    }
    WRoadNetwork &road_network = WRoadNetwork::Get();
    const WRoadNode *node = road_network.GetNode(segment.fNodeIndex[which_end]);
    const UMath::Vector3 &nodePos = node->fPosition;
    UMath::Vector3 rightVec;
    float sign = forward ? 1.0f : -1.0f;

    segment.GetRightVec(which_end, rightVec);

    if (bCookieTrail) {
        float vehicle_half_width = fVehicleHalfWidth;
        float left_offset = offset + vehicle_half_width * 0.5f;
        float right_offset = offset - vehicle_half_width * 0.5f;
        int nav_type = GetNavType();

        if (nav_type != kTypeTraffic) {
            left_offset = offset + 2.0f;
            right_offset = offset - 2.0f;

            const WRoadProfile *profile = road_network.GetSegmentProfile(segment, which_end);
            int num_lanes = profile->fNumZones;

            {
                int closest_drivable = -1;
                float closest_offset = 0.0f;
                bool inverted = !forward != segment.IsProfileInverted(fNodeInd);
                int middle_lane = profile->GetMiddleZone(inverted);

                {
                    int lane_index;
                    for (lane_index = 0; lane_index < num_lanes; lane_index++) {
                        int lane_type = profile->GetLaneType(lane_index, inverted);
                        if (IsDrivable(lane_type)) {
                            float lane_offset = profile->GetLaneOffset(lane_index, inverted);
                            float lane_distance = bAbs(offset - lane_offset);
                            if (closest_drivable < 0 || lane_distance < closest_offset) {
                                closest_drivable = lane_index;
                                closest_offset = lane_distance;
                            }
                        }
                    }
                }

                {
                    int left_lane = closest_drivable;
                    int right_lane = closest_drivable;
                    float left_lane_offset;
                    float right_lane_offset;
                    float safety_margin;
                    float how_unsafe;

                    while (left_lane > 0) {
                        int prev = left_lane - 1;
                        int lt = profile->GetLaneType(prev, inverted);
                        if (!IsDrivable(lt)) break;
                        left_lane = prev;
                    }

                    while (right_lane < num_lanes - 1) {
                        int next = right_lane + 1;
                        int lt = profile->GetLaneType(next, inverted);
                        if (!IsDrivable(lt)) break;
                        right_lane = next;
                    }

                    left_lane_offset = profile->GetRelativeLaneOffset(left_lane, inverted);
                    right_lane_offset = profile->GetRelativeLaneOffset(right_lane, inverted);

                    float left_width = profile->GetLaneWidth(left_lane, inverted);
                    float right_width = profile->GetLaneWidth(right_lane, inverted);

                    safety_margin = bMax(vehicle_half_width + vehicle_half_width, 0.0f);

                    how_unsafe = (vehicle_half_width + vehicle_half_width) - (right_lane_offset - left_lane_offset);
                    if (how_unsafe < 0.0f) {
                        how_unsafe = 0.0f;
                    }

                    left_offset = left_lane_offset - how_unsafe * 0.5f;
                    right_offset = right_lane_offset + how_unsafe * 0.5f;
                }

                offset = bClamp(offset, left_offset, right_offset);
            }
        }

        UMath::Vector3 &leftRef = start ? fLeftStartPos : fLeftEndPos;
        UMath::Vector3 &rightRef = start ? fRightStartPos : fRightEndPos;

        UMath::ScaleAdd(rightVec, sign * left_offset, nodePos, leftRef);
        UMath::ScaleAdd(rightVec, sign * right_offset, nodePos, rightRef);
    }

    UMath::Vector3 &posRef = start ? fStartPos : fEndPos;
    UMath::ScaleAdd(rightVec, sign * offset, nodePos, posRef);
}

bool WRoadNav::ChangeDragDecision(int left_right) {
    WRoadNetwork &rn = WRoadNetwork::Get();
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = rn.GetSegment(segment_number);

    if (!segment->IsDecision()) return false;

    {
        UMath::Vector2 ray;
        const WRoadNode *nodes[2];
        rn.GetSegmentNodes(*segment, nodes);
        int from_which_node = GetNodeInd() ^ 1;
        const WRoadNode *from_node = nodes[from_which_node];
        int new_which_node = 0;
        float best = 0.7f;
        const WRoadSegment *new_segment = nullptr;
        float sign = -1.0f;
        int num_segments;

        segment->GetForwardVec(from_which_node, ray);
        if (from_which_node == 1) {
            UMath::Scale(ray, -1.0f);
        }
        UMath::Normalize(ray);

        if (left_right < 0) {
            sign = 1.0f;
        }

        num_segments = from_node->fNumSegments;
        for (int i = 0; i < num_segments; i++) {
            unsigned short departing_segment_number = from_node->fSegmentIndex[i];
            if (departing_segment_number != segment_number) {
                const WRoadSegment *departing_segment = rn.GetSegment(departing_segment_number);
                if (departing_segment->IsDecision() && departing_segment->IsInRace()) {
                    int to_which_node = from_node->fIndex != static_cast< int >(departing_segment->fNodeIndex[0]);

                    UMath::Vector2 departing_ray;
                    departing_segment->GetForwardVec(to_which_node, departing_ray);
                    if (to_which_node) {
                        UMath::Scale(departing_ray, -1.0f);
                    }
                    UMath::Normalize(departing_ray);

                    float cross_val = sign * Cross(ray, departing_ray);
                    if (cross_val >= 0.0f) {
                        float dot = UMath::Dot(ray, departing_ray);
                        if (best < dot) {
                            new_segment = departing_segment;
                            best = dot;
                            new_which_node = to_which_node;
                        }
                    }
                }
            }
        }

        if (new_segment != nullptr) {
            int new_segment_index = new_segment->fIndex;
            bool inverted = new_segment->IsProfileInverted(new_which_node) != static_cast< bool >(new_which_node);
            const WRoadProfile *new_profile = rn.GetSegmentProfile(*new_segment, new_which_node);
            int num_lanes = new_profile->fNumZones;
            int end;
            int start;
            int new_lane;
            float new_lane_offset;
            UMath::Vector3 new_spline_point;

            if (left_right >= 0) {
                end = num_lanes;
            } else {
                end = -1;
            }

            if (left_right >= 0) {
                start = 0;
            } else {
                start = num_lanes - 1;
            }

            new_lane = new_profile->GetMiddleZone(inverted);

            for (int lane = start; lane != end; lane += left_right) {
                int actual_lane = lane;
                if (!inverted) {
                    actual_lane = (new_profile->fNumZones - lane) - 1;
                }
                int lane_type = new_profile->GetLaneType(actual_lane, inverted);
                if (IsDrivable(lane_type)) {
                    new_lane = lane;
                    break;
                }
            }

            new_lane_offset = new_profile->GetRelativeLaneOffset(new_lane, inverted);
            SetLaneOffset(new_lane_offset);

            fNodeInd = new_which_node;
            fSegmentInd = new_segment_index;
            SetControlPos(*new_segment, false);
            SetBoundPos(*new_segment, new_lane_offset, false);
            RebuildSplines(new_segment);
            FindClosestOnSpline(fPosition, new_spline_point, fSegTime, 1.0f, new_segment_index);
            EvaluateSplines(new_segment);
            return true;
        }
    }
    return false;
}

void WRoadNav::ChangeDragLanes(int left_right) {
    WRoadNetwork &rn = WRoadNetwork::Get();
    int segment_number = GetSegmentInd();
    const WRoadSegment *segment = rn.GetSegment(segment_number);
    char node_ind = GetNodeInd();
    const WRoadProfile *profile = rn.GetSegmentProfile(*segment, node_ind);

    bool backward;
    if (node_ind != 0) {
        backward = segment->IsStartInverted();
    } else {
        backward = segment->IsEndInverted();
    }

    float current_offset = fLaneOffset;

    if (left_right == 0) {
        AIVehicle *vehicle = pAIVehicle;
        ISimable *sim = nullptr;
        if (vehicle != nullptr) {
            sim = vehicle->GetSimable();
        }
        IRigidBody *rb = nullptr;
        if (sim != nullptr) {
            rb = sim->GetRigidBody();
        }
        if (rb != nullptr) {
            WRoadNav temp;
            temp.bRaceFilter = WRoadNetwork::Get().IsRaceFilterValid();
            const UMath::Vector3 &rbPos = rb->GetPosition();
            temp.InitAtPoint(rbPos, fForwardVector, false, 1.0f);
            if (temp.fValid) {
                current_offset = temp.fLaneOffset;
            }
        }
    }

    bool inverted = (backward != 0) == (node_ind != 0);
    inverted = !inverted;

    // Find closest selectable lane
    int current_lane;
    if (inverted) {
        current_lane = profile->GetMiddleZone(inverted);
    } else {
        current_lane = profile->fNumZones - profile->GetMiddleZone(inverted);
    }

    // Find initial selectable lane
    while (true) {
        int actual_lane = current_lane;
        if (inverted) {
            actual_lane = (profile->fNumZones - current_lane) - 1;
        }
        int lane_type = profile->GetLaneType(actual_lane, inverted);
        if (IsSelectable(lane_type)) break;
        current_lane++;
    }

    // Get offset for initial lane
    int lane_for_offset = current_lane;
    int middle;
    if (inverted) {
        middle = profile->fNumZones - profile->GetMiddleZone(inverted);
        lane_for_offset = (profile->fNumZones - current_lane) - 1;
    } else {
        middle = profile->GetMiddleZone(inverted);
    }
    float initial_offset = static_cast< float >(profile->GetLaneOffset(lane_for_offset, inverted)) * 0.012208521f;
    if (static_cast< int >(current_lane) < static_cast< int >(middle)) {
        initial_offset = -initial_offset;
    }

    float offset_difference = bAbs(initial_offset - current_offset);
    int num_lanes = profile->fNumZones;

    // Find closest selectable lane to current offset
    for (int i = 0; i < num_lanes; i++) {
        int actual_i = i;
        if (inverted) {
            actual_i = (profile->fNumZones - i) - 1;
        }
        int lane_type = profile->GetLaneType(actual_i, inverted);
        if (IsSelectable(lane_type)) {
            int middle2;
            int lane_idx;
            if (inverted) {
                middle2 = profile->fNumZones - profile->GetMiddleZone(inverted);
                lane_idx = (profile->fNumZones - i) - 1;
            } else {
                middle2 = profile->GetMiddleZone(inverted);
                lane_idx = i;
            }
            float this_offset = static_cast< float >(profile->GetLaneOffset(lane_idx, inverted)) * 0.012208521f;
            if (static_cast< int >(i) < static_cast< int >(middle2)) {
                this_offset = -this_offset;
            }
            float diff = this_offset - current_offset;
            float clamped = bClamp(diff, -offset_difference, offset_difference);
            if (diff == clamped) {
                offset_difference = bAbs(diff);
                current_lane = i;
            }
        }
    }

    int target_lane = current_lane;
    if (left_right != 0) {
        int test_lane = current_lane;
        while (true) {
            test_lane += left_right;
            target_lane = current_lane;
            if (test_lane >= num_lanes || test_lane < 0) break;

            int actual_test = test_lane;
            if (inverted) {
                actual_test = (profile->fNumZones - test_lane) - 1;
            }
            int lt = profile->GetLaneType(actual_test, inverted);
            target_lane = current_lane;
            if (!IsDrivable(lt) || IsSelectable(lt)) {
                target_lane = test_lane;
                break;
            }
        }

        if (target_lane == current_lane) {
            if (ChangeDragDecision(left_right)) return;
        }
    }

    // Compute final offset
    int final_lane;
    int final_middle;
    if (inverted) {
        final_middle = profile->fNumZones - profile->GetMiddleZone(inverted);
        final_lane = (profile->fNumZones - target_lane) - 1;
    } else {
        final_middle = profile->GetMiddleZone(inverted);
        final_lane = target_lane;
    }
    float final_offset = static_cast< float >(profile->GetLaneOffset(final_lane, inverted)) * 0.012208521f;
    if (static_cast< int >(target_lane) < static_cast< int >(final_middle)) {
        final_offset = -final_offset;
    }
    ChangeLanes(final_offset, 0.0f);
}

bool WRoadNav::MakeShortcutDecision(int shortcut_number, unsigned int *cached, unsigned int *allowed) {
    if (shortcut_number == 0xff) return true;

    if (GetPathType() == kPathPlayer) {
        return shortcut_number == GetShortcutNumber();
    }
    if (GetPathType() != kPathRacer) return true;

    int mask = 1 << shortcut_number;
    if (cached != nullptr && (mask & *cached) != 0) {
        bool shortcut_allowed = (mask & *allowed) != 0;
        return shortcut_allowed;
    }
    GRaceParameters *race_parameters = GRaceStatus::Get().GetRaceParameters();
    if (race_parameters != nullptr) {
        AIVehicle *vehicle = GetVehicle();
        float skill;
        if (vehicle != nullptr) {
            skill = vehicle->GetSkill();
        } else {
            skill = 0.0f;
        }
        GMarker *marker = race_parameters->GetShortcut(shortcut_number);
        float min = marker->ShortcutMinChance(0);
        float max = marker->ShortcutMaxChance(0);
        if (min > 1.0f) {
            min = min * 0.01f;
        }
        if (max > 1.0f) {
            max = max * 0.01f;
        }
        float chance = skill * (max - min) + min;
        bool shortcut_allowed = bRandom(1.0f) < chance;
        if (shortcut_allowed && allowed != nullptr) {
            *allowed = *allowed | mask;
        }
        if (cached != nullptr) {
            *cached = *cached | mask;
        }
        return shortcut_allowed;
    }
    return true;
}

short WRoadNav::GetNextTraffic(const UMath::Vector3 &toVec, float &nextLaneOffset, char &nodeInd, bool &useOldStartPos) {
    struct Candidate {
        int Lane;
        int WhichNode;
        int SegmentNumber;
        bool LastResort;
    };

    WRoadNetwork &roadNetwork = WRoadNetwork::Get();
    int which_node = static_cast< int >(GetNodeInd());
    short oldSegInd = GetSegmentInd();
    const WRoadSegment *segment = roadNetwork.GetSegment(oldSegInd);
    const WRoadProfile *profile = roadNetwork.GetSegmentProfile(*segment, which_node);
    bool forward = (which_node == 1);
    bool inverted = segment->IsProfileInverted(which_node);
    char newLaneInd = GetLaneInd();
    int nth_lane = 0;
    int num_traffic_lanes = 0;

    int num_lanes = profile->GetNumLanes(forward, inverted);
    for (int i = 0; i < num_lanes; i++) {
        int real_lane = profile->GetNthLane(i, forward, inverted);
        if (profile->GetLaneType(real_lane, inverted) == 1) {
            if (newLaneInd == real_lane) {
                nth_lane = num_traffic_lanes;
            }
            num_traffic_lanes++;
        }
    }

    const WRoadNode *node = roadNetwork.GetNode(segment->fNodeIndex[which_node]);
    short newSegInd = GetSegmentInd();
    int current_lane = static_cast< int >(GetLaneInd());
    const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(node, oldSegInd);

    if (node->fNumSegments < 2) {
        newSegInd = segment->fIndex;
    } else if (checkSegment != nullptr) {
        newSegInd = checkSegment->fIndex;
        bool new_forward = (node == roadNetwork.GetNode(checkSegment->fNodeIndex[0]));
        nodeInd = new_forward;
        bool new_inverted = checkSegment->IsProfileInverted(static_cast< int >(nodeInd));
        const WRoadProfile *new_profile = roadNetwork.GetSegmentProfile(*checkSegment, static_cast< int >(nodeInd));
        current_lane = new_profile->GetNthTrafficLane(nth_lane, new_forward, new_inverted);
    } else {
        float lenSq = UMath::LengthSquare(toVec);
        if (lenSq > 0.01f) {
            float bestDot = -1.0f;
            float check_segment_number = bestDot;
            for (int onSeg = 0; onSeg < static_cast< int >(node->fNumSegments); onSeg++) {
                const WRoadSegment *intersectionSegment = roadNetwork.GetSegment(node->fSegmentIndex[onSeg]);
                if (intersectionSegment->fIndex == GetSegmentInd()) continue;
                if (!intersectionSegment->IsTrafficAllowed()) continue;

                const WRoadNode *oppNode = roadNetwork.GetSegmentOppNode(*intersectionSegment, node);
                const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(oppNode, -1);
                if (checkSegment == nullptr) continue;
                if (!checkSegment->IsTrafficAllowed()) continue;

                bool reverse = (oppNode != roadNetwork.GetNode(checkSegment->fNodeIndex[0]));
                UMath::Vector3 vec;
                roadNetwork.GetSegmentForwardVector(*checkSegment, vec);
                if (reverse) {
                    vec = UMath::Vector3Make(-vec.x, -vec.y, -vec.z);
                }
                UMath::Unit(vec, vec);
                float dot = UMath::Dot(vec, toVec);

                if (dot >= bestDot) {
                    newSegInd = node->fSegmentIndex[onSeg];
                    const WRoadSegment *newSegment = roadNetwork.GetSegment(newSegInd);
                    nodeInd = (node == roadNetwork.GetNode(newSegment->fNodeIndex[0]));
                    check_segment_number = static_cast< float >(checkSegment->fIndex);
                    bestDot = dot;
                }
            }

            if (check_segment_number != -1.0f) {
                const WRoadSegment *newSegment = roadNetwork.GetSegment(newSegInd);
                bool new_forward = (nodeInd == 1);
                const WRoadProfile *new_profile = roadNetwork.GetSegmentProfile(*newSegment, static_cast< int >(nodeInd));
                bool new_inverted = newSegment->IsProfileInverted(static_cast< int >(nodeInd));

                int rightmost = roadNetwork.GetRightMostTrafficEntrance(
                    newSegment->fNodeIndex[static_cast< int >(nodeInd)],
                    static_cast< int >(check_segment_number));

                if (newSegInd == rightmost) {
                    int nth_from_curb = num_traffic_lanes - nth_lane - 1;
                    current_lane = new_profile->GetNthTrafficLaneFromCurb(nth_from_curb, new_forward, new_inverted);
                } else {
                    current_lane = new_profile->GetNthTrafficLane(nth_lane, new_forward, new_inverted);
                }
            }
        } else {
            int num_candidates = 0;
            Candidate candidates[7];

            for (int i = 0; i < static_cast< int >(node->fNumSegments); i++) {
                int new_segment_number = node->fSegmentIndex[i];
                if (new_segment_number == GetSegmentInd()) continue;

                const WRoadSegment *decision_segment = roadNetwork.GetSegment(new_segment_number);
                if (!decision_segment->IsTrafficAllowed()) continue;

                const WRoadNode *nodePtr[2];
                roadNetwork.GetSegmentNodes(*decision_segment, nodePtr);
                const WRoadNode *oppNode = nodePtr[0];
                if (node == nodePtr[0]) {
                    oppNode = nodePtr[1];
                }

                const WRoadSegment *checkSegment = GetAttachedDirectionalSegment(oppNode, -1);
                if (checkSegment == nullptr) continue;
                if (!checkSegment->IsTrafficAllowed()) continue;
                if (oppNode != roadNetwork.GetNode(checkSegment->fNodeIndex[0]) && checkSegment->IsOneWay()) continue;

                int new_which_node = (node->fIndex != decision_segment->fNodeIndex[1]);
                bool new_forward = new_which_node;
                bool new_inverted = decision_segment->IsProfileInverted(new_which_node);
                const WRoadProfile *new_profile = roadNetwork.GetSegmentProfile(*decision_segment, new_which_node);
                int num_traffic_lanes = new_profile->GetNumTrafficLanes(new_forward, new_inverted);

                if (num_traffic_lanes == 0) continue;

                int rightmost_traffic_entrance = roadNetwork.GetRightMostTrafficEntrance(
                    oppNode->fIndex, checkSegment->fIndex);

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
                nodeInd = static_cast< char >(candidates[selection].WhichNode);
                newSegInd = static_cast< short >(candidates[selection].SegmentNumber);
                current_lane = candidates[selection].Lane;
            }
        }
    }

    if (newSegInd != GetSegmentInd()) {
        if (current_lane < 0) {
            current_lane = 0;
        }
        SetLaneInd(static_cast< char >(current_lane));
        fToLaneInd = static_cast< char >(current_lane);

        const WRoadNode *oppNode = roadNetwork.GetSegmentOppNode(*roadNetwork.GetSegment(newSegInd), node);
        nextLaneOffset = roadNetwork.GetProfile(oppNode->fProfileIndex)->GetLaneOffset(current_lane, false);
    }

    useOldStartPos = true;
    return newSegInd;
}
