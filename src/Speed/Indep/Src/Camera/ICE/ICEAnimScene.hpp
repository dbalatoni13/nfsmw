#ifndef CAMERA_ICE_ICEANIMSCENE_H
#define CAMERA_ICE_ICEANIMSCENE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

class ICEScene {
  public:
    virtual unsigned int GetSceneHash();
    virtual int GetCameraTrackNumber();
    virtual bool IsControllingCamera();
    virtual bool IsCameraFixingElevation();
    virtual void SetTime(float time);
    virtual bool Pause();
    virtual bool UnPause();
    virtual bool IsPlaying();
    virtual float GetTimeStart();
    virtual float GetTimeTotalLength();
    virtual float GetTimeElapsed();
    virtual bMatrix4 &GetSceneRotationMatrix();
    virtual bMatrix4 &GetSceneTransformMatrix();
};

#endif
