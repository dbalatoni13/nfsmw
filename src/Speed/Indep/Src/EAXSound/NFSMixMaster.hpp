#ifndef EAXSOUND_NFSMIXMASTER_H
#define EAXSOUND_NFSMIXMASTER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"

struct stDynMixMap;
struct NFSMixMap;
struct NFSMixChannel;

enum eRACETYPE {
    eRACE_CIRCUIT = 0,
    eRACE_DRAG = 1,
    eRACE_TWOCIRC = 2,
    eRACE_TWODRG = 3,
};

enum eMMTYPE {
    MMTYPE_MAIN = 0,
    MMTYPE_SECONDARY = 1,
    MMTYPE_DYNAMIC = 2,
    MMTYPE_SECONDARYDYNAMIC = 3,
};

struct NFSMixMaster : AudioMemBase {
    // total size: 0x74
    int m_LoadMapID;                   // offset 0x04, size 0x4
    stDynMixMap *m_pCurDynMMP;         // offset 0x08, size 0x4
    stDynMixMap *m_pHeadDynMMP;        // offset 0x0C, size 0x4
    stDynMixMap *m_pTailDynMMP;        // offset 0x10, size 0x4
    stDynMixMap *m_pSecDynamicMMP;     // offset 0x14, size 0x4
    stDynMixMap *m_pSecDynMMPHead;     // offset 0x18, size 0x4
    stDynMixMap *m_pSecDynMMPTail;     // offset 0x1C, size 0x4
    int *m_pMainMixMapData;            // offset 0x20, size 0x4
    int *m_pSecondaryMixMapData;       // offset 0x24, size 0x4
    NFSMixMap *m_pMainMixMap;          // offset 0x28, size 0x4
    NFSMixMap *m_pSecondaryMixMap;     // offset 0x2C, size 0x4
    NFSMixChannel *m_pSFXMxCh;         // offset 0x30, size 0x4
    bool m_bMapReady;                  // offset 0x34, size 0x1
    int m_StateRefCount[13];           // offset 0x38, size 0x34
    int m_nCurNumDMMP;                 // offset 0x6C, size 0x4
    int m_nCurNumSecondaryDMMP;        // offset 0x70, size 0x4

    NFSMixMap *GetMasterMixMap() { return m_pMainMixMap; }
    bool IsMixMapReady() { return m_bMapReady; }

    NFSMixMaster();
    virtual ~NFSMixMaster();

    void *operator new(unsigned int size, const char *debug_name);
    void *operator new(size_t, void *p) { return p; }

    void SetMainMixMapPtr(int *pmem);
    void ConnectMap();
    void DestroyMainMainMap();
    void CreateMainMainMap(eRACETYPE eMapType);
    void LoadMixMapFile(eMMTYPE etype, char *pfilename);
    static void LoadDataCallback(int param, int error_status);
    void DestroyMap();
    void InitMixMap(int param);
    void ProcessMixMap(float dt, eCamStates ecam);
    static void AssignSFXCallbacks(int *(*GetPointerCB)(int), void (*SetSFXOutCB)(int, int *),
                                   bool (*SetSFXInputCB)(int, int *), int (*GetStateRefCountCB)(int), void (*MixReadyCB)());
};

#endif
