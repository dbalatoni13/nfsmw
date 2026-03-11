#ifndef FRONTEND_MENUSCREENS_INGAME_UISMSMESSAGE_H
#define FRONTEND_MENUSCREENS_INGAME_UISMSMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/CTextScroller.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

struct FEObject;
struct SMSMessage;

// total size: 0xE8
struct uiSMSMessage : public MenuScreen {
    CTextScroller m_TextScroller; // offset 0x2C, size 0x54
    FEScrollBar ScrollBar;        // offset 0x80, size 0x64
    SMSMessage* the_msg;          // offset 0xE4, size 0x4

    uiSMSMessage(ScreenConstructorData* sd);
    ~uiSMSMessage() override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;

    void Setup();
    void RefreshHeader();
};

#endif
