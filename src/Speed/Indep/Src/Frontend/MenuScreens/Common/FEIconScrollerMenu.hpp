#ifndef FEICONSCROLLERMENU_HPP
#define FEICONSCROLLERMENU_HPP

#include <types.h>

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Misc/Point.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

#define FEMSG_REFRESH_WIDGETS 0x92b703b5 // :10
#define FEMSG_RESET_SCREEN 0xaf0bbd92    // :11
#define FEMSG_BEGIN_INPUT 0x81017864     // :12
#define FEMSG_INIT_STARTED 0x1265ece9    // :13
#define TEXT_OPTION_LENGTH 64            // : 45

// File: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp
// total size: 0x5C
// Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:131
class IconOption : public bTNode<IconOption> {
  public:
    IconOption(uint32 tex_hash, uint32 name_hash, uint32 desc_hash);
    virtual ~IconOption() {} // Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:134
    virtual void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) = 0;

    uint32 GetName() {
        return NameHash;
    }
    uint32 GetDesc() {
        return DescHash;
    }

    void StartScale(float scale_to, float duration);

    float GetScaleToPcnt() {
        return fScaleToPcnt;
    }
    float GetScaleStartSecs() {
        return fScaleStartSecs;
    }
    float GetScaleDurSecs() {
        return fScaleDurSecs;
    }
    float GetScaleAtStart() {
        return fScaleAtStart;
    }
    void SetScaleAtStart(float scale) {
        fScaleAtStart = scale;
    }
    bool IsAnimComplete() {
        return bAnimComplete;
    }
    void SetAnimComplete(bool b) {
        bAnimComplete = b;
    }
    bool ReactsImmediately() {
        return bReactImmediately;
    }
    bool IsLocked() {
        return Locked;
    }
    void SetLocked(bool b) {
        Locked = b;
    }
    void SetReactImmediately(bool b) {
        bReactImmediately = b;
    }
    bool IsTutorialAvailable() {
        return bIsTutorialAvailable;
    }
    const char *GetTutorialMovieName() {
        return pTutorialMovieName;
    }
    void SetTutorialMovieName(const char *name) {
        pTutorialMovieName = name;
    }

    void SetFEngObject(FEObject *obj);

    uint32 Item;          // offset 0x8, size 0x4
    FEObject *FEngObject; // offset 0xC, size 0x4
    float XPos;           // offset 0x10, size 0x4
    float YPos;           // offset 0x14, size 0x4
    uint32 OriginalColor; // offset 0x18, size 0x4
    bool IsGreyOut;       // offset 0x1C, size 0x1
    bool IsFlashable;     // offset 0x20, size 0x1
    bool Locked;          // offset 0x24, size 0x1
    float OrigWidth;      // offset 0x28, size 0x4
    float OrigHeight;     // offset 0x2C, size 0x4

  private:
    uint32 NameHash;                // offset 0x30, size 0x4
    uint32 DescHash;                // offset 0x34, size 0x4
    float fScaleToPcnt;             // offset 0x38, size 0x4
    float fScaleStartSecs;          // offset 0x3C, size 0x4
    float fScaleDurSecs;            // offset 0x40, size 0x4
    float fScaleAtStart;            // offset 0x44, size 0x4
    bool bAnimComplete;             // offset 0x48, size 0x1
    bool bReactImmediately;         // offset 0x4C, size 0x1
    bool bIsTutorialAvailable;      // offset 0x50, size 0x1
    const char *pTutorialMovieName; // offset 0x54, size 0x4
};

// total size: 0x5C
// Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:254
class FEScrollyBookEnd : public IconOption {
  public:
    FEScrollyBookEnd(uint32 tex_hash) : IconOption(tex_hash, 0, 0) {}
    ~FEScrollyBookEnd() override {} // Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:258

    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:262
// Range: 0x8017D56C -> 0x8017D570
// Overrides: IconOption
inline void FEScrollyBookEnd::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override {}

// total size: 0x38
class IconPanel {
  public:
    IconPanel(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region, bool wrap);
    virtual ~IconPanel() {}
    virtual void Update();
    virtual FEImage *AddOption(IconOption *option);
    virtual void RemoveAll() {
        while (Options.GetHead() != Options.EndOfList()) {
            IconOption *node = Options.GetHead();
            node->Remove();
            delete node;
        }
        iIndexToAdd = 1;
    }
    virtual void Act(uint32 data, FEObject *obj, uint32 param1, uint32 param2);
    virtual IconOption *GetHead() {
        return Options.GetHead();
    }
    IconOption *GetCurrentOption() {
        return pCurrentNode;
    }
    FEObject *GetMaster() {
        return pMaster;
    }
    IconOption *GetOption(int to_find);
    bool AtHead() {
        return IsHead(pCurrentNode);
    }
    bool AtTail() {
        return IsTail(pCurrentNode);
    }
    virtual bool IsHead(IconOption *option) {
        return option == Options.GetHead();
    }
    virtual bool IsTail(IconOption *option) {
        return option == Options.GetTail();
    }
    virtual bool IsEndOfList(IconOption *opt) {
        return opt == Options.EndOfList();
    }
    uint32 GetCurrentDesc() {
        if (this->pCurrentNode != nullptr) {
            return pCurrentNode->GetDesc();
        }
        return 0;
    }
    uint32 GetCurrentName() {
        if (this->pCurrentNode != nullptr) {
            return pCurrentNode->GetName();
        }
        return 0;
    }
    bool CurrentReactsImmediately() {
        if (this->pCurrentNode != nullptr) {
            return pCurrentNode->ReactsImmediately();
        }
        return false;
    }
    int GetIndexToAdd() {
        return iIndexToAdd;
    }
    int GetCurrentIndex() {
        if (pCurrentNode) {
            return GetOptionIndex(pCurrentNode);
        }
        return 0;
    }
    virtual int GetOptionIndex(IconOption *to_find);
    void ScrollNext() {
        if (bWrap) {
            ScrollWrapped(eSD_NEXT);
        } else {
            Scroll(eSD_NEXT);
        }
    }
    void ScrollPrev() {
        if (bWrap) {
            ScrollWrapped(eSD_PREV);
        } else {
            Scroll(eSD_PREV);
        }
    }
    virtual bool SetSelection(IconOption *option);
    bool JustScrolled() {
        return bJustScrolled;
    }
    virtual void SetInitialPos();
    void SetWrap(bool wrap) {
        bWrap = wrap;
    }
    void SetReactToInput(bool react) {
        bReactToInput = react;
    }
    bool IsHorizontal() {
        return bHorizontal;
    }
    bool ReactsToInput() {
        return bReactToInput;
    }

  protected:
    virtual void Scroll(eScrollDir dir);
    virtual void ScrollWrapped(eScrollDir dir);

  private:
    void AnimateList();
    void AnimateSelected(float &list_width, float &list_height);
    void ResizeList(float list_width, float list_height);

  protected:
    bTList<IconOption> Options; // offset 0x0, size 0x8
    IconOption *pCurrentNode;   // offset 0x8, size 0x4
    FEObject *pMaster;          // offset 0xC, size 0x4
    FEObject *pScrollRegion;    // offset 0x10, size 0x4
    const char *pPackageName;   // offset 0x14, size 0x4
    const char *pButtonName;    // offset 0x18, size 0x4
    float fIconSpacing;         // offset 0x1C, size 0x4
    int iIndexToAdd;            // offset 0x20, size 0x4
    bool bWrap;                 // offset 0x24, size 0x1
    bool bHorizontal;           // offset 0x28, size 0x1
    bool bJustScrolled;         // offset 0x2C, size 0x1
    bool bReactToInput;         // offset 0x30, size 0x1
};

// total size: 0x11C
class IconScroller : public IconPanel {
  public:
    IconScroller(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region, float width);
    ~IconScroller() override {}

    void Update() override;
    virtual void AddInitialBookEnds();
    FEImage *AddOption(IconOption *option) override;
    virtual void SetInitialPos(int index);
    bool SetSelection(IconOption *option) override;
    void RemoveAll() override;
    void DelayUpdate() {
        bDelayUpdate = true;
    }
    void SetAllowFade(bool allow) {
        bAllowColorAnim = allow;
    }
    void StartFadeIn() {
        bFadingIn = true;
        bDelayUpdate = false;
        bFadingOut = false;
        fCurFadeTime = 0.0f;
    }
    void StartFadeOut() {
        bFadingIn = false;
        fCurFadeTime = fMaxFadeTime;
        bFadingOut = true;
    }
    void SetIdleColor(uint32 color) {
        IdleColor = color;
    }
    void SetFadeColor(uint32 color) {
        FadeColor = color;
    }
    FEScrollBar *GetScrollBar() {
        return &ScrollBar;
    }
    IconOption *GetHead() override {
        return static_cast<IconOption *>(HeadBookEnd->GetNext());
    }
    bool IsHead(IconOption *option) override {
        return option == static_cast<IconOption *>(HeadBookEnd->GetNext());
    }
    bool IsTail(IconOption *option) override {
        return option == static_cast<IconOption *>(TailBookEnd->GetPrev());
    }
    bool IsEndOfList(IconOption *opt) override {
        return opt == TailBookEnd || opt == HeadBookEnd;
    }
    bool IsInitialized() {
        return bInitialized;
    }
    void SetInitialized() {
        bInitialized = true;
    }
    int GetOptionIndex(IconOption *to_find) override;
    int CountElements() {
        return Options.CountElements();
    }
    bool IsEmpty() {
        return Options.CountElements() == iNumBookEnds;
    }

  protected:
    void Scroll(eScrollDir dir) override;
    void ScrollWrapped(eScrollDir dir) override;
    void ClipEdges(IconOption *option, float pos);
    float Scale(float x, float center, float scroll_size, float thumb_size);
    void PositionOption(IconOption *option);
    void UpdateFade(IconOption *option, float scale);
    void UpdateArrows();
    void PulseSelected();

    IconOption *HeadBookEnd;                // offset 0x38, size 0x4
    IconOption *TailBookEnd;                // offset 0x3C, size 0x4
    FEScrollBar ScrollBar;                  // offset 0x40, size 0x64
    tCubic1D AnimateCubic;                  // offset 0xA4, size 0x2C
    eScrollerAlignment AlignmentToSelected; // offset 0xD0, size 0x4
    int iNumBookEnds;                       // offset 0xD4, size 0x4
    int iNumVisible;                        // offset 0xD8, size 0x4
    int iCurSelectedIndex;                  // offset 0xDC, size 0x4
    float fWidth;                           // offset 0xE0, size 0x4
    float fHeight;                          // offset 0xE4, size 0x4
    float fXCenter;                         // offset 0xE8, size 0x4
    float fYCenter;                         // offset 0xEC, size 0x4
    float fPulseState;                      // offset 0xF0, size 0x4
    float fCurrentAddPos;                   // offset 0xF4, size 0x4
    float fCurFadeTime;                     // offset 0xF8, size 0x4
    float fMaxFadeTime;                     // offset 0xFC, size 0x4
    bool bAllowColorAnim;                   // offset 0x100, size 0x1
    bool bFadingIn;                         // offset 0x104, size 0x1
    bool bFadingOut;                        // offset 0x108, size 0x1
    bool bInitialized;                      // offset 0x10C, size 0x1
    bool bDelayUpdate;                      // offset 0x110, size 0x1
    uint32 IdleColor;                       // offset 0x114, size 0x4
    uint32 FadeColor;                       // offset 0x118, size 0x4
};

// total size: 0x16C
// Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:538
class IconScrollerMenu : public MenuScreen {
  public:
    IconScrollerMenu(ScreenConstructorData *sd); // Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:543

    ~IconScrollerMenu() override {} // Decl: speed/indep/src/frontend/menuscreens/common/FEIconScrollerMenu.hpp:545

    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

    void DelayFadeIn() {
        bFadeInIconsImmediately = false;
        Options.DelayUpdate();
    }

    void SetInitialOption(int index) {
        if (bFadeInIconsImmediately) {
            Options.StartFadeIn();
        }
        Options.SetInitialPos(index);
    }

    void StartInput() {
        Options.SetReactToInput(true);
    }

    void StopInput() {
        Options.SetReactToInput(false);
    }

    void StorePrevNotification(uint32 msg, FEObject *pobj, uint32 param1, uint32 param2);

  protected:
    virtual void RefreshHeader();
    virtual void Setup() {}

    void AddOption(IconOption *option);

  protected:
    IconScroller Options;         // offset 0x2C, size 0x11C
    bool bWasLeftMouseDown;       // offset 0x148, size 0x1
    bool bFadeInIconsImmediately; // offset 0x14C, size 0x1
    FEString *pOptionName;        // offset 0x150, size 0x4
    FEString *pOptionNameShadow;  // offset 0x154, size 0x4
    FEString *pOptionDesc;        // offset 0x158, size 0x4
    uint32 PrevButtonMessage;     // offset 0x15C, size 0x4
    FEObject *PrevButtonObj;      // offset 0x160, size 0x4
    uint32 PrevParam1;            // offset 0x164, size 0x4
    uint32 PrevParam2;            // offset 0x168, size 0x4
};

#endif
