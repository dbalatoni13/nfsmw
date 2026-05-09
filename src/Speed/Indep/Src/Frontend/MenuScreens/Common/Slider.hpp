#ifndef _CSLIDER
#define _CSLIDER

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include <types.h>

class cSlider {
  public:
    cSlider();
    virtual ~cSlider() {}
    virtual bool Update(u32 msg);
    virtual void Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float range);
    virtual void InitObjects(const char *pkg_name, const char *name);
    virtual void InitValues(float min, float max, float inc, float cur, float range);
    virtual void ToggleVisible(bool bOn);
    void SetValue(float fvalue);
    void Increment();
    void Decrement();
    virtual void Highlight();
    virtual void UnHighlight();
    float GetMax() {
        return fMaxValue;
    }
    float GetMin() {
        return fMinValue;
    }
    float GetValue() {
        return fCurValue;
    }
    float GetPrevValue() {
        return fPrevValue;
    }
    float GetBaseWidth() {
        return FEngGetSizeX(reinterpret_cast<FEObject *>(pBase));
    };
    float GetBaseHeight() {
        return FEngGetSizeY(reinterpret_cast<FEObject *>(pBase));
    };
    virtual void SetPos(float x, float y);
    void SetValueText();
    virtual void Draw();

  protected:
    FEImage *pBase;      // 0x00
    FEImage *pFillBar;   // 0x04
    FEImage *pHandle;    // 0x08
    FEImage *pLeftCap;   // 0x0C
    FEImage *pRightCap;  // 0x10
    FEString *pValue;    // 0x14
    float fMaxValue;     // 0x18
    float fMinValue;     // 0x1C
    float fPrevValue;    // 0x20
    float fCurValue;     // 0x24
    float fDesiredValue; // 0x28
    float fIncrement;    // 0x2C
    float fRange;        // 0x30
    float fInnerOffset;  // 0x34
};

class TwoStageSlider : public cSlider {
  public:
    TwoStageSlider() {}
    ~TwoStageSlider() override {}
    virtual void Init(const char *pkg_name, const char *name, float min, float max, float inc, float cur, float preview, float range);
    void InitObjects(const char *pkg_name, const char *name) override;
    virtual void InitValues(float min, float max, float inc, float cur, float preview, float range);

    float GetPreviewValue() {
        return fPreviewValue;
    }

    void SetPreviewValue(float preview_value) {
        fPreviewValue = preview_value;
    }

    void ToggleVisible(bool bOn) override;
    void Draw() override;

  protected:
    FEImage *pPreviewBar; // offset 0x3C
    float fPreviewValue;  // offset 0x40
};

#endif
