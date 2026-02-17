#ifndef FRONTEND_MENUSCREENS_COMMON_FEARRAYSCROLLERMENU_H
#define FRONTEND_MENUSCREENS_COMMON_FEARRAYSCROLLERMENU_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "feScrollerina.hpp"

// total size: 0x14
struct ArrayScripts {
    ArrayScripts();

    virtual ~ArrayScripts() {}

    void SetGreyHash(uint32 hash) {
        greyHash = hash;
    }

    uint32 GetGreyHash() {
        return greyHash;
    }

    void SetNormalHash(uint32 hash) {
        normHash = hash;
    }

    uint32 GetNormalHash() {
        return normHash;
    }

    void SetHighlightHash(uint32 hash) {
        highHash = hash;
    }

    uint32 GetHighlightHash() {
        return highHash;
    }

    void SetUnHighlightHash(uint32 hash) {
        unHighHash = hash;
    }

    uint32 GetUnHighlightHash() {
        return unHighHash;
    }

  private:
    uint32 greyHash;   // offset 0x0, size 0x4
    uint32 normHash;   // offset 0x4, size 0x4
    uint32 highHash;   // offset 0x8, size 0x4
    uint32 unHighHash; // offset 0xC, size 0x4
};

// total size: 0x24
class ArrayDatum : public bTNode<ArrayDatum> {
  public:
    void SetHash(uint32 newHash) {
        hash = newHash;
    }

    void SetDesc(uint32 newDesc) {
        desc = newDesc;
    }

    uint32 GetHash() {
        return hash;
    }

    uint32 GetDesc() {
        return desc;
    }

    bool IsEnabled() {
        return enabled;
    }

    void SetEnabled(bool b) {
        enabled = b;
    }

    bool IsLocked() {
        return locked;
    }

    void SetLocked(bool b) {
        locked = b;
    }

    bool IsGreyedOut() {
        return greyedOut;
    }

    void SetGreyedOut(bool b) {
        greyedOut = b;
    }

    bool IsChecked() {
        return checked;
    }

    void SetChecked(bool b) {
        checked = b;
    }

    virtual ~ArrayDatum() {}

    virtual void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {}

  private:
    uint32 hash;    // offset 0x8, size 0x4
    uint32 desc;    // offset 0xC, size 0x4
    bool enabled;   // offset 0x10, size 0x1
    bool greyedOut; // offset 0x14, size 0x1
    bool locked;    // offset 0x18, size 0x1
    bool checked;   // offset 0x1C, size 0x1
};

// total size: 0x14
class ArraySlot : public bTNode<ArraySlot> {
  public:
    ArraySlot(FEObject *obj);

    virtual ~ArraySlot() {}

    virtual void Update(ArrayDatum *datum, bool isSelected);

    FEObject *GetFEngObject() {
        return FEngObject;
    }

    void SetScripts(ArrayScripts *theScripts) {
        scripts = theScripts;
    }

  private:
    FEObject *FEngObject;  // offset 0x8, size 0x4
    ArrayScripts *scripts; // offset 0xC, size 0x4
};

// total size: 0xBC
class ArrayScroller {
  public:
    ArrayScroller(const char *pkg_name, int w, int h, bool selectable);

    virtual ~ArrayScroller() {}

    virtual void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2);

    virtual void RefreshHeader();

    void AddSlot(ArraySlot *slot);

    void AddDatum(ArrayDatum *datum);

    void SetSelection(ArrayDatum *newDatum, int newStartDatum);

    void SetSelectionAsOffScreen();

    int ForceSelectionOnScreen(int new_datum, int start);

    // void ScrollHor(enum eScrollDir dir);

    // void ScrollVer(enum eScrollDir dir);

    void UpdateScrollbar();

    ArraySlot *GetSlotAt(int index);

    ArrayDatum *GetDatumAt(int index);

    void SetInitialPosition(int index);

    void UpdateMouse();

    void ClearData();

    void SetDescLabel(uint32 hash) {}

    void SetDimensions(int w, int h) {}

    int GetWidth() {}

    int GetHeight() {}

    void ScrollLeft() {}

    void ScrollRight() {}

    void ScrollUp() {}

    void ScrollDown() {}

    void SetMouseDownMsg(uint32 msg) {}

    void SetClickToSelectMode(bool flag) {}

    int GetNumSlots() {}

    int GetStartDatumNum() {}

    int GetCurrentDatumNum() {}

    int GetNumDatum() {}

    ArrayDatum *GetCurrentDatum() {
        return currentDatum;
    }

    const char *GetPkgName() {
        return pkg_name;
    }

    struct FEPackage *GetPkg() {
        return pkg;
    }

    bool IsSelectableArray() {
        return bSelectableArray;
    }

    void SetScrollRegion(FEObject *region) {
        pScrollRegion = region;
    }

    ArrayScripts *GetScripts() {
        return &scripts;
    }

  private:
    bool bShouldPlaySound;     // offset 0x0, size 0x1
    bTList<ArraySlot> slots;   // offset 0x4, size 0x8
    bTList<ArrayDatum> data;   // offset 0xC, size 0x8
    ArrayDatum *currentDatum;  // offset 0x14, size 0x4
    int startDatum;            // offset 0x18, size 0x4
    int width;                 // offset 0x1C, size 0x4
    int height;                // offset 0x20, size 0x4
    uint32 descLabel;          // offset 0x24, size 0x4
    const char *pkg_name;      // offset 0x28, size 0x4
    struct FEPackage *pkg;     // offset 0x2C, size 0x4
    FEScrollBar ScrollBar;     // offset 0x30, size 0x64
    FEObject *pScrollRegion;   // offset 0x94, size 0x4
    bool bSelectableArray;     // offset 0x98, size 0x1
    ArrayScripts scripts;      // offset 0x9C, size 0x14
    uint32 mouseDownMsg;       // offset 0xB0, size 0x4
    bool bInClickToSelectMode; // offset 0xB4, size 0x1
};

// total size: 0xE8
class ArrayScrollerMenu : public MenuScreen, public ArrayScroller {
  public:
    ArrayScrollerMenu(ScreenConstructorData *sd, int w, int h, bool selectable);

    // Overrides: MenuScreen
    ~ArrayScrollerMenu() override {}

    // Overrides: MenuScreen
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    // Overrides: MenuScreen
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

    // Overrides: ArrayScroller
    void RefreshHeader() override;
};

#endif
