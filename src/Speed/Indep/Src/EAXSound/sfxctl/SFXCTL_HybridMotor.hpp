#ifndef EAXSOUND_SFXCTL_SFXCTL_HYBRIDMOTOR_H
#define EAXSOUND_SFXCTL_SFXCTL_HYBRIDMOTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_HybridMotor : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    ~SFXCTL_HybridMotor() override;
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
};

#endif
