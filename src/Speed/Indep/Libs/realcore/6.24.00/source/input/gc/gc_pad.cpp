#include "gc_pad.h"

#include "../cmn/eventqueue.h"
#include "../cmn/imem.h"
#include "gc_interface.h"

#include <dolphin/os.h>
#include <dolphin/si.h>

namespace RealInput {

extern Interface *gInterface;
extern unsigned int gResetBit;
extern int gPadUpdate;

GcPad::GcPad(GcInterface *pInterface, unsigned int port, volatile PADStatus *PStat)
    : GcDevice(TYPE_PAD, pInterface) {
    this->mInfo.mPortNum = port;
    this->mInfo.mControllerID = 0;
    this->mCapabilities.mNumDigitalButtons = 0;
    this->mCapabilities.mNumAnalogButtons = 0;
    this->mCapabilities.mAttached = 0;
    this->mCapabilities.mForceFeedback = 0;
    this->mpEffect = nullptr;
    this->mInitialized = false;
    this->SetPortNum(port);
    this->mPadStat = PStat;
    PStat->err = -1;

    switch (port) {
    case 0:
        this->mResetBit = 0x80000000;
        break;
    case 1:
        this->mResetBit = 0x40000000;
        break;
    case 2:
        this->mResetBit = 0x20000000;
        break;
    case 3:
        this->mResetBit = 0x10000000;
        break;
    }

    IMem::Clear(&this->mData, sizeof(this->mData));
    this->mpInterface->RegisterDevice(this);
}

Effect *GcPad::CreateEffect(Effect::Info *pInfo) {
    GcInterface *pInt;

    if (!this->mCapabilities.mAttached) {
        return nullptr;
    }
    if (!this->GetCapabilities()->mForceFeedback) {
        return nullptr;
    }
    if (this->mpEffect != nullptr) {
        this->mpEffect->Release();
    }
    pInt = static_cast<GcInterface *>(gInterface);
    this->mpEffect = pInt->GetUnusedEffectSlot();
    this->mpEffect->Init(this, pInfo);
    return this->mpEffect;
}

GcPad::~GcPad() {
    this->mpInterface->UnRegisterDevice(this);
    if (this->mpEffect != nullptr) {
        delete this->mpEffect;
    }
}

RiResult GcPad::Update() {
    PADStatus PStat;
    int i;
    EventQueue *pEventQ = this->mpInterface->GetEventQueue();

    if (pEventQ == nullptr) {
        return RI_OK;
    }

    {
    Event event;
    Device::Pad &data = this->mData.mPad;
    int enabled;
    int ctype;

    IMem::Clear(&event, sizeof(event));
    event.mpDevice = this;
    event.mTimestamp = ITimer::GetTime();

    enabled = OSDisableInterrupts();
    MEM_copy(&PStat, const_cast<const PADStatus *>(this->mPadStat), sizeof(PADStatus));
    OSRestoreInterrupts(enabled);

    ctype = SIProbe(this->mInfo.mPortNum);
    if (ctype == SI_GC_WAVEBIRD || ctype == SI_GC_CONTROLLER) {
        if (!(PStat.err != PAD_ERR_NO_CONTROLLER && this->mInitialized)) {
            PStat.err = PAD_ERR_NOT_READY;
            gResetBit |= this->mResetBit;
            if (ctype == SI_GC_CONTROLLER) {
                this->mInfo.mControllerID = 1;
                this->mCapabilities.mForceFeedback = 1;
            } else {
                this->mInfo.mControllerID = 2;
                this->mCapabilities.mForceFeedback = 0;
            }
            this->mCapabilities.mNumDigitalButtons = 12;
            this->mCapabilities.mNumAnalogButtons = 2;
            if (!this->mCapabilities.mAttached) {
                event.mType = TYPE_CONNECTED;
                pEventQ->AddEvent(&event);
                PADReset(gResetBit);
            }
            this->mCapabilities.mAttached = 1;
        } else {
            int DButtons[64];
            int Analog[64];
            int dvalue;
            const int AnalogDisplace = 32;
            const int NumDButtons = 12;
            int button = PStat.button;

            for (i = 0; i < NumDButtons; i++) {
                if (i == 7) {
                    button >>= 1;
                }
                DButtons[i] = (button & 1) != 0 ? 0xff : 0;
                button >>= 1;
            }

            Analog[0] = PStat.stickX;
            Analog[1] = PStat.stickY;
            Analog[2] = PStat.substickX;
            Analog[3] = PStat.substickY;
            Analog[4] = PStat.triggerLeft;
            Analog[5] = PStat.triggerRight;

            for (i = 0; i < 4; i++) {
                if (Analog[i] != data.mButtons[AnalogDisplace + i]) {
                    event.mType = TYPE_CHANGE;
                    event.mObject = OBJECT_DIRECTIONLEFT;
                    switch (i) {
                    case 0:
                        data.mDirection[0].mX = Analog[i];
                        event.mObjectIndex = 0;
                        break;
                    case 1:
                        data.mDirection[0].mY = Analog[i];
                        event.mObjectIndex = 0;
                        break;
                    case 2:
                        event.mObject = OBJECT_DIRECTIONRIGHT;
                        data.mDirection[1].mX = Analog[i];
                        event.mObjectIndex = 1;
                        break;
                    case 3:
                        event.mObject = OBJECT_DIRECTIONRIGHT;
                        data.mDirection[1].mY = Analog[i];
                        event.mObjectIndex = 1;
                        break;
                    }
                    pEventQ->AddEvent(&event);
                    data.mButtons[AnalogDisplace + i] = Analog[i];
                }
            }

            for (i = 0; i < 2; i++) {
                if (Analog[i + 4] != data.mButtons[i + NumDButtons]) {
                    event.mObject = OBJECT_BUTTON;
                    event.mType = TYPE_CHANGE;
                    event.mObjectIndex = i + NumDButtons;
                    if (data.mButtons[i + NumDButtons] != 0) {
                        if (Analog[i + 4] == 0) {
                            event.mType = TYPE_RELEASED;
                        }
                    } else if (Analog[i + 4] != 0) {
                        event.mType = TYPE_PRESSED;
                    }
                    pEventQ->AddEvent(&event);
                    data.mButtons[i + NumDButtons] = Analog[i + 4];
                }
            }

            for (i = 0; i < NumDButtons; i++) {
                dvalue = DButtons[i];
                if (dvalue != data.mButtons[i]) {
                    data.mButtons[i] = dvalue;
                    event.mObject = OBJECT_BUTTON;
                    event.mType = TYPE_RELEASED;
                    if (dvalue != 0) {
                        event.mType = TYPE_PRESSED;
                    }
                    event.mObjectIndex = i;
                    pEventQ->AddEvent(&event);
                }
            }
        }
    } else {
        this->mPadStat->err = PAD_ERR_NO_CONTROLLER;
        if (this->mCapabilities.mAttached) {
            IMem::Clear(&data, sizeof(data));
            event.mType = TYPE_DISCONNECTED;
            pEventQ->AddEvent(&event);
        }
        this->mCapabilities.mAttached = 0;
    }

    this->mInitialized = true;
    if (gPadUpdate != 0 && gResetBit != 0) {
        PADReset(gResetBit);
        gResetBit = 0;
    }
    gPadUpdate = 0;
    return RI_OK;
    }
}

} // namespace RealInput
