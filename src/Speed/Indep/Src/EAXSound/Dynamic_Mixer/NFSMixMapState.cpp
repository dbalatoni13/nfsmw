#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMapState.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixShape.hpp"
#include "Speed/Indep/Src/EAXSound/Dynamic_Mixer/NFSMixMap.hpp"

NFSMixMapState::NFSMixMapState() {}

void NFSMixMapState::Initialize(NFSMixMap *pmm, int stateindex, int numstatecopies, int objnum) {
    this->m_pNFSMixMap = pmm;
    this->m_StateIndex = stateindex;
    this->m_NumStateCopies = numstatecopies;
    this->m_ObjectIndex = objnum;

    if (objnum == 0) {
        this->m_ThisStateRefCnt = 1;
    } else {
        NFSMixMapState *pstate = this->m_pFirstInstance;
        pstate->m_ThisStateRefCnt++;

        for (int n = 0; n < objnum; n++) {
            pstate = this->m_pFirstInstance + n;
            pstate->m_ThisStateRefCnt = this->m_pFirstInstance->m_ThisStateRefCnt;
        }
    }
}

int NFSMixMapState::GetStateRefCount() {
    NFSMixMapState *pstate = m_pFirstInstance;
    return pstate->m_ThisStateRefCnt;
}

// STRIPPED
void NFSMixMapState::DestroyState() {}

NFSMixMapState::~NFSMixMapState() {}

stMixCtlProc *NFSMixMapState::GetMixCtlProc(int nMIXCTLIN_ID, int nInstance) {
    int idx = nMIXCTLIN_ID & 0xFF;
    NFSMixMapState *pstate = this->m_pFirstInstance + nInstance;

    if (pstate != nullptr && idx < pstate->GetNumMixCtlsAdded()) {
        int naddr = idx * sizeof(stMixCtlProc);
        return reinterpret_cast<stMixCtlProc *>(reinterpret_cast<char *>(pstate->m_MixStateParams.pMixCtlProcs) + naddr);
    }

    return nullptr;
}

st3DMixCtlProc *NFSMixMapState::Get3DMixCtlProc(int n3DMIXCTLCREATE_ID, int nInstance) {
    int idx = n3DMIXCTLCREATE_ID & 0xFF;
    int inst = nInstance;
    NFSMixMapState *pstate = this->m_pFirstInstance + inst;

    if (pstate != nullptr && idx < pstate->GetNum3DMixCtlsAdded()) {
        st3DMixCtlProc *paddr = pstate->m_MixStateParams.p3DMixCtlProc + idx;
        return paddr;
    }

    return nullptr;
}

stEvtMixCtlProc *NFSMixMapState::GetEvtMixCtlProc(int nEVENTID, int nInstance) {
    int idx = nEVENTID & 0xFF;
    NFSMixMapState *pstate = this->m_pFirstInstance + nInstance;

    if (pstate != nullptr && idx < pstate->GetNumEvtMixCtlsAdded()) {
        stEvtMixCtlProc *paddr = pstate->m_MixStateParams.pEvtMixCtlProc + idx;
        return paddr;
    }

    return nullptr;
}

stSubMixChProc *NFSMixMapState::GetSubMixChProc(int nMIXCHIN_ID, int nInstance) {
    int idx = nMIXCHIN_ID & 0xFF;
    NFSMixMapState *pstate = this->m_pFirstInstance + nInstance;

    if (pstate != nullptr && idx < pstate->GetNumSubMixChannelsAdded()) {
        stSubMixChProc *paddr = pstate->m_MixStateParams.pSubMixChProcs + idx;
        return paddr;
    }

    return nullptr;
}

stMasterMixChProc *NFSMixMapState::GetMasterMixChProc(int nMIXCHIN_ID, int nInstance) {
    int idx = nMIXCHIN_ID & 0xFF;
    NFSMixMapState *pstate = this->m_pFirstInstance + nInstance;

    if (pstate != nullptr && idx < pstate->GetNumMasterChannelsAdded()) {
        stMasterMixChProc *paddr = pstate->m_MixStateParams.pMasterMixChProcs + idx;
        return paddr;
    }

    return nullptr;
}

// UNSOLVED
void NFSMixMapState::CreateMixCtls() {
    int offset = this->m_pMMStateHdr->OffsetMixCtlData;
    this->m_MixCtlsAdded = 0;

    if (offset < 0) {
        return;
    }
    this->m_pMixCtlHdr = reinterpret_cast<stMixCtlHdr *>(reinterpret_cast<char *>(&this->m_pMMStateHdr->StateIndex) + offset);

    if (this->m_pMixCtlHdr->NumMixCtls > 0) {
        stMixCtlParams *pparams = reinterpret_cast<stMixCtlParams *>(this->m_pMixCtlHdr + 1);
        stMixCtlParams mixctlparm;

        this->m_MixStateParams.pMixCtlProcs = this->m_pNFSMixMap->GetProcessMixCtlPtr(false);

        for (int n = 0; n < this->m_pMixCtlHdr->NumMixCtls; n++) {
            mixctlparm.nINPUTID = pparams->nINPUTID;
            mixctlparm.nUScaleCntSwing = pparams->nUScaleCntSwing;
            mixctlparm.nINPUTID &= 0xFFFF07FFU;
            mixctlparm.nINPUTID |= this->m_ObjectIndex << 11;

            stCurveDataProc *pcurvedata = this->m_pNFSMixMap->GetCurveDataPtr(&mixctlparm);
            int *pscaleaddr = this->m_pNFSMixMap->AddScaleIDs(pparams, this->m_ObjectIndex);
            stMixCtlProc *pmcp = this->m_pNFSMixMap->GetProcessMixCtlPtr(true);

            this->m_pNFSMixMap->AssignMixCtlDataPtrs(pmcp, pparams, this->m_ObjectIndex, n);

            int nstateid = pparams->nINPUTID & 0x0F000000;
            int ncurveid = pparams->nINPUTID & 0x00FF0000;
            int maptype;
            int nOBJID;
            int nummapscales = (mixctlparm.nUScaleCntSwing >> 16) & 0x1F;
            int nnext = sizeof(stMixCtlParams);

            {
                int nshift = nummapscales * sizeof(int);
                nnext += nshift;
            }

            nOBJID = (pparams->nINPUTID >> 16) & 0xE000;
            maptype = this->m_pNFSMixMap->GetMapType();

            pmcp->psdata->MIXCTLOBJID = (nstateid | ncurveid) | nOBJID | n | (maptype << 8);
            pmcp->psdata->pstMixCtlParms = pparams;
            pmcp->psdata->nOffset = 0;
            pmcp->psdata->nRatio = 0;

            {
                int ntmp;

                if ((pparams->nUScaleCntSwing & 0x8000) == 0) {
                    pmcp->psdata->nOffset = pparams->nUScaleCntSwing & 0x7FFF;
                    ntmp = -pmcp->psdata->nOffset;
                } else {
                    ntmp = pparams->nUScaleCntSwing | 0xFFFF0000;
                }

                pmcp->psdata->nRatio = 0x7FFF - NFSMixShape::GetQ15FromHundredthsdB(ntmp);
            }

            pmcp->pudata->CmpdBOut = 0;
            pmcp->pudata->pstCurveData = pcurvedata;
            pmcp->pudata->ppScaleRatios = reinterpret_cast<int **>(pscaleaddr);

            this->m_MixCtlsAdded++;

            // TODO fake match?
            pparams =
                reinterpret_cast<stMixCtlParams *>(reinterpret_cast<char *>(&pparams[1].nINPUTID) + ((mixctlparm.nUScaleCntSwing >> 14) & 0x7CU));
        }
    }
}

// UNSOLVED
void NFSMixMapState::CreateSubMixChannels() {
    int offset;

    offset = this->m_pMMStateHdr->OffsetSubMixData;
    this->m_SubMixChannelsAdded = 0;

    if (offset < 0) {
        return;
    }
    this->m_pSubChHdr = reinterpret_cast<stMixChHdr *>(reinterpret_cast<char *>(&this->m_pMMStateHdr->StateIndex) + offset);

    if (this->m_pSubChHdr->NumMixChannels > 0) {
        stSubMixChParams *pSubMixParms;

        this->m_MixStateParams.pSubMixChProcs = this->m_pNFSMixMap->GetNextSubMixProc(false);
        pSubMixParms = reinterpret_cast<stSubMixChParams *>(&this->m_pSubChHdr[1]);

        for (int n = 0; n < this->m_pSubChHdr->NumMixChannels;) {
            stSubMixChProc *pSMCP;
            stMixChSharedData *pSMSD;
            stMixChUniqueData *pSMUD;
            int numin;

            if (this->m_ObjectIndex != 0) {
                pSMSD = (this->m_pFirstInstance->m_MixStateParams.pSubMixChProcs + n)->pMixChData_S;
                pSMCP = this->m_pNFSMixMap->GetNextSubMixProc(true);
                pSMUD = this->m_pNFSMixMap->GetNextSubMixUnique(true);
            } else {
                pSMSD = this->m_pNFSMixMap->GetNextSubMixShared(true);
                pSMCP = this->m_pNFSMixMap->GetNextSubMixProc(true);
                pSMUD = this->m_pNFSMixMap->GetNextSubMixUnique(true);

                int MixInID = pSubMixParms->MIXCHID;
                int nstate = (MixInID & 0xFF00) << 8;
                MixInID &= 0x10000000;
                MixInID |= nstate;
                MixInID |= 0x20000000;
                MixInID |= n;

                pSMSD->MIXCHINID = MixInID;
                numin = (pSubMixParms->MIXCHID & 0x00FF0000) >> 16;
                pSMSD->pMapParams = pSubMixParms;
                pSMSD->NumInputs = numin;
            }

            pSMCP->pMixChData_S = pSMSD;
            n++;
            pSMUD->Output = 0;
            pSMUD->pInputs = nullptr;
            pSMCP->pMixChData_U = pSMUD;

            numin = (pSubMixParms->MIXCHID & 0x00FF0000) >> 16;

            this->m_SubMixChannelsAdded++;
            pSubMixParms = reinterpret_cast<stSubMixChParams *>(reinterpret_cast<char *>(reinterpret_cast<int *>(pSubMixParms) + numin) +
                                                                sizeof(stSubMixChParams));
        }
    }
}

void NFSMixMapState::CreateMasterMixChannels() {
    int offset = this->m_pMMStateHdr->OffsetMasterMixData;

    this->m_MasterChannelsAdded = 0;

    if (offset < 0) {
        return;
    }
    stMixChHdr *pMixChHdr = reinterpret_cast<stMixChHdr *>(reinterpret_cast<char *>(&this->m_pMMStateHdr->StateIndex) + offset);
    this->m_pMixChHdr = pMixChHdr;

    int nUnique = pMixChHdr->NumUniqueSFXOBJs;
    this->m_pChOutArrays = this->m_pNFSMixMap->GetMasterChannelOutputArrayPtr(nUnique);

    int nUniqueMastersAdded = 0;
    this->m_MasterChannelsAdded = nUniqueMastersAdded;

    if (this->m_pMixChHdr->NumMixChannels > 0) {
        this->m_MixStateParams.pMasterMixChProcs = this->m_pNFSMixMap->GetNextMasterMixProc(false);

        stMasterMixChParams *pMasterParms = reinterpret_cast<stMasterMixChParams *>(this->m_pMixChHdr + 1);
        int *pOutputs;
        int nid = 0;
        int nuniqueout = 0;

        for (int n = 0; n < this->m_pMixChHdr->NumMixChannels; n++) {
            stMasterMixChProc *pMMCP;
            stMasterMixChSharedData *pMMSD;
            stMasterMixChUniqueData *pMMUD;

            if (this->m_ObjectIndex != 0) {
                // TODO 64 bit
                // TODO fake match
                pMMSD = reinterpret_cast<stMasterMixChProc *>(
                            reinterpret_cast<unsigned int>(this->m_pFirstInstance->m_MixStateParams.pMasterMixChProcs) + (n << 3))
                            ->pMixChData_S;

                pMMCP = this->m_pNFSMixMap->GetNextMasterMixProc(true);
                pMMUD = this->m_pNFSMixMap->GetNextMasterMixUnique(true);
            } else {
                pMMSD = this->m_pNFSMixMap->GetNextMasterMixShared(true);
                pMMCP = this->m_pNFSMixMap->GetNextMasterMixProc(true);
                pMMUD = this->m_pNFSMixMap->GetNextMasterMixUnique(true);

                pMMSD->pMapParams = pMasterParms;

                int MixInID = pMasterParms->MIXCHID;
                pMMSD->pPRESETS = nullptr;

                int nstate = (MixInID & 0xFF00U) << 8;
                MixInID &= 0x10000000U;
                MixInID |= nstate;
                MixInID |= 0x20000000;
                MixInID |= n;

                pMMSD->MIXCHINID = MixInID;
                pMMSD->NumInputs = (pMasterParms->MIXCHID & 0x00FF0000) >> 16;
            }

            pMMCP->pMixChData_S = pMMSD;
            pMMUD->Output = -10000;
            pMMUD->p3DData = nullptr;
            pMMUD->pInputs = nullptr;
            pMMUD->outputID = pMMSD->pMapParams->SFXOBJID | (this->m_ObjectIndex << 11);

            bool bisattached;
            if (nid != pMMSD->pMapParams->SFXOBJID) {
                pOutputs = this->m_pChOutArrays + (nuniqueout * 0x10);
                nuniqueout++;
                bisattached = this->m_pNFSMixMap->SETSFXID(pMMUD->outputID, pOutputs);
            } else {
                pOutputs = this->m_pChOutArrays + ((nuniqueout - 1) * 0x10);
                bisattached = this->m_pNFSMixMap->SETSFXID(pMMUD->outputID, pOutputs);
            }

            pMMUD->pOutputs = pOutputs;

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

            int numin = (pMasterParms->MIXCHID & 0x00FF0000) >> 16;

            this->m_MasterChannelsAdded++;

            pMasterParms = reinterpret_cast<stMasterMixChParams *>(reinterpret_cast<char *>(reinterpret_cast<int *>(pMasterParms) + numin) +
                                                                   sizeof(stMasterMixChParams));
        }
    }
}

void NFSMixMapState::CreateEvtMixCtls() {
    int offset = this->m_pMMStateHdr->OffsetEventCtlData;
    this->m_EvtMixCtlsAdded = 0;

    if (offset < 0) {
        return;
    }
    this->m_pEvtMixCtlHdr = reinterpret_cast<stMixEventHdr *>(reinterpret_cast<char *>(&this->m_pMMStateHdr->StateIndex) + offset);

    if (this->m_pEvtMixCtlHdr->NumEvents > 0) {
        stMixEvtParams *pEvtMixParams = reinterpret_cast<stMixEvtParams *>(this->m_pEvtMixCtlHdr + 1);

        this->m_MixStateParams.pEvtMixCtlProc = this->m_pNFSMixMap->GetNextEvtMixCtlProc(false);

        for (int n = 0; n < this->m_pEvtMixCtlHdr->NumEvents; n++) {
            stEvtMixCtlProc *pEVP;
            stEvtMixCtlSharedData *pEVS;
            stEvtMixCtlUniqueData *pEVU;
            eDMIXENVELOPS ntype;
            int nID;
            int numscale;

            if (this->m_ObjectIndex != 0) {
                pEVS = this->m_pFirstInstance->m_MixStateParams.pEvtMixCtlProc[n].pData_S;
            } else {
                pEVS = this->m_pNFSMixMap->GetNextEvtMixCtlShared(true);
                pEVS->pMapParms = pEvtMixParams;
            }

            pEVP = this->m_pNFSMixMap->GetNextEvtMixCtlProc(true);
            pEVU = this->m_pNFSMixMap->GetNextEvtMixCtlUnique(true);

            pEVP->pData_S = pEVS;

            nID = (pEVS->pMapParms->nEVTCTLID >> 24) & 0xF;
            ntype = static_cast<eDMIXENVELOPS>(nID);

            switch (ntype) {
                case DMENV_AR:
                case DMENV_ATR:
                    if ((pEVP->pData_S->pMapParms->nParam_00 & 0xFFF) == 0) {
                        pEVP->pData_S->pMapParms->nParam_00 |= 1;
                    }

                    if ((pEVP->pData_S->pMapParms->nParam_02 & 0xFFF) == 0) {
                        pEVP->pData_S->pMapParms->nParam_02 |= 1;
                    }
                    break;

                case DMENV_ASR:
                    if ((pEVS->pMapParms->nParam_00 & 0xFFF) == 0) {
                        pEVS->pMapParms->nParam_00 |= 1;
                    }

                    if ((pEVP->pData_S->pMapParms->nParam_01 & 0xFFF) == 0) {
                        pEVP->pData_S->pMapParms->nParam_01 |= 1;
                    }

                    if ((pEVP->pData_S->pMapParms->nParam_02 & 0xFFF) == 0) {
                        pEVP->pData_S->pMapParms->nParam_02 |= 1;
                    }
                    break;
                case DMENV_LFO:
                    break;
            }

            pEVP->pData_U = pEVU;
            n++;

            pEVU->msResetTime = 0.0f;
            pEVU->msTimeElapsed = 0.0f;
            pEVU->output = 0;
            pEVU->qoutput = 0x7FFF;

            pEVU->pTriggerPtr = reinterpret_cast<int *>(pEVS->pMapParms->nTriggerID | (this->m_ObjectIndex << 11));

            pEVU->ppScaleRatios = reinterpret_cast<int **>(this->m_pNFSMixMap->AddScaleIDs(pEVS->pMapParms, this->m_ObjectIndex));

            this->m_EvtMixCtlsAdded++;

            numscale = (pEVS->pMapParms->nUScaleCntSwing >> 16) & 0xF;

            pEvtMixParams = reinterpret_cast<stMixEvtParams *>(reinterpret_cast<char *>(pEvtMixParams) + sizeof(stMixEvtParams) + (numscale * 4));
        }
    }
}

void NFSMixMapState::Create3DMixCtls() {
    this->m_3DMixCtlsAdded = 0;
    int offset = this->m_pMMStateHdr->Offset3DMixCtlData;

    if (offset < 0) {
        return;
    }

    this->m_p3DMixCtlHdr = reinterpret_cast<st3DMixCtlHdr *>(reinterpret_cast<char *>(&this->m_pMMStateHdr->StateIndex) + offset);

    if (this->m_p3DMixCtlHdr->Num3DMixCtls > 0) {
        st3DMixCtlParams *p3DMixCtlParams = reinterpret_cast<st3DMixCtlParams *>(&this->m_p3DMixCtlHdr[1]);

        this->m_MixStateParams.p3DMixCtlProc = this->m_pNFSMixMap->GetNext3DMixCtlProc(false);

        for (int n = 0; n < this->m_p3DMixCtlHdr->Num3DMixCtls; n++) {
            st3DMixCtlProc *p3DCP = nullptr;
            st3DMixCtlSharedData *p3DSD;

            if (this->m_ObjectIndex != 0) {
                // TODO fake match
                p3DSD = *reinterpret_cast<st3DMixCtlSharedData **>(reinterpret_cast<char *>(this->m_pFirstInstance->m_MixStateParams.p3DMixCtlProc) +
                                                                   (n << 3));
            } else {
                p3DSD = this->m_pNFSMixMap->GetNext3DMixCtlShared(true);
                p3DSD->pMapParams = p3DMixCtlParams;
                p3DSD->msSinceCamTrans = 0;
                p3DSD->CurCamState = 0;
                p3DSD->PrevCamState = 0;
            }

            p3DCP = this->m_pNFSMixMap->GetNext3DMixCtlProc(true);
            st3DMixCtlUniqueData *p3DUD = this->m_pNFSMixMap->GetNext3DMixCtlUnique(true);

            p3DCP->p3DMixCtlData_S = p3DSD;
            p3DSD->pCurStateParams = &p3DMixCtlParams->StateParams;
            p3DCP->p3DMixCtlData_U = p3DUD;

            p3DCP->p3DMixCtlData_U->azimuth = 0;
            p3DCP->p3DMixCtlData_U->dBRolloff = 0;
            p3DCP->p3DMixCtlData_U->q15Rolloff = 0x7FFF;
            p3DCP->p3DMixCtlData_U->DopplerCents = 0;
            p3DCP->p3DMixCtlData_U->fPrevDeltaDist = 0.0f;
            p3DCP->p3DMixCtlData_U->fPrevDist = 0.0f;

            int nID = p3DCP->p3DMixCtlData_S->pMapParams->nINPUTID & 0xFFFF07FF;
            p3DCP->p3DMixCtlData_U->nINPUTID = (this->m_ObjectIndex << 11) | nID;

            p3DCP->p3DMixCtlData_U->pInputs =
                reinterpret_cast<int *>(((p3DCP->p3DMixCtlData_S->pMapParams->nINPUTID | (this->m_ObjectIndex << 11)) & 0x1FFFFFFF) | 0x60000000);

            this->m_3DMixCtlsAdded++;

            int numstates = (p3DMixCtlParams->nINPUTID >> 24) & 0xF;

            p3DMixCtlParams =
                reinterpret_cast<st3DMixCtlParams *>(reinterpret_cast<char *>(p3DMixCtlParams) + 4 + numstates * sizeof(st3DStateParams));
        }
    }
}

void NFSMixMapState::InitializeSubChannels() {
    for (int n = 0; n < this->m_SubMixChannelsAdded; n++) {
        stSubMixChProc *psbmxchproc = this->m_MixStateParams.pSubMixChProcs + n;
        int numfixedinputs = 0;
        int *pinputs = &psbmxchproc->pMixChData_S->pMapParams[1].MIXCHID;
        int numinputs = (static_cast<unsigned int>(psbmxchproc->pMixChData_S->pMapParams->MIXCHID) >> 16) & 0xFF;
        int j = numfixedinputs;

        for (; j < numinputs; j++) {
            int chid = *pinputs++;
            int nstate = (chid >> 16) & 0xFF;

            if (nstate == this->m_StateIndex) {
                numfixedinputs++;
            } else {
                int numdups = this->m_pNFSMixMap->m_StateRefCount[nstate];
                numfixedinputs += numdups;
            }
        }

        pinputs = this->m_pNFSMixMap->GetSubChannelInputPtr(numfixedinputs);
        psbmxchproc->pMixChData_U->pInputs = pinputs;
        psbmxchproc->pMixChData_S->NumInputs = numfixedinputs;

        int *pstore = psbmxchproc->pMixChData_U->pInputs;
        pinputs = &psbmxchproc->pMixChData_S->pMapParams[1].MIXCHID;
        j = 0;

        for (; j < numfixedinputs; j++) {
            int chid = *pinputs++;
            int ncnt = (chid >> 16) & 0xFF;
            int newid = chid & 0xFFFF07FF;

            if (ncnt == this->m_StateIndex) {
                *pstore++ = newid | (this->m_ObjectIndex << 11);
            } else {
                int nstate = ncnt;
                int usethisid = chid & 0xFFFF07FF;
                j--;

                int numdups = this->m_pNFSMixMap->m_StateRefCount[nstate];

                for (int nd = 0; nd < numdups; nd++) {
                    *pstore++ = usethisid | (nd << 11);
                    j++;
                }
            }
        }
    }
}

int nDUMMY_3DOUT = 0xFFFFD8F0; // size: 0x4, address: 0xFFFFFFFF, Decl: 761

void NFSMixMapState::InitializeMasterChannels() {
    int *pPresetTable = reinterpret_cast<int *>(reinterpret_cast<char *>(&this->m_pMMStateHdr->StateIndex) + this->m_pMMStateHdr->OffsetPresetData);

    for (int n = 0; n < this->m_MasterChannelsAdded;) {
        int numfixedinputs = 0;
        int num3DCtlConnections = 0;
        stMasterMixChProc *pmstmxchproc = this->m_MixStateParams.pMasterMixChProcs + n;
        int nchanneltype = n + 1;
        int *pinputs = &pmstmxchproc->pMixChData_S->pMapParams[1].MIXCHID;
        int nummapinputs = (static_cast<unsigned int>(pmstmxchproc->pMixChData_S->pMapParams->MIXCHID) >> 16) & 0xFF;
        int sfxid;
        int index = numfixedinputs;

        pmstmxchproc->pMixChData_S->pPRESETS = pPresetTable;

        int presettableID = *pPresetTable;
        int nPresetType;
        int nTotalPresets = presettableID & 0x1F;
        int nUnused;

        for (; index < nummapinputs; index++) {
            int chid = *pinputs++;

            if ((chid & 0xE0000000U) == 0x80000000U) {
                num3DCtlConnections++;
            } else {
                int nstate = (chid >> 16) & 0xFF;

                if (nstate == this->m_StateIndex) {
                    numfixedinputs++;
                } else {
                    int numdups = this->m_pNFSMixMap->m_StateRefCount[nstate];
                    numfixedinputs += numdups;
                }
            }
        }

        pinputs = this->m_pNFSMixMap->GetMasterChannelInputPtr(numfixedinputs + num3DCtlConnections);
        pmstmxchproc->pMixChData_U->pInputs = pinputs;

        if (num3DCtlConnections > 0) {
            pmstmxchproc->pMixChData_U->p3DData = reinterpret_cast<st3DMixCtlProc **>(pmstmxchproc->pMixChData_U->pInputs + numfixedinputs);
        } else {
            pmstmxchproc->pMixChData_U->p3DData = nullptr;
        }

        pmstmxchproc->pMixChData_S->NumInputs = numfixedinputs | (num3DCtlConnections << 16);

        {
            int *pin = reinterpret_cast<int *>(pmstmxchproc->pMixChData_U->p3DData);

            pinputs = &pmstmxchproc->pMixChData_S->pMapParams[1].MIXCHID;

            for (int n3d = 0; n3d < num3DCtlConnections; n3d++) {
                int n3DCTLID = *pinputs++;
                int lookupid = n3DCTLID & 0xFFFF07FF;
                int nidx = this->m_ObjectIndex << 11;

                *pin++ = lookupid | nidx;
            }
        }

        int nChannelInstance;
        int j;
        int nchid;
        int *pstore = pmstmxchproc->pMixChData_U->pInputs;

        for (j = 0; j < numfixedinputs; j++) {
            int chid = *pinputs++;
            int ncnt = (chid >> 16) & 0xFF;
            int nid = chid & 0xFFFF07FF;

            if (ncnt == this->m_StateIndex) {
                *pstore++ = nid | (this->m_ObjectIndex << 11);
            } else {
                int nstate = ncnt;
                int numdups = this->m_pNFSMixMap->m_StateRefCount[nstate];
                int usethisid = nid;

                j--;

                for (int nd = 0; nd < numdups; nd++) {
                    j++;
                    *pstore++ = usethisid | (nd << 11);
                }
            }
        }

        pPresetTable += nTotalPresets;
        pPresetTable++;
        n = nchanneltype;
    }
}

// STRIPPED
NFSMixMap *NFSMixMapState::GetConnectMap(int nID) {}

// STRIPPED
void NFSMixMapState::UpdateState(float t) {}

// STRIPPED
void NFSMixMapState::ProcessMixMap(float t) {}

// STRIPPED
NFSMixMapState *NFSMixMapState::GetMixMapProcAddress() {}

NFSMixMapState *NFSMixMapState::GetMixMapProc(int refcnt) {
    NFSMixMapState *pstate = &this->m_pFirstInstance[refcnt];
    return pstate;
}

void NFSMixMapState::SetFirstStateInst(NFSMixMapState *pstate) {
    this->m_pFirstInstance = pstate;
}

void NFSMixMapState::AddMixState(int objnum, NFSMixMapState *pinst0) {
    this->m_pFirstInstance = pinst0;

    if (objnum == 0) {
        this->Initialize(this->m_pNFSMixMap, this->m_StateIndex, 1, 0);
    } else {
        NFSMixMapState *pstates = this->m_pNFSMixMap->GetNextMapState(true);
        pstates->SetFirstStateInst(pinst0);
        pstates->Initialize(this->m_pNFSMixMap, this->m_StateIndex, 1, objnum);
    }
}

// STRIPPED
void NFSMixMapState::RemoveState() {}
