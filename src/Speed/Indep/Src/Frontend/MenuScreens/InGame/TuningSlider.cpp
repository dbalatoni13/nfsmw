#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/TuningSlider.hpp"

#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

TuningSlider::TuningSlider(Physics::Tunings::Path path, unsigned int title, unsigned int help_blurb, bool active)
    : FEToggleWidget(true), Negative(), Positive() {
    Title = title;
    HelpBlurb = help_blurb;
    bActive = active;
    TuningPath = path;
    bMovedLastUpdate = true;
    FEToggleWidget &self = *this;
    self.BlinkArrows(0);
}

void TuningSlider::Act(const char *parent_pkg, uint32 data) {
    if (!bActive) {
        return;
    }
    if (data == 0xB5971BF1) {
        Current = Current + Increment;
    } else if (data == 0x9120409E) {
        Current = Current - Increment;
    }
    Current = bMin(bMax(Current, Min), Max);
    Negative.SetValue(Min + (Max + Min) * 0.5f - Current);
    Positive.SetValue(Current);
    Update(data);
}

void TuningSlider::CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) {}

void TuningSlider::Draw() {
    FEngSetLanguageHash(GetTitleObject(), Title);
    float val = (Current - Min) / (Max - Min) * 10.0f;
    FEPrintf(GetDataObject(), "%2.1f", val);
    if (bActive) {
        const u32 FEObj_Init = 0x1744B3;
        FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(pSliderGroup, FEObj_Init, true);
    } else {
        const u32 FEObj_GREY = 0x163C76;
        FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        FEngSetScript(pSliderGroup, FEObj_GREY, true);
    }
    Negative.Draw();
    Positive.Draw();
}

void TuningSlider::Position() {}

void TuningSlider::SetFocus(const char *parent_pkg) {
    FEngSetCurrentButton(parent_pkg, GetTitleObject()->NameHash);
    if (bActive) {
        const u32 FEObj_Init = 0x1744B3;
        FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(pSliderGroup, FEObj_Init, true);
    }
}

void TuningSlider::UnsetFocus() {
    if (bActive) {
        const u32 FEObj_Init = 0x1744B3;
        FEngSetScript(GetTitleObject(), FEHASH_UNHIGHLIGHT, true);
        FEngSetScript(pSliderGroup, FEObj_Init, true);
    } else {
        const u32 FEObj_GREY = 0x163C76;
        FEngSetScript(GetTitleObject(), FEObj_GREY, true);
        FEngSetScript(pSliderGroup, FEObj_GREY, true);
    }
}

void TuningSlider::SetSliderGroup(const char *pkg_name, unsigned int group_name) {
    pSliderGroup = FEngFindObject(pkg_name, group_name);
}

void TuningSlider::InitSliderObjects(const char *pkg_name, const char *name) {
    char sztemp[0x20];
    bSNPrintf(sztemp, 0x20, "LEFT_%s", name);
    Negative.InitObjects(pkg_name, sztemp);
    bSNPrintf(sztemp, 0x20, "RIGHT_%s", name);
    Positive.InitObjects(pkg_name, sztemp);
}

void TuningSlider::SetSliderValues(float min, float max, float inc, float cur) {
    Increment = inc;
    Min = min;
    Max = max;
    Current = cur;
    Negative.InitValues(min, (max + min) * 0.5f, 0.0f, (max + min) * 0.5f + min - cur, -82.0f);
    Positive.InitValues((Max + Min) * 0.5f, Max, 0.0f, Current, 82.0f);
}
