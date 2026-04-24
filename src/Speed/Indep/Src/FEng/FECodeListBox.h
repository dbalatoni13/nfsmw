#ifndef FENG_FECODELISTBOX_H
#define FENG_FECODELISTBOX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEObject.h"
#include "FEListBox.h"
#include "FETypes.h"

struct FEGameInterface;
struct FEPoint;

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
struct FECodeListBox : public FEObject {
    static void (*mpDefaultCallback)(FECodeListBox*);

    FEGameInterface* mpobRenderer;           // offset 0x5C, size 0x4
    unsigned long mulNumVisibleColumns;      // offset 0x60, size 0x4
    unsigned long mulNumVisibleRows;         // offset 0x64, size 0x4
    unsigned long mulFlags;                  // offset 0x68, size 0x4
    unsigned long mulNumTotalColumns;        // offset 0x6C, size 0x4
    unsigned long mulNumTotalRows;           // offset 0x70, size 0x4
    unsigned long mulCurrentVirtualColumn;   // offset 0x74, size 0x4
    unsigned long mulCurrentVirtualRow;      // offset 0x78, size 0x4
    unsigned long mulTargetColumn;           // offset 0x7C, size 0x4
    unsigned long mulTargetRow;              // offset 0x80, size 0x4
    FEPoint mstViewDimensions;               // offset 0x84, size 0x8
    FEListBoxCell* mpstCells;                // offset 0x8C, size 0x4
    unsigned long mulNumStrings;             // offset 0x90, size 0x4
    unsigned long mulStringSize;             // offset 0x94, size 0x4
    unsigned long mulCurrentString;          // offset 0x98, size 0x4
    short** mppsStringData;                  // offset 0x9C, size 0x4
    short* mpsStrings;                       // offset 0xA0, size 0x4
    float mfCurrentAlpha;                    // offset 0xA4, size 0x4
    float mfAlphaDelta;                      // offset 0xA8, size 0x4
    FEColor mstSelectionColor;               // offset 0xAC, size 0x10
    void (*mpSelectionCallback)(FECodeListBox*); // offset 0xBC, size 0x4
    void (*mpSetCellCallback)(void*, FECodeListBox*, unsigned long, unsigned long); // offset 0xC0, size 0x4
    void* mpvCallbackData;                   // offset 0xC4, size 0x4

    FECodeListBox();
    FECodeListBox(const FECodeListBox& Src, bool bReference);
    ~FECodeListBox() override;

    void CopyProperties(const FECodeListBox& Src);
    void Initialize(unsigned long ulNumColumns, unsigned long ulNumRows);
    FEObject* Clone(bool bReference);
    void FillAllCells();
    void SetTotalNumColumns(unsigned long ulNumTotalColumns);
    void SetTotalNumRows(unsigned long ulNumTotalRows);
    void AllocateStrings(unsigned long ulNumStrings, unsigned long ulStringSize);
    void ScrollSelection(long lColumnNum, long lRowNum);
    void Update(float fNumTicks);
    static void DefaultSelectCallback(FECodeListBox* pList);
    short* AllocateString();
    void DeallocateString(short* psString);
    long GetRealColumn(long lColumn) const;
    long GetRealRow(long lRow) const;
    bool CheckMovement(long lTargetColumn, long lTargetRow, long lOldColumn, long lOldRow, long lFlags);
    bool MakeMove(long lDirection, unsigned long& ulVirtual, unsigned long& ulTarget, unsigned long ulTotal, unsigned long ulVisible);
    bool ScrollSelection(long lDirection, unsigned long& ulVirtual, unsigned long& ulTarget, unsigned long ulTotal, unsigned long ulVisible, bool bIsColumn);
    unsigned long CalculateCurrentFromTarget(unsigned long ulTarget, unsigned long ulTotal, unsigned long ulVisible);
    void SetCellColor(unsigned long ulColumn, unsigned long ulRow, unsigned long ulColor, unsigned long ulNumColumns, unsigned long ulNumRows);
    void SetCellScale(unsigned long ulColumn, unsigned long ulRow, const FEPoint& stScale, unsigned long ulNumColumns, unsigned long ulNumRows);
    void SetCellJustification(unsigned long ulColumn, unsigned long ulRow, unsigned long ulJustification, unsigned long ulNumColumns, unsigned long ulNumRows);

    inline unsigned long GetNumVisibleColumns() const { return mulNumVisibleColumns; }
    inline unsigned long GetNumVisibleRows() const { return mulNumVisibleRows; }
    inline unsigned long GetNumTotalColumns() const { return mulNumTotalColumns; }
    inline unsigned long GetNumTotalRows() const { return mulNumTotalRows; }
    inline unsigned long GetCurrentVirtualColumn() const { return mulCurrentVirtualColumn; }
    inline unsigned long GetCurrentVirtualRow() const { return mulCurrentVirtualRow; }
    inline FEListBoxCell* GetCellData(unsigned long ulColumn, unsigned long ulRow) { return &mpstCells[ulRow * mulNumVisibleColumns + ulColumn]; }
    inline FEListBoxCell* GetRealCellData(long lColumnIndex, long lRowIndex) { return &mpstCells[GetRealRow(lRowIndex) * mulNumVisibleColumns + GetRealColumn(lColumnIndex)]; }
    inline void SetSelectionCallback(void (*pCallback)(FECodeListBox*)) { mpSelectionCallback = pCallback; }
    inline void SetSetCellCallback(void (*pCallback)(void*, FECodeListBox*, unsigned long, unsigned long), void* pData) { mpSetCellCallback = pCallback; mpvCallbackData = pData; }
    inline const FEColor& GetSelectionColor() const { return mstSelectionColor; }
    inline void SetSelectionColor(const FEColor& stColor) { mstSelectionColor = stColor; }
    inline float GetAlphaHilite() const { return mfCurrentAlpha; }
    inline unsigned long GetVisualSelectionColumn() { return mulCurrentVirtualColumn % mulNumVisibleColumns; }
    inline unsigned long GetVisualSelectionRow() { return mulCurrentVirtualRow % mulNumVisibleRows; }
};

#endif
