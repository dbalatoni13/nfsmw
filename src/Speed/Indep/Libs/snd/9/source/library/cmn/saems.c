#include "csis/csis.h"
#include "snd/sndo.h"
#include "sndcmn.h"
#include "Speed/Indep/Libs/snd/9/extern/aemsdef.h"
#include "saemsi.h"

void SNDAEMSI_bankpitchmult(int handle, int value);
void SNDAEMSI_banktimemult(int handle, int value);
void SNDAEMSI_bankvol(int handle, int value);
void SNDAEMSI_bankazimuth(int handle, int value);
void SNDAEMSI_bankinputstub(int handle, int value);
void SNDAEMSI_bankfxwet0(int handle, int value);
void SNDAEMSI_banklowpass(int handle, int value);
void SNDAEMSI_bankhighpass(int handle, int value);
void SNDAEMSI_bankdrylevel(int handle, int value);

void SNDAEMSI_optpitchmult(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_opttimemult(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_optvol(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_optazimuth(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_optstub(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_optfxwet0(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_optlowpass(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_opthighpass(SNDPLAYOPTS *psndplayopts, int value);
void SNDAEMSI_optdrylevel(SNDPLAYOPTS *psndplayopts, int value);

AemsDef::PLAYERHANDLE SNDAEMSI_playerplaybank(AemsDef::PLAYERSTATE *pplayerstate, AemsDef::SAMPLEENTRY *psampleentry);

void SNDAEMSI_playerstopbank(AemsDef::PLAYERSTATE *pplayerstate);

void SNDAEMSI_playerpausebank(AemsDef::PLAYERHANDLE pplayerstate);

void SNDAEMSI_playerunpausebank(AemsDef::PLAYERSTATE *pplayerstate);

int SNDAEMSI_playerupdatebank(AemsDef::PLAYERSTATE *pplayerstate);

AemsDef::PLAYERINTERFACEFN * playerinterfacefn[12] = {
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankpitchmult),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_banktimemult),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankvol),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankazimuth),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankinputstub),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankfxwet0),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_banklowpass),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankhighpass),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankdrylevel),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankinputstub),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankinputstub),
    reinterpret_cast<AemsDef::PLAYERINTERFACEFN *>(SNDAEMSI_bankinputstub),
};
AemsDef::PLAYERSETOPTSFN *SNDAEMSoptfn[12] = {
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optpitchmult),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_opttimemult),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optvol),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optazimuth),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optstub),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optfxwet0),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optlowpass),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_opthighpass),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optdrylevel),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optstub),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optstub),
    reinterpret_cast<AemsDef::PLAYERSETOPTSFN *>(SNDAEMSI_optstub),
};

AemsDef::PLAYERPLAYFN *SNDAEMSplayerplayfn[4] = {
    reinterpret_cast<AemsDef::PLAYERPLAYFN *>(SNDAEMSI_playerplaybank),
};

AemsDef::PLAYERSTOPFN *SNDAEMSplayerstopfn[4] = {
    reinterpret_cast<AemsDef::PLAYERSTOPFN *>(SNDAEMSI_playerstopbank),
};

AemsDef::PLAYERPAUSEFN *SNDAEMSplayerpausefn[4] = {
    reinterpret_cast<AemsDef::PLAYERPAUSEFN *>(SNDAEMSI_playerpausebank),
};

AemsDef::PLAYERUNPAUSEFN *SNDAEMSplayerunpausefn[4] = {
    reinterpret_cast<AemsDef::PLAYERUNPAUSEFN *>(SNDAEMSI_playerunpausebank),
};

AemsDef::PLAYERUPDATEFN *SNDAEMSplayerupdatefn[4] = {
    reinterpret_cast<AemsDef::PLAYERUPDATEFN *>(SNDAEMSI_playerupdatebank),
};

// Decl: 465
int SNDAEMSI_UpdateClassDestructor(AemsDef::ClassDestructorState *pClassDestructorState) {
    int ret = pClassDestructorState->settings.triggered;
    pClassDestructorState->settings.triggered = 0;
    return ret;
}

// 479
int SNDAEMSI_UpdateClassData(AemsDef::ClassDataState *pClassDataState) {
    return pClassDataState->outputs.value[0];
}

// 486
int SNDAEMSI_UpdateGlobalVariable(AemsDef::GlobalVariableState *pGlobalVariableState) {
    return pGlobalVariableState->settings.globalValue;
}

// 492
int SNDAEMSI_updatecreate(AemsDef::CREATESTATE *pcreatestate) {
    int ret = pcreatestate->settings.triggered;
    pcreatestate->settings.triggered = 0;
    return ret;
}

// 505
int SNDAEMSI_updatedestroy(AemsDef::DESTROYSTATE *pds) {
    int i;

    if (pds->triggered == 0) return 0;

    pds->settings.pModule->moduleInstance.Remove(&pds->settings.pModuleInstance->ln);
    sndaems.timerclient.Remove(&pds->settings.pModuleInstance->timerclient.ln);

    AemsDef::ClassDestructorState *pClassDestructorState =
        reinterpret_cast<AemsDef::ClassDestructorState *>(&pds->settings.pModuleInstance[1]);
    if (pds->settings.pModule->classDestructorPresent != 0) {
        pds->settings.pClass->UnsubscribeDestructorFast(&pClassDestructorState->settings.client);
        pClassDestructorState++;
    }

    AemsDef::GlobalVariableState *pGlobalVariableState =
        reinterpret_cast<AemsDef::GlobalVariableState *>(pClassDestructorState);
    for (i = 0; i < pds->settings.pModule->numGlobals; i++, pGlobalVariableState++) {
        Csis::GlobalVariable::UnsubscribeFast(&pGlobalVariableState->settings.globalVariableHandle, &pGlobalVariableState->settings.client);
    }

    AemsDef::ClassDataState *pClassDataState =
        reinterpret_cast<AemsDef::ClassDataState *>(pGlobalVariableState);
    if (pds->settings.pModule->classDataPresent != 0) {
        pds->settings.pClass->UnsubscribeMemberDataFast(&pClassDataState->settings.client);
        // TODO is this real
        pClassDataState = reinterpret_cast<AemsDef::ClassDataState *>(
            reinterpret_cast<int *>(pClassDataState) + (pClassDataState->settings.numOutputs + 5)
        );
    }

    AemsDef::FunctionState *pFunctionState =
        reinterpret_cast<AemsDef::FunctionState *>(pClassDataState);
    for (i = 0; i < pds->settings.pModule->numFunctions; i++) {
        Csis::Function::UnsubscribeFast(&pFunctionState->settings.functionHandle, &pFunctionState->settings.client);
        // TODO is this real
        pFunctionState = reinterpret_cast<AemsDef::FunctionState *>(
            reinterpret_cast<int *>(pFunctionState) + (pFunctionState->settings.numParameters + 7)
        );
    }

    AemsDef::PLAYERSTATE *pplayerstate;
    int *pOffset = reinterpret_cast<int *>(&pds->settings.pModule[1]);
    for (i = 0; i < pds->settings.pModule->numPlayers; i++) {
        pplayerstate = reinterpret_cast<AemsDef::PLAYERSTATE *>(
            reinterpret_cast<char *>(pds->settings.pModuleInstance) + *pOffset++
        );
        if (pplayerstate->settings.sampletype >= 0) {
            SNDAEMSplayerstopfn[pplayerstate->settings.sampletype](pplayerstate);
        }
    }

    for (i = 0; i < pds->settings.pModule->numClassControllers; i++) {
        AemsDef::ControlClassState *pControlClassState = reinterpret_cast<AemsDef::ControlClassState *>(
            reinterpret_cast<char *>(pds->settings.pModuleInstance) + *pOffset++
        );

        if (pControlClassState->settings.pClass != NULL) {
            pControlClassState->settings.pClass->Release();
        }
    }

    pds->settings.pModule->curinstances -= 1;
    SNDMEMI_free(pds->settings.pModuleInstance);

    return 0;
}

// 642
int UpdateCallFunction(AemsDef::CallFunctionState *pCallFunctionState) {
    AemsDef::CallFunctionInputs *pInputs;
    if (pCallFunctionState->settings.clampParameters != 0) {
        AemsDef::ParameterClampDesc *pParameterClampDesc =
            reinterpret_cast<AemsDef::ParameterClampDesc *>(&pCallFunctionState[1]);
        pInputs = reinterpret_cast<AemsDef::CallFunctionInputs *>(&pParameterClampDesc[pCallFunctionState->settings.numParameters]);
        int i;
        for (i = 0; i < pCallFunctionState->settings.numParameters; i++) {
            pInputs->parameter[i] = SNDI_clipint32(pInputs->parameter[i], pParameterClampDesc[i].minVal, pParameterClampDesc[i].maxVal);
        }
    } else {
        pInputs = reinterpret_cast<AemsDef::CallFunctionInputs *>(&pCallFunctionState[1]);
    }
    if (pInputs->triggered != 0) {
        Csis::Function::CallFast(&pCallFunctionState->settings.functionHandle, pInputs->parameter);
    }

    return 0;
}

// 688
int UpdateControlClass(AemsDef::ControlClassState *pControlClassState) {
    AemsDef::ControlClassInputs *pInputs;
    AemsDef::ParameterClampDesc *pParameterClampDesc = NULL;

    if (pControlClassState->settings.clampParameters != 0) {
        pParameterClampDesc = reinterpret_cast<AemsDef::ParameterClampDesc *>(&pControlClassState[1]);
        pInputs = reinterpret_cast<AemsDef::ControlClassInputs *>(&pParameterClampDesc[pControlClassState->settings.numParameters]);
    } else {
        pInputs = reinterpret_cast<AemsDef::ControlClassInputs *>(&pControlClassState[1]);
    }

    if (pInputs->destructorTriggered != 0) {
        if (pControlClassState->settings.pClass != NULL) {
            pControlClassState->settings.pClass->Release();
            pControlClassState->settings.pClass = NULL;
        } else {
            return 0;
        }
    } else if (pInputs->constructorTriggered != 0) {
        if (pControlClassState->settings.pClass == NULL) {
            if (pControlClassState->settings.clampParameters != 0) {
                for (int i = 0; i < pControlClassState->settings.numParameters; i++) {
                    pInputs->parameter[i] = SNDI_clipint32(
                        pInputs->parameter[i],
                        pParameterClampDesc[i].minVal,
                        pParameterClampDesc[i].maxVal
                    );
                }
            }

            if (pControlClassState->settings.pClass->CreateInstanceFast(
                &pControlClassState->settings.classHandle,
                pInputs->parameter,
                &pControlClassState->settings.pClass
            ) < 0) {
                pControlClassState->settings.pClass = NULL;
            }
        }
    } else if (pControlClassState->settings.pClass != NULL) {
        if (pControlClassState->settings.clampParameters != 0) {
            for (int i = 0; i < pControlClassState->settings.numParameters; i++) {
                pInputs->parameter[i] = SNDI_clipint32(
                    pInputs->parameter[i],
                    pParameterClampDesc[i].minVal,
                    pParameterClampDesc[i].maxVal
                );
            }
        }
        pControlClassState->settings.pClass->SetMemberDataFast(pInputs->parameter);
    } else {
        return 0;
    }

    if (pControlClassState->settings.pClass != NULL) {
        int refCount;
        pControlClassState->settings.pClass->GetRefCount(&refCount);
        return refCount;
    }

    return 0;
}

// 794
int UpdateSetGlobalVariable(AemsDef::SetGlobalVariableState *pSetGlobalVariableState) {
    int value;

    if (pSetGlobalVariableState->settings.prevValue != pSetGlobalVariableState->inputs.value) {
        pSetGlobalVariableState->settings.prevValue = pSetGlobalVariableState->inputs.value;

        value = SNDI_clipint32(
            pSetGlobalVariableState->settings.prevValue,
            pSetGlobalVariableState->settings.minValue,
            pSetGlobalVariableState->settings.maxValue
        );
        Csis::GlobalVariable::SetFast(&pSetGlobalVariableState->settings.globalVariableHandle, &value);
    }

    return 0;
}

// 822
int SNDAEMSI_updatecounter(AemsDef::COUNTERSTATE *pcounterstate) {
    if (pcounterstate->override >= pcounterstate->settings.minvalue && pcounterstate->override <= pcounterstate->settings.maxvalue) {
        return pcounterstate->override;
    }
    int retval;

    if (pcounterstate->triggered > 0) {
        pcounterstate->settings.value += pcounterstate->settings.direction;
        if (pcounterstate->settings.value > pcounterstate->settings.maxvalue) {
            pcounterstate->settings.value = pcounterstate->settings.minvalue;
        } else if (pcounterstate->settings.value < pcounterstate->settings.minvalue) {
            pcounterstate->settings.value = pcounterstate->settings.maxvalue;
        }
        return pcounterstate->settings.value;
    }
    return pcounterstate->settings.value;
}

// 856
int SNDAEMSI_updaterandom(AemsDef::RANDOMSTATE *prandomstate) {
    if (prandomstate->triggered == 0) {
        return prandomstate->settings.outputcurrent;
    }

    prandomstate->settings.outputcurrent = (iSNDrandom() % prandomstate->settings.range) + prandomstate->settings.minvalue;
    return prandomstate->settings.outputcurrent;
}

// 874
int SNDAEMSI_updaterandomshuffle(AemsDef::RANDOMSHUFFLESTATE *prandomshufflestate) {
    AemsDef::RANDOMSHUFFLEINPUTS *prandomshuffleinputs =
        reinterpret_cast<AemsDef::RANDOMSHUFFLEINPUTS *>(
            &reinterpret_cast<char *>(prandomshufflestate)[prandomshufflestate->settings.inputoffset]
    );
    if (prandomshuffleinputs->triggered == 0) {
        return prandomshufflestate->settings.outputcurrent;
    }

    unsigned int swapindex = (
        iSNDrandom() % (
            prandomshufflestate->settings.range - prandomshufflestate->settings.numbersetindex - (char)prandomshufflestate->settings.avoidrepeat
        )
    ) + prandomshufflestate->settings.numbersetindex;

    if (prandomshufflestate->settings.sizeofnumbersetentry == 1) {
        prandomshufflestate->settings.outputcurrent = prandomshufflestate->settings.numberset.setu8[swapindex];
        prandomshufflestate->settings.numberset.setu8[swapindex] = prandomshufflestate->settings.numberset.setu8[prandomshufflestate->settings.numbersetindex];
        prandomshufflestate->settings.numberset.setu8[prandomshufflestate->settings.numbersetindex] = prandomshufflestate->settings.outputcurrent;
    } else {
        prandomshufflestate->settings.outputcurrent = prandomshufflestate->settings.numberset.setu16[swapindex];
        prandomshufflestate->settings.numberset.setu16[swapindex] = prandomshufflestate->settings.numberset.setu16[prandomshufflestate->settings.numbersetindex];
        prandomshufflestate->settings.numberset.setu16[prandomshufflestate->settings.numbersetindex] = prandomshufflestate->settings.outputcurrent;
    }

    prandomshufflestate->settings.numbersetindex++;
    prandomshufflestate->settings.outputcurrent += prandomshufflestate->settings.minvalue;
    if (prandomshufflestate->settings.numbersetindex >= prandomshufflestate->settings.range) {
        prandomshufflestate->settings.numbersetindex = 0;
        prandomshufflestate->settings.avoidrepeat = 1;
    } else {
        prandomshufflestate->settings.avoidrepeat = 0;
    }

    return prandomshufflestate->settings.outputcurrent;
}

// 967
int SNDAEMSI_updaterandomweighted(AemsDef::RANDOMWEIGHTEDSTATE *prandomweightedstate) {
    if (prandomweightedstate->triggered != 0) {
        unsigned int randval = iSNDrandom() % 100;
        AemsDef::TABLE *ptable = prandomweightedstate->settings.ptable;
        unsigned int total = 0;

        for (int i = 0; i < prandomweightedstate->settings.range; i++) {
            total += ptable->table.entry8[i];
            if (total > randval) {
                prandomweightedstate->settings.outputcurrent = i + prandomweightedstate->settings.minvalue;
                break;
            }
        }
    }

    return prandomweightedstate->settings.outputcurrent;
}

// 1007
int SNDAEMSI_updaterangetrig(AemsDef::RANGETRIGSTATE *prangetrigstate) {
    if (prangetrigstate->input >= prangetrigstate->settings.tripgte && prangetrigstate->input <= prangetrigstate->settings.triplte) {
        if (prangetrigstate->settings.hastripped == 0) {
            prangetrigstate->settings.hastripped = 1;
            prangetrigstate->settings.output = 1;
            return 1;
        }
    } else if (prangetrigstate->input >= prangetrigstate->settings.resetgte && prangetrigstate->input <= prangetrigstate->settings.resetlte) {
        prangetrigstate->settings.hastripped = 0;
    }

    prangetrigstate->settings.output = 0;
    return 0;
}

// 1035
int SNDAEMSI_updatedelaytrig(AemsDef::DELAYTRIGSTATE *pdelaytrigstate) {
    if (pdelaytrigstate->triggered != 0) {
        pdelaytrigstate->settings.curtime = 0.0f;
    }
    if (pdelaytrigstate->settings.curtime >= 0) {
        if (pdelaytrigstate->settings.curtime >= pdelaytrigstate->delaytime) {
            pdelaytrigstate->settings.output = 1;
            pdelaytrigstate->settings.curtime = -1;
            return 1;
        } else {
            pdelaytrigstate->settings.curtime += Snd::gVariableTimerPeriod;
        }
    }

    pdelaytrigstate->settings.output = 0;
    return 0;
}

// 1066
int SNDAEMSI_updatestategen(AemsDef::STATEGENSTATE *pstategenstate) {
    AemsDef::STATEGENINPUTS *pstategeninputs = reinterpret_cast<AemsDef::STATEGENINPUTS *>(
        &reinterpret_cast<char *>(pstategenstate)[pstategenstate->settings.inputoffset]
    );

    for (int i = 0; i < pstategenstate->settings.numinputs; i++) {
        if (pstategeninputs[0].triggered[i] != 0) {
            pstategenstate->settings.outputcurrent = pstategenstate->settings.outputvalue[i];
            break;
        }
    }

    return pstategenstate->settings.outputcurrent;
}

// 1093
int SNDAEMSI_updatemerge(AemsDef::MERGETRIGSTATE *pmergetrigstate) {
    int i = 0;
    if (i < pmergetrigstate->settings.numinputs) {
        for(; i < pmergetrigstate->settings.numinputs; i++) {
            if (pmergetrigstate->settings.triggered[i] != 0) {
                return 1;
            }
        }
    }

    return 0;
}

// 1107
void SNDAEMSI_envelopeprogramsegment(AemsDef::ENVELOPESTATE *penvelopestate) {
    penvelopestate->settings.segmentremaining = penvelopestate->settings.segment[penvelopestate->settings.cursegment].duration;
    penvelopestate->settings.delta = ((penvelopestate->settings.segment[penvelopestate->settings.cursegment].targetvalue - penvelopestate->settings.outputvalue) / penvelopestate->settings.segmentremaining);
    penvelopestate->settings.delta *= Snd::gVariableTimerPeriod;
}

// 1124
int SNDAEMSI_updateenvelope(AemsDef::ENVELOPESTATE *penvelopestate) {
    AemsDef::ENVELOPEINPUTS *penvelopeinputs = reinterpret_cast<AemsDef::ENVELOPEINPUTS *>(
        &reinterpret_cast<char *>(penvelopestate)[penvelopestate->settings.inputoffset]
    );

    if (penvelopeinputs->control == 1 && penvelopestate->settings.prevcontrol == 0) {
        penvelopestate->settings.outputvalue = penvelopestate->settings.initialvalue;
        penvelopestate->settings.cursegment = static_cast<unsigned char>(penvelopestate->settings.prevcontrol != 0);
        SNDAEMSI_envelopeprogramsegment(penvelopestate);
    } else if (penvelopeinputs->control == 3 && penvelopestate->settings.prevcontrol != 3 && penvelopestate->settings.cursegment < penvelopestate->settings.releasesegment) {
        penvelopestate->settings.cursegment = penvelopestate->settings.releasesegment;
        SNDAEMSI_envelopeprogramsegment(penvelopestate);
    } else if ((penvelopeinputs->control == 1 || penvelopeinputs->control == 3) && penvelopestate->settings.cursegment < penvelopestate->settings.numsegments) {
        penvelopestate->settings.segmentremaining -= Snd::gVariableTimerPeriod;
        if (penvelopestate->settings.segmentremaining <= 0.0f) {
            penvelopestate->settings.outputvalue = penvelopestate->settings.segment[penvelopestate->settings.cursegment].targetvalue;
            penvelopestate->settings.cursegment++;
            if (penvelopestate->settings.cursegment < penvelopestate->settings.numsegments) {
                SNDAEMSI_envelopeprogramsegment(penvelopestate);
            } else {
                penvelopestate->settings.outputvalue = 0.0f;
            }
        } else {
            penvelopestate->settings.outputvalue += penvelopestate->settings.delta;
        }
    } else if (penvelopeinputs->control != 2) {
        penvelopestate->settings.outputvalue = 0.0f;
    }

    penvelopestate->settings.prevcontrol = penvelopeinputs->control;
    return SNDI_ftoifast(penvelopestate->settings.outputvalue);
}

// 1218
int SNDAEMSI_updatetable(AemsDef::TABLESTATE *ptablestate) {
    if (ptablestate->inputvalue != ptablestate->settings.previnputvalue) {
        AemsDef::TABLE *ptable = ptablestate->settings.ptable;
        ptablestate->settings.previnputvalue = ptablestate->inputvalue;

        float floatindex;
        int indexa = SNDI_clipint32(ptablestate->inputvalue, ptable->minvalue, ptable->maxvalue);
        indexa -= ptable->minvalue;
        floatindex = ptable->resolution;

        if (ptable->resolution == 1.0f) {
            if (ptable->sizeofentry == 2) {
                ptablestate->settings.outputvalue = ptable->table.entry16[indexa];
            } else if (ptable->sizeofentry == 1) {
                ptablestate->settings.outputvalue = ptable->table.entry8[indexa];
            } else {
                ptablestate->settings.outputvalue = ptable->table.entry32[indexa];
            }
        } else {
            floatindex = indexa * ptable->resolution;
            indexa = SNDI_ftoitruncpositive(floatindex);
            floatindex -= indexa;

            int indexb = indexa;
            if (++indexb >= ptable->numentries) {
                indexb = ptable->numentries - 1;
            }

            float entrya;
            float entryb;

            if (ptable->sizeofentry == 2) {
                entrya = ptable->table.entry16[indexa];
                entryb = ptable->table.entry16[indexb];
            } else if (ptable->sizeofentry == 1) {
                entrya = ptable->table.entry8[indexa];
                entryb = ptable->table.entry8[indexb];
            } else {
                entrya = ptable->table.entry32[indexa];
                entryb = ptable->table.entry32[indexb];
            }
            ptablestate->settings.outputvalue = SNDI_ftoifast(floatindex * (entryb - entrya) + entrya);
        }
    }

    return ptablestate->settings.outputvalue;
}

// 1326
int SNDAEMSI_updatedelayline(AemsDef::DELAYLINESTATE *pdelaylinestate) {
    AemsDef::DELAYLINEINPUTS *pdelaylineinputs = reinterpret_cast<AemsDef::DELAYLINEINPUTS *>(
        &reinterpret_cast<char *>(pdelaylinestate)[pdelaylinestate->settings.inputoffset]
    );

    if (pdelaylineinputs->delaytime != pdelaylinestate->settings.prevdelaytime) {
        pdelaylinestate->settings.prevdelaytime = pdelaylineinputs->delaytime;
        if (pdelaylineinputs->delaytime < 0) {
            pdelaylineinputs->delaytime = 0;
        }

        int offset = SNDI_ftoiroundpositive(pdelaylineinputs->delaytime / Snd::gVariableTimerPeriod);
        if (offset >= pdelaylinestate->settings.maxslots) {
            offset = pdelaylinestate->settings.maxslots - 1;
        }
        pdelaylinestate->settings.inputslot = pdelaylinestate->settings.outputslot + offset;
    }

    if (pdelaylinestate->settings.inputslot >= pdelaylinestate->settings.maxslots) {
        pdelaylinestate->settings.inputslot -= pdelaylinestate->settings.maxslots;
    }

    if (pdelaylinestate->settings.outputslot >= pdelaylinestate->settings.maxslots) {
        pdelaylinestate->settings.outputslot = 0;
    }

    pdelaylinestate->settings.delayline[pdelaylinestate->settings.inputslot] = pdelaylineinputs->inputvalue;
    int outputvalue = pdelaylinestate->settings.delayline[pdelaylinestate->settings.outputslot];

    pdelaylinestate->settings.inputslot++;
    pdelaylinestate->settings.outputslot++;

    return outputvalue;
}

// 1411
int SNDAEMSI_updatemux(AemsDef::MUXSTATE *pmuxstate) {
    if (pmuxstate->u.control <= 0 || pmuxstate->u.control > pmuxstate->settings.numinputs) {
        return 0;
    }

    return pmuxstate->u.inputvalue[pmuxstate->u.control];
}

// 1427
int SNDAEMSI_updatedemux(AemsDef::DEMUXSTATE *pdemuxstate) {
    pdemuxstate->outputs.value[pdemuxstate->settings.prevcontrol - 1] = 0;

    if (pdemuxstate->inputs.control > 0 && pdemuxstate->inputs.control <= pdemuxstate->settings.numoutputs) {
        pdemuxstate->outputs.value[pdemuxstate->inputs.control - 1] = pdemuxstate->inputs.value;
        pdemuxstate->settings.prevcontrol = pdemuxstate->inputs.control;
    }

    return pdemuxstate->outputs.value[0];
}

// 1452
int SNDAEMSI_updatemin(AemsDef::MINSTATE *pminstate) {
    int min = pminstate->input[0];
    int i;

    for (i = 1; i < pminstate->settings.numinputs; i++) {
        if (pminstate->input[i] < min) {
            min = pminstate->input[i];
        }
    }

    return min;
}

// 1468
int SNDAEMSI_updatemin2(AemsDef::MIN2STATE *pmin2state) {
    if (pmin2state->inputa < pmin2state->inputb) {
        return pmin2state->inputa;
    } else {
        return pmin2state->inputb;
    }
}

// 1478
int SNDAEMSI_updatemax(AemsDef::MAXSTATE *pmaxstate) {
    int max = pmaxstate->input[0];
    int i;

    for (i = 1; i < pmaxstate->settings.numinputs; i++) {
        if (pmaxstate->input[i] > max) {
            max = pmaxstate->input[i];
        }
    }

    return max;
}

// 1495
int SNDAEMSI_updatemax2(AemsDef::MAX2STATE *pmax2state) {
    if (pmax2state->inputa > pmax2state->inputb) {
        return pmax2state->inputa;
    } else {
        return pmax2state->inputb;
    }
}

// 1504
int SNDAEMSI_updatescale(AemsDef::SCALESTATE *pscalestate) {
    float output = pscalestate->input[0];
    int i;

    for (i = 1; i < pscalestate->settings.numinputs; i++) {
        output *= pscalestate->input[i];
    }

    return SNDI_ftoifast(output * pscalestate->settings.scalereciprocal);
}

// 1529
int SNDAEMSI_updatescale2(AemsDef::SCALE2STATE *pscale2state) {
    return SNDI_ftoifast(static_cast<float>(pscale2state->inputa) * static_cast<float>(pscale2state->inputb) * pscale2state->settings.scalereciprocal);
}

// 1537
int SNDAEMSI_updateadd(AemsDef::ADDSTATE *paddstate) {
    int i;
    int output = paddstate->input[0];
    for (i = 1; i < paddstate->settings.numinputs; i++) {
        output += paddstate->input[i];
    }

    return output;
}

// 1552
int SNDAEMSI_updateadd2(AemsDef::ADD2STATE *padd2state) {
    return padd2state->inputa + padd2state->inputb;
}

// 1559
int SNDAEMSI_updatesubtract(AemsDef::SUBTRACTSTATE *psubtractstate) {
    return psubtractstate->inputa - psubtractstate->inputb;
}

// 1566
int SNDAEMSI_updatemultiply(AemsDef::MULTIPLYSTATE *pmultiplystate) {
    return pmultiplystate->inputa * pmultiplystate->inputb;
}

// 1572
int SNDAEMSI_updatedivide(AemsDef::DIVIDESTATE *pdividestate) {
    if (pdividestate->inputb != 0) {
        return pdividestate->inputa / pdividestate->inputb;
    }
    return 0;
}

// 1582
int SNDAEMSI_updatemodulo(AemsDef::MODULOSTATE *pmodulostate) {
    if (pmodulostate->inputb != 0) {
        return pmodulostate->inputa % pmodulostate->inputb;
    }
    return 0;
}

void SNDAEMSI_bankinputstub(int handle, int value) {}

void SNDAEMSI_bankpitchmult(int handle, int value) {
    sndgs.chan[handle].finalpitch = SNDI_clipint32(value, 0, 0xFFFF);
    SNDPLATFORM_setpitch(handle);
}

void SNDAEMSI_banktimemult(int handle, int value) {
    if (value > 0x2000) {
        value = 0x2000;
    } else if (value < 0x800) {
        value = 0x800;
    }
    sndgs.chan[handle].timemult = value;
    SNDPLATFORM_timemult(handle, value);
}

void SNDAEMSI_bankvol(int handle, int value) {
    CHANPUB *pv = &sndgs.chan[handle];

    value = SNDI_clipint32(value, 0, 0x7FFF);
    pv->programmedVol = value * (1.0f / 32767.0f);
    Snd::Util::FastVol(pv);

    int i;
    for (i = 0; i < pv->numchan; i++) {
        CHANPUB *pSourceVoice;
        int sourceVoice;

        sourceVoice = pv->voices[i];
        pSourceVoice = &sndgs.chan[pv->voices[i]];

        pSourceVoice->programmedVol = pv->programmedVol;
        pSourceVoice->finalvol = pv->finalvol;
        Snd::Hal::SetVol(sourceVoice);
    }
}

void SNDAEMSI_bankazimuth(int handle, int value) {
    CHANPUB *pVoice = &sndgs.chan[handle];
    if (pVoice->numchan == 1) {
        pVoice->azimuth = value;
        Snd::Hal::SetPan(handle);
    }
}

void SNDAEMSI_bankfxwet0(int handle, int value) {
    sndgs.chan[handle].progfxlevel = SNDI_clipint32(value, 0, 0x7FFF) >> 8;
    sndgs.chan[handle].pFxVolume->fxLevel = sndgs.chan[handle].progfxlevel * (1.0f / 127.0f);

    SNDPLATFORM_setfxlevel(handle, 0);
}

void SNDAEMSI_banklowpass(int handle, int value) {
    SNDPLATFORM_lowpass(handle, SNDI_clipint32(value, 0, 0xFFFF));
}

void SNDAEMSI_bankhighpass(int handle, int value) {
    SNDPLATFORM_highpass(handle, SNDI_clipint32(value, 0, 0xFFFF));
}

void SNDAEMSI_bankdrylevel(int handle, int value) {
    CHANPUB *pVoice = &sndgs.chan[handle];
    int i;

    value = SNDI_clipint32(value, 0, 0x7FFF) >> 8;
    for (i = 0; i < pVoice->numchan; i++) {
        int sourceVoice = pVoice->voices[i];
        sndgs.chan[sourceVoice].drylevel = value;
        Snd::Hal::SetDry(sourceVoice);
    }
}

void SNDAEMSI_optpitchmult(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->pitchmult = SNDI_clipint32(value, 0, 0xFFFF);
}

void SNDAEMSI_opttimemult(SNDPLAYOPTS *psndplayopts, int value) {
    if (value > 0x2000) {
        value = 0x2000;
    } else if (value < 0x800) {
        value = 0x800;
    }
    psndplayopts->timemult = value;
}

void SNDAEMSI_optvol(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->vol = SNDI_clipint32(value, 0, 0x7FFF) >> 8;
}

void SNDAEMSI_optazimuth(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->azimuth = value;
}

void SNDAEMSI_optfxwet0(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->fxlevel0 = SNDI_clipint32(value, 0, 0x7FFF) >> 8;
}

void SNDAEMSI_optlowpass(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->lowpasscutoff = SNDI_clipint32(value, 0, 0xFFFF);
}

void SNDAEMSI_opthighpass(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->highpasscutoff = SNDI_clipint32(value, 0, 0xFFFF);
}

void SNDAEMSI_optdrylevel(SNDPLAYOPTS *psndplayopts, int value) {
    psndplayopts->drylevel = SNDI_clipint32(value, 0, 0x7FFF) >> 8;
}

void SNDAEMSI_optstub(SNDPLAYOPTS *psndplayopts, int value) {}

AemsDef::PLAYERHANDLE SNDAEMSI_playerplaybank(AemsDef::PLAYERSTATE *pplayerstate, AemsDef::SAMPLEENTRY *psampleentry) {
    SNDPLAYOPTS spo;
    AemsDef::PLAYERHANDLE playerhandle;
    AemsDef::PLAYERINPUTSTATE *pplayerinputstate;
    int i;

    SNDplaysetdef(&spo);
    spo.fxlevel0 = 0;
    pplayerinputstate = reinterpret_cast<AemsDef::PLAYERINPUTSTATE *>(&pplayerstate[1]);

    for (i = 0; i < pplayerstate->settings.numinputs; i++) {
        SNDAEMSoptfn[pplayerinputstate->settings.type](&spo, pplayerinputstate->value);
        pplayerinputstate->settings.prevvalue = pplayerinputstate->value;
        pplayerinputstate++;
    }

    playerhandle.u.shandle = SNDBANK_play(pplayerstate->settings.pModuleBank->sfxbhandle, psampleentry->u.stream.offset, &spo);

    return playerhandle;
}

void SNDAEMSI_playerstopbank(AemsDef::PLAYERSTATE *pplayerstate) {
    SNDstop(pplayerstate->settings.handle.u.shandle);
}

void SNDAEMSI_playerpausebank(AemsDef::PLAYERHANDLE pplayerstate) {
    SNDvol(pplayerstate.u.shandle, 0);
    SNDpitchmult(pplayerstate.u.shandle, 0);
}

void SNDAEMSI_playerunpausebank(AemsDef::PLAYERSTATE *pplayerstate) {
    int pitchmult = 0x1000;
    int vol = 0x7FFF;
    int voice = SNDVOICEI_get(pplayerstate->settings.handle.u.shandle);
    if (voice < 0) return;

    AemsDef::PLAYERINPUTSTATE *pplayerinputstate = reinterpret_cast<AemsDef::PLAYERINPUTSTATE *>(&pplayerstate[1]);
    int i;

    for (i = 0; i < pplayerstate->settings.numinputs; i++) {
        if (pplayerinputstate->settings.type == 0) {
            pitchmult = pplayerinputstate->value;
            pplayerinputstate->settings.prevvalue = pitchmult;
        } else if (pplayerinputstate->settings.type == 2) {
            vol = pplayerinputstate->value;
            pplayerinputstate->settings.prevvalue = vol;
        }

        pplayerinputstate++;
    }

    SNDAEMSI_bankpitchmult(voice, pitchmult);
    SNDAEMSI_bankvol(voice, vol);
}

// 1595
int SNDAEMSI_playerresetoutputs(AemsDef::PLAYERSTATE *pplayerstate) {
    pplayerstate->settings.sampletype = -1;
    if (pplayerstate->settings.updateoutputs != 0) {
        AemsDef::PLAYERINPUTSTATE *pplayerinputstate = reinterpret_cast<AemsDef::PLAYERINPUTSTATE *>(&pplayerstate[1]);
        AemsDef::PLAYEROUTPUTS *pplayeroutputs = reinterpret_cast<AemsDef::PLAYEROUTPUTS *>(
            &pplayerinputstate[pplayerstate->settings.numinputs]
        );

        pplayeroutputs->timecurr = 0;
        pplayeroutputs->timeleft = 0;
    }
    return 0;
}

int SNDAEMSI_playerupdatebank(AemsDef::PLAYERSTATE *pplayerstate) {
    AemsDef::PLAYERINPUTSTATE *pplayerinputstate = reinterpret_cast<AemsDef::PLAYERINPUTSTATE *>(&pplayerstate[1]);
    AemsDef::PLAYEROUTPUTS *pplayeroutputs;

    int voice = SNDVOICEI_get(pplayerstate->settings.handle.u.shandle);
    if (voice < 0) {
        return SNDAEMSI_playerresetoutputs(pplayerstate);
    }

    int i;
    for (i = 0; i < pplayerstate->settings.numinputs; i++, pplayerinputstate++) {
        if (pplayerinputstate->value != pplayerinputstate->settings.prevvalue) {
            playerinterfacefn[pplayerinputstate->settings.type](voice, pplayerinputstate->value);
            pplayerinputstate->settings.prevvalue = pplayerinputstate->value;
        }
    }

    pplayeroutputs = reinterpret_cast<AemsDef::PLAYEROUTPUTS *>(pplayerinputstate);
    if (pplayerstate->settings.updateoutputs > 0) {
        CHANPUB *pv = &sndgs.chan[voice];
        int curframe;
        float inverse = 1000.0f / static_cast<float>(pv->samplerate);
        curframe = SNDPLATFORM_getcurframe(voice);

        pplayeroutputs->timecurr = SNDI_ftoifast(curframe * inverse);
        if (pv->sustainend <= 0) {
            pplayeroutputs->timeleft = SNDI_ftoifast((pv->frames - curframe) * inverse);
        } else {
            pplayeroutputs->timeleft = 0;
        }
    }

    return 1;
}

int SNDAEMSI_updateplayer(AemsDef::PLAYERSTATE *pplayerstate) {
    int outputplaystate = SNDI_clipint32(pplayerstate->playcontrol, 0, 2);

    if (outputplaystate != pplayerstate->settings.prevplaycontrol[0]) {
        if (outputplaystate == 0) {
            if (pplayerstate->settings.sampletype >= 0) {
                SNDAEMSplayerstopfn[pplayerstate->settings.sampletype](pplayerstate);
                SNDAEMSI_playerresetoutputs(pplayerstate);
            }
        } else if (outputplaystate == 1) {
            if (pplayerstate->settings.sampletype >= 0) {
                SNDAEMSplayerunpausefn[pplayerstate->settings.sampletype](pplayerstate);
            } else if ((pplayerstate->settings.prevplaycontrol[0] & ~0xFFFF) != 0x2010000) {
                int sampleselect = pplayerstate->sampleselect;
                if (sampleselect >= pplayerstate->settings.psamplegroup->numentries) {
                    sampleselect = pplayerstate->settings.psamplegroup->numentries - 1;
                } else if (sampleselect < 0) {
                    sampleselect = 0;
                }

                int sampletype = (signed char)pplayerstate->settings.psamplegroup->entry[sampleselect].type;
                pplayerstate->settings.sampletype = sampletype;
                pplayerstate->settings.handle = SNDAEMSplayerplayfn[sampletype](pplayerstate, &pplayerstate->settings.psamplegroup->entry[sampleselect]);

                if (pplayerstate->settings.handle.u.shandle < 0) {
                    SNDAEMSI_playerresetoutputs(pplayerstate);
                }
            }
        } else {
            if (pplayerstate->settings.sampletype >= 0) {
                SNDAEMSplayerpausefn[pplayerstate->settings.sampletype](pplayerstate->settings.handle);
            }
        }

        pplayerstate->settings.prevplaycontrol[1] = pplayerstate->settings.prevplaycontrol[0];
        pplayerstate->settings.prevplaycontrol[0] = outputplaystate;
    }
    if (outputplaystate == 1 && pplayerstate->settings.sampletype >= 0) {
        outputplaystate = SNDAEMSplayerupdatefn[pplayerstate->settings.sampletype](pplayerstate);
    }
    if (pplayerstate->settings.sampletype & 0x80) {
        outputplaystate = 0;
    }

    return outputplaystate;
}

unsigned int sndaemsfuncs[40] = {
    reinterpret_cast<unsigned int>(SNDAEMSI_UpdateClassDestructor),
    reinterpret_cast<unsigned int>(SNDAEMSI_UpdateClassData),
    reinterpret_cast<unsigned int>(SNDAEMSI_UpdateGlobalVariable),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatecreate),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatedestroy),
    reinterpret_cast<unsigned int>(UpdateCallFunction),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatecounter),
    reinterpret_cast<unsigned int>(SNDAEMSI_updaterandom),
    reinterpret_cast<unsigned int>(SNDAEMSI_updaterandomshuffle),
    reinterpret_cast<unsigned int>(SNDAEMSI_updaterandomweighted),
    reinterpret_cast<unsigned int>(SNDAEMSI_updaterangetrig),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatedelaytrig),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatestategen),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatemerge),
    reinterpret_cast<unsigned int>(SNDAEMSI_updateenvelope),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatetable),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatedelayline),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatemux),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatedemux),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatemin),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatemax),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatescale),
    reinterpret_cast<unsigned int>(SNDAEMSI_updateadd),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatesubtract),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatemultiply),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatedivide),
    reinterpret_cast<unsigned int>(SNDAEMSI_updatemodulo),
    // reinterpret_cast<unsigned int>(SNDAEMSI_updateplayer),
};
