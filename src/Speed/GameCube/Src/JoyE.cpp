#include "Speed/GameCube/Src/Logitech/LGWheels.h"

#include "dolphin.h"

// Estado crudo de los cuatro puertos de mando; lo rellenan PADRead y los
// volantes Logitech.
PADStatus HardwarePadStatus[4];

// Ring buffer de instantaneas de joystick (32 entradas de 0x3C bytes por
// puerto).
unsigned char PadRingData[0x1E00];

LGWheels *plat_lgwheels = 0;

int JoystickRingBufferTop = 0;
int JoystickRingBufferBottom = 0;

int JoystickInitialized = 0;

float calibrationTimer[4];
float lastCalibTime[4];
char notYetCalibrating[4];
char wasWheelConnected[4];

void AutoCalibrateWheel(int channel) {
    plat_lgwheels->StopConstantForce(channel);
    plat_lgwheels->StopSurfaceEffect(channel);
    plat_lgwheels->StopDamperForce(channel);
    plat_lgwheels->StopCarAirborne(channel);
    plat_lgwheels->StopSlipperyRoadEffect(channel);
    plat_lgwheels->StopSpringForce(channel);
    plat_lgwheels->PlayAutoCalibAndSpringForce(channel);
}

// Instantanea de 0x3C bytes por muestreo: botones activos a nivel bajo, ejes
// transformados a 0..255 y una copia cruda de PADStatus al final.
//
// r47: los NOMBRES Y LA FORMA son los del volcado DWARF del original
// (`symbols/mw_dwarfdump.nothpp`, linea 1851117): la instantanea es
// `JoyData { PadData ThePadData[4]; u8 Bytes[4]; int RegularControllerType;
// PADStatus padSTATUS; }` y todos los campos de eje se escriben a traves de
// `ThePadData[slot]`, no como miembros sueltos en offsets fijos. Escribirlo asi
// arregla los TRES `add`/`sthx` con los operandos al reves de la rama del
// volante: la indexacion de un array MIEMBRO expande la direccion como
// `(plus base offset)` (base primero, la forma del objetivo) mientras que
// `((LGPosition *)joy_data + slot)` la expande al reves. Medido: 19 filas
// distintas -> 16, fuzzy 99,24433 % -> 99,3199 %. La forma `[slot]` sobre el
// puntero casteado (`((LGPosition *)(void *)joy_data)[slot].x`) NO vale: da las
// mismas 19 filas; hace falta que el array sea un MIEMBRO.
struct PadData {
    unsigned short DigitalButtons; // offset 0x0
    unsigned char LTrigger;        // offset 0x2
    unsigned char RTrigger;        // offset 0x3
    unsigned char AnalogLeftX;     // offset 0x4
    unsigned char AnalogLeftY;     // offset 0x5
    unsigned char AnalogRightX;    // offset 0x6
    unsigned char AnalogRightY;    // offset 0x7
    unsigned char Error;           // offset 0x8
    unsigned char Type;            // offset 0x9
};

struct JoyData {
    PadData ThePadData[4];         // offset 0x0,  size 0x28
    unsigned char Bytes[4];        // offset 0x28
    int RegularControllerType;     // offset 0x2C
    PADStatus padSTATUS;           // offset 0x30
};

// Mascara de botones que PADReset admite por puerto (A, B, X, Y como bits 31 a
// 28). Va a .rodata, igual que en el original.
extern const unsigned int PADMASKS[4];
const unsigned int PADMASKS[4] = {0x80000000, 0x40000000, 0x20000000, 0x10000000};

int ActualReadJoystickData() {
    if (JoystickInitialized != 0) {
        int nNewTop = (JoystickRingBufferTop + 1) & 0x1F;
        if (nNewTop != JoystickRingBufferBottom) {
            int port;

            PADRead(HardwarePadStatus);
            PADClamp(HardwarePadStatus);
            plat_lgwheels->ReadAll();

            for (port = 0; port <= 3; port++) {
                {
                    JoyData *joy_data;
                    int pad_state;
                    int slot;

                    slot = 0;
                    joy_data = (JoyData *)(PadRingData + port * 0x780 + JoystickRingBufferTop * 0x3C);
                    bMemSet(joy_data, 0xFF, sizeof(JoyData));

                    pad_state = HardwarePadStatus[port].err;
                    if (pad_state == 0) {
                        {
                            int v;

                            joy_data->padSTATUS = HardwarePadStatus[port];
                            joy_data->ThePadData[slot].Type = 0x41;
                            joy_data->ThePadData[slot].Error = 0;
                            joy_data->ThePadData[slot].DigitalButtons = ~(((joy_data->padSTATUS.button & 0x100) >> 8) | ((joy_data->padSTATUS.button & 0x200) >> 8) |
                                                  ((joy_data->padSTATUS.button & 0x400) >> 8) | ((joy_data->padSTATUS.button & 0x800) >> 8) |
                                                  (joy_data->padSTATUS.button & 0x10) | ((joy_data->padSTATUS.button & 0x1000) >> 7) |
                                                  ((joy_data->padSTATUS.button & 8) << 5) | ((joy_data->padSTATUS.button & 4) << 7) |
                                                  ((joy_data->padSTATUS.button & 1) << 10) | ((joy_data->padSTATUS.button & 2) << 10));
                            v = (int)((float)joy_data->padSTATUS.substickX * 2.15f) + 0x80;
                            if (v & 0x8000) {
                                v = 0;
                            }
                            {
                            int data = v;
                            if ((short)data > 255) {
                                data = 255;
                            }
                            joy_data->ThePadData[slot].AnalogRightX = data;
                            }
                            v = 0x80 - (int)((float)joy_data->padSTATUS.substickY * 2.15f);
                            if (v & 0x8000) {
                                v = 0;
                            }
                            {
                            int data = v;
                            if ((short)data > 255) {
                                data = 255;
                            }
                            joy_data->ThePadData[slot].AnalogRightY = data;
                            }
                            v = (int)((float)joy_data->padSTATUS.stickX * 1.75f) + 0x80;
                            if (v & 0x8000) {
                                v = 0;
                            }
                            {
                            int data = v;
                            if ((short)data > 255) {
                                data = 255;
                            }
                            joy_data->ThePadData[slot].AnalogLeftX = data;
                            }
                            {
                            int data = 0x80 - (int)((float)joy_data->padSTATUS.stickY * 1.75f);
                            joy_data->ThePadData[slot].AnalogLeftY = data;
                            }
                            joy_data->ThePadData[slot].LTrigger = (unsigned char)((float)joy_data->padSTATUS.triggerLeft * 1.7f);
                            joy_data->ThePadData[slot].RTrigger = (unsigned char)((float)joy_data->padSTATUS.triggerRight * 1.7f);
                        }
                        continue;
                    }

                    if (plat_lgwheels->IsConnected(port) != 0) {
                        if (wasWheelConnected[port] == 0) {
                            wasWheelConnected[port] = 1;
                            calibrationTimer[port] = 7.0f;
                            lastCalibTime[port] = RealTimer.GetSeconds();
                        }

                        if (calibrationTimer[port] < 5.0f && notYetCalibrating[port] != 0) {
                            AutoCalibrateWheel(port);
                            notYetCalibrating[port] = 0;
                        }

                        if (calibrationTimer[port] > 0.0f) {
                            calibrationTimer[port] = calibrationTimer[port] - (RealTimer.GetSeconds() - lastCalibTime[port]);
                            lastCalibTime[port] = RealTimer.GetSeconds();
                        }

                        joy_data->padSTATUS.button = plat_lgwheels->Position[port].button;
                        HardwarePadStatus[port].button = plat_lgwheels->Position[port].button;
                        joy_data->ThePadData[slot].Error = 0;
                        if (plat_lgwheels->PedalsConnected(port) != 0) {
                            joy_data->ThePadData[slot].Type = 0x51;
                        } else {
                            joy_data->ThePadData[slot].Type = 0x50;
                        }
                        joy_data->ThePadData[slot].DigitalButtons =
                            ~(((joy_data->padSTATUS.button & 0x100) >> 8) | ((joy_data->padSTATUS.button & 0x200) >> 8) |
                              ((joy_data->padSTATUS.button & 0x400) >> 8) | ((joy_data->padSTATUS.button & 0x800) >> 8) |
                              (joy_data->padSTATUS.button & 0x10) | ((joy_data->padSTATUS.button & 0x1000) >> 7) |
                              ((joy_data->padSTATUS.button & 8) << 5) | ((joy_data->padSTATUS.button & 4) << 7) |
                              ((joy_data->padSTATUS.button & 1) << 10) | ((joy_data->padSTATUS.button & 2) << 10));
                        joy_data->ThePadData[slot].AnalogLeftX = (unsigned char)plat_lgwheels->Position[port].wheel + 0x80;
                        joy_data->ThePadData[slot].AnalogRightX = 0;
                        if (plat_lgwheels->PedalsConnected(port) != 0) {
                            joy_data->ThePadData[slot].AnalogRightY = plat_lgwheels->Position[port].accelerator;
                            joy_data->ThePadData[slot].AnalogLeftY = plat_lgwheels->Position[port].brake;
                        } else {
                            joy_data->ThePadData[slot].AnalogRightY = 0;
                            joy_data->ThePadData[slot].AnalogLeftY = 0;
                        }
                        joy_data->ThePadData[slot].LTrigger = plat_lgwheels->Position[port].triggerLeft;
                        joy_data->ThePadData[slot].RTrigger = plat_lgwheels->Position[port].triggerRight;
                    } else {
                        wasWheelConnected[port] = 0;
                        notYetCalibrating[port] = 1;
                        joy_data->ThePadData[slot].Error = 1;
                        joy_data->ThePadData[slot].Type = 0xFF;
                        PADReset(PADMASKS[port]);
                        HardwarePadStatus[port].button = 0;
                    }
                }
            }

            JoystickRingBufferTop = nNewTop;

            return 1;
        }
    }

    return 0;
}
void PlatformInitJoystick() {
    plat_lgwheels = new ("plat_lgwheels", 0) LGWheels;

    for (int channel = 0; channel < 4; channel++) {
        notYetCalibrating[channel] = 1;
        wasWheelConnected[channel] = 0;
    }

    PADRead(HardwarePadStatus);
    JoystickInitialized = 1;
}

void ReadLGWheelDataForProgressiveMenu() {
    if (plat_lgwheels != 0) {
        plat_lgwheels->ReadAll();
    }
}

unsigned short ReadLGWheelButtonsForProgressiveMenu(int channel) {
    unsigned short buttons = 0;

    if (plat_lgwheels != 0) {
        if (plat_lgwheels->IsConnected(channel)) {
            buttons = plat_lgwheels->Position[channel].button;
        }
    }

    return buttons;
}

int IsWheelActiveForProgressiveMenu(int channel) {
    int active = 0;

    if (plat_lgwheels != 0) {
        active = plat_lgwheels->IsConnected(channel);
    }

    return active;
}
