#ifndef FRONTEND_MENUSCREENS_INGAME_UISMS_H
#define FRONTEND_MENUSCREENS_INGAME_UISMS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/FEGroup.h"

// total size : 0xC
class SMSSortNode : public bTNode<SMSSortNode> {
  public:
    SMSSortNode(struct SMSMessage *msg) {}
    ~SMSSortNode() {}

    SMSMessage *the_msg; // offset 0x8, size 0x4
};

// total size: 0x28
class SMSDatum : public ArrayDatum {
  public:
    SMSDatum(SMSMessage *msg) : ArrayDatum(0, 0), my_msg(msg) {}
    ~SMSDatum() override {}
    void NotificationMessage(u32 msg, struct FEObject *pObj, u32 param1, u32 param2) override;

    SMSMessage *my_msg; // offset 0x24, size 0x4
};

// total size: 0x1C
class SMSSlot : public ArraySlot {
  public:
    SMSSlot(FEGroup *grp, FEImage *icon, FEString *text) : ArraySlot(grp), icon(icon), text(text) {}
    ~SMSSlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

  private:
    FEImage *icon;  // offset 0x14, size 0x4
    FEString *text; // offset 0x18, size 0x4
};

typedef enum { SMS_VOICE = 0, SMS_TEXT = 1, SMS_NUMTYPES = 2 } SMS_TYPE;

// total size: 0x100
class uiSMS : public ArrayScrollerMenu {
  public:
    uiSMS(ScreenConstructorData *sd);
    inline ~uiSMS() override {}
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void Setup();
    void AddSMSDatum(SMSMessage *msg);
    void AddSMSSlot(uint32 index);
    void RefreshHeader() override;
    void ScrollBoxes(eScrollDir dir);

    // Members
    uint8 last_msg[2];       // offset 0xE8, size 0x2
    int button_pressed;      // offset 0xEC, size 0x4
    bool bVoiceMsg;          // offset 0xF0, size 0x1
    bool bAutoPlay;          // offset 0xF4, size 0x1
    bool bWaitingForMemcard; // offset 0xF8, size 0x1
    bool bInitCompleted;     // offset 0xFC, size 0x1
};

#endif
