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

  private:
    FEObject * mpMessageFirstLine;
    FEObject * mpMessageSecondLine;
    FEObject * mpIcon;
    int mPriority;
    unsigned int mNumFramesPlayed;
    char mStringBuffer[64];
    unsigned int mFengHash;
    bool mPlayOneFrame;
};

#endif
