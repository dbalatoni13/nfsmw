
#ifndef FRONTEND_MENUSCREENS_CAREER_FEPKG_ENGAGEEVENTDIALOG_H
#define FRONTEND_MENUSCREENS_CAREER_FEPKG_ENGAGEEVENTDIALOG_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"
#include "Speed/Indep/Src/Gameplay/GRuntimeInstance.h"

namespace nsEngageEventDialog {

class EngageEventDialog : public MenuScreen {
  public:
    EngageEventDialog(ScreenConstructorData *sd);
    ~EngageEventDialog() override;
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void NotifyTheGameAcceptEvent();
    void NotifyTheGameDeclineEvent();
    void NotifyTheGameButton3();

    GRuntimeInstance *mpRaceActivity; // offset 0x2C
    UITrackMapStreamer *MapStreamer;  // offset 0x30
    FEMultiImage *TrackMap;           // offset 0x34
};

} // namespace nsEngageEventDialog

#endif
