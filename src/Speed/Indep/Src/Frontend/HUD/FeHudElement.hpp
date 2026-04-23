#ifndef FRONTEND_HUD_FEHUDELEMENT_H
#define FRONTEND_HUD_FEHUDELEMENT_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/bWare/Inc/bList.hpp"

typedef unsigned long long HudFeaturesType;

struct FEGroup;
struct FEImage;
struct FEMultiImage;
struct FEString;
struct Car;
class Player;
class IPlayer;

// total size: 0x28
class HudElement {
  public:
    HudElement(const char *pkg_name, unsigned long long mask);
    virtual ~HudElement() {}
    virtual void Update(IPlayer *player);

    FEString *RegisterString(const char *name);
    FEImage *RegisterImage(const char *name);
    FEObject *RegisterObject(const char *name);
    const char *GetPackageName() {
        return pPackageName;
    }
    bool IsElementVisible() {
        return (CurrentHudFeatures & Mask) != 0;
    }

    Car *GetHudCar(Player *player);

    FEString *RegisterString(unsigned int hash);
    FEImage *RegisterImage(unsigned int hash);
    FEMultiImage *RegisterMultiImage(unsigned int hash);
    FEObject *RegisterObject(unsigned int hash);
    FEGroup *RegisterGroup(unsigned int hash);

    void Toggle(unsigned long long hud_features);

  protected:
    void SetVerticalBarValue(FEImage *vertical_bar, float current_value, float max_value, float texture_offset_bottom, float texture_offset_top,
                             bool top_down);
    void SetHorizontalBarValue(FEImage *bar, float current_value, float max_value, float offset_left, float offset_right, bool left_to_right);

  private:
    bPList<FEObject> Objects; // offset 0x0, size 0x8

    // TODO private
  protected:
    const char *pPackageName; // offset 0x8, size 0x4

  private:
    HudFeaturesType Mask;               // offset 0x10, size 0x8
    HudFeaturesType CurrentHudFeatures; // offset 0x18, size 0x8
    bool mCurrentlySetVisible;          // offset 0x20, size 0x1
};

#endif
