#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UISHOWCASE_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE_UISHOWCASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"
#include <types.h>

struct FECarRecord;
struct FEImage;

// total size: 0x70
struct Showcase : public MenuScreen {
    Showcase(ScreenConstructorData *sd);
    ~Showcase() override;

    void NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) override;

    static const char *FromPackage;
    static unsigned int FromArgs;
    static unsigned int FromIndex;
    static unsigned int BlackListNumber;
    static int FromFilter;
    static void *FromColor[3];

    FECarRecord *car;                      // offset 0x2C, size 0x4
    FEImage *pTagImg;                      // offset 0x30, size 0x4
    uiRepSheetRivalStreamer RivalStreamer; // offset 0x34, size 0x3C
};

#endif
