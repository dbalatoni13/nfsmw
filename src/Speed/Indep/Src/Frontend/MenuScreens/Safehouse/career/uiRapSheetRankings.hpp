#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRANKINGS_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIRAPSHEETRANKINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"

struct uiRapSheetRankings : public MenuScreen {
    unsigned int button_pressed; // offset 0x2C, size 0x4
    unsigned int init_button; // offset 0x30, size 0x4

    static bool career_view;

    uiRapSheetRankings(ScreenConstructorData* sd);
    ~uiRapSheetRankings() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void RefreshHeader();
    void Setup();
    void PrintRanking(unsigned int fe_rank, unsigned int button_hash, ePursuitDetailTypes type);
};

#endif
