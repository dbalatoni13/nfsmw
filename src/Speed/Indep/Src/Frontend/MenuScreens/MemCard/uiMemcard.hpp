#ifndef FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARD_H
#define FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardBase.hpp"

class UIMemcardList;

// total size: 0x24
class FEMemWidget : public ScrollerDatum {
  public:
    bool IsCorrupt();

    // Sin destructor propio: el sintetizado repone la vtable de ScrollerDatum
    // y no emite el store de la suya, que es lo que hace el original.
    FEMemWidget(const char *pName, const char *pDate, int size, int flag) {
        bEnabled = true;
        m_Flag = static_cast<MemCardFileFlag>(flag);
        m_Size = size;

        AddData(pName, 0);
        AddData(pDate, 0);
    }

    void Act(const char *parent_pkg, unsigned int data);

    MemCardFileFlag m_Flag;   // offset 0x18
    int m_Size;               // offset 0x1C
    UIMemcardList *m_pParent; // offset 0x20
};

// total size: 0x104
class UIMemcardList : public MenuScreen {
  public:
    enum ListOp {
        MCLO_Load = 0,
        MCLO_Delete = 1,
    };

    UIMemcardList(ScreenConstructorData *sd);
    ~UIMemcardList() override;

    const char *GetFileName(int find);

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    FEMemWidget *AddItem(const char *pName, const char *pDate, int size, int flag);

    int GetSize() {
        return m_SaveGameList.GetNumData();
    }

    bool IsReady() {
        return m_Initialized;
    }

    ListOp GetListOp() {
        return static_cast<ListOp>(m_ListOp);
    }

  protected:
    Scrollerina m_SaveGameList;   // offset 0x2C, size 0xC8
    int m_Initialized;            // offset 0xF4
    int m_ListOp;                 // offset 0xF8
    unsigned int m_LastMsg;       // offset 0xFC
    FEMemWidget *m_pCreateNew;    // offset 0x100
};

// total size: 0x94
class UIMemcardBoot : public UIMemcardBase {
  public:
    UIMemcardBoot(ScreenConstructorData *sd) : UIMemcardBase(sd) {
        FEngFindString(GetPackageName(), 0x1E2640FA)->Flags &= ~0x200;
    }

    ~UIMemcardBoot() override {}

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
};

// total size: 0x94
class UIMemcardMain : public UIMemcardBase {
  public:
    UIMemcardMain(ScreenConstructorData *sd);

    ~UIMemcardMain() override {}

    void SetPopupWindow(UIMemcardList *pChild) {
        m_pChild = pChild;
    }

    void DoSelect(const char *pName) override;

    void ListDone();

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
};

#endif
