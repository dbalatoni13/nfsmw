#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndenum.h"
#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndfxcmn.h"
#include "csis/csis.h"
#include "snd/sndo.h"
#include <cstring>

namespace Snd {

GlobalFxProcessor::~GlobalFxProcessor() {
    GlobalFxProcessorData *pGlobalFxProcessorData = reinterpret_cast<GlobalFxProcessorData *>(this);
    if (pGlobalFxProcessorData != NULL && (unsigned int)pGlobalFxProcessorData->fxBusTag == 0x46584253) {
        if (!pGlobalFxProcessorData->isReset) {
            this->Reset();
        }

        memset(pGlobalFxProcessorData, 0, sizeof(GlobalFxProcessorData));

        SNDSYS_entercritical();
        SNDMEMI_free(this);
        SNDSYS_leavecritical();
    }
}

Csis::Result GlobalFxProcessor::CreateInstance(Device device, int bus, GlobalFxProcessor **ppGlobalFxProcessor) {
    int i;

    SNDSYS_entercritical();
    GlobalFxProcessorData *pGlobalFxProcessorData = reinterpret_cast<GlobalFxProcessorData *>(
        SNDMEMI_allocz(sizeof(GlobalFxProcessorData))
    );
    SNDSYS_leavecritical();

    memset(pGlobalFxProcessorData, 0, sizeof(GlobalFxProcessorData));

    if (pGlobalFxProcessorData == NULL) {
        return Csis::RESULT_ERR_ALLOCATE;
    }

    pGlobalFxProcessorData->fxBusTag = 0x46584253;
    pGlobalFxProcessorData->busId = bus;
    for (i = 0; i < 6; i++) {
        pGlobalFxProcessorData->fxBusOutputLevel[i] = 1.0f;
    }
    pGlobalFxProcessorData->device = device;
    pGlobalFxProcessorData->isReset = 1;

    *ppGlobalFxProcessor = reinterpret_cast<GlobalFxProcessor *>(pGlobalFxProcessorData);

    return Csis::RESULT_OK;
}

Csis::Result GlobalFxProcessor::Release() {
    this->~GlobalFxProcessor();
    return Csis::RESULT_OK;
}

Csis::Result GlobalFxProcessor::SetCustom(void *pFxDefinition) {
    GlobalFxProcessorData *mp = reinterpret_cast<GlobalFxProcessorData *>(this);
    if (!mp->isReset) {
        this->Reset();
    }
    mp->instanceHandle = Snd::Hal::SetCustomFx(mp, pFxDefinition);
    mp->isReset = 0;

    return Csis::RESULT_OK;
}

Csis::Result GlobalFxProcessor::Reset() {
    GlobalFxProcessorData *mp = reinterpret_cast<GlobalFxProcessorData *>(this);
    if (mp->fxBusTag == 0x46584253) {
        if (!mp->isReset) {
            mp->instanceHandle = Snd::Hal::Reset(mp);
        }
        mp->isReset = 1;
    }

    return Csis::RESULT_OK;
}

// TODO: This match feels kinda bad. Are we really keeping rodata from a
//      stripped function?
Csis::Result GlobalFxProcessor::SetOutputLevel(float level) {
    int i;
    GlobalFxProcessorData *mp = reinterpret_cast<GlobalFxProcessorData *>(this);

    if (level > 1.0f) {
        level = 1.0f;
    } else if (level < 0.0f) {
        level = 0.0f;
    }

    for (i = 0; i < 6; i++) {
        mp->fxBusOutputLevel[i] = level;
    }

    return Csis::RESULT_OK;
}

}

// La .rodata del original tiene alineacion 8 (16 B); la nuestra sale con
// alineacion 4 (12 B) y el enlace se desplaza 8 B. No hay ningun objeto con
// nombre en esta seccion donde colgar el __attribute__((aligned(8))): esta
// constante solo existe para dar esa alineacion y el enlazador la retira
// (-strip-unused-data), como demuestra el DOL byte a byte identico.
namespace Snd {
extern const double sndfxbus_rodata_align8 __attribute__((aligned(8))) = 0.0;
}
