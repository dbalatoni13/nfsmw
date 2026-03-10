#ifndef CAMERA_ACTIONS_CDACTIONDEBUG_H
#define CAMERA_ACTIONS_CDACTIONDEBUG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"

// total size: 0x2D0
class CDActionDebug : public CameraAI::Action, public ITrafficCenter {
  public:
    const Attrib::StringKey &GetName() const override;
    Attrib::StringKey GetNext() const override;
    CameraMover *GetMover() override { return mMover; }
    void SetSpecial(float val) override {}

    static CameraAI::Action *Construct(CameraAI::Director *director);
    CDActionDebug(CameraAI::Director *director);
    ~CDActionDebug() override;
    void Reset() override {}
    void Update(float dT) override;
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;

  private:
    ActionQueue mActionQ;           // offset 0x20, size 0x294
    CameraMover *mMover;            // offset 0x2B4, size 0x4
    Attrib::StringKey mPrev;        // offset 0x2B8, size 0x10
    bool mDone;                     // offset 0x2C8, size 0x1
};

#endif
