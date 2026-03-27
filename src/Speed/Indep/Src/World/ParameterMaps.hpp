#ifndef WORLD_PARAMETERMAPS_H
#define WORLD_PARAMETERMAPS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bList.hpp"

class ParameterMapLayer;

class ParameterAccessor : public bTNode<ParameterAccessor> {
  public:
    ParameterAccessor();
    ParameterAccessor(const char *layer_name);
    virtual ~ParameterAccessor();

    virtual void CaptureData(float x, float y);
    virtual void ClearData();
    virtual float GetDataFloat(int field_index);
    virtual int GetDataInt(int field_index);

  protected:
    virtual void SetUpForNewLayer();

    ParameterMapLayer *Layer;
    unsigned int AutoAttachLayerNamehash;
    const char *DebugName;
    void *CurrentParameterData;
};

class ParameterAccessorBlend : public ParameterAccessor {
  public:
    ParameterAccessorBlend();
    ParameterAccessorBlend(const char *layer_name);
    virtual ~ParameterAccessorBlend();

    virtual void CaptureData(float x, float y, float ratio);
    virtual void ClearData();

  protected:
    virtual void SetUpForNewLayer();

    void *LastData;
    int HaveLastData;

  private:
    virtual void CaptureData(float x, float y);
};

class ParameterAccessorBlendByDistance : public ParameterAccessorBlend {
  public:
    ParameterAccessorBlendByDistance();
    ParameterAccessorBlendByDistance(const char *layer_name);
    virtual ~ParameterAccessorBlendByDistance();

    virtual void CaptureData(float x, float y, float full_blend_distance);

  protected:
    virtual void SetUpForNewLayer();

    float last_x;
    float last_y;
    int HaveLastPosition;

  private:
    virtual void CaptureData(float x, float y);
};

extern ParameterAccessorBlendByDistance TintSunRiseAccessor[2];
extern ParameterAccessorBlendByDistance TintMiddayAccessor[2];

#endif
