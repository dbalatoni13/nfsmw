#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

#include <stdarg.h>

bool IsJoystickTypeWheel(JoystickPort port);

unsigned int Button_Action_Hashes_GAMECUBE[17][5] = {
    {0x6AB80AFA, 0x56AFE190, 0x6AB80AFA, 0xB70FF4A4, 0x6AB80AFA},
    {0x63AA639D, 0x63AA639D, 0x63AA639D, 0x63AA639D, 0x63AA639D},
    {0x619B23B0, 0xDC1CD97D, 0x619B23B0, 0x5E5C5F2E, 0x619B23B0},
    {0xB70FF4A4, 0x13121940, 0x5E5C5F2E, 0x619B23B0, 0x5E5C5F2E},
    {0x5E5C5F2E, 0x5E5C5F2E, 0xB70FF4A4, 0x6AB80AFA, 0xB70FF4A4},
    {0x018AE810, 0x619B23B0, 0xDC1CD97D, 0xDC1CD97D, 0x018AE810},
    {0x13121940, 0x6AB80AFA, 0x56AFE190, 0x56AFE190, 0x13121940},
    {0xAB1032FA, 0xAB1032FA, 0xAB1032FA, 0xAB1032FA, 0xAB1032FA},
    {0xFF0E7232, 0xFF0E7232, 0xFF0E7232, 0xFF0E7232, 0xFF0E7232},
    {0xB864880D, 0xB864880D, 0xB864880D, 0xB864880D, 0xB864880D},
    {0xDC03C093, 0xDC03C093, 0xDC03C093, 0xDC03C093, 0xDC03C093},
    {0x56AFE190, 0x018AE810, 0x13121940, 0x018AE810, 0x56AFE190},
    {0xDC1CD97D, 0x1CEDC132, 0x1CEDC132, 0x1CEDC132, 0xDC1CD97D},
    {0x1CEDC132, 0xB70FF4A4, 0x018AE810, 0x13121940, 0x1CEDC132},
    {0xB70FF4A4, 0x13121940, 0x5E5C5F2E, 0x619B23B0, 0x5E5C5F2E},
    {0x5E5C5F2E, 0x5E5C5F2E, 0xB70FF4A4, 0x6AB80AFA, 0xB70FF4A4},
    {0x6369F4B8, 0x6369F4B8, 0x6369F4B8, 0x6369F4B8, 0x6369F4B8},
};

unsigned int Button_Action_Hashes_GAMECUBE_Wheel[17][5] = {
    {0x63AA639D, 0x63AA639D, 0x63AA639D, 0x63AA639D, 0x63AA639D},
    {0x6AB80AFA, 0x6AB80AFA, 0x6AB80AFA, 0x6AB80AFA, 0x6AB80AFA},
    {0x56AFE190, 0x1CEDC132, 0x1CEDC132, 0x1CEDC132, 0xDC1CD97D},
    {0xDC1CD97D, 0x018AE810, 0x13121940, 0x018AE810, 0x56AFE190},
    {0x018AE810, 0x13121940, 0x5E5C5F2E, 0x619B23B0, 0x5E5C5F2E},
    {0x00000000, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0x619B23B0, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0xAB1032FA, 0xAB1032FA, 0xAB1032FA, 0xAB1032FA, 0xAB1032FA},
    {0xFF0E7232, 0xFF0E7232, 0xFF0E7232, 0xFF0E7232, 0xFF0E7232},
    {0xB864880D, 0xB864880D, 0xB864880D, 0xB864880D, 0xB864880D},
    {0xDC03C093, 0xDC03C093, 0xDC03C093, 0xDC03C093, 0xDC03C093},
    {0xDC1CD97D, 0x018AE810, 0x13121940, 0x018AE810, 0x56AFE190},
    {0x56AFE190, 0x1CEDC132, 0x1CEDC132, 0x1CEDC132, 0xDC1CD97D},
    {0x1CEDC132, 0xB70FF4A4, 0x018AE810, 0x13121940, 0x1CEDC132},
    {0x018AE810, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0x5E5C5F2E, 0x00000000, 0x00000000, 0x00000000, 0x00000000},
    {0x6369F4B8, 0x6369F4B8, 0x6369F4B8, 0x6369F4B8, 0x6369F4B8},
};

uint32 FindButtonNameHashForFEString(int config, int string_number, JoystickPort player) {
    if (IsJoystickTypeWheel(player)) {
        return Button_Action_Hashes_GAMECUBE_Wheel[string_number][config];
    }
    return Button_Action_Hashes_GAMECUBE[string_number][config];
}

int FEngMapJoyParamToJoyport(int feng_param) {
    if (feng_param & 1) {
        return 0;
    }
    if (feng_param & 2) {
        return 1;
    }
    if (feng_param & 4) {
        return 2;
    }
    if (feng_param & 8) {
        return 3;
    }
    return -1;
}

int FEngMapJoyportToJoyParam(int joyport) {
    if (joyport == 0)
        return 1;
    if (joyport == 1)
        return 2;
    if (joyport == 2)
        return 4;
    if (joyport == 3)
        return 8;
    return 0;
}

void FEngSNMakeHidden(char *outBuffer, int32 out_buf_size, const char *strInput) {
    int len = bStrLen(strInput);
    int i = 0;
    if (i < len) {
        int max = out_buf_size - 1;
        if (i != max) {
            do {
                outBuffer[i] = '*';
                i++;
                if (i >= len) {
                    break;
                }
            } while (i != max);
        }
    }
    outBuffer[i] = 0;
}

void FEngSNMakeHidden(char *outBuffer, int32 out_buf_size, uint16 *strInput) {
    int len = bStrLen(strInput);
    int i = 0;
    if (i < len) {
        int max = out_buf_size - 1;
        if (i != max) {
            do {
                outBuffer[i] = '*';
                i++;
                if (i >= len) {
                    break;
                }
            } while (i != max);
        }
    }
    outBuffer[i] = 0;
}

void FEngTickSinglePackage(const char *pkg_name, unsigned int ticks) {
    FEPackage *single_package = cFEng::Get()->FindPackage(pkg_name);
    if (single_package) {
        FEObject *pObject = single_package->GetFirstObject();
        single_package->SetTickIncrement(ticks);
        while (pObject) {
            single_package->UpdateObject(pObject, ticks);
            pObject = pObject->GetNext();
        }
    }
}

uint32 FEngHashString(const char *fmt, ...) {
    va_list argList;
    va_start(argList, fmt);

    char print_buffer[256];
    bVSPrintf(print_buffer, fmt, argList);
    va_end(argList);
    return bStringHash(print_buffer);
}
