#include "gc_interface.h"

#include "gc_effect.h"
#include "gc_pad.h"

#include "../../../include/common/realcore/std.h"

#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/pad.h>
#endif
#ifdef EA_PLATFORM_GAMECUBE
#include <dolphin/si.h>
#endif

static char FatalParam[12];
static char FatalContext[712];

namespace RealInput {

volatile PADStatus gPadstat[4];
int gPadUpdate = 0;
/* gResetBit vive en .sdata 0x804FF6C8, FUERA del rango que splits.txt le da a
   esta unidad (0x804FF6C0..0x804FF6C8, que son gPadUpdate mas 4 B de relleno);
   lo define el comodin auto_08_804FF6C8_sdata.  Definirlo aqui ponia a
   gc_pad.cpp 4 B por debajo en sus cinco @sda21. */
extern unsigned int gResetBit;

static PADSamplingCallback sGcOriginalSamplingCallback;

static void GCPADVBLUpdate() {
    PADStatus pads[4];
    int chan;

    PADRead(pads);
    PADClamp(pads);
    for (chan = 0; chan < 4; chan++) {
        if (pads[chan].err != PAD_ERR_TRANSFER) {
            MEM_copy(const_cast<PADStatus *>(&gPadstat[chan]), &pads[chan], sizeof(PADStatus));
        }
    }
    gPadUpdate = 1;
}

GcInterface::GcInterface(const ConfigOptions &options)
    : InterfaceImp(options) {
    this->mpEffectTable = nullptr;
    PADSetAnalogMode(3);
    PADInit();
    do {
    } while (PADReset(0xf0000000) == 0);
    sGcOriginalSamplingCallback = PADSetSamplingCallback(GCPADVBLUpdate);
    SISetSamplingRate(0);
    if (this->mConfigOptions.mMaxNumEffects > 0) {
        this->mpEffectTable = new GcEffect[this->mConfigOptions.mMaxNumEffects];
    }
    this->EnumerateDevices();
}

GcInterface::~GcInterface() {
    if (this->mpEffectTable != nullptr) {
        delete[] this->mpEffectTable;
    }
    PADSetSamplingCallback(sGcOriginalSamplingCallback);
}

GcEffect *GcInterface::GetUnusedEffectSlot() {
    GcEffect *pE;

    if (this->mpEffectTable == nullptr) {
        return nullptr;
    }

    {
        unsigned int i;

        pE = this->mpEffectTable;
        i = 0;
        while (i < this->mConfigOptions.mMaxNumEffects) {
            if (!pE->IsInUse()) {
                return pE;
            }
            i++;
            pE++;
        }
    }
    return nullptr;
}

RiResult GcInterface::EnumerateDevices() {
    GcInterface *pInterface = this;
    Device *pDevice;

    {
        int dvindex;

        dvindex = 0;
    again:
        pDevice = nullptr;
        switch (dvindex) {
        case 0:
            pDevice = new GcPad(pInterface, 0, gPadstat + 0);
            break;
        case 1:
            pDevice = new GcPad(pInterface, 1, gPadstat + 1);
            break;
        case 2:
            pDevice = new GcPad(pInterface, 2, gPadstat + 2);
            break;
        case 3:
            pDevice = new GcPad(pInterface, 3, gPadstat + 3);
            break;
        }

        if (pDevice != nullptr) {
            {
                int keepit = 1;
                ConfigOptions *pCfgOptions = pInterface->GetConfigOptions();
                if (pCfgOptions != nullptr && pCfgOptions->mpEnumDevicesCallback != nullptr) {
                    keepit = pCfgOptions->mpEnumDevicesCallback(
                        pDevice, pCfgOptions->mEnumDevicesCallbackUserData, pInterface);
                }
            if (keepit == 0 && pDevice != nullptr) {
                delete pDevice;
            }
            }
        }
        dvindex++;
        if (dvindex <= 3) {
            goto again;
        }
    }
    return RI_OK;
}

} // namespace RealInput
