#ifndef _ICONSCROLLER
#define _ICONSCROLLER

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconPanel.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

struct tCubic1D;

enum eScrollerAlignment {
    eSA_TOP = 0,
    eSA_MIDDLE = 1,
    eSA_BOTTOM = 2,
    eSA_LEFT = 0,
    eSA_RIGHT = 2,
};

// total size: 0x11C
struct IconScroller : public IconPanel {
    IconOption* HeadBookEnd;                    // offset 0x38, size 0x4
    IconOption* TailBookEnd;                    // offset 0x3C, size 0x4
    FEScrollBar ScrollBar;                      // offset 0x40, size 0x64
    char AnimateCubicData[0x2C];                // offset 0xA4, size 0x2C (tCubic1D)
    eScrollerAlignment AlignmentToSelected;     // offset 0xD0, size 0x4
    int iNumBookEnds;                           // offset 0xD4, size 0x4
    int iNumVisible;                            // offset 0xD8, size 0x4
    int iCurSelectedIndex;                      // offset 0xDC, size 0x4
    float fWidth;                               // offset 0xE0, size 0x4
    float fHeight;                              // offset 0xE4, size 0x4
    float fXCenter;                             // offset 0xE8, size 0x4
    float fYCenter;                             // offset 0xEC, size 0x4
    float fPulseState;                          // offset 0xF0, size 0x4
    float fCurrentAddPos;                       // offset 0xF4, size 0x4
    float fCurFadeTime;                         // offset 0xF8, size 0x4
    float fMaxFadeTime;                         // offset 0xFC, size 0x4
    bool bAllowColorAnim;                       // offset 0x100, size 0x1
    bool bFadingIn;                             // offset 0x104, size 0x1
    bool bFadingOut;                            // offset 0x108, size 0x1
    bool bInitialized;                          // offset 0x10C, size 0x1
    bool bDelayUpdate;                          // offset 0x110, size 0x1
    unsigned int IdleColor;                     // offset 0x114, size 0x4
    unsigned int FadeColor;                     // offset 0x118, size 0x4

    IconScroller() {}
    IconScroller(const char* pkg_name, const char* master, const char* fe_button, const char* scroll_region, float width);
    ~IconScroller() override {}

    void Update() override;
    virtual void AddInitialBookEnds();
    FEImage* AddOption(IconOption* option) override;
    virtual void SetInitialPos(int index);
    bool SetSelection(IconOption* option) override;
    void RemoveAll() override;
    int GetOptionIndex(IconOption* to_find) override;
    void Scroll(eScrollDir dir) override;
    void ScrollWrapped(eScrollDir dir) override;
    void ClipEdges(IconOption* option, float pos);
    float Scale(float x, float center, float scroll_size, float thumb_size);
    void PositionOption(IconOption* option);
    void UpdateFade(IconOption* option, float scale);
    void UpdateArrows();
    void PulseSelected();

    IconOption* GetHead() override;
    bool IsHead(IconOption* option) override;
    bool IsTail(IconOption* option) override;
    bool IsEndOfList(IconOption* opt) override;

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
        bFadingOut = true;
        fCurFadeTime = 0.0f;
    }

    void SetIdleColor(unsigned int color) {
        IdleColor = color;
    }

    void SetFadeColor(unsigned int color) {
        FadeColor = color;
    }

    FEScrollBar* GetScrollBar() {
        return &ScrollBar;
    }

    bool IsInitialized() {
        return bInitialized;
    }

    void SetInitialized() {
        bInitialized = true;
    }

    int CountElements();

    bool IsEmpty();
};

#endif
