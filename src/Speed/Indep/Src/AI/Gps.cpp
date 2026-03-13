#include "Speed/Indep/Src/AI/Gps.h"

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"

Sim::IActivity *Gps::Construct(Sim::Param params) {
    Gps *gps = Get();
    if (!gps) {
        gps = new Gps();
    }
    if (gps) {
        return static_cast<Sim::IActivity *>(gps);
    }
    return nullptr;
}

Gps::Gps()
    : Activity(0) //
    , mRoadNav() //
    , mState(GPS_DOWN) {
    mArrowModel = new eModel();
    mArrowModel->Init(bStringHash("gps_arrow"));
    mTask = AddTask(UCrc32("Gps"), 0.0f, 0.0f, Sim::TaskMode(1));
    Sim::ProfileTask(mTask, "Gps");
    mTarget = UMath::Vector3::kZero;
    mPosition = UMath::Vector3::kZero;
    mDirection = UMath::Vector3::kZero;
    mRoadNav.SetNavType(WRoadNav::kTypeNone);
    mRoadNav.SetLaneType(WRoadNav::kLaneAny);
    mPathDist = 0.0f;
    mDrawn = false;
    mMaxDeviation = 200.0f;
    mScale = 0.0f;
    mAngle = 0.0f;
}

Gps::~Gps() {
    mRoadNav.CancelPathFinding();
    RemoveTask(mTask);
    if (mArrowModel) {
        mArrowModel->UnInit();
        delete mArrowModel;
    }
    mArrowModel = nullptr;
}

bool Gps::OnTask(HSIMTASK htask, float dT) {
    bool matched = htask == mTask;
    if (matched) {
        Update(dT);
    }
    return matched;
}

void GPS_Disengage() {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Disengage();
    }
}

void GPS_Engage(const UMath::Vector3 &target, float maxDeviation) {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Engage(target, maxDeviation);
    }
}

bool GPS_IsEngaged() {
    Gps *gps = Gps::Get();
    if (gps) {
        return gps->IsEngaged();
    }
    return false;
}

void RenderGpsArrows(eView *view) {
    Gps *gps = Gps::Get();
    if (gps) {
        gps->Render(view);
    }
}
