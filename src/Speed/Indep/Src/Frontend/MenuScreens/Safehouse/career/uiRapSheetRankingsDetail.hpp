#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRANKINGSDETAIL_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRANKINGSDETAIL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"

struct RapSheetRankingsArraySlot : public ArraySlot {
    FEString* pItemNum; // offset 0x14, size 0x4
    FEString* pPlayerName; // offset 0x18, size 0x4
    FEString* pCarName; // offset 0x1C, size 0x4
    FEString* pValue; // offset 0x20, size 0x4

    RapSheetRankingsArraySlot(FEString* item_num, FEString* player_name, FEString* car_name, FEString* value)
        : ArraySlot(item_num) //
        , pItemNum(item_num) //
        , pPlayerName(player_name) //
        , pCarName(car_name) //
        , pValue(value)
    {}
    ~RapSheetRankingsArraySlot() override {}
    void Update(ArrayDatum* datum, bool isSelected) override;
};

struct RapSheetRankingsTimerArraySlot : public RapSheetRankingsArraySlot {
    RapSheetRankingsTimerArraySlot(FEString* item_num, FEString* player_name, FEString* car_name, FEString* value)
        : RapSheetRankingsArraySlot(item_num, player_name, car_name, value)
    {}
    ~RapSheetRankingsTimerArraySlot() override {}
    void Update(ArrayDatum* datum, bool isSelected) override;
};

struct RapSheetRankingsDatum : public ArrayDatum {
    unsigned int itemNum; // offset 0x24, size 0x4
    unsigned int nameHash; // offset 0x28, size 0x4
    unsigned int carHash; // offset 0x2C, size 0x4
    float value; // offset 0x30, size 0x4

    RapSheetRankingsDatum(unsigned int item_num, unsigned int player_hash, unsigned int car_hash, float val)
        : ArrayDatum(0, 0) //
        , itemNum(item_num) //
        , nameHash(player_hash) //
        , carHash(car_hash) //
        , value(val)
    {}
    ~RapSheetRankingsDatum() override {}
    void NotificationMessage(unsigned long msg, FEObject* pObj, unsigned long param1, unsigned long param2) override;

    unsigned int getItemNum() { return itemNum; }
    unsigned int getPlayerName() { return nameHash; }
    unsigned int getCarName() { return carHash; }
    float getValue() { return value; }
};

struct uiRapSheetRankingsDetail : public ArrayScrollerMenu {
    ePursuitDetailTypes rank_type; // offset 0xE8, size 0x4
    int player_rank; // offset 0xEC, size 0x4

    static bool career_view;

    uiRapSheetRankingsDetail(ScreenConstructorData* sd);
    ~uiRapSheetRankingsDetail() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup();
    void RefreshHeader() override;
    void UpdateHighlight();
};

#endif
