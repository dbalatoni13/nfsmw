#ifndef FEGENERICMESSAGE_H
#define FEGENERICMESSAGE_H

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"

// File: speed/indep/src/frontend/hud/FeGenericMessage.hpp
// total size: 0x90
// Decl: speed/indep/src/frontend/hud/FeGenericMessage.hpp:36
class GenericMessage : public HudElement, public IGenericMessage {
  public:
    GenericMessage(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    ~GenericMessage() override {} // Decl: speed/indep/src/frontend/hud/FeGenericMessage.hpp:39

    bool RequestGenericMessage(const char *string, bool singleFrame, uint32 fengHash, uint32 iconTextureHash, uint32 iconFengHash,
                               GenericMessage_Priority priority) override;

    void RequestGenericMessageZoomOut(uint32 fengHash) override;

    bool IsGenericMessageShowing() override;

    GenericMessage_Priority GetCurrentGenericMessagePriority() override {}

    void Update(IPlayer *player) override;

  private:
    FEObject *mpMessageFirstLine;      // offset 0x30, size 0x4
    FEObject *mpMessageSecondLine;     // offset 0x34, size 0x4
    FEObject *mpIcon;                  // offset 0x38, size 0x4
    GenericMessage_Priority mPriority; // offset 0x3C, size 0x4
    uint32 mNumFramesPlayed;           // offset 0x40, size 0x4
    char mStringBuffer[64];            // offset 0x44, size 0x40
    uint32 mFengHash;                  // offset 0x84, size 0x4
    bool mPlayOneFrame;                // offset 0x88, size 0x1
};

#endif
