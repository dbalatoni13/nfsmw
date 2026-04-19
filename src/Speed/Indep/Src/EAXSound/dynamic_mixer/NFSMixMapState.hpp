#ifndef EAXSOUND_DYNAMIC_MIXER_NFSMIXMAPSTATE_H
#define EAXSOUND_DYNAMIC_MIXER_NFSMIXMAPSTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct NFSMixMap;
struct stMixCtlProc;
struct st3DMixCtlProc;
struct stEvtMixCtlProc;
struct stSubMixChProc;
struct stMasterMixChProc;
struct stMixCtlHdr;
struct st3DMixCtlHdr;
struct stMixEventHdr;
struct stMixChHdr;

struct stMixStateParams {
    // total size: 0x14
    stMixCtlProc *pMixCtlProcs;             // offset 0x00, size 0x4
    stSubMixChProc *pSubMixChProcs;         // offset 0x04, size 0x4
    stMasterMixChProc *pMasterMixChProcs;   // offset 0x08, size 0x4
    st3DMixCtlProc *p3DMixCtlProc;          // offset 0x0C, size 0x4
    stEvtMixCtlProc *pEvtMixCtlProc;        // offset 0x10, size 0x4
};

struct stMixMapStateHdr {
    // total size: 0x20
    int StateIndex;         // offset 0x00, size 0x4
    int OffsetMixCtlData;   // offset 0x04, size 0x4
    int Offset3DMixCtlData; // offset 0x08, size 0x4
    int OffsetSubMixData;   // offset 0x0C, size 0x4
    int OffsetMasterMixData; // offset 0x10, size 0x4
    int OffsetPresetData;   // offset 0x14, size 0x4
    int OffsetEventCtlData; // offset 0x18, size 0x4
    int Offset_Reserved_07; // offset 0x1C, size 0x4
};

struct NFSMixMapState {
    // total size: 0x60
    NFSMixMapState();
    virtual ~NFSMixMapState();
    virtual void Initialize(NFSMixMap *pmm, int stateindex, int numstatecopies, int objnum);
    int GetStateRefCount();
    void DestroyState();
    stMixCtlProc *GetMixCtlProc(int nMIXCTLIN_ID, int nInstance);
    st3DMixCtlProc *Get3DMixCtlProc(int n3DMIXCTLCREATE_ID, int nInstance);
    stEvtMixCtlProc *GetEvtMixCtlProc(int nEVENTID, int nInstance);
    stSubMixChProc *GetSubMixChProc(int nMIXCHIN_ID, int nInstance);
    stMasterMixChProc *GetMasterMixChProc(int nMIXCHIN_ID, int nInstance);
    void CreateMixCtls();
    void CreateSubMixChannels();
    void CreateMasterMixChannels();
    void CreateEvtMixCtls();
    void Create3DMixCtls();
    void InitializeSubChannels();
    void InitializeMasterChannels();
    NFSMixMap *GetConnectMap(int nID);
    void UpdateState(float t);
    void ProcessMixMap(float t);
    NFSMixMapState *GetMixMapProcAddress();
    NFSMixMapState *GetMixMapProc(int refcnt);
    void SetFirstStateInst(NFSMixMapState *pstate);
    void AddMixState(int objnum, NFSMixMapState *pinst0);
    void RemoveState();

    int GetNumStateCopies() { return m_NumStateCopies; }
    int GetNumCurveProcsAdded();
    int GetNumMixCtlsAdded() { return m_MixCtlsAdded; }
    int GetNumEvtMixCtlsAdded() { return m_EvtMixCtlsAdded; }
    int GetNum3DMixCtlsAdded() { return m_3DMixCtlsAdded; }
    int GetNumSubMixChannelsAdded() { return m_SubMixChannelsAdded; }
    int GetNumMasterChannelsAdded() { return m_MasterChannelsAdded; }
    void SetStateHdr(stMixMapStateHdr *phdr) { m_pMMStateHdr = phdr; }

    NFSMixMap *m_pNFSMixMap;               // offset 0x00, size 0x4
    stMixStateParams m_MixStateParams;     // offset 0x04, size 0x14
    stMixMapStateHdr *m_pMMStateHdr;       // offset 0x18, size 0x4
    stMixCtlHdr *m_pMixCtlHdr;             // offset 0x1C, size 0x4
    st3DMixCtlHdr *m_p3DMixCtlHdr;         // offset 0x20, size 0x4
    stMixEventHdr *m_pEvtMixCtlHdr;        // offset 0x24, size 0x4
    stMixChHdr *m_pMixChHdr;               // offset 0x28, size 0x4
    stMixChHdr *m_pSubChHdr;               // offset 0x2C, size 0x4
    int *m_pChOutArrays;                   // offset 0x30, size 0x4
    int m_StateIndex;                      // offset 0x34, size 0x4
    int m_ThisStateRefCnt;                 // offset 0x38, size 0x4
    int m_ObjectIndex;                     // offset 0x3C, size 0x4
    int m_NumStateCopies;                  // offset 0x40, size 0x4
    int m_MixCtlsAdded;                    // offset 0x44, size 0x4
    int m_3DMixCtlsAdded;                  // offset 0x48, size 0x4
    int m_EvtMixCtlsAdded;                 // offset 0x4C, size 0x4
    int m_SubMixChannelsAdded;             // offset 0x50, size 0x4
    int m_MasterChannelsAdded;             // offset 0x54, size 0x4
    NFSMixMapState *m_pFirstInstance;      // offset 0x58, size 0x4
};

#endif
