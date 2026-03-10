#ifndef FRONTEND_MENUSCREENS_INGAME_UISMSMESSAGE_H
#define FRONTEND_MENUSCREENS_INGAME_UISMSMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct FEObject;
struct CTextScroller;

struct uiSMSMessage : public MenuScreen {
    CTextScroller* pScrollBar;

    uiSMSMessage(ScreenConstructorData* sd);
    ~uiSMSMessage() override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;

    void Setup();
    void RefreshHeader();
};

#endif
