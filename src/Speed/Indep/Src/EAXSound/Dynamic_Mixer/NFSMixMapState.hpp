#ifndef NFSMIXMAPSTATE_HPP
#define NFSMIXMAPSTATE_HPP // Decl: 104

#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixerDefines.hpp"

// total size: 0x14
// Decl: 107
struct stMixStateParams {
    stMixCtlProc *pMixCtlProcs;           // offset 0x0, size 0x4, Decl: 108
    stSubMixChProc *pSubMixChProcs;       // offset 0x4, size 0x4, Decl: 109
    stMasterMixChProc *pMasterMixChProcs; // offset 0x8, size 0x4, Decl: 110
    st3DMixCtlProc *p3DMixCtlProc;        // offset 0xC, size 0x4, Decl: 111
    stEvtMixCtlProc *pEvtMixCtlProc;      // offset 0x10, size 0x4, Decl: 112
};

class NFSMixMap;

// total size: 0x60
// Decl: 126
class NFSMixMapState {
  public:
    NFSMixMapState();

    virtual ~NFSMixMapState();

    virtual void Initialize(NFSMixMap *pmm, int stateindex, int numstatecopies, int objnum);

    void AddMixState(int objnum, NFSMixMapState *pinst0);
    void RemoveState();
    void DestroyState();

    NFSMixMap *GetConnectMap(int nID);
    NFSMixMapState *GetMixMapProc(int refcnt);
    stMixCtlProc *GetMixCtlProc(int nMIXCTLIN_ID, int nInstance);
    st3DMixCtlProc *Get3DMixCtlProc(int n3DMIXCTLCREATE_ID, int nInstance);
    stEvtMixCtlProc *GetEvtMixCtlProc(int nEVENTID, int nInstance);
    stSubMixChProc *GetSubMixChProc(int nMIXCHIN_ID, int nInstance);
    stMasterMixChProc *GetMasterMixChProc(int nMIXCHIN_ID, int nInstance);
    NFSMixMapState *GetMixMapProcAddress();

    void InitializeSubChannels();
    void InitializeMasterChannels();

    int GetNumStateCopies() {
        return this->m_NumStateCopies;
    } // Decl: 153
    // int GetNumCurveProcsAdded() {}     // Decl: 154
    int GetNumMixCtlsAdded() {
        return this->m_MixCtlsAdded;
    } // Decl: 155
    int GetNumEvtMixCtlsAdded() {
        return this->m_EvtMixCtlsAdded;
    } // Decl: 156
    int GetNum3DMixCtlsAdded() {
        return this->m_3DMixCtlsAdded;
    } // Decl: 157
    int GetNumSubMixChannelsAdded() {
        return this->m_SubMixChannelsAdded;
    } // Decl: 158
    int GetNumMasterChannelsAdded() {
        return this->m_MasterChannelsAdded;
    } // Decl: 159

    void SetStateHdr(stMixMapStateHdr *phdr) {
        this->m_pMMStateHdr = phdr;
    } // Decl: 161

    void SetFirstStateInst(NFSMixMapState *pstate);

    int GetStateRefCount();

    void CreateMixCtls();
    void Create3DMixCtls();
    void CreateEvtMixCtls();
    void CreateSubMixChannels();
    void CreateMasterMixChannels();

    void ProcessMixMap(float t);

    void UpdateState(float t);

    NFSMixMap *m_pNFSMixMap; // offset 0x0, size 0x4, Decl: 176

    stMixStateParams m_MixStateParams; // offset 0x4, size 0x14, Decl: 178

    stMixMapStateHdr *m_pMMStateHdr; // offset 0x18, size 0x4, Decl: 186
    stMixCtlHdr *m_pMixCtlHdr;       // offset 0x1C, size 0x4, Decl: 187
    st3DMixCtlHdr *m_p3DMixCtlHdr;   // offset 0x20, size 0x4, Decl: 188
    stMixEventHdr *m_pEvtMixCtlHdr;  // offset 0x24, size 0x4, Decl: 189
    stMixChHdr *m_pMixChHdr;         // offset 0x28, size 0x4, Decl: 190
    stMixChHdr *m_pSubChHdr;         // offset 0x2C, size 0x4, Decl: 191

    int *m_pChOutArrays; // offset 0x30, size 0x4, Decl: 195

    int m_StateIndex;      // offset 0x34, size 0x4, Decl: 197
    int m_ThisStateRefCnt; // offset 0x38, size 0x4, Decl: 198
    int m_ObjectIndex;     // offset 0x3C, size 0x4, Decl: 199

    int m_NumStateCopies; // offset 0x40, size 0x4, Decl: 201

    int m_MixCtlsAdded;               // offset 0x44, size 0x4, Decl: 205
    int m_3DMixCtlsAdded;             // offset 0x48, size 0x4, Decl: 206
    int m_EvtMixCtlsAdded;            // offset 0x4C, size 0x4, Decl: 207
    int m_SubMixChannelsAdded;        // offset 0x50, size 0x4, Decl: 208
    int m_MasterChannelsAdded;        // offset 0x54, size 0x4, Decl: 209
    NFSMixMapState *m_pFirstInstance; // offset 0x58, size 0x4, Decl: 210
};

#endif
