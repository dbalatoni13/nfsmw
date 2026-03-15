#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETCTS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETCTS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

struct RapSheetCTSArraySlot : public ArraySlot {
    FEString* pTimes; // offset 0x14, size 0x4
    FEString* pItem; // offset 0x18, size 0x4
    FEString* pValue; // offset 0x1C, size 0x4

    RapSheetCTSArraySlot(FEString* times, FEString* item, FEString* value)
        : ArraySlot(times) //
        , pTimes(times) //
        , pItem(item) //
        , pValue(value)
    {}
    ~RapSheetCTSArraySlot() override {}
    void Update(ArrayDatum* datum, bool isSelected) override;
};

struct RapSheetCTSDatum : public ArrayDatum {
    int times; // offset 0x24, size 0x4
    unsigned long itemHash; // offset 0x28, size 0x4
    int value; // offset 0x2C, size 0x4

    RapSheetCTSDatum(int num_times, unsigned long item_name, int total_value)
        : ArrayDatum(0, 0) //
        , times(num_times) //
        , itemHash(item_name) //
        , value(total_value)
    {}
    ~RapSheetCTSDatum() override {}
    void NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) override;

    int getNumTimes() { return times; }
    unsigned long getItemHash() { return itemHash; }
    int getTotalValue() { return value; }
};

struct uiRapSheetCTS : public ArrayScrollerMenu {
    uiRapSheetCTS(ScreenConstructorData* sd);
    ~uiRapSheetCTS() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup();
    void RefreshHeader() override;
};

#endif
