#ifndef FEHUDELEMENT_H
#define FEHUDELEMENT_H

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/World/Car.hpp"
#include "Speed/Indep/Src/World/Player.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

typedef unsigned long long HudFeaturesType; // :13

// total size: 0x28
// Decl: 17
class HudElement {
  private:
    bPList<FEObject> Objects;           // offset 0x0, size 0x8, Decl: 19
    const char *pPackageName;           // offset 0x8, size 0x4, Decl: 20
    HudFeaturesType Mask;               // offset 0x10, size 0x8, Decl: 21
    HudFeaturesType CurrentHudFeatures; // offset 0x18, size 0x8, Decl: 22
    bool mCurrentlySetVisible;          // offset 0x20, size 0x1

  protected:
    void SetVerticalBarValue(FEImage *vertical_bar, float current_value, float max_value, float texture_offset_bottom, float texture_offset_top,
                             bool top_down);

    void SetHorizontalBarValue(FEImage *bar, float current_value, float max_value, float offset_left, float offset_right, bool left_to_right);

  public:
    HudElement(const char *pkg_name, HudFeaturesType mask);
    virtual ~HudElement() {} // Decl: 33

    virtual void Update(IPlayer *player) {} // Decl: 35

    FEString *RegisterString(const char *name) { // Decl: 45
        return RegisterString(FEHashUpper(name));
    }
    FEString *RegisterString(uint32 hash);     // Decl: 45
    FEImage *RegisterImage(const char *name) { // Decl: 46
        return RegisterImage(FEHashUpper(name));
    }
    FEImage *RegisterImage(uint32 hash); // Decl: 46

    FEObject *RegisterObject(const char *name) { // Decl: 47
        return RegisterObject(FEHashUpper(name));
    }
    FEObject *RegisterObject(uint32 hash); // Decl: 47

    FEMultiImage *RegisterMultiImage(uint32 hash);

    FEGroup *RegisterGroup(uint32 hash);

    const char *GetPackageName() { // Decl: 55
        return pPackageName;
    }

    Car *GetHudCar(Player *player);

    void Toggle(HudFeaturesType hud_features);

    bool IsElementVisible() { // Decl: 58
        return (CurrentHudFeatures & Mask) != 0;
    }
};

// TODO: these are from carbon and are not accurate to MW
#define MAKE_HUD_FEATURE(__bit) (((HudFeaturesType)1) << (__bit)) // :64
#define HUD_FEATURE_TACHOMETER MAKE_HUD_FEATURE(1)                // :66
#define HUD_FEATURE_RACE_OVER MAKE_HUD_FEATURE(2)                 // :67
#define HUD_FEATURE_LEADERBOARD MAKE_HUD_FEATURE(3)               // :68
#define HUD_FEATURE_SPLIT_TIME MAKE_HUD_FEATURE(4)                // :69
#define HUD_FEATURE_WRONG_WAY MAKE_HUD_FEATURE(5)                 // :70
#define HUD_FEATURE_FULL_SCREEN_FADE MAKE_HUD_FEATURE(7)          // :72
#define HUD_FEATURE_EVENT_INDICATOR MAKE_HUD_FEATURE(8)           // :73
#define HUD_FEATURE_321_GO MAKE_HUD_FEATURE(10)                   // :75
#define HUD_FEATURE_NITROUS MAKE_HUD_FEATURE(11)                  // :76
#define HUD_FEATURE_ENGAGE_EVENT MAKE_HUD_FEATURE(13)             // :78
#define HUD_FEATURE_HEAT_METER MAKE_HUD_FEATURE(14)               // :79
#define HUD_FEATURE_MINIMAP MAKE_HUD_FEATURE(16)                  // :80
#define HUD_FEATURE_SPEEDBREAKER_METER MAKE_HUD_FEATURE(18)       // :82
#define HUD_FEATURE_RVM MAKE_HUD_FEATURE(19)                      // :83
#define HUD_FEATURE_PIP MAKE_HUD_FEATURE(20)                      // :84
#define HUD_FEATURE_WINGMAN_METER MAKE_HUD_FEATURE(21)            // :85
#define HUD_FEATURE_MENU_ZONE MAKE_HUD_FEATURE(22)                // :86
#define HUD_FEATURE_RADAR_DETECTOR MAKE_HUD_FEATURE(23)           // :87
#define HUD_FEATURE_GENERIC_MESSAGE MAKE_HUD_FEATURE(24)          // :88
#define HUD_FEATURE_TIME_EXTENSION MAKE_HUD_FEATURE(25)           // :89
#define HUD_FEATURE_RACEINFORMATION MAKE_HUD_FEATURE(26)          // :90
#define HUD_FEATURE_DRIFTSCORES MAKE_HUD_FEATURE(27)              // :91
#define HUD_FEATURE_AUTO_SAVE_ICON MAKE_HUD_FEATURE(28)           // :92
#define HUD_FEATURE_PURSUITINFORMATION MAKE_HUD_FEATURE(29)       // :93
#define HUD_FEATURE_CANYON_METER MAKE_HUD_FEATURE(30)             // :95
#define HUD_FEATURE_REVOLUTION_WHEEL MAKE_HUD_FEATURE(31)         // :96
#define HUD_FEATURE_FADE_TO_BLACK MAKE_HUD_FEATURE(32)            // :97

#endif
