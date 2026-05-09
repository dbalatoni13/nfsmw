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
    HudElement(const char *pkg_name, HudFeaturesType mask);

  protected:
    void SetVerticalBarValue(FEImage *vertical_bar, float current_value, float max_value, float texture_offset_bottom, float texture_offset_top,
                             bool top_down);
    void SetHorizontalBarValue(FEImage *bar, float current_value, float max_value, float offset_left, float offset_right, bool left_to_right);

  public:
    virtual ~HudElement() {}
    virtual void Update(IPlayer *player);

    FEString *RegisterString(const char *name) {};
    FEImage *RegisterImage(const char *name) {};
    FEObject *RegisterObject(const char *name) {};
    FEGroup *RegisterGroup(uint32 hash);
    FEString *RegisterString(uint32 hash);
    FEImage *RegisterImage(uint32 hash);
    FEMultiImage *RegisterMultiImage(uint32 hash);
    FEObject *RegisterObject(uint32 hash);

    const char *GetPackageName() {
        return pPackageName;
    }

    Car *GetHudCar(Player *player);

    void Toggle(HudFeaturesType hud_features);

    bool IsElementVisible() {
        return (CurrentHudFeatures & Mask) != 0;
    }

  private:
    bPList<FEObject> Objects;           // offset 0x0, size 0x8
    const char *pPackageName;           // offset 0x8, size 0x4
    HudFeaturesType Mask;               // offset 0x10, size 0x8
    HudFeaturesType CurrentHudFeatures; // offset 0x18, size 0x8
    bool mCurrentlySetVisible;          // offset 0x20, size 0x1
};

#endif
