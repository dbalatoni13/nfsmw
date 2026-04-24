#ifndef FRONTEND_HUD_FEGENERICMESSAGE_H
#define FRONTEND_HUD_FEGENERICMESSAGE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

class GenericMessage : public HudElement, public IGenericMessage {
  public:
    GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    GenericMessage_Priority GetCurrentGenericMessagePriority() override;
    bool RequestGenericMessage(const char *string, bool singleFrame, unsigned int fengHash, unsigned int iconTextureHash, unsigned int iconFengHash, GenericMessage_Priority priority) override;
    void RequestGenericMessageZoomOut(unsigned int fengHash) override;
    bool IsGenericMessageShowing() override;

  private:
    FEObject * mpMessageFirstLine;
    FEObject * mpMessageSecondLine;
    FEObject * mpIcon;
    GenericMessage_Priority mPriority;
    unsigned int mNumFramesPlayed;
    char mStringBuffer[64];
    unsigned int mFengHash;
    bool mPlayOneFrame;
};

#endif
