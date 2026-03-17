#include "Logitech/LGWheels.hpp"

#include <dolphin/pad.h>

unsigned char notYetCalibrating[4];
unsigned char wasWheelConnected[4];
PADStatus HardwarePadStatus[4];
int JoystickInitialized;

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
