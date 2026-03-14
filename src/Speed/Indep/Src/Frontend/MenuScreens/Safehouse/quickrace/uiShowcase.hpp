#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UISHOWCASE_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UISHOWCASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"

#include <types.h>

// total size: 0x70
struct Showcase : public MenuScreen {
    Showcase(ScreenConstructorData *sd);
    ~Showcase() override;

    void NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) override;

    static const char *FromPackage;      // address
    static unsigned int FromArgs;        // address
    static unsigned int FromIndex;       // address
    static unsigned int BlackListNumber; // address
    static int FromFilter;               // address
    static void *FromColor[3];           // address

    FECarRecord *car;                              // offset 0x2C, size 0x4
    FEImage *pTagImg;                              // offset 0x30, size 0x4
    uiRepSheetRivalStreamer RivalStreamer;           // offset 0x34, size 0x3C
};

// total size: 0x34
struct uiQRPressStart : public MenuScreen {
    uiQRPressStart(ScreenConstructorData *sd);
    ~uiQRPressStart() override;

    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) override;

    void Setup();

    int iPlayerNum; // offset 0x2C, size 0x4
    int param;      // offset 0x30, size 0x4
};

#endif
