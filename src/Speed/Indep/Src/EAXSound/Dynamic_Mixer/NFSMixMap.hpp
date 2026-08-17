//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
//
#ifndef NFSMIXMAP_HPP
#define NFSMIXMAP_HPP // Decl: 18

#include "Speed/Indep/Src/EAXSound/AudioMemBase.hpp"
#include "Speed/Indep/Src/EAXSound/CARSFX/SFXObj_Enums.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMapState.hpp"

// total size: 0x1E0
// Decl: 36
class NFSMixMap : public AudioMemBase {
  public:
    NFSMixMap();
    ~NFSMixMap() override;

    void DestroyMainMixMap();
    void ResetMapData();
    void SetupStateRefCount();
    void AllocateMixerMemory();
    void PreProcessMixMap();
    void AllocateInputArrays();
    NFSMixMapState *GetNextMapState(bool bincrement);
    stCurveDataProc *GetCurveDataPtr(stMixCtlParams *pparams);

    int *AddScaleIDs(stMixCtlParams *pmixctl, int instance);
    int *AddScaleIDs(stMixEvtParams *pevtmixctl, int instance);

    stMixCtlProc *GetProcessMixCtlPtr(bool bincrement);

    void AssignMixCtlDataPtrs(stMixCtlProc *pmixctl, stMixCtlParams *pparms, int nstateindex, int ctlcount);

    stSubMixChProc *GetNextSubMixProc(bool bincrement);
    stMixChSharedData *GetNextSubMixShared(bool bincrement);
    stMixChUniqueData *GetNextSubMixUnique(bool bincrement);
    stMasterMixChProc *GetNextMasterMixProc(bool bincrement);
    stMasterMixChSharedData *GetNextMasterMixShared(bool bincrement);
    stMasterMixChUniqueData *GetNextMasterMixUnique(bool bincrement);
    st3DMixCtlProc *GetNext3DMixCtlProc(bool bincrement);
    st3DMixCtlSharedData *GetNext3DMixCtlShared(bool bincrement);
    st3DMixCtlUniqueData *GetNext3DMixCtlUnique(bool bincrement);
    stEvtMixCtlProc *GetNextEvtMixCtlProc(bool bincrement);
    stEvtMixCtlSharedData *GetNextEvtMixCtlShared(bool bincrement);
    stEvtMixCtlUniqueData *GetNextEvtMixCtlUnique(bool bincrement);

    NFSMixMap *GetMasterMixMap();
    NFSMixMap *GetSecondaryMap();

    int GetMapType() {
        return this->m_MapType;
    } // Decl: 97

    virtual void InitMixMap(int *pmixmap, NFSMixMap *pMasterMixMap);

    void CreateMainMapState(eMAINMAPSTATES estate, int numstates, int objnum);

    void InitMainMapStates();

    void AddMapState(eMAINMAPSTATES estate, int numdups);

    int m_StateRefCount[13]; // offset 0x4, size 0x34, Decl: 127

    virtual void ProcessMixMap(float dt, eCamStates camstate);

    void Update3DMixCtls();
    void UpdateEvtMixCtls();
    void UpdateATREvent(stEvtMixCtlProc *pProc);
    void UpdateASREvent(stEvtMixCtlProc *pProc);
    void UpdateAREvent(stEvtMixCtlProc *pProc);
    void UpdateLFOEvent(stEvtMixCtlProc *pProc);
    void UpdateSubChannels();
    void UpdateMasterChannels();

    void MixMasterChannels();

    void SetupStateRefCountFromGame(int nstate, int ncopy);
    void SetupStateProcArrays();

    int *GetMasterChannelInputPtr(int nsize);
    int *GetSubChannelInputPtr(int nsize);
    int *GetMasterChannelOutputArrayPtr(int nNumChannels);
    int *GetNextInputBlock(bool bincrement);
    int GetMapStateCopies(int nstate);

    NFSMixMapState *GetStateProc(int nID) {
        return this->m_pStateProcs[nID];
    } // Decl: 145

    int *GetMixMapPointer() {
        return this->m_pMixMap;
    } // Decl: 152

    void ConnectMixMap();

    int *GetObjectPtr(int sfxid, bool busedB, bool bHACKINIT);

    static void AssignSFXCallbacks(int *(*getptrcb)(int), void (*setsfxoutcb)(int, int *), bool (*setsfxincb)(int, int *), int (*getstaterefcnt)(int),
                                   void (*mapreadycb)());

    bool SETSFXID(int id, int *ptr);

    void TestSETAllocationParams();

    static int *(*mGetOutPtrCB)(int);        // size: 0x4, address: 0x80418A10
    static void (*mSetSFXOutCB)(int, int *); // size: 0x4, address: 0x80418A14
    static bool (*mSetSFXInCB)(int, int *);  // size: 0x4, address: 0x80418A18
    static int (*mGetStateRefCnt)(int);      // size: 0x4, address: 0x80418A1C
    static void (*mMapReadyCB)();            // size: 0x4, address: 0x80418A20

    stMixMapHeader *m_pMMHdr;  // offset 0x38, size 0x4, Decl: 171
    int m_MapType;             // offset 0x3C, size 0x4, Decl: 172
    eCamStates m_PrevCamState; // offset 0x40, size 0x4, Decl: 173
    eCamStates m_CurCamState;  // offset 0x44, size 0x4, Decl: 174
    float m_fDeltaTime;        // offset 0x48, size 0x4, Decl: 175
    float m_msDeltaTime;       // offset 0x4C, size 0x4, Decl: 176

    NFSMixMap *m_pMasterMixMap; // offset 0x50, size 0x4, Decl: 178

    int *m_pMixMap; // offset 0x54, size 0x4, Decl: 181
    int m_dummyout; // offset 0x58, size 0x4, Decl: 182

    NFSMixMapState **m_pStateProcs;        // offset 0x5C, size 0x4, Decl: 184
    NFSMixMapState **m_pStateProcMemBlock; // offset 0x60, size 0x4, Decl: 185

    int m_nStateMapCount; // offset 0x64, size 0x4, Decl: 201

    int m_nAssignedInputBlocks;  // offset 0x68, size 0x4, Decl: 222
    int m_nAssignedMixMapStates; // offset 0x6C, size 0x4, Decl: 223
    int m_SharedMixCtlCount;     // offset 0x70, size 0x4, Decl: 224
    int m_SharedMixCtlsAssigned; // offset 0x74, size 0x4, Decl: 225
    int m_UniqueMixCtlsAssigned; // offset 0x78, size 0x4, Decl: 226

    int m_CurveProcsAdded;    // offset 0x7C, size 0x4, Decl: 228
    int m_ScaleParamsAdded;   // offset 0x80, size 0x4, Decl: 229
    int m_ScaleParamsIDCount; // offset 0x84, size 0x4, Decl: 230

    int m_CurveProcsTotal[10][2]; // offset 0x88, size 0x50, Decl: 232

    int m_SharedSubMixCount;    // offset 0xD8, size 0x4, Decl: 234
    int m_SharedMasterMixCount; // offset 0xDC, size 0x4, Decl: 235
    int m_Shared3DMixCtlCount;  // offset 0xE0, size 0x4, Decl: 236
    int m_SharedEvtMixCtlCount; // offset 0xE4, size 0x4, Decl: 237

    int m_nAssignedMixCtlProc;   // offset 0xE8, size 0x4, Decl: 239
    int m_AssignedMixCtlsShared; // offset 0xEC, size 0x4, Decl: 240
    int m_AssignedMixCtlsUnique; // offset 0xF0, size 0x4, Decl: 241

    int m_nAssignedSubMixProc;   // offset 0xF4, size 0x4, Decl: 244
    int m_nAssignedSubMixShared; // offset 0xF8, size 0x4, Decl: 245
    int m_nAssignedSubMixUnique; // offset 0xFC, size 0x4, Decl: 246

    int m_nAssignedMasterMixProc;   // offset 0x100, size 0x4, Decl: 248
    int m_nAssignedMasterMixShared; // offset 0x104, size 0x4, Decl: 249
    int m_nAssignedMasterMixUnique; // offset 0x108, size 0x4, Decl: 250

    int m_nAssigned3DMixCtlProc;   // offset 0x10C, size 0x4, Decl: 252
    int m_nAssigned3DMixCtlShared; // offset 0x110, size 0x4, Decl: 253
    int m_nAssigned3DMixCtlUnique; // offset 0x114, size 0x4, Decl: 254

    int m_nAssignedEvtMixCtlProc;   // offset 0x118, size 0x4, Decl: 256
    int m_nAssignedEvtMixCtlShared; // offset 0x11C, size 0x4, Decl: 257
    int m_nAssignedEvtMixCtlUnique; // offset 0x120, size 0x4, Decl: 258

    int *m_pMasterChannelOutputArrayBlock; // offset 0x124, size 0x4, Decl: 261
    int **m_pDynMixInputBlocks;            // offset 0x128, size 0x4, Decl: 262
    int **m_pScalePtrArray;                // offset 0x12C, size 0x4, Decl: 263

    stCurveDataProc *m_pCurveDataArray;  // offset 0x130, size 0x4, Decl: 266
    stMixCtlSharedData *m_pMixCtlData_S; // offset 0x134, size 0x4, Decl: 267
    stMixCtlUniqueData *m_pMixCtlData_U; // offset 0x138, size 0x4, Decl: 268
    stMixCtlProc *m_pMixCtlProc;         // offset 0x13C, size 0x4, Decl: 269

    stEvtMixCtlProc *m_pEvtMixCtlProc;         // offset 0x140, size 0x4, Decl: 271
    stEvtMixCtlSharedData *m_pEvtMixCtlData_S; // offset 0x144, size 0x4, Decl: 272
    stEvtMixCtlUniqueData *m_pEvtMixCtlData_U; // offset 0x148, size 0x4, Decl: 273

    st3DMixCtlProc *m_p3DMixCtlProc;         // offset 0x14C, size 0x4, Decl: 275
    st3DMixCtlSharedData *m_p3DMixCtlData_S; // offset 0x150, size 0x4, Decl: 276
    st3DMixCtlUniqueData *m_p3DMixCtlData_U; // offset 0x154, size 0x4, Decl: 277

    stMixChSharedData *m_pSubChData_S; // offset 0x158, size 0x4, Decl: 279
    stMixChUniqueData *m_pSubChData_U; // offset 0x15C, size 0x4, Decl: 280
    stSubMixChProc *m_pSubChProc;      // offset 0x160, size 0x4, Decl: 281

    stMasterMixChSharedData *m_pMasterChData_S; // offset 0x164, size 0x4, Decl: 283
    stMasterMixChUniqueData *m_pMasterChData_U; // offset 0x168, size 0x4, Decl: 284
    stMasterMixChProc *m_pMasterChProc;         // offset 0x16C, size 0x4, Decl: 285

    int m_SFXOBJsAdded;        // offset 0x170, size 0x4, Decl: 292
    int m_SFXCTLsAdded;        // offset 0x174, size 0x4, Decl: 293
    int m_DataProcsAdded;      // offset 0x178, size 0x4, Decl: 294
    int m_MixCtlsAdded;        // offset 0x17C, size 0x4, Decl: 295
    int m_3DMixCtlsAdded;      // offset 0x180, size 0x4, Decl: 296
    int m_SubMixChannelsAdded; // offset 0x184, size 0x4, Decl: 297
    int m_MasterChannelsAdded; // offset 0x188, size 0x4, Decl: 298
    int m_EventCtlsAdded;      // offset 0x18C, size 0x4, Decl: 299
    int m_n3DCamStatesAdded;   // offset 0x190, size 0x4, Decl: 300

    int m_nTotalMasterChannelInputs;    // offset 0x194, size 0x4, Decl: 302
    int m_nTotalMasterChannel3DOutputs; // offset 0x198, size 0x4, Decl: 303
    int m_nTotalSubChannelInputs;       // offset 0x19C, size 0x4, Decl: 304
    int m_nTotalSubChannel3DOutputs;    // offset 0x1A0, size 0x4, Decl: 305
    int m_nTotalUniqueMasterChannels;   // offset 0x1A4, size 0x4, Decl: 306

    int m_CurrentMasterInputOffset; // offset 0x1A8, size 0x4, Decl: 308
    int m_CurrentSubInputOffset;    // offset 0x1AC, size 0x4, Decl: 309
    int *m_pMasterChannelInputs;    // offset 0x1B0, size 0x4, Decl: 310
    int *m_pSubChannelInputs;       // offset 0x1B4, size 0x4, Decl: 311

    int m_CurrentStateProcBlockOffset;        // offset 0x1B8, size 0x4, Decl: 319
    int m_CurrentEvtMixCtlPtrBlockOffset;     // offset 0x1BC, size 0x4, Decl: 320
    int m_Current3DMixCtlPtrBlockOffset;      // offset 0x1C0, size 0x4, Decl: 321
    int m_CurrentSubChannelPtrBlockOffset;    // offset 0x1C4, size 0x4, Decl: 322
    int m_CurrentMasterChannelPtrBlockOffset; // offset 0x1C8, size 0x4, Decl: 323
    int m_CurrentMasterInputBlockOffset;      // offset 0x1CC, size 0x4, Decl: 324
    int m_CurrentSubInputBlockOffset;         // offset 0x1D0, size 0x4, Decl: 325
    int m_CurrentMasterOutputBlockOffset;     // offset 0x1D4, size 0x4, Decl: 326

    float m_fDeltaTimeRatio[2]; // offset 0x1D8, size 0x8, Decl: 328
};

extern int g_DMIX_DummyOutputBlock[16];
extern int g_DMIX_DummyInputBlock[16];

#endif
