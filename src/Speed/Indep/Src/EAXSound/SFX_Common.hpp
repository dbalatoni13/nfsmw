#ifndef EAXSOUND_SFX_COMMON_H
#define EAXSOUND_SFX_COMMON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/SFX_base.hpp"
#include "Speed/Indep/Src/Misc/Hermes.h"

struct MMiscSound;

class FX_Camera;
class FX_UVES;

class SFX_Common : public SFX_Base {
  protected:
    static TypeInfo s_TypeInfo;

    static TypeInfo *GetStaticTypeInfo() {
        return &s_TypeInfo;
    }

  public:
    FX_Camera *m_pcsisCameraShot;      // offset 0x24, size 0x4
    FX_UVES *m_pUves;                  // offset 0x28, size 0x4
    FX_UVES *m_pPursuitBreakStart;     // offset 0x2C, size 0x4
    FX_UVES *m_pPursuitBreakEnd;       // offset 0x30, size 0x4
    Hermes::HHANDLER mMsgMiscSound;    // offset 0x34, size 0x4

    SFX_Common();
    ~SFX_Common() override;

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;

    static SndBase *CreateObject(unsigned int allocator);

    void AttachController(SFXCTL *psfxctl) override;
    void SetupLoadData() override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;

    void MsgPlayMiscSound(const MMiscSound &message);
};

#endif
