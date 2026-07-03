#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include <types.h>

// File: speed/indep/src/frontend/menuscreens/common/Slider.hpp
// total size: 0x3C
// Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:74
class cSlider {
  public:
    cSlider();
    virtual ~cSlider() {} // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:77

    virtual bool Update(u32 msg);
    virtual void Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float range);
    virtual void InitObjects(const char *pkg_name, const char *name);
    virtual void InitValues(float min, float max, float inc, float cur, float range);
    virtual void ToggleVisible(bool bOn);

    void SetValue(float fvalue);

    void Increment() {} // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:87
    void Decrement() {} // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:88

    virtual void Highlight();
    virtual void UnHighlight();

    float GetMax() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:93
        return fMaxValue;
    }
    float GetMin() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:94
        return fMinValue;
    }
    float GetValue() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:95
        return fCurValue;
    }
    float GetPrevValue() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:96
        return fPrevValue;
    }
    float GetBaseWidth() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:97
        return FEngGetSizeX(reinterpret_cast<FEObject *>(pBase));
    }
    float GetBaseHeight() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:98
        return FEngGetSizeY(reinterpret_cast<FEObject *>(pBase));
    }

    virtual void SetPos(float x, float y);

    virtual void Draw();

  protected:
    FEImage *pBase;      // offset 0x0, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:109
    FEImage *pFillBar;   // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:110
    FEImage *pHandle;    // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:112
    FEImage *pLeftCap;   // offset 0xC, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:113
    FEImage *pRightCap;  // offset 0x10, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:114
    FEString *pValue;    // offset 0x14, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:115
    float fMaxValue;     // offset 0x18, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:117
    float fMinValue;     // offset 0x1C, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:118
    float fPrevValue;    // offset 0x20, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:119
    float fCurValue;     // offset 0x24, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:120
    float fDesiredValue; // offset 0x28, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:121
    float fIncrement;    // offset 0x2C, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:122
    float fRange;        // offset 0x30, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:123
    float fInnerOffset;  // offset 0x34, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:124
};

// total size: 0x44
// Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:130
class TwoStageSlider : public cSlider {
  public:
    TwoStageSlider() {}           // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:132
    ~TwoStageSlider() override {} // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:133

    virtual void Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float preview, float range);
    void InitObjects(const char *pkg_name, const char *name) override;
    virtual void InitValues(float min, float max, float inc, float cur, float preview, float range);

    float GetPreviewValue() { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:143
        return fPreviewValue;
    }

    void SetPreviewValue(float preview_value) { // Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:146
        fPreviewValue = preview_value;
    }

    void ToggleVisible(bool bOn) override;
    void Draw() override;

  protected:
    FEImage *pPreviewBar; // offset 0x3C, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:153
    float fPreviewValue;  // offset 0x40, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/Slider.hpp:155
};

#endif
