#ifndef FRONTEND_MENUSCREENS_INGAME_TUNINGSLIDER_H
#define FRONTEND_MENUSCREENS_INGAME_TUNINGSLIDER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Physics/PhysicsTunings.h"

// File: speed/indep/src/frontend/menuscreens/ingame/TuningSlider.hpp
// total size: 0x100
class TuningSlider : public FEToggleWidget {
  public:
    TuningSlider(Physics::Tunings::Path path, unsigned int title, unsigned int help_blurb, bool active);
    ~TuningSlider() override {}

    void Act(const char *parent_pkg, uint32 data) override;
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Position() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;

    void SetSliderGroup(const char *pkg_name, unsigned int group_name);
    void InitSliderObjects(const char *pkg_name, const char *name);
    void SetSliderValues(float min, float max, float inc, float cur);

    FEObject *pSliderGroup;              // offset 0x64, size 0x4
    cSlider Negative;                    // offset 0x68, size 0x3C
    cSlider Positive;                    // offset 0xA4, size 0x3C
    unsigned int Title;                  // offset 0xE0, size 0x4
    unsigned int HelpBlurb;              // offset 0xE4, size 0x4
    float Min;                           // offset 0xE8, size 0x4
    float Max;                           // offset 0xEC, size 0x4
    float Current;                       // offset 0xF0, size 0x4
    float Increment;                     // offset 0xF4, size 0x4
    bool bActive;                        // offset 0xF8, size 0x4 (bool ocupa 4 B en este compilador)
    Physics::Tunings::Path TuningPath;   // offset 0xFC, size 0x4
};

#endif
