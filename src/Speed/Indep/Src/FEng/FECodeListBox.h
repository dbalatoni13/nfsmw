#ifndef FENG_FECODELISTBOX_H
#define FENG_FECODELISTBOX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "FEObject.h"
#include "FEListBox.h"
#include "FETypes.h"

class FEGameInterface;
class FEPoint;

inline int GetValidIndex(int lIndex, int lRange) {
    if (lIndex >= 0) {
        int rem = lIndex - (lIndex / lRange) * lRange;
        return rem;
    }

    int posIndex = -lIndex;
    int ret = 0;
    int rem = posIndex - (posIndex / lRange) * lRange;
    if (lRange > 1) {
        ret = lRange - rem;
    }
    return ret;
}

inline int GetRealValue(int i, int lNumTotal, int lCurrentVirtual, int lNumVisible) {
    int lRet;

    if (lNumTotal == 0) {
        return -1;
    }

    if (i >= lNumTotal) {
        i = i % lNumTotal;
    }

    lRet = i - lCurrentVirtual;
    if (lRet < 0) {
        lRet += lNumTotal;
    }

    if (lRet >= 0) {
        int rem = lRet - (lRet / lNumVisible) * lNumVisible;
        return rem;
    }

    int posIndex = -lRet;
    int ret = 0;
    int rem = posIndex - (posIndex / lNumVisible) * lNumVisible;
    if (lNumVisible > 1) {
        ret = lNumVisible - rem;
    }
    return ret;
}

// total size: 0xC8
class FECodeListBox : public FEObject {
  public:
    static void (*mpDefaultCallback)(FECodeListBox *);

    FECodeListBox();
    FECodeListBox(const FECodeListBox &Src, bool bReference);
    ~FECodeListBox() override;

    void CopyProperties(const FECodeListBox &Src);
    void Initialize(u32 ulNumColumns, u32 ulNumRows);
    FEObject *Clone(bool bReference);
    void FillAllCells();
    void SetTotalNumColumns(u32 ulNumTotalColumns);
    void SetTotalNumRows(u32 ulNumTotalRows);
    void AllocateStrings(u32 ulNumStrings, u32 ulStringSize);
    void ScrollSelection(long lColumnNum, long lRowNum);
    void Update(float fNumTicks);
    static void DefaultSelectCallback(FECodeListBox *pList);
    short *AllocateString();
    void DeallocateString(short *psString);
    long GetRealColumn(long lColumn) const;
    long GetRealRow(long lRow) const;
    bool CheckMovement(long lTargetColumn, long lTargetRow, long lOldColumn, long lOldRow, long lFlags);
    bool MakeMove(long lDirection, u32 &ulVirtual, u32 &ulTarget, u32 ulTotal, u32 ulVisible);
    bool ScrollSelection(long lDirection, u32 &ulVirtual, u32 &ulTarget, u32 ulTotal, u32 ulVisible, bool bIsColumn);
    u32 CalculateCurrentFromTarget(u32 ulTarget, u32 ulTotal, u32 ulVisible);
    void SetCellColor(u32 ulColumn, u32 ulRow, u32 ulColor, u32 ulNumColumns, u32 ulNumRows);
    void SetCellScale(u32 ulColumn, u32 ulRow, const FEPoint &stScale, u32 ulNumColumns, u32 ulNumRows);
    void SetCellJustification(u32 ulColumn, u32 ulRow, u32 ulJustification, u32 ulNumColumns, u32 ulNumRows);

    inline u32 GetNumVisibleColumns() const {
        return mulNumVisibleColumns;
    }
    inline u32 GetNumVisibleRows() const {
        return mulNumVisibleRows;
    }
    inline u32 GetNumTotalColumns() const {
        return mulNumTotalColumns;
    }
    inline u32 GetNumTotalRows() const {
        return mulNumTotalRows;
    }
    inline u32 GetCurrentVirtualColumn() const {
        return mulCurrentVirtualColumn;
    }
    inline u32 GetCurrentVirtualRow() const {
        return mulCurrentVirtualRow;
    }
    inline FEListBoxCell *GetCellData(u32 ulColumn, u32 ulRow) {
        return &mpstCells[ulRow * mulNumVisibleColumns + ulColumn];
    }
    inline FEListBoxCell *GetRealCellData(long lColumnIndex, long lRowIndex) {
        return &mpstCells[GetRealRow(lRowIndex) * mulNumVisibleColumns + GetRealColumn(lColumnIndex)];
    }
    inline void SetSelectionCallback(void (*pCallback)(FECodeListBox *)) {
        mpSelectionCallback = pCallback;
    }
    inline void SetSetCellCallback(void (*pCallback)(void *, FECodeListBox *, u32, u32), void *pData) {
        mpSetCellCallback = pCallback;
        mpvCallbackData = pData;
    }
    inline const FEColor &GetSelectionColor() const {
        return mstSelectionColor;
    }
    inline void SetSelectionColor(const FEColor &stColor) {
        mstSelectionColor = stColor;
    }
    inline float GetAlphaHilite() const {
        return mfCurrentAlpha;
    }
    inline u32 GetVisualSelectionColumn() {
        return mulCurrentVirtualColumn % mulNumVisibleColumns;
    }
    inline u32 GetVisualSelectionRow() {
        return mulCurrentVirtualRow % mulNumVisibleRows;
    }

  private:
    FEGameInterface *mpobRenderer;                                // offset 0x5C, size 0x4
    u32 mulNumVisibleColumns;                                     // offset 0x60, size 0x4
    u32 mulNumVisibleRows;                                        // offset 0x64, size 0x4
    u32 mulFlags;                                                 // offset 0x68, size 0x4
    u32 mulNumTotalColumns;                                       // offset 0x6C, size 0x4
    u32 mulNumTotalRows;                                          // offset 0x70, size 0x4
    u32 mulCurrentVirtualColumn;                                  // offset 0x74, size 0x4
    u32 mulCurrentVirtualRow;                                     // offset 0x78, size 0x4
    u32 mulTargetColumn;                                          // offset 0x7C, size 0x4
    u32 mulTargetRow;                                             // offset 0x80, size 0x4
    FEPoint mstViewDimensions;                                    // offset 0x84, size 0x8
    FEListBoxCell *mpstCells;                                     // offset 0x8C, size 0x4
    u32 mulNumStrings;                                            // offset 0x90, size 0x4
    u32 mulStringSize;                                            // offset 0x94, size 0x4
    u32 mulCurrentString;                                         // offset 0x98, size 0x4
    short **mppsStringData;                                       // offset 0x9C, size 0x4
    short *mpsStrings;                                            // offset 0xA0, size 0x4
    float mfCurrentAlpha;                                         // offset 0xA4, size 0x4
    float mfAlphaDelta;                                           // offset 0xA8, size 0x4
    FEColor mstSelectionColor;                                    // offset 0xAC, size 0x10
    void (*mpSelectionCallback)(FECodeListBox *);                 // offset 0xBC, size 0x4
    void (*mpSetCellCallback)(void *, FECodeListBox *, u32, u32); // offset 0xC0, size 0x4
    void *mpvCallbackData;                                        // offset 0xC4, size 0x4
};

#endif
