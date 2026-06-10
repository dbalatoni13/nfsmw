#ifndef __UIMEMCARD_HPP__
#define __UIMEMCARD_HPP__

#include "uiMemcardBase.hpp"

class UIMemcardBoot : public UIMemcardBase {
  public:
    UIMemcardBoot(ScreenConstructorData *sd) : UIMemcardBase(sd) {}
    ~UIMemcardBoot() override {}

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
};

class UIMemcardMain : public UIMemcardBase {
  public:
    UIMemcardMain(ScreenConstructorData *sd);
    ~UIMemcardMain() override {}
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    void DoSelect(const char *pName) override;

    void SetPopupWindow(UIMemcardList *pChild) {
        m_pChild = pChild;
    }

    void ListDone();
};

MenuScreen *CreateMemCardBootScreen(ScreenConstructorData *sd);
MenuScreen *CreateMemcardMainMenu(ScreenConstructorData *sd);
MenuScreen *CreateMemcardListFiles(ScreenConstructorData *sd);

typedef void MemCardOpType(void *);

#endif
