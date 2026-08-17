#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMap.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/SndBase.hpp"
#include <new>

char *szMixMapFiles[4] = {
    "SOUND\\MixMaps\\MAPOUTPUT.mxb",
    "SOUND\\MixMaps\\MAPOUTPUTDRG.mxb",
    "SOUND\\MixMaps\\MAPOUTPUT2CR.mxb",
    "SOUND\\MixMaps\\MAPOUTPUT2DR.mxb",
};
char *szMainMapStates[13] = {"eMM_MAIN", "eMM_PLAYERCAR", "eMM_AIRACECAR", "eMM_TRAFFIC"};

int DEBUG_EVTMIXCTL; // size: 0x4, address: 0xFFFFFFFF, Decl: 35

int DEBUG_MIXER_BAIL = 0;    // size: 0x4, address: 0xFFFFFFFF, Decl: 37
int nDBGP_DYNMIX = 0;        // size: 0x4, address: 0xFFFFFFFF, Decl: 38
int nDBGP_DYNMIX_SUBS = 0;   // size: 0x4, address: 0xFFFFFFFF, Decl: 39
int nUnityMixScale = 0x7FFF; // size: 0x4, address: 0xFFFFFFFF, Decl: 40

int g_DMIX_DummyOutputBlock[16] = {}; // size: 0x40, address: 0x80418990, Decl: 42
int g_DMIX_DummyInputBlock[16] = {};  // size: 0x40, Decl: 43

int *(*NFSMixMap::mGetOutPtrCB)(int) = nullptr;
void (*NFSMixMap::mSetSFXOutCB)(int, int *) = nullptr;
bool (*NFSMixMap::mSetSFXInCB)(int, int *) = nullptr;
int (*NFSMixMap::mGetStateRefCnt)(int) = nullptr;
void (*NFSMixMap::mMapReadyCB)() = nullptr;

NFSMixMap::NFSMixMap() : AudioMemBase() {
    this->m_pStateProcs = nullptr;
    this->m_fDeltaTimeRatio[1] = 0.0f;
    this->m_nStateMapCount = 0;
    this->m_fDeltaTimeRatio[0] = 0.0f;
}

NFSMixMap::~NFSMixMap() {}

void NFSMixMap::DestroyMainMixMap() {
    this->FreeMemory(this->m_pStateProcs);
    this->m_pStateProcs = nullptr;
    this->FreeMemory(this->m_pMasterChannelInputs);
    this->m_pMasterChannelInputs = nullptr;
    this->FreeMemory(this->m_pSubChannelInputs);
    this->m_pSubChannelInputs = nullptr;
    this->FreeMemory(this->m_pMasterChannelOutputArrayBlock);
    this->m_pMasterChannelOutputArrayBlock = nullptr;
    this->FreeMemory(this->m_pStateProcMemBlock);
    this->m_pStateProcMemBlock = nullptr;
    this->FreeMemory(this->m_pScalePtrArray);
    this->m_pScalePtrArray = nullptr;
    this->FreeMemory(this->m_pCurveDataArray);
    this->m_pCurveDataArray = nullptr;
    this->FreeMemory(this->m_pMixCtlData_S);
    this->m_pMixCtlData_S = nullptr;
    this->FreeMemory(this->m_pMixCtlData_U);
    this->m_pMixCtlData_U = nullptr;
    this->FreeMemory(this->m_pMixCtlProc);
    this->m_pMixCtlProc = nullptr;
    this->FreeMemory(this->m_pSubChData_S);
    this->m_pSubChData_S = nullptr;
    this->FreeMemory(this->m_pSubChData_U);
    this->m_pSubChData_U = nullptr;
    this->FreeMemory(this->m_pSubChProc);
    this->m_pSubChProc = nullptr;
    this->FreeMemory(this->m_pMasterChData_S);
    this->m_pMasterChData_S = nullptr;
    this->FreeMemory(this->m_pMasterChData_U);
    this->m_pMasterChData_U = nullptr;
    this->FreeMemory(this->m_pMasterChProc);
    this->m_pMasterChProc = nullptr;
    this->FreeMemory(this->m_p3DMixCtlData_S);
    this->m_p3DMixCtlData_S = nullptr;
    this->FreeMemory(this->m_p3DMixCtlData_U);
    this->m_p3DMixCtlData_U = nullptr;
    this->FreeMemory(this->m_p3DMixCtlProc);
    this->m_p3DMixCtlProc = nullptr;
    this->FreeMemory(this->m_pEvtMixCtlData_S);
    this->m_pEvtMixCtlData_S = nullptr;
    this->FreeMemory(this->m_pEvtMixCtlData_U);
    this->m_pEvtMixCtlData_U = nullptr;
    this->FreeMemory(this->m_pEvtMixCtlProc);
    this->m_pEvtMixCtlProc = nullptr;
    this->FreeMemory(this->m_pDynMixInputBlocks);
    this->m_pDynMixInputBlocks = nullptr;
}

void NFSMixMap::AssignSFXCallbacks(int *(*getptrcb)(int), void (*setsfxoutcb)(int, int *), bool (*setsfxincb)(int, int *), int (*getstaterefcnt)(int),
                                   void (*mapreadycb)()) {
    mGetOutPtrCB = getptrcb;
    mSetSFXOutCB = setsfxoutcb;
    mSetSFXInCB = setsfxincb;
    mGetStateRefCnt = getstaterefcnt;
    mMapReadyCB = mapreadycb;
}

bool NFSMixMap::SETSFXID(int id, int *ptr) {
    return (*this->mSetSFXInCB)(id, ptr);
}

void NFSMixMap::InitMixMap(int *pmixmap, NFSMixMap *pMasterMixMap) {
    this->m_pMasterMixMap = pMasterMixMap;
    this->m_pMixMap = pmixmap;
    this->m_pMMHdr = reinterpret_cast<stMixMapHeader *>(pmixmap);
    this->m_MapType = *pmixmap;

    int n = 0;
    for (; n < m_pMMHdr->NumStates; n++) {
        m_StateRefCount[n] = 0;
    }

    this->PreProcessMixMap();
}

void NFSMixMap::ResetMapData() {
    this->m_nAssignedMixMapStates = 0;
    this->m_MixCtlsAdded = 0;
    this->m_SharedMixCtlCount = 0;
    this->m_nAssignedMixCtlProc = 0;
    this->m_AssignedMixCtlsShared = 0;
    this->m_AssignedMixCtlsUnique = 0;
    this->m_ScaleParamsAdded = 0;
    this->m_ScaleParamsIDCount = 0;

    for (int n = 0; n < 10; n++) {
        this->m_CurveProcsTotal[n][0] = 0;
        this->m_CurveProcsTotal[n][1] = 0;
    }

    this->m_3DMixCtlsAdded = 0;
    this->m_EventCtlsAdded = 0;
    this->m_nAssignedInputBlocks = 0;
    this->m_nAssigned3DMixCtlProc = 0;
    this->m_nAssigned3DMixCtlShared = 0;
    this->m_nAssigned3DMixCtlUnique = 0;
    this->m_nAssignedEvtMixCtlProc = 0;
    this->m_nAssignedEvtMixCtlShared = 0;
    this->m_nAssignedEvtMixCtlUnique = 0;
    this->m_PrevCamState = DMIX_DEFAULT_CAM;
    this->m_CurCamState = DMIX_DEFAULT_CAM;
    this->m_Shared3DMixCtlCount = 0;
    this->m_SharedEvtMixCtlCount = 0;
    this->m_SubMixChannelsAdded = 0;
    this->m_SharedSubMixCount = 0;
    this->m_nAssignedSubMixProc = 0;
    this->m_nAssignedSubMixShared = 0;
    this->m_nAssignedSubMixUnique = 0;
    this->m_MasterChannelsAdded = 0;
    this->m_SharedMasterMixCount = 0;
    this->m_nAssignedMasterMixProc = 0;
    this->m_nAssignedMasterMixShared = 0;
    this->m_nAssignedMasterMixUnique = 0;
    this->m_CurrentStateProcBlockOffset = 0;
    this->m_nTotalMasterChannelInputs = 0;
    this->m_CurrentMasterInputBlockOffset = 0;
    this->m_CurrentSubInputBlockOffset = 0;
    this->m_CurrentMasterOutputBlockOffset = 0;
    this->m_CurrentMasterChannelPtrBlockOffset = 0;
    this->m_CurrentSubChannelPtrBlockOffset = 0;
    this->m_Current3DMixCtlPtrBlockOffset = 0;
    this->m_CurrentEvtMixCtlPtrBlockOffset = 0;
    this->m_nTotalMasterChannel3DOutputs = 0;
    this->m_nTotalSubChannelInputs = 0;
    this->m_nTotalSubChannel3DOutputs = 0;
    this->m_nTotalUniqueMasterChannels = 0;
    this->m_SFXOBJsAdded = 0;
    this->m_SFXCTLsAdded = 0;
    this->m_DataProcsAdded = 0;
    this->m_n3DCamStatesAdded = 0;
    this->m_SharedMixCtlsAssigned = 0;
    this->m_UniqueMixCtlsAssigned = 0;
    this->m_CurveProcsAdded = 0;
    this->m_CurrentMasterInputOffset = 0;
    this->m_CurrentSubInputOffset = 0;
    this->m_pStateProcMemBlock = nullptr;
    this->m_pDynMixInputBlocks = nullptr;
    this->m_pScalePtrArray = nullptr;
    this->m_pCurveDataArray = nullptr;
    this->m_pMixCtlData_S = nullptr;
    this->m_pMixCtlData_U = nullptr;
    this->m_pMixCtlProc = nullptr;
    this->m_pEvtMixCtlProc = nullptr;
    this->m_pEvtMixCtlData_S = nullptr;
    this->m_pEvtMixCtlData_U = nullptr;
    this->m_p3DMixCtlProc = nullptr;
    this->m_p3DMixCtlData_S = nullptr;
    this->m_p3DMixCtlData_U = nullptr;
    this->m_pSubChData_S = nullptr;
    this->m_pSubChData_U = nullptr;
    this->m_pSubChProc = nullptr;
    this->m_pMasterChData_S = nullptr;
    this->m_pMasterChData_U = nullptr;
    this->m_pMasterChProc = nullptr;
    this->m_pMasterChannelInputs = nullptr;
    this->m_pSubChannelInputs = nullptr;
    this->m_pMasterChannelOutputArrayBlock = nullptr;
}

void NFSMixMap::SetupStateRefCount() {
    int ntotalstateprocs;
    int *pStateOffsetTable = reinterpret_cast<int *>(reinterpret_cast<char *>(this->m_pMMHdr) + this->m_pMMHdr->StateTableOffset);

    for (int nst = 0; nst < this->m_pMMHdr->NumStates; pStateOffsetTable++, nst++) {
        this->m_StateRefCount[nst] = 0;
        if (*pStateOffsetTable != -1) {
            this->m_StateRefCount[nst] = (*this->mGetStateRefCnt)(nst);
        }
    }
}

// TODO dwarf wrong
void NFSMixMap::PreProcessMixMap() {
    stMixMapHeader *pmmhdr = reinterpret_cast<stMixMapHeader *>(this->m_pMixMap);
    int *pStateOffsetTable = reinterpret_cast<int *>(reinterpret_cast<char *>(pmmhdr) + pmmhdr->StateTableOffset);

    this->ResetMapData();
    this->SetupStateRefCount();
    this->m_nStateMapCount = 0;

    int ntotalcurveprocs;
    int n;
    for (n = 0; n < pmmhdr->NumStates; pStateOffsetTable++) {
        int ns = n + 1;

        if (*pStateOffsetTable != -1) {
            stMixMapStateHdr *pmmsthdr;
            stMixCtlHdr *pmctlhdr;
            st3DMixCtlHdr *p3Dmctlhdr;
            stMixEventHdr *pevtctlhdr;
            stMixChHdr *pmsubchhdr;
            stMixChHdr *pmasterchhdr;
            int numScaleParams;
            int numSFXOBJIDs;
            int numSFXCTLIDs;
            int num3DSFXCTLIDs;

            pmmsthdr = reinterpret_cast<stMixMapStateHdr *>(*pStateOffsetTable + reinterpret_cast<int>(pmmhdr));
            this->m_nStateMapCount += this->m_StateRefCount[n];

            pmctlhdr = nullptr;
            if (pmmsthdr->OffsetMixCtlData != -1) {
                pmctlhdr = reinterpret_cast<stMixCtlHdr *>(pmmsthdr->OffsetMixCtlData + reinterpret_cast<int>(pmmsthdr));
            }

            if (pmctlhdr == nullptr) {
                numScaleParams = 0;
            }

            if (pmmsthdr->OffsetMixCtlData != -1) {
                int *InputIDs = static_cast<int *>(this->AllocateMemory(pmctlhdr->NumMixCtls << 2, "Temp MIXMAP ALLOC"));
                stMixCtlHdr *pmctlhdr = reinterpret_cast<stMixCtlHdr *>(pmmsthdr->OffsetMixCtlData + reinterpret_cast<int>(pmmsthdr));
                stMixCtlParams *pctlparms = reinterpret_cast<stMixCtlParams *>(&pmctlhdr[1]);

                this->m_MixCtlsAdded += this->m_StateRefCount[n] * pmctlhdr->NumMixCtls;
                this->m_SharedMixCtlCount += pmctlhdr->NumMixCtls;
                this->m_DataProcsAdded += this->m_StateRefCount[n] * pmctlhdr->NumNewMixDataProcs;

                for (int nctl = 0; nctl < pmctlhdr->NumMixCtls; nctl++) {
                    int ncurve = (pctlparms->nINPUTID >> 24) & 0xF;
                    int nscale = (pctlparms->nUScaleCntSwing >> 16) & 0xF;
                    bool bunique = true;
                    bool buniquesfx = true;

                    InputIDs[nctl] = pctlparms->nINPUTID;

                    for (int ntest = 0; ntest < nctl; ntest++) {
                        if (InputIDs[ntest] == pctlparms->nINPUTID) {
                            bunique = false;
                        }

                        int sfxid = pctlparms->nINPUTID & 0xE0FFFFF0;
                        int idtest = InputIDs[ntest] & 0xE0FFFFF0;

                        if (sfxid == idtest) {
                            buniquesfx = false;
                        }
                    }

                    if (bunique == true) {
                        this->m_CurveProcsTotal[ncurve][0] += this->m_StateRefCount[n];
                    }

                    if (buniquesfx) {
                        switch (pctlparms->nINPUTID & 0xE0000000) {
                            case 0x40000000:
                                numSFXOBJIDs++;
                                break;
                            case 0x60000000:
                                numSFXCTLIDs++;
                                break;
                            case 0x80000000:
                                num3DSFXCTLIDs++;
                                break;
                        }
                    }

                    int *pscaleid = reinterpret_cast<int *>(&pctlparms[1]);
                    int ntotaladded = 0;

                    for (int s = 0; s < nscale; s++) {
                        int ID = *pscaleid;
                        int state = (ID >> 16) & 0xFF;

                        if (state != n) {
                            ntotaladded += this->m_StateRefCount[state];
                        } else {
                            ntotaladded++;
                        }

                        pscaleid++;
                    }

                    this->m_ScaleParamsAdded += ntotaladded * this->m_StateRefCount[n];
                    pctlparms =
                        reinterpret_cast<stMixCtlParams *>(reinterpret_cast<char *>(pctlparms) + sizeof(stMixCtlParams) + nscale * sizeof(int));
                }

                this->FreeMemory(InputIDs);
            }

            if (pmmsthdr->Offset3DMixCtlData != -1) {
                st3DMixCtlHdr *p3Dmctlhdr = reinterpret_cast<st3DMixCtlHdr *>(pmmsthdr->Offset3DMixCtlData + reinterpret_cast<int>(pmmsthdr));
                int num3d = p3Dmctlhdr->Num3DMixCtls & 0xFF;
                int *p3DMixCtlData = reinterpret_cast<int *>(&p3Dmctlhdr[1]);
                int nTotal3DCamStates = 0;

                this->m_3DMixCtlsAdded += this->m_StateRefCount[n] * num3d;
                this->m_Shared3DMixCtlCount += num3d;

                for (int j = 0; j < p3Dmctlhdr->Num3DMixCtls; j++) {
                    int n3DCamStates = (*p3DMixCtlData >> 24) & 0xF;

                    nTotal3DCamStates += n3DCamStates;
                    p3DMixCtlData += n3DCamStates * 6;
                }

                this->m_n3DCamStatesAdded += nTotal3DCamStates;
            }

            if (pmmsthdr->OffsetEventCtlData != -1) {
                stMixEventHdr *pevtctlhdr = reinterpret_cast<stMixEventHdr *>(pmmsthdr->OffsetEventCtlData + reinterpret_cast<int>(pmmsthdr));
                stMixEvtParams *pevtctlparms = reinterpret_cast<stMixEvtParams *>(&pevtctlhdr[1]);
                int ntotaladded = 0;

                this->m_EventCtlsAdded += this->m_StateRefCount[n] * pevtctlhdr->NumEvents;
                this->m_SharedEvtMixCtlCount += pevtctlhdr->NumEvents;

                for (int nctl = 0; nctl < pevtctlhdr->NumEvents; nctl++) {
                    int nscale = (pevtctlparms->nUScaleCntSwing >> 16) & 0xF;
                    int *pscaleid = reinterpret_cast<int *>(&pevtctlparms[1]);

                    for (int s = 0; s < nscale; s++) {
                        int ID = *pscaleid;
                        int state = (ID >> 16) & 0xFF;

                        if (state != n) {
                            ntotaladded += this->m_StateRefCount[state];
                        } else {
                            ntotaladded++;
                        }

                        pscaleid++;
                    }

                    pevtctlparms =
                        reinterpret_cast<stMixEvtParams *>(reinterpret_cast<char *>(pevtctlparms) + sizeof(stMixEvtParams) + nscale * sizeof(int));
                }

                this->m_ScaleParamsAdded += ntotaladded * this->m_StateRefCount[n];
            }

            if (pmmsthdr->OffsetSubMixData != -1) {
                stMixChHdr *pmsubchhdr = reinterpret_cast<stMixChHdr *>(pmmsthdr->OffsetSubMixData + reinterpret_cast<int>(pmmsthdr));
                int *pSubChData = reinterpret_cast<int *>(&pmsubchhdr[1]);

                this->m_SharedSubMixCount += pmsubchhdr->NumMixChannels;
                this->m_SubMixChannelsAdded += this->m_StateRefCount[n] * pmsubchhdr->NumMixChannels;

                for (int nsc = 0; nsc < pmsubchhdr->NumMixChannels; nsc++) {
                    int nMIXCHID = *pSubChData;
                    int nChType = (nMIXCHID >> 24) & 0xF;
                    int numinputs = (nMIXCHID >> 16) & 0xFF;
                    int ntotalchannelinputs = 0;

                    pSubChData += 2;

                    for (int nin = 0; nin < numinputs; nin++) {
                        int ninputid = *pSubChData;
                        int nstate = (ninputid >> 16) & 0xFF;

                        if (nstate != n) {
                            ntotalchannelinputs += this->m_StateRefCount[nstate];
                        } else {
                            ntotalchannelinputs++;
                        }

                        pSubChData++;
                    }

                    this->m_nTotalSubChannelInputs += ntotalchannelinputs * this->m_StateRefCount[n];
                }
            }

            if (pmmsthdr->OffsetMasterMixData != -1) {
                stMixChHdr *pmasterchhdr = reinterpret_cast<stMixChHdr *>(pmmsthdr->OffsetMasterMixData + reinterpret_cast<int>(pmmsthdr));
                int *pMstChData = reinterpret_cast<int *>(&pmasterchhdr[1]);

                this->m_nTotalUniqueMasterChannels += pmasterchhdr->NumUniqueSFXOBJs + (this->m_StateRefCount[n] * pmasterchhdr->NumUniqueSFXOBJs);
                this->m_SharedMasterMixCount += pmasterchhdr->NumMixChannels;
                this->m_MasterChannelsAdded += this->m_StateRefCount[n] * pmasterchhdr->NumMixChannels;

                for (int nmc = 0; nmc < pmasterchhdr->NumMixChannels; nmc++) {
                    int nMIXCHID = *pMstChData;
                    int nChType = (nMIXCHID >> 24) & 0xF;
                    int numinputs = (nMIXCHID >> 16) & 0xFF;
                    int ntotalchannelinputs = 0;

                    pMstChData += 3;

                    for (int nin = 0; nin < numinputs; nin++) {
                        int ninputid = *pMstChData;
                        int nstate = (ninputid >> 16) & 0xFF;

                        if (nstate != n) {
                            ntotalchannelinputs += this->m_StateRefCount[nstate];
                        } else {
                            ntotalchannelinputs++;
                        }

                        pMstChData++;
                    }

                    this->m_nTotalMasterChannelInputs += ntotalchannelinputs * this->m_StateRefCount[n];
                }
            }
        }

        n = ns;
    }

    for (ntotalcurveprocs = 0; ntotalcurveprocs < 10; ntotalcurveprocs++) {
        this->m_CurveProcsAdded += this->m_CurveProcsTotal[ntotalcurveprocs][0];
    }
}

// STRIPPED
void NFSMixMap::TestSETAllocationParams() {}

void NFSMixMap::AllocateMixerMemory() {
    if (this->m_pStateProcs == nullptr) {
        this->m_pStateProcs =
            static_cast<NFSMixMapState **>(this->AllocateMemory(this->m_pMMHdr->NumStates * sizeof(*this->m_pStateProcs), "Dyn Mix Proc Array"));
    }

    int n;
    for (n = 0; n < this->m_pMMHdr->NumStates; n++) {
        this->m_pStateProcs[n] = nullptr;
    }

    this->m_pMasterChannelInputs = static_cast<int *>(
        this->AllocateMemory(this->m_nTotalMasterChannelInputs * sizeof(*this->m_pMasterChannelInputs), "Master Channel Input Array Block"));

    this->m_pSubChannelInputs = static_cast<int *>(
        this->AllocateMemory(this->m_nTotalSubChannelInputs * sizeof(*this->m_pSubChannelInputs), "Sub Channel Input Array Block"));

    this->m_pMasterChannelOutputArrayBlock = static_cast<int *>(this->AllocateMemory(
        this->m_nTotalUniqueMasterChannels * sizeof(*this->m_pMasterChannelOutputArrayBlock) * 16, "Master Channel Output Array Block"));

    NFSMixMapState *pmem =
        static_cast<NFSMixMapState *>(this->AllocateMemory(this->m_nStateMapCount * sizeof(NFSMixMapState), "NFSMixMapState Object Memory"));

    this->m_pStateProcMemBlock = reinterpret_cast<NFSMixMapState **>(pmem);

    for (n = 0; n < this->m_nStateMapCount; n++) {
        NFSMixMapState *ps = reinterpret_cast<NFSMixMapState *>(reinterpret_cast<char *>(this->m_pStateProcMemBlock) + (n * sizeof(NFSMixMapState)));
        new (ps) NFSMixMapState();
    }

    this->m_pScalePtrArray =
        static_cast<int **>(this->AllocateMemory(this->m_ScaleParamsAdded * sizeof(*this->m_pScalePtrArray), "Scale Input Ptr Array Block"));

    this->m_pCurveDataArray =
        static_cast<stCurveDataProc *>(this->AllocateMemory(this->m_CurveProcsAdded * sizeof(*this->m_pCurveDataArray), "Curve Proc Data Array"));

    this->m_pMixCtlData_S = static_cast<stMixCtlSharedData *>(
        this->AllocateMemory(this->m_SharedMixCtlCount * sizeof(*this->m_pMixCtlData_S), "NFSMixCtl Shared Data Array"));

    this->m_pMixCtlData_U =
        static_cast<stMixCtlUniqueData *>(this->AllocateMemory(this->m_MixCtlsAdded * sizeof(*this->m_pMixCtlData_U), "NFSMixCtl Unique Data Array"));

    this->m_pMixCtlProc =
        static_cast<stMixCtlProc *>(this->AllocateMemory(this->m_MixCtlsAdded * sizeof(*this->m_pMixCtlProc), "NFSMixCtl Process Data Array"));

    this->m_pSubChData_S =
        static_cast<stMixChSharedData *>(this->AllocateMemory(this->m_SharedSubMixCount * sizeof(*this->m_pSubChData_S), "SubMix Shared Data Block"));

    this->m_pSubChData_U = static_cast<stMixChUniqueData *>(
        this->AllocateMemory(this->m_SubMixChannelsAdded * sizeof(*this->m_pSubChData_U), "SubMix Unique Data Block"));

    this->m_pSubChProc =
        static_cast<stSubMixChProc *>(this->AllocateMemory(this->m_SubMixChannelsAdded * sizeof(*this->m_pSubChProc), "SubMix Proc Data Block"));

    this->m_pMasterChData_S = static_cast<stMasterMixChSharedData *>(
        this->AllocateMemory(this->m_SharedMasterMixCount * sizeof(*this->m_pMasterChData_S), "MasterMix Shared Data Block"));

    this->m_pMasterChData_U = static_cast<stMasterMixChUniqueData *>(
        this->AllocateMemory(this->m_MasterChannelsAdded * sizeof(*this->m_pMasterChData_U), "MasterMix Unique Data Block"));

    this->m_pMasterChProc = static_cast<stMasterMixChProc *>(
        this->AllocateMemory(this->m_MasterChannelsAdded * sizeof(*this->m_pMasterChProc), "MasterMix Proc Data Block"));

    this->m_p3DMixCtlData_S = static_cast<st3DMixCtlSharedData *>(
        this->AllocateMemory(this->m_Shared3DMixCtlCount * sizeof(*this->m_p3DMixCtlData_S), "3DMixCtl Shared Data Block"));

    this->m_p3DMixCtlData_U = static_cast<st3DMixCtlUniqueData *>(
        this->AllocateMemory(this->m_3DMixCtlsAdded * sizeof(*this->m_p3DMixCtlData_U), "3DMixCtl Unique Data Block"));

    this->m_p3DMixCtlProc =
        static_cast<st3DMixCtlProc *>(this->AllocateMemory(this->m_3DMixCtlsAdded * sizeof(*this->m_p3DMixCtlProc), "3DMixCtl Proc Data Block"));

    this->m_pEvtMixCtlData_S = static_cast<stEvtMixCtlSharedData *>(
        this->AllocateMemory(this->m_SharedEvtMixCtlCount * sizeof(*this->m_pEvtMixCtlData_S), "EvtMixCtl Shared Data Block"));

    this->m_pEvtMixCtlData_U = static_cast<stEvtMixCtlUniqueData *>(
        this->AllocateMemory(this->m_EventCtlsAdded * sizeof(*this->m_pEvtMixCtlData_U), "EvtMixCtl Unique Data Block"));

    this->m_pEvtMixCtlProc =
        static_cast<stEvtMixCtlProc *>(this->AllocateMemory(this->m_EventCtlsAdded * sizeof(*this->m_pEvtMixCtlProc), "EvtMixCtl Proc Data Block"));
}

int *NFSMixMap::GetNextInputBlock(bool bincrement) {
    int *pAddr = reinterpret_cast<int *>(m_pDynMixInputBlocks) + (m_nAssignedInputBlocks * 16);
    if (bincrement) {
        m_nAssignedInputBlocks++;
    }

    int *pclear = pAddr;
    for (int n = 0; n < 16; n++) {
        *pclear++ = 0;
    }

    return pAddr;
}

stEvtMixCtlProc *NFSMixMap::GetNextEvtMixCtlProc(bool bincrement) {
    stEvtMixCtlProc *pAddr = &this->m_pEvtMixCtlProc[this->m_nAssignedEvtMixCtlProc];
    if (bincrement) {
        this->m_nAssignedEvtMixCtlProc++;
    }

    return pAddr;
}

stEvtMixCtlSharedData *NFSMixMap::GetNextEvtMixCtlShared(bool bincrement) {
    stEvtMixCtlSharedData *pAddr = &this->m_pEvtMixCtlData_S[this->m_nAssignedEvtMixCtlShared];
    if (bincrement) {
        this->m_nAssignedEvtMixCtlShared++;
    }

    return pAddr;
}

stEvtMixCtlUniqueData *NFSMixMap::GetNextEvtMixCtlUnique(bool bincrement) {
    stEvtMixCtlUniqueData *pAddr = &this->m_pEvtMixCtlData_U[this->m_nAssignedEvtMixCtlUnique];
    pAddr->qoutput = 0x7FFF;
    pAddr->output = 0;
    pAddr->reset = 0;
    pAddr->reset_level = -10000;
    if (bincrement) {
        this->m_nAssignedEvtMixCtlUnique++;
    }

    return pAddr;
}

st3DMixCtlProc *NFSMixMap::GetNext3DMixCtlProc(bool bincrement) {
    st3DMixCtlProc *pAddr = &this->m_p3DMixCtlProc[this->m_nAssigned3DMixCtlProc];
    if (bincrement) {
        this->m_nAssigned3DMixCtlProc++;
    }

    return pAddr;
}

st3DMixCtlSharedData *NFSMixMap::GetNext3DMixCtlShared(bool bincrement) {
    st3DMixCtlSharedData *pAddr = &this->m_p3DMixCtlData_S[this->m_nAssigned3DMixCtlShared];
    if (bincrement) {
        this->m_nAssigned3DMixCtlShared++;
    }

    return pAddr;
}

st3DMixCtlUniqueData *NFSMixMap::GetNext3DMixCtlUnique(bool bincrement) {
    st3DMixCtlUniqueData *pAddr = &this->m_p3DMixCtlData_U[this->m_nAssigned3DMixCtlUnique];
    if (bincrement) {
        this->m_nAssigned3DMixCtlUnique++;
    }

    return pAddr;
}

stMasterMixChProc *NFSMixMap::GetNextMasterMixProc(bool bincrement) {
    stMasterMixChProc *pAddr = &this->m_pMasterChProc[this->m_nAssignedMasterMixProc];
    if (bincrement) {
        this->m_nAssignedMasterMixProc++;
    }

    return pAddr;
}

stMasterMixChSharedData *NFSMixMap::GetNextMasterMixShared(bool bincrement) {
    stMasterMixChSharedData *pAddr = &this->m_pMasterChData_S[this->m_nAssignedMasterMixShared];
    if (bincrement) {
        this->m_nAssignedMasterMixShared++;
    }

    return pAddr;
}

stMasterMixChUniqueData *NFSMixMap::GetNextMasterMixUnique(bool bincrement) {
    stMasterMixChUniqueData *pAddr = &this->m_pMasterChData_U[this->m_nAssignedMasterMixUnique];
    if (bincrement) {
        this->m_nAssignedMasterMixUnique++;
    }

    return pAddr;
}

stSubMixChProc *NFSMixMap::GetNextSubMixProc(bool bincrement) {
    stSubMixChProc *pAddr = &this->m_pSubChProc[this->m_nAssignedSubMixProc];
    if (bincrement) {
        this->m_nAssignedSubMixProc++;
    }

    return pAddr;
}

stMixChUniqueData *NFSMixMap::GetNextSubMixUnique(bool bincrement) {
    stMixChUniqueData *pAddr = &this->m_pSubChData_U[this->m_nAssignedSubMixUnique];
    if (bincrement) {
        this->m_nAssignedSubMixUnique++;
    }

    return pAddr;
}

stMixChSharedData *NFSMixMap::GetNextSubMixShared(bool bincrement) {
    stMixChSharedData *pAddr = &this->m_pSubChData_S[this->m_nAssignedSubMixShared];
    if (bincrement) {
        this->m_nAssignedSubMixShared++;
    }

    return pAddr;
}

NFSMixMapState *NFSMixMap::GetNextMapState(bool bincrement) {
    NFSMixMapState *pAddr =
        reinterpret_cast<NFSMixMapState *>(reinterpret_cast<char *>(this->m_pStateProcMemBlock) + this->m_CurrentStateProcBlockOffset);

    if (bincrement) {
        this->m_CurrentStateProcBlockOffset += sizeof(NFSMixMapState);
    }

    return pAddr;
}

stCurveDataProc *NFSMixMap::GetCurveDataPtr(stMixCtlParams *pparams) {
    if (this->m_pCurveDataArray == nullptr) {
        return nullptr;
    }

    int ncurve = (pparams->nINPUTID >> 24) & 0xF;
    int noffset = 0;
    for (int m = 0; m < ncurve; m++) {
        noffset += this->m_CurveProcsTotal[m][0];
    }

    stCurveDataProc *pcdp = this->m_pCurveDataArray + noffset;
    int n;
    for (n = 0; n < this->m_CurveProcsTotal[ncurve][1]; n++) {
        if (pcdp->nINPUTID == pparams->nINPUTID) {
            return pcdp;
        }
        pcdp++;
    }

    this->m_CurveProcsTotal[ncurve][1]++;
    pcdp->nINPUTID = pparams->nINPUTID;
    pcdp->pInputParam = nullptr;
    pcdp->dBOutput = 0;
    pcdp->Q15Output = 0x7FFF;
    return pcdp;
}

int *NFSMixMap::AddScaleIDs(stMixCtlParams *pmixctl, int instance) {
    int numscale = (pmixctl->nUScaleCntSwing >> 16) & 0x1F;
    if (numscale == 0) {
        return nullptr;
    }

    int *pIDs = &pmixctl[1].nINPUTID;
    int *paddr = reinterpret_cast<int *>(&this->m_pScalePtrArray[this->m_ScaleParamsIDCount]);
    int ntotaladded = 0;

    for (int n = 0; n < numscale; n++) {
        int ID = *pIDs++;
        int state = (ID >> 16) & 0xFF;
        int selfstate = (pmixctl->nINPUTID >> 16) & 0xFF;

        if (state != selfstate) {
            for (int m = 0; m < this->m_StateRefCount[state]; m++) {
                int *pfill = reinterpret_cast<int *>(&this->m_pScalePtrArray[n + m + this->m_ScaleParamsIDCount]);
                *pfill = ID | (m << 11);
                ntotaladded++;
            }
        } else {
            int *pfill = reinterpret_cast<int *>(&this->m_pScalePtrArray[n + this->m_ScaleParamsIDCount]);
            *pfill = ID | (instance << 11);
            ntotaladded++;
        }
    }

    pmixctl->nUScaleCntSwing = (pmixctl->nUScaleCntSwing & 0x00FFFFFF) | (ntotaladded << 24);
    this->m_ScaleParamsIDCount += ntotaladded;

    return paddr;
}

int *NFSMixMap::AddScaleIDs(stMixEvtParams *pevtmixctl, int instance) {
    if (this->m_ScaleParamsIDCount == this->m_ScaleParamsAdded) {
        return nullptr;
    }

    int numscale = (pevtmixctl->nUScaleCntSwing >> 16) & 0x1F;
    if (numscale == 0) {
        return nullptr;
    }

    int *pIDs = &pevtmixctl[1].nEVTCTLID;
    int *paddr = reinterpret_cast<int *>(&this->m_pScalePtrArray[this->m_ScaleParamsIDCount]);
    int ntotaladded = 0;

    for (int n = 0; n < numscale; n++) {
        int ID = *pIDs++;
        int state = (ID >> 16) & 0xFF;
        int selfstate = (pevtmixctl->nEVTCTLID >> 16) & 0xFF;

        if (state != selfstate) {
            for (int m = 0; m < this->m_StateRefCount[state]; m++) {
                this->m_pScalePtrArray[this->m_ScaleParamsIDCount + n + m] = reinterpret_cast<int *>(ID | (m << 11));
                ntotaladded++;
            }
        } else {
            this->m_pScalePtrArray[this->m_ScaleParamsIDCount + n] = reinterpret_cast<int *>(ID | (instance << 11));
            ntotaladded++;
        }
    }

    pevtmixctl->nUScaleCntSwing = (pevtmixctl->nUScaleCntSwing & 0x00FFFFFF) | (ntotaladded << 24);
    this->m_ScaleParamsIDCount += ntotaladded;

    return paddr;
}

stMixCtlProc *NFSMixMap::GetProcessMixCtlPtr(bool bincrement) {
    stMixCtlProc *pAddr = &this->m_pMixCtlProc[this->m_nAssignedMixCtlProc];
    if (bincrement) {
        this->m_nAssignedMixCtlProc++;
    }

    return pAddr;
}

void NFSMixMap::AssignMixCtlDataPtrs(stMixCtlProc *pmixctl, stMixCtlParams *pparms, int nstateindex, int ctlcount) {
    if (nstateindex == 0) {
        pmixctl->psdata = &this->m_pMixCtlData_S[this->m_AssignedMixCtlsShared];
        this->m_AssignedMixCtlsShared++;
    } else {
        int ncurveid = pparms->nINPUTID & 0x00FF0000;
        int nOBJIDType = (pparms->nINPUTID >> 16) & 0xE000U;
        int nstateid = pparms->nINPUTID & 0x0F000000U;
        int MIXCTLOBJID = nstateid | ncurveid | nOBJIDType | ctlcount | (this->GetMapType() << 8);
        bool bfound = false;

        for (int n = 0; n < this->m_AssignedMixCtlsShared; n++) {
            stMixCtlSharedData *ps = this->m_pMixCtlData_S + n;

            if (ps->MIXCTLOBJID == MIXCTLOBJID) {
                pmixctl->psdata = ps;
                bfound = true;
            }
        }
    }

    pmixctl->pudata = &this->m_pMixCtlData_U[this->m_AssignedMixCtlsUnique];
    this->m_AssignedMixCtlsUnique++;
}

int *NFSMixMap::GetMasterChannelOutputArrayPtr(int nNumChannels) {
    int *pAddr = &this->m_pMasterChannelOutputArrayBlock[this->m_CurrentMasterOutputBlockOffset];
    this->m_CurrentMasterOutputBlockOffset += nNumChannels * 16;
    return pAddr;
}

int *NFSMixMap::GetMasterChannelInputPtr(int nsize) {
    int *pAddr = &this->m_pMasterChannelInputs[this->m_CurrentMasterInputBlockOffset];
    this->m_CurrentMasterInputBlockOffset += nsize;
    return pAddr;
}

int *NFSMixMap::GetSubChannelInputPtr(int nsize) {
    int *pAddr = &this->m_pSubChannelInputs[this->m_CurrentSubInputBlockOffset];
    this->m_CurrentSubInputBlockOffset += nsize;
    return pAddr;
}

int NFSMixMap::GetMapStateCopies(int nstate) {
    if (nstate < this->m_pMMHdr->NumStates) {
        return this->m_StateRefCount[nstate];
    }

    return 0;
}

// STRIPPED
void NFSMixMap::SetupStateRefCountFromGame(int nstate, int ncopy) {}

void NFSMixMap::CreateMainMapState(eMAINMAPSTATES estate, int numstates, int objnum) {
    int refcnt = estate;
    int ntemp = objnum;

    {
        NFSMixMapState *pstates = this->m_pStateProcs[estate];
        if (pstates == nullptr) {
            this->m_pStateProcs[estate] = this->GetNextMapState(true);
            this->m_pStateProcs[estate]->Initialize(this, estate, numstates, ntemp);
        }
    }

    this->m_pStateProcs[estate]->AddMixState(ntemp, this->m_pStateProcs[estate]);

    stMixMapStateHdr *psmhdr = reinterpret_cast<stMixMapStateHdr *>(this->m_pMMHdr);
    int *psmp = reinterpret_cast<int *>(reinterpret_cast<char *>(psmhdr) + this->m_pMMHdr->StateTableOffset);
    // TODO fake match
    int smoffset = *reinterpret_cast<int *>(reinterpret_cast<char *>(psmp) + (refcnt << 2));
    psmhdr = reinterpret_cast<stMixMapStateHdr *>(reinterpret_cast<char *>(psmhdr) + smoffset);

    NFSMixMapState *pmmp = this->m_pStateProcs[estate]->GetMixMapProc(ntemp);
    pmmp->SetStateHdr(psmhdr);
    pmmp->CreateMixCtls();
    pmmp->Create3DMixCtls();
    pmmp->CreateEvtMixCtls();
}

void NFSMixMap::AllocateInputArrays() {
    int ntotalcurveprocs = 0;
    int n;
    stCurveDataProc *pcdp = this->m_pCurveDataArray;
    bool bUniqueCurveID;
    int nTotalUniqueCurveIDs = 0;

    for (int k = n = 0; k < this->m_CurveProcsAdded; k++) {
        int sfxid = pcdp->nINPUTID & 0xE0FFFFF0;
        unsigned int ntype = pcdp->nINPUTID & 0xE0000000;
        bUniqueCurveID = true;

        if (static_cast<unsigned int>(ntype == 0x40000000) | static_cast<unsigned int>(ntype == 0x60000000) |
            static_cast<unsigned int>(ntype == 0x80000000)) {
            stCurveDataProc *ptestcdp = this->m_pCurveDataArray;

            for (int tcv = 0; tcv < k; tcv++) {
                int testID = ptestcdp->nINPUTID & 0xE0FFFFF0;
                if (testID == sfxid) {
                    bUniqueCurveID = false;
                }

                ptestcdp++;
            }
        } else {
            bUniqueCurveID = false;
        }

        pcdp++;

        if (bUniqueCurveID) {
            nTotalUniqueCurveIDs++;
        }
    }

    int ntotaluniqueScaleID = 0;
    int ns = 0;
    int nid;

    for (ns = ntotaluniqueScaleID; ns < this->m_ScaleParamsAdded; ns++) {
        int *psc = *reinterpret_cast<int **>(reinterpret_cast<char *>(this->m_pScalePtrArray) + (ns << 2));
        bool buniquescale = true;
        nid = reinterpret_cast<unsigned int>(psc) & 0xE0FFFFF0;
        unsigned int ntype = reinterpret_cast<unsigned int>(psc) & 0xE0000000;

        if ((ntype == 0x40000000) || (ntype == 0x60000000) || (ntype == 0x80000000)) {
            for (int m = 0; m < ns; m++) {
                int *ptid = *reinterpret_cast<int **>(reinterpret_cast<char *>(this->m_pScalePtrArray) + (m << 2));
                int ntestid = reinterpret_cast<unsigned int>(ptid) & 0xE0FFFFF0;

                if (nid == ntestid) {
                    buniquescale = false;
                }
            }

            if (buniquescale) {
                stCurveDataProc *pcdp = this->m_pCurveDataArray;

                for (int k = 0; k < ntotalcurveprocs; k++) {
                    int sfxid = pcdp->nINPUTID & 0xE0FFFFF0;

                    if (sfxid == nid) {
                        buniquescale = false;
                    }

                    pcdp++;
                }
            }
        } else {
            buniquescale = false;
        }

        if (buniquescale) {
            ntotaluniqueScaleID++;
        }
    }

    int ntotalunique3DID = 0;
    int n3d = 0;

    for (n3d = ntotalunique3DID; n3d < this->m_3DMixCtlsAdded; n3d++) {
        bool bunique3d = true;
        st3DMixCtlProc *p3d = this->m_p3DMixCtlProc + n3d;
        int nID = p3d->p3DMixCtlData_U->nINPUTID & 0xE0FFFFF0;
        st3DMixCtlProc *ptest3d = this->m_p3DMixCtlProc;

        for (int k = 0; k < n3d; k++) {
            int testID = ptest3d->p3DMixCtlData_U->nINPUTID & 0xE0FFFFF0;

            if (nID == testID) {
                bunique3d = false;
            }

            ptest3d++;
        }

        if (bunique3d) {
            for (int s = 0; s < this->m_ScaleParamsAdded; s++) {
                int *ptestsc = *reinterpret_cast<int **>(reinterpret_cast<char *>(this->m_pScalePtrArray) + (s << 2));
                int nsid = reinterpret_cast<unsigned int>(ptestsc) & 0xE0FFFFF0;

                if (nsid == nID) {
                    bunique3d = false;
                }
            }
        }

        if (bunique3d) {
            stCurveDataProc *pcdp = this->m_pCurveDataArray;

            for (int c = 0; c < ntotalcurveprocs; c++) {
                int sfxid = pcdp->nINPUTID & 0xE0FFFFF0;

                if (sfxid == nID) {
                    bunique3d = false;
                }

                pcdp++;
            }
        }

        if (bunique3d) {
            ntotalunique3DID++;
        }
    }

    int ntotaluniqueEvents = 0;

    for (n = 0; n < this->m_pMMHdr->NumStates; n++) {
        if (this->m_pStateProcs[n] != nullptr) {
            int numevtctls = this->m_pStateProcs[n]->m_EvtMixCtlsAdded;

            for (int e = 0; e < numevtctls; e++) {
                stEvtMixCtlProc *pevtproc = this->m_pStateProcs[n]->GetEvtMixCtlProc(e, 0);
                int nID = pevtproc->pData_S->pMapParms->nTriggerID & 0xE0FFFFF0;
                bool buniqueevent = true;

                for (int es = 0; es < e; es++) {
                    stEvtMixCtlProc *ptestproc = this->m_pStateProcs[n]->GetEvtMixCtlProc(es, 0);
                    int testID = ptestproc->pData_S->pMapParms->nTriggerID & 0xE0FFFFF0;

                    if (testID == nID) {
                        buniqueevent = false;
                    }
                }

                if (buniqueevent) {
                    for (int s = 0; s < this->m_ScaleParamsAdded; s++) {
                        int *ptestsc = *reinterpret_cast<int **>(reinterpret_cast<char *>(this->m_pScalePtrArray) + (s << 2));
                        int nsid = reinterpret_cast<unsigned int>(ptestsc) & 0xE0FFFFF0;

                        if (nsid == nID) {
                            buniqueevent = false;
                        }
                    }
                }

                if (buniqueevent) {
                    stCurveDataProc *pcdp = this->m_pCurveDataArray;

                    for (int c = 0; c < ntotalcurveprocs; c++) {
                        int sfxid = pcdp->nINPUTID & 0xE0FFFFF0;

                        if (sfxid == nID) {
                            buniqueevent = false;
                        }

                        pcdp++;
                    }

                    if (buniqueevent) {
                        st3DMixCtlProc *p3d = this->m_p3DMixCtlProc;

                        for (int nt3d = 0; nt3d < this->m_3DMixCtlsAdded; nt3d++) {
                            int ntestID = p3d->p3DMixCtlData_U->nINPUTID;

                            if (ntestID == nID) {
                                buniqueevent = false;
                            }

                            p3d++;
                        }

                        if (buniqueevent) {
                            ntotaluniqueEvents += this->m_pStateProcs[n]->GetStateRefCount();
                        }
                    }
                }
            }
        }
    }

    int ntotalsize = nTotalUniqueCurveIDs + ntotaluniqueScaleID + ntotalunique3DID + ntotaluniqueEvents;
    this->m_pDynMixInputBlocks = static_cast<int **>(this->AllocateMemory(ntotalsize * 16 * sizeof(int), "DMIX SFXOBJ, SFXCTL Input Block"));
}

int DUMMYINPUT = 0; // size: 0x4, address: 0x80418A24, Decl: 2173

int *NFSMixMap::GetObjectPtr(int sfxid, bool busedB, bool bHACKINIT) {
    int *ptr = &DUMMYINPUT;
    int ntype = sfxid & 0xE0000000;

    switch (ntype) {
        case 0: {
            int nstate = (sfxid >> 16) & 0xFF;
            int ninst = (sfxid >> 11) & 0x1F;
            int nidx = sfxid & 0xFF;
            stMixCtlProc *pmxctlproc = this->m_pStateProcs[nstate]->GetMixCtlProc(nidx, ninst);
            if (!busedB) {
                ptr = &pmxctlproc->pudata->pstCurveData->Q15Output;
                break;
            }

            ptr = &pmxctlproc->pudata->CmpdBOut;
            break;
        }
        case 0x20000000: {
            int nstate = (sfxid >> 16) & 0xFF;
            int ninst = (sfxid >> 11) & 0x1F;
            int ntype = sfxid & 0x10000000;
            int nidx = sfxid & 0xFF;

            if (ntype == 0) {
                stMasterMixChProc *pmch = this->m_pStateProcs[nstate]->GetMasterMixChProc(nidx, ninst);
                ptr = &pmch->pMixChData_U->Output;
                break;
            }

            {
                stSubMixChProc *psch = this->m_pStateProcs[nstate]->GetSubMixChProc(nidx, ninst);
                ptr = &psch->pMixChData_U->Output;
                break;
            }
        }
        case 0x40000000:
        case 0x60000000: {
            int idx = sfxid & 0xF;
            int *pinput = (*this->mGetOutPtrCB)(sfxid);
            if (pinput == nullptr) {
                pinput = this->GetNextInputBlock(true);
                (*this->mSetSFXOutCB)(sfxid, pinput);
            }

            ptr = pinput + idx;
            break;
        }
        case static_cast<int>(0x80000000U): {
            st3DMixCtlProc *p3d;

            int nState = (sfxid >> 16) & 0xFF;
            int ninst = (sfxid >> 11) & 0x1F;
            int nidx = sfxid & 0xFF;
            if (bHACKINIT) {
                p3d = this->m_pStateProcs[nState]->Get3DMixCtlProc(nidx, ninst);
                ptr = reinterpret_cast<int *>(p3d);
                break;
            }

            p3d = this->m_pStateProcs[nState]->Get3DMixCtlProc(nidx, ninst);
            if (busedB) {
                ptr = &p3d->p3DMixCtlData_U->dBRolloff;
                break;
            }
            ptr = &p3d->p3DMixCtlData_U->q15Rolloff;
            break;
        }
        case static_cast<int>(0xA0000000U): {
            int nState = (sfxid >> 16) & 0xFF;
            int ninst = (sfxid >> 11) & 0x1F;
            int nidx = sfxid & 0xFF;
            stEvtMixCtlProc *pevt = this->m_pStateProcs[nState]->GetEvtMixCtlProc(nidx, ninst);
            if (busedB) {
                ptr = &pevt->pData_U->output;
                break;
            }
            ptr = &pevt->pData_U->qoutput;
            break;
        }
        default:
            break;
    }

    return ptr;
}

void NFSMixMap::ConnectMixMap() {
    stCurveDataProc *pcdp = this->m_pCurveDataArray;
    int k = 0;
    for (; k < this->m_CurveProcsAdded; k++, pcdp++) {
        int sfxid = pcdp->nINPUTID;
        pcdp->pInputParam = this->GetObjectPtr(sfxid, false, false);
    }

    for (k = 0; k < this->m_ScaleParamsAdded; k++) {
        int sfxid = reinterpret_cast<int>(this->m_pScalePtrArray[k]);
        this->m_pScalePtrArray[k] = this->GetObjectPtr(sfxid, false, false);
    }

    st3DMixCtlProc *p3DProc = this->m_p3DMixCtlProc;
    for (k = 0; k < this->m_3DMixCtlsAdded; k++, p3DProc++) {
        int nID = reinterpret_cast<int>(p3DProc->p3DMixCtlData_U->pInputs);
        p3DProc->p3DMixCtlData_U->pInputs = this->GetObjectPtr(nID, false, false);
    }

    stEvtMixCtlProc *pEVP = this->m_pEvtMixCtlProc;
    for (k = 0; k < this->m_EventCtlsAdded; k++, pEVP++) {
        int nID = reinterpret_cast<int>(pEVP->pData_U->pTriggerPtr);
        pEVP->pData_U->pTriggerPtr = this->GetObjectPtr(nID, false, false);
    }

    for (k = 0; k < this->m_nTotalSubChannelInputs; k++) {
        int *newptr = this->m_pSubChannelInputs + k;
        int sfxid = *newptr;
        *newptr = reinterpret_cast<int>(this->GetObjectPtr(sfxid, true, false));
    }

    for (k = 0; k < this->m_nTotalMasterChannelInputs; k++) {
        int *newptr = this->m_pMasterChannelInputs + k;
        int sfxid = *newptr;
        *newptr = reinterpret_cast<int>(this->GetObjectPtr(sfxid, true, true));
    }
}

void NFSMixMap::SetupStateProcArrays() {
    int j;
    int k;
    int s;
    int t;
    int z;

    for (int n = 0; n < this->m_pMMHdr->NumStates; n++) {
        int i;
        int m = 0;

        if (this->m_pStateProcs[n] != nullptr) {
            for (; m < this->m_pStateProcs[n]->m_ThisStateRefCnt; m++) {
                this->m_pStateProcs[n]->GetMixMapProc(m)->InitializeSubChannels();
                this->m_pStateProcs[n]->GetMixMapProc(m)->InitializeMasterChannels();
            }
        }
    }
}

void NFSMixMap::InitMainMapStates() {
    this->SetupStateProcArrays();
    this->ConnectMixMap();
}

// STRIPPED
void NFSMixMap::AddMapState(eMAINMAPSTATES estate, int numdups) {}

// STRIPPED
NFSMixMap *NFSMixMap::GetMasterMixMap() {}

// STRIPPED
NFSMixMap *NFSMixMap::GetSecondaryMap() {}

float F_DT_FRAME_LOCK = 0.0333667f; // size: 0x4, address: 0x80418A28, Decl: 2611

// UNSOLVED
void NFSMixMap::ProcessMixMap(float dt, eCamStates camstate) {
    this->m_msDeltaTime = dt * 1000.0f;
    this->m_fDeltaTimeRatio[1] = this->m_fDeltaTimeRatio[0];
    this->m_PrevCamState = this->m_CurCamState;
    this->m_CurCamState = camstate;
    this->m_fDeltaTimeRatio[0] = dt / F_DT_FRAME_LOCK;
    this->m_fDeltaTime = dt;

    stCurveDataProc *pcvdp;
    stMixCtlProc *pmxdp;

    pcvdp = this->m_pCurveDataArray;
    for (int ncv = 0; ncv < this->m_CurveProcsAdded; ncv++) {
        eMIXTABLEID ncurve = static_cast<eMIXTABLEID>((pcvdp->nINPUTID >> 24) & 0xF);
        pcvdp->Q15Output = NFSMixShape::GetCurveOutput(ncurve, *pcvdp->pInputParam, false);

        pcvdp->dBOutput = NFSMixShape::GetdBFromQ15(pcvdp->Q15Output);
        pcvdp++;
    }

    pmxdp = this->m_pMixCtlProc;

    for (int nmxctl = 0; nmxctl < this->m_MixCtlsAdded; nmxctl++) {
        short swing;
        int nout;
        int scaleby;
        int scale;

        {
            int Q15Val = 0x7FFF - pmxdp->pudata->pstCurveData->Q15Output;
            Q15Val = (Q15Val * pmxdp->psdata->nRatio) >> 15;
            nout = 0x7FFF - Q15Val;
        }

        {
            int Q15Val = nout;
            nout = NFSMixShape::GetdBFromQ15(Q15Val);
        }

        nout += pmxdp->psdata->nOffset;

        scaleby = 0x7FFF;

        if (pmxdp->pudata->ppScaleRatios != nullptr) {
            scale = (pmxdp->psdata->pstMixCtlParms->nUScaleCntSwing >> 24) & 0xFF;

            int numscale = scale;
            for (int ns = 0; ns < numscale; ns++) {
                scaleby = (*pmxdp->pudata->ppScaleRatios[ns] * scaleby) >> 15;
            }
        }

        nout = (scaleby * nout) >> 15;
        pmxdp->pudata->CmpdBOut = nout;

        pmxdp++;
    }

    this->Update3DMixCtls();
    this->UpdateEvtMixCtls();
    this->UpdateSubChannels();
    this->UpdateMasterChannels();
    this->MixMasterChannels();
}

void NFSMixMap::UpdateSubChannels() {
    stSubMixChProc *pSChP = this->m_pSubChProc;
    stMixChSharedData *pSChD_S;
    stMixChUniqueData *pSChD_U;

    for (int nsub = 0; nsub < this->m_SubMixChannelsAdded; pSChP++, nsub++) {
        pSChD_U = pSChP->pMixChData_U;
        pSChD_S = pSChP->pMixChData_S;

        if (pSChD_U->pInputs != nullptr) {
            int numin = pSChD_S->NumInputs & 0xFF;
            pSChD_U->Output = 0;

            for (int mix = 0; mix < numin; mix++) {
                int **pin = reinterpret_cast<int **>(&pSChD_U->pInputs[mix]);
                pSChD_U->Output += **pin;
            }

            int nUp = (pSChD_S->pMapParams->UpperLowerSwing >> 16) & 0x7FFF;
            int nDwn = pSChD_S->pMapParams->UpperLowerSwing | 0xFFFF0000;

            if (pSChD_U->Output > nUp) {
                pSChD_U->Output = nUp;
            }

            if (pSChD_U->Output < nDwn) {
                pSChD_U->Output = nDwn;
            }
        }
    }
}

void NFSMixMap::UpdateMasterChannels() {
    stMasterMixChProc *pMChP = this->m_pMasterChProc;
    stMasterMixChSharedData *pMChD_S;
    stMasterMixChUniqueData *pMChD_U;

    for (int nmst = 0; nmst < this->m_MasterChannelsAdded; pMChP++, nmst++) {
        pMChD_U = pMChP->pMixChData_U;
        pMChD_S = pMChP->pMixChData_S;

        if (pMChD_U->pOutputs[0xF] & 1U) {
            pMChD_U->Output = *reinterpret_cast<short *>(&pMChD_S->pMapParams->MixData);
            pMChD_U->Output &= 0xFFFF;

            if ((pMChD_U->Output & 0x8000) != 0) {
                pMChD_U->Output |= 0xFFFF0000;
            }

            if (pMChP->pMixChData_U->pInputs != nullptr) {
                int numin = pMChD_S->NumInputs & 0xFF;

                for (int mix = 0; mix < numin; mix++) {
                    int **pin = reinterpret_cast<int **>(&pMChD_U->pInputs[mix]);
                    pMChD_U->Output += **pin;
                }
            }
        } else {
            pMChD_U->Output = -10000;
        }
    }
}

void NFSMixMap::MixMasterChannels() {
    stMasterMixChProc *pMChP = this->m_pMasterChProc;

    for (int nmst = 0; nmst < this->m_MasterChannelsAdded; pMChP++, nmst++) {
        int *pPresets = pMChP->pMixChData_S->pPRESETS;
        int masterindex = *pPresets++;
        int NumPresets = masterindex & 0xF;
        int type = (masterindex >> 24) & 0xF;

        if ((pMChP->pMixChData_U->pOutputs[0xF] & 1) != 0) {
            for (int np = 0; np < NumPresets; np++, pPresets++) {
                int nmasteridx = *pPresets;
                int nIndex = nmasteridx >> 26;
                int n3DIndex = (nmasteridx >> 21) & 0x1F;
                short vol = static_cast<short>(*pPresets);
                int *pOut = pMChP->pMixChData_U->pOutputs;
                int *pAzim;
                int tmpvol;
                int num3d = (pMChP->pMixChData_S->NumInputs >> 16) & 0x1F;
                int out;
                int shift = (nIndex & 1) << 4;
                int maskshift = (((nIndex & 0x1F) + 1) & 1) << 4;
                int mask = 0xFFFF << maskshift;
                int *pSlot = pOut + ((nIndex & 0x1F) >> 1);

                if ((num3d > 0) && (n3DIndex < num3d)) {
                    st3DMixCtlProc *p3d = pMChP->pMixChData_U->p3DData[n3DIndex];

                    if (nmasteridx < 0) {
                        pAzim = &p3d->p3DMixCtlData_U->azimuth;
                        out = *pAzim & 0xFFFF;
                    } else {
                        switch (type) {
                            case 0:
                            case 4:
                                out = p3d->p3DMixCtlData_U->dBRolloff;
                                out += pMChP->pMixChData_U->Output + vol;

                                if (out < -10000) {
                                    out = -10000;
                                }

                                if (out > 0) {
                                    out = 0;
                                }

                                out = NFSMixShape::GetQ15FromHundredthsdB(out);
                                break;

                            case 1:
                                out = p3d->p3DMixCtlData_U->DopplerCents;
                                out += pMChP->pMixChData_U->Output + vol;

                                if (out > 0x960) {
                                    out = 0x960;
                                }

                                if (out < -0x12C0) {
                                    out = 0;
                                }
                                break;

                            case 2:
                                out = pMChP->pMixChData_U->Output + vol;

                                if (out < -10000) {
                                    out = -10000;
                                }

                                if (out > 0) {
                                    out = 0;
                                }
                                break;

                            default:
                                out = pMChP->pMixChData_U->Output;
                                break;
                        }
                    }
                } else {
                    out = pMChP->pMixChData_U->Output + vol;

                    switch (type) {
                        case 0:
                        case 4:
                            if (out < -10000) {
                                out = -10000;
                            }

                            if (out > 0) {
                                out = 0;
                            }

                            out = NFSMixShape::GetQ15FromHundredthsdB(out);
                            break;

                        case 1:
                            if (out > 0x960) {
                                out = 0x960;
                            }

                            if (out < -0x12C0) {
                                out = -0x12C0;
                            }
                            break;

                        case 2: {
                            if (out < -10000) {
                                out = -10000;
                            }

                            if (out > 0) {
                                out = 0;
                            }

                            float ftmp = NFSMixShape::GetPitchMultFromCents(out);
                            out = static_cast<int>(ftmp * 25000.0f);
                            break;
                        }

                        default:
                            if (out < 0) {
                                out = 0;
                            }

                            if (out > 25000) {
                                out = 25000;
                            }
                            break;
                    }
                }

                tmpvol = out & 0xFFFF;
                tmpvol <<= shift;
                *pSlot &= mask;
                out = *pSlot | tmpvol;
                *pSlot = out;
            }
        } else {
            int out;

            switch (type) {
                case 0:
                case 4:
                    out = -10000;
                    break;
                case 1:
                    out = 0;
                    break;
                case 2:
                    out = 25000;
                    break;
                default:
                    out = -10000;
                    break;
            }

            int nmasteridx = *pPresets;
            int shift = ((nmasteridx >> 26) & 1) << 4;
            int maskshift = (((((nmasteridx >> 26) & 0x1F) + 1) & 1) << 4);
            int mask = 0xFFFF << maskshift;
            int *pSlot = pMChP->pMixChData_U->pOutputs + (((nmasteridx >> 26) & 0x1F) >> 1);
            int tmpvol = out & 0xFFFF;
            out = (*pSlot & mask) | (tmpvol << shift);
            *pSlot = out;
        }
    }
}

void NFSMixMap::UpdateEvtMixCtls() {
    stEvtMixCtlProc *pevtproc = this->m_pEvtMixCtlProc;

    for (int n = 0; n < this->m_EventCtlsAdded; pevtproc++, n++) {
        stEvtMixCtlUniqueData *pevt_U = pevtproc->pData_U;
        stEvtMixCtlSharedData *pevt_S;

        if ((pevt_U->msTimeElapsed == 0.0f) && (*pevt_U->pTriggerPtr == 0)) {
            pevt_U->output = 0;
            pevt_U->qoutput = 0x7FFF;
            pevt_U->reset = 0;
            pevt_U->reset_level = -10000;
        } else {
            pevt_U->msTimeElapsed += this->m_msDeltaTime;

            eDMIXENVELOPS ntype = static_cast<eDMIXENVELOPS>((pevtproc->pData_S->pMapParms->nEVTCTLID >> 24) & 0xF);

            switch (ntype) {
                case DMENV_AR:
                    this->UpdateAREvent(pevtproc);
                    break;

                case DMENV_ASR:
                    this->UpdateASREvent(pevtproc);
                    break;

                case DMENV_ATR:
                    this->UpdateATREvent(pevtproc);
                    break;

                case DMENV_LFO:
                    this->UpdateLFOEvent(pevtproc);
                    break;
            }

            if (pevtproc->pData_U->ppScaleRatios != nullptr) {
                int numscale = (pevtproc->pData_S->pMapParms->nUScaleCntSwing >> 24) & 0xFF;
                int nout = pevtproc->pData_U->output;

                for (int ns = 0; ns < numscale; ns++) {
                    nout = (nout * *pevtproc->pData_U->ppScaleRatios[ns]) >> 15;
                }

                pevtproc->pData_U->output = nout;
            }
        }
    }
}

void NFSMixMap::UpdateLFOEvent(stEvtMixCtlProc *pProc) {}

void NFSMixMap::UpdateATREvent(stEvtMixCtlProc *pProc) {
    float ftstage_0 = static_cast<float>(pProc->pData_S->pMapParms->nParam_00 & 0xFFF) * 16.66667f;
    eMIXTABLEID ncurvestage_0 = static_cast<eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_00 >> 12) & 0xF);
    float ftstage_2 = static_cast<float>(pProc->pData_S->pMapParms->nParam_02 & 0xFFF) * 16.66667f;
    eMIXTABLEID ncurvestage_2 = static_cast<eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_02 >> 12) & 0xF);
    int nSwing = pProc->pData_S->pMapParms->nUScaleCntSwing & 0xFFFF;

    if ((pProc->pData_S->pMapParms->nUScaleCntSwing & 0x8000) != 0) {
        nSwing |= 0xFFFF0000;
    }

    if (pProc->pData_U->msTimeElapsed < ftstage_0) {
        if (*pProc->pData_U->pTriggerPtr == 0) {
            pProc->pData_U->reset = 1;
            pProc->pData_U->reset_level = pProc->pData_U->output;
            pProc->pData_U->msTimeElapsed = ftstage_0;
            return;
        }

        int ndt = static_cast<int>((pProc->pData_U->msTimeElapsed * 32767.0f) / ftstage_0);

        if (nSwing < 0) {
            pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
        } else {
            pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
        }
    } else {
        if (*pProc->pData_U->pTriggerPtr == 0) {
            if (pProc->pData_U->msResetTime == 0.0f) {
                pProc->pData_U->msResetTime = pProc->pData_U->msTimeElapsed;
            }

            if (pProc->pData_U->msTimeElapsed - pProc->pData_U->msResetTime > ftstage_2) {
                pProc->pData_U->reset = 0;
                pProc->pData_U->reset_level = 0;
                pProc->pData_U->msTimeElapsed = 0.0f;
                pProc->pData_U->msResetTime = 0.0f;
                pProc->pData_U->qoutput = 0x7FFF;
                return;
            }

            int ndt = static_cast<int>(32767.0f - (((pProc->pData_U->msTimeElapsed - pProc->pData_U->msResetTime) * 32767.0f) / ftstage_2));

            if (nSwing < 0) {
                pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_2, ndt, false);
            } else {
                pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_2, ndt, false);
            }
        } else {
            if ((*pProc->pData_U->pTriggerPtr == 1) && (pProc->pData_U->msResetTime != 0.0f)) {
                pProc->pData_U->reset = 1;
                pProc->pData_U->msTimeElapsed = 0.0f;
                pProc->pData_U->reset_level = pProc->pData_U->output;
                pProc->pData_U->qoutput = 0x7FFF;
                pProc->pData_U->msResetTime = 0.0f;
                return;
            }

            pProc->pData_U->reset = 0;
            pProc->pData_U->reset_level = 0;
            pProc->pData_U->msResetTime = 0.0f;
            pProc->pData_U->msTimeElapsed = ftstage_0;
            pProc->pData_U->qoutput = 0;
        }
    }

    if (pProc->pData_U->reset != 0) {
        if (*pProc->pData_U->pTriggerPtr == 0) {
            float nratio = (32767.0f - static_cast<float>(pProc->pData_U->qoutput)) / 32767.0f;

            pProc->pData_U->output = static_cast<int>(nratio * static_cast<float>(pProc->pData_U->reset_level));
            return;
        }

        float nratio = (32767.0f - static_cast<float>(pProc->pData_U->qoutput)) / 32767.0f;

        pProc->pData_U->output = static_cast<int>(nratio * static_cast<float>(nSwing - pProc->pData_U->reset_level)) + pProc->pData_U->reset_level;
        return;
    }

    float nratio = (32767.0f - static_cast<float>(pProc->pData_U->qoutput)) / 32767.0f;

    pProc->pData_U->output = static_cast<int>(nratio * static_cast<float>(nSwing));
    return;
}

// UNSOLVED
void NFSMixMap::UpdateASREvent(stEvtMixCtlProc *pProc) {
    float ftstage_0 = static_cast<float>(pProc->pData_S->pMapParms->nParam_00 & 0xFFF) * 16.66667f;
    eMIXTABLEID ncurvestage_0 = static_cast<eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_00 >> 12) & 0xF);
    float ftstage_1 = static_cast<float>(pProc->pData_S->pMapParms->nParam_01 & 0xFFF) * 16.66667f;
    float ftstage_2 = static_cast<float>(pProc->pData_S->pMapParms->nParam_02 & 0xFFF) * 16.66667f;
    eMIXTABLEID ncurvestage_2 = static_cast<eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_02 >> 12) & 0xF);
    int nSwing = pProc->pData_S->pMapParms->nUScaleCntSwing & 0xFFFF;

    if ((pProc->pData_S->pMapParms->nUScaleCntSwing & 0x8000) != 0) {
        nSwing |= 0xFFFF0000;
    }

    float nratio = pProc->pData_U->msTimeElapsed;

    if (nratio < ftstage_0) {
        nratio = (nratio * 32767.0f) / ftstage_0;

        {
            int ndt = static_cast<int>(nratio);

            if (nSwing < 0) {
                pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
            } else {
                pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
            }
        }
    } else if ((nratio - ftstage_0) > ftstage_1) {
        nratio = nratio - (ftstage_0 + ftstage_1);

        if (nratio < ftstage_2) {
            nratio = 32767.0f - ((nratio * 32767.0f) / ftstage_2);

            {
                int ndt = static_cast<int>(nratio);

                if (nSwing < 0) {
                    pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_2, ndt, false);
                } else {
                    pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_2, ndt, false);
                }
            }
        } else {
            pProc->pData_U->msTimeElapsed = 0.0f;
            pProc->pData_U->qoutput = 0x7FFF;
        }
    } else {
        pProc->pData_U->qoutput = 0;
    }

    pProc->pData_U->output = static_cast<int>(((32767.0f - static_cast<float>(pProc->pData_U->qoutput)) / 32767.0f) * static_cast<float>(nSwing));
}

void NFSMixMap::UpdateAREvent(stEvtMixCtlProc *pProc) {
    float ftstage_0 = static_cast<float>(pProc->pData_S->pMapParms->nParam_00 & 0xFFF) * 16.66667f;
    eMIXTABLEID ncurvestage_0 = static_cast<eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_00 >> 12) & 0xF);
    float ftstage_2 = static_cast<float>(pProc->pData_S->pMapParms->nParam_02 & 0xFFF) * 16.66667f;
    eMIXTABLEID ncurvestage_2 = static_cast<eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_02 >> 12) & 0xF);
    int nSwing = pProc->pData_S->pMapParms->nUScaleCntSwing & 0xFFFF;

    if ((pProc->pData_S->pMapParms->nUScaleCntSwing & 0x8000) != 0) {
        nSwing |= 0xFFFF0000;
    }

    float nratio = pProc->pData_U->msTimeElapsed;

    if (nratio < ftstage_0) {
        int ndt = static_cast<int>((nratio * 32767.0f) / ftstage_0);

        if (nSwing < 0) {
            pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
        } else {
            pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
        }
    } else if ((nratio - ftstage_0) < ftstage_2) {
        int ndt = static_cast<int>(32767.0f - (((nratio - ftstage_0) * 32767.0f) / ftstage_2));

        if (nSwing < 0) {
            pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_2, ndt, false);
        } else {
            pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_2, ndt, false);
        }
    } else {
        pProc->pData_U->msTimeElapsed = 0.0f;
        pProc->pData_U->qoutput = 0x7FFF;
    }

    nratio = (32767.0f - static_cast<float>(pProc->pData_U->qoutput)) / 32767.0f;
    pProc->pData_U->output = static_cast<int>(nratio * static_cast<float>(nSwing));
}

float DOPPLER_SMOOTHING_FACTOR = 0.2f; // size: 0x4, address: 0x80418A2C, Decl: 4039

void NFSMixMap::Update3DMixCtls() {
    if (this->m_CurCamState != this->m_PrevCamState) {
        st3DMixCtlSharedData *p3Ds = this->m_p3DMixCtlData_S;

        for (int ns = 0; ns < this->m_nAssigned3DMixCtlShared; ns++, p3Ds++) {
            int numstates = (p3Ds->pMapParams->nINPUTID >> 24) & 0xF;
            st3DStateParams *pstateparams = &p3Ds->pMapParams->StateParams;
            eCamStates camstate = this->m_CurCamState;
            bool found = false;

        RestartLoop:
            for (int nsp = 0; nsp < numstates; nsp++) {
                st3DStateParams *p3dsp = pstateparams + nsp;
                eCamStates testcamstate = static_cast<eCamStates>((p3dsp->n3DSTATEINFOID >> 24) & 0xF);
                if (testcamstate == camstate) {
                    p3Ds->pCurStateParams = p3dsp;
                    found = true;
                    p3Ds->PrevCamState = this->m_PrevCamState;
                    p3Ds->CurCamState = this->m_CurCamState;
                    p3Ds->msSinceCamTrans = 0;
                    break;
                }
            }

            if (!found) {
                camstate = DMIX_DEFAULT_CAM;
                goto RestartLoop;
            }
        }
    }

    st3DMixCtlProc *p3Dproc = this->m_p3DMixCtlProc;

    for (int n = 0; n < this->m_3DMixCtlsAdded; n++) {
        if (p3Dproc->p3DMixCtlData_U->pInputs[15] & 1) {
            st3DStateParams *psparams = p3Dproc->p3DMixCtlData_S->pCurStateParams;
            int nid = psparams->n3DSTATEINFOID;
            int nDistType = (nid >> 12) & 0xF;
            int nAzimType = (nid >> 8) & 0xF;
            int ntables = psparams->nCURVEID_DOPPLER;
            int nazim;
            float fdist[2] = {0.0f, 0.0f};
            float fmindist[2];
            float fmaxdist[2];
            int uAverage = 0;
            eMIXTABLEID nqOne = SHAPE_DWN_LINEAR;
            eMIXTABLEID nqTwo = SHAPE_DWN_LINEAR;

            switch (nDistType) {
                case 0:
                    fdist[0] = static_cast<float>(p3Dproc->p3DMixCtlData_U->pInputs[1]) * 0.01f;
                    break;
                case 1:
                    fdist[0] = static_cast<float>(p3Dproc->p3DMixCtlData_U->pInputs[0]) * 0.01f;
                    break;
                default:
                    fdist[0] = -1.00000006f;
                    break;
            }
            fdist[1] = fdist[0];

            switch (nAzimType) {
                case 0:
                    nazim = p3Dproc->p3DMixCtlData_U->pInputs[3];
                    break;
                case 1:
                    nazim = p3Dproc->p3DMixCtlData_U->pInputs[2];
                    break;
                default:
                    nazim = 0;
                    break;
            }

            int AzimOut = nazim;
            int nQuad = (static_cast<unsigned int>(AzimOut) >> 14) & 3;
            p3Dproc->p3DMixCtlData_U->azimuth = AzimOut;

            int nNextQuad;
            switch (nQuad) {
                case 0:
                    nNextQuad = 1;
                    nqOne = static_cast<eMIXTABLEID>((ntables >> 28) & 0xF);
                    nqTwo = static_cast<eMIXTABLEID>((ntables >> 16) & 0xF);
                    fmindist[0] = static_cast<float>(psparams->nQ0MinMax & 0x7FFF);
                    fmaxdist[0] = static_cast<float>((psparams->nQ0MinMax >> 16) & 0x7FFF);
                    fmindist[1] = static_cast<float>(psparams->nQ1MinMax & 0x7FFF);
                    fmaxdist[1] = static_cast<float>((psparams->nQ1MinMax >> 16) & 0x7FFF);
                    uAverage = AzimOut;
                    break;

                case 1:
                    nNextQuad = 2;
                    nqOne = static_cast<eMIXTABLEID>((ntables >> 16) & 0xF);
                    nqTwo = static_cast<eMIXTABLEID>((ntables >> 24) & 0xF);
                    fmindist[0] = static_cast<float>(psparams->nQ1MinMax & 0x7FFF);
                    fmaxdist[0] = static_cast<float>((psparams->nQ1MinMax >> 16) & 0x7FFF);
                    fmindist[1] = static_cast<float>(psparams->nQ2MinMax & 0x7FFF);
                    fmaxdist[1] = static_cast<float>((psparams->nQ2MinMax >> 16) & 0x7FFF);
                    uAverage = AzimOut - 0x4000;
                    break;

                case 2:
                    nNextQuad = 3;
                    nqOne = static_cast<eMIXTABLEID>((ntables >> 24) & 0xF);
                    nqTwo = static_cast<eMIXTABLEID>((ntables >> 20) & 0xF);
                    fmindist[0] = static_cast<float>(psparams->nQ2MinMax & 0x7FFF);
                    fmaxdist[0] = static_cast<float>((psparams->nQ2MinMax >> 16) & 0x7FFF);
                    fmindist[1] = static_cast<float>(psparams->nQ3MinMax & 0x7FFF);
                    fmaxdist[1] = static_cast<float>((psparams->nQ3MinMax >> 16) & 0x7FFF);
                    uAverage = AzimOut - 0x8000;
                    break;

                case 3:
                    nNextQuad = 0;
                    nqOne = static_cast<eMIXTABLEID>((ntables >> 20) & 0xF);
                    nqTwo = static_cast<eMIXTABLEID>((ntables >> 28) & 0xF);
                    fmindist[0] = static_cast<float>(psparams->nQ3MinMax & 0x7FFF);
                    fmaxdist[0] = static_cast<float>((psparams->nQ3MinMax >> 16) & 0x7FFF);
                    fmindist[1] = static_cast<float>(psparams->nQ0MinMax & 0x7FFF);
                    fmaxdist[1] = static_cast<float>((psparams->nQ0MinMax >> 16) & 0x7FFF);
                    uAverage = AzimOut - 0xC000;
                    break;
            }

            if ((fdist[0] > fmaxdist[0]) && (fdist[0] > fmaxdist[1])) {
                p3Dproc->p3DMixCtlData_U->dBRolloff = -10000;
                p3Dproc->p3DMixCtlData_U->q15Rolloff = 0;
                p3Dproc->p3DMixCtlData_U->DopplerCents = 0;
                p3Dproc++;
                continue;
            }

            if (fdist[0] < fmindist[0]) {
                fdist[0] = fmindist[0];
            }
            if (fdist[1] < fmindist[1]) {
                fdist[1] = fmindist[1];
            }
            if (fdist[0] > fmaxdist[0]) {
                fdist[0] = fmaxdist[0];
            }
            if (fdist[1] > fmaxdist[1]) {
                fdist[1] = fmaxdist[1];
            }

            int qDist[2] = {0, 0};

            fdist[0] = (fdist[0] - fmindist[0]) / (fmaxdist[0] - fmindist[0]);
            fdist[1] = (fdist[1] - fmindist[1]) / (fmaxdist[1] - fmindist[1]);

            qDist[0] = static_cast<int>(fdist[0] * 32767.0f);
            qDist[1] = static_cast<int>(fdist[1] * 32767.0f);
            p3Dproc->p3DMixCtlData_U->q15Rolloff = NFSMixShape::GetAzimShapeOutput(nqOne, nqTwo, qDist, uAverage);
            p3Dproc->p3DMixCtlData_U->dBRolloff = NFSMixShape::GetdBFromQ15(p3Dproc->p3DMixCtlData_U->q15Rolloff);

            if ((psparams->nCURVEID_DOPPLER & 0xFFFF) != 0) {
                float fdopplerboost;
                float fvelsound = static_cast<float>(psparams->nCURVEID_DOPPLER & 0xFFFF);
                float fvel;
                float ftratio;
                float fdoppler;
                float fdeltanewdist;
                float fcurdist;
                float fdistratio;
                float fcents = 0.0f;

                if ((this->m_CurCamState < 3) || (this->m_CurCamState == DMIX_NFS_NIS_CAM)) {
                    fcurdist = static_cast<float>(p3Dproc->p3DMixCtlData_U->pInputs[0]) * 0.01f;
                    fvel = static_cast<float>(p3Dproc->p3DMixCtlData_U->pInputs[13]) * 0.01f;

                    if (p3Dproc->p3DMixCtlData_U->pInputs[15] < 0) {
                        p3Dproc->p3DMixCtlData_U->pInputs[15] &= 0x7FFFFFFF;
                    } else {
                        {
                            float divisor = fvelsound + fvel;
                            if (divisor <= 0.0f) {
                                divisor = fvelsound;
                            }
                            ftratio = fvelsound / divisor;
                        }
                        fcents = static_cast<float>(NFSMixShape::GetCentsFromPitchMult(ftratio));
                    }
                } else {
                    fcurdist = static_cast<float>(p3Dproc->p3DMixCtlData_U->pInputs[1]) * 0.01f;
                    fvel = static_cast<float>(p3Dproc->p3DMixCtlData_U->pInputs[14]) * 0.01f;

                    if ((p3Dproc->p3DMixCtlData_U->pInputs[0xF] & 0x40000000) == 0) {
                        {
                            float divisor = fvelsound + fvel;
                            if (divisor <= 0.0f) {
                                divisor = fvelsound;
                            }
                            ftratio = fvelsound / divisor;
                        }
                        fcents = static_cast<float>(NFSMixShape::GetCentsFromPitchMult(ftratio));
                    } else {
                        p3Dproc->p3DMixCtlData_U->pInputs[15] &= 0xBFFFFFFF;
                    }
                }

                if (p3Dproc->p3DMixCtlData_U->fPrevDeltaDist == 0.0f) {
                    p3Dproc->p3DMixCtlData_U->fPrevDeltaDist = 1.0f;
                }

                if (fcurdist > p3Dproc->p3DMixCtlData_U->fPrevDist) {
                    fdeltanewdist = fcurdist - p3Dproc->p3DMixCtlData_U->fPrevDist;
                } else {
                    fdeltanewdist = p3Dproc->p3DMixCtlData_U->fPrevDist - fcurdist;
                }

                p3Dproc->p3DMixCtlData_U->fPrevDeltaDist = fdeltanewdist;
                p3Dproc->p3DMixCtlData_U->fPrevDist = fcurdist;
                fdoppler = fcents - static_cast<float>(p3Dproc->p3DMixCtlData_U->DopplerCents);
                fdopplerboost = fdoppler * DOPPLER_SMOOTHING_FACTOR;
                p3Dproc->p3DMixCtlData_U->DopplerCents = p3Dproc->p3DMixCtlData_U->DopplerCents + static_cast<int>(fdopplerboost);

                if ((STATE_MASK(p3Dproc->p3DMixCtlData_U->nINPUTID) == eMM_AIRACECAR) &&
                    (fdeltanewdist / p3Dproc->p3DMixCtlData_U->fPrevDeltaDist > 0.0f)) {
                    fdistratio = fdeltanewdist / p3Dproc->p3DMixCtlData_U->fPrevDeltaDist;
                }
            }
        } else {
            p3Dproc->p3DMixCtlData_U->dBRolloff = -10000;
            p3Dproc->p3DMixCtlData_U->q15Rolloff = 0;
            p3Dproc->p3DMixCtlData_U->azimuth = 0;
            p3Dproc->p3DMixCtlData_U->DopplerCents = 0;
        }

        p3Dproc++;
    }
}
