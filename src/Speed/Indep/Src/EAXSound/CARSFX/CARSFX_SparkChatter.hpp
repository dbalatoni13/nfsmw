//
#ifndef CARSFX_SPARKCHATTER_H
#define CARSFX_SPARKCHATTER_H

#include "Speed/Indep/Src/EAXSound/CARSFX/CARSFX.hpp"
#include "Speed/Indep/Src/EAXSound/SND_GEN/ENGINES_AEMS2.h"
#include "Speed/Indep/Src/EAXSound/sfxctl/SFXCTL_3DCarPos.hpp"

// total size: 0x74
// Decl: 27
class CARSFX_SparkChatter : public CARSFX {
  public:
    DECLARE_CREATABLE();
    CARSFX_SparkChatter();
    ~CARSFX_SparkChatter() override;

    // Overrides: SndBase
    int GetController(int Index) override;
    void AttachController(SFXCTL *psfxctl) override;
    void SetupSFX(CSTATE_Base *_StateBase) override;
    void InitSFX() override;
    void Destroy() override;
    void SetupLoadData() override;
    void UpdateParams(float t) override;
    void ProcessUpdate() override;
    void UpdateMixerOutputs() override;

    static void SparkChatCreateCallBack(Csis::Class *pSparkChatClass, Csis::Parameter *pParameters, void *pClientData);

    static void SparkChatUpdateCallBack(Csis::Parameter *pParameters, void *pClientData);

    static void SparkChatDestroyCallBack(Csis::Class *pSparkChatClass, void *pClientData);

    void ReceiveSparkChatterInputs(Csis::CAR_SputOutputStruct *pInputs);

    SFXCTL_Shifting *m_pShiftingCTL;             // offset 0x28, size 0x4, Decl: 45
    Csis::CAR_Sputter *m_pSparkChatterControl;   // offset 0x2C, size 0x4, Decl: 47
    Csis::CAR_SputOutput *m_pSparkChatterOutput; // offset 0x30, size 0x4, Decl: 48
    Attrib::Gen::engineaudio *m_pSweetnersData;  // offset 0x34, size 0x4, Decl: 50

    // Decl: 55
    struct SparkChatOutputInstance {
        SparkChatOutputInstance() {} // Decl: 56

        CARSFX_SparkChatter *m_pThis;              // offset 0x0, size 0x4, Decl: 60
        Csis::ClassConstructorClient CreateClient; // offset 0x4, size 0x10, Decl: 61
        Csis::MemberDataClient UpdateClient;       // offset 0x14, size 0x10, Decl: 62
        Csis::ClassDestructorClient DestroyClient; // offset 0x24, size 0x10, Decl: 63
    };

    SparkChatOutputInstance SparkChatOutputClients; // offset 0x38, size 0x34, Decl: 66
    int BlipVol;                                    // offset 0x6C, size 0x4, Decl: 76
    SFXCTL_3DCarPos *m_p3DCarPosCtl;                // offset 0x70, size 0x4, Decl: 77
};

#endif
