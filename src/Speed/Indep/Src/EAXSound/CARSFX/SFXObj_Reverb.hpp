#ifndef EAXSOUND_CARSFX_SFXOBJ_REVERB_H
#define EAXSOUND_CARSFX_SFXOBJ_REVERB_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Tunnel.hpp"

namespace Snd {
class GlobalFxProcessor;
}

struct SFXObj_Reverb : public CARSFX {
  public:
    struct ReverbStructure {
        void *Alloc; // offset 0x0, size 0x4
        int Size;    // offset 0x4, size 0x4

        ReverbStructure() {
            Clear();
        }

        void Clear() {
            Alloc = nullptr;
            Size = 0;
        }
    };

    static TypeInfo s_TypeInfo;
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

  public:
    static Snd::GlobalFxProcessor *m_pFXEditModule[2];
    static char *m_pFXEditPatch[12];
    static void *m_EchoBuffer;
    static ReverbStructure m_EchoAllocs[4];

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    static SndBase *CreateObject(unsigned int allocator);
    SFXObj_Reverb();
    ~SFXObj_Reverb() override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupLoadData() override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    SFXCTL_Tunnel *m_pTunnelCtl; // offset 0x28, size 0x4
    void InitSFX() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void Destroy() override;
};

#endif
