#include "ResetCar.h"

#include "Speed/Indep/Src/Generated/Events/EVehicleReset.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Sim/SimSurface.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

bool DoLinesIntersect(const bVector2 &a, const bVector2 &b, const bVector2 &c, const bVector2 &d);

struct TrackPathBarrier {
    bVector2 Points[2];
    char Enabled;
    char Pad;
    char PlayerBarrier;
    char LeftHanded;
    unsigned int GroupHash;
};

static inline int GetNumBarriers() {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(&TheTrackPathManager) + 0x484);
}

static inline TrackPathBarrier *GetBarrier(int n) {
    TrackPathBarrier **barriers = reinterpret_cast<TrackPathBarrier **>(reinterpret_cast<char *>(&TheTrackPathManager) + 0x488);
    return barriers[0] + n;
}

static inline bool IsBarrierEnabled(TrackPathBarrier *barrier) {
    return barrier->Enabled != 0;
}

static inline bool BarrierIntersects(TrackPathBarrier *barrier, const bVector2 *pointa, const bVector2 *pointb) {
    return DoLinesIntersect(barrier->Points[0], barrier->Points[1], *pointa, *pointb);
}

static inline char GetNodeInd(const WRoadNav &nav) {
    return *reinterpret_cast<const char *>(reinterpret_cast<const char *>(&nav) + 0x98);
}

static inline bool RaceRouteForward(const WRoadSegment &seg) {
    return seg.fFlags & (1 << 2);
}

Behavior *ResetCar::Construct(const BehaviorParams &params) {
    return new ResetCar(params);
}

ResetCar::ResetCar(const BehaviorParams &params)
    : VehicleBehavior(params, 0), //
      IResetable(params.fowner),  //
      mFlippedOver(0.0f),         //
      mCookies() {
    mCheckTask = AddTask("Physics", 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(mCheckTask, "ResetCar");

    GetOwner()->QueryInterface(&mCollisionBody);
    GetOwner()->QueryInterface(&mSuspension);
    GetOwner()->QueryInterface(&mVehicleBody);
}

ResetCar::~ResetCar() {
    RemoveTask(mCheckTask);

    if (mVehicleBody) {
        mVehicleBody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
    }
}

void ResetCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        GetOwner()->QueryInterface(&mSuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        GetOwner()->QueryInterface(&mCollisionBody);
        GetOwner()->QueryInterface(&mVehicleBody);
    }
    Behavior::OnBehaviorChange(mechanic);
}

bool ResetCar::ValidTerrain(unsigned int checktires) const {
    if (mSuspension) {
        SimSurface owner_surface(GetOwner()->GetWPos().GetSurface());

        if (owner_surface == SimSurface::kNull) {
            unsigned int invalid_tires = 0;

            for (unsigned int i = 0; i < mSuspension->GetNumWheels(); i++) {
                if (mSuspension->GetWheelRoadSurface(i) == SimSurface::kNull) {
                    invalid_tires++;
                }
            }

            if (!(invalid_tires < checktires)) {
                return false;
            }
        }
    }

    return true;
}

bool ResetCar::CanRecord() const {
    if (mCookies.Count() != 0) {
        const ResetCookie &cookie = mCookies.Newest();
        if (UMath::DistanceSquare(cookie.position, GetOwner()->GetPosition()) < 4.0f) {
            return false;
        }
    }

    if (!ValidTerrain(0)) {
        return false;
    }

    if (mSuspension) {
        if (mSuspension->GetNumWheelsOnGround() != mSuspension->GetNumWheels()) {
            return false;
        }
    }

    return true;
}

bool ResetCar::ShouldReset() const {
    bool should_reset = false;

    if (mCookies.Count() != 0) {
        if (!GetVehicle()->IsLoading() && !ValidTerrain(2)) {
            return true;
        }
        should_reset = 4.0f < mFlippedOver;
    }

    return should_reset;
}

void ResetCar::TrackState(float dT) {
    bool flippedover = false;

    if (mSuspension) {
        if (mSuspension->GetNumWheelsOnGround() != mSuspension->GetNumWheels() && mCollisionBody && mCollisionBody->IsModeling()) {
            {
                const UMath::Vector3 &vup = mCollisionBody->GetUpVector();
                float ground_dot = UMath::Dot(vup, UMath::Vector4To3(mCollisionBody->GetGroundNormal()));

                if (ground_dot < 0.5f) {
                    flippedover = vup.y < 0.5f;
                }
            }
        }
    }

    if (flippedover) {
        mFlippedOver += dT;
    } else {
        mFlippedOver = 0.0f;
    }
}

bool ResetCar::CheckZone(eTrackPathZoneType type) {
    bVector2 position2d(mCollisionBody->GetPosition().z, -mCollisionBody->GetPosition().x);
    TrackPathZone *zone = TheTrackPathManager.FindZone(&position2d, type, nullptr);

    if (zone) {
        bVector2 *zone_position = zone->GetPosition();
        bVector2 *zone_direction = zone->GetDirection();
        UMath::Vector3 reset_position = UMath::Vector3Make(-zone_position->y, 0.0f, zone_position->x);
        UMath::Vector3 reset_dir = UMath::Vector3Make(-zone_direction->y, 0.0f, zone_direction->x);

        if (1e-06f < UMath::Normalize(reset_dir)) {
            return ResetTo(reset_position, reset_dir, false);
        }
    }

    return false;
}

void ResetCar::Check(float dT) {
    {
        ResetCookie cookie;
        UMath::Matrix4 mat;

        TrackState(dT);
        if (!CheckZone(TRACK_PATH_ZONE_RESET_TO_POINT)) {
            if (CanRecord()) {
                GetOwner()->GetTransform(mat);
                cookie.time = Sim::GetTime();
                cookie.position = UMath::Vector4To3(mat.v3);
                cookie.direction = UMath::Vector4To3(mat.v2);
                cookie.flags = 1;
                mCookies.AddNew(cookie);
            } else if (ShouldReset()) {
                static_cast<IResetable *>(this)->ResetVehicle(false);
            }
        }
    }
}

void ResetCar::Reset() {
    if (mVehicleBody) {
        mVehicleBody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
    }
    mCookies.Clear();
    mFlippedOver = 0.0f;
}

bool ResetCar::HasResetPosition() {
    return mCookies.Count() != 0;
}

inline void ResetCar::SetResetPosition(const UMath::Vector3 &position, const UMath::Vector3 &direction) {
    ResetCookie cookie;

    mCookies.Clear();
    cookie.position = position;
    cookie.direction = direction;
    cookie.time = Sim::GetTime();
    cookie.flags = 0;
    mCookies.AddNew(cookie);
}

void ResetCar::ClearResetPosition() {
    mCookies.Clear();
}

bool ResetCar::ResetVehicle(bool manual) {
    ResetCookie cookie;

    if (manual && CheckZone(TRACK_PATH_ZONE_GUIDED_RESET)) {
        return true;
    }

    if (HasResetPosition() && (!manual || !mVehicleBody || !mVehicleBody->GetInvulnerability())) {
        cookie = mCookies.Oldest();
        if (cookie.flags & 1) {
            FindNearestRoad(&cookie);
        }
        if (ResetTo(cookie.position, cookie.direction, manual)) {
            mCookies.Clear();
            mCookies.AddNew(cookie);
            return true;
        }
    }

    return false;
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
                int num_barriers = GetNumBarriers();

                for (int barrier_number = 0; barrier_number < num_barriers; barrier_number++) {
                    TrackPathBarrier *barrier = GetBarrier(barrier_number);
                    if (IsBarrierEnabled(barrier) && BarrierIntersects(barrier, &a, &b)) {
                        crosses_barrier = true;
                        break;
                    }
                }

                if (!crosses_barrier) {
                    float worldHeight;
                    if (WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(nav.GetPosition(), worldHeight, nullptr)) {
                        if (UMath::Abs(cookie->position.y - worldHeight) < 2.5f) {
                            cookie->position = nav.GetPosition();
                            UMath::Unit(nav.GetForwardVector(), cookie->direction);

                            const WRoadSegment *seg = nav.GetSegment();
                            if (seg && seg->IsInRace()) {
                                char raceind = RaceRouteForward(*seg);
                                if (GetNodeInd(nav) != raceind) {
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

bool ResetCar::ResetTo(const UMath::Vector3 &position, const UMath::Vector3 &direction, bool manual) {
    UMath::Vector3 from_positon = GetOwner()->GetPosition();

    if (GetVehicle()->SetVehicleOnGround(position, direction)) {
        new EVehicleReset(reinterpret_cast<uintptr_t>(GetOwner()->GetInstanceHandle()), from_positon, position);
        mCookies.Clear();
        mFlippedOver = 0.0f;

        if (mVehicleBody) {
            mVehicleBody->SetInvulnerability(manual ? INVULNERABLE_FROM_MANUAL_RESET : INVULNERABLE_FROM_RESET, 2.0f);
        }

        return true;
    }

    return false;
}
