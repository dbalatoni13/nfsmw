#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"

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
