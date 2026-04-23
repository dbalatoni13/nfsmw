#ifndef FRONTEND_HUD_FEREPUTATION_H
#define FRONTEND_HUD_FEREPUTATION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class IReputation : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IReputation(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IReputation() {}

  public:
    virtual void SetReputationCareer(int rep);
    virtual void SetReputationPursuit(int rep);
};

// total size: 0x48
class Reputation : public HudElement, public IReputation {
  public:
    Reputation(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetReputationCareer(int rep) override;
    void SetReputationPursuit(int rep) override;

  private:
    int mReputationCareer;                 // offset 0x30
    int mNumFramesLeftToShow;              // offset 0x34
    FEString *mDataReputationCareer;       // offset 0x38
    FEString *mDataTitle;                  // offset 0x3C
    FEObject *mDataReputationGrp;          // offset 0x40
};

#endif
