#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"

NFSMixMapState::NFSMixMapState() {}

NFSMixMapState::~NFSMixMapState() {}

int NFSMixMapState::GetStateRefCount() {
    return m_pFirstInstance->m_ThisStateRefCnt;
}

NFSMixMapState *NFSMixMapState::GetMixMapProc(int refcnt) {
    return m_pFirstInstance + refcnt;
}

void NFSMixMapState::SetFirstStateInst(NFSMixMapState *pstate) {
    m_pFirstInstance = pstate;
}

void NFSMixMapState::Initialize(NFSMixMap *pmm, int stateindex, int numstatecopies, int objnum) {
    m_pNFSMixMap = pmm;
    m_StateIndex = stateindex;
    m_NumStateCopies = numstatecopies;
    m_ObjectIndex = objnum;

    if (objnum == 0) {
        m_ThisStateRefCnt = 1;
        return;
    }

    m_pFirstInstance->m_ThisStateRefCnt = m_pFirstInstance->m_ThisStateRefCnt + 1;
    if (objnum > 0) {
        int n;

        for (n = 0; n < objnum; n++) {
            m_pFirstInstance[n].m_ThisStateRefCnt = m_pFirstInstance->m_ThisStateRefCnt;
        }
    }
}

stMixCtlProc *NFSMixMapState::GetMixCtlProc(int nMIXCTLIN_ID, int nInstance) {
    NFSMixMapState *pstate;

    pstate = m_pFirstInstance + nInstance;
    if (pstate && ((nMIXCTLIN_ID & 0xFFU) < pstate->m_MixCtlsAdded)) {
        return pstate->m_MixStateParams.pMixCtlProcs + (nMIXCTLIN_ID & 0xFFU);
    }

    return nullptr;
}

st3DMixCtlProc *NFSMixMapState::Get3DMixCtlProc(int n3DMIXCTLCREATE_ID, int nInstance) {
    NFSMixMapState *pstate;

    pstate = m_pFirstInstance + nInstance;
    if (pstate && ((n3DMIXCTLCREATE_ID & 0xFFU) < pstate->m_3DMixCtlsAdded)) {
        return pstate->m_MixStateParams.p3DMixCtlProc + (n3DMIXCTLCREATE_ID & 0xFFU);
    }

    return nullptr;
}

stEvtMixCtlProc *NFSMixMapState::GetEvtMixCtlProc(int nEVENTID, int nInstance) {
    NFSMixMapState *pstate;

    pstate = m_pFirstInstance + nInstance;
    if (pstate && ((nEVENTID & 0xFFU) < pstate->m_EvtMixCtlsAdded)) {
        return pstate->m_MixStateParams.pEvtMixCtlProc + (nEVENTID & 0xFFU);
    }

    return nullptr;
}

stSubMixChProc *NFSMixMapState::GetSubMixChProc(int nMIXCHIN_ID, int nInstance) {
    NFSMixMapState *pstate;

    pstate = m_pFirstInstance + nInstance;
    if (pstate && ((nMIXCHIN_ID & 0xFFU) < pstate->m_SubMixChannelsAdded)) {
        return pstate->m_MixStateParams.pSubMixChProcs + (nMIXCHIN_ID & 0xFFU);
    }

    return nullptr;
}

stMasterMixChProc *NFSMixMapState::GetMasterMixChProc(int nMIXCHIN_ID, int nInstance) {
    NFSMixMapState *pstate;

    pstate = m_pFirstInstance + nInstance;
    if (pstate && ((nMIXCHIN_ID & 0xFFU) < pstate->m_MasterChannelsAdded)) {
        return pstate->m_MixStateParams.pMasterMixChProcs + (nMIXCHIN_ID & 0xFFU);
    }

    return nullptr;
}
