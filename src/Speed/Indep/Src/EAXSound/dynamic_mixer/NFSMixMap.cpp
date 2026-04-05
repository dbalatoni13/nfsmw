#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"

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
