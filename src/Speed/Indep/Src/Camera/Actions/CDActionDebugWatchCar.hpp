#ifndef CAMERA_ACTIONS_CDACTIONDEBUGWATCHCAR_H
#define CAMERA_ACTIONS_CDACTIONDEBUGWATCHCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/CameraAI.hpp"
#include "Speed/Indep/Src/Camera/IDebugWatchCar.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/ITrafficCenter.h"
#include "Speed/Indep/Src/World/WorldConn.h"

// total size: 0x60
class CDActionDebugWatchCar : public CameraAI::Action, public IDebugWatchCar, public ITrafficCenter {
  public:
    const Attrib::StringKey &GetName() const override;
    Attrib::StringKey GetNext() const override;
    CameraMover *GetMover() override { return mMover; }
    void SetSpecial(float val) override {}

    static CameraAI::Action *Construct(CameraAI::Director *director);
    CDActionDebugWatchCar(CameraAI::Director *director);
    ~CDActionDebugWatchCar() override;
    void Reset() override {}
    ISimable *GetSimable() override;
    void ReleaseTarget();
    void AquireTarget();
    void Update(float dT) override;
    bool GetTrafficBasis(UMath::Matrix4 &matrix, UMath::Vector3 &velocity) override;

  private:
    CameraMover *mMover;            // offset 0x2C, size 0x4
    CameraAnchor *mAnchor;          // offset 0x30, size 0x4
    WorldConn::Reference mTarget;   // offset 0x34, size 0x10
    Attrib::StringKey mPrev;        // offset 0x48, size 0x10
    HSIMABLE mhSimable;             // offset 0x58, size 0x4
};

#endif
