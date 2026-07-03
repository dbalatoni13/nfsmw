#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

cSlider::cSlider() {
    pBase = nullptr;
    pFillBar = nullptr;
    pHandle = nullptr;
    pLeftCap = nullptr;
    pRightCap = nullptr;
    pValue = nullptr;
    fMaxValue = 0.0f;
    fMinValue = 0.0f;
    fPrevValue = 0.0f;
    fCurValue = 0.0f;
    fDesiredValue = 0.0f;
    fIncrement = 0.0f;
    fRange = 0.0f;
    fInnerOffset = 0.0f;
}

bool cSlider::Update(u32 msg) {
    bool actual_scroll = false;

    switch (msg) {
        case 0x9120409E: {
            fPrevValue = fCurValue;
            fCurValue = bMax(fMinValue, fCurValue - fIncrement);
            if (!(fCurValue == fMinValue && fPrevValue == fMinValue)) {
                actual_scroll = true;
            }
            break;
        }
        case 0xB59719F1: {
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
    if (pkg_name && name) {
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
    cur = bMax(cur, min);
    fIncrement = inc;
    fMaxValue = max;
    max = bMin(cur, max);
    fMinValue = min;
    fDesiredValue = max;
    fCurValue = max;
}

void cSlider::Draw() {
    float d = fMaxValue - fMinValue;
    if (d == 0.0f) {
        fMinValue = 0.0f;
        d = 1.0f;
        fMaxValue = 1.0f;
    }
    float cur_pcnt = fRange * ((fCurValue - fMinValue) / d);

    float y;
    FEngGetSize(reinterpret_cast<FEObject *>(pFillBar), cur_pcnt, y);
    FEngSetSize(reinterpret_cast<FEObject *>(pFillBar), cur_pcnt, y);

    float base_x;
    float dummy1;
    FEngGetTopLeft(reinterpret_cast<FEObject *>(pBase), base_x, dummy1);

    float dummy2;
    float base_y;
    FEngGetTopLeft(reinterpret_cast<FEObject *>(pBase), dummy2, base_y);
    float vert_offset = base_y + -12.0f;

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pFillBar), base_x + 2.0f, vert_offset);

    float dummy3;
    float v;
    FEngGetBottomRightUV(pFillBar, dummy3, v);
    FEngSetBottomRightUV(pFillBar, cur_pcnt, v);

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pLeftCap), base_x - 1.5f, vert_offset);
    FEngSetTopLeft(reinterpret_cast<FEObject *>(pRightCap), base_x + cur_pcnt - 7.0f, vert_offset);

    if (pValue) {
        FEPrintf(pValue, "%d", static_cast<int>(fCurValue));
    }
}

void cSlider::ToggleVisible(bool bOn) {
    if (bOn) {
        FEngSetVisible(reinterpret_cast<FEObject *>(pBase));
        FEngSetVisible(reinterpret_cast<FEObject *>(pFillBar));
        FEngSetVisible(reinterpret_cast<FEObject *>(pValue));
        FEngSetVisible(reinterpret_cast<FEObject *>(pHandle));
    } else {
        FEngSetInvisible(reinterpret_cast<FEObject *>(pValue));
        FEngSetInvisible(reinterpret_cast<FEObject *>(pBase));
        FEngSetInvisible(reinterpret_cast<FEObject *>(pFillBar));
        FEngSetInvisible(reinterpret_cast<FEObject *>(pHandle));
    }
}

void cSlider::SetValue(float fvalue) {
    fvalue = bMax(fvalue, fMinValue);
    fPrevValue = fCurValue;
    float max = bMin(fvalue, fMaxValue);
    fCurValue = max;
}

void cSlider::Highlight() {
    FEngSetScript(reinterpret_cast<FEObject *>(pBase), 0x249DB7B7, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pFillBar), 0x249DB7B7, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pValue), 0x249DB7B7, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pHandle), 0x249DB7B7, true);
}

void cSlider::UnHighlight() {
    FEngSetScript(reinterpret_cast<FEObject *>(pBase), 0x7AB5521A, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pFillBar), 0x7AB5521A, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pValue), 0x7AB5521A, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pHandle), 0x7AB5521A, true);
}

void cSlider::SetPos(float x, float y) {
    FEngSetTopLeft(reinterpret_cast<FEObject *>(pBase), x, y);
    FEngSetTopLeft(reinterpret_cast<FEObject *>(pFillBar), x, y);
}

void TwoStageSlider::Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float preview, float range) {
    cSlider::InitObjects(pkg_name, name);
    InitObjects(pkg_name, name);
    InitValues(min, max, inc, cur, preview, range);
}

void TwoStageSlider::InitObjects(const char *pkg_name, const char *name) {
    cSlider::InitObjects(pkg_name, name);
    pPreviewBar = FEngFindImage(pkg_name, FEngHashString("PREVIEWBAR_%s", name));
}

void TwoStageSlider::InitValues(float min, float max, float inc, float cur, float preview, float range) {
    cSlider::InitValues(min, max, inc, cur, range);
    fPreviewValue = bMax(bMin(fMaxValue, preview), fMinValue);
}

void TwoStageSlider::ToggleVisible(bool bOn) {
    cSlider::ToggleVisible(bOn);
    if (bOn) {
        FEngSetVisible(reinterpret_cast<FEObject *>(pPreviewBar));
    } else {
        FEngSetInvisible(reinterpret_cast<FEObject *>(pPreviewBar));
    }
}

void TwoStageSlider::Draw() {
    float d = fMaxValue - fMinValue;
    if (d == 0.0f) {
        fMinValue = 0.0f;
        d = 1.0f;
        fMaxValue = 1.0f;
    }
    float cur_pcnt = fRange * ((fCurValue - fMinValue) / d);
    float fill_size = cur_pcnt;

    float y;
    FEngGetSize(reinterpret_cast<FEObject *>(pFillBar), fill_size, y);
    FEngSetSize(reinterpret_cast<FEObject *>(pFillBar), fill_size, y);

    float base_x;
    float dummy1;
    FEngGetTopLeft(reinterpret_cast<FEObject *>(pBase), base_x, dummy1);

    float dummy2;
    float base_y;
    FEngGetTopLeft(reinterpret_cast<FEObject *>(pBase), dummy2, base_y);
    float vert_offset = base_y + -12.0f;

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pFillBar), base_x + 2.0f, vert_offset);

    float dummy3;
    float v;
    FEngGetBottomRightUV(pFillBar, dummy3, v);
    FEngSetBottomRightUV(pFillBar, fill_size, v);

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pLeftCap), base_x - 1.5f, vert_offset);
    FEngSetTopLeft(reinterpret_cast<FEObject *>(pRightCap), base_x + fill_size - 7.0f, vert_offset);

    if (pValue) {
        FEPrintf(pValue, "%d", static_cast<int>(fCurValue));
    }

    float preview_pcnt = fRange * ((fPreviewValue - fMinValue) / d);

    float y2;
    FEngGetSize(reinterpret_cast<FEObject *>(pPreviewBar), preview_pcnt, y2);
    FEngSetSize(reinterpret_cast<FEObject *>(pPreviewBar), preview_pcnt, y2);

    float dummy4;
    float v2;
    FEngGetBottomRightUV(pPreviewBar, dummy4, v2);
    FEngSetBottomRightUV(pPreviewBar, preview_pcnt, v2);
}
