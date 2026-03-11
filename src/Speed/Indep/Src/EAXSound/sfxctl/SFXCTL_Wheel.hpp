#ifndef EAXSOUND_SFXCTL_SFXCTL_WHEEL_H
#define EAXSOUND_SFXCTL_SFXCTL_WHEEL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_Wheel : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    ~SFXCTL_Wheel() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    void UpdateParams(float t) override;

    void UpdateTireParams();
};

#endif
