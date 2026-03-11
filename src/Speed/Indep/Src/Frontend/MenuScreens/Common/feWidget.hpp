#ifndef _FEWIDGET
#define _FEWIDGET

#include <types.h>

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

struct FEObject;
struct FEString;
struct FEImage;

// 0x34
struct FEWidget : public bTNode<FEWidget> {
private:
    bVector2 vTopLeft;        // 0x08
    bVector2 vSize;           // 0x10
    bVector2 vBackingOffset;  // 0x18
    FEObject* pBacking;       // 0x20
    bool bEnabled;            // 0x24
    bool bHidden;             // 0x28

protected:
    bool bMovedLastUpdate;    // 0x2C

public:
    FEWidget(FEObject* backing, bool enabled, bool hidden);
    virtual ~FEWidget();
    virtual void Act(const char* parent_pkg, unsigned int data);
    virtual void CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y);
    virtual void Draw();
    virtual void Position();
    virtual void Show();
    virtual void Hide();
    virtual void Enable() { bEnabled = true; }
    virtual void Disable() { bEnabled = false; }
    virtual void SetFocus(const char* parent_pkg);
    virtual void UnsetFocus();
    virtual void SetPos(bVector2& pos) { SetTopLeft(pos); }
    virtual void SetPosX(float x) { SetTopLeftX(x); }
    virtual void SetPosY(float y) { SetTopLeftY(y); }

    bool IsEnabled();
    bool IsHidden();
    void GetTopLeft(bVector2& top_left);
    float GetTopLeftX();
    float GetTopLeftY();
    void GetSize(bVector2& size);
    float GetWidth();
    float GetHeight();
    void SetTopLeft(bVector2& top_left) { vTopLeft = top_left; }
    void SetTopLeftX(float x) { vTopLeft.x = x; }
    void SetTopLeftY(float y) { vTopLeft.y = y; }
    void SetSize(bVector2& size);
    void SetWidth(float width);
    void SetHeight(float height);
    void SetBacking(FEObject* obj);
    FEObject* GetBacking();
    void GetBackingOffset(bVector2& offset);
    float GetBackingOffsetX();
    float GetBackingOffsetY();
    void SetBackingOffset(bVector2& offset);
    void SetBackingOffset(float x, float y);
    void SetBackingOffsetX(float x);
    void SetBackingOffsetY(float y);
    bool MovedLastUpdate();
};

// 0x54
struct FEStatWidget : public FEWidget {
private:
    FEString* pTitle;        // 0x34
    FEString* pData;         // 0x38
    bVector2 vMaxTitleSize;  // 0x3C
    bVector2 vMaxDataSize;   // 0x44
    bVector2 vDataPos;       // 0x4C

public:
    FEStatWidget(bool enabled);
    ~FEStatWidget() override;
    void Act(const char* parent_pkg, unsigned int data) override;
    void CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Position() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char* parent_pkg) override;
    void UnsetFocus() override;
    void SetPos(bVector2& pos) override;
    void SetPosX(float x) override;
    void SetPosY(float y) override;

    FEString* GetTitleObject() { return pTitle; }
    FEString* GetDataObject() { return pData; }
    void SetTitleObject(FEString* string);
    void SetDataObject(FEString* string);
    void GetDataPos(bVector2& pos);
    float GetDataPosX();
    float GetDataPosY();
    void SetDataPos(bVector2& pos);
    void SetDataPosX(float x);
    void SetDataPosY(float y);
    void GetMaxTitleSize(bVector2& size);
    float GetMaxTitleWidth();
    float GetMaxTitleHeight();
    void GetMaxDataSize(bVector2& size);
    float GetMaxDataWidth();
    float GetMaxDataHeight();
    void SetMaxTitleSize(bVector2& size);
    void SetMaxTitleWidth(float width);
    void SetMaxTitleHeight(float height);
    void SetMaxDataSize(bVector2& size);
    void SetMaxDataWidth(float x);
    void SetMaxDataHeight(float y);
};

// 0x64
struct FEToggleWidget : public FEStatWidget {
private:
    FEImage* pLeftImage;       // 0x54
    FEImage* pRightImage;      // 0x58
    unsigned int EnableScript; // 0x5C
    unsigned int DisableScript;// 0x60

public:
    FEToggleWidget(bool enabled);
    ~FEToggleWidget() override;
    void Act(const char* parent_pkg, unsigned int data) override;
    void CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Position() override;
    void Show() override;
    void Hide() override;
    void Enable() override;
    void Disable() override;
    void SetFocus(const char* parent_pkg) override;
    void UnsetFocus() override;
    virtual void BlinkArrows(unsigned int data);

    FEImage* GetLeftImage() { return pLeftImage; }
    FEImage* GetRightImage() { return pRightImage; }
    void SetLeftImage(FEImage* img);
    void SetRightImage(FEImage* img);
    bool Update(unsigned int msg) {
        bMovedLastUpdate = true;
        BlinkArrows(msg);
        Draw();
        return true;
    }
    unsigned int GetEnableScript();
    unsigned int GetDisableScript();
    void SetEnableScript(unsigned int script);
    void SetDisableScript(unsigned int script);
    void SetScript(unsigned int script);
};

// 0xA4
struct FESliderWidget : public FEToggleWidget {
protected:
    cSlider Slider;     // 0x64
    float fVertOffset;  // 0xA0

public:
    FESliderWidget(bool enabled);
    ~FESliderWidget() override;
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void Position() override;
    void Show() override;
    void Hide() override;
    void Enable() override;
    void Disable() override;
    void SetFocus(const char* parent_pkg) override;
    void UnsetFocus() override;
    virtual void SetInitialValues();

    void SetDataObject(FEString* string);
    void InitSliderObjects(const char* pkg_name, const char* name);
    void SetSliderValues(float min, float max, float inc, float cur) {
        Slider.InitValues(min, max, inc, cur, 160.0f);
    }
    float GetValue() { return Slider.GetValue(); }
    void SetValue(float val) { Slider.SetValue(val); }
    void Increment() { Slider.Increment(); }
    void Decrement() { Slider.Decrement(); }
    void DrawSlider() { Slider.Draw(); }
    void ToggleSlider(bool on);
    void UpdateSlider(unsigned int msg);
    float GetVertOffset();
    void SetVertOffset(bool vertOffset);
};

#endif
