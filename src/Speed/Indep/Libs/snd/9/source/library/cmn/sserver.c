/* VEDA r46 ROTA en la r51.  El orden del `.bss` SI tiene mando, pero no desde
   las definiciones: lo fija la posicion de la PRIMERA declaracion de cada
   nombre en la cadena del espacio de nombres, y `gVariableTimerPeriod` ya
   estaba declarada en `sndcmn.h` antes de que este fichero abra su bloque
   `namespace Snd`.  Por eso las seis permutaciones de las definiciones --y un
   `extern` puesto DESPUES del include-- salian identicas.  Declarando el tick
   ANTES del include entra el primero en la cadena y el volcado final del
   `.lcomm` sale lista, tick, periodo, que es el del objetivo.  */
namespace Snd {
extern int gVariableTimerTick;
};

#include "./sndcmn.h"

namespace Snd {
// La lista sale antes que las otras dos porque tiene constructor y se emite
// junto a su init; las dos POD caen en la volcada final, y el orden de esa
// volcada es el de la cadena del espacio de nombres (ver la nota de arriba).
CListDStack gVariableTimerList;
int gVariableTimerTick;
float gVariableTimerPeriod;
};


void SNDI_mutexlock();
void SNDI_mutexunlock();

void SNDSYSI_variabletimerservice() {
    Snd::VariableTimerClient *pClient;
    /* El objetivo SI lo define: su `.data` son 12 B --lastTick, gMutexLockFn y
       gMutexUnlockFn-- y aqui estaba puesto como `extern` con el nombre del
       troceador. Con inicializador explicito GCC 2.9 lo emite en `.data` aunque
       el valor sea cero; sin el se va a `.bss` y sobran 4 B alli. */
    static unsigned int lastTick = 0;

    if (Snd::gVariableTimerTick != (int)lastTick) {
        lastTick = Snd::gVariableTimerTick;
        pClient = (Snd::VariableTimerClient *)Snd::gVariableTimerList.GetHead();
        while (pClient != 0) {
            CListDNode *pNode = pClient->ln.GetNext();

            pClient->pClientFunc(pClient->pClientData);
            pClient = (Snd::VariableTimerClient *)pNode;
        }
    }
}

void iSNDserve();

void SNDSYSI_100hzserver() {
    int i;

    sndgs.audiotick++;
    iSNDserve();

    for (i = 0; i < sndgs.numserverclients100hz; i++) {
        sndgs.serverclient100hz[i]();
    }

    for (i = 0; i < sndgs.voicestotal; i++) {
        CHANPUB *pc = &sndgs.chan[i];
        ENVELOPE *pe;
        int recalc;

        if (pc->status != 1) {
            continue;
        }

        if (sndgs.chan[i].handle >= 0 && pc->ppitchlfo != 0) {
            pc->curpitchlfoentry++;
            if (pc->curpitchlfoentry >= pc->pitchlfolength) {
                pc->curpitchlfoentry = 0;
            }
            pc->detunepitch = 0;
            iSNDcalcpitch(i);
            SNDPLATFORM_setpitch(i);
        }

        recalc = 0;

        if (pc->pvollfo != 0) {
            recalc = 1;
            pc->curvollfoentry++;
            if (pc->curvollfoentry >= pc->vollfolength) {
                pc->curvollfoentry = 0;
            }
        }

        if (pc->fadePer100Hz != 0.0f) {
            recalc = 1;
            pc->programmedVol += pc->fadePer100Hz;
            if (pc->fadePer100Hz < 0.0f) {
                if (pc->programmedVol <= pc->fadeTargetVol) {
                    pc->programmedVol = pc->fadeTargetVol;
                    pc->fadePer100Hz = 0.0f;
                }
            } else {
                if (pc->programmedVol >= pc->fadeTargetVol) {
                    pc->programmedVol = pc->fadeTargetVol;
                    pc->fadePer100Hz = 0.0f;
                }
            }
            if (pc->programmedVol < 0.0f) {
                SNDstop(pc->handle);
                continue;
            }
        }

        pc->envduration--;
        if (pc->envpertick != 0) {
            recalc = 1;
            pc->envvol += pc->envpertick;
        }

        if (pc->envduration == 0) {
            pc->curenvelope++;
            if (pc->curenvelope >= pc->numenvelopes) {
                SNDstop(pc->handle);
                continue;
            }
            pe = &pc->paenvelope[pc->curenvelope];
            pc->envduration = pe->duration;
            if (pc->envduration < 0) {
                pc->envduration = 0x7FFFFFFF;
            }
            pc->envpertick = ((pe->targetvol << 16) - pc->envvol) / pc->envduration;
        }

        if (recalc) {
            iSNDcalcvol(i);
            Snd::Hal::SetVol(i);
        }
    }
}


namespace Snd {

void DefaultMutexLockFn() {
    SNDI_mutexlock();
}

void DefaultMutexUnlockFn() {
    SNDI_mutexunlock();
}

void (*gMutexLockFn)() = DefaultMutexLockFn;
void (*gMutexUnlockFn)() = DefaultMutexUnlockFn;

} // namespace Snd

void SNDSYS_entercritical() {
    Snd::gMutexLockFn();
    sndgs.incritical++;
}

void SNDSYS_leavecritical() {
    sndgs.incritical--;
    Snd::gMutexUnlockFn();
}
