#ifndef FRONTEND_MENUSCREENS_COMMON_FESCROLLERINA_H
#define FRONTEND_MENUSCREENS_COMMON_FESCROLLERINA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

// total size: 0x64
class FEScrollBar {
  public:
    void UpdateArrowsMouse();

    void UpdateHandleMouse();

    void UpdateBackingMouse();

    FEScrollBar(const char *parent_pkg, const char *name, bool vert, bool resize, bool arrows_only);

    ~FEScrollBar() {}

    void SetArrow1Visibility(bool visible) {}

    void SetArrow2Visibility(bool visible) {}

    bool IsVisible() {
        return bVisible;
    }

    void SetGroupVisible(bool visible);

    void Update(int num_view_items, int num_list_items, int view_head_index, int selected_item);

    void SetPosResized(int num_view_items, int num_list_items, int view_head_index);

    void SetPosNonResized(int num_view_items, int num_list_items, int view_head_index);

    void SetArrowVisibility(int arrow_num, bool visible);

    void SetBackingVisibility(bool visible);

    void SetVisible(FEObject *obj);

    void SetInvisible(FEObject *obj);

    void SetArrow1Dim(bool dim);

    void SetArrow2Dim(bool dim);

  private:
    bool bVertical;              // offset 0x0, size 0x1
    bool bResizeHandle;          // offset 0x4, size 0x1
    bool bHandleGrabbed;         // offset 0x8, size 0x1
    bool bArrowsOnly;            // offset 0xC, size 0x1
    bool bVisible;               // offset 0x10, size 0x1
    bVector2 vGrabbedPos;        // offset 0x14, size 0x8
    bVector2 vCurPos;            // offset 0x1C, size 0x8
    bVector2 vGrabOffset;        // offset 0x24, size 0x8
    bVector2 vBackingPos;        // offset 0x2C, size 0x8
    bVector2 vBackingSize;       // offset 0x34, size 0x8
    bVector2 vHandleMinSize;     // offset 0x3C, size 0x8
    float fSegSize;              // offset 0x44, size 0x4
    Timer ScrollTime;            // offset 0x48, size 0x4
    FEObject *pBacking;          // offset 0x4C, size 0x4
    FEObject *pHandle;           // offset 0x50, size 0x4
    FEObject *pFirstArrow;       // offset 0x54, size 0x4
    FEObject *pSecondArrow;      // offset 0x58, size 0x4
    FEObject *pFirstBackingEnd;  // offset 0x5C, size 0x4
    FEObject *pSecondBackingEnd; // offset 0x60, size 0x4
};

class FEImage;
class FEGroup;

int FEngSNPrintf(char *dest, int size, const char *fmt, ...);

// total size: 0x8C
class ScrollerDatumNode : public bTNode<ScrollerDatumNode> {
  public:
    char String[128];          // offset 0x8, size 0x80
    unsigned int LanguageHash; // offset 0x88, size 0x4

    ScrollerDatumNode(const char *string, unsigned int hash) {
        FEngSNPrintf(String, 0x80, string);
        LanguageHash = hash;
    }

    virtual ~ScrollerDatumNode() {}
};

// total size: 0xC
class ScrollerSlotNode : public bTNode<ScrollerSlotNode> {
  public:
    FEObject *String; // offset 0x8, size 0x4

    ScrollerSlotNode(FEObject *string) {
        String = string;
    }

    virtual ~ScrollerSlotNode() {}
};

// total size: 0x14
class ScrollerDatum : public bTNode<ScrollerDatum> {
  public:
    bTList<ScrollerDatumNode> Strings; // offset 0x8, size 0x8
    bool bEnabled;                     // offset 0x10, size 0x1

    ScrollerDatum() {
        bEnabled = true;
    }
    ScrollerDatum(const char *string, unsigned int hash);

    virtual ~ScrollerDatum() {}

    void AddData(const char *string, unsigned int hash) {
        Strings.AddTail(new ("ScrollerDatumNode", 0) ScrollerDatumNode(string, hash));
    }
    ScrollerDatumNode *Find(const char *to_find);
    ScrollerDatumNode *Find(unsigned int hash);
    void Printf();
    char *GetTopDatumModeString();
    void Enable() {
        bEnabled = true;
    }
    void Disable() {
        bEnabled = false;
    }
    bool IsEnabled() {
        return bEnabled;
    }
};

// total size: 0x28
class ScrollerSlot : public bTNode<ScrollerSlot> {
  public:
    bTList<ScrollerSlotNode> FEStrings; // offset 0x8, size 0x8
    FEObject *pBacking;                 // offset 0x10, size 0x4
    bVector2 vTopLeft;                  // offset 0x14, size 0x8
    bVector2 vSize;                     // offset 0x1C, size 0x8
    bool bEnabled;                      // offset 0x24, size 0x1

    ScrollerSlot() {
        pBacking = nullptr;
        vTopLeft.x = vTopLeft.y = vSize.x = vSize.y = 0.0f;
        bEnabled = true;
    }
    ScrollerSlot(FEObject *string);

    virtual ~ScrollerSlot() {}

    void AddData(FEObject *string) {
        FEStrings.AddTail(new ("ScrollerSlotNode", 0) ScrollerSlotNode(string));
    }
    void SetBacking(FEObject *obj) {
        pBacking = obj;
    }
    void Highlight() {
        SetScript(FEHASH_HIGHLIGHT);
    }
    void UnHighlight() {
        SetScript(FEHASH_UNHIGHLIGHT);
    }
    void Enable() {
        bEnabled = true;
    }
    void Disable() {
        bEnabled = false;
    }
    void GetSize(bVector2 &size) {
        size = vSize;
    }
    void GetTopLeft(bVector2 &top_left) {
        top_left = vTopLeft;
    }
    bool IsEnabled() {
        return bEnabled;
    }
    void SetScript(unsigned int script_hash);
    void FindSize();
    void Show();
    void Hide();
    bool Find(FEObject *obj);
};


// total size: 0xC8
class Scrollerina {
  public:
    const char *pParentPkg;          // offset 0x0, size 0x4
    bTList<ScrollerSlot> Slots;      // offset 0x4, size 0x8
    bTList<ScrollerDatum> Data;      // offset 0xC, size 0x8
    unsigned int iNumSlots;          // offset 0x14, size 0x4
    unsigned int iNumData;           // offset 0x18, size 0x4
    unsigned int iViewHeadDataIndex; // offset 0x1C, size 0x4
    ScrollerDatum *SelectedDatum;    // offset 0x20, size 0x4
    ScrollerDatum *TopDatum;         // offset 0x24, size 0x4
    ScrollerSlot *SelectedSlot;      // offset 0x28, size 0x4
    FEImage *pBacking;               // offset 0x2C, size 0x4
    FEScrollBar ScrollBar;           // offset 0x30, size 0x64
    bVector2 vTopLeft;               // offset 0x94, size 0x8
    bVector2 vSize;                  // offset 0x9C, size 0x8
    bool bHasScrollBar;              // offset 0xA4, size 0x1
    bool bViewNeedsSync;             // offset 0xA8, size 0x1
    bool bWrapped;                   // offset 0xAC, size 0x1
    bool bAlwaysShowBacking;         // offset 0xB0, size 0x1
    bool bVertical;                  // offset 0xB4, size 0x1
    unsigned int mouseDownMsg;       // offset 0xB8, size 0x4
    bool bInClickToSelectMode;       // offset 0xBC, size 0x1
    FEObject *pScrollRegion;         // offset 0xC0, size 0x4

    Scrollerina(const char *parent_pkg, const char *backing, const char *scrollbar, bool vert, bool resize, bool wrapped,
                bool alwaysShowBacking);

    virtual ~Scrollerina() {}

    ScrollerSlot *GetSelectedSlot() {
        return SelectedSlot;
    }
    ScrollerDatum *GetSelectedDatum() {
        return SelectedDatum;
    }
    unsigned int GetNumSlots() {
        return iNumSlots;
    }
    unsigned int GetNumData() {
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
    FEScrollBar *GetScrollBarPointer() {
        return &ScrollBar;
    }
    void SetMouseDownMsg(unsigned int msg) {
        mouseDownMsg = msg;
    }
    void SetClickToSelectMode(bool flag) {
        bInClickToSelectMode = flag;
    }
    bool IsAtHead() {
        return SelectedDatum == Data.GetHead();
    }
    bool IsAtTail() {
        return SelectedDatum == Data.GetTail();
    }
    bool IsWrapped() {
        return bWrapped;
    }
    bool HasActiveSelection();
    unsigned int GetSelectedNodeIndex() {
        return GetNodeIndex(GetSelectedDatum());
    }
    // El ELF no tiene GetSelectedSlotIndex__11Scrollerina: MoveTrack llama
    // directamente a GetNodeIndex(ScrollerSlot*), o sea que es un inline.
    unsigned int GetSelectedSlotIndex() {
        return GetNodeIndex(GetSelectedSlot());
    }
    void SetSelectedDatum(ScrollerDatum *datum) {
        SelectedDatum = datum;
    }

    ScrollerSlot *AddSlot(const char *string_name, const char *backing);
    void AddSlot(ScrollerSlot *slot);
    ScrollerDatum *AddData(const char *string);
    ScrollerDatum *AddData(unsigned int hash);
    void AddData(ScrollerDatum *datum);
    ScrollerDatum *FindDatum(const char *to_find);
    ScrollerDatum *FindDatumInSlot(ScrollerSlot *to_find);
    ScrollerSlot *FindSlotWithDatum(ScrollerDatum *to_find);
    ScrollerSlot *FindSlot(FEObject *to_find);
    void ScrollNext();
    void ScrollPrev();
    // Tampoco existen MoveNext__11Scrollerina ni MovePrev__11Scrollerina: el
    // objetivo emite bl MoveSelected__11Scrollerina10eScrollDirb con
    // li r4,-1 / li r4,1 y li r5,1. Son envoltorios inline.
    void MoveNext() {
        MoveSelected(eSD_NEXT, true);
    }
    void MovePrev() {
        MoveSelected(eSD_PREV, true);
    }
    bool Scroll(eScrollDir dir);
    bool ScrollWrapped(eScrollDir dir);
    bool MoveSelected(eScrollDir dir, bool bprint);
    bool ScrollSelection(eScrollDir dir);
    void SyncViewToSelection();
    void SetDisabledScripts();

    virtual void Print();

    void DrawScrollBar();
    bool Reset(bool update);
    void Update(bool print);
    void Enable(ScrollerDatum *datum);
    void Disable(ScrollerDatum *datum);
    void UpdateSlotVisibility();
    void CountListIndices();
    void OrganizeForDataRemoval(ScrollerDatum *to_remove);
    unsigned int GetNodeIndex(ScrollerDatum *datum);
    unsigned int GetNodeIndex(ScrollerSlot *slot);
    bool SetSelectedSlot(unsigned int index);
    void SetSelected(ScrollerSlot *slot);
    void PageUp();
    void PageDown();
    void DeleteScrollerData();
    ScrollerDatum *RemoveDatum(ScrollerDatum *to_remove);
    ScrollerDatum *RemoveDatum(const char *string);
    ScrollerDatum *RemoveDatum(int index);
    void UnHighlightSelected() {
        if (SelectedSlot) {
            SelectedSlot->UnHighlight();
        }
    }
    void HighlightSelected() {
        if (SelectedSlot) {
            SelectedSlot->Highlight();
        }
    }
    const char *GetActiveSelection();
    void FindSize();
    void SetWrapped(bool wrapped);
    void SetSelectedDatum(unsigned int index);
};

#endif
