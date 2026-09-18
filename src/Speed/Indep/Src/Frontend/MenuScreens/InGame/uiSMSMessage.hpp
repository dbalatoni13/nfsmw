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
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(u32 msg, FEObject* pobj, u32 param1, u32 param2) override;

    void Setup();
    void RefreshHeader();
};

#endif
