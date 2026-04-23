#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

struct FEObject;

extern FEImage *FEngFindImage(const char *pkg_name, int name_hash);
extern FEString *FEngFindString(const char *pkg_name, int name_hash);
extern unsigned int FEngHashString(const char *, ...);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern void FEngGetSize(FEObject *object, float &x, float &y);
extern void FEngSetSize(FEObject *object, float x, float y);
extern void FEngGetTopLeft(FEObject *object, float &x, float &y);
extern void FEngSetTopLeft(FEObject *object, float x, float y);
extern void FEngGetBottomRightUV(FEImage *img, float &u, float &v);
extern void FEngSetBottomRightUV(FEImage *img, float u, float v);
extern int FEPrintf(FEString *text, const char *fmt, ...);

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

bool cSlider::Update(unsigned long msg) {
    bool actual_scroll = false;
    if (msg == 0x9120409E) {
        fPrevValue = fCurValue;
        float newVal = fCurValue - fIncrement;
        float d = fMinValue;
        if (fMinValue - newVal < 0.0f) {
            d = newVal;
        }
        fCurValue = d;
        if (d != fMinValue || fCurValue != fMinValue) {
            actual_scroll = true;
        }
    } else if (msg == 0xB59719F1) {
        fPrevValue = fCurValue;
        float newVal = fCurValue + fIncrement;
        float d = fMaxValue;
        if (fMaxValue - newVal < 0.0f) {
            d = newVal;
        }
        fCurValue = d;
        if (d != fMaxValue || fCurValue != fMaxValue) {
            actual_scroll = true;
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

void cSlider::SetValue(float fvalue) {
    fvalue = bMax(fvalue, fMinValue);
    fPrevValue = fCurValue;
    float max = bMin(fvalue, fMaxValue);
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
    float d = fMaxValue;
    if (fMaxValue - preview < 0.0f) {
        d = preview;
    }
    float c = fMinValue;
    if (fMinValue - d < 0.0f) {
        c = d;
    }
    fPreviewValue = c;
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
