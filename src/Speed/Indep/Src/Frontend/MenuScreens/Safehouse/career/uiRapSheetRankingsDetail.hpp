#ifndef __UI_RAP_SHEET_DETAIL_HPP__
#define __UI_RAP_SHEET_DETAIL_HPP__

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"

class RapSheetRankingsDatum : public ArrayDatum {
  public:
    RapSheetRankingsDatum(uint32 item_num, uint32 player_hash, uint32 car_hash, float val)
        : ArrayDatum(0, 0), itemNum(item_num), nameHash(player_hash), carHash(car_hash), value(val) {}
    ~RapSheetRankingsDatum() override {}
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override {};

    uint32 getItemNum() {
        return itemNum;
    }
    uint32 getPlayerName() {
        return nameHash;
    }
    uint32 getCarName() {
        return carHash;
    }
    float getValue() {
        return value;
    }

  private:
    uint32 itemNum;  // offset 0x24, size 0x4
    uint32 nameHash; // offset 0x28, size 0x4
    uint32 carHash;  // offset 0x2C, size 0x4
    float value;     // offset 0x30, size 0x4
};

class RapSheetRankingsArraySlot : public ArraySlot {
  public:
    RapSheetRankingsArraySlot(FEString *item_num, FEString *player_name, FEString *car_name, FEString *value)
        : ArraySlot(item_num), pItemNum(item_num), pPlayerName(player_name), pCarName(car_name), pValue(value) {}
    ~RapSheetRankingsArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

  protected:
    FEString *pItemNum;    // offset 0x14, size 0x4
    FEString *pPlayerName; // offset 0x18, size 0x4
    FEString *pCarName;    // offset 0x1C, size 0x4
    FEString *pValue;      // offset 0x20, size 0x4
};

class RapSheetRankingsTimerArraySlot : public RapSheetRankingsArraySlot {
  public:
    RapSheetRankingsTimerArraySlot(FEString *item_num, FEString *player_name, FEString *car_name, FEString *value)
        : RapSheetRankingsArraySlot(item_num, player_name, car_name, value) {}
    ~RapSheetRankingsTimerArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;
};

class uiRapSheetRankingsDetail : public ArrayScrollerMenu {
  public:
    uiRapSheetRankingsDetail(ScreenConstructorData *sd);
    ~uiRapSheetRankingsDetail() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader() override;

    static bool career_view;

  private:
    void Setup();
    void UpdateHighlight();

    ePursuitDetailTypes rank_type; // offset 0xE8, size 0x4
    int player_rank;               // offset 0xEC, size 0x4
};

#endif
