#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/Cubic.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/World/WorldConn.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int CameraDebugWatchCar;
extern int mToggleCar;
extern int mToggleCarList;

// Listable<IDebugWatchCar,2>::_mTable: en .bss del original va entre
// _CDActionIce (0x8045AC70) y _CDActionDebugWatchCar (0x8045AC94), asi que la
// instanciacion vive aqui y no al final del SourceList.
IMPLEMENT_LISTABLE(IDebugWatchCar)

int mToggleCar = 0;
int mToggleCarList = 0;

class CDActionDebugWatchCar : public CameraAI::Action, public IDebugWatchCar, public ITrafficCenter {
  public:
    static CameraAI::Action *Construct(CameraAI::Director *director);

    CDActionDebugWatchCar(CameraAI::Director *director);
    ~CDActionDebugWatchCar() override;

    ISimable *GetSimable() override;
    void ReleaseTarget();
    void AquireTarget();

    void Reset() override;
    const Attrib::StringKey &GetName() const override {
        static Attrib::StringKey name("CDActionDebugWatchCar");
        return name;
    }
    Attrib::StringKey GetNext() const override {
        if (CameraDebugWatchCar) {
            return Attrib::StringKey();
        }
        return mNext;
    }
    CameraMover *GetMover() override {
        return mMover;
    }
    void Update(float dT) override;
    void SetSpecial(float scale) override {}
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;

    USE_FASTALLOC(CDActionDebugWatchCar)

  private:
    CameraMover *mMover;          // offset 0x2C, size 0x4
    CameraAnchor *mAnchor;        // offset 0x30, size 0x4
    WorldConn::Reference mTarget; // offset 0x34, size 0x10
    Attrib::StringKey mNext;      // offset 0x48, size 0x10
    unsigned int mHandle;         // offset 0x58, size 0x4
};

UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype
    _CDActionDebugWatchCar("CDActionDebugWatchCar", CDActionDebugWatchCar::Construct);

CameraAI::Action *CDActionDebugWatchCar::Construct(CameraAI::Director *director) {
    if (!director->GetAction()) {
        return 0;
    }

    return new CDActionDebugWatchCar(director);
}

CDActionDebugWatchCar::CDActionDebugWatchCar(CameraAI::Director *director)
    : CameraAI::Action(0), IDebugWatchCar(this), mTarget(0) {
    mHandle = 0;
    mNext = director->GetAction()->GetName();

    mAnchor = new ("CameraAnchor", 0) CameraAnchor(0);
    mMover = new ("CubicCameraMover", 0) CubicCameraMover(director->GetView(), mAnchor, 3, false, false, false, true);
}

CDActionDebugWatchCar::~CDActionDebugWatchCar() {
    ReleaseTarget();

    delete mMover;
    delete mAnchor;
}

void CDActionDebugWatchCar::Reset() {}

ISimable *CDActionDebugWatchCar::GetSimable() {
    return ISimable::FindInstance(reinterpret_cast<HSIMABLE>(mHandle));
}

void CDActionDebugWatchCar::ReleaseTarget() {
    if (mTarget.IsValid()) {
        mHandle = 0;
        mTarget.Set(0);
    }
}

void CDActionDebugWatchCar::AquireTarget() {
    ISimable *current = ISimable::FindInstance(reinterpret_cast<HSIMABLE>(mHandle));
    if (!current) {
        ReleaseTarget();
    }

    if (mToggleCar < 0) {
        return;
    }
    if (mToggleCarList > 9) {
        return;
    }
    if (mToggleCarList < 0) {
        return;
    }

    int count = IVehicle::Count(static_cast<eVehicleList>(mToggleCarList));
    if (!count) {
        return;
    }

    IVehicle *vehicle = IVehicle::GetList(static_cast<eVehicleList>(mToggleCarList))[mToggleCar % count];
    if (!vehicle) {
        return;
    }

    if (vehicle->GetSimable()->GetInstanceHandle() == reinterpret_cast<HSIMABLE>(mHandle)) {
        return;
    }

    unsigned int wuid = vehicle->GetSimable()->GetWorldID();
    if (!wuid) {
        return;
    }

    ReleaseTarget();
    mAnchor->SetModel(bStringHash(vehicle->GetVehicleAttributes().MODEL().GetString()));
    mTarget.Set(wuid);
    mHandle = reinterpret_cast<unsigned int>(vehicle->GetSimable()->GetInstanceHandle());
}

void CDActionDebugWatchCar::Update(float dT) {
    AquireTarget();

    if (mTarget.IsValid()) {
        mAnchor->SetWorldID(mTarget.GetWorldID());
        mAnchor->Update(dT, *mTarget.GetMatrix(), *mTarget.GetVelocity(), *mTarget.GetAcceleration());
    }
}

bool CDActionDebugWatchCar::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    bool valid = mTarget.IsValid();
    if (valid) {
        // audit.py: aqui el objetivo instancia <bMatrix4,...> y <bVector3,...>, o sea sin
        // ningun cast; los que habia mandaban a las plantillas de CDActionDebug.
        ConversionUtil::RightToLeftMatrix4(*mTarget.GetMatrix(), matrix);
        ConversionUtil::RightToLeftVector3(*mTarget.GetVelocity(), velocity);
    }

    return valid;
}
