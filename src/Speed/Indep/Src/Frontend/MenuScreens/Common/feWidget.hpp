#ifndef __FEWIDGET_HPP__
#define __FEWIDGET_HPP__

#include <types.h>

#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/Slider.hpp"

// Decl: 30
enum eScrollDir {
    eSD_PREV = -1,
    eSD_NEXT = 1,
    eSD_PAGE_PREV = -10000,
    eSD_PAGE_NEXT = 10000,
    eSD_NONE = 10001,
};

// Decl: 41
enum eScrollerAlignment {
    eSA_TOP = 0,
    eSA_MIDDLE = 1,
    eSA_BOTTOM = 2,
    eSA_LEFT = 0,
    eSA_RIGHT = 2,
};

// total size: 0x34
// Decl: 66
class FEWidget : public bTNode<FEWidget> {
  public:
    FEWidget(FEObject *backing, bool enabled, bool hidden);
    virtual ~FEWidget() {} // Decl: 73

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
    bool IsEnabled() { // Decl: 102
        return bEnabled;
    }
    bool IsHidden() { // Decl: 103
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
    bool bEnabled;           // offset 0x24, size 0x1, Decl: 152
    bool bHidden;            // offset 0x28, size 0x1, Decl: 155

  protected:
    bool bMovedLastUpdate; // offset 0x2C, size 0x1
};

// total size: 0x40
// Decl: 163
class FEButtonWidget : public FEWidget {
  public:
    FEButtonWidget(bool enabled);
    ~FEButtonWidget() override {} // Decl: 167
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

  private:
    FEString *pTitle;       // offset 0x34, size 0x4
    bVector2 vMaxTitleSize; // offset 0x38, size 0x8
};

// total size: 0x54
// Decl: 195
class FEStatWidget : public FEWidget {
  public:
    FEStatWidget(bool enabled);
    ~FEStatWidget() override {} // Decl: 198
    void Act(const char *parent_pkg, uint32 data) override {}
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override {}
    void Position() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override {} // Decl: 205
    void UnsetFocus() override {}                     // Decl: 206
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
// Decl: 279
class FEToggleWidget : public FEStatWidget {
  public:
    FEToggleWidget(bool enabled);
    ~FEToggleWidget() override {} // Decl: 282
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
    bool Update(uint32 msg) { // Decl: 300
        bMovedLastUpdate = true;
        BlinkArrows(msg);
        return true;
    }
    uint32 GetEnableScript() { // Decl: 302
        return EnableScript;
    }

    uint32 GetDisableScript() { // Decl: 303
        return DisableScript;
    }

    void SetEnableScript(uint32 script) { // Decl: 304
        EnableScript = script;
    }

    void SetDisableScript(uint32 script) { // Decl: 305
        DisableScript = script;
    }
    void SetScript(uint32 script);

  private:
    FEImage *pLeftImage;  // offset 0x54, size 0x4
    FEImage *pRightImage; // offset 0x58, size 0x4
    uint32 EnableScript;  // offset 0x5C, size 0x4, Decl: 311
    uint32 DisableScript; // offset 0x60, size 0x4, Decl: 312
};

// total size: 0x68
// Decl: 328
class FEToggleImageWidget : public FEToggleWidget {
  public:
    FEToggleImageWidget(bool enabled);
    ~FEToggleImageWidget() override {} // Decl: 331
    void Position() override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetDataImage(FEImage *img) { // Decl: 346
        pDataImage = img;
    }
    FEImage *GetDataImage() { // Decl: 348
        return pDataImage;
    }

  private:
    FEImage *pDataImage; // offset 0x64, size 0x4, Decl: 352
};

// total size: 0xA4
// Decl: 358
class FESliderWidget : public FEToggleWidget {
  public:
    FESliderWidget(bool enabled);
    ~FESliderWidget() override {} // Decl: 361
    virtual void SetInitialValues() = 0;
    void Position() override;
    void Show() override;
    void Hide() override;
    void Enable() override;
    void Disable() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetDataObject(FEString *string) {}
    void InitSliderObjects(const char *pkg_name, const char *name) { // Decl: 378
        Slider.InitObjects(pkg_name, name);
    }
    void SetSliderValues(float min, float max, float inc, float cur) { // Decl: 382
        Slider.InitValues(min, max, inc, cur, 160.0f);
    }
    float GetValue() { // Decl: 384
        return Slider.GetValue();
    }
    void SetValue(float val) { // Decl: 387
        Slider.SetValue(val);
    }
    void Increment() { // Decl: 388
        Slider.Increment();
    }
    void Decrement() { // Decl: 389
        Slider.Decrement();
    }
    void DrawSlider() {
        Slider.Draw();
    }
    void ToggleSlider(bool on) { // Decl: 391
        Slider.ToggleVisible(on);
    }
    void UpdateSlider(uint32 msg);
    float GetVertOffset() {
        return fVertOffset;
    }
    void SetVertOffset(bool vertOffset) {}

  private:
    cSlider Slider;    // offset 0x64, size 0x3C, Decl: 400
    float fVertOffset; // offset 0xA0, size 0x4
};

// total size: 0x194
// Decl: 411
class FEInputWidget : public FEStatWidget {
  public:
    FEInputWidget(uint32 max_input_length, const char *init_text, uint32 edit_mode, bool enabled);
    ~FEInputWidget() override {} // Decl: 419

    void Act(const char *parent_pkg, uint32 data) override;
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
    void SetInputFocus();
    void SetInputText(const char *text) { // Decl: 437
        bStrNCpy(InputText, text, sizeof(InputText) - 1);
    }
    const char *GetInputText() { // Decl: 438
        return InputText;
    }
    void SetEditMode(uint32 mode) { // Decl: 441
        EditMode = mode;
    }
    uint32 GetEditMode() { // Decl: 442
        return EditMode;
    }
    void SetTitle(const char *text);
    const char *GetTitle() { // Decl: 439
        return Title;
    }
    uint32 GetMaxInputLength() { // Decl: 444
        return MaxInputLength;
    }

  private:
    char InputText[156];   // offset 0x54, size 0x9C, Decl: 451
    char Title[156];       // offset 0xF0, size 0x9C
    uint32 MaxInputLength; // offset 0x18C, size 0x4, Decl: 453
    uint32 EditMode;       // offset 0x190, size 0x4, Decl: 454
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

// total size: 0x64
// Decl: 517
class FEScrollBar {
  public:
    FEScrollBar(const char *parent_pkg, const char *name, bool vert, bool resize,
                bool arrows_only); // Decl: 523

    ~FEScrollBar() {} // Decl: 524

    void Update(int num_view_items, int num_list_items, int view_head_index,
                int selected_item); // Decl: 527
    void UpdateMouse();
    void SetGroupVisible(bool visible);      // Decl: 545
    void SetArrow1Visibility(bool visible) { // Decl: 550
        this->SetArrowVisibility(1, visible);
    }
    void SetArrow2Visibility(bool visible) { // Decl: 551
        this->SetArrowVisibility(2, visible);
    }
    void SetBackingVisibility(bool visible); // Decl: 553
    void SetArrow1Dim(bool dim);             // Decl: 555
    void SetArrow2Dim(bool dim);             // Decl: 556
    bool IsVisible() {                       // Decl: 530
        return bVisible;
    }

  private:
    void SetPosResized(int num_view_items, int num_list_items,
                       int view_head_index); // Decl: 539
    void SetPosNonResized(int num_view_items, int num_list_items,
                          int view_head_index); // Decl: 540
    void UpdateArrowsMouse();
    void UpdateHandleMouse();
    void UpdateBackingMouse();
    void SetVisible(FEObject *obj);                       // Decl: 546
    void SetInvisible(FEObject *obj);                     // Decl: 547
    void SetArrowVisibility(int arrow_num, bool visible); // Decl: 549

    bool bVertical;              // offset 0x0, size 0x1, Decl: 559
    bool bResizeHandle;          // offset 0x4, size 0x1, Decl: 560
    bool bHandleGrabbed;         // offset 0x8, size 0x1, Decl: 561
    bool bArrowsOnly;            // offset 0xC, size 0x1, Decl: 562
    bool bVisible;               // offset 0x10, size 0x1, Decl: 563
    bVector2 vGrabbedPos;        // offset 0x14, size 0x8, Decl: 565
    bVector2 vCurPos;            // offset 0x1C, size 0x8, Decl: 566
    bVector2 vGrabOffset;        // offset 0x24, size 0x8, Decl: 567
    bVector2 vBackingPos;        // offset 0x2C, size 0x8, Decl: 568
    bVector2 vBackingSize;       // offset 0x34, size 0x8, Decl: 569
    bVector2 vHandleMinSize;     // offset 0x3C, size 0x8, Decl: 570
    float fSegSize;              // offset 0x44, size 0x4, Decl: 572
    Timer ScrollTime;            // offset 0x48, size 0x4
    FEObject *pBacking;          // offset 0x4C, size 0x4, Decl: 574
    FEObject *pHandle;           // offset 0x50, size 0x4, Decl: 575
    FEObject *pFirstArrow;       // offset 0x54, size 0x4, Decl: 576
    FEObject *pSecondArrow;      // offset 0x58, size 0x4, Decl: 577
    FEObject *pFirstBackingEnd;  // offset 0x5C, size 0x4, Decl: 578
    FEObject *pSecondBackingEnd; // offset 0x60, size 0x4, Decl: 579
};

// total size: 0x54
// Decl: 615
class CTextScroller {
  public:
    CTextScroller();  // Decl: 617
    ~CTextScroller(); // Decl: 618

    bool PreAllocate(int BytesToAllocate); // Decl: 621

    void Initialise(MenuScreen *pOwner, int ViewWidth, int ViewLines, char *pTextDisplayNameTempl,
                    FEngFont *pFont); // Decl: 624

    void UseScrollBar(FEScrollBar *pScrollBar) { // Decl: 627
        m_pScrollBar = pScrollBar;
    }

    void SetScrollMessages(uint32 ScrollDownMsg, uint32 ScrollUpMsg) {} // Decl: 630

    void SetText(int16 *pText);             // Decl: 633
    void SetTextHash(uint32 language_hash); // Decl: 634

    void Scroll(int Amount); // Decl: 637

    bool HandleNotificationMessage(uint32 Msg); // Decl: 640

    int GetNumVisibleLines() { // Decl: 643
        return m_ViewVisibleLines;
    }
    int GetNumLines() { // Decl: 644
        return m_NumAddedLines;
    }
    int GetTopLine() { // Decl: 645
        return m_TopLine;
    }

  private:
    uint32 CalculateMemorySize(int NumLines, int NumChars) {} // Decl: 652

    void Display(int32 TopLine);            // Decl: 654
    void AddLine(int16 *pLine, int32 Size); // Decl: 655

    void WordWrapCountLinesAndChars(int16 *pTextStart, int16 *pTextEnd, int &NumLines,
                                    int &NumChars); // Decl: 657
    int WordWrapAddLines(int16 *pTextStart, int16 *pTextEnd, bool bCountOnly,
                         int *pNumCharsOut); // Decl: 658

    int16 *FindCR(int16 *pText);  // Decl: 660
    int16 *FindEND(int16 *pText); // Decl: 661

    void UpdateScrollBar(); // Decl: 663

    MenuScreen *m_pOwner;           // offset 0x0, size 0x4, Decl: 668
    FEngFont *m_pFont;              // offset 0x4, size 0x4, Decl: 669
    FEScrollBar *m_pScrollBar;      // offset 0x8, size 0x4, Decl: 672
    char m_TextBoxNameTemplate[32]; // offset 0xC, size 0x20, Decl: 676
    int m_ViewWidth;                // offset 0x2C, size 0x4, Decl: 679
    int m_ViewVisibleLines;         // offset 0x30, size 0x4, Decl: 680
    int m_NumAddedLines;            // offset 0x34, size 0x4, Decl: 683
    int16 **m_pLines;               // offset 0x38, size 0x4, Decl: 684
    char *m_pRawDataBlock;          // offset 0x3C, size 0x4, Decl: 687
    uint32 m_DataBlockSize;         // offset 0x40, size 0x4, Decl: 688
    uint32 m_DataBlockCurPos;       // offset 0x44, size 0x4, Decl: 689
    int m_TopLine;                  // offset 0x48, size 0x4, Decl: 692
    uint32 m_ScrollDownMsg;         // offset 0x4C, size 0x4, Decl: 695
    uint32 m_ScrollUpMsg;           // offset 0x50, size 0x4, Decl: 695
};

#endif
