#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

cSlider::cSlider()
    : pBase(nullptr), pFillBar(nullptr), pHandle(nullptr), pLeftCap(nullptr), pRightCap(nullptr), pValue(nullptr), fMaxValue(0.0f), fMinValue(0.0f),
      fPrevValue(0.0f), fCurValue(0.0f), fDesiredValue(0.0f), fIncrement(0.0f), fRange(0.0f), fInnerOffset(0.0f) {}

bool cSlider::Update(u32 msg) {
    bool actual_scroll = false;

    switch (msg) {
        case __PAD_LEFT__: {
            fPrevValue = fCurValue;
            fCurValue = bMax(fMinValue, fCurValue - fIncrement);
            if (!(fCurValue == fMinValue && fPrevValue == fMinValue)) {
                actual_scroll = true;
            }
            break;
        }
        case __PAD_RIGHT__: {
            fPrevValue = fCurValue;
            fCurValue = bMin(fMaxValue, fCurValue + fIncrement);
            if (!(fCurValue == fMaxValue && fPrevValue == fMaxValue)) {
                actual_scroll = true;
            }
            break;
        }
    }
    Draw();
    return actual_scroll;
}

void cSlider::Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float range) {
    InitObjects(pkg_name, name);
    InitValues(min, max, inc, cur, range);
}

void cSlider::InitObjects(const char *pkg_name, const char *name) {
    if ((pkg_name != nullptr) && (name != nullptr)) {
        pBase = FEngFindImage(pkg_name, FEngHashString("BASE_%s", name));
        pFillBar = FEngFindImage(pkg_name, FEngHashString("FILLBAR_%s", name));
        pHandle = FEngFindImage(pkg_name, FEngHashString("HANDLE_%s", name));
        pLeftCap = FEngFindImage(pkg_name, FEngHashString("LEFT_CAP_%s", name));
        pRightCap = FEngFindImage(pkg_name, FEngHashString("RIGHT_CAP_%s", name));
        pValue = FEngFindString(pkg_name, FEngHashString("VALUE_%s", name));
    }
}

void cSlider::InitValues(float min, float max, float inc, float cur, float range) {
    fRange = range;
    fMaxValue = max;
    fMinValue = min;
    fIncrement = inc;
    cur = bMin(bMax(cur, min), max);
    fCurValue = cur;
    fDesiredValue = cur;
}

void cSlider::Draw() {
    float d = fMaxValue - fMinValue;
    if (d == 0.0f) {
        fMaxValue = 1.0f;
        fMinValue = 0.0f;
        d = 1.0f;
    }

    float cur_pcnt = (fCurValue - fMinValue) / d;

    float fill_size = fRange * cur_pcnt;

    FEngSetSizeX(pFillBar, fill_size);

    float base_x = FEngGetTopLeftX(pBase);
    float base_y = FEngGetTopLeftY(pBase);

    float vert_offset = -12.0f;
    float horiz_offset = 2.0f;

    FEngSetTopLeft(pFillBar, base_x + horiz_offset, base_y + vert_offset);
    FEngSetBottomRightU(pFillBar, fill_size);

    vert_offset = base_y + vert_offset;
    horiz_offset = base_x - 1.5f;
    FEngSetTopLeft(pLeftCap, horiz_offset, vert_offset);
    horiz_offset = base_x + fill_size - 7.0f;
    FEngSetTopLeft(pRightCap, horiz_offset, vert_offset);

    if (pValue != nullptr) {
        FEPrintf(pValue, "%d", static_cast<int>(fCurValue));
    }
}

void cSlider::ToggleVisible(bool bOn) {
    if (bOn) {
        FEngSetVisible(pBase);
        FEngSetVisible(pFillBar);
        FEngSetVisible(pValue);
        FEngSetVisible(pHandle);
    } else {
        FEngSetInvisible(pValue);
        FEngSetInvisible(pBase);
        FEngSetInvisible(pFillBar);
        FEngSetInvisible(pHandle);
    }
}

void cSlider::SetValue(float fvalue) {
    fvalue = bMax(fvalue, fMinValue);
    fPrevValue = fCurValue;
    fCurValue = bMin(fvalue, fMaxValue);
}

void cSlider::Highlight() {
    FEngSetScript(pBase, 0x249DB7B7, true);
    FEngSetScript(pFillBar, 0x249DB7B7, true);
    FEngSetScript(pValue, 0x249DB7B7, true);
    FEngSetScript(pHandle, 0x249DB7B7, true);
}

void cSlider::UnHighlight() {
    FEngSetScript(pBase, 0x7AB5521A, true);
    FEngSetScript(pFillBar, 0x7AB5521A, true);
    FEngSetScript(pValue, 0x7AB5521A, true);
    FEngSetScript(pHandle, 0x7AB5521A, true);
}

void cSlider::SetPos(float x, float y) {
    FEngSetTopLeft(pBase, x, y);
    FEngSetTopLeft(pFillBar, x, y);
}

void TwoStageSlider::Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float preview, float range) {
    InitObjects(pkg_name, name);
    InitValues(min, max, inc, cur, preview, range);
}

void TwoStageSlider::InitObjects(const char *pkg_name, const char *name) {
    cSlider::InitObjects(pkg_name, name);
    if (pkg_name != nullptr && name != nullptr) {
        pPreviewBar = FEngFindImage(pkg_name, FEngHashString("PREVIEWBAR_%s", name));
    }
}

void TwoStageSlider::InitValues(float min, float max, float inc, float cur, float preview, float range) {
    cSlider::InitValues(min, max, inc, cur, range);
    fPreviewValue = bMin(bMax(min, preview), max);
}

void TwoStageSlider::ToggleVisible(bool bOn) {
    cSlider::ToggleVisible(bOn);
    if (bOn) {
        FEngSetVisible(pPreviewBar);
    } else {
        FEngSetInvisible(pPreviewBar);
    }
}

// UNSOLVED
void TwoStageSlider::Draw() {
    float d = fMaxValue - fMinValue;
    if (d == 0.0f) {
        fMaxValue = 1.0f;
        fMinValue = 0.0f;
        d = 1.0f;
    }
    float cur_pcnt = (fCurValue - fMinValue) / d;
    float fill_size = cur_pcnt * fRange;

    FEngSetSizeX(pFillBar, fill_size);

    float base_x = FEngGetTopLeftX(pBase);
    float base_y = FEngGetTopLeftY(pBase);

    FEngSetTopLeft(pFillBar, base_x, base_y);
    FEngSetBottomRightU(pFillBar, fill_size);

    float preview_pcnt = (fPreviewValue - fMinValue) / d;
    float preview_pos = fRange * preview_pcnt;

    FEngSetTopLeft(pHandle, base_x + preview_pos, base_y);

    if (pValue != nullptr) {
        FEPrintf(pValue, "%d", static_cast<int>(fCurValue));
    }

    if (pPreviewBar != nullptr) {
        preview_pcnt *= fRange;
        FEngSetSizeX(pPreviewBar, preview_pcnt);
        FEngSetBottomRightU(pPreviewBar, preview_pcnt);
        FEngSetTopLeft(pPreviewBar, base_x, base_y);
    }
}
