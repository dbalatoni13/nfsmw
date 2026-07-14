#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EVehicleReset.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/ICollisionBody.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRBVehicle.h"
#include "Speed/Indep/Src/Interfaces/Simables/IResetable.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISuspension.h"
#include "Speed/Indep/Src/Physics/VehicleBehaviors.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/WCollisionMgr.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// total size: 0x20
struct ResetCookie {
    UMath::Vector3 position;  // offset 0x0, size 0xC
    uint32 flags;             // offset 0xC, size 0x4
    UMath::Vector3 direction; // offset 0x10, size 0xC
    float time;               // offset 0x1C, size 0x4
};

static const bool Tweak_DisableAutoReset = false;         // size: 0x1, Decl: speed/indep/src/Physics/behaviors/ResetCar.cpp:20
static const float Tweak_ResetCar_UprightDot = 0.5f;      // size: 0x4, Decl: speed/indep/src/Physics/behaviors/ResetCar.cpp:21
static const float Tweak_ResetCar_GroundDot = 0.5f;       // size: 0x4, Decl: speed/indep/src/Physics/behaviors/ResetCar.cpp:22
static const float Tweak_ResetCar_FlipTime = 4.0f;        // size: 0x4, Decl: speed/indep/src/Physics/behaviors/ResetCar.cpp:23
static const float Tweak_CookieDist = 2.0f;               // size: 0x4, Decl: speed/indep/src/Physics/behaviors/ResetCar.cpp:24
static const float Tweak_ResetInvulnerablitiyTime = 2.0f; // size: 0x4, Decl: speed/indep/src/Physics/behaviors/ResetCar.cpp:25

// total size: 0xFC

class ResetCar : protected VehicleBehavior, protected IResetable {
  public:
    typedef VehicleBehavior Base;

    static Behavior *Construct(const BehaviorParams &params);
    ResetCar(const BehaviorParams &params);
    ~ResetCar() override;

    // Overrides: IResetable
    bool HasResetPosition() override {
        return this->mCookies.Count() != 0;
    }
    bool ResetVehicle(bool manual) override;
    void SetResetPosition(const UMath::Vector3 &position, const UMath::Vector3 &direction) override {
        ResetCookie cookie;

        this->mCookies.Clear();
        cookie.position = position;
        cookie.direction = direction;
        cookie.time = Sim::GetTime();
        cookie.flags = 0;
        this->mCookies.AddNew(cookie);
    }

    // Overrides: ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    // Overrides: IResetable
    void ClearResetPosition() override {
        this->mCookies.Clear();
    }

  protected:
    // Overrides: Behavior
    void OnBehaviorChange(const UCrc32 &mechanic) override;
    void Reset() override;

  private:
    bool FindNearestRoad(ResetCookie *cookie) const;
    void Check(float dT);
    bool ValidTerrain(unsigned int checktires) const;
    bool CanRecord() const;
    bool ShouldReset() const;
    void TrackState(float dT);
    bool CheckZone(eTrackPathZoneType type);
    bool ResetTo(const UMath::Vector3 &position, const UMath::Vector3 &direction, bool manual);

    float mFlippedOver;                   // offset 0x58, size 0x4
    CookieTrail<ResetCookie, 4> mCookies; // offset 0x5C, size 0x90
    ICollisionBody *mCollisionBody;       // offset 0xEC, size 0x4
    ISuspension *mSuspension;             // offset 0xF0, size 0x4
    IRBVehicle *mVehicleBody;             // offset 0xF4, size 0x4
    HSIMTASK mCheckTask;                  // offset 0xF8, size 0x4
};

BIND_BEHAVIOR_FACTORY(ResetCar);

Behavior *ResetCar::Construct(const BehaviorParams &params) {
    return new ResetCar(params);
}

ResetCar::ResetCar(const BehaviorParams &params)
    : VehicleBehavior(params, 0), //
      IResetable(params.fowner),  //
      mFlippedOver(0.0f),         //
      mCookies() {
    this->mCheckTask = this->AddTask("Physics", 1.0f, 0.0f, Sim::TASK_FRAME_VARIABLE);
    Sim::ProfileTask(this->mCheckTask, "ResetCar");

    this->GetOwner()->QueryInterface(&this->mCollisionBody);
    this->GetOwner()->QueryInterface(&this->mSuspension);
    this->GetOwner()->QueryInterface(&this->mVehicleBody);
}

ResetCar::~ResetCar() {
    this->RemoveTask(this->mCheckTask);

    if (this->mVehicleBody != nullptr) {
        this->mVehicleBody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
    }
}

void ResetCar::OnBehaviorChange(const UCrc32 &mechanic) {
    if (mechanic == BEHAVIOR_MECHANIC_SUSPENSION) {
        this->GetOwner()->QueryInterface(&this->mSuspension);
    }
    if (mechanic == BEHAVIOR_MECHANIC_RIGIDBODY) {
        this->GetOwner()->QueryInterface(&this->mCollisionBody);
        this->GetOwner()->QueryInterface(&this->mVehicleBody);
    }
    this->Behavior::OnBehaviorChange(mechanic);
}

bool ResetCar::ValidTerrain(unsigned int checktires) const {
    if (this->mSuspension != nullptr) {
        SimSurface owner_surface(this->GetOwner()->GetWPos().GetSurface());

        if (owner_surface == SimSurface::kNull) {
            unsigned int invalid_tires = 0;

            for (unsigned int i = 0; i < this->mSuspension->GetNumWheels(); i++) {
                if (this->mSuspension->GetWheelRoadSurface(i) == SimSurface::kNull) {
                    invalid_tires++;
                }
            }

            if (invalid_tires >= checktires) {
                return false;
            }
        }
    }

    return true;
}

bool ResetCar::CanRecord() const {
    if (this->mCookies.Count() != 0) {
        const ResetCookie &cookie = this->mCookies.Newest();
        if (UMath::DistanceSquare(cookie.position, this->GetOwner()->GetPosition()) < (Tweak_CookieDist * Tweak_CookieDist)) {
            return false;
        }
    }

    if (!this->ValidTerrain(0)) {
        return false;
    }

    if (this->mSuspension != nullptr) {
        if (this->mSuspension->GetNumWheelsOnGround() != this->mSuspension->GetNumWheels()) {
            return false;
        }
    }

    return true;
}

bool ResetCar::ShouldReset() const {
    if (Tweak_DisableAutoReset || this->mCookies.Count() == 0) {
        return false;
    }

    if (this->GetVehicle()->GetDriverClass() == DRIVER_HUMAN && !this->ValidTerrain(2)) {
        return true;
    }

    return Tweak_ResetCar_FlipTime < this->mFlippedOver;
}

void ResetCar::TrackState(float dT) {
    bool flippedover = false;

    if (this->mSuspension != nullptr) {
        if (this->mSuspension->GetNumWheelsOnGround() != this->mSuspension->GetNumWheels() && this->mCollisionBody != nullptr &&
            this->mCollisionBody->IsModeling()) {
            const UMath::Vector3 &vup = this->mCollisionBody->GetUpVector();
            float ground_dot = UMath::Dot(vup, UMath::Vector4To3(this->mCollisionBody->GetGroundNormal()));

            if (ground_dot < Tweak_ResetCar_GroundDot) {
                flippedover = vup.y < Tweak_ResetCar_UprightDot;
            }
        }
    }

    if (flippedover) {
        this->mFlippedOver += dT;
    } else {
        this->mFlippedOver = 0.0f;
    }
}

bool ResetCar::CheckZone(eTrackPathZoneType type) {
    bVector2 position2d(this->mCollisionBody->GetPosition().z, -this->mCollisionBody->GetPosition().x);
    TrackPathZone *zone = TheTrackPathManager.FindZone(&position2d, type, nullptr);

    if (zone != nullptr) {
        bVector2 *zone_position = zone->GetPosition();
        bVector2 *zone_direction = zone->GetDirection();
        UMath::Vector3 reset_position = UMath::Vector3Make(-zone_position->y, 0.0f, zone_position->x);
        UMath::Vector3 reset_dir = UMath::Vector3Make(-zone_direction->y, 0.0f, zone_direction->x);

        if (UMath::Epsilon < UMath::Normalize(reset_dir)) {
            return this->ResetTo(reset_position, reset_dir, false);
        }
    }

    return false;
}

void ResetCar::Check(float dT) {
    {
        ResetCookie cookie;
        UMath::Matrix4 mat;

        this->TrackState(dT);
        if (!this->CheckZone(TRACK_PATH_ZONE_RESET_TO_POINT)) {
            if (this->CanRecord()) {
                this->GetOwner()->GetTransform(mat);
                cookie.time = Sim::GetTime();
                cookie.position = UMath::Vector4To3(mat.v3);
                cookie.direction = UMath::Vector4To3(mat.v2);
                cookie.flags = 1;
                this->mCookies.AddNew(cookie);
            } else if (this->ShouldReset()) {
                this->ResetVehicle(false);
            }
        }
    }
}

bool ResetCar::OnTask(HSIMTASK htask, float dT) {
    if (htask == this->mCheckTask) {
        if (!this->IsPaused()) {
            this->Check(dT);
        }
        return true;
    }
    return this->Object::OnTask(htask, dT);
}

void ResetCar::Reset() {
    if (this->mVehicleBody != nullptr) {
        this->mVehicleBody->SetInvulnerability(INVULNERABLE_NONE, 0.0f);
    }
    this->mCookies.Clear();
    this->mFlippedOver = 0.0f;
}

bool ResetCar::ResetTo(const UMath::Vector3 &position, const UMath::Vector3 &direction, bool manual) {
    UMath::Vector3 from_positon = this->GetOwner()->GetPosition();

    if (this->GetVehicle()->SetVehicleOnGround(position, direction)) {
        new EVehicleReset(reinterpret_cast<uintptr_t>(this->GetOwner()->GetInstanceHandle()), from_positon, position);
        this->mCookies.Clear();
        this->mFlippedOver = 0.0f;

        if (this->mVehicleBody != nullptr) {
            this->mVehicleBody->SetInvulnerability(manual ? INVULNERABLE_FROM_MANUAL_RESET : INVULNERABLE_FROM_RESET, Tweak_ResetInvulnerablitiyTime);
        }

        return true;
    }

    return false;
}

static const float Tweak_MaxResetAltitudeChange = 3.5f;

bool ResetCar::FindNearestRoad(ResetCookie *cookie) const {
    IVehicleAI *iai;
    if (this->GetOwner()->QueryInterface(&iai)) {
        WRoadNav nav;

        nav.SetNavType(WRoadNav::kTypeDirection);
        nav.SetLaneType(WRoadNav::kLaneReset);
        nav.SetRaceFilter(GRaceStatus::Exists() && GRaceStatus::Get().GetActivelyRacing());
        nav.SetDecisionFilter(true);

        UMath::Vector3 drive_dir = cookie->direction;
        nav.InitAtPoint(cookie->position, drive_dir, false, 1.0f);
        nav.SnapToSelectableLane();

        if (nav.IsValid()) {
            float elevation_difference = nav.GetPosition().y - cookie->position.y;
            if (elevation_difference == bClamp(elevation_difference, -6.0f, 6.0f)) {
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

                float worldHeight;
                if (!crosses_barrier && WCollisionMgr(0, 3).GetWorldHeightAtPointRigorous(nav.GetPosition(), worldHeight, nullptr)) {
                    if (UMath::Abs(cookie->position.y - worldHeight) < Tweak_MaxResetAltitudeChange) {
                        cookie->position = nav.GetPosition();
                        UMath::Unit(nav.GetForwardVector(), cookie->direction);

                        const WRoadSegment *seg = nav.GetSegment();
                        if (seg != nullptr && seg->IsInRace()) {
                            char raceind = seg->RaceRouteForward() ? 1 : 0;

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

    return false;
}

bool ResetCar::ResetVehicle(bool manual) {
    if (manual && this->CheckZone(TRACK_PATH_ZONE_GUIDED_RESET)) {
        return true;
    }

    ResetCookie cookie;
    if (this->HasResetPosition() && (!manual || this->mVehicleBody == nullptr || !this->mVehicleBody->GetInvulnerability())) {
        cookie = this->mCookies.Oldest();
        if (cookie.flags & 1) {
            this->FindNearestRoad(&cookie);
        }
        if (this->ResetTo(cookie.position, cookie.direction, manual)) {
            this->mCookies.Clear();
            this->mCookies.AddNew(cookie);
            return true;
        }
    }

    return false;
}
