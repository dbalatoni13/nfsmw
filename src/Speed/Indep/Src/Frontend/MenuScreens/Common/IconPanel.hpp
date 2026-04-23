#ifndef _ICONPANEL
#define _ICONPANEL

#include <types.h>

#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

struct FEImage;

enum eScrollDir {
    eSD_PREV = -1,
    eSD_NEXT = 1,
    eSD_PAGE_PREV = -10000,
    eSD_PAGE_NEXT = 10000,
    eSD_NONE = 10001,
};

// total size: 0x38
struct IconPanel {
    bTList<IconOption> Options; // offset 0x0, size 0x8
    IconOption* pCurrentNode;   // offset 0x8, size 0x4
    FEObject* pMaster;          // offset 0xC, size 0x4
    FEObject* pScrollRegion;    // offset 0x10, size 0x4
    const char* pPackageName;   // offset 0x14, size 0x4
    const char* pButtonName;    // offset 0x18, size 0x4
    float fIconSpacing;         // offset 0x1C, size 0x4
    int iIndexToAdd;            // offset 0x20, size 0x4
    bool bWrap;                 // offset 0x24, size 0x1
    bool bHorizontal;           // offset 0x28, size 0x1
    bool bJustScrolled;         // offset 0x2C, size 0x1
    bool bReactToInput;         // offset 0x30, size 0x1

    IconPanel() {}
    IconPanel(const char* pkg_name, const char* master, const char* fe_button, const char* scroll_region, bool wrap);
    virtual ~IconPanel() {}

    virtual void Update();
    virtual FEImage* AddOption(IconOption* option);
    virtual void RemoveAll();
    virtual void Act(unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2);
    virtual IconOption* GetHead();
    virtual bool IsHead(IconOption* option);
    virtual bool IsTail(IconOption* option);
    virtual bool IsEndOfList(IconOption* opt);
    virtual int GetOptionIndex(IconOption* to_find);
    virtual bool SetSelection(IconOption* option);
    virtual void SetInitialPos();
    virtual void Scroll(eScrollDir dir);
    virtual void ScrollWrapped(eScrollDir dir);

    IconOption* GetOption(int to_find);
    void AnimateList();
    void AnimateSelected(float& list_width, float& list_height);
    void ResizeList(float list_width, float list_height);

    IconOption* GetCurrentOption() {
        return pCurrentNode;
    }

    FEObject* GetMaster() {
        return pMaster;
    }

    bool AtHead();
    bool AtTail();

    unsigned int GetCurrentDesc();
    unsigned int GetCurrentName();
    bool CurrentReactsImmediately();

    int GetIndexToAdd() {
        return iIndexToAdd;
    }

    int GetCurrentIndex() {
        if (pCurrentNode) {
            return GetOptionIndex(pCurrentNode);
        }
        return 0;
    }

    void ScrollNext();
    void ScrollPrev();

    bool JustScrolled() {
        return bJustScrolled;
    }

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
};

#endif
