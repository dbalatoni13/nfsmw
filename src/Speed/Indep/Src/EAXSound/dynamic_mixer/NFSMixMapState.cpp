#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMapState.hpp"
#include "Speed/Indep/Src/EAXSound/dynamic_mixer/NFSMixMap.hpp"


NFSMixMapState::NFSMixMapState() {}

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

int NFSMixMapState::GetStateRefCount() {
    return m_pFirstInstance->m_ThisStateRefCnt;
}

NFSMixMapState::~NFSMixMapState() {}

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

                q15ratio = NFSMixShape::GetQ15FromHundredthsdB(scaleInfo);
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

void NFSMixMapState::CreateSubMixChannels() {
    int offset;

    offset = m_pMMStateHdr->OffsetSubMixData;
    m_SubMixChannelsAdded = 0;

    if (offset >= 0) {
        m_pSubChHdr = reinterpret_cast<stMixChHdr *>(reinterpret_cast<char *>(&m_pMMStateHdr->StateIndex) + offset);
        if (m_pSubChHdr->NumMixChannels > 0) {
            stSubMixChParams *pSubMixParms;
            {
                int n;

                n = 0;
                m_MixStateParams.pSubMixChProcs = m_pNFSMixMap->GetNextSubMixProc(false);
                pSubMixParms = reinterpret_cast<stSubMixChParams *>(m_pSubChHdr + 1);
                while (n < m_pSubChHdr->NumMixChannels) {
                    stSubMixChProc *pSMCP;
                    stMixChSharedData *pSMSD;
                    stMixChUniqueData *pSMUD;
                    int numin;

                    if (m_ObjectIndex != 0) {
                        pSMSD = m_pFirstInstance->m_MixStateParams.pSubMixChProcs[n].pMixChData_S;
                        pSMCP = m_pNFSMixMap->GetNextSubMixProc(true);
                        pSMUD = m_pNFSMixMap->GetNextSubMixUnique(true);
                    } else {
                        int MixInID;
                        int nstate;

                        pSMSD = m_pNFSMixMap->GetNextSubMixShared(true);
                        pSMCP = m_pNFSMixMap->GetNextSubMixProc(true);
                        pSMUD = m_pNFSMixMap->GetNextSubMixUnique(true);
                        MixInID = pSubMixParms->MIXCHID;
                        nstate = (MixInID & 0xFF00U) << 8;
                        MixInID &= 0x10000000U;
                        MixInID |= nstate;
                        MixInID |= 0x20000000;
                        MixInID |= n;
                        pSMSD->MIXCHINID = MixInID;
                        numin = *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(&pSubMixParms->MIXCHID) + 1);
                        pSMSD->pMapParams = pSubMixParms;
                        pSMSD->NumInputs = numin;
                    }

                    pSMCP->pMixChData_S = pSMSD;
                    n++;
                    pSMUD->Output = 0;
                    pSMUD->pInputs = nullptr;
                    pSMCP->pMixChData_U = pSMUD;
                    numin = *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(&pSubMixParms->MIXCHID) + 1);
                    m_SubMixChannelsAdded++;
                    pSubMixParms = reinterpret_cast<stSubMixChParams *>(
                        reinterpret_cast<char *>(reinterpret_cast<int *>(pSubMixParms) + numin)
                        + sizeof(stSubMixChParams));
                }
            }
        }
    }
}

void NFSMixMapState::CreateMasterMixChannels() {
    int offset;
    stMixChHdr *pMixChHdr;
    int nUnique;
    int nUniqueMastersAdded;

    offset = m_pMMStateHdr->OffsetMasterMixData;
    m_MasterChannelsAdded = 0;

    if (offset >= 0) {
        pMixChHdr = reinterpret_cast<stMixChHdr *>(reinterpret_cast<char *>(&m_pMMStateHdr->StateIndex) + offset);
        m_pMixChHdr = pMixChHdr;
        nUnique = pMixChHdr->NumUniqueSFXOBJs;
        m_pChOutArrays = m_pNFSMixMap->GetMasterChannelOutputArrayPtr(nUnique);
        nUniqueMastersAdded = 0;
        m_MasterChannelsAdded = nUniqueMastersAdded;
        if (m_pMixChHdr->NumMixChannels > 0) {
            stMasterMixChParams *pMasterParms;
            int *pOutputs;
            int nid;
            int nuniqueout;
            {
                int n;

                nid = 0;
                m_MixStateParams.pMasterMixChProcs = m_pNFSMixMap->GetNextMasterMixProc(false);
                nuniqueout = 0;
                pMasterParms = reinterpret_cast<stMasterMixChParams *>(m_pMixChHdr + 1);
                n = 0;
                while (n < m_pMixChHdr->NumMixChannels) {
                    stMasterMixChProc *pMMCP;
                    stMasterMixChSharedData *pMMSD;
                    stMasterMixChUniqueData *pMMUD;
                    bool bisattached;
                    int numin;

                    if (m_ObjectIndex != 0) {
                        pMMSD = m_pFirstInstance->m_MixStateParams.pMasterMixChProcs[n].pMixChData_S;
                        pMMCP = m_pNFSMixMap->GetNextMasterMixProc(true);
                        pMMUD = m_pNFSMixMap->GetNextMasterMixUnique(true);
                    } else {
                        int MixInID;
                        int nstate;

                        pMMSD = m_pNFSMixMap->GetNextMasterMixShared(true);
                        pMMCP = m_pNFSMixMap->GetNextMasterMixProc(true);
                        pMMUD = m_pNFSMixMap->GetNextMasterMixUnique(true);
                        pMMSD->pMapParams = pMasterParms;
                        MixInID = pMasterParms->MIXCHID;
                        pMMSD->pPRESETS = nullptr;
                        nstate = (MixInID & 0xFF00U) << 8;
                        MixInID &= 0x10000000U;
                        MixInID |= nstate;
                        MixInID |= 0x20000000;
                        MixInID |= n;
                        pMMSD->MIXCHINID = MixInID;
                        numin = *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(&pMasterParms->MIXCHID) + 1);
                        pMMSD->NumInputs = numin;
                    }

                    pMMCP->pMixChData_S = pMMSD;
                    pMMUD->Output = -10000;
                    pMMUD->p3DData = nullptr;
                    pMMUD->pInputs = nullptr;
                    pMMUD->outputID = pMMSD->pMapParams->SFXOBJID | (m_ObjectIndex << 11);

                    if (nid != pMMSD->pMapParams->SFXOBJID) {
                        pOutputs = m_pChOutArrays + (nuniqueout * 0x10);
                        nuniqueout++;
                        bisattached = m_pNFSMixMap->SETSFXID(pMMUD->outputID, pOutputs);
                    } else {
                        pOutputs = m_pChOutArrays + ((nuniqueout - 1) * 0x10);
                        bisattached = m_pNFSMixMap->SETSFXID(pMMUD->outputID, pOutputs);
                    }

                    pMMUD->pOutputs = pOutputs;
                    n++;

                    for (int out = 0; out < 0xF; out++) {
                        *pOutputs++ = 0;
                    }

                    if (bisattached) {
                        *pOutputs = 1;
                    } else {
                        *pOutputs = 0;
                    }

                    pMMCP->pMixChData_U = pMMUD;
                    nid = pMMSD->pMapParams->SFXOBJID;
                    numin = *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(&pMasterParms->MIXCHID) + 1);
                    m_MasterChannelsAdded++;
                    pMasterParms = reinterpret_cast<stMasterMixChParams *>(
                        reinterpret_cast<char *>(reinterpret_cast<int *>(pMasterParms) + numin)
                        + sizeof(stMasterMixChParams));
                }
            }
        }
    }
}

void NFSMixMapState::CreateEvtMixCtls() {
    int offset;

    offset = m_pMMStateHdr->OffsetEventCtlData;
    m_EvtMixCtlsAdded = 0;

    if (offset > -1) {
        stMixEventHdr *pHdr;

        pHdr = reinterpret_cast<stMixEventHdr *>(reinterpret_cast<char *>(&m_pMMStateHdr->StateIndex) + offset);
        m_pEvtMixCtlHdr = pHdr;
        if (pHdr->NumEvents > 0) {
            int n;
            stMixEvtParams *pEvtMixParams;

            pEvtMixParams = reinterpret_cast<stMixEvtParams *>(pHdr + 1);
            n = 0;
            m_MixStateParams.pEvtMixCtlProc = m_pNFSMixMap->GetNextEvtMixCtlProc(false);
            while (n < m_pEvtMixCtlHdr->NumEvents) {
                eDMIXENVELOPS ntype;
                stEvtMixCtlProc *pEVP;
                stEvtMixCtlSharedData *pEVS;
                stEvtMixCtlUniqueData *pEVU;
                unsigned int scaleCount;

                if (m_ObjectIndex == 0) {
                    pEVS = m_pNFSMixMap->GetNextEvtMixCtlShared(true);
                    pEVS->pMapParms = pEvtMixParams;
                } else {
                    pEVS = m_pFirstInstance->m_MixStateParams.pEvtMixCtlProc[n].pData_S;
                }

                pEVP = m_pNFSMixMap->GetNextEvtMixCtlProc(true);
                pEVU = m_pNFSMixMap->GetNextEvtMixCtlUnique(true);
                pEVP->pData_S = pEVS;
                ntype = static_cast<eDMIXENVELOPS>((static_cast<unsigned int>(pEVS->pMapParms->nEVTCTLID) >> 24) & 0xFU);

                if (ntype == DMENV_ASR) {
                    if ((pEVS->pMapParms->nParam_00 & 0xFFF) == 0) {
                        pEVS->pMapParms->nParam_00 = pEVS->pMapParms->nParam_00 | 1;
                    }
                    if ((pEVS->pMapParms->nParam_01 & 0xFFF) == 0) {
                        pEVS->pMapParms->nParam_01 = pEVS->pMapParms->nParam_01 | 1;
                    }
                    if ((pEVS->pMapParms->nParam_02 & 0xFFF) == 0) {
                        pEVS->pMapParms->nParam_02 = pEVS->pMapParms->nParam_02 | 1;
                    }
                } else if (ntype < DMENV_ATR) {
                    if (ntype == DMENV_AR) {
                        if ((pEVS->pMapParms->nParam_00 & 0xFFF) == 0) {
                            pEVS->pMapParms->nParam_00 = pEVS->pMapParms->nParam_00 | 1;
                        }
                        if ((pEVS->pMapParms->nParam_02 & 0xFFF) == 0) {
                            pEVS->pMapParms->nParam_02 = pEVS->pMapParms->nParam_02 | 1;
                        }
                    }
                } else if (ntype == DMENV_ATR) {
                    if ((pEVS->pMapParms->nParam_00 & 0xFFF) == 0) {
                        pEVS->pMapParms->nParam_00 = pEVS->pMapParms->nParam_00 | 1;
                    }
                    if ((pEVS->pMapParms->nParam_02 & 0xFFF) == 0) {
                        pEVS->pMapParms->nParam_02 = pEVS->pMapParms->nParam_02 | 1;
                    }
                }

                pEVP->pData_U = pEVU;
                n++;
                pEVU->msResetTime = 0.0f;
                pEVU->msTimeElapsed = 0.0f;
                pEVU->output = 0;
                pEVU->qoutput = 0x7FFF;
                pEVU->pTriggerPtr = reinterpret_cast<int *>(pEVS->pMapParms->nTriggerID | (m_ObjectIndex << 11));
                pEVU->ppScaleRatios = m_pNFSMixMap->AddScaleIDs(pEVS->pMapParms, m_ObjectIndex);
                m_EvtMixCtlsAdded++;
                scaleCount = (static_cast<unsigned int>(pEVS->pMapParms->nUScaleCntSwing) >> 16) & 0xFU;
                pEvtMixParams = reinterpret_cast<stMixEvtParams *>(
                    reinterpret_cast<char *>(pEvtMixParams) + sizeof(stMixEvtParams) + (scaleCount << 2));
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

void NFSMixMapState::InitializeSubChannels() {
    int n;

    n = 0;
    while (n < m_SubMixChannelsAdded) {
        int j;
        int numfixedinputs;
        int numin;
        int *pinputs;
        int *pstore;
        int *pmapinputs;
        stSubMixChProc *psbmxchproc;

        psbmxchproc = m_MixStateParams.pSubMixChProcs + n;
        numfixedinputs = 0;
        pmapinputs = &psbmxchproc->pMixChData_S->pMapParams[1].MIXCHID;
        numin = (static_cast<unsigned int>(psbmxchproc->pMixChData_S->pMapParams->MIXCHID) >> 16) & 0xFF;

        for (j = 0; j < numin; j++) {
            int chid;
            int nstate;

            chid = *pmapinputs++;
            nstate = (chid >> 16) & 0xFF;
            if (nstate == m_StateIndex) {
                numfixedinputs++;
            } else {
                numfixedinputs += m_pNFSMixMap->m_StateRefCount[nstate];
            }
        }

        pinputs = m_pNFSMixMap->GetSubChannelInputPtr(numfixedinputs);
        psbmxchproc->pMixChData_U->pInputs = pinputs;
        psbmxchproc->pMixChData_S->NumInputs = numfixedinputs;
        pstore = psbmxchproc->pMixChData_U->pInputs;
        pmapinputs = &psbmxchproc->pMixChData_S->pMapParams[1].MIXCHID;

        for (j = 0; j < numin; j++) {
            int chid;
            int newid;
            int nstate;

            chid = *pmapinputs++;
            nstate = (chid >> 16) & 0xFF;
            newid = chid & 0xFFFF07FF;
            if (nstate == m_StateIndex) {
                *pstore++ = newid | (m_ObjectIndex << 11);
            } else {
                int nd;
                int numdups;

                numdups = m_pNFSMixMap->m_StateRefCount[nstate];
                for (nd = 0; nd < numdups; nd++) {
                    *pstore++ = newid | (nd << 11);
                }
            }
        }

        n++;
    }
}

void NFSMixMapState::InitializeMasterChannels() {
    int *pPresetTable;
    int n;

    pPresetTable = reinterpret_cast<int *>(reinterpret_cast<char *>(&m_pMMStateHdr->StateIndex) + m_pMMStateHdr->OffsetPresetData);
    n = 0;
    while (n < m_MasterChannelsAdded) {
        int j;
        int num3DCtlConnections;
        int numfixedinputs;
        int nummapinputs;
        int *pinputs;
        int *pmapinputs;
        int totalPresets;
        st3DMixCtlProc **p3dstore;
        stMasterMixChProc *pmstmxchproc;

        pmstmxchproc = m_MixStateParams.pMasterMixChProcs + n;
        numfixedinputs = 0;
        num3DCtlConnections = 0;
        totalPresets = *pPresetTable;
        pmstmxchproc->pMixChData_S->pPRESETS = pPresetTable;
        nummapinputs = (static_cast<unsigned int>(pmstmxchproc->pMixChData_S->pMapParams->MIXCHID) >> 16) & 0xFF;
        pmapinputs = &pmstmxchproc->pMixChData_S->pMapParams[1].MIXCHID;

        for (j = 0; j < nummapinputs; j++) {
            unsigned int sfxid;

            sfxid = static_cast<unsigned int>(*pmapinputs++);
            if ((sfxid & 0xE0000000U) == 0x80000000U) {
                num3DCtlConnections++;
            } else {
                int nstate;

                nstate = (sfxid >> 16) & 0xFF;
                if (nstate == m_StateIndex) {
                    numfixedinputs++;
                } else {
                    numfixedinputs += m_pNFSMixMap->m_StateRefCount[nstate];
                }
            }
        }

        pinputs = m_pNFSMixMap->GetMasterChannelInputPtr(numfixedinputs + num3DCtlConnections);
        pmstmxchproc->pMixChData_U->pInputs = pinputs;
        if (num3DCtlConnections < 1) {
            pmstmxchproc->pMixChData_U->p3DData = nullptr;
        } else {
            pmstmxchproc->pMixChData_U->p3DData = reinterpret_cast<st3DMixCtlProc **>(pinputs + numfixedinputs);
        }

        pmstmxchproc->pMixChData_S->NumInputs = numfixedinputs | (num3DCtlConnections << 16);
        p3dstore = pmstmxchproc->pMixChData_U->p3DData;
        pmapinputs = &pmstmxchproc->pMixChData_S->pMapParams[1].MIXCHID;

        for (j = 0; j < num3DCtlConnections; j++) {
            *p3dstore++ = reinterpret_cast<st3DMixCtlProc *>((*pmapinputs++ & 0xFFFF07FFU) | (m_ObjectIndex << 11));
        }

        pinputs = pmstmxchproc->pMixChData_U->pInputs;
        for (j = 0; j < (nummapinputs - num3DCtlConnections); j++) {
            int chid;
            int newid;
            int nstate;

            chid = *pmapinputs++;
            nstate = (chid >> 16) & 0xFF;
            newid = chid & 0xFFFF07FF;
            if (nstate == m_StateIndex) {
                *pinputs++ = newid | (m_ObjectIndex << 11);
            } else {
                int nd;
                int numdups;

                numdups = m_pNFSMixMap->m_StateRefCount[nstate];
                for (nd = 0; nd < numdups; nd++) {
                    *pinputs++ = newid | (nd << 11);
                }
            }
        }

        pPresetTable = pPresetTable + (totalPresets & 0x1F) + 1;
        n++;
    }
}

NFSMixMapState *NFSMixMapState::GetMixMapProc(int refcnt) {
    return m_pFirstInstance + refcnt;
}

void NFSMixMapState::SetFirstStateInst(NFSMixMapState *pstate) {
    m_pFirstInstance = pstate;
}

void NFSMixMapState::AddMixState(int objnum, NFSMixMapState *pinst0) {
    m_pFirstInstance = pinst0;

    if (objnum == 0) {
        Initialize(m_pNFSMixMap, m_StateIndex, 1, 0);
    } else {
        NFSMixMapState *pstates;

        pstates = m_pNFSMixMap->GetNextMapState(true);
        pstates->SetFirstStateInst(pinst0);
        pstates->Initialize(m_pNFSMixMap, m_StateIndex, 1, objnum);
    }
}
