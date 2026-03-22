#include "ResetCar.h"

#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

Behavior *ResetCar::Construct(const BehaviorParams &params) {
    return new ResetCar(params);
}

bool ResetCar::OnTask(HSIMTASK htask, float dT) {
    if (htask == mCheckTask) {
        if (!IsPaused()) {
            Check(dT);
        }
        return true;
    }
    return Object::OnTask(htask, dT);
}

bool ResetCar::FindNearestRoad(ResetCookie *cookie) const {
    IVehicleAI *iai = nullptr;
    if (GetOwner()->QueryInterface(&iai)) {
        WRoadNav nav;
        UMath::Vector3 drive_dir;

        nav.SetNavType(WRoadNav::kTypeDirection);
        nav.SetLaneType(WRoadNav::kLaneReset);

        bool actively_racing = false;
        if (GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing()) {
            actively_racing = true;
        }
        nav.SetRaceFilter(actively_racing);
        nav.SetDecisionFilter(true);
        drive_dir = cookie->direction;
        nav.InitAtPoint(cookie->position, drive_dir, false, 1.0f);
        nav.SnapToSelectableLane();
        if (nav.IsValid()) {
            float elevation_difference = bClamp(nav.GetPosition().y - cookie->position.y, -6.0f, 6.0f);
            if (nav.GetPosition().y - cookie->position.y == elevation_difference) {
                bool crosses_barrier = false;
                bVector2 a(cookie->position.z, -cookie->position.x);
                bVector2 b(nav.GetPosition().z, -nav.GetPosition().x);
                int num_barriers = TheTrackPathManager.GetNumBarriers();

                for (int barrier_number = 0; barrier_number < num_barriers; barrier_number++) {
                    TrackPathBarrier *barrier = TheTrackPathManager.GetBarrier(barrier_number);
                    if (barrier->IsEnabled() && barrier->Intersects(&a, &b)) {
                        crosses_barrier = true;
                        break;
                    }
                }

                if (!crosses_barrier) {
                    float worldHeight;
                    if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(nav.GetPosition(), worldHeight, nullptr)) {
                        if (UMath::Abs(cookie->position.y - worldHeight) < 2.5f) {
                            cookie->position = nav.GetPosition();
                            drive_dir = nav.GetForwardVector();
                            UMath::Unit(drive_dir, cookie->direction);

                            const WRoadSegment *seg = nav.GetSegment();
                            if (seg && seg->IsInRace()) {
                                char raceind = seg->RaceRouteForward();
                                if (nav.GetNodeInd() != raceind) {
                                    UMath::Negate(cookie->direction);
                                }
                            }

                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}
