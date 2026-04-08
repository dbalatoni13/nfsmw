#ifndef EAXSOUND_CARSFX_CARSFX_SPARKCHATTER_H
#define EAXSOUND_CARSFX_CARSFX_SPARKCHATTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_Shifting.hpp"
#include "Speed/Indep/Src/EAXSound/snd_gen/ENGINES_AEMS2.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engineaudio.h"

struct CARSFX_SparkChatter : public CARSFX {
    struct SparkChatOutputInstance {
        SparkChatOutputInstance() {}

        CARSFX_SparkChatter *m_pThis;              // offset 0x0, size 0x4
        Csis::ClassConstructorClient CreateClient; // offset 0x4, size 0x10
        Csis::MemberDataClient UpdateClient;       // offset 0x14, size 0x10
        Csis::ClassDestructorClient DestroyClient; // offset 0x24, size 0x10
    };

  public:
    static TypeInfo s_TypeInfo;

  public:
    static TypeInfo *GetStaticTypeInfo() { return &s_TypeInfo; }

    CARSFX_SparkChatter();
    ~CARSFX_SparkChatter() override;

    static SndBase *CreateObject(unsigned int allocator);

    TypeInfo *GetTypeInfo() const override;
    const char *GetTypeName() const override;
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void UpdateMixerOutputs() override;
    void SetupLoadData() override;

    static void SparkChatCreateCallBack(Csis::Class *pSparkChatClass, Csis::Parameter *pParameters, void *pClientData);
    static void SparkChatUpdateCallBack(Csis::Parameter *pParameters, void *pClientData);
    static void SparkChatDestroyCallBack(Csis::Class *pSparkChatClass, void *pClientData);

    void ReceiveSparkChatterInputs(Csis::CAR_SputOutputStruct *pInputs);

  private:
    SFXCTL_Shifting *m_pShiftingCTL;               // offset 0x28, size 0x4
    Csis::CAR_Sputter *m_pSparkChatterControl;     // offset 0x2C, size 0x4
    Csis::CAR_SputOutput *m_pSparkChatterOutput;   // offset 0x30, size 0x4
    Attrib::Gen::engineaudio *m_pSweetnersData;    // offset 0x34, size 0x4
    SparkChatOutputInstance SparkChatOutputClients; // offset 0x38, size 0x34
    int BlipVol;                                 // offset 0x6C, size 0x4
    SFXCTL_3DCarPos *m_p3DCarPosCtl;             // offset 0x70, size 0x4
};

#endif
