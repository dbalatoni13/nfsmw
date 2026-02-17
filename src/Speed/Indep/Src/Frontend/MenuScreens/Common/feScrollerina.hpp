#ifndef FRONTEND_MENUSCREENS_COMMON_FESCROLLERINA_H
#define FRONTEND_MENUSCREENS_COMMON_FESCROLLERINA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

// total size: 0x64
class FEScrollBar {
  public:
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

#endif
