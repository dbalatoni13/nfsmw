#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"

extern "C" int GetQ15FromHundredthsdB__11NFSMixShapei(int ndB);

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

void NFSMixMapState::CreateMixCtls() {
    int offset;

    offset = m_pMMStateHdr->OffsetMixCtlData;
    m_MixCtlsAdded = 0;

    if (offset > -1) {
        stMixCtlHdr *pHdr;

        pHdr = reinterpret_cast<stMixCtlHdr *>(reinterpret_cast<char *>(&m_pMMStateHdr->StateIndex) + offset);
        m_pMixCtlHdr = pHdr;
        if (pHdr->NumMixCtls > 0) {
            stMixCtlParams *pparams;
            unsigned int ctlcount;

            pparams = reinterpret_cast<stMixCtlParams *>(pHdr + 1);
            ctlcount = 0;
            m_MixStateParams.pMixCtlProcs = m_pNFSMixMap->GetProcessMixCtlPtr(false);
            while (static_cast<int>(ctlcount) < m_pMixCtlHdr->NumMixCtls) {
                int q15ratio;
                unsigned int scaleInfo;
                stMixCtlParams mixctlparm;
                stMixCtlProc *pmcp;
                stCurveDataProc *pcurvedata;
                int **pscaleaddr;

                mixctlparm.nUScaleCntSwing = pparams->nUScaleCntSwing;
                mixctlparm.nINPUTID = (pparams->nINPUTID & 0xFFFF07FFU) | (m_ObjectIndex << 11);
                pcurvedata = m_pNFSMixMap->GetCurveDataPtr(&mixctlparm);
                pscaleaddr = m_pNFSMixMap->AddScaleIDs(pparams, m_ObjectIndex);
                pmcp = m_pNFSMixMap->GetProcessMixCtlPtr(true);
                m_pNFSMixMap->AssignMixCtlDataPtrs(pmcp, pparams, m_ObjectIndex, ctlcount);
                pmcp->psdata->MIXCTLOBJID =
                    (pparams->nINPUTID & 0x0F000000) | (m_pNFSMixMap->m_MapType << 8) |
                    ((pparams->nINPUTID >> 16) & 0xE000U) | (pparams->nINPUTID & 0x00FF0000) | ctlcount;
                pmcp->psdata->nOffset = 0;
                pmcp->psdata->nRatio = 0;
                pmcp->psdata->pstMixCtlParms = pparams;

                scaleInfo = pparams->nUScaleCntSwing;
                if ((scaleInfo & 0x8000) == 0) {
                    pmcp->psdata->nOffset = scaleInfo & 0x7FFF;
                    scaleInfo = -(scaleInfo & 0x7FFF);
                } else {
                    scaleInfo = (scaleInfo & 0xFFFF) | 0xFFFF0000;
                }

                q15ratio = GetQ15FromHundredthsdB__11NFSMixShapei(scaleInfo);
                pmcp->psdata->nRatio = 0x7FFF - q15ratio;
                pmcp->pudata->CmpdBOut = 0;
                pmcp->pudata->ppScaleRatios = pscaleaddr;
                pmcp->pudata->pstCurveData = pcurvedata;
                m_MixCtlsAdded++;
                pparams = reinterpret_cast<stMixCtlParams *>(
                    reinterpret_cast<char *>(&pparams[1].nINPUTID) + ((mixctlparm.nUScaleCntSwing >> 14) & 0x7CU));
                ctlcount++;
            }
        }
    }
}

void NFSMixMapState::Create3DMixCtls() {
    int offset;

    m_3DMixCtlsAdded = 0;
    offset = m_pMMStateHdr->Offset3DMixCtlData;

    if (offset > -1) {
        st3DMixCtlHdr *pHdr;

        pHdr = reinterpret_cast<st3DMixCtlHdr *>(reinterpret_cast<char *>(&m_pMMStateHdr->StateIndex) + offset);
        m_p3DMixCtlHdr = pHdr;
        if (pHdr->Num3DMixCtls > 0) {
            int n;
            st3DMixCtlParams *pparams;

            pparams = reinterpret_cast<st3DMixCtlParams *>(pHdr + 1);
            n = 0;
            m_MixStateParams.p3DMixCtlProc = m_pNFSMixMap->GetNext3DMixCtlProc(false);
            while (n < m_p3DMixCtlHdr->Num3DMixCtls) {
                st3DMixCtlProc *p3DCP;
                st3DMixCtlSharedData *p3DSD;
                st3DMixCtlUniqueData *p3DUD;

                if (m_ObjectIndex == 0) {
                    p3DSD = m_pNFSMixMap->GetNext3DMixCtlShared(true);
                    p3DSD->pMapParams = pparams;
                    p3DSD->msSinceCamTrans = 0;
                    p3DSD->CurCamState = 0;
                    p3DSD->PrevCamState = 0;
                } else {
                    p3DSD = m_pFirstInstance->m_MixStateParams.p3DMixCtlProc[n].p3DMixCtlData_S;
                }

                n++;
                p3DCP = m_pNFSMixMap->GetNext3DMixCtlProc(true);
                p3DUD = m_pNFSMixMap->GetNext3DMixCtlUnique(true);
                p3DUD->azimuth = 0;
                p3DUD->q15Rolloff = 0x7FFF;
                p3DUD->dBRolloff = 0;
                p3DUD->DopplerCents = 0;
                p3DSD->pCurStateParams = &pparams->StateParams;
                p3DCP->p3DMixCtlData_U = p3DUD;
                p3DUD->nINPUTID = (m_ObjectIndex << 11) | (p3DSD->pMapParams->nINPUTID & 0xFFFF07FF);
                p3DUD->fPrevDist = 1.0f;
                p3DCP->p3DMixCtlData_S = p3DSD;
                m_3DMixCtlsAdded++;
                p3DUD->fPrevDeltaDist = 1.0f;
                p3DUD->pInputs = reinterpret_cast<int *>(
                    ((p3DSD->pMapParams->nINPUTID | (m_ObjectIndex << 11)) & 0x1FFFFFFFU) | 0x60000000);
                pparams = reinterpret_cast<st3DMixCtlParams *>(
                    &pparams->StateParams + ((pparams->nINPUTID >> 24) & 0xF));
            }
        }
    }
}
