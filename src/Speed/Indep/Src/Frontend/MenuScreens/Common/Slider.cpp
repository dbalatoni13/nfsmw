#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

void cSlider::InitValues(float min, float max, float inc, float cur, float range) {
    fRange = range;
    if (cur - min < 0.0f) {
        cur = min;
    }
    fIncrement = inc;
    fMaxValue = max;
    if (cur - max < 0.0f) {
        max = cur;
    }
    fMinValue = min;
    fDesiredValue = max;
    fCurValue = max;
}

void cSlider::SetValue(float fvalue) {
    if (fvalue - fMinValue < 0.0f) {
        fvalue = fMinValue;
    }
    fPrevValue = fCurValue;
    float max = fMaxValue;
    if (fvalue - fMaxValue < 0.0f) {
        max = fvalue;
    }
    fCurValue = max;
}
