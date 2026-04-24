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
protected:
    bVector2 vTopLeft;        // 0x08
    bVector2 vSize;           // 0x10
    bVector2 vBackingOffset;  // 0x18
    FEObject* pBacking;       // 0x20
    bool bEnabled;            // 0x24
    bool bHidden;             // 0x28
    bool bMovedLastUpdate;    // 0x2C

public:
    FEWidget(FEObject* backing, bool enabled, bool hidden);
    virtual ~FEWidget() {}
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

    bool IsEnabled() { return bEnabled; }
    bool IsHidden() { return bHidden; }
    void GetTopLeft(bVector2& top_left) { top_left.x = vTopLeft.x; top_left.y = vTopLeft.y; }
    float GetTopLeftX() { return vTopLeft.x; }
    float GetTopLeftY() { return vTopLeft.y; }
    void GetSize(bVector2& size) { size.x = vSize.x; size.y = vSize.y; }
    float GetWidth() { return vSize.x; }
    float GetHeight() { return vSize.y; }
    void SetTopLeft(bVector2& top_left) { vTopLeft.x = top_left.x; vTopLeft.y = top_left.y; }
    void SetTopLeftX(float x) { vTopLeft.x = x; }
    void SetTopLeftY(float y) { vTopLeft.y = y; }
    void SetSize(bVector2& size) { vSize = size; }
    void SetWidth(float width) { vSize.x = width; }
    void SetHeight(float height) { vSize.y = height; }
    void SetBacking(FEObject* obj) { pBacking = obj; }
    FEObject* GetBacking() { return pBacking; }
    void GetBackingOffset(bVector2& offset) { offset.x = vBackingOffset.x; offset.y = vBackingOffset.y; }
    float GetBackingOffsetX() { return vBackingOffset.x; }
    float GetBackingOffsetY() { return vBackingOffset.y; }
    void SetBackingOffset(bVector2& offset) { vBackingOffset = offset; }
    void SetBackingOffset(float x, float y) { vBackingOffset.x = x; vBackingOffset.y = y; }
    void SetBackingOffsetX(float x) { vBackingOffset.x = x; }
    void SetBackingOffsetY(float y) { vBackingOffset.y = y; }
    bool MovedLastUpdate() { return bMovedLastUpdate; }
};

// 0x40
struct FEButtonWidget : public FEWidget {
    FEString* pTitle;        // 0x34
    bVector2 vMaxTitleSize;  // 0x38

    FEButtonWidget(bool enabled);
    ~FEButtonWidget() override {}

    FEString* GetTitleObject() { return pTitle; }
    void SetTitleObject(FEString* string) { pTitle = string; }
    void SetPos(bVector2& pos) override;
    void GetMaxTitleSize(bVector2& size) { size = vMaxTitleSize; }
    float GetMaxTitleWidth() { return vMaxTitleSize.x; }
    float GetMaxTitleHeight() { return vMaxTitleSize.y; }
    void SetMaxTitleSize(bVector2& size) { vMaxTitleSize.x = size.x; vMaxTitleSize.y = size.y; }
    void SetMaxTitleWidth(float width) { vMaxTitleSize.x = width; }
    void SetMaxTitleHeight(float height) { vMaxTitleSize.y = height; }
    void CheckMouse(const char* parent_pkg, float mouse_x, float mouse_y) override;
    void Position() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char* parent_pkg) override;
    void UnsetFocus() override;
};

// 0x54
struct FEStatWidget : public FEWidget {
protected:
    FEString* pTitle;        // 0x34
    FEString* pData;         // 0x38
    bVector2 vMaxTitleSize;  // 0x3C
    bVector2 vMaxDataSize;   // 0x44
    bVector2 vDataPos;       // 0x4C

public:
    FEStatWidget(bool enabled);
    ~FEStatWidget() override {}
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
    void SetTitleObject(FEString* string) { pTitle = string; }
    void SetDataObject(FEString* string) { pData = string; }
    void GetDataPos(bVector2& pos);
    float GetDataPosX();
    float GetDataPosY();
    void SetDataPos(bVector2& pos) { vDataPos.x = pos.x; vDataPos.y = pos.y; }
    void SetDataPosX(float x) { vDataPos.x = x; }
    void SetDataPosY(float y) { vDataPos.y = y; }
    void GetMaxTitleSize(bVector2& size);
    float GetMaxTitleWidth();
    float GetMaxTitleHeight();
    void GetMaxDataSize(bVector2& size);
    float GetMaxDataWidth();
    float GetMaxDataHeight();
    void SetMaxTitleSize(bVector2& size) { vMaxTitleSize.x = size.x; vMaxTitleSize.y = size.y; }
    void SetMaxTitleWidth(float width);
    void SetMaxTitleHeight(float height);
    void SetMaxDataSize(bVector2& size) { vMaxDataSize.x = size.x; vMaxDataSize.y = size.y; }
    void SetMaxDataWidth(float x);
    void SetMaxDataHeight(float y);
};

// 0x64
struct FEToggleWidget : public FEStatWidget {
protected:
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
    void SetLeftImage(FEImage* img) { pLeftImage = img; }
    void SetRightImage(FEImage* img) { pRightImage = img; }
    bool Update(unsigned int msg) {
        bMovedLastUpdate = true;
        BlinkArrows(msg);
        Draw();
        return true;
    }
    unsigned int GetEnableScript();
    unsigned int GetDisableScript();
    void SetEnableScript(unsigned int script) { EnableScript = script; }
    void SetDisableScript(unsigned int script) { DisableScript = script; }
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
    void InitSliderObjects(const char* pkg_name, const char* name) { Slider.InitObjects(pkg_name, name); }
    void SetSliderValues(float min, float max, float inc, float cur) {
        Slider.InitValues(min, max, inc, cur, 160.0f);
    }
    float GetValue() { return Slider.GetValue(); }
    void SetValue(float val) { Slider.SetValue(val); }
    void Increment() { Slider.Increment(); }
    void Decrement() { Slider.Decrement(); }
    void DrawSlider() { Slider.Draw(); }
    void ToggleSlider(bool on) { Slider.ToggleVisible(on); }
    void UpdateSlider(unsigned int msg);
    float GetVertOffset();
    void SetVertOffset(bool vertOffset);
};

#endif
