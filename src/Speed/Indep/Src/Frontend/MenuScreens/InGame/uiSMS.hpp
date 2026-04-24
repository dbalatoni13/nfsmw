#ifndef FRONTEND_MENUSCREENS_INGAME_UISMS_H
#define FRONTEND_MENUSCREENS_INGAME_UISMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

struct SMSMessage;
struct FEGroup;
struct FEImage;
struct FEString;
enum eScrollDir;

struct SMSSlot : public ArraySlot {
    FEImage* pIcon; // offset 0x14
    FEString* pText; // offset 0x18

    SMSSlot(FEGroup* grp, FEImage* icon, FEString* text)
        : ArraySlot(reinterpret_cast<FEObject*>(grp)) //
        , pIcon(icon) //
        , pText(text)
    {}
    ~SMSSlot() override {}
    void Update(ArrayDatum* datum, bool isSelected) override;
};

// total size: 0x100
struct uiSMS : public ArrayScrollerMenu {
    unsigned char last_msg[2];     // offset 0xE8, size 0x2
    int button_pressed;            // offset 0xEC, size 0x4
    bool bVoiceMsg;                // offset 0xF0, size 0x1
    bool bAutoPlay;                // offset 0xF4, size 0x1
    bool bWaitingForMemcard;       // offset 0xF8, size 0x1
    bool bInitCompleted;           // offset 0xFC, size 0x1

    uiSMS(ScreenConstructorData* sd);
    ~uiSMS() override {}

    void Setup();
    void AddSMSDatum(SMSMessage* msg);
    void AddSMSSlot(unsigned int index);
    void RefreshHeader() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1,
                             unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void ScrollBoxes(eScrollDir dir);
};

#endif
