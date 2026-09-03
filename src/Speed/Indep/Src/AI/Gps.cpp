#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/AI/AIVehicle.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSFinished.hpp"
#include "Speed/Indep/Src/Generated/Events/EGPSLost.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/IActivity.h"
#include "Speed/Indep/Src/Interfaces/Simables/IRigidBody.h"
#include "Speed/Indep/Src/Sim/SimActivity.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/WRoadNetwork.h"

// total size: 0x388
// Decl: 16
class Gps : public Sim::Activity, public UTL::Collections::Singleton<Gps> {
  public:
    USE_FASTALLOC(Gps);
    Gps();
    ~Gps() override;

    static Sim::IActivity *Construct(Sim::Param params);

    // Overrides: ITaskable
    bool OnTask(HSIMTASK htask, float dT) override;

    void Render(eView *view);

    // Decl: 29
    bool IsEngaged() const {
        return this->mState != GPS_DOWN;
    }

    // Decl: 31
    void Disengage() {
        this->mState = GPS_DOWN;
    }

    bool Engage(const UMath::Vector3 &target, float maxDeviation);
    void Update(float dT);

  private:
    UMath::Vector3 mTarget;    // offset 0x50, size 0xC, Decl: 39
    HSIMTASK mTask;            // offset 0x5C, size 0x4, Decl: 40
    UMath::Vector3 mPosition;  // offset 0x60, size 0xC, Decl: 41
    eModel *mArrowModel;       // offset 0x6C, size 0x4, Decl: 42
    UMath::Vector3 mDirection; // offset 0x70, size 0xC, Decl: 43

    // Decl: 45
    enum eGPSState {
        GPS_DOWN = 0,
        GPS_SEARCHING = 1,
        GPS_TRACKING = 2,
    };

    eGPSState mState;     // offset 0x7C, size 0x4, Decl: 51
    WRoadNav mRoadNav;    // offset 0x80, size 0x2F0, Decl: 52
    Meters mPathDist;     // offset 0x370, size 0x4, Decl: 53
    bool mDrawn;          // offset 0x374, size 0x1, Decl: 54
    Radians mAngle;       // offset 0x378, size 0x4, Decl: 55
    float mScale;         // offset 0x37C, size 0x4, Decl: 56
    Meters mMaxDeviation; // offset 0x380, size 0x4, Decl: 57
    float mDeviation;     // offset 0x384, size 0x4, Decl: 58
};

BIND_ACTIVITY_FACTORY(Gps); // Decl: 65
IMPLEMENT_SINGLETON(Gps);   // Decl: 66

static const bVector2 Tweak_ForceGPSArrowTo(0.0f, 0.0f); // Decl: 69
static const bool Tweak_ForceGPSArrow = false;           // Decl: 70
bool gHideGPSArrow = false;                              // Decl: 71

static const float Tweak_MinGPSLookAhead = 20.0f; // Decl: 85
static const float Tweak_MaxGPSLookAhead = 80.0f; // Decl: 86
static const float Tweak_GPSLookAheadTime = 3.0f; // Decl: 87
static const float Tweak_MaxDeviation = 200.0f;   // Decl: 88
static const float Tweak_ArrivedDistance = 30.0f; // Decl: 89

static const float Tweak_GPSCatchupSpeed = 10.0f;      // Decl: 91
static const float Tweak_GPSMinRotationSpeed = 0.125f; // Decl: 92 TODO use in ::Render
static const float Tweak_GPSMaxRotationSpeed = 1.0f;   // Decl: 93 TODO use in ::Render
static const bool Tweak_ForceGPSSearching = false;     // Decl: 94

Sim::IActivity *Gps::Construct(Sim::Param params) {
    Gps *gps = Gps::Get();
    if (gps == nullptr) {
        gps = new Gps();
    }
    return gps;
}

Gps::Gps() : Activity(0) {
    this->mArrowModel = new eModel();
    this->mArrowModel->Init(bStringHash("MARKER_DIRECTION_AID"));

    this->mTask = this->AddTask(UCrc32("WorldUpdate"), 1.0f, 0.0f, Sim::TaskMode(1));
    Sim::ProfileTask(this->mTask, "GPS");

    this->mTarget = UMath::Vector3::kZero;
    this->mPosition = UMath::Vector3::kZero;
    this->mDirection = UMath::Vector3::kZero;

    this->mState = GPS_DOWN;

    this->mRoadNav.SetNavType(WRoadNav::kTypePath);
    this->mRoadNav.SetPathType(WRoadNav::kPathGPS);
    this->mRoadNav.SetLaneType(WRoadNav::kLaneRacing);

    this->mPathDist = Tweak_MinGPSLookAhead;
    this->mDrawn = false;
    this->mAngle = 0.0f;
    this->mMaxDeviation = Tweak_MaxDeviation;
    this->mScale = 0.0f;
    this->mDeviation = 0.0f;
}

Gps::~Gps() {
    this->mRoadNav.CancelPathFinding();
    Object::RemoveTask(this->mTask);
    delete this->mArrowModel;
    this->mArrowModel = nullptr;
}

bool Gps::OnTask(HSIMTASK htask, float dT) {
    if (htask != this->mTask) {
        return false;
    }
    this->Update(dT);
    return true;
}

void Gps::Update(float dT) {
    if (this->mState == GPS_DOWN) {
        return;
    }

    IVehicle *vehicle = IVehicle::First(VEHICLE_PLAYERS);

    IRigidBody *body;
    if (vehicle == nullptr || !vehicle->QueryInterface(&body)) {
        this->mState = GPS_DOWN;
        return;
    }

    const UMath::Vector3 &position = body->GetPosition();
    const UMath::Vector3 &velocity = body->GetLinearVelocity();
    UMath::Vector3 heading;

    vehicle->ComputeHeading(&heading);
    this->mRoadNav.SetNavType(WRoadNav::kTypePath);
    this->mRoadNav.InitAtPoint(position, heading, true, 1.0f);

    if (!this->mRoadNav.OnPath() || Tweak_ForceGPSSearching) {
        this->mRoadNav.SetNavType(WRoadNav::kTypePath);
        this->mState = GPS_SEARCHING;
        this->mRoadNav.InitAtPath(position, true);

        if (!this->mRoadNav.IsValid()) {
            new EGPSLost();
            this->mState = GPS_DOWN;
            return;
        }
    } else {
        this->mState = GPS_TRACKING;
    }

    UMath::Vector3 path_dir;
    path_dir = this->mRoadNav.GetForwardVector();
    UMath::Unit(path_dir);

    float speed = UMath::Clamp(UMath::Dot(velocity, path_dir) * Tweak_GPSLookAheadTime, Tweak_MinGPSLookAhead, Tweak_MaxGPSLookAhead);
    if (speed > this->mPathDist) {
        this->mPathDist = UMath::Min(this->mPathDist + dT * Tweak_GPSCatchupSpeed, speed);
    } else {
        this->mPathDist = UMath::Max(this->mPathDist - dT * Tweak_GPSCatchupSpeed, speed);
    }

    this->mRoadNav.IncNavPosition(this->mPathDist, path_dir, 0.0f);

    this->mPosition = this->mRoadNav.GetPosition();
    this->mDirection = this->mRoadNav.GetForwardVector();
    UMath::Unit(this->mDirection);

    if (UMath::Distance(this->mTarget, vehicle->GetPosition()) < Tweak_ArrivedDistance) {
        new EGPSFinished();
        this->mState = GPS_DOWN;
        return;
    }

    if (this->mMaxDeviation > UMath::Epsilon && this->mState == GPS_SEARCHING) {
        float distance = UMath::Distance(this->mPosition, vehicle->GetPosition());

        if (distance > this->mMaxDeviation) {
            new EGPSLost();
            this->mState = GPS_DOWN;
            return;
        }

        this->mDeviation = distance / this->mMaxDeviation;
    } else {
        this->mDeviation = 0.0f;
    }
}

bool Gps::Engage(const UMath::Vector3 &target, float maxDeviation) {
    this->mState = GPS_DOWN;
    this->mTarget = target;
    this->mRoadNav.CancelPathFinding();
    this->mDrawn = false;
    this->mScale = 0.0f;
    this->mDeviation = 0.0f;

    if (maxDeviation < 0.1f) {
        this->mMaxDeviation = Tweak_MaxDeviation;
    } else {
        this->mMaxDeviation = maxDeviation;
    }

    IVehicle *vehicle = IVehicle::First(VEHICLE_PLAYERS);
    ICollisionBody *body;

    if (vehicle != nullptr && vehicle->QueryInterface(&body)) {
        const UMath::Vector3 &direction = body->GetForwardVector();
        const UMath::Vector3 &position = body->GetPosition();

        this->mRoadNav.SetNavType(WRoadNav::kTypePath);
        this->mRoadNav.SetPathType(WRoadNav::kPathGPS);
        this->mRoadNav.SetLaneType(WRoadNav::kLaneRacing);
        this->mRoadNav.InitAtPoint(position, direction, true, 1.0f);

        if (!this->mRoadNav.IsValid()) {
            return false;
        }

        if (!this->mRoadNav.FindPathNow(&this->mTarget, nullptr, nullptr)) {
            return false;
        }

        if (this->mRoadNav.OnPath()) {
            this->mPosition = this->mRoadNav.GetPosition();
            this->mDirection = this->mRoadNav.GetForwardVector();
            UMath::Unit(this->mDirection);
            this->mState = GPS_TRACKING;
            this->mPathDist = Tweak_MinGPSLookAhead;
            return true;
        }
    }

    return false;
}

// TODO do once FE is merged
// void Gps::Render(eView *view) {}

void GPS_Disengage() {
    Gps *gps = Gps::Get();
    if (gps != nullptr) {
        gps->Disengage();
    }
}

bool GPS_Engage(const UMath::Vector3 &target, float maxDeviation) {
    Gps *gps = Gps::Get();
    if (gps != nullptr) {
        return gps->Engage(target, maxDeviation);
    }
    return false;
}

bool GPS_IsEngaged() {
    Gps *gps = Gps::Get();
    if (gps != nullptr) {
        return gps->IsEngaged();
    }
    return false;
}

void RenderGpsArrows(eView *view) {
    Gps *gps = Gps::Get();
    if (gps != nullptr) {
        gps->Render(view);
    }
}
