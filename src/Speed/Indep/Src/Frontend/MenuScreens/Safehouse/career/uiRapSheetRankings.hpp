#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRANKINGS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRANKINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"

class uiRapSheetRankings : public MenuScreen {
  public:
    uiRapSheetRankings(ScreenConstructorData *sd);
    ~uiRapSheetRankings() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void RefreshHeader();

    static bool career_view;

  private:
    void Setup();
    void PrintRanking(uint32 fe_rank, uint32 button_hash, ePursuitDetailTypes type);

    uint32 button_pressed; // offset 0x2C, size 0x4
    uint32 init_button;    // offset 0x30, size 0x4
};

#endif
