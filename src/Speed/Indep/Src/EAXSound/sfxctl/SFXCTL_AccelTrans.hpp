#ifndef EAXSOUND_SFXCTL_SFXCTL_ACCELTRANS_H
#define EAXSOUND_SFXCTL_SFXCTL_ACCELTRANS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL.hpp"

struct SFXCTL_AccelTrans : public SFXCTL {
  protected:
    static TypeInfo s_TypeInfo;

  public:
    TypeInfo *GetTypeInfo() const override;
    char *GetTypeName() const override;
    void Destroy() override;
};

#endif
