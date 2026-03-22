#ifndef FRONTEND_HUD_FECOSTTOSTATE_H
#define FRONTEND_HUD_FECOSTTOSTATE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"

class ICostToState : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    ICostToState(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~ICostToState() {}

  public:
    virtual void SetCostToState(int cost);
    virtual void SetInPursuit(bool inPursuit);
};

// total size: 0x48
class CostToState : public HudElement, public ICostToState {
  public:
    CostToState(UTL::COM::Object *pOutter, const char *pkg_name, int player_number);
    void Update(IPlayer *player) override;
    void SetCostToState(int cost) override;
    void SetInPursuit(bool inPursuit) override;

  private:
    bool mCostToStateOn;             // offset 0x30
    int mCostToState;                // offset 0x34
    bool mInPursuit;                 // offset 0x38
    int mNumFramesLeftToShow;        // offset 0x3C
    FEString *mDataCostToState;      // offset 0x40
    FEString *mDataTitle;            // offset 0x44
};

#endif
