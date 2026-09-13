#ifndef SLIDER_HPP
#define SLIDER_HPP

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include <types.h>

// total size: 0x3C
// Decl: 74
class cSlider {
  public:
    cSlider();
    virtual ~cSlider() {} // Decl: 77

    virtual bool Update(u32 msg);
    virtual void Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float range);
    virtual void InitObjects(const char *pkg_name, const char *name);
    virtual void InitValues(float min, float max, float inc, float cur, float range);
    virtual void ToggleVisible(bool bOn);

    void SetValue(float fvalue);

    void Increment() {} // Decl: 87
    void Decrement() {} // Decl: 88

    virtual void Highlight();
    virtual void UnHighlight();

    float GetMax() { // Decl: 93
        return fMaxValue;
    }
    float GetMin() { // Decl: 94
        return fMinValue;
    }
    float GetValue() { // Decl: 95
        return fCurValue;
    }
    float GetPrevValue() { // Decl: 96
        return fPrevValue;
    }
    float GetBaseWidth() { // Decl: 97
        return FEngGetSizeX(pBase);
    }
    float GetBaseHeight() { // Decl: 98
        return FEngGetSizeY(pBase);
    }

    virtual void SetPos(float x, float y);

    virtual void Draw();

  protected:
    FEImage *pBase;      // offset 0x0, size 0x4, Decl: 109
    FEImage *pFillBar;   // offset 0x4, size 0x4, Decl: 110
    FEImage *pHandle;    // offset 0x8, size 0x4, Decl: 112
    FEImage *pLeftCap;   // offset 0xC, size 0x4, Decl: 113
    FEImage *pRightCap;  // offset 0x10, size 0x4, Decl: 114
    FEString *pValue;    // offset 0x14, size 0x4, Decl: 115
    float fMaxValue;     // offset 0x18, size 0x4, Decl: 117
    float fMinValue;     // offset 0x1C, size 0x4, Decl: 118
    float fPrevValue;    // offset 0x20, size 0x4, Decl: 119
    float fCurValue;     // offset 0x24, size 0x4, Decl: 120
    float fDesiredValue; // offset 0x28, size 0x4, Decl: 121
    float fIncrement;    // offset 0x2C, size 0x4, Decl: 122
    float fRange;        // offset 0x30, size 0x4, Decl: 123
    float fInnerOffset;  // offset 0x34, size 0x4, Decl: 124
};

// total size: 0x44
// Decl: 130
class TwoStageSlider : public cSlider {
  public:
    TwoStageSlider() {}           // Decl: 132
    ~TwoStageSlider() override {} // Decl: 133

    virtual void Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float preview, float range);
    void InitObjects(const char *pkg_name, const char *name) override;
    virtual void InitValues(float min, float max, float inc, float cur, float preview, float range);

    float GetPreviewValue() { // Decl: 143
        return fPreviewValue;
    }

    void SetPreviewValue(float preview_value) { // Decl: 146
        fPreviewValue = bMin(bMax(preview_value, fMinValue), fMaxValue);
    }

    void ToggleVisible(bool bOn) override;
    void Draw() override;

  protected:
    FEImage *pPreviewBar; // offset 0x3C, size 0x4, Decl: 153
    float fPreviewValue;  // offset 0x40, size 0x4, Decl: 155
};

#endif
