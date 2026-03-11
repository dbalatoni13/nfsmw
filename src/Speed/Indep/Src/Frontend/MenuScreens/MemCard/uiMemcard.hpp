#ifndef FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARD_H
#define FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "uiMemcardBase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

struct FEMemWidget : public ScrollerDatum {
    MemCardFileFlag m_Flag;
    int m_Size;
    UIMemcardList *m_pParent;

    FEMemWidget() {}
    ~FEMemWidget() override {}

    void Act(const char *parent_pkg, unsigned int data);
    bool IsCorrupt();
    int GetSize() const { return m_Size; }
    const char *GetFileName();

    static const int MAX_SIZE;
};

struct UIMemcardList : public MenuScreen {
    enum ListOp {
        MCLO_Load = 0,
        MCLO_Delete = 1,
    };

    Scrollerina m_SaveGameList;
    int m_Initialized;
    int m_ListOp;
    unsigned int m_LastMsg;
    FEMemWidget *m_pCreateNew;

    UIMemcardList(ScreenConstructorData *sd);
    ~UIMemcardList() override;

    int GetSize() { return m_SaveGameList.GetNumData(); }
    bool IsReady() { return m_Initialized != 0; }
    ListOp GetListOp() { return static_cast<ListOp>(m_ListOp); }

    const char *GetFileName(int find);
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1,
                             unsigned long param2) override;
    FEMemWidget *AddItem(const char *pName, const char *pDate, int size, int flag);
};

struct UIMemcardBoot : public UIMemcardBase {
    UIMemcardBoot(ScreenConstructorData *sd) : UIMemcardBase(sd) {}
    ~UIMemcardBoot() override {}

    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1,
                             unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg,
                                           eMenuSoundTriggers maybe) override;
};

struct UIMemcardMain : public UIMemcardBase {
    ~UIMemcardMain() override {}

    void SetPopupWindow(UIMemcardList *pChild) { m_pChild = pChild; }

    UIMemcardMain(ScreenConstructorData *sd);
    void DoSelect(const char *pName) override;
    void ListDone();
    void NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1,
                             unsigned long param2) override;
};

MenuScreen *CreateMemCardBootScreen(ScreenConstructorData *sd);
MenuScreen *CreateMemcardMainMenu(ScreenConstructorData *sd);
MenuScreen *CreateMemcardListFiles(ScreenConstructorData *sd);

#endif
