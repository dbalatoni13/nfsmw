#ifndef _UIWIDGETMENU
#define _UIWIDGETMENU

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconPanel.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct FEButtonWidget;
struct FEToggleImageWidget;
struct FEInputWidget;
struct FEDateWidget;

// total size: 0x138
struct UIWidgetMenu : public MenuScreen {
    bTList<FEWidget> Options;       // offset 0x2C, size 0x8
    FEWidget* pCurrentOption;       // offset 0x34, size 0x4
    FEWidget* pViewTop;             // offset 0x38, size 0x4
    FEObject* pTitleMaster;         // offset 0x3C, size 0x4
    FEObject* pDataMaster;          // offset 0x40, size 0x4
    FEObject* pPrevButtonObj;       // offset 0x44, size 0x4
    FEString* pDoneText;            // offset 0x48, size 0x4
    FEObject* pDone;                // offset 0x4C, size 0x4
    FEObject* pCursor;              // offset 0x50, size 0x4
    FEScrollBar ScrollBar;          // offset 0x54, size 0x64
    const char* pTitleName;         // offset 0xB8, size 0x4
    const char* pDataName;          // offset 0xBC, size 0x4
    const char* pDataImageName;     // offset 0xC0, size 0x4
    const char* pBackingName;       // offset 0xC4, size 0x4
    const char* pLeftArrowName;     // offset 0xC8, size 0x4
    const char* pRightArrowName;    // offset 0xCC, size 0x4
    const char* pSliderName;        // offset 0xD0, size 0x4
    bVector2 vWidgetStartPos;       // offset 0xD4, size 0x8
    bVector2 vLastWidgetPos;        // offset 0xDC, size 0x8
    bVector2 vWidgetSize;           // offset 0xE4, size 0x8
    bVector2 vMaxTitleSize;         // offset 0xEC, size 0x8
    bVector2 vMaxDataSize;          // offset 0xF4, size 0x8
    bVector2 vDataPos;              // offset 0xFC, size 0x8
    bVector2 vWidgetSpacing;        // offset 0x104, size 0x8
    unsigned int iIndexToAdd;       // offset 0x10C, size 0x4
    unsigned int iLastSelectedIndex;// offset 0x110, size 0x4
    unsigned int iMaxWidgetsOnScreen; // offset 0x114, size 0x4
    unsigned int iPrevButtonMessage;// offset 0x118, size 0x4
    unsigned int iPrevParam1;       // offset 0x11C, size 0x4
    unsigned int iPrevParam2;       // offset 0x120, size 0x4
    bool bScrollWrapped;            // offset 0x124, size 0x1
    bool bCurrentOptionSet;         // offset 0x128, size 0x1
    bool bHasScrollBar;             // offset 0x12C, size 0x1
    bool bViewNeedsSync;            // offset 0x130, size 0x1
    bool bAllowScroll;              // offset 0x134, size 0x1

    UIWidgetMenu(ScreenConstructorData* sd);
    ~UIWidgetMenu() override {}

    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void StorePrevNotification(unsigned int msg, FEObject* pobj, unsigned int param1, unsigned int param2);
    FEWidget* GetWidget(unsigned int id);
    void Scroll(eScrollDir dir);
    void ScrollWrapped(eScrollDir dir);
    void ScrollView(int dir);
    void PageUp();
    void PageDown();
    unsigned int AddButtonOption(FEButtonWidget* option);
    unsigned int AddStatOption(FEStatWidget* option);
    unsigned int AddToggleOption(FEToggleWidget* option, bool use_arrow);
    unsigned int AddToggleImageOption(FEToggleImageWidget* option, bool use_arrow);
    unsigned int AddSliderOption(FESliderWidget* option, bool use_arrow);
    unsigned int AddInputOption(FEInputWidget* option);
    unsigned int AddDateOption(FEDateWidget* option);
    FEString* GetCurrentFEString(const char* string_name);
    FEImage* GetCurrentFEImage(const char* img_name);
    FEObject* GetCurrentFEObject(const char* name);
    void ClearWidgets();
    void RefreshWidgets();
    void SetInitialOption(int number);
    void SetOption(FEWidget* opt);
    void SetInitialPositions();
    void Reposition();
    void Reset();
    void UpdateCursorPos();
    void IncrementStartPos();
    void SyncViewToSelection();
    unsigned int GetWidgetIndex(FEWidget* opt);

    unsigned int GetNumWidgets();

    virtual void Setup();

    void SetWidgetStartPos(bVector2& pos) {
        vWidgetStartPos = pos;
    }

    void SetWidgetStartX(float x) {
        vWidgetStartPos.x = x;
    }

    void SetWidgetStartY(float y) {
        vWidgetStartPos.y = y;
    }

    void SetScrollWrapped(bool wrapped) {
        bScrollWrapped = wrapped;
    }

    FEInputWidget* GetCurrentInputWidget();
};

#endif
