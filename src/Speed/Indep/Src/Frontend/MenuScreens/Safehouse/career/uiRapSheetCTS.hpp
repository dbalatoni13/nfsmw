#ifndef __UI_RAP_SHEET_CTS_HPP__
#define __UI_RAP_SHEET_CTS_HPP__

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feArrayScrollerMenu.hpp"

class RapSheetCTSDatum : public ArrayDatum {
  public:
    RapSheetCTSDatum(int num_times, u32 item_name, int total_value) : ArrayDatum(0, 0), times(num_times), itemHash(item_name), value(total_value) {}
    ~RapSheetCTSDatum() override {}
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override {};

    int getNumTimes() {
        return times;
    }
    u32 getItemHash() {
        return itemHash;
    }
    int getTotalValue() {
        return value;
    }

  private:
    int times;    // offset 0x24, size 0x4
    u32 itemHash; // offset 0x28, size 0x4
    int value;    // offset 0x2C, size 0x4
};

class RapSheetCTSArraySlot : public ArraySlot {
  public:
    RapSheetCTSArraySlot(FEString *times, FEString *item, FEString *value) : ArraySlot(times), pTimes(times), pItem(item), pValue(value) {}
    ~RapSheetCTSArraySlot() override {}
    void Update(ArrayDatum *datum, bool isSelected) override;

  private:
    FEString *pTimes; // offset 0x14, size 0x4
    FEString *pItem;  // offset 0x18, size 0x4
    FEString *pValue; // offset 0x1C, size 0x4
};

class uiRapSheetCTS : public ArrayScrollerMenu {
  public:
    uiRapSheetCTS(ScreenConstructorData *sd);
    ~uiRapSheetCTS() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader() override;

  private:
    void Setup();
};

#endif
