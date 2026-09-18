#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/Movers/DebugWorld.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int Tweak_EnableICEAuthoring;

class CDActionDebug : public CameraAI::Action, public ITrafficCenter {
  public:
    static CameraAI::Action *Construct(CameraAI::Director *director);

    CDActionDebug(CameraAI::Director *director);
    ~CDActionDebug() override;

    const Attrib::StringKey &GetName() const override {
        static Attrib::StringKey name("CDActionDebug");
        return name;
    }
    Attrib::StringKey GetNext() const override {
        return mSwitchOut ? mNext : Attrib::StringKey("");
    }
    CameraMover *GetMover() override {
        return mMover;
    }
    void SetSpecial(float scale) override {}

    void Reset() override;
    void Update(float dT) override;
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;

    USE_FASTALLOC(CDActionDebug)

  private:
    ActionQueue mQueue;       // offset 0x20, size 0x294
    CameraMover *mMover;      // offset 0x2B4, size 0x4
    Attrib::StringKey mNext;  // offset 0x2B8, size 0x10
    int mSwitchOut;           // offset 0x2C8, size 0x4
};

UTL::COM::Factory<CameraAI::Director *, CameraAI::Action, UCrc32>::Prototype _CDActionDebug("CDActionDebug", CDActionDebug::Construct);

CameraAI::Action *CDActionDebug::Construct(CameraAI::Director *director) {
    return new CDActionDebug(director);
}

CDActionDebug::CDActionDebug(CameraAI::Director *director)
    : CameraAI::Action(0), mQueue(1, 0x98c7a2f5, "CDActionDebug", false), mNext(), mSwitchOut(0) {
    bVector3 start_position;
    bFill(&start_position, 0.0f, 0.0f, 1.0f);
    bVector3 start_direction;
    bFill(&start_direction, 0.0f, 0.0f, 1.0f);

    CameraAI::Action *prev_action = director->GetAction();
    if (prev_action) {
        mNext = prev_action->GetName();
        CameraMover *prev_mover = prev_action->GetMover();
        if (prev_mover) {
            start_position = *prev_mover->GetPosition();
            bScaleAdd(&start_direction, prev_mover->GetPosition(), prev_mover->GetDirection(), 20.0f);
        }
    }

    mMover = new ("DebugWorldCameraMover", 0) DebugWorldCameraMover(director->GetView(), &start_position, &start_direction, static_cast<JoystickPort>(1));
}

CDActionDebug::~CDActionDebug() {
    delete mMover;
}

void CDActionDebug::Reset() {}

void CDActionDebug::Update(float dT) {
    while (!mQueue.IsEmpty()) {
        const ActionRef action = mQueue.GetAction();

        if (action.ID() == 21 && !Tweak_EnableICEAuthoring) {
            mSwitchOut = 1;
        }

        mQueue.PopAction();
    }
}

bool CDActionDebug::GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) {
    bMatrix4 world;
    eInvertTransformationMatrix(&world, reinterpret_cast<const bMatrix4 *>(mMover->GetCamera()));

    // audit.py: el objetivo llama a RightToLeftMatrix4<UMath::Matrix4,UMath::Matrix4> aqui
    // y a <bMatrix4,UMath::Matrix4> en CDActionDebugWatchCar -- estaban cruzados, y objdiff
    // daba 100% a las dos porque compara la grafia y no el destino de la reubicacion. `world`
    // sigue siendo bMatrix4: con `UMath::Matrix4 world` el `&world` deja de compartirse entre
    // las dos llamadas y se pierde el `addi r30,r1,8` unico del objetivo (84,58%).
    ConversionUtil::RightToLeftMatrix4(*reinterpret_cast<const UMath::Matrix4 *>(&world), matrix);
    ConversionUtil::RightToLeftVector3(*reinterpret_cast<const UMath::Vector3 *>(mMover->GetCamera()->GetVelocityPosition()), velocity);

    return true;
}
