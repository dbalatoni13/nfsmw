#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETUS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETUS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

struct RapSheetUSArraySlot : public ArraySlot {
    FEString* pItemName; // offset 0x14, size 0x4
    FEString* pUnserved; // offset 0x18, size 0x4
    FEString* pTotal; // offset 0x1C, size 0x4

    RapSheetUSArraySlot(FEObject* obj, FEString* item_name, FEString* unserved_infractions, FEString* total_infractions)
        : ArraySlot(obj) //
        , pItemName(item_name) //
        , pUnserved(unserved_infractions) //
        , pTotal(total_infractions)
    {}
    ~RapSheetUSArraySlot() override {}
    void Update(ArrayDatum* datum, bool isSelected) override;
};

struct RapSheetUSDatum : public ArrayDatum {
    unsigned int itemName; // offset 0x24, size 0x4
    unsigned int unserved; // offset 0x28, size 0x4
    unsigned int total; // offset 0x2C, size 0x4

    RapSheetUSDatum(unsigned int item_name, unsigned int unserved_infractions, unsigned int total_infractions)
        : ArrayDatum(0, 0) //
        , itemName(item_name) //
        , unserved(unserved_infractions) //
        , total(total_infractions)
    {}
    ~RapSheetUSDatum() override {}
    void NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) override;

    unsigned int getItemName() { return itemName; }
    unsigned int getNumUnserved() { return unserved; }
    unsigned int getTotalUnserved() { return total; }
};

struct uiRapSheetUS : public ArrayScrollerMenu {
    bool view_unserved; // offset 0xE8, size 0x1

    uiRapSheetUS(ScreenConstructorData* sd);
    ~uiRapSheetUS() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup();
    void RefreshHeader() override;
    void ToggleView();
};

#endif
