#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSndUtil.h"

extern float DOPPLER_SMOOTHING_FACTOR;
extern float F_DT_FRAME_LOCK;

int *(*NFSMixMap::mGetOutPtrCB)(int) = nullptr;
void (*NFSMixMap::mSetSFXOutCB)(int, int *) = nullptr;
bool (*NFSMixMap::mSetSFXInCB)(int, int *) = nullptr;
int (*NFSMixMap::mGetStateRefCnt)(int) = nullptr;
void (*NFSMixMap::mMapReadyCB)() = nullptr;
int DUMMYINPUT = 0;

NFSMixMap::NFSMixMap()
    : AudioMemBase() {
    m_nStateMapCount = 0;
    m_fDeltaTimeRatio[0] = 0.0f;
    m_pStateProcs = nullptr;
    m_fDeltaTimeRatio[1] = 0.0f;
}

NFSMixMap::~NFSMixMap() {}

void NFSMixMap::DestroyMainMixMap() {
    gAudioMemoryManager.FreeMemory(m_pStateProcs);
    m_pStateProcs = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMasterChannelInputs);
    m_pMasterChannelInputs = nullptr;
    gAudioMemoryManager.FreeMemory(m_pSubChannelInputs);
    m_pSubChannelInputs = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMasterChannelOutputArrayBlock);
    m_pMasterChannelOutputArrayBlock = nullptr;
    gAudioMemoryManager.FreeMemory(m_pStateProcMemBlock);
    m_pStateProcMemBlock = nullptr;
    gAudioMemoryManager.FreeMemory(m_pScalePtrArray);
    m_pScalePtrArray = nullptr;
    gAudioMemoryManager.FreeMemory(m_pCurveDataArray);
    m_pCurveDataArray = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMixCtlData_S);
    m_pMixCtlData_S = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMixCtlData_U);
    m_pMixCtlData_U = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMixCtlProc);
    m_pMixCtlProc = nullptr;
    gAudioMemoryManager.FreeMemory(m_pSubChData_S);
    m_pSubChData_S = nullptr;
    gAudioMemoryManager.FreeMemory(m_pSubChData_U);
    m_pSubChData_U = nullptr;
    gAudioMemoryManager.FreeMemory(m_pSubChProc);
    m_pSubChProc = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMasterChData_S);
    m_pMasterChData_S = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMasterChData_U);
    m_pMasterChData_U = nullptr;
    gAudioMemoryManager.FreeMemory(m_pMasterChProc);
    m_pMasterChProc = nullptr;
    gAudioMemoryManager.FreeMemory(m_p3DMixCtlData_S);
    m_p3DMixCtlData_S = nullptr;
    gAudioMemoryManager.FreeMemory(m_p3DMixCtlData_U);
    m_p3DMixCtlData_U = nullptr;
    gAudioMemoryManager.FreeMemory(m_p3DMixCtlProc);
    m_p3DMixCtlProc = nullptr;
    gAudioMemoryManager.FreeMemory(m_pEvtMixCtlData_S);
    m_pEvtMixCtlData_S = nullptr;
    gAudioMemoryManager.FreeMemory(m_pEvtMixCtlData_U);
    m_pEvtMixCtlData_U = nullptr;
    gAudioMemoryManager.FreeMemory(m_pEvtMixCtlProc);
    m_pEvtMixCtlProc = nullptr;
    gAudioMemoryManager.FreeMemory(m_pDynMixInputBlocks);
    m_pDynMixInputBlocks = nullptr;
}

void NFSMixMap::AssignSFXCallbacks(int *(*getptrcb)(int), void (*setsfxoutcb)(int, int *),
                                   bool (*setsfxincb)(int, int *), int (*getstaterefcnt)(int),
                                   void (*mapreadycb)()) {
    mGetOutPtrCB = getptrcb;
    mSetSFXOutCB = setsfxoutcb;
    mSetSFXInCB = setsfxincb;
    mGetStateRefCnt = getstaterefcnt;
    mMapReadyCB = mapreadycb;
}

bool NFSMixMap::SETSFXID(int id, int *ptr) {
    return (*mSetSFXInCB)(id, ptr);
}

void NFSMixMap::InitMixMap(int *pmixmap, NFSMixMap *pMasterMixMap) {
    int n;

    m_pMasterMixMap = pMasterMixMap;
    m_pMixMap = pmixmap;
    m_pMMHdr = reinterpret_cast<stMixMapHeader *>(pmixmap);
    m_MapType = *pmixmap;

    if (pmixmap[1] > 0) {
        for (n = 0; n < m_pMMHdr->NumStates; n++) {
            m_StateRefCount[n] = 0;
        }
    }

    PreProcessMixMap();
}

void NFSMixMap::ResetMapData() {
    int n;

    m_ScaleParamsIDCount = 0;
    m_nAssignedMixMapStates = 0;
    m_MixCtlsAdded = 0;
    m_SharedMixCtlCount = 0;
    m_nAssignedMixCtlProc = 0;
    m_AssignedMixCtlsShared = 0;
    m_AssignedMixCtlsUnique = 0;
    m_ScaleParamsAdded = 0;

    for (n = 0; n < 10; n++) {
        m_CurveProcsTotal[n][0] = 0;
        m_CurveProcsTotal[n][1] = 0;
    }

    m_3DMixCtlsAdded = 0;
    m_EventCtlsAdded = 0;
    m_nAssignedInputBlocks = 0;
    m_nAssigned3DMixCtlProc = 0;
    m_nAssigned3DMixCtlShared = 0;
    m_nAssigned3DMixCtlUnique = 0;
    m_nAssignedEvtMixCtlProc = 0;
    m_nAssignedEvtMixCtlShared = 0;
    m_nAssignedEvtMixCtlUnique = 0;
    m_PrevCamState = DMIX_DEFAULT_CAM;
    m_CurCamState = DMIX_DEFAULT_CAM;
    m_Shared3DMixCtlCount = 0;
    m_SharedEvtMixCtlCount = 0;
    m_SubMixChannelsAdded = 0;
    m_SharedSubMixCount = 0;
    m_nAssignedSubMixProc = 0;
    m_nAssignedSubMixShared = 0;
    m_nAssignedSubMixUnique = 0;
    m_MasterChannelsAdded = 0;
    m_SharedMasterMixCount = 0;
    m_nAssignedMasterMixProc = 0;
    m_nAssignedMasterMixShared = 0;
    m_nAssignedMasterMixUnique = 0;
    m_CurrentStateProcBlockOffset = 0;
    m_nTotalMasterChannelInputs = 0;
    m_CurrentMasterInputBlockOffset = 0;
    m_CurrentSubInputBlockOffset = 0;
    m_CurrentMasterOutputBlockOffset = 0;
    m_CurrentMasterChannelPtrBlockOffset = 0;
    m_CurrentSubChannelPtrBlockOffset = 0;
    m_Current3DMixCtlPtrBlockOffset = 0;
    m_CurrentEvtMixCtlPtrBlockOffset = 0;
    m_nTotalMasterChannel3DOutputs = 0;
    m_nTotalSubChannelInputs = 0;
    m_pMasterChannelOutputArrayBlock = nullptr;
    m_nTotalSubChannel3DOutputs = 0;
    m_nTotalUniqueMasterChannels = 0;
    m_SFXOBJsAdded = 0;
    m_SFXCTLsAdded = 0;
    m_DataProcsAdded = 0;
    m_n3DCamStatesAdded = 0;
    m_SharedMixCtlsAssigned = 0;
    m_UniqueMixCtlsAssigned = 0;
    m_CurveProcsAdded = 0;
    m_CurrentMasterInputOffset = 0;
    m_CurrentSubInputOffset = 0;
    m_pStateProcMemBlock = nullptr;
    m_pDynMixInputBlocks = nullptr;
    m_pScalePtrArray = nullptr;
    m_pCurveDataArray = nullptr;
    m_pMixCtlData_S = nullptr;
    m_pMixCtlData_U = nullptr;
    m_pMixCtlProc = nullptr;
    m_pEvtMixCtlProc = nullptr;
    m_pEvtMixCtlData_S = nullptr;
    m_pEvtMixCtlData_U = nullptr;
    m_p3DMixCtlProc = nullptr;
    m_p3DMixCtlData_S = nullptr;
    m_p3DMixCtlData_U = nullptr;
    m_pSubChData_S = nullptr;
    m_pSubChData_U = nullptr;
    m_pSubChProc = nullptr;
    m_pMasterChData_S = nullptr;
    m_pMasterChData_U = nullptr;
    m_pMasterChProc = nullptr;
    m_pMasterChannelInputs = nullptr;
    m_pSubChannelInputs = nullptr;
}

void NFSMixMap::SetupStateRefCount() {
    int ntotalstateprocs;
    int *pStateOffsetTable;

    ntotalstateprocs = 0;
    pStateOffsetTable =
        reinterpret_cast<int *>(reinterpret_cast<char *>(m_pMMHdr) + m_pMMHdr->StateTableOffset);

    if (m_pMMHdr->NumStates > 0) {
        do {
            m_StateRefCount[ntotalstateprocs] = 0;

            if (*pStateOffsetTable != -1) {
                m_StateRefCount[ntotalstateprocs] = (*mGetStateRefCnt)(ntotalstateprocs);
            }

            ntotalstateprocs++;
            pStateOffsetTable++;
        } while (ntotalstateprocs < m_pMMHdr->NumStates);
    }
}

void NFSMixMap::PreProcessMixMap() {
    int n;
    int ntotalcurveprocs;
    int ns;
    char *pMixMapBase;
    stMixMapHeader *pmmhdr;
    int *pStateOffsetTable;

    n = 0;
    pmmhdr = reinterpret_cast<stMixMapHeader *>(m_pMixMap);
    pMixMapBase = reinterpret_cast<char *>(pmmhdr);
    pStateOffsetTable = reinterpret_cast<int *>(pMixMapBase + pmmhdr->StateTableOffset);

    ResetMapData();
    SetupStateRefCount();
    m_nStateMapCount = 0;

    if (pmmhdr->NumStates > 0) {
        do {
            stMixMapStateHdr *pmmsthdr;

            ns = n + 1;
            if (*pStateOffsetTable != -1) {
                int mixCtlOffset;
                int threeDMixCtlOffset;
                int eventCtlOffset;
                int subMixOffset;
                int masterMixOffset;

                pmmsthdr = reinterpret_cast<stMixMapStateHdr *>(pMixMapBase + *pStateOffsetTable);
                m_nStateMapCount += m_StateRefCount[n];

                mixCtlOffset = pmmsthdr->OffsetMixCtlData;
                if (mixCtlOffset != -1) {
                    stMixCtlHdr *pmctlhdr;
                    unsigned int *pCurveWord;
                    unsigned int *pCurveIds;
                    int ctlCount;
                    int ctlIndex;

                    pmctlhdr = reinterpret_cast<stMixCtlHdr *>(reinterpret_cast<char *>(pmmsthdr) + mixCtlOffset);
                    ctlCount = pmctlhdr->NumMixCtls;
                    pCurveWord = reinterpret_cast<unsigned int *>(pmctlhdr + 1);
                    pCurveIds = static_cast<unsigned int *>(
                        gAudioMemoryManager.AllocateMemory(ctlCount << 2, "Temp MIXMAP ALLOC", false));

                    m_MixCtlsAdded += m_StateRefCount[n] * ctlCount;
                    m_SharedMixCtlCount += ctlCount;
                    m_DataProcsAdded += m_StateRefCount[n] * pmctlhdr->NumNewMixDataProcs;

                    for (ctlIndex = 0; ctlIndex < ctlCount; ctlIndex++) {
                        bool uniqueCurve;
                        int scaleParamsAdded;
                        int existingCurveIndex;
                        int scaleIndex;
                        int scaleCount;
                        unsigned int curveType;
                        unsigned int curveId;
                        unsigned int *pScaleWord;

                        curveId = *pCurveWord;
                        curveType = (curveId >> 24) & 0xF;
                        scaleCount = static_cast<int>(*reinterpret_cast<short *>(pCurveWord + 1)) & 0xF;
                        pCurveIds[ctlIndex] = curveId;

                        uniqueCurve = true;
                        for (existingCurveIndex = 0; existingCurveIndex < ctlIndex; existingCurveIndex++) {
                            if ((pCurveIds[existingCurveIndex] & 0xE0FFFFF0U) == (curveId & 0xE0FFFFF0U)) {
                                uniqueCurve = false;
                            }
                        }

                        if (uniqueCurve) {
                            m_CurveProcsTotal[curveType][0] += m_StateRefCount[n];
                        }

                        scaleParamsAdded = 0;
                        pScaleWord = pCurveWord + 2;
                        for (scaleIndex = 0; scaleIndex < scaleCount; scaleIndex++) {
                            unsigned char inputState;

                            inputState = *(reinterpret_cast<unsigned char *>(pScaleWord) + 1);
                            if (inputState != n) {
                                scaleParamsAdded += m_StateRefCount[inputState];
                            } else {
                                scaleParamsAdded++;
                            }

                            pScaleWord++;
                        }

                        pCurveWord += scaleCount + 2;
                        m_ScaleParamsAdded += scaleParamsAdded * m_StateRefCount[n];
                    }

                    gAudioMemoryManager.FreeMemory(pCurveIds);
                }

                threeDMixCtlOffset = pmmsthdr->Offset3DMixCtlData;
                if (threeDMixCtlOffset != -1) {
                    st3DMixCtlHdr *p3Dmctlhdr;
                    int ctlCount;
                    int totalCamStates;
                    int ctlIndex;
                    unsigned int *p3DMixCtlWord;

                    p3Dmctlhdr = reinterpret_cast<st3DMixCtlHdr *>(reinterpret_cast<char *>(pmmsthdr) + threeDMixCtlOffset);
                    ctlCount = p3Dmctlhdr->Num3DMixCtls;
                    p3DMixCtlWord = reinterpret_cast<unsigned int *>(p3Dmctlhdr + 1);
                    totalCamStates = 0;

                    m_Shared3DMixCtlCount += p3Dmctlhdr->Num3DMixCtls & 0xFF;
                    m_3DMixCtlsAdded += m_StateRefCount[n] * (p3Dmctlhdr->Num3DMixCtls & 0xFF);

                    for (ctlIndex = 0; ctlIndex < ctlCount; ctlIndex++) {
                        int camStatesAdded;

                        camStatesAdded = (*p3DMixCtlWord >> 24) & 0xF;
                        totalCamStates += camStatesAdded;
                        p3DMixCtlWord += camStatesAdded * 6;
                    }

                    m_n3DCamStatesAdded += totalCamStates;
                }

                eventCtlOffset = pmmsthdr->OffsetEventCtlData;
                if (eventCtlOffset != -1) {
                    stMixEventHdr *pevtctlhdr;
                    int ctlCount;
                    int ctlIndex;
                    int scaleParamsAdded;
                    char *pEventCtlData;

                    pevtctlhdr = reinterpret_cast<stMixEventHdr *>(reinterpret_cast<char *>(pmmsthdr) + eventCtlOffset);
                    ctlCount = pevtctlhdr->NumEvents;
                    pEventCtlData = reinterpret_cast<char *>(pevtctlhdr + 1);
                    scaleParamsAdded = 0;

                    m_EventCtlsAdded += m_StateRefCount[n] * ctlCount;
                    m_SharedEvtMixCtlCount += ctlCount;

                    for (ctlIndex = 0; ctlIndex < ctlCount; ctlIndex++) {
                        int scaleCount;
                        int scaleIndex;
                        char *pScaleData;

                        pScaleData = pEventCtlData + 0x18;
                        scaleCount = static_cast<int>(*reinterpret_cast<short *>(pEventCtlData + 4)) & 0xF;
                        for (scaleIndex = 0; scaleIndex < scaleCount; scaleIndex++) {
                            unsigned char inputState;

                            inputState = *(reinterpret_cast<unsigned char *>(pScaleData) + 1);
                            if (inputState != n) {
                                scaleParamsAdded += m_StateRefCount[inputState];
                            } else {
                                scaleParamsAdded++;
                            }

                            pScaleData += 4;
                        }

                        pEventCtlData += (scaleCount * 4) + 0x18;
                    }

                    m_ScaleParamsAdded += scaleParamsAdded * m_StateRefCount[n];
                }

                subMixOffset = pmmsthdr->OffsetSubMixData;
                if (subMixOffset != -1) {
                    stMixChHdr *pmsubchhdr;
                    int subMixCount;
                    int subMixIndex;
                    int *pInputData;

                    pmsubchhdr = reinterpret_cast<stMixChHdr *>(reinterpret_cast<char *>(pmmsthdr) + subMixOffset);
                    subMixCount = pmsubchhdr->NumMixChannels;
                    pInputData = reinterpret_cast<int *>(pmsubchhdr + 1);

                    m_SharedSubMixCount += subMixCount;
                    m_SubMixChannelsAdded += m_StateRefCount[n] * subMixCount;

                    for (subMixIndex = 0; subMixIndex < subMixCount; subMixIndex++) {
                        int inputIndex;
                        int totalInputs;
                        unsigned char inputCount;

                        inputCount = *(reinterpret_cast<unsigned char *>(pInputData) + 1);
                        pInputData += 2;
                        totalInputs = 0;

                        for (inputIndex = 0; inputIndex < inputCount; inputIndex++) {
                            unsigned char inputState;

                            inputState = *(reinterpret_cast<unsigned char *>(pInputData) + 1);
                            if (inputState != n) {
                                totalInputs += m_StateRefCount[inputState];
                            } else {
                                totalInputs++;
                            }

                            pInputData++;
                        }

                        m_nTotalSubChannelInputs += totalInputs * m_StateRefCount[n];
                    }
                }

                masterMixOffset = pmmsthdr->OffsetMasterMixData;
                if (masterMixOffset != -1) {
                    stMixChHdr *pmasterchhdr;
                    int masterMixCount;
                    int masterMixIndex;
                    int *pInputData;

                    pmasterchhdr = reinterpret_cast<stMixChHdr *>(reinterpret_cast<char *>(pmmsthdr) + masterMixOffset);
                    masterMixCount = pmasterchhdr->NumMixChannels;
                    pInputData = reinterpret_cast<int *>(pmasterchhdr + 1);

                    m_nTotalUniqueMasterChannels += pmasterchhdr->NumUniqueSFXOBJs + (m_StateRefCount[n] * pmasterchhdr->NumUniqueSFXOBJs);
                    m_SharedMasterMixCount += masterMixCount;
                    m_MasterChannelsAdded += m_StateRefCount[n] * masterMixCount;

                    for (masterMixIndex = 0; masterMixIndex < masterMixCount; masterMixIndex++) {
                        int inputIndex;
                        int totalInputs;
                        unsigned char inputCount;

                        inputCount = *(reinterpret_cast<unsigned char *>(pInputData) + 1);
                        pInputData += 3;
                        totalInputs = 0;

                        for (inputIndex = 0; inputIndex < inputCount; inputIndex++) {
                            unsigned char inputState;

                            inputState = *(reinterpret_cast<unsigned char *>(pInputData) + 1);
                            if (inputState != n) {
                                totalInputs += m_StateRefCount[inputState];
                            } else {
                                totalInputs++;
                            }

                            pInputData++;
                        }

                        m_nTotalMasterChannelInputs += totalInputs * m_StateRefCount[n];
                    }
                }
            }

            pStateOffsetTable++;
            n = ns;
        } while (ns < pmmhdr->NumStates);
    }

    ntotalcurveprocs = 0;
    do {
        m_CurveProcsAdded += m_CurveProcsTotal[ntotalcurveprocs][0];
        ntotalcurveprocs++;
    } while (ntotalcurveprocs < 10);
}

void NFSMixMap::AllocateMixerMemory() {
    int n;

    if (!m_pStateProcs) {
        m_pStateProcs = static_cast<NFSMixMapState **>(
            gAudioMemoryManager.AllocateMemory(m_pMMHdr->NumStates << 2, "Dyn Mix Proc Array", false));
    }

    for (n = 0; n < m_pMMHdr->NumStates; n++) {
        m_pStateProcs[n] = nullptr;
    }

    m_pMasterChannelInputs = static_cast<int *>(gAudioMemoryManager.AllocateMemory(
        m_nTotalMasterChannelInputs << 2, "Master Channel Input Array Block", false));
    m_pSubChannelInputs = static_cast<int *>(gAudioMemoryManager.AllocateMemory(
        m_nTotalSubChannelInputs << 2, "Sub Channel Input Array Block", false));
    m_pMasterChannelOutputArrayBlock = static_cast<int *>(gAudioMemoryManager.AllocateMemory(
        m_nTotalUniqueMasterChannels << 6, "Master Channel Output Array Block", false));

    m_pStateProcMemBlock = static_cast<NFSMixMapState **>(
        gAudioMemoryManager.AllocateMemory(m_nStateMapCount * sizeof(NFSMixMapState), "NFSMixMapState Object Memory", false));
    for (n = 0; n < m_nStateMapCount; n++) {
        new (reinterpret_cast<char *>(m_pStateProcMemBlock) + (n * sizeof(NFSMixMapState))) NFSMixMapState();
    }

    m_pScalePtrArray = static_cast<int **>(
        gAudioMemoryManager.AllocateMemory(m_ScaleParamsAdded << 2, "Scale Input Ptr Array Block", false));
    m_pCurveDataArray = static_cast<stCurveDataProc *>(
        gAudioMemoryManager.AllocateMemory(m_CurveProcsAdded << 4, "Curve Proc Data Array", false));
    m_pMixCtlData_S = static_cast<stMixCtlSharedData *>(
        gAudioMemoryManager.AllocateMemory(m_SharedMixCtlCount << 4, "NFSMixCtl Shared Data Array", false));
    m_pMixCtlData_U = static_cast<stMixCtlUniqueData *>(
        gAudioMemoryManager.AllocateMemory(m_MixCtlsAdded * 0xC, "NFSMixCtl Unique Data Array", false));
    m_pMixCtlProc = static_cast<stMixCtlProc *>(
        gAudioMemoryManager.AllocateMemory(m_MixCtlsAdded << 3, "NFSMixCtl Process Data Array", false));
    m_pSubChData_S = static_cast<stMixChSharedData *>(
        gAudioMemoryManager.AllocateMemory(m_SharedSubMixCount * 0xC, "SubMix Shared Data Block", false));
    m_pSubChData_U = static_cast<stMixChUniqueData *>(
        gAudioMemoryManager.AllocateMemory(m_SubMixChannelsAdded << 3, "SubMix Unique Data Block", false));
    m_pSubChProc = static_cast<stSubMixChProc *>(
        gAudioMemoryManager.AllocateMemory(m_SubMixChannelsAdded << 3, "SubMix Proc Data Block", false));
    m_pMasterChData_S = static_cast<stMasterMixChSharedData *>(
        gAudioMemoryManager.AllocateMemory(m_SharedMasterMixCount << 4, "MasterMix Shared Data Block", false));
    m_pMasterChData_U = static_cast<stMasterMixChUniqueData *>(
        gAudioMemoryManager.AllocateMemory(m_MasterChannelsAdded * 0x14, "MasterMix Unique Data Block", false));
    m_pMasterChProc = static_cast<stMasterMixChProc *>(
        gAudioMemoryManager.AllocateMemory(m_MasterChannelsAdded << 3, "MasterMix Proc Data Block", false));
    m_p3DMixCtlData_S = static_cast<st3DMixCtlSharedData *>(
        gAudioMemoryManager.AllocateMemory(m_Shared3DMixCtlCount * 0x14, "3DMixCtl Shared Data Block", false));
    m_p3DMixCtlData_U = static_cast<st3DMixCtlUniqueData *>(
        gAudioMemoryManager.AllocateMemory(m_3DMixCtlsAdded << 5, "3DMixCtl Unique Data Block", false));
    m_p3DMixCtlProc = static_cast<st3DMixCtlProc *>(
        gAudioMemoryManager.AllocateMemory(m_3DMixCtlsAdded << 3, "3DMixCtl Proc Data Block", false));
    m_pEvtMixCtlData_S = static_cast<stEvtMixCtlSharedData *>(
        gAudioMemoryManager.AllocateMemory(m_SharedEvtMixCtlCount << 2, "EvtMixCtl Shared Data Block", false));
    m_pEvtMixCtlData_U = static_cast<stEvtMixCtlUniqueData *>(
        gAudioMemoryManager.AllocateMemory(m_EventCtlsAdded << 5, "EvtMixCtl Unique Data Block", false));
    m_pEvtMixCtlProc = static_cast<stEvtMixCtlProc *>(
        gAudioMemoryManager.AllocateMemory(m_EventCtlsAdded << 3, "EvtMixCtl Proc Data Block", false));
}

stEvtMixCtlProc *NFSMixMap::GetNextEvtMixCtlProc(bool bincrement) {
    stEvtMixCtlProc *pAddr;

    pAddr = m_pEvtMixCtlProc + m_nAssignedEvtMixCtlProc;
    if (bincrement) {
        ++m_nAssignedEvtMixCtlProc;
    }

    return pAddr;
}

stEvtMixCtlSharedData *NFSMixMap::GetNextEvtMixCtlShared(bool bincrement) {
    stEvtMixCtlSharedData *pAddr;

    pAddr = m_pEvtMixCtlData_S + m_nAssignedEvtMixCtlShared;
    if (bincrement) {
        ++m_nAssignedEvtMixCtlShared;
    }

    return pAddr;
}

stEvtMixCtlUniqueData *NFSMixMap::GetNextEvtMixCtlUnique(bool bincrement) {
    stEvtMixCtlUniqueData *pAddr;

    pAddr = m_pEvtMixCtlData_U + m_nAssignedEvtMixCtlUnique;
    if (bincrement) {
        ++m_nAssignedEvtMixCtlUnique;
    }

    return pAddr;
}

st3DMixCtlProc *NFSMixMap::GetNext3DMixCtlProc(bool bincrement) {
    st3DMixCtlProc *pAddr;

    pAddr = m_p3DMixCtlProc + m_nAssigned3DMixCtlProc;
    if (bincrement) {
        ++m_nAssigned3DMixCtlProc;
    }

    return pAddr;
}

st3DMixCtlSharedData *NFSMixMap::GetNext3DMixCtlShared(bool bincrement) {
    st3DMixCtlSharedData *pAddr;

    pAddr = m_p3DMixCtlData_S + m_nAssigned3DMixCtlShared;
    if (bincrement) {
        ++m_nAssigned3DMixCtlShared;
    }

    return pAddr;
}

st3DMixCtlUniqueData *NFSMixMap::GetNext3DMixCtlUnique(bool bincrement) {
    st3DMixCtlUniqueData *pAddr;

    pAddr = m_p3DMixCtlData_U + m_nAssigned3DMixCtlUnique;
    if (bincrement) {
        ++m_nAssigned3DMixCtlUnique;
    }

    return pAddr;
}

stMasterMixChProc *NFSMixMap::GetNextMasterMixProc(bool bincrement) {
    stMasterMixChProc *pAddr;

    pAddr = m_pMasterChProc + m_nAssignedMasterMixProc;
    if (bincrement) {
        ++m_nAssignedMasterMixProc;
    }

    return pAddr;
}

stMasterMixChSharedData *NFSMixMap::GetNextMasterMixShared(bool bincrement) {
    stMasterMixChSharedData *pAddr;

    pAddr = m_pMasterChData_S + m_nAssignedMasterMixShared;
    if (bincrement) {
        ++m_nAssignedMasterMixShared;
    }

    return pAddr;
}

stMasterMixChUniqueData *NFSMixMap::GetNextMasterMixUnique(bool bincrement) {
    stMasterMixChUniqueData *pAddr;

    pAddr = m_pMasterChData_U + m_nAssignedMasterMixUnique;
    if (bincrement) {
        ++m_nAssignedMasterMixUnique;
    }

    return pAddr;
}

stSubMixChProc *NFSMixMap::GetNextSubMixProc(bool bincrement) {
    stSubMixChProc *pAddr;

    pAddr = m_pSubChProc + m_nAssignedSubMixProc;
    if (bincrement) {
        ++m_nAssignedSubMixProc;
    }

    return pAddr;
}

stMixChUniqueData *NFSMixMap::GetNextSubMixUnique(bool bincrement) {
    stMixChUniqueData *pAddr;

    pAddr = m_pSubChData_U + m_nAssignedSubMixUnique;
    if (bincrement) {
        ++m_nAssignedSubMixUnique;
    }

    return pAddr;
}

stMixChSharedData *NFSMixMap::GetNextSubMixShared(bool bincrement) {
    stMixChSharedData *pAddr;

    pAddr = m_pSubChData_S + m_nAssignedSubMixShared;
    if (bincrement) {
        ++m_nAssignedSubMixShared;
    }

    return pAddr;
}

stMixCtlProc *NFSMixMap::GetProcessMixCtlPtr(bool bincrement) {
    stMixCtlProc *pAddr;

    pAddr = m_pMixCtlProc + m_nAssignedMixCtlProc;
    if (bincrement) {
        ++m_nAssignedMixCtlProc;
    }

    return pAddr;
}

int *NFSMixMap::GetNextInputBlock(bool bincrement) {
    int blockIndex;
    int n;
    int *pclear;

    blockIndex = m_nAssignedInputBlocks;
    if (bincrement) {
        m_nAssignedInputBlocks = blockIndex + 1;
    }

    pclear = reinterpret_cast<int *>(m_pDynMixInputBlocks) + (blockIndex * 0x10);
    for (n = 0; n < 0x10; n++) {
        *pclear++ = 0;
    }

    return reinterpret_cast<int *>(m_pDynMixInputBlocks) + (blockIndex * 0x10);
}

NFSMixMapState *NFSMixMap::GetNextMapState(bool bincrement) {
    int offset;

    offset = m_CurrentStateProcBlockOffset;
    if (bincrement) {
        m_CurrentStateProcBlockOffset = offset + 0x60;
    }

    return reinterpret_cast<NFSMixMapState *>(reinterpret_cast<char *>(m_pStateProcMemBlock) + offset);
}

int *NFSMixMap::GetMasterChannelOutputArrayPtr(int nNumChannels) {
    int offset;

    offset = m_CurrentMasterOutputBlockOffset;
    m_CurrentMasterOutputBlockOffset = offset + (nNumChannels * 0x10);
    return m_pMasterChannelOutputArrayBlock + offset;
}

int *NFSMixMap::GetMasterChannelInputPtr(int nsize) {
    int offset;

    offset = m_CurrentMasterInputBlockOffset;
    m_CurrentMasterInputBlockOffset = offset + nsize;
    return m_pMasterChannelInputs + offset;
}

int *NFSMixMap::GetSubChannelInputPtr(int nsize) {
    int offset;

    offset = m_CurrentSubInputBlockOffset;
    m_CurrentSubInputBlockOffset = offset + nsize;
    return m_pSubChannelInputs + offset;
}

int NFSMixMap::GetMapStateCopies(int nstate) {
    if (nstate < m_pMMHdr->NumStates) {
        return m_StateRefCount[nstate];
    }

    return 0;
}

void NFSMixMap::CreateMainMapState(eMAINMAPSTATES estate, int numstates, int objnum) {
    NFSMixMapState *pstate;
    int smoffset;
    int *pStateOffsets;

    if (!m_pStateProcs[estate]) {
        pstate = GetNextMapState(true);
        m_pStateProcs[estate] = pstate;
        m_pStateProcs[estate]->Initialize(this, estate, numstates, objnum);
    }

    m_pStateProcs[estate]->AddMixState(objnum, m_pStateProcs[estate]);
    pStateOffsets = reinterpret_cast<int *>(reinterpret_cast<char *>(m_pMMHdr) + m_pMMHdr->StateTableOffset);
    smoffset = pStateOffsets[estate];
    pstate = m_pStateProcs[estate]->GetMixMapProc(objnum);
    pstate->m_pMMStateHdr = reinterpret_cast<stMixMapStateHdr *>(reinterpret_cast<char *>(m_pMMHdr) + smoffset);
    pstate->CreateMixCtls();
    pstate->Create3DMixCtls();
    pstate->CreateEvtMixCtls();
}

void NFSMixMap::AllocateInputArrays() {
    int nTotalUniqueCurveIDs;
    int ntotaluniqueScaleID;
    int ntotalunique3DID;
    int ntotaluniqueEvents;
    int n;

    nTotalUniqueCurveIDs = 0;
    ntotaluniqueScaleID = 0;
    ntotalunique3DID = 0;
    ntotaluniqueEvents = 0;

    for (n = 0; n < m_CurveProcsAdded; n++) {
        unsigned int ntype;
        bool bUniqueCurveID;

        ntype = m_pCurveDataArray[n].nINPUTID & 0xE0000000;
        bUniqueCurveID = true;

        if ((ntype == 0x40000000) || (ntype == 0x60000000) || (ntype == 0x80000000)) {
            int k;

            for (k = 0; k < n; k++) {
                if ((m_pCurveDataArray[k].nINPUTID & 0xE0FFFFF0U) ==
                    (m_pCurveDataArray[n].nINPUTID & 0xE0FFFFF0U)) {
                    bUniqueCurveID = false;
                }
            }
        } else {
            bUniqueCurveID = false;
        }

        if (bUniqueCurveID) {
            nTotalUniqueCurveIDs++;
        }
    }

    for (n = 0; n < m_ScaleParamsAdded; n++) {
        unsigned int ntype;
        bool buniquescale;
        int *psc;

        psc = m_pScalePtrArray[n];
        ntype = reinterpret_cast<unsigned int>(psc) & 0xE0000000;
        buniquescale = true;

        if ((ntype == 0x40000000) || (ntype == 0x60000000) || (ntype == 0x80000000)) {
            int m;

            for (m = 0; m < n; m++) {
                if ((reinterpret_cast<unsigned int>(psc) & 0xE0FFFFF0) ==
                    (reinterpret_cast<unsigned int>(m_pScalePtrArray[m]) & 0xE0FFFFF0)) {
                    buniquescale = false;
                }
            }
        } else {
            buniquescale = false;
        }

        if (buniquescale) {
            ntotaluniqueScaleID++;
        }
    }

    for (n = 0; n < m_3DMixCtlsAdded; n++) {
        bool bunique3d;
        unsigned int nID;
        int k;

        nID = m_p3DMixCtlProc[n].p3DMixCtlData_U->nINPUTID & 0xE0FFFFF0;
        bunique3d = true;

        for (k = 0; k < n; k++) {
            if (nID == (m_p3DMixCtlProc[k].p3DMixCtlData_U->nINPUTID & 0xE0FFFFF0U)) {
                bunique3d = false;
            }
        }

        if (bunique3d) {
            int s;

            for (s = 0; s < m_ScaleParamsAdded; s++) {
                if ((reinterpret_cast<unsigned int>(m_pScalePtrArray[s]) & 0xE0FFFFF0) == nID) {
                    bunique3d = false;
                }
            }

            if (bunique3d) {
                ntotalunique3DID++;
            }
        }
    }

    for (n = 0; n < m_pMMHdr->NumStates; n++) {
        if (m_pStateProcs[n]) {
            int e;
            int numevtctls;

            numevtctls = m_pStateProcs[n]->m_EvtMixCtlsAdded;
            for (e = 0; e < numevtctls; e++) {
                bool buniqueevent;
                unsigned int nID;
                stEvtMixCtlProc *pevtproc;

                pevtproc = m_pStateProcs[n]->GetEvtMixCtlProc(e, 0);
                nID = pevtproc->pData_S->pMapParms->nTriggerID & 0xE0FFFFF0;
                buniqueevent = true;

                if (e > 0) {
                    int es;

                    for (es = 0; es < e; es++) {
                        pevtproc = m_pStateProcs[n]->GetEvtMixCtlProc(es, 0);
                        if ((pevtproc->pData_S->pMapParms->nTriggerID & 0xE0FFFFF0U) == nID) {
                            buniqueevent = false;
                        }
                    }
                }

                if (buniqueevent) {
                    int s;

                    for (s = 0; s < m_ScaleParamsAdded; s++) {
                        if ((reinterpret_cast<unsigned int>(m_pScalePtrArray[s]) & 0xE0FFFFF0) == nID) {
                            buniqueevent = false;
                        }
                    }

                    if (buniqueevent) {
                        int nt3d;

                        for (nt3d = 0; nt3d < m_3DMixCtlsAdded; nt3d++) {
                            if (m_p3DMixCtlProc[nt3d].p3DMixCtlData_U->nINPUTID == static_cast<int>(nID)) {
                                buniqueevent = false;
                            }
                        }

                        if (buniqueevent) {
                            ntotaluniqueEvents += m_pStateProcs[n]->GetStateRefCount();
                        }
                    }
                }
            }
        }
    }

    m_pDynMixInputBlocks = static_cast<int **>(gAudioMemoryManager.AllocateMemory(
        (nTotalUniqueCurveIDs + ntotaluniqueScaleID + ntotalunique3DID + ntotaluniqueEvents) * 0x40,
        "DMIX SFXOBJ, SFXCTL Input Block", false));
}

int *NFSMixMap::GetObjectPtr(int sfxid, bool busedB, bool bHACKINIT) {
    int *ptr;
    int ntype;

    ptr = &DUMMYINPUT;
    ntype = sfxid & 0xE0000000;

    if (ntype == 0) {
        int nstate;
        int ninst;
        int nidx;
        stMixCtlProc *pmxctlproc;

        nstate = (sfxid >> 16) & 0xFF;
        ninst = (sfxid >> 11) & 0x1F;
        nidx = sfxid & 0xFF;
        pmxctlproc = m_pStateProcs[nstate]->GetMixCtlProc(nidx, ninst);
        if (!busedB) {
            return &pmxctlproc->pudata->pstCurveData->Q15Output;
        }

        return &pmxctlproc->pudata->CmpdBOut;
    }

    if (ntype > 0) {
        if (ntype == 0x20000000) {
            int nstate;
            int ninst;
            int ntype;
            int nidx;

            nstate = (sfxid >> 16) & 0xFF;
            ninst = (sfxid >> 11) & 0x1F;
            ntype = sfxid & 0x10000000;
            nidx = sfxid & 0xFF;
            stMasterMixChProc *pmch;

            if (ntype == 0) {
                pmch = m_pStateProcs[nstate]->GetMasterMixChProc(nidx, ninst);
                return &pmch->pMixChData_U->Output;
            }

            stSubMixChProc *psch;

            psch = m_pStateProcs[nstate]->GetSubMixChProc(nidx, ninst);
            return &psch->pMixChData_U->Output;
        }

        if (ntype == 0x40000000 || ntype == 0x60000000) {
            int idx;

            ptr = (*mGetOutPtrCB)(sfxid);
            if (!ptr) {
                ptr = GetNextInputBlock(true);
                (*mSetSFXOutCB)(sfxid, ptr);
            }

            idx = sfxid & 0xF;
            return ptr + idx;
        }

        return ptr;
    }

    if (ntype == static_cast<int>(0x80000000U)) {
        int nState;
        int ninst;
        int nidx;
        st3DMixCtlProc *p3d;

        if (bHACKINIT) {
            nState = (sfxid >> 16) & 0xFF;
            ninst = (sfxid >> 11) & 0x1F;
            nidx = sfxid & 0xFF;
            p3d = m_pStateProcs[nState]->Get3DMixCtlProc(nidx, ninst);
            return reinterpret_cast<int *>(p3d);
        }

        nState = (sfxid >> 16) & 0xFF;
        ninst = (sfxid >> 11) & 0x1F;
        nidx = sfxid & 0xFF;
        p3d = m_pStateProcs[nState]->Get3DMixCtlProc(nidx, ninst);
        if (busedB) {
            return &p3d->p3DMixCtlData_U->dBRolloff;
        }
        return &p3d->p3DMixCtlData_U->q15Rolloff;
    }

    if (ntype == static_cast<int>(0xA0000000U)) {
        int nState;
        int ninst;
        int nidx;
        stEvtMixCtlProc *pevt;

        nState = (sfxid >> 16) & 0xFF;
        ninst = (sfxid >> 11) & 0x1F;
        nidx = sfxid & 0xFF;
        pevt = m_pStateProcs[nState]->GetEvtMixCtlProc(nidx, ninst);
        if (busedB) {
            return &pevt->pData_U->output;
        }
        return &pevt->pData_U->qoutput;
    }

    return ptr;
}

void NFSMixMap::ConnectMixMap() {
    int n;

    for (n = 0; n < m_CurveProcsAdded; n++) {
        m_pCurveDataArray[n].pInputParam = GetObjectPtr(m_pCurveDataArray[n].nINPUTID, false, false);
    }

    for (n = 0; n < m_ScaleParamsAdded; n++) {
        m_pScalePtrArray[n] = GetObjectPtr(reinterpret_cast<int>(m_pScalePtrArray[n]), false, false);
    }

    for (n = 0; n < m_3DMixCtlsAdded; n++) {
        m_p3DMixCtlProc[n].p3DMixCtlData_U->pInputs = GetObjectPtr(
            reinterpret_cast<int>(m_p3DMixCtlProc[n].p3DMixCtlData_U->pInputs), false, false);
    }

    for (n = 0; n < m_EventCtlsAdded; n++) {
        m_pEvtMixCtlProc[n].pData_U->pTriggerPtr =
            GetObjectPtr(reinterpret_cast<int>(m_pEvtMixCtlProc[n].pData_U->pTriggerPtr), false, false);
    }

    for (n = 0; n < m_nTotalSubChannelInputs; n++) {
        m_pSubChannelInputs[n] = reinterpret_cast<int>(GetObjectPtr(m_pSubChannelInputs[n], true, false));
    }

    for (n = 0; n < m_nTotalMasterChannelInputs; n++) {
        m_pMasterChannelInputs[n] = reinterpret_cast<int>(GetObjectPtr(m_pMasterChannelInputs[n], true, true));
    }
}

void NFSMixMap::SetupStateProcArrays() {
    int stateIndex;

    if (m_pMMHdr->NumStates > 0) {
        for (stateIndex = 0; stateIndex < m_pMMHdr->NumStates; stateIndex++) {
            if (m_pStateProcs[stateIndex] && m_pStateProcs[stateIndex]->m_ThisStateRefCnt > 0) {
                int refcnt;

                for (refcnt = 0; refcnt < m_pStateProcs[stateIndex]->m_ThisStateRefCnt; refcnt++) {
                    NFSMixMapState *pstate;

                    pstate = m_pStateProcs[stateIndex]->GetMixMapProc(refcnt);
                    pstate->InitializeSubChannels();
                    pstate = m_pStateProcs[stateIndex]->GetMixMapProc(refcnt);
                    pstate->InitializeMasterChannels();
                }
            }
        }
    }
}

void NFSMixMap::InitMainMapStates() {
    SetupStateProcArrays();
    ConnectMixMap();
}

stCurveDataProc *NFSMixMap::GetCurveDataPtr(stMixCtlParams *pparams) {
    int n;
    int ncurve;
    int noffset;
    stCurveDataProc *pcdp;

    if (!m_pCurveDataArray) {
        return nullptr;
    }

    ncurve = (pparams->nINPUTID >> 24) & 0xF;
    noffset = 0;
    for (n = 0; n < ncurve; n++) {
        noffset += m_CurveProcsTotal[n][0];
    }

    pcdp = m_pCurveDataArray + noffset;
    for (n = 0; n < m_CurveProcsTotal[ncurve][1]; n++) {
        if (pcdp->nINPUTID == pparams->nINPUTID) {
            return pcdp;
        }
        pcdp++;
    }

    m_CurveProcsTotal[ncurve][1] = m_CurveProcsTotal[ncurve][1] + 1;
    pcdp->dBOutput = 0;
    pcdp->nINPUTID = pparams->nINPUTID;
    pcdp->Q15Output = 0x7FFF;
    pcdp->pInputParam = nullptr;
    return pcdp;
}

int **NFSMixMap::AddScaleIDs(stMixCtlParams *pmixctl, int instance) {
    int numscale;
    int ntotaladded;
    int n;
    int **paddr;
    int *pIDs;

    numscale = (pmixctl->nUScaleCntSwing >> 16) & 0x1F;
    if (numscale == 0) {
        return nullptr;
    }

    ntotaladded = 0;
    pIDs = &pmixctl[1].nINPUTID;
    paddr = m_pScalePtrArray + m_ScaleParamsIDCount;
    for (n = 0; n < numscale; n++) {
        unsigned int ID;

        ID = *pIDs++;
        if ((ID & 0xFF0000) == (pmixctl->nINPUTID & 0xFF0000U)) {
            m_pScalePtrArray[m_ScaleParamsIDCount + n] = reinterpret_cast<int *>(ID | (instance << 11));
            ntotaladded++;
        } else {
            int m;
            int state;

            state = (ID >> 16) & 0xFF;
            for (m = 0; m < m_StateRefCount[state]; m++) {
                m_pScalePtrArray[m_ScaleParamsIDCount + n + m] = reinterpret_cast<int *>(ID | (m << 11));
                ntotaladded++;
            }
        }
    }

    pmixctl->nUScaleCntSwing = (pmixctl->nUScaleCntSwing & 0x00FFFFFF) | (ntotaladded << 24);
    m_ScaleParamsIDCount = m_ScaleParamsIDCount + ntotaladded;
    return paddr;
}

int **NFSMixMap::AddScaleIDs(stMixEvtParams *pevtmixctl, int instance) {
    int numscale;
    int ntotaladded;
    int n;
    int **paddr;
    int *pIDs;

    if (m_ScaleParamsIDCount == m_ScaleParamsAdded) {
        return nullptr;
    }

    numscale = (pevtmixctl->nUScaleCntSwing >> 16) & 0x1F;
    if (numscale == 0) {
        return nullptr;
    }

    ntotaladded = 0;
    pIDs = &pevtmixctl[1].nEVTCTLID;
    paddr = m_pScalePtrArray + m_ScaleParamsIDCount;
    for (n = 0; n < numscale; n++) {
        unsigned int ID;

        ID = *pIDs++;
        if ((ID & 0xFF0000) == (pevtmixctl->nEVTCTLID & 0xFF0000U)) {
            m_pScalePtrArray[m_ScaleParamsIDCount + n] = reinterpret_cast<int *>(ID | (instance << 11));
            ntotaladded++;
        } else {
            int m;
            int state;

            state = (ID >> 16) & 0xFF;
            for (m = 0; m < m_StateRefCount[state]; m++) {
                m_pScalePtrArray[m_ScaleParamsIDCount + n + m] = reinterpret_cast<int *>(ID | (m << 11));
                ntotaladded++;
            }
        }
    }

    pevtmixctl->nUScaleCntSwing = (pevtmixctl->nUScaleCntSwing & 0x00FFFFFF) | (ntotaladded << 24);
    m_ScaleParamsIDCount = m_ScaleParamsIDCount + ntotaladded;
    return paddr;
}

void NFSMixMap::AssignMixCtlDataPtrs(stMixCtlProc *pmixctl, stMixCtlParams *pparms, int nstateindex, int ctlcount) {
    int nunique;

    if (nstateindex == 0) {
        int nshared;

        nshared = m_AssignedMixCtlsShared;
        nunique = m_AssignedMixCtlsUnique;
        m_AssignedMixCtlsShared = nshared + 1;
        pmixctl->psdata = m_pMixCtlData_S + nshared;
    } else {
        int n;
        int mapType;
        int nshared;
        stMixCtlSharedData *ps;

        mapType = m_MapType;
        nshared = m_AssignedMixCtlsShared;
        nunique = m_AssignedMixCtlsUnique;
        ps = m_pMixCtlData_S;
        for (n = 0; n < nshared; n++) {
            if (ps[n].MIXCTLOBJID ==
                ((pparms->nINPUTID & 0x0F000000U) | (mapType << 8) | ((pparms->nINPUTID >> 16) & 0xE000U) |
                 (pparms->nINPUTID & 0x00FF0000) | ctlcount)) {
                pmixctl->psdata = ps + n;
            }
        }
    }

    pmixctl->pudata = m_pMixCtlData_U + nunique;
    m_AssignedMixCtlsUnique = nunique + 1;
}

void NFSMixMap::UpdateEvtMixCtls() {
    int n;
    stEvtMixCtlProc *pevtproc;

    n = 0;
    pevtproc = m_pEvtMixCtlProc;
    while (n < m_EventCtlsAdded) {
        stEvtMixCtlUniqueData *pevt_U;

        pevt_U = pevtproc->pData_U;
        if ((pevt_U->msTimeElapsed == 0.0f) && (*pevt_U->pTriggerPtr == 0)) {
            pevt_U->reset = 0;
            pevt_U->reset_level = -10000;
            pevt_U->output = 0;
            pevt_U->qoutput = 0x7FFF;
        } else {
            pevt_U->msTimeElapsed = pevt_U->msTimeElapsed + m_msDeltaTime;

            switch ((pevtproc->pData_S->pMapParms->nEVTCTLID >> 24) & 0xF) {
                case DMENV_ASR:
                    UpdateASREvent(pevtproc);
                    pevt_U = pevtproc->pData_U;
                    break;
                case DMENV_AR:
                    UpdateAREvent(pevtproc);
                    pevt_U = pevtproc->pData_U;
                    break;
                case DMENV_ATR:
                    UpdateATREvent(pevtproc);
                    pevt_U = pevtproc->pData_U;
                    break;
                case DMENV_LFO:
                    UpdateLFOEvent(pevtproc);
                    pevt_U = pevtproc->pData_U;
                    break;
            }

            if (pevt_U->ppScaleRatios) {
                int nout;
                int ns;
                unsigned int numscale;

                nout = pevt_U->output;
                ns = 0;
                numscale = (pevtproc->pData_S->pMapParms->nUScaleCntSwing >> 24) & 0xFF;
                while (ns < static_cast<int>(numscale)) {
                    nout = (nout * *pevt_U->ppScaleRatios[ns]) >> 15;
                    ns++;
                }
                pevt_U->output = nout;
            }
        }

        n++;
        pevtproc++;
    }
}

void NFSMixMap::ProcessMixMap(float dt, eCamStates camstate) {
    int n;

    m_msDeltaTime = dt * 1000.0f;
    m_fDeltaTimeRatio[1] = m_fDeltaTimeRatio[0];
    m_PrevCamState = m_CurCamState;
    m_CurCamState = camstate;
    m_fDeltaTimeRatio[0] = dt / F_DT_FRAME_LOCK;
    m_fDeltaTime = dt;

    for (n = 0; n < m_CurveProcsAdded; n++) {
        stCurveDataProc *pcvdp;
        int q15;

        pcvdp = m_pCurveDataArray + n;
        q15 = NFSMixShape::GetCurveOutput(static_cast<NFSMixShape::eMIXTABLEID>((pcvdp->nINPUTID >> 24) & 0xF),
                                         *pcvdp->pInputParam, false);
        pcvdp->Q15Output = q15;
        pcvdp->dBOutput = NFSMixShape::GetdBFromQ15(q15);
    }

    for (n = 0; n < m_MixCtlsAdded; n++) {
        int numscale;
        int nout;
        int ns;
        int scaleby;
        stMixCtlProc *pmxdp;
        stMixCtlUniqueData *pudata;

        pmxdp = m_pMixCtlProc + n;
        nout = NFSMixShape::GetdBFromQ15(
            0x7FFF - (((0x7FFF - pmxdp->pudata->pstCurveData->Q15Output) * pmxdp->psdata->nRatio) >> 15));
        nout = nout + pmxdp->psdata->nOffset;
        scaleby = 0x7FFF;
        pudata = pmxdp->pudata;
        if (!pudata->ppScaleRatios) {
            scaleby = nout * 0x7FFF;
        } else {
            numscale = (pmxdp->psdata->pstMixCtlParms->nUScaleCntSwing >> 24) & 0xFF;
            if (numscale == 0) {
                scaleby = nout * 0x7FFF;
            } else {
                for (ns = 0; ns < numscale; ns++) {
                    scaleby = (*pudata->ppScaleRatios[ns] * scaleby) >> 15;
                }
                scaleby = scaleby * nout;
            }
        }
        pudata->CmpdBOut = scaleby >> 15;
    }

    Update3DMixCtls();
    UpdateEvtMixCtls();
    UpdateSubChannels();
    UpdateMasterChannels();
    MixMasterChannels();
}

void NFSMixMap::UpdateSubChannels() {
    int nsub;

    for (nsub = 0; nsub < m_SubMixChannelsAdded; nsub++) {
        stSubMixChProc *pSChP;
        stMixChSharedData *pSChD_S;
        stMixChUniqueData *pSChD_U;

        pSChP = m_pSubChProc + nsub;
        pSChD_S = pSChP->pMixChData_S;
        pSChD_U = pSChP->pMixChData_U;
        if (pSChD_U->pInputs) {
            int mix;
            int numin;
            int nUp;
            int nDwn;
            unsigned int swing;

            numin = pSChD_S->NumInputs & 0xFF;
            pSChD_U->Output = 0;
            for (mix = 0; mix < numin; mix++) {
                pSChD_U->Output = pSChD_U->Output + *reinterpret_cast<int *>(pSChD_U->pInputs[mix]);
            }

            swing = pSChD_S->pMapParams->UpperLowerSwing;
            nUp = (swing >> 16) & 0x7FFF;
            nDwn = static_cast<short>(swing & 0xFFFF);
            if (nUp < pSChD_U->Output) {
                pSChD_U->Output = nUp;
            }
            if (pSChD_U->Output < nDwn) {
                pSChD_U->Output = nDwn;
            }
        }
    }
}

void NFSMixMap::UpdateMasterChannels() {
    int nmst;

    for (nmst = 0; nmst < m_MasterChannelsAdded; nmst++) {
        int mix;
        int numin;
        stMasterMixChProc *pMChP;
        stMasterMixChSharedData *pMChD_S;
        stMasterMixChUniqueData *pMChD_U;

        pMChP = m_pMasterChProc + nmst;
        pMChD_S = pMChP->pMixChData_S;
        pMChD_U = pMChP->pMixChData_U;
        if ((pMChD_U->pOutputs[0xF] & 1U) == 0) {
            pMChD_U->Output = -10000;
        } else {
            pMChD_U->Output = static_cast<short>(pMChD_S->pMapParams->MixData & 0xFFFF);
            if (pMChD_U->pInputs) {
                numin = pMChD_S->NumInputs & 0xFF;
                for (mix = 0; mix < numin; mix++) {
                    pMChD_U->Output = pMChD_U->Output + *reinterpret_cast<int *>(pMChD_U->pInputs[mix]);
                }
            }
        }
    }
}

void NFSMixMap::MixMasterChannels() {
    stMasterMixChProc *pMChP;
    int nmst;

    pMChP = m_pMasterChProc;
    for (nmst = 0; nmst < m_MasterChannelsAdded; nmst++, pMChP++) {
        int *pPresets;
        int NumPresets;
        int masterindex;
        int type;

        pPresets = pMChP->pMixChData_S->pPRESETS;
        masterindex = *pPresets;
        pPresets = pPresets + 1;
        NumPresets = masterindex & 0xF;
        type = (masterindex >> 24) & 0xF;

        if ((pMChP->pMixChData_U->pOutputs[0xF] & 1U) != 0) {
            int np;

            for (np = 0; np < NumPresets; np++, pPresets++) {
                int maskshift;
                int out;
                int shift;
                int *pSlot;
                int tmpvol;
                st3DMixCtlProc *p3d;
                unsigned int n3DIndex;
                unsigned int num3d;
                unsigned int preset;
                unsigned int slot;

                preset = static_cast<unsigned int>(*pPresets);
                slot = (preset >> 26) & 0x1F;
                pSlot = pMChP->pMixChData_U->pOutputs + (slot >> 1);
                shift = (slot & 1) << 4;
                maskshift = ((slot + 1) & 1) << 4;
                num3d = (pMChP->pMixChData_S->NumInputs >> 16) & 0x1F;
                n3DIndex = (preset >> 21) & 0x1F;
                tmpvol = static_cast<short>(*pPresets);

                if ((num3d != 0) && (n3DIndex < num3d)) {
                    p3d = pMChP->pMixChData_U->p3DData[n3DIndex];
                    if (*pPresets < 0) {
                        out = static_cast<unsigned short>(p3d->p3DMixCtlData_U->azimuth);
                    } else if (type == 1) {
                        out = p3d->p3DMixCtlData_U->DopplerCents + pMChP->pMixChData_U->Output + tmpvol;
                        if (out > 0x960) {
                            out = 0x960;
                        }
                        if (out < -0x12C0) {
                            out = 0;
                        }
                    } else if (type < 2) {
                        if (type == 0) {
                            out = p3d->p3DMixCtlData_U->dBRolloff + pMChP->pMixChData_U->Output + tmpvol;
                            if (out < -10000) {
                                out = -10000;
                            }
                            if (out > 0) {
                                out = 0;
                            }
                            out = NFSMixShape::GetQ15FromHundredthsdB(out);
                        } else {
                            out = pMChP->pMixChData_U->Output;
                        }
                    } else if (type == 2) {
                        out = pMChP->pMixChData_U->Output + tmpvol;
                        if (out < -10000) {
                            out = -10000;
                        }
                        if (out > 0) {
                            out = 0;
                        }
                    } else if (type == 4) {
                        out = p3d->p3DMixCtlData_U->dBRolloff + pMChP->pMixChData_U->Output + tmpvol;
                        if (out < -10000) {
                            out = -10000;
                        }
                        if (out > 0) {
                            out = 0;
                        }
                        out = NFSMixShape::GetQ15FromHundredthsdB(out);
                    } else {
                        out = pMChP->pMixChData_U->Output;
                    }
                } else {
                    out = pMChP->pMixChData_U->Output + tmpvol;
                    if (type == 1) {
                        if (out > 0x960) {
                            out = 0x960;
                        }
                        if (out < -0x12C0) {
                            out = -0x12C0;
                        }
                    } else if (type < 2) {
                        if (type == 0) {
                            if (out < -10000) {
                                out = -10000;
                            }
                            if (out > 0) {
                                out = 0;
                            }
                            out = NFSMixShape::GetQ15FromHundredthsdB(out);
                        }
                    } else {
                        if (type == 2) {
                            if (out < -10000) {
                                out = -10000;
                            }
                            if (out > 0) {
                                out = 0;
                            }
                            out = static_cast<int>(NFSMixShape::GetPitchMultFromCents(out) * 25000.0f);
                        } else if (type == 4) {
                            if (out < -10000) {
                                out = -10000;
                            }
                            if (out > 0) {
                                out = 0;
                            }
                            out = NFSMixShape::GetQ15FromHundredthsdB(out);
                        } else {
                            if (out < 0) {
                                out = 0;
                            }
                            if (out > 25000) {
                                out = 25000;
                            }
                        }
                    }
                }

                *pSlot = (*pSlot & (0xFFFF << maskshift)) | ((out & 0xFFFF) << shift);
            }
        } else {
            int out;
            int *pSlot;
            unsigned int slot;

            if (type == 1) {
                out = 0;
            } else if (type == 2) {
                out = 25000;
            } else {
                out = -10000;
            }

            slot = (static_cast<unsigned int>(*pPresets) >> 26) & 0x1F;
            pSlot = pMChP->pMixChData_U->pOutputs + (slot >> 1);
            *pSlot = (*pSlot & (0xFFFF << (((slot + 1) & 1) << 4))) | ((out & 0xFFFF) << ((slot & 1) << 4));
        }
    }
}

void NFSMixMap::Update3DMixCtls() {
    int n;
    int i;

    if (m_CurCamState != m_PrevCamState && m_nAssigned3DMixCtlShared > 0) {
        st3DMixCtlSharedData *p3dsp = m_p3DMixCtlData_S;

        for (i = 0; i < m_nAssigned3DMixCtlShared; i++) {
            int found;
            int numstates;
            eCamStates testcamstate;

            found = 0;
            numstates = (p3dsp->pMapParams->nINPUTID >> 24) & 0xF;
            testcamstate = m_CurCamState;

        RestartLoop:
            for (n = 0; n < numstates; n++) {
                st3DStateParams *pstateparams;

                pstateparams = (&p3dsp->pMapParams->StateParams) + n;
                if (((pstateparams->n3DSTATEINFOID >> 24) & 0xF) == testcamstate) {
                    p3dsp->pCurStateParams = pstateparams;
                    p3dsp->msSinceCamTrans = 0;
                    p3dsp->PrevCamState = m_PrevCamState;
                    p3dsp->CurCamState = m_CurCamState;
                    found = 1;
                    break;
                }
            }

            if (!found && testcamstate != DMIX_DEFAULT_CAM) {
                testcamstate = DMIX_DEFAULT_CAM;
                goto RestartLoop;
            }

            p3dsp++;
        }
    }

    st3DMixCtlProc *p3Dproc = m_p3DMixCtlProc;

    for (n = 0; n < m_3DMixCtlsAdded; n++, p3Dproc++) {
        st3DMixCtlUniqueData *p3Du;

        p3Du = p3Dproc->p3DMixCtlData_U;
        if (p3Du->pInputs[0xF] & 1U) {
            st3DStateParams *psparams;
            int nid;
            int nDistType;
            int nAzimType;
            int ntables;
            int nazim;
            float fdist[2];
            float fmindist[2];
            float fmaxdist[2];
            int uAverage;
            eMIXTABLEID nqOne;
            eMIXTABLEID nqTwo;
            int AzimOut;
            int nQuad;
            int qDist[2];

            nqOne = SHAPE_DWN_LINEAR;
            nqTwo = SHAPE_DWN_LINEAR;
            psparams = p3Dproc->p3DMixCtlData_S->pCurStateParams;
            nid = psparams->n3DSTATEINFOID;
            nDistType = (nid >> 12) & 0xF;
            nAzimType = (nid >> 8) & 0xF;
            ntables = psparams->nCURVEID_DOPPLER;
            if (nDistType == 0) {
                fdist[0] = static_cast<float>(p3Du->pInputs[1]) * 0.01f;
            } else if (nDistType == 1) {
                fdist[0] = static_cast<float>(p3Du->pInputs[0]) * 0.01f;
            } else {
                fdist[0] = -1.0f;
            }
            fdist[1] = fdist[0];

            if (nAzimType == 0) {
                nazim = p3Du->pInputs[3];
            } else if (nAzimType == 1) {
                nazim = p3Du->pInputs[2];
            } else {
                nazim = 0;
            }

            AzimOut = nazim;
            p3Du->azimuth = AzimOut;
            nQuad = (static_cast<unsigned int>(AzimOut) >> 14) & 3;
            uAverage = AzimOut;

            switch (nQuad) {
            case 1:
                nqTwo = static_cast<eMIXTABLEID>((ntables >> 24) & 0xF);
                nqOne = static_cast<eMIXTABLEID>((ntables >> 16) & 0xF);
                fmindist[0] = static_cast<float>(psparams->nQ1MinMax & 0x7FFF);
                fmaxdist[0] = static_cast<float>((static_cast<unsigned int>(psparams->nQ1MinMax) >> 16) & 0x7FFF);
                fmindist[1] = static_cast<float>(psparams->nQ2MinMax & 0x7FFF);
                fmaxdist[1] = static_cast<float>((static_cast<unsigned int>(psparams->nQ2MinMax) >> 16) & 0x7FFF);
                uAverage = AzimOut - 0x4000;
                break;

            case 2:
                nqTwo = static_cast<eMIXTABLEID>((ntables >> 20) & 0xF);
                nqOne = static_cast<eMIXTABLEID>((ntables >> 24) & 0xF);
                fmindist[0] = static_cast<float>(psparams->nQ2MinMax & 0x7FFF);
                fmaxdist[0] = static_cast<float>((static_cast<unsigned int>(psparams->nQ2MinMax) >> 16) & 0x7FFF);
                fmindist[1] = static_cast<float>(psparams->nQ3MinMax & 0x7FFF);
                fmaxdist[1] = static_cast<float>((static_cast<unsigned int>(psparams->nQ3MinMax) >> 16) & 0x7FFF);
                uAverage = AzimOut - 0x8000;
                break;

            case 3:
                nqTwo = static_cast<eMIXTABLEID>((ntables >> 28) & 0xF);
                nqOne = static_cast<eMIXTABLEID>((ntables >> 20) & 0xF);
                fmindist[0] = static_cast<float>(psparams->nQ3MinMax & 0x7FFF);
                fmaxdist[0] = static_cast<float>((static_cast<unsigned int>(psparams->nQ3MinMax) >> 16) & 0x7FFF);
                fmindist[1] = static_cast<float>(psparams->nQ0MinMax & 0x7FFF);
                fmaxdist[1] = static_cast<float>((static_cast<unsigned int>(psparams->nQ0MinMax) >> 16) & 0x7FFF);
                uAverage = AzimOut - 0xC000;
                break;

            default:
                nqTwo = static_cast<eMIXTABLEID>((ntables >> 16) & 0xF);
                nqOne = static_cast<eMIXTABLEID>((ntables >> 28) & 0xF);
                fmindist[0] = static_cast<float>(psparams->nQ0MinMax & 0x7FFF);
                fmaxdist[0] = static_cast<float>((static_cast<unsigned int>(psparams->nQ0MinMax) >> 16) & 0x7FFF);
                fmindist[1] = static_cast<float>(psparams->nQ1MinMax & 0x7FFF);
                fmaxdist[1] = static_cast<float>((static_cast<unsigned int>(psparams->nQ1MinMax) >> 16) & 0x7FFF);
                break;
            }

            if ((fdist[0] <= fmaxdist[0]) || (fdist[0] <= fmaxdist[1])) {
                float fdistratio;

                if (fdist[0] < fmindist[0]) {
                    fdist[0] = fmindist[0];
                }
                if (fmaxdist[0] < fdist[0]) {
                    fdist[0] = fmaxdist[0];
                }

                if (fdist[1] < fmindist[1]) {
                    fdist[1] = fmindist[1];
                }
                if (fmaxdist[1] < fdist[1]) {
                    fdist[1] = fmaxdist[1];
                }

                fdistratio = (fdist[0] - fmindist[0]) / (fmaxdist[0] - fmindist[0]);
                fdist[1] = (fdist[1] - fmindist[1]) / (fmaxdist[1] - fmindist[1]);
                qDist[0] = static_cast<int>(fdistratio * 32767.0f);
                qDist[1] = static_cast<int>(fdist[1] * 32767.0f);
                p3Du->q15Rolloff = NFSMixShape::GetAzimShapeOutput(nqOne, nqTwo, qDist, uAverage);
                p3Du->dBRolloff = NFSMixShape::GetdBFromQ15(p3Du->q15Rolloff);

                if ((ntables & 0xFFFF) != 0) {
                    float fcents;
                    float fcurdist;
                    float fdeltanewdist;
                    float ftratio;
                    float fvel;
                    float fvelsound;
                    unsigned int velocity;

                    fvelsound = static_cast<float>(ntables & 0xFFFF);
                    fcents = 0.0f;

                    if ((m_CurCamState < 3) || (m_CurCamState == DMIX_NFS_NIS_CAM)) {
                        velocity = static_cast<unsigned int>(p3Du->pInputs[0xD]);
                        fcurdist = static_cast<float>(p3Du->pInputs[0]) * 0.01f;
                        if (p3Du->pInputs[0xF] < 0) {
                            velocity = static_cast<unsigned int>(p3Du->pInputs[0xF] & 0x7FFFFFFF);
                            p3Du->pInputs[0xF] = velocity;
                        } else {
                            fvel = fvelsound + (static_cast<float>(velocity) * 0.01f);
                            if (fvel <= 0.01f) {
                                fvel = fvelsound;
                            }
                            ftratio = fvelsound / fvel;
                            fcents = static_cast<float>(NFSMixShape::GetCentsFromPitchMult(ftratio));
                        }
                    } else {
                        velocity = static_cast<unsigned int>(p3Du->pInputs[0xE]);
                        fcurdist = static_cast<float>(p3Du->pInputs[1]) * 0.01f;
                        if ((p3Du->pInputs[0xF] & 0x40000000) != 0) {
                            velocity = static_cast<unsigned int>(p3Du->pInputs[0xF] & 0xBFFFFFFF);
                            p3Du->pInputs[0xF] = velocity;
                        } else {
                            fvel = fvelsound + (static_cast<float>(velocity) * 0.01f);
                            if (fvel <= 0.01f) {
                                fvel = fvelsound;
                            }
                            ftratio = fvelsound / fvel;
                            fcents = static_cast<float>(NFSMixShape::GetCentsFromPitchMult(ftratio));
                        }
                    }

                    if (p3Du->fPrevDeltaDist == 0.01f) {
                        p3Du->fPrevDeltaDist = 1.0f;
                    }

                    fdeltanewdist = fcurdist - p3Du->fPrevDist;
                    if (fdeltanewdist < 0.0f) {
                        fdeltanewdist = -fdeltanewdist;
                    }

                    p3Du->fPrevDeltaDist = fdeltanewdist;
                    p3Du->fPrevDist = fcurdist;
                    p3Du->DopplerCents =
                        p3Du->DopplerCents +
                        static_cast<int>((fcents - static_cast<float>(p3Du->DopplerCents)) * DOPPLER_SMOOTHING_FACTOR);
                }
            } else {
                p3Du->dBRolloff = -10000;
                p3Du->q15Rolloff = 0;
                p3Du->DopplerCents = 0;
            }
        } else {
            p3Du->dBRolloff = -10000;
            p3Du->q15Rolloff = 0;
            p3Du->azimuth = 0;
            p3Du->DopplerCents = 0;
        }
    }
}

void NFSMixMap::UpdateASREvent(stEvtMixCtlProc *pProc) {
    float ftstage_0;
    NFSMixShape::eMIXTABLEID ncurvestage_0;
    float ftstage_1;
    float ftstage_2;
    NFSMixShape::eMIXTABLEID ncurvestage_2;
    int nSwing;
    float nratio;

    ncurvestage_0 = static_cast<NFSMixShape::eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_00 >> 12) & 0xF);
    ftstage_0 = static_cast<float>(pProc->pData_S->pMapParms->nParam_00 & 0xFFF) * 16.66667f;
    ftstage_1 = static_cast<float>(pProc->pData_S->pMapParms->nParam_01 & 0xFFF) * 16.66667f;
    ncurvestage_2 = static_cast<NFSMixShape::eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_02 >> 12) & 0xF);
    ftstage_2 = static_cast<float>(pProc->pData_S->pMapParms->nParam_02 & 0xFFF) * 16.66667f;
    nSwing = pProc->pData_S->pMapParms->nUScaleCntSwing & 0xFFFF;
    if ((pProc->pData_S->pMapParms->nUScaleCntSwing & 0x8000) != 0) {
        nSwing |= 0xFFFF0000;
    }
    nratio = pProc->pData_U->msTimeElapsed;

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
    } else {
        nratio = nratio - ftstage_0;
        if (ftstage_1 < nratio) {
            nratio = nratio - ftstage_1;
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
                goto set_output;
            }
        } else {
            pProc->pData_U->qoutput = 0;
            goto set_output;
        }
    }

set_output:
    pProc->pData_U->output = static_cast<int>(((32767.0f - static_cast<float>(pProc->pData_U->qoutput)) * 3.051851e-05f) *
                                       static_cast<float>(nSwing));
}

void NFSMixMap::UpdateATREvent(stEvtMixCtlProc *pProc) {
    float ftstage_0;
    NFSMixShape::eMIXTABLEID ncurvestage_0;
    float ftstage_2;
    NFSMixShape::eMIXTABLEID ncurvestage_2;
    int nSwing;
    float nratio;

    ncurvestage_0 = static_cast<NFSMixShape::eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_00 >> 12) & 0xF);
    ftstage_0 = static_cast<float>(pProc->pData_S->pMapParms->nParam_00 & 0xFFF) * 16.66667f;
    ncurvestage_2 = static_cast<NFSMixShape::eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_02 >> 12) & 0xF);
    ftstage_2 = static_cast<float>(pProc->pData_S->pMapParms->nParam_02 & 0xFFF) * 16.66667f;
    nSwing = pProc->pData_S->pMapParms->nUScaleCntSwing & 0xFFFF;
    if ((pProc->pData_S->pMapParms->nUScaleCntSwing & 0x8000) != 0) {
        nSwing |= 0xFFFF0000;
    }
    nratio = pProc->pData_U->msTimeElapsed;

    if (nratio < ftstage_0) {
        if (*pProc->pData_U->pTriggerPtr == 0) {
            pProc->pData_U->reset = 1;
            pProc->pData_U->reset_level = pProc->pData_U->output;
            pProc->pData_U->msTimeElapsed = ftstage_0;
            return;
        }
        nratio = (nratio * 32767.0f) / ftstage_0;
        {
            int ndt = static_cast<int>(nratio);

            if (nSwing < 0) {
                pProc->pData_U->qoutput = NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
            } else {
                pProc->pData_U->qoutput = 0x7FFF - NFSMixShape::GetCurveOutput(ncurvestage_0, ndt, false);
            }
        }
    } else {
        if (*pProc->pData_U->pTriggerPtr == 0) {
            if (pProc->pData_U->msResetTime == 0.0f) {
                pProc->pData_U->msResetTime = nratio;
            }
            nratio = pProc->pData_U->msTimeElapsed - pProc->pData_U->msResetTime;
            if (ftstage_2 < nratio) {
                pProc->pData_U->reset = 0;
                pProc->pData_U->reset_level = 0;
                pProc->pData_U->msTimeElapsed = 0.0f;
                pProc->pData_U->msResetTime = 0.0f;
                pProc->pData_U->qoutput = 0x7FFF;
                return;
            }
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
            if (*pProc->pData_U->pTriggerPtr == 1 && pProc->pData_U->msResetTime != 0.0f) {
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
            goto set_output;
        }
    }

set_output:
    if (pProc->pData_U->reset != 0) {
        if (*pProc->pData_U->pTriggerPtr == 0) {
            pProc->pData_U->output =
                static_cast<int>(((32767.0f - static_cast<float>(pProc->pData_U->qoutput)) * 3.051851e-05f) *
                                 static_cast<float>(pProc->pData_U->reset_level));
            return;
        }
        pProc->pData_U->output =
            static_cast<int>(((32767.0f - static_cast<float>(pProc->pData_U->qoutput)) * 3.051851e-05f) *
                             static_cast<float>(nSwing - pProc->pData_U->reset_level)) +
            pProc->pData_U->reset_level;
        return;
    }
    pProc->pData_U->output =
        static_cast<int>(((32767.0f - static_cast<float>(pProc->pData_U->qoutput)) * 3.0517578e-05f) *
                         static_cast<float>(nSwing));
    return;
}

void NFSMixMap::UpdateAREvent(stEvtMixCtlProc *pProc) {
    float ftstage_0;
    NFSMixShape::eMIXTABLEID ncurvestage_0;
    float ftstage_2;
    NFSMixShape::eMIXTABLEID ncurvestage_2;
    int nSwing;
    float nratio;

    ncurvestage_0 = static_cast<NFSMixShape::eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_00 >> 12) & 0xF);
    ftstage_0 = static_cast<float>(pProc->pData_S->pMapParms->nParam_00 & 0xFFF) * 16.66667f;
    ncurvestage_2 = static_cast<NFSMixShape::eMIXTABLEID>((pProc->pData_S->pMapParms->nParam_02 >> 12) & 0xF);
    ftstage_2 = static_cast<float>(pProc->pData_S->pMapParms->nParam_02 & 0xFFF) * 16.66667f;
    nSwing = pProc->pData_S->pMapParms->nUScaleCntSwing & 0xFFFF;
    if ((pProc->pData_S->pMapParms->nUScaleCntSwing & 0x8000) != 0) {
        nSwing |= 0xFFFF0000;
    }
    nratio = pProc->pData_U->msTimeElapsed;

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
    } else {
        nratio = nratio - ftstage_0;
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
    }

set_output:
    pProc->pData_U->output = static_cast<int>(((32767.0f - static_cast<float>(pProc->pData_U->qoutput)) * 3.051851e-05f) *
                                              static_cast<float>(nSwing));
}

void NFSMixMap::UpdateLFOEvent(stEvtMixCtlProc *pProc) {}
