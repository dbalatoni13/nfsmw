#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

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
        case 0xB5971BF1: {
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
    fIncrement = inc;
    fMaxValue = max;
    fMinValue = min;
    cur = bMax(cur, min);
    max = bMin(cur, max);
    fCurValue = max;
    fDesiredValue = max;
}

void cSlider::Draw() {
    float d = fMaxValue - fMinValue;
    if (d == 0.0f) {
        fMaxValue = 1.0f;
        fMinValue = 0.0f;
        d = 1.0f;
    }
    float pcnt = (fCurValue - fMinValue) / d;
    float cur_pcnt = fRange * pcnt;

    FEngSetSizeX(reinterpret_cast<FEObject *>(pFillBar), cur_pcnt);

    float base_x = FEngGetTopLeftX(reinterpret_cast<FEObject *>(pBase));
    float base_y = FEngGetTopLeftY(reinterpret_cast<FEObject *>(pBase));

    float vert_offset = base_y + -12.0f;

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pFillBar), base_x + 2.0f, vert_offset);

    FEngSetBottomRightU(pFillBar, cur_pcnt);

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
    FEngSetScript(reinterpret_cast<FEObject *>(pBase), FEHASH_HIGHLIGHT, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pFillBar), FEHASH_HIGHLIGHT, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pValue), FEHASH_HIGHLIGHT, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pHandle), FEHASH_HIGHLIGHT, true);
}

void cSlider::UnHighlight() {
    FEngSetScript(reinterpret_cast<FEObject *>(pBase), FEHASH_UNHIGHLIGHT, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pFillBar), FEHASH_UNHIGHLIGHT, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pValue), FEHASH_UNHIGHLIGHT, true);
    FEngSetScript(reinterpret_cast<FEObject *>(pHandle), FEHASH_UNHIGHLIGHT, true);
}

void cSlider::SetPos(float x, float y) {
    FEngSetTopLeft(reinterpret_cast<FEObject *>(pBase), x, y);
    FEngSetTopLeft(reinterpret_cast<FEObject *>(pFillBar), x, y);
}

void TwoStageSlider::Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float preview, float range) {
    InitObjects(pkg_name, name);
    InitValues(min, max, inc, cur, preview, range);
}

void TwoStageSlider::InitObjects(const char *pkg_name, const char *name) {
    cSlider::InitObjects(pkg_name, name);

    if (pkg_name && name) {
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
        FEngSetVisible(reinterpret_cast<FEObject *>(pPreviewBar));
    } else {
        FEngSetInvisible(reinterpret_cast<FEObject *>(pPreviewBar));
    }
}

void TwoStageSlider::Draw() {
    float d = fMaxValue - fMinValue;
    if (d == 0.0f) {
        fMaxValue = 1.0f;
        fMinValue = 0.0f;
        d = 1.0f;
    }
    float pcnt = (fCurValue - fMinValue) / d;
    float scale = fRange * pcnt;

    FEngSetSizeX(reinterpret_cast<FEObject *>(pFillBar), scale);

    float base_x = FEngGetTopLeftX(reinterpret_cast<FEObject *>(pBase));
    float base_y = FEngGetTopLeftY(reinterpret_cast<FEObject *>(pBase));

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pFillBar), base_x, base_y);

    FEngSetBottomRightU(pFillBar, scale);

    scale = (fPreviewValue - fMinValue) / d;

    FEngSetTopLeft(reinterpret_cast<FEObject *>(pHandle), fRange * scale + base_x, base_y);

    if (pValue) {
        FEPrintf(pValue, "%d", static_cast<int>(fCurValue));
    }

    if (pPreviewBar != nullptr) {
        float preview_pcnt = fRange * scale;
        scale = preview_pcnt;
        FEngSetSizeX(reinterpret_cast<FEObject *>(pPreviewBar), scale);
        FEngSetBottomRightU(pPreviewBar, scale);
        FEngSetTopLeft(reinterpret_cast<FEObject *>(pPreviewBar), base_x, base_y);
    }
}
