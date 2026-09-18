#ifndef FRONTEND_MENUSCREENS_COMMON_FEWIDGET_H
#define FRONTEND_MENUSCREENS_COMMON_FEWIDGET_H

#include <types.h>

#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:30
// r71e: el typedef-anonimo hacia que el "enum eScrollDir" de las
// declaraciones (feArrayScrollerMenu.hpp:187) creara un TAG distinto en
// MSVC (C2664 eScrollDir->eScrollDir); GCC los fusiona. El original
// tenia el enum con nombre. Cero emision: sin riesgo de codegen.
enum eScrollDir { eSD_PREV = -1, eSD_NEXT = 1, eSD_PAGE_PREV = -10000, eSD_PAGE_NEXT = 10000, eSD_NONE = 10001 };

// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:41
typedef enum { eSA_TOP = 0, eSA_MIDDLE = 1, eSA_BOTTOM = 2, eSA_LEFT = 0, eSA_RIGHT = 2 } eScrollerAlignment;

// total size: 0x34
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:66
class FEWidget : public bTNode<FEWidget> {
  public:
    FEWidget(FEObject *backing, bool enabled, bool hidden);
    virtual ~FEWidget() {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:73

    virtual void Act(const char *parent_pkg, uint32 data) = 0;
    virtual void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) = 0;
    virtual void Draw() = 0;
    virtual void Position() = 0;
    virtual void Show() = 0;
    virtual void Hide() = 0;
    virtual void Enable() {
        bEnabled = true;
    }
    virtual void Disable() {
        bEnabled = false;
    }
    bool IsEnabled() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:102
        return bEnabled;
    }
    bool IsHidden() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:103
        return bHidden;
    }
    virtual void SetFocus(const char *parent_pkg) = 0;
    virtual void UnsetFocus() = 0;
    void GetTopLeft(bVector2 &top_left) {
        top_left.x = vTopLeft.x;
        top_left.y = vTopLeft.y;
    }
    float GetTopLeftX() {
        return vTopLeft.x;
    }
    float GetTopLeftY() {
        return vTopLeft.y;
    }
    void GetSize(bVector2 &size) {
        size.x = vSize.x;
        size.y = vSize.y;
    }
    float GetWidth() {
        return vSize.x;
    }
    float GetHeight() {
        return vSize.y;
    }
    virtual void SetPos(bVector2 &pos) {
        SetTopLeft(pos);
    }
    virtual void SetPosX(float x) {
        SetTopLeftX(x);
    }
    virtual void SetPosY(float y) {
        SetTopLeftY(y);
    }
    void SetTopLeft(bVector2 &top_left) {
        vTopLeft.x = top_left.x;
        vTopLeft.y = top_left.y;
    }
    void SetTopLeftX(float x) {
        vTopLeft.x = x;
    }
    void SetTopLeftY(float y) {
        vTopLeft.y = y;
    }
    void SetSize(bVector2 &size) {
        vSize = size;
    }
    void SetWidth(float width) {
        vSize.x = width;
    }
    void SetHeight(float height) {
        vSize.y = height;
    }
    void SetBacking(FEObject *obj) {
        pBacking = obj;
    }
    FEObject *GetBacking() {
        return pBacking;
    }
    void GetBackingOffset(bVector2 &offset) {
        offset.x = vBackingOffset.x;
        offset.y = vBackingOffset.y;
    }
    float GetBackingOffsetX() {
        return vBackingOffset.x;
    }
    float GetBackingOffsetY() {
        return vBackingOffset.y;
    }
    void SetBackingOffset(bVector2 &offset) {
        vBackingOffset = offset;
    }
    void SetBackingOffset(float x, float y) {
        vBackingOffset.x = x;
        vBackingOffset.y = y;
    }
    void SetBackingOffsetX(float x) {
        vBackingOffset.x = x;
    }
    void SetBackingOffsetY(float y) {
        vBackingOffset.y = y;
    }
    bool MovedLastUpdate() {
        return bMovedLastUpdate;
    }

  private:
    bVector2 vTopLeft;       // offset 0x8, size 0x8
    bVector2 vSize;          // offset 0x10, size 0x8
    bVector2 vBackingOffset; // offset 0x18, size 0x8
    FEObject *pBacking;      // offset 0x20, size 0x4
    bool bEnabled;           // offset 0x24, size 0x1, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:152
    bool bHidden;            // offset 0x28, size 0x1, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:155

  protected:
    bool bMovedLastUpdate; // offset 0x2C, size 0x1
};

// total size: 0x40
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:163
class FEButtonWidget : public FEWidget {
  public:
    FEButtonWidget(bool enabled);
    ~FEButtonWidget() override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:167
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Position() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    FEString *GetTitleObject() {
        return pTitle;
    }
    void SetTitleObject(FEString *string) {
        pTitle = string;
    }
    void SetPos(bVector2 &pos) override {
        SetPosX(pos.x);
        SetPosY(pos.y);
    }
    void GetMaxTitleSize(bVector2 &size) {
        size = vMaxTitleSize;
    }
    float GetMaxTitleWidth() {
        return vMaxTitleSize.x;
    }
    float GetMaxTitleHeight() {
        return vMaxTitleSize.y;
    }
    void SetMaxTitleSize(bVector2 &size) {
        vMaxTitleSize.x = size.x;
        vMaxTitleSize.y = size.y;
    }
    void SetMaxTitleWidth(float width) {
        vMaxTitleSize.x = width;
    }
    void SetMaxTitleHeight(float height) {
        vMaxTitleSize.y = height;
    }

  private:
    FEString *pTitle;       // offset 0x34, size 0x4
    bVector2 vMaxTitleSize; // offset 0x38, size 0x8
};

// total size: 0x54
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:195
class FEStatWidget : public FEWidget {
  public:
    FEStatWidget(bool enabled);
    ~FEStatWidget() override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:198
    void Act(const char *parent_pkg, uint32 data) override {}
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override {}
    void Position() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:205
    void UnsetFocus() override {}                     // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:206
    FEString *GetTitleObject() {
        return pTitle;
    }
    FEString *GetDataObject() {
        return pData;
    }
    void SetTitleObject(FEString *string) {
        pTitle = string;
    }
    void SetDataObject(FEString *string) {
        pData = string;
    }
    void SetPos(bVector2 &pos) override {
        SetPosX(pos.x);
        SetPosY(pos.y);
    }
    void SetPosX(float x) override;
    void SetPosY(float y) override;
    void GetDataPos(bVector2 &pos) {
        pos.x = vDataPos.x;
        pos.y = vDataPos.y;
    }
    float GetDataPosX() {
        return vDataPos.x;
    }
    float GetDataPosY() {
        return vDataPos.y;
    }
    void SetDataPos(bVector2 &pos) {
        vDataPos.x = pos.x;
        vDataPos.y = pos.y;
    }
    void SetDataPosX(float x) {
        vDataPos.x = x;
    }
    void SetDataPosY(float y) {
        vDataPos.y = y;
    }
    void GetMaxTitleSize(bVector2 &size) {
        size.x = vMaxTitleSize.x;
        size.y = vMaxTitleSize.y;
    }
    float GetMaxTitleWidth() {
        return vMaxTitleSize.x;
    }
    float GetMaxTitleHeight() {
        return vMaxTitleSize.y;
    }
    void SetMaxTitleSize(bVector2 &size) {
        vMaxTitleSize.x = size.x;
        vMaxTitleSize.y = size.y;
    }
    void SetMaxTitleWidth(float width) {
        vMaxTitleSize.x = width;
    }
    void SetMaxTitleHeight(float height) {
        vMaxTitleSize.y = height;
    }
    void GetMaxDataSize(bVector2 &size) {
        size.x = vMaxDataSize.x;
        size.y = vMaxDataSize.y;
    }
    float GetMaxDataWidth() {
        return vMaxDataSize.x;
    }
    float GetMaxDataHeight() {
        return vMaxDataSize.y;
    }
    void SetMaxDataSize(bVector2 &size) {
        vMaxDataSize.x = size.x;
        vMaxDataSize.y = size.y;
    }
    void SetMaxDataWidth(float x) {
        vMaxDataSize.x = x;
    }
    void SetMaxDataHeight(float y) {
        vMaxDataSize.y = y;
    }

  private:
    FEString *pTitle;       // offset 0x34, size 0x4
    FEString *pData;        // offset 0x38, size 0x4
    bVector2 vMaxTitleSize; // offset 0x3C, size 0x8
    bVector2 vMaxDataSize;  // offset 0x44, size 0x8
    bVector2 vDataPos;      // offset 0x4C, size 0x8
};

// total size: 0x64
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:279
class FEToggleWidget : public FEStatWidget {
  public:
    FEToggleWidget(bool enabled);
    ~FEToggleWidget() override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:282
    virtual void Act(const char *parent_pkg, uint32 data) = 0;
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Position() override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    virtual void BlinkArrows(uint32 data);
    FEImage *GetLeftImage() {
        return pLeftImage;
    }
    FEImage *GetRightImage() {
        return pRightImage;
    }
    void SetLeftImage(FEImage *img) {
        pLeftImage = img;
    }
    void SetRightImage(FEImage *img) {
        pRightImage = img;
    }
    bool Update(uint32 msg) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:300
        bMovedLastUpdate = true;
        BlinkArrows(msg);
        Draw();
        return true;
    }
    uint32 GetEnableScript() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:302
        return EnableScript;
    }

    uint32 GetDisableScript() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:303
        return DisableScript;
    }

    void SetEnableScript(uint32 script) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:304
        EnableScript = script;
    }

    void SetDisableScript(uint32 script) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:305
        DisableScript = script;
    }
    void SetScript(uint32 script);

  private:
    FEImage *pLeftImage;  // offset 0x54, size 0x4
    FEImage *pRightImage; // offset 0x58, size 0x4
    uint32 EnableScript;  // offset 0x5C, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:311
    uint32 DisableScript; // offset 0x60, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:312
};

// total size: 0x68
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:328
class FEToggleImageWidget : public FEToggleWidget {
  public:
    FEToggleImageWidget(bool enabled);
    ~FEToggleImageWidget() override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:331
    void Position() override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetDataImage(FEImage *img) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:346
        pDataImage = img;
    }
    FEImage *GetDataImage() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:348
        return pDataImage;
    }

  private:
    FEImage *pDataImage; // offset 0x64, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:352
};

// total size: 0xA4
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:358
class FESliderWidget : public FEToggleWidget {

  public:
    FESliderWidget(bool enabled);
    ~FESliderWidget() override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:361
    void Position() override;
    void Show() override;
    void Hide() override;
    void Enable() override;
    void Disable() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetDataObject(FEString *string) {}
    void InitSliderObjects(const char *pkg_name, const char *name) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:378
        Slider.InitObjects(pkg_name, name);
    }
    void SetSliderValues(float min, float max, float inc, float cur) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:382
        Slider.InitValues(min, max, inc, cur, 160.0f);
    }
    float GetValue() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:384
        return Slider.GetValue();
    }
    void SetValue(float val) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:387
        Slider.SetValue(val);
    }
    void Increment() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:388
        Slider.Increment();
    }
    void Decrement() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:389
        Slider.Decrement();
    }
    void DrawSlider() {
        Slider.Draw();
    }
    void ToggleSlider(bool on) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:391
        Slider.ToggleVisible(on);
    }
    void UpdateSlider(uint32 msg);
    float GetVertOffset() {}
    void SetVertOffset(bool vertOffset) {}
    virtual void SetInitialValues() = 0;

  private:
    cSlider Slider;    // offset 0x64, size 0x3C, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:400
    float fVertOffset; // offset 0xA0, size 0x4
};

// total size: 0x194
// Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:411
class FEInputWidget : public FEStatWidget {
  public:
    FEInputWidget(uint32 max_input_length, const char *init_text, uint32 edit_mode, bool enabled);
    ~FEInputWidget() override {} // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:419

    void Act(const char *parent_pkg, uint32 data) override;
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetInputFocus();
    void SetInputText(const char *text) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:437
        bStrNCpy(InputText, text, 0x9b);
    }
    const char *GetInputText() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:438
        return InputText;
    }
    void SetEditMode(uint32 mode) { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:441
        EditMode = mode;
    }
    uint32 GetEditMode() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:442
        return EditMode;
    }
    void SetTitle(const char *text);
    const char *GetTitle() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:439
        return Title;
    }
    uint32 GetMaxInputLength() { // Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:444
        return MaxInputLength;
    }

  private:
    char InputText[156];   // offset 0x54, size 0x9C, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:451
    char Title[156];       // offset 0xF0, size 0x9C
    uint32 MaxInputLength; // offset 0x18C, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:453
    uint32 EditMode;       // offset 0x190, size 0x4, Decl: speed/indep/src/frontend/menuscreens/common/feWidget.hpp:454
};

// total size: 0x64
class FEDateWidget : public FEStatWidget {
  public:
    FEDateWidget(bool enabled);
    ~FEDateWidget() override {}
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    virtual const char *GetHeaderText() {}
    virtual int GetMaxYear() {}
    void SetDate(int d, int m, int y) {
        mDay = d;
        mMonth = m;
        mYear = y;
    }
    void GetDate(int *pDay, int *pMonth, int *pYear) {
        pDay = &mDay;
        pMonth = &mMonth;
        pYear = &mYear;
    }
    void SetPackageName(const char *pName) {
        mPkgName = pName;
    }

  private:
    int mDay;             // offset 0x54, size 0x4
    int mMonth;           // offset 0x58, size 0x4
    int mYear;            // offset 0x5C, size 0x4
    const char *mPkgName; // offset 0x60, size 0x4
};

#endif
