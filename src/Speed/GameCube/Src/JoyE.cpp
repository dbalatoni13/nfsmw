#include "Logitech/LGWheels.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"

#include <dolphin/pad.h>

struct PadData {
    unsigned short DigitalButtons;
    unsigned char LTrigger;
    unsigned char RTrigger;
    unsigned char AnalogLeftX;
    unsigned char AnalogLeftY;
    unsigned char AnalogRightX;
    unsigned char AnalogRightY;
    unsigned char Error;
    unsigned char Type;
};

struct JoyData {
    PadData ThePadData[4];
    unsigned char Bytes[4];
    int RegularControllerType;
    PADStatus padSTATUS;
};

static const unsigned long PADMASKS[4] = {
    PAD_CHAN0_BIT,
    PAD_CHAN1_BIT,
    PAD_CHAN2_BIT,
    PAD_CHAN3_BIT,
};

extern Timer RealTimer;

JoyData PadRingData[4][32];
int JoystickRingBufferTop;
int JoystickRingBufferBottom;
unsigned char notYetCalibrating[4];
unsigned char wasWheelConnected[4];
PADStatus HardwarePadStatus[4];
float calibrationTimer[4];
float lastCalibTime[4];
int JoystickInitialized;

static inline unsigned short ConvertPadButtons(unsigned short buttons) {
    unsigned short converted = 0xFFFF;

    if (buttons & PAD_BUTTON_A) {
        converted &= ~0x0001;
    }
    if (buttons & PAD_BUTTON_B) {
        converted &= ~0x0002;
    }
    if (buttons & PAD_BUTTON_X) {
        converted &= ~0x0004;
    }
    if (buttons & PAD_BUTTON_Y) {
        converted &= ~0x0008;
    }
    if (buttons & PAD_TRIGGER_Z) {
        converted &= ~0x0010;
    }
    if (buttons & PAD_BUTTON_START) {
        converted &= ~0x0020;
    }
    if (buttons & PAD_BUTTON_UP) {
        converted &= ~0x0100;
    }
    if (buttons & PAD_BUTTON_DOWN) {
        converted &= ~0x0200;
    }
    if (buttons & PAD_BUTTON_LEFT) {
        converted &= ~0x0400;
    }
    if (buttons & PAD_BUTTON_RIGHT) {
        converted &= ~0x0800;
    }

    return converted;
}

static inline unsigned char ClampAnalogValue(int value) {
    if (value < 0) {
        return 0;
    }
    if (value > 0xFF) {
        return 0xFF;
    }
    return value;
}

void PlatformInitJoystick() {
    int i;

    plat_lgwheels = new LGWheels();
    for (i = 0; i < 4; i++) {
        notYetCalibrating[i] = 1;
        wasWheelConnected[i] = 0;
    }
    PADRead(HardwarePadStatus);
    JoystickInitialized = 1;
}

void AutoCalibrateWheel(int channel) {
    plat_lgwheels->StopConstantForce(channel);
    plat_lgwheels->StopSurfaceEffect(channel);
    plat_lgwheels->StopDamperForce(channel);
    plat_lgwheels->StopCarAirborne(channel);
    plat_lgwheels->StopSlipperyRoadEffect(channel);
    plat_lgwheels->StopSpringForce(channel);
    plat_lgwheels->PlayAutoCalibAndSpringForce(channel);
}

void ReadLGWheelDataForProgressiveMenu() {
    if (plat_lgwheels) {
        plat_lgwheels->ReadAll();
    }
}

unsigned int ReadLGWheelButtonsForProgressiveMenu(int ix) {
    unsigned int wheel_buttons;

    wheel_buttons = 0;
    if (plat_lgwheels && plat_lgwheels->IsConnected(ix)) {
        wheel_buttons = reinterpret_cast<const LGPosition *>(plat_lgwheels)[ix].button;
    }
    return wheel_buttons;
}

unsigned int IsWheelActiveForProgressiveMenu(int ix) {
    unsigned int wheel_connected;

    wheel_connected = 0;
    if (plat_lgwheels) {
        wheel_connected = plat_lgwheels->IsConnected(ix);
    }
    return wheel_connected;
}

int ActualReadJoystickData() {
    int nNewTop;
    int port;

    if (!JoystickInitialized) {
        return 0;
    }

    nNewTop = (JoystickRingBufferTop + 1) & 0x1F;
    if (nNewTop == JoystickRingBufferBottom) {
        return 0;
    }

    PADRead(HardwarePadStatus);
    PADClamp(HardwarePadStatus);
    plat_lgwheels->ReadAll();

    for (port = 0; port <= 3; port++) {
        JoyData *joy_data = &PadRingData[port][JoystickRingBufferTop];
        PadData *pad_data = &joy_data->ThePadData[0];

        bMemSet(joy_data, 0xFF, sizeof(JoyData));

        if (HardwarePadStatus[port].err == PAD_ERR_NONE) {
            joy_data->padSTATUS = HardwarePadStatus[port];
            pad_data->Type = 0x41;
            pad_data->Error = 0;
            pad_data->DigitalButtons = ConvertPadButtons(joy_data->padSTATUS.button);
            pad_data->AnalogRightX = ClampAnalogValue(static_cast<int>(joy_data->padSTATUS.substickX * 2.15f) + 0x80);
            pad_data->AnalogRightY = ClampAnalogValue(0x80 - static_cast<int>(joy_data->padSTATUS.substickY * 2.15f));
            pad_data->AnalogLeftX = ClampAnalogValue(static_cast<int>(joy_data->padSTATUS.stickX * 1.75f) + 0x80);
            pad_data->AnalogLeftY = ClampAnalogValue(0x80 - static_cast<int>(joy_data->padSTATUS.stickY * 1.75f));
            pad_data->LTrigger = static_cast<unsigned char>(joy_data->padSTATUS.triggerL * 1.7f);
            pad_data->RTrigger = static_cast<unsigned char>(joy_data->padSTATUS.triggerR * 1.7f);
        } else if (plat_lgwheels->IsConnected(port)) {
            const LGPosition *wheel_position;
            int wheel_connected;
            int pedals_connected;

            wheel_connected = wasWheelConnected[port];
            if (!wheel_connected) {
                wasWheelConnected[port] = 1;
                calibrationTimer[port] = 7.0f;
                lastCalibTime[port] = RealTimer.GetSeconds();
            }

            if (calibrationTimer[port] < 5.0f && notYetCalibrating[port]) {
                AutoCalibrateWheel(port);
                notYetCalibrating[port] = 0;
            }

            if (calibrationTimer[port] > 0.0f) {
                float now = RealTimer.GetSeconds();
                float elapsed = now - lastCalibTime[port];
                lastCalibTime[port] = now;
                calibrationTimer[port] -= elapsed;
            }

            wheel_position = &reinterpret_cast<LGPosition *>(plat_lgwheels)[port];
            joy_data->padSTATUS.button = wheel_position->button;
            HardwarePadStatus[port].button = wheel_position->button;
            pad_data->Error = 0;
            pedals_connected = plat_lgwheels->PedalsConnected(port);
            pad_data->Type = pedals_connected ? 0x51 : 0x50;
            pad_data->DigitalButtons = ConvertPadButtons(wheel_position->button);
            pad_data->AnalogRightX = 0;
            pad_data->AnalogLeftX = wheel_position->wheel + 0x80;

            if (pedals_connected) {
                pad_data->AnalogRightY = wheel_position->accelerator;
                pad_data->AnalogLeftY = wheel_position->brake;
            } else {
                pad_data->AnalogLeftY = 0;
                pad_data->AnalogRightY = 0;
            }

            pad_data->LTrigger = wheel_position->triggerLeft;
            pad_data->RTrigger = wheel_position->triggerRight;
        } else {
            pad_data->Type = 0xFF;
            wasWheelConnected[port] = 0;
            notYetCalibrating[port] = 1;
            pad_data->Error = 1;
            PADReset(PADMASKS[port]);
            HardwarePadStatus[port].button = 0;
        }
    }

    JoystickRingBufferTop = nNewTop;
    return 1;
}
