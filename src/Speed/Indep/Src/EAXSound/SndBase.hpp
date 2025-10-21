#ifndef EAXSOUND_SNDBASE_H
#define EAXSOUND_SNDBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"

enum DMX_PRESET_TYPE {
    DMX_DEPTH = 4,
    DMX_AZIM = 3,
    DMX_FREQ = 2,
    DMX_PITCH = 1,
    DMX_VOL = 0,
};

enum eAemsUpgradeLevel { AEMS_NO_LEVEL = -1, AEMS_LEVEL0 = 0, AEMS_LEVEL1 = 1, AEMS_LEVEL2 = 2, AEMS_LEVEL3 = 3, AEMS_MAX_LEVEL = 4 };

struct SndBase : public AudioMemBase {
  public:
    struct TypeInfo {
        // total size: 0x10
        int ObjectID;               // offset 0x0, size 0x4
        char *typeName;             // offset 0x4, size 0x4
        TypeInfo *baseTypeInfo;     // offset 0x8, size 0x4
        SndBase *(*createObject)(); // offset 0xC, size 0x4

        SndBase *CreateObject(unsigned int allocator);
    };

    static float m_fRunningTime;
    static float m_fDeltaTime;

    SndBase();
    virtual ~SndBase();
    virtual TypeInfo *GetTypeInfo(void) const;
    virtual char *GetTypeName() const;
    virtual int GetController(int Index) {
        return -1;
    }
    virtual void AttachController(struct SFXCTL *psfxctl) {}
    virtual void SetupSFX(struct CSTATE_Base *_StateBase);
    virtual void SetupLoadData() {
        InitSFX();
    }
    virtual void InitSFX() {
        if (m_pInputBlock)
            m_pInputBlock[15] = 1;
    }
    virtual void Destroy() {}
    virtual void UpdateParams(float) {}
    virtual void ProcessUpdate() {}
    virtual void Detach() {}
    virtual void UpdateMixerOutputs() {}

    int GetDMixOutput(int idx, DMX_PRESET_TYPE etype);

  protected:
    static TypeInfo s_TypeInfo; // for some ungodly reason these are all defined in zEAXSound.cpp
                                // but the rest of SndBase's funcs are in zEAXSound2.cpp

  private:
    // total size: 0x24
    SndBase *m_pNextSFX;              // offset 0x4, size 0x4
    bool m_bIsEnabled;                // offset 0x8, size 0x1
    int *m_pOutPutBlock;              // offset 0xC, size 0x4
    int *m_pInputBlock;               // offset 0x10, size 0x4
    struct CSTATE_Base *m_pStateBase; // offset 0x14, size 0x4
    struct EAXCar *m_pEAXCar;         // offset 0x18, size 0x4
    unsigned short m_refCount;        // offset 0x1C, size 0x2
    unsigned short m_allocatorIndex;  // offset 0x1E, size 0x2
    int objectID;                     // offset 0x20, size 0x4
};

#endif
