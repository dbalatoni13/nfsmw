#ifndef CAMERA_ACTIONS_CDACTIONSHOWCASE_H
#define CAMERA_ACTIONS_CDACTIONSHOWCASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Sim/SimAttachable.h"
#include "Speed/Indep/Src/World/WorldConn.h"

// total size: 0x48
class CDActionShowcase : public CameraAI::Action, public IAttachable {
  public:
    const Attrib::StringKey &GetName() const override;
    Attrib::StringKey GetNext() const override;
    CameraMover *GetMover() override { return mMover; }
    void SetSpecial(float val) override {}
    bool Attach(UTL::COM::IUnknown *pOther) override;
    bool Detach(UTL::COM::IUnknown *pOther) override;
    bool IsAttached(const UTL::COM::IUnknown *pOther) const override;
    void OnAttached(IAttachable *pOther) override {}
    const IAttachable::List *GetAttachments() const override;

    static CameraAI::Action *Construct(CameraAI::Director *director);
    CDActionShowcase(CameraAI::Director *director, IPlayer *player);
    ~CDActionShowcase() override;
    void Reset() override {}
    void OnDetached(IAttachable *pOther) override;
    void OnCarDetached();
    void AquireCar();
    void Update(float dT) override;

  private:
    CameraMover *mMover;            // offset 0x20, size 0x4
    CameraAnchor *mAnchor;          // offset 0x24, size 0x4
    WorldConn::Reference mTarget;   // offset 0x28, size 0x10
    IPlayer *mPlayer;               // offset 0x38, size 0x4
    Sim::Attachments *mAttachments; // offset 0x3C, size 0x4
    IVehicle *mVehicle;             // offset 0x40, size 0x4
    EVIEW_ID mViewID;               // offset 0x44, size 0x4
};

#endif
