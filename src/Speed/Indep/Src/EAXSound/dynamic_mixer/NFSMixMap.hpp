#ifndef EAXSOUND_DYNAMIC_MIXER_NFSMIXMAP_H
#define EAXSOUND_DYNAMIC_MIXER_NFSMIXMAP_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/SndCamera.hpp"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"

struct NFSMixMapState;

struct stMixMapHeader {
    // total size: 0x10
    int MixMapID;          // offset 0x00, size 0x4
    int NumStates;         // offset 0x04, size 0x4
    int StateTableOffset;  // offset 0x08, size 0x4
    int DynamicMapOffset;  // offset 0x0C, size 0x4
};

struct stMixCtlParams;
struct stSubMixChParams;
struct stMasterMixChParams;
struct st3DMixCtlParams;
struct st3DStateParams;
struct stEvtMixCtlUniqueData;

struct stCurveDataProc {
    // total size: 0x10
    int nINPUTID;      // offset 0x00, size 0x4
    int *pInputParam;  // offset 0x04, size 0x4
    int dBOutput;      // offset 0x08, size 0x4
    int Q15Output;     // offset 0x0C, size 0x4
};

struct st3DMixCtlUniqueData {
    // total size: 0x20
    int nINPUTID;        // offset 0x00, size 0x4
    int *pInputs;        // offset 0x04, size 0x4
    int azimuth;         // offset 0x08, size 0x4
    int dBRolloff;       // offset 0x0C, size 0x4
    int q15Rolloff;      // offset 0x10, size 0x4
    int DopplerCents;    // offset 0x14, size 0x4
    float fPrevDist;     // offset 0x18, size 0x4
    float fPrevDeltaDist; // offset 0x1C, size 0x4
};

struct st3DMixCtlSharedData {
    // total size: 0x14
    st3DMixCtlParams *pMapParams;  // offset 0x00, size 0x4
    st3DStateParams *pCurStateParams; // offset 0x04, size 0x4
    int CurCamState;               // offset 0x08, size 0x4
    int PrevCamState;              // offset 0x0C, size 0x4
    int msSinceCamTrans;           // offset 0x10, size 0x4
};

struct st3DMixCtlProc {
    // total size: 0x8
    st3DMixCtlSharedData *p3DMixCtlData_S;  // offset 0x00, size 0x4
    st3DMixCtlUniqueData *p3DMixCtlData_U;  // offset 0x04, size 0x4
};

struct stMixEvtParams {
    // total size: 0x18
    int nEVTCTLID;       // offset 0x00, size 0x4
    int nUScaleCntSwing; // offset 0x04, size 0x4
    int nTriggerID;      // offset 0x08, size 0x4
    int nParam_00;       // offset 0x0C, size 0x4
    int nParam_01;       // offset 0x10, size 0x4
    int nParam_02;       // offset 0x14, size 0x4
};

struct stEvtMixCtlSharedData {
    // total size: 0x4
    stMixEvtParams *pMapParms;  // offset 0x00, size 0x4
};

struct stEvtMixCtlUniqueData {
    // total size: 0x20
    float msTimeElapsed;   // offset 0x00, size 0x4
    float msResetTime;     // offset 0x04, size 0x4
    int reset;             // offset 0x08, size 0x4
    int reset_level;       // offset 0x0C, size 0x4
    int *pTriggerPtr;      // offset 0x10, size 0x4
    int **ppScaleRatios;   // offset 0x14, size 0x4
    int output;            // offset 0x18, size 0x4
    int qoutput;           // offset 0x1C, size 0x4
};

struct stEvtMixCtlProc {
    // total size: 0x8
    stEvtMixCtlSharedData *pData_S;  // offset 0x00, size 0x4
    stEvtMixCtlUniqueData *pData_U;  // offset 0x04, size 0x4
};

struct stMixCtlSharedData {
    // total size: 0x10
    stMixCtlParams *pstMixCtlParms;  // offset 0x00, size 0x4
    int MIXCTLOBJID;                 // offset 0x04, size 0x4
    int nOffset;                     // offset 0x08, size 0x4
    int nRatio;                      // offset 0x0C, size 0x4
};

struct stMixCtlUniqueData {
    // total size: 0xC
    stCurveDataProc *pstCurveData;  // offset 0x00, size 0x4
    int **ppScaleRatios;            // offset 0x04, size 0x4
    int CmpdBOut;                   // offset 0x08, size 0x4
};

struct stMixCtlProc {
    // total size: 0x8
    stMixCtlSharedData *psdata;  // offset 0x00, size 0x4
    stMixCtlUniqueData *pudata;  // offset 0x04, size 0x4
};

struct stMixChSharedData {
    // total size: 0xC
    stSubMixChParams *pMapParams;  // offset 0x00, size 0x4
    int MIXCHINID;                 // offset 0x04, size 0x4
    int NumInputs;                 // offset 0x08, size 0x4
};

struct stMixChUniqueData {
    // total size: 0x8
    int *pInputs;  // offset 0x00, size 0x4
    int Output;    // offset 0x04, size 0x4
};

struct stSubMixChProc {
    // total size: 0x8
    stMixChSharedData *pMixChData_S;  // offset 0x00, size 0x4
    stMixChUniqueData *pMixChData_U;  // offset 0x04, size 0x4
};

struct stMasterMixChSharedData {
    // total size: 0x10
    stMasterMixChParams *pMapParams;  // offset 0x00, size 0x4
    int MIXCHINID;                    // offset 0x04, size 0x4
    int NumInputs;                    // offset 0x08, size 0x4
    int *pPRESETS;                    // offset 0x0C, size 0x4
};

struct stMasterMixChUniqueData {
    // total size: 0x14
    int outputID;                // offset 0x00, size 0x4
    int *pInputs;                // offset 0x04, size 0x4
    int Output;                  // offset 0x08, size 0x4
    st3DMixCtlProc **p3DData;    // offset 0x0C, size 0x4
    int *pOutputs;               // offset 0x10, size 0x4
};

struct stMasterMixChProc {
    // total size: 0x8
    stMasterMixChSharedData *pMixChData_S;  // offset 0x00, size 0x4
    stMasterMixChUniqueData *pMixChData_U;  // offset 0x04, size 0x4
};

struct NFSMixMap : AudioMemBase {
    // total size: 0x1E0
    static int *(*mGetOutPtrCB)(int);
    static void (*mSetSFXOutCB)(int, int *);
    static bool (*mSetSFXInCB)(int, int *);
    static int (*mGetStateRefCnt)(int);
    static void (*mMapReadyCB)();

    NFSMixMap();
    ~NFSMixMap() override;

    void DestroyMainMixMap();
    static void AssignSFXCallbacks(int *(*getptrcb)(int), void (*setsfxoutcb)(int, int *),
                                   bool (*setsfxincb)(int, int *), int (*getstaterefcnt)(int),
                                   void (*mapreadycb)());
    bool SETSFXID(int id, int *ptr);
    virtual void InitMixMap(int *pmixmap, NFSMixMap *pMasterMixMap);
    void ResetMapData();
    void SetupStateRefCount();
    void PreProcessMixMap();
    void TestSETAllocationParams();
    void AllocateMixerMemory();
    int *GetNextInputBlock(bool bincrement);
    stEvtMixCtlProc *GetNextEvtMixCtlProc(bool bincrement);
    stEvtMixCtlSharedData *GetNextEvtMixCtlShared(bool bincrement);
    stEvtMixCtlUniqueData *GetNextEvtMixCtlUnique(bool bincrement);
    st3DMixCtlProc *GetNext3DMixCtlProc(bool bincrement);
    st3DMixCtlSharedData *GetNext3DMixCtlShared(bool bincrement);
    st3DMixCtlUniqueData *GetNext3DMixCtlUnique(bool bincrement);
    stMasterMixChProc *GetNextMasterMixProc(bool bincrement);
    stMasterMixChSharedData *GetNextMasterMixShared(bool bincrement);
    stMasterMixChUniqueData *GetNextMasterMixUnique(bool bincrement);
    stSubMixChProc *GetNextSubMixProc(bool bincrement);
    stMixChUniqueData *GetNextSubMixUnique(bool bincrement);
    stMixChSharedData *GetNextSubMixShared(bool bincrement);
    NFSMixMapState *GetNextMapState(bool bincrement);
    stCurveDataProc *GetCurveDataPtr(stMixCtlParams *pparams);
    int *AddScaleIDs(stMixCtlParams *pmixctl, int instance);
    int *AddScaleIDs(stMixEvtParams *pevtmixctl, int instance);
    stMixCtlProc *GetProcessMixCtlPtr(bool bincrement);
    void AssignMixCtlDataPtrs(stMixCtlProc *pmixctl, stMixCtlParams *pparms, int nstateindex, int ctlcount);
    int *GetMasterChannelOutputArrayPtr(int nNumChannels);
    int *GetMasterChannelInputPtr(int nsize);
    int *GetSubChannelInputPtr(int nsize);
    int GetMapStateCopies(int nstate);
    void SetupStateRefCountFromGame(int nstate, int ncopy);
    void CreateMainMapState(eMAINMAPSTATES estate, int numstates, int objnum);
    void AllocateInputArrays();
    int *GetObjectPtr(int sfxid, bool busedB, bool bHACKINIT);
    void ConnectMixMap();
    void SetupStateProcArrays();
    void InitMainMapStates();
    void AddMapState(eMAINMAPSTATES estate, int numdups);
    NFSMixMap *GetMasterMixMap();
    NFSMixMap *GetSecondaryMap();
    virtual void ProcessMixMap(float dt, eCamStates camstate);
    void UpdateSubChannels();
    void UpdateMasterChannels();
    void MixMasterChannels();
    void UpdateEvtMixCtls();
    void UpdateLFOEvent(stEvtMixCtlProc *pProc);
    void UpdateATREvent(stEvtMixCtlProc *pProc);
    void UpdateASREvent(stEvtMixCtlProc *pProc);
    void UpdateAREvent(stEvtMixCtlProc *pProc);
    void Update3DMixCtls();

    int GetMapType() { return m_MapType; }
    NFSMixMapState *GetStateProc(int nID) { return m_pStateProcs[nID]; }
    int *GetMixMapPointer() { return m_pMixMap; }

    int m_StateRefCount[13];                        // offset 0x04, size 0x34
    stMixMapHeader *m_pMMHdr;                      // offset 0x38, size 0x4
    int m_MapType;                                 // offset 0x3C, size 0x4
    eCamStates m_PrevCamState;                     // offset 0x40, size 0x4
    eCamStates m_CurCamState;                      // offset 0x44, size 0x4
    float m_fDeltaTime;                            // offset 0x48, size 0x4
    float m_msDeltaTime;                           // offset 0x4C, size 0x4
    NFSMixMap *m_pMasterMixMap;                    // offset 0x50, size 0x4
    int *m_pMixMap;                                // offset 0x54, size 0x4
    int m_dummyout;                                // offset 0x58, size 0x4
    NFSMixMapState **m_pStateProcs;                // offset 0x5C, size 0x4
    NFSMixMapState **m_pStateProcMemBlock;         // offset 0x60, size 0x4
    int m_nStateMapCount;                          // offset 0x64, size 0x4
    int m_nAssignedInputBlocks;                    // offset 0x68, size 0x4
    int m_nAssignedMixMapStates;                   // offset 0x6C, size 0x4
    int m_SharedMixCtlCount;                       // offset 0x70, size 0x4
    int m_SharedMixCtlsAssigned;                   // offset 0x74, size 0x4
    int m_UniqueMixCtlsAssigned;                   // offset 0x78, size 0x4
    int m_CurveProcsAdded;                         // offset 0x7C, size 0x4
    int m_ScaleParamsAdded;                        // offset 0x80, size 0x4
    int m_ScaleParamsIDCount;                      // offset 0x84, size 0x4
    int m_CurveProcsTotal[10][2];                  // offset 0x88, size 0x50
    int m_SharedSubMixCount;                       // offset 0xD8, size 0x4
    int m_SharedMasterMixCount;                    // offset 0xDC, size 0x4
    int m_Shared3DMixCtlCount;                     // offset 0xE0, size 0x4
    int m_SharedEvtMixCtlCount;                    // offset 0xE4, size 0x4
    int m_nAssignedMixCtlProc;                     // offset 0xE8, size 0x4
    int m_AssignedMixCtlsShared;                   // offset 0xEC, size 0x4
    int m_AssignedMixCtlsUnique;                   // offset 0xF0, size 0x4
    int m_nAssignedSubMixProc;                     // offset 0xF4, size 0x4
    int m_nAssignedSubMixShared;                   // offset 0xF8, size 0x4
    int m_nAssignedSubMixUnique;                   // offset 0xFC, size 0x4
    int m_nAssignedMasterMixProc;                  // offset 0x100, size 0x4
    int m_nAssignedMasterMixShared;                // offset 0x104, size 0x4
    int m_nAssignedMasterMixUnique;                // offset 0x108, size 0x4
    int m_nAssigned3DMixCtlProc;                   // offset 0x10C, size 0x4
    int m_nAssigned3DMixCtlShared;                 // offset 0x110, size 0x4
    int m_nAssigned3DMixCtlUnique;                 // offset 0x114, size 0x4
    int m_nAssignedEvtMixCtlProc;                  // offset 0x118, size 0x4
    int m_nAssignedEvtMixCtlShared;                // offset 0x11C, size 0x4
    int m_nAssignedEvtMixCtlUnique;                // offset 0x120, size 0x4
    int *m_pMasterChannelOutputArrayBlock;         // offset 0x124, size 0x4
    int **m_pDynMixInputBlocks;                    // offset 0x128, size 0x4
    int **m_pScalePtrArray;                        // offset 0x12C, size 0x4
    stCurveDataProc *m_pCurveDataArray;            // offset 0x130, size 0x4
    stMixCtlSharedData *m_pMixCtlData_S;           // offset 0x134, size 0x4
    stMixCtlUniqueData *m_pMixCtlData_U;           // offset 0x138, size 0x4
    stMixCtlProc *m_pMixCtlProc;                   // offset 0x13C, size 0x4
    stEvtMixCtlProc *m_pEvtMixCtlProc;             // offset 0x140, size 0x4
    stEvtMixCtlSharedData *m_pEvtMixCtlData_S;     // offset 0x144, size 0x4
    stEvtMixCtlUniqueData *m_pEvtMixCtlData_U;     // offset 0x148, size 0x4
    st3DMixCtlProc *m_p3DMixCtlProc;               // offset 0x14C, size 0x4
    st3DMixCtlSharedData *m_p3DMixCtlData_S;       // offset 0x150, size 0x4
    st3DMixCtlUniqueData *m_p3DMixCtlData_U;       // offset 0x154, size 0x4
    stMixChSharedData *m_pSubChData_S;             // offset 0x158, size 0x4
    stMixChUniqueData *m_pSubChData_U;             // offset 0x15C, size 0x4
    stSubMixChProc *m_pSubChProc;                  // offset 0x160, size 0x4
    stMasterMixChSharedData *m_pMasterChData_S;    // offset 0x164, size 0x4
    stMasterMixChUniqueData *m_pMasterChData_U;    // offset 0x168, size 0x4
    stMasterMixChProc *m_pMasterChProc;            // offset 0x16C, size 0x4
    int m_SFXOBJsAdded;                            // offset 0x170, size 0x4
    int m_SFXCTLsAdded;                            // offset 0x174, size 0x4
    int m_DataProcsAdded;                          // offset 0x178, size 0x4
    int m_MixCtlsAdded;                            // offset 0x17C, size 0x4
    int m_3DMixCtlsAdded;                          // offset 0x180, size 0x4
    int m_SubMixChannelsAdded;                     // offset 0x184, size 0x4
    int m_MasterChannelsAdded;                     // offset 0x188, size 0x4
    int m_EventCtlsAdded;                          // offset 0x18C, size 0x4
    int m_n3DCamStatesAdded;                       // offset 0x190, size 0x4
    int m_nTotalMasterChannelInputs;               // offset 0x194, size 0x4
    int m_nTotalMasterChannel3DOutputs;            // offset 0x198, size 0x4
    int m_nTotalSubChannelInputs;                  // offset 0x19C, size 0x4
    int m_nTotalSubChannel3DOutputs;               // offset 0x1A0, size 0x4
    int m_nTotalUniqueMasterChannels;              // offset 0x1A4, size 0x4
    int m_CurrentMasterInputOffset;                // offset 0x1A8, size 0x4
    int m_CurrentSubInputOffset;                   // offset 0x1AC, size 0x4
    int *m_pMasterChannelInputs;                   // offset 0x1B0, size 0x4
    int *m_pSubChannelInputs;                      // offset 0x1B4, size 0x4
    int m_CurrentStateProcBlockOffset;             // offset 0x1B8, size 0x4
    int m_CurrentEvtMixCtlPtrBlockOffset;          // offset 0x1BC, size 0x4
    int m_Current3DMixCtlPtrBlockOffset;           // offset 0x1C0, size 0x4
    int m_CurrentSubChannelPtrBlockOffset;         // offset 0x1C4, size 0x4
    int m_CurrentMasterChannelPtrBlockOffset;      // offset 0x1C8, size 0x4
    int m_CurrentMasterInputBlockOffset;           // offset 0x1CC, size 0x4
    int m_CurrentSubInputBlockOffset;              // offset 0x1D0, size 0x4
    int m_CurrentMasterOutputBlockOffset;          // offset 0x1D4, size 0x4
    float m_fDeltaTimeRatio[2];                    // offset 0x1D8, size 0x8
};

#endif
