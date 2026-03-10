#ifndef CAMERA_ACTIONS_CDACTIONDRIVE_H
#define CAMERA_ACTIONS_CDACTIONDRIVE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/IListener.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/World/WorldConn.h"

struct _h_HHANDLER__;
typedef _h_HHANDLER__ *HHANDLER;

class MJumpCut;

// total size: 0x80
class CDActionDrive : public CameraAI::Action, public IAttachable, public Sim::Collision::IListener, public ITrafficCenter {
  public:
    const Attrib::StringKey &GetName() const override;
    Attrib::StringKey GetNext() const override;
    CameraMover *GetMover() override { return mMover; }
    void SetSpecial(float val) override;
    bool Attach(UTL::COM::IUnknown *pOther) override;
    bool Detach(UTL::COM::IUnknown *pOther) override;
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override;
    void OnAttached(IAttachable *pOther) override {}
    const IAttachable::List *GetAttachments() const override;

    static CameraAI::Action *Construct(CameraAI::Director *director);
    CDActionDrive(CameraAI::Director *director, IPlayer *player);
    ~CDActionDrive() override;
    void Reset() override;
    void OnDetached(IAttachable *pOther) override;
    void OnCarDetached();
    void OnCollision(const Sim::Collision::Info &cinfo) override;
    void AquireCar();
    void Update(float dT) override;
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;
    void MessageJumpCut(const MJumpCut &message);

  private:
    CameraMover *mMover;                // offset 0x2C, size 0x4
    CameraMover *mRearViewMirrorMover;  // offset 0x30, size 0x4
    CameraAnchor *mAnchor;              // offset 0x34, size 0x4
    WorldConn::Reference mTarget;       // offset 0x38, size 0x10
    IPlayer *mPlayer;                   // offset 0x48, size 0x4
    Sim::Attachments *mAttachments;     // offset 0x4C, size 0x4
    IVehicle *mVehicle;                 // offset 0x50, size 0x4
    float mGameBreakerScale;            // offset 0x54, size 0x4
    EVIEW_ID mViewID;                   // offset 0x58, size 0x4
    float mDampCollisionTime;           // offset 0x5C, size 0x4
    float mGroundCollisionTime;         // offset 0x60, size 0x4
    float mObjectCollisionTime;         // offset 0x64, size 0x4
    const float mMaxCollisionTime;      // offset 0x68, size 0x4
    float mPulseTimer;                  // offset 0x6C, size 0x4
    float mCinematicMomementTimer;      // offset 0x70, size 0x4
    bool mCinematicMomementTimerInc;    // offset 0x74, size 0x1
    int mGear;                          // offset 0x78, size 0x4
    HHANDLER mMsgJumpCut;               // offset 0x7C, size 0x4
};

#endif
