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
    bool RequestGenericMessage(const char *string, bool singleFrame, uint32 fengHash, uint32 iconTextureHash, uint32 iconFengHash,
                               GenericMessage_Priority priority) override;
    void RequestGenericMessageZoomOut(uint32 fengHash) override;
    bool IsGenericMessageShowing() override;
    GenericMessage_Priority GetCurrentGenericMessagePriority() override {
        return mPriority;
    };
    void Update(IPlayer *player) override;

  private:
    FEObject *mpMessageFirstLine;
    FEObject *mpMessageSecondLine;
    FEObject *mpIcon;
    GenericMessage_Priority mPriority;
    uint32 mNumFramesPlayed;
    char mStringBuffer[64];
    uint32 mFengHash;
    bool mPlayOneFrame;
};

#endif
