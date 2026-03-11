#include "Speed/Indep/Src/Camera/Actions/CDActionDebugWatchCar.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern int mToggleCar;
extern eVehicleList mToggleCarList;

extern bool CameraDebugWatchCar;

template <>
UTL::Collections::Listable<IDebugWatchCar, 2>::List
    UTL::Collections::Listable<IDebugWatchCar, 2>::_mTable;

static UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDebugWatchCar("DEBUGWATCHCAR", CDActionDebugWatchCar::Construct);

const Attrib::StringKey &CDActionDebugWatchCar::GetName() const {
    static Attrib::StringKey name("DEBUGWATCHCAR");
    return name;
}

Attrib::StringKey CDActionDebugWatchCar::GetNext() const {
    if (CameraDebugWatchCar) {
        return Attrib::StringKey();
    }
    return mPrev;
}

ISimable *CDActionDebugWatchCar::GetSimable() {
    return ISimable::FindInstance(mhSimable);
}

void CDActionDebugWatchCar::ReleaseTarget() {
    if (mTarget.IsValid()) {
        mhSimable = 0;
        mTarget.Set(0);
    }
}

void CDActionDebugWatchCar::AquireTarget() {
    ISimable *isim = ISimable::FindInstance(mhSimable);
    if (isim == nullptr) {
        ReleaseTarget();
    }

    if (mToggleCar >= 0 && mToggleCarList <= 9 && mToggleCarList >= 0) {
        int count = IVehicle::Count(mToggleCarList);
        if (count != 0) {
            IVehicle *ivehicle = IVehicle::GetList(mToggleCarList)[static_cast<unsigned int>(mToggleCar % count)];
            if (ivehicle != nullptr) {
                if (ivehicle->GetSimable()->GetInstanceHandle() != mhSimable) {
                    unsigned int world_id = ivehicle->GetSimable()->GetWorldID();
                    if (world_id != 0) {
                        ReleaseTarget();
                        CameraAnchor *anchor = mAnchor;
                        const char *model_str = ivehicle->GetVehicleAttributes().MODEL().GetString();
                        if (model_str == nullptr) {
                            model_str = "";
                        }
                        anchor->SetModel(bStringHash(model_str));
                        mTarget.Set(world_id);
                        mhSimable = ivehicle->GetSimable()->GetInstanceHandle();
                    }
                }
            }
        }
    }
}

CameraAI::Action *CDActionDebugWatchCar::Construct(CameraAI::Director *director) {
    if (!director->GetAction()) {
        return nullptr;
    }
    return new CDActionDebugWatchCar(director);
}

CDActionDebugWatchCar::CDActionDebugWatchCar(CameraAI::Director *director)
    : CameraAI::Action(), //
      IDebugWatchCar(this) {
    mTarget = WorldConn::Reference(0);
    mhSimable = nullptr;
    mPrev = director->GetAction()->GetName();

    mAnchor = new CameraAnchor(0);
    mMover = new TrackCarCameraMover(static_cast<int>(director->GetViewID()), mAnchor, true);
}

CDActionDebugWatchCar::~CDActionDebugWatchCar() {
    ReleaseTarget();
    CameraMover *m = mMover;
    delete m;
    delete mAnchor;
}

void CDActionDebugWatchCar::Update(float dT) {
    AquireTarget();
    if (mTarget.IsValid()) {
        mAnchor->SetWorldID(mTarget.GetWorldID());
        const bVector3 *accel = mTarget.GetAcceleration();
        mAnchor->Update(dT, *mTarget.GetMatrix(), *mTarget.GetVelocity(), *accel);
    }
}

bool CDActionDebugWatchCar::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    bool valid = mTarget.IsValid();
    if (valid) {
        ConversionUtil::RightToLeftMatrix4(*mTarget.GetMatrix(), matrix);
        ConversionUtil::RightToLeftVector3(*mTarget.GetVelocity(), velocity);
    }
    return valid;
}

template class UTL::Collections::Listable<IDebugWatchCar, 2>::List;
