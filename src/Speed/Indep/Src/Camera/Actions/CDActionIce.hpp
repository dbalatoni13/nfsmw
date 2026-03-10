#ifndef CAMERA_ACTIONS_CDACTIONICE_H
#define CAMERA_ACTIONS_CDACTIONICE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/World/WorldConn.h"

class ICEAnchor;
class ICEMover;
class ICETrack;

// total size: 0x2F8
class CDActionIce : public CameraAI::Action, public IAttachable {
  public:
    const Attrib::StringKey &GetName() const override;
    Attrib::StringKey GetNext() const override;
    CameraMover *GetMover() override;
    void SetSpecial(float val) override {}
    bool Attach(UTL::COM::IUnknown *pOther) override;
    bool Detach(UTL::COM::IUnknown *pOther) override;
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override;
    void OnAttached(IAttachable *pOther) override {}
    const IAttachable::List *GetAttachments() const override;

    static CameraAI::Action *Construct(CameraAI::Director *director);
    CDActionIce(CameraAI::Director *director, IPlayer *player);
    ~CDActionIce() override;
    void Reset() override {}
    void OnDetached(IAttachable *pOther) override;
    void ReleaseCar(bool detach);
    void AquireCar();
    void Update(float dT) override;

  private:
    ActionQueue mActionQ;           // offset 0x20, size 0x294
    Attrib::StringKey mPrev;        // offset 0x2B8, size 0x10
    ICEAnchor *mAnchor;             // offset 0x2C8, size 0x4
    ICEMover *mMover;               // offset 0x2CC, size 0x4
    WorldConn::Reference mTarget;   // offset 0x2D0, size 0x10
    IPlayer *mPlayer;               // offset 0x2E0, size 0x4
    Sim::Attachments *mAttachments; // offset 0x2E4, size 0x4
    IVehicle *mVehicle;             // offset 0x2E8, size 0x4
    ICETrack *mTrack;               // offset 0x2EC, size 0x4
    bool mDone;                     // offset 0x2F0, size 0x1
};

#endif
