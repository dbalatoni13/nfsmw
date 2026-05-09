#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETUS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

class RapSheetUSDatum : public ArrayDatum {
  public:
    RapSheetUSDatum(uint32 item_name, uint32 unserved_infractions, uint32 total_infractions)
        : ArrayDatum(0, 0), itemName(item_name), unserved(unserved_infractions), total(total_infractions) {}
    ~RapSheetUSDatum() override {}
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override {};

    uint32 getItemName() {
        return itemName;
    }
    uint32 getNumUnserved() {
        return unserved;
    }
    uint32 getTotalUnserved() {
        return total;
    }

  private:
    uint32 itemName; // offset 0x24, size 0x4
    uint32 unserved; // offset 0x28, size 0x4
    uint32 total;    // offset 0x2C, size 0x4
};

class RapSheetUSArraySlot : public ArraySlot {
  public:
    RapSheetUSArraySlot(FEObject *obj, FEString *item_name, FEString *unserved_infractions, FEString *total_infractions)
        : ArraySlot(obj), pItemName(item_name), pUnserved(unserved_infractions), pTotal(total_infractions) {}
    ~RapSheetUSArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

  private:
    FEString *pItemName; // offset 0x14, size 0x4
    FEString *pUnserved; // offset 0x18, size 0x4
    FEString *pTotal;    // offset 0x1C, size 0x4
};

class uiRapSheetUS : public ArrayScrollerMenu {
  public:
    uiRapSheetUS(ScreenConstructorData *sd);
    ~uiRapSheetUS() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader() override;
    void ToggleView();

  private:
    void Setup();

    bool view_unserved; // offset 0xE8, size 0x1
};

#endif
