#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"

int *(*NFSMixMap::mGetOutPtrCB)(int) = nullptr;
void (*NFSMixMap::mSetSFXOutCB)(int, int *) = nullptr;
bool (*NFSMixMap::mSetSFXInCB)(int, int *) = nullptr;
int (*NFSMixMap::mGetStateRefCnt)(int) = nullptr;
void (*NFSMixMap::mMapReadyCB)() = nullptr;

NFSMixMap::NFSMixMap()
    : AudioMemBase() {}

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
    unsigned int stateIndex;
    int *pMixMap;
    char *pMixMapBase;
    int *pStateOffsetTable;

    stateIndex = 0;
    pMixMap = m_pMixMap;
    pMixMapBase = reinterpret_cast<char *>(pMixMap);
    pStateOffsetTable = reinterpret_cast<int *>(pMixMapBase + pMixMap[2]);

    ResetMapData();
    SetupStateRefCount();
    m_nStateMapCount = 0;

    if (pMixMap[1] > 0) {
        unsigned int nextState;

        do {
            int stateOffset;

            stateOffset = *pStateOffsetTable++;
            nextState = stateIndex + 1;

            if (stateOffset != -1) {
                int stateRefCount;
                int mixCtlOffset;
                int threeDMixCtlOffset;
                int eventCtlOffset;
                int subMixOffset;
                int masterMixOffset;

                stateRefCount = m_StateRefCount[stateIndex];
                m_nStateMapCount += stateRefCount;

                mixCtlOffset = *reinterpret_cast<int *>(pMixMapBase + stateOffset + 4);
                if (mixCtlOffset != -1) {
                    int *pMixCtlHeader;
                    unsigned int *pCurveWord;
                    unsigned int *pCurveIds;
                    int ctlCount;
                    int ctlIndex;

                    pMixCtlHeader = reinterpret_cast<int *>(pMixMapBase + mixCtlOffset + stateOffset);
                    ctlCount = pMixCtlHeader[0];
                    pCurveWord = reinterpret_cast<unsigned int *>(pMixCtlHeader + 4);
                    pCurveIds = static_cast<unsigned int *>(
                        gAudioMemoryManager.AllocateMemory(ctlCount << 2, "Temp MIXMAP ALLOC", false));

                    m_MixCtlsAdded += stateRefCount * ctlCount;
                    m_SharedMixCtlCount += ctlCount;
                    m_DataProcsAdded += stateRefCount * pMixCtlHeader[1];

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
                            if (pCurveIds[existingCurveIndex] == curveId) {
                                uniqueCurve = false;
                            }
                        }

                        if (uniqueCurve) {
                            m_CurveProcsTotal[curveType][0] += stateRefCount;
                        }

                        scaleParamsAdded = 0;
                        pScaleWord = pCurveWord + 2;
                        for (scaleIndex = 0; scaleIndex < scaleCount; scaleIndex++) {
                            unsigned char inputState;

                            inputState = *(reinterpret_cast<unsigned char *>(pScaleWord) + 1);
                            if (inputState == stateIndex) {
                                scaleParamsAdded++;
                            } else {
                                scaleParamsAdded += m_StateRefCount[inputState];
                            }

                            pScaleWord++;
                        }

                        pCurveWord += scaleCount + 2;
                        m_ScaleParamsAdded += scaleParamsAdded * stateRefCount;
                    }

                    gAudioMemoryManager.FreeMemory(pCurveIds);
                }

                threeDMixCtlOffset = *reinterpret_cast<int *>(pMixMapBase + stateOffset + 8);
                if (threeDMixCtlOffset != -1) {
                    int ctlCount;
                    int totalCamStates;
                    int ctlIndex;
                    unsigned int *p3DMixCtlWord;

                    ctlCount = *reinterpret_cast<int *>(pMixMapBase + threeDMixCtlOffset + stateOffset);
                    p3DMixCtlWord =
                        reinterpret_cast<unsigned int *>(pMixMapBase + threeDMixCtlOffset + stateOffset + 0x10);
                    totalCamStates = 0;

                    m_Shared3DMixCtlCount += ctlCount & 0xFF;
                    m_3DMixCtlsAdded += stateRefCount * (ctlCount & 0xFF);

                    for (ctlIndex = 0; ctlIndex < ctlCount; ctlIndex++) {
                        int camStatesAdded;

                        camStatesAdded = (*p3DMixCtlWord >> 24) & 0xF;
                        totalCamStates += camStatesAdded;
                        p3DMixCtlWord += camStatesAdded * 6;
                    }

                    m_n3DCamStatesAdded += totalCamStates;
                }

                eventCtlOffset = *reinterpret_cast<int *>(pMixMapBase + stateOffset + 0x18);
                if (eventCtlOffset != -1) {
                    int ctlCount;
                    int ctlIndex;
                    int scaleParamsAdded;
                    char *pEventCtlData;

                    ctlCount = *reinterpret_cast<int *>(pMixMapBase + eventCtlOffset + stateOffset);
                    pEventCtlData = pMixMapBase + eventCtlOffset + stateOffset + 0x10;
                    scaleParamsAdded = 0;

                    m_EventCtlsAdded += stateRefCount * ctlCount;
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
                            if (inputState == stateIndex) {
                                scaleParamsAdded++;
                            } else {
                                scaleParamsAdded += m_StateRefCount[inputState];
                            }

                            pScaleData += 4;
                        }

                        pEventCtlData += (scaleCount * 4) + 0x18;
                    }

                    m_ScaleParamsAdded += scaleParamsAdded * stateRefCount;
                }

                subMixOffset = *reinterpret_cast<int *>(pMixMapBase + stateOffset + 0xC);
                if (subMixOffset != -1) {
                    int *pSubMixHeader;
                    int subMixCount;
                    int subMixIndex;
                    int *pInputData;

                    pSubMixHeader = reinterpret_cast<int *>(pMixMapBase + subMixOffset + stateOffset);
                    subMixCount = pSubMixHeader[0];
                    pInputData = pSubMixHeader + 4;

                    m_SharedSubMixCount += subMixCount;
                    m_SubMixChannelsAdded += stateRefCount * subMixCount;

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
                            if (inputState == stateIndex) {
                                totalInputs++;
                            } else {
                                totalInputs += m_StateRefCount[inputState];
                            }

                            pInputData++;
                        }

                        m_nTotalSubChannelInputs += totalInputs * stateRefCount;
                    }
                }

                masterMixOffset = *reinterpret_cast<int *>(pMixMapBase + stateOffset + 0x10);
                if (masterMixOffset != -1) {
                    int *pMasterMixHeader;
                    int masterMixCount;
                    int masterMixIndex;
                    int *pInputData;

                    pMasterMixHeader = reinterpret_cast<int *>(pMixMapBase + masterMixOffset + stateOffset);
                    masterMixCount = pMasterMixHeader[0];
                    pInputData = pMasterMixHeader + 4;

                    m_nTotalUniqueMasterChannels += pMasterMixHeader[1] + (stateRefCount * pMasterMixHeader[1]);
                    m_SharedMasterMixCount += masterMixCount;
                    m_MasterChannelsAdded += stateRefCount * masterMixCount;

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
                            if (inputState == stateIndex) {
                                totalInputs++;
                            } else {
                                totalInputs += m_StateRefCount[inputState];
                            }

                            pInputData++;
                        }

                        m_nTotalMasterChannelInputs += totalInputs * stateRefCount;
                    }
                }
            }

            stateIndex = nextState;
        } while (static_cast<int>(nextState) < pMixMap[1]);
    }

    for (int curveType = 0; curveType < 10; curveType++) {
        m_CurveProcsAdded += m_CurveProcsTotal[curveType][0];
    }
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

void NFSMixMap::InitMainMapStates() {
    SetupStateProcArrays();
    ConnectMixMap();
}
