#ifndef FRONTEND_MENUSCREENS_COMMON_FESCROLLERINA_H
#define FRONTEND_MENUSCREENS_COMMON_FESCROLLERINA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"

class ScrollerDatumNode : public bTNode<ScrollerDatumNode> {
  public:
    ScrollerDatumNode(const char *string, uint32 hash) {
        FEngSNPrintf(String, 0x80, string);
        LanguageHash = hash;
    }

    virtual ~ScrollerDatumNode() {}

    char String[128];    // offset 0x8
    uint32 LanguageHash; // offset 0x88
};

class ScrollerDatum : public bTNode<ScrollerDatum> {
  public:
    ScrollerDatum() {}
    ScrollerDatum(const char *string, uint32 hash) {};
    virtual ~ScrollerDatum() {}

    void AddData(const char *string, uint32 hash) {
        Strings.AddTail(FNEW ScrollerDatumNode(string, hash));
    }
    char *GetTopDatumModeString() {};
    ScrollerDatumNode *Find(const char *to_find);
    ScrollerDatumNode *Find(uint32 hash);
    void Printf();
    void Enable() {
        bEnabled = true;
    }
    void Disable() {
        bEnabled = false;
    }
    bool IsEnabled() {
        return bEnabled;
    }

    bTList<ScrollerDatumNode> Strings; // offset 0x8
  private:
    bool bEnabled; // offset 0x10
};

class ScrollerSlotNode : public bTNode<ScrollerSlotNode> {
  public:
    ScrollerSlotNode(FEObject *string) {
        String = string;
    }

    virtual ~ScrollerSlotNode() {}

    FEObject *String; // offset 0x8
};

class ScrollerSlot : public bTNode<ScrollerSlot> {
  public:
    ScrollerSlot() {}
    ScrollerSlot(FEObject *string) {};
    virtual ~ScrollerSlot() {}

    void AddData(FEObject *string) {
        FEStrings.AddTail(FNEW ScrollerSlotNode(string));
    }
    void SetBacking(FEObject *obj) {
        pBacking = obj;
    }
    void Hide();
    void Show();
    void Highlight() {};
    void UnHighlight() {};
    void Enable() {
        bEnabled = true;
    }
    void Disable() {
        bEnabled = false;
    }
    void SetScript(uint32 script_hash);
    void FindSize();
    void GetSize(bVector2 &size) {
        size = vSize;
    }
    void GetTopLeft(bVector2 &top_left) {
        top_left = vTopLeft;
    }
    bool IsEnabled() {
        return bEnabled;
    }

    bool Find(FEObject *obj);

    bTList<ScrollerSlotNode> FEStrings; // offset 0x8
    FEObject *pBacking;                 // offset 0x10
    bVector2 vTopLeft;                  // offset 0x14
    bVector2 vSize;                     // offset 0x1C
    bool bEnabled;                      // offset 0x24
};

// total size: 0xC8
class Scrollerina {
  public:
    Scrollerina(const char *parent_pkg, const char *backing, const char *scrollbar, bool vert, bool resize, bool wrapped, bool alwaysShowBacking);
    virtual ~Scrollerina() {}

    ScrollerSlot *AddSlot(const char *string_name, const char *backing);
    void AddSlot(ScrollerSlot *slot);
    ScrollerDatum *AddData(const char *string);
    ScrollerDatum *AddData(uint32 hash);
    void AddData(ScrollerDatum *datum);
    ScrollerDatum *FindDatum(const char *to_find);
    ScrollerSlot *GetSelectedSlot() {
        return SelectedSlot;
    }
    ScrollerDatum *GetSelectedDatum() {
        return SelectedDatum;
    }
    void SetSelected(ScrollerSlot *slot);
    void ScrollNext();
    void ScrollPrev();
    void ScrollViewNext() {
        ScrollView(1);
    }
    void ScrollViewPrev() {
        ScrollView(-1);
    }
    void PageUp();
    void PageDown();
    void MoveNext();
    void MovePrev();
    bool Reset(bool update);
    void Update(bool print);
    void UnHighlightSelected();
    void HighlightSelected();
    void Enable(ScrollerDatum *datum);
    void Disable(ScrollerDatum *datum);
    void DeleteScrollerData();
    ScrollerDatum *RemoveDatum(ScrollerDatum *to_remove);
    ScrollerDatum *RemoveDatum(const char *string);
    ScrollerDatum *RemoveDatum(int index);
    bool IsAtHead() {
        return SelectedDatum == Data.GetHead();
    }
    bool IsAtTail() {
        return SelectedDatum == Data.GetTail();
    }
    bool IsWrapped() {
        return bWrapped;
    }
    void SetWrapped(bool wrapped);
    bool HasActiveSelection();
    const char *GetActiveSelection();
    uint32 GetSelectedNodeIndex() {
        return GetNodeIndex(GetSelectedDatum());
    }
    uint32 GetSelectedSlotIndex();
    void SetSelectedDatum(uint32 index);
    void SetSelectedDatum(ScrollerDatum *datum) {
        SelectedDatum = datum;
    }
    bool SetSelectedSlot(uint32 index);
    uint32 GetNumSlots() {
        return iNumSlots;
    }
    uint32 GetNumData() {
        return iNumData;
    }
    ScrollerDatum *GetFirstDatum() {
        return Data.GetHead();
    }
    ScrollerDatum *GetLastDatum() {
        return Data.GetTail();
    }
    ScrollerDatum *GetTopDatum() {
        return TopDatum;
    }
    ScrollerSlot *GetFirstSlot();
    ScrollerSlot *GetLastSlot();
    ScrollerSlot *GetSlot(int ordinal_number);
    ScrollerDatum *FindDatumInSlot(ScrollerSlot *to_find);
    FEScrollBar *GetScrollBarPointer() {
        return &ScrollBar;
    }
    void SetMouseDownMsg(uint32 msg) {
        mouseDownMsg = msg;
    }
    void SetClickToSelectMode(bool flag) {
        bInClickToSelectMode = flag;
    }

  protected:
    virtual void Print();
    void DrawScrollBar();
    bool Scroll(eScrollDir dir);
    bool ScrollWrapped(eScrollDir dir);
    bool ScrollView(int dir);
    bool MoveSelected(eScrollDir dir, bool bprint);
    bool ScrollSelection(eScrollDir dir);
    void SyncViewToSelection();
    void SetDisabledScripts();
    void UpdateSlotVisibility();
    void CountListIndices();
    void OrganizeForDataRemoval(ScrollerDatum *to_remove);
    uint32 GetNodeIndex(ScrollerDatum *datum);
    uint32 GetNodeIndex(ScrollerSlot *slot);
    void FindSize();
    ScrollerSlot *FindSlot(FEObject *to_find);
    ScrollerSlot *FindSlotWithDatum(ScrollerDatum *to_find);

    const char *pParentPkg;       // offset 0x0, size 0x4
    bTList<ScrollerSlot> Slots;   // offset 0x4, size 0x8
    bTList<ScrollerDatum> Data;   // offset 0xC, size 0x8
    uint32 iNumSlots;             // offset 0x14, size 0x4
    uint32 iNumData;              // offset 0x18, size 0x4
    uint32 iViewHeadDataIndex;    // offset 0x1C, size 0x4
    ScrollerDatum *SelectedDatum; // offset 0x20, size 0x4
    ScrollerDatum *TopDatum;      // offset 0x24, size 0x4
    ScrollerSlot *SelectedSlot;   // offset 0x28, size 0x4
    FEImage *pBacking;            // offset 0x2C, size 0x4
    FEScrollBar ScrollBar;        // offset 0x30, size 0x64
    bVector2 vTopLeft;            // offset 0x94, size 0x8
    bVector2 vSize;               // offset 0x9C, size 0x8
    bool bHasScrollBar;           // offset 0xA4, size 0x1
    bool bViewNeedsSync;          // offset 0xA8, size 0x1
    bool bWrapped;                // offset 0xAC, size 0x1
    bool bAlwaysShowBacking;      // offset 0xB0, size 0x1
    bool bVertical;               // offset 0xB4, size 0x1
    uint32 mouseDownMsg;          // offset 0xB8, size 0x4
    bool bInClickToSelectMode;    // offset 0xBC, size 0x1
    FEObject *pScrollRegion;      // offset 0xC0, size 0x4
};

#endif
