#ifndef EAXSOUND_EAXTUNERCAR_H
#define EAXSOUND_EAXTUNERCAR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/EAXCar.hpp"

struct EAXTunerCar : public EAXCar {
    EAXTunerCar();
    virtual ~EAXTunerCar();

    virtual void PreLoadAssets() override;
    virtual void UpdateParams(float t) override;
    virtual StateInfo *GetStateInfo(void) const override;
    virtual const char *GetStateName(void) const override;

    virtual void ProcessSoundSphere(unsigned int unamehash, int nparamid, bVector3 *pv3pos, float fradius) override;
    virtual int SFXMessage(eSFXMessageType SFXMessageType, unsigned int param1, unsigned int param2) override;
    virtual void UpdatePov() override;
    virtual int UpdateRotation() override;
    virtual int Play(void *peventst);

    static StateInfo *GetStaticStateInfo(void) {
        return &s_StateInfo;
    }
    static CSTATE_Base *CreateState(unsigned int allocator);

    void *operator new(size_t s, unsigned int allocator) {
        if (allocator != 0) {
            return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, true);
        } else {
            return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, false);
        }
    }

    void *operator new(size_t s) {
        return gAudioMemoryManager.AllocateMemory(s, GetStaticStateInfo()->stateName, 0);
    }

    void *operator new(size_t, void *p) { return p; }

  protected:
    void FirstUpdate(float t);

  public:
    static StateInfo s_StateInfo;

    int m_playerID;              // offset 0x114
    float m_fCarVolume;          // offset 0x118
    bool BottomOutPlay;          // offset 0x11C
    int BottomOutIntensity;      // offset 0x120
    bool TrunkBouncePlay;        // offset 0x124
    float TrunkBounceInstensity; // offset 0x128
    bool PlayBackFire;           // offset 0x12C
  protected:
    bool bFirstUpdate;           // offset 0x130
};

#endif
