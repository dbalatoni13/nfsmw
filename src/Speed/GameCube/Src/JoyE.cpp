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
    unsigned short result;
    result = (buttons >> 8) & 1;
    result |= (buttons >> 8) & 2;
    result |= (buttons >> 8) & 4;
    result |= (buttons >> 8) & 8;
    result |= buttons & 0x10;
    result |= (buttons >> 7) & 0x20;
    result |= (buttons & 8) << 5;
    result |= (buttons & 4) << 7;
    result |= (buttons & 1) << 10;
    result |= (buttons & 2) << 10;
    return ~result;
}

static inline unsigned char ClampAnalogValue(int value) {
    if (value & 0x8000) {
        return 0;
    }
    if (static_cast<short>(value) > 0xFF) {
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
    if (JoystickInitialized) {
        int nNewTop = (JoystickRingBufferTop + 1) & 0x1F;

        if (nNewTop != JoystickRingBufferBottom) {
            int port;

            PADRead(HardwarePadStatus);
            PADClamp(HardwarePadStatus);
            plat_lgwheels->ReadAll();

            for (port = 0; port <= 3; port++) {
                JoyData *joy_data = &PadRingData[port][JoystickRingBufferTop];
                short data;

                bMemSet(joy_data, 0xFF, sizeof(JoyData));

                if (HardwarePadStatus[port].err == PAD_ERR_NONE) {
                    joy_data->padSTATUS = HardwarePadStatus[port];
                    joy_data->ThePadData[0].Type = 0x41;
                    joy_data->ThePadData[0].Error = 0;
                    data = joy_data->padSTATUS.button;
                    joy_data->ThePadData[0].DigitalButtons =
                        ~((data >> 8) & 1 | (data >> 8) & 2 | (data >> 8) & 4 | (data >> 8) & 8 | data & 0x10 |
                          (data >> 7) & 0x20 | (data & 8) << 5 | (data & 4) << 7 | (data & 1) << 10 |
                          (data & 2) << 10);
                    data = static_cast<int>(joy_data->padSTATUS.substickX * 2.15f) + 0x80;
                    if (data & 0x8000) {
                        data = 0;
                    }
                    if (data > 0xFF) {
                        data = 0xFF;
                    }
                    joy_data->ThePadData[0].AnalogRightX = data;
                    data = 0x80 - static_cast<int>(joy_data->padSTATUS.substickY * 2.15f);
                    if (data & 0x8000) {
                        data = 0;
                    }
                    if (data > 0xFF) {
                        data = 0xFF;
                    }
                    joy_data->ThePadData[0].AnalogRightY = data;
                    data = static_cast<int>(joy_data->padSTATUS.stickX * 1.75f) + 0x80;
                    if (data & 0x8000) {
                        data = 0;
                    }
                    if (data > 0xFF) {
                        data = 0xFF;
                    }
                    joy_data->ThePadData[0].AnalogLeftX = data;
                    joy_data->ThePadData[0].AnalogLeftY = 0x80 - static_cast<int>(joy_data->padSTATUS.stickY * 1.75f);
                    joy_data->ThePadData[0].LTrigger = static_cast<unsigned char>(joy_data->padSTATUS.triggerL * 1.7f);
                    joy_data->ThePadData[0].RTrigger = static_cast<unsigned char>(joy_data->padSTATUS.triggerR * 1.7f);
                } else if (plat_lgwheels->IsConnected(port)) {
                    if (!wasWheelConnected[port]) {
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

                    joy_data->padSTATUS.button = reinterpret_cast<LGPosition *>(plat_lgwheels)[port].button;
                    HardwarePadStatus[port].button = reinterpret_cast<LGPosition *>(plat_lgwheels)[port].button;
                    joy_data->ThePadData[0].Error = 0;
                    if (plat_lgwheels->PedalsConnected(port)) {
                        joy_data->ThePadData[0].Type = 0x51;
                    } else {
                        joy_data->ThePadData[0].Type = 0x50;
                    }
                    data = reinterpret_cast<LGPosition *>(plat_lgwheels)[port].button;
                    joy_data->ThePadData[0].DigitalButtons =
                        ~((data >> 8) & 1 | (data >> 8) & 2 | (data >> 8) & 4 | (data >> 8) & 8 | data & 0x10 |
                          (data >> 7) & 0x20 | (data & 8) << 5 | (data & 4) << 7 | (data & 1) << 10 |
                          (data & 2) << 10);
                    joy_data->ThePadData[0].AnalogRightX = 0;
                    joy_data->ThePadData[0].AnalogLeftX =
                        reinterpret_cast<LGPosition *>(plat_lgwheels)[port].wheel + 0x80;

                    if (plat_lgwheels->PedalsConnected(port)) {
                        joy_data->ThePadData[0].AnalogRightY =
                            reinterpret_cast<LGPosition *>(plat_lgwheels)[port].accelerator;
                        joy_data->ThePadData[0].AnalogLeftY = reinterpret_cast<LGPosition *>(plat_lgwheels)[port].brake;
                    } else {
                        joy_data->ThePadData[0].AnalogLeftY = 0;
                        joy_data->ThePadData[0].AnalogRightY = 0;
                    }

                    joy_data->ThePadData[0].LTrigger = reinterpret_cast<LGPosition *>(plat_lgwheels)[port].triggerLeft;
                    joy_data->ThePadData[0].RTrigger = reinterpret_cast<LGPosition *>(plat_lgwheels)[port].triggerRight;
                } else {
                    joy_data->ThePadData[0].Type = 0xFF;
                    wasWheelConnected[port] = 0;
                    notYetCalibrating[port] = 1;
                    joy_data->ThePadData[0].Error = 1;
                    PADReset(PADMASKS[port]);
                    HardwarePadStatus[port].button = 0;
                }
            }

            JoystickRingBufferTop = nNewTop;
            return 1;
        }
    }

    return 0;
}
