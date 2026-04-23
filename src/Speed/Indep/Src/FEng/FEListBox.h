#ifndef FENG_FELISTBOX_H
#define FENG_FELISTBOX_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "FEObject.h"
#include "FETypes.h"

inline unsigned long ClampIndex(unsigned long val, unsigned long range) {
    if (range == 0) return 0;
    if (val < range) return val;
    return range - 1;
}

// total size: 0xC
struct FEListEntryData {
    float fValue;            // offset 0x0, size 0x4
    float fCummulativeValue; // offset 0x4, size 0x4
    unsigned long ulJustification; // offset 0x8, size 0x4
};

// total size: 0xC
struct ListBoxResource {
    unsigned long Handle;        // offset 0x0, size 0x4
    unsigned long UserParam;     // offset 0x4, size 0x4
    unsigned long ResourceIndex; // offset 0x8, size 0x4

    inline ListBoxResource() {}
};

// total size: 0x30
struct FEListBoxCell {
    union _u {
        struct {
            float uv_left;      // offset 0x0
            float uv_top;       // offset 0x4
            float uv_right;     // offset 0x8
            float uv_bottom;    // offset 0xC
        } rect;
        struct {
            short* pStr;        // offset 0x0, size 0x4
            unsigned long Label; // offset 0x4, size 0x4
        } string;
    };

    unsigned long ulColor;          // offset 0x0, size 0x4
    FEPoint stScale;                // offset 0x4, size 0x8
    ListBoxResource stResource;    // offset 0xC, size 0xC
    unsigned long ulType;           // offset 0x18, size 0x4
    unsigned long ulJustification;  // offset 0x1C, size 0x4
    _u u;                           // offset 0x20, size 0x10

    inline FEListBoxCell() : ulColor(0), stScale(1.0f, 1.0f) {
        stResource.Handle = 0;
        stResource.UserParam = 0;
        stResource.ResourceIndex = 0;
        ulType = 0;
        u.string.pStr = nullptr;
        u.string.Label = 0xFFFFFFFF;
    }

    inline unsigned long GetLabelHash() const { return u.string.Label; }
    inline const short* GetStringPtr() const { return u.string.pStr; }
    inline const FERect& GetUV() const { return *reinterpret_cast<const FERect*>(&u.rect); }
    inline FERect& SetUV() { return *reinterpret_cast<FERect*>(&u.rect); }
};

// total size: 0xAC
struct FEListBox : public FEObject {
    unsigned long mulFlags;           // offset 0x5C, size 0x4
    unsigned long mulNumColumns;      // offset 0x60, size 0x4
    unsigned long mulNumRows;         // offset 0x64, size 0x4
    FEPoint mstViewDimensions;        // offset 0x68, size 0x8
    FEPoint mstCurrentLocation;       // offset 0x70, size 0x8
    FEListEntryData* mpstColumnData;  // offset 0x78, size 0x4
    FEListEntryData* mpstRowData;     // offset 0x7C, size 0x4
    FEPoint mstSelectionSpeed;        // offset 0x80, size 0x8
    unsigned long mulCurrentColumn;   // offset 0x88, size 0x4
    unsigned long mulCurrentRow;      // offset 0x8C, size 0x4
    FEListBoxCell* mpstCells;         // offset 0x90, size 0x4
    FEPoint mstTargetLocation;        // offset 0x94, size 0x8
    FEPoint mstDirection;             // offset 0x9C, size 0x8
    float mfCurrentAlpha;             // offset 0xA4, size 0x4
    float mfAlphaDelta;               // offset 0xA8, size 0x4

    FEListBox();
    FEListBox(const FEListBox& Object);
    ~FEListBox() override;

    void Initialize(unsigned long ulNumColumns, unsigned long ulNumRows);
    void Terminate();
    void SetNumColumns(unsigned long ulNumColumns);
    void SetNumRows(unsigned long ulNumRows);
    void SetColumnWidth(float fWidth);
    void SetRowHeight(float fHeight);
    void SetCellType(unsigned long ulType);
    void SetCellString(const short* psString);
    void IncrementCellByRow();
    void IncrementCellByColumn();
    unsigned long GetFirstVisibleColumn() const;
    unsigned long GetFirstVisibleRow() const;
    unsigned long GetLastVisibleColumn() const;
    unsigned long GetLastVisibleRow() const;
    bool GetCellInfo(unsigned long ulColumn, unsigned long ulRow, FERect& stCellRect, FERect& stClippedCellRect, FEListBoxCell& stCellInfo, unsigned long& ulJustification) const;
    void ScrollSelection(long lColumnNum, long lRowNum);
    void Update(float fNumTicks);
    void SetAutoWrap(bool bStopWrap);
    static void InitializeListEntry(FEListEntryData* pstEntries, unsigned long ulNumEntries);
    static void InitializeCell(FEListBoxCell* pstCells, unsigned long ulNumCells);
    void CleanupColumns();
    void CleanupRows();
    void CleanupCells();
    void RecalculateCummulative();
    void CompleteScroll();

    inline FEObject* Clone(bool bReference) {
        FEListBox* pNew = new (0) FEListBox(*this);
        return pNew;
    }

    inline void SetViewDimensions(const FEPoint& stViewDimensions) { mstViewDimensions = stViewDimensions; }
    inline void SetCurrentLocation(const FEPoint& stCurrentLocation) { mstCurrentLocation = stCurrentLocation; }
    inline void SetSelectionSpeed(const FEPoint& stSelectionSpeed) { mstSelectionSpeed = stSelectionSpeed; }
    inline void SetCurrentColumn(unsigned long ulCurrentColumn) { mulCurrentColumn = ClampIndex(ulCurrentColumn, mulNumColumns); }
    inline void SetCurrentRow(unsigned long ulCurrentRow) { mulCurrentRow = ClampIndex(ulCurrentRow, mulNumRows); }
    inline void SetColumnJustification(unsigned long ulJustification) { mpstColumnData[mulCurrentColumn].ulJustification = ulJustification; }
    inline void SetRowJustification(unsigned long ulJustification) { mpstRowData[mulCurrentRow].ulJustification = ulJustification; }
    inline void SetCellColor(const FEColor& stColor) { mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn].ulColor = static_cast<unsigned long>(stColor); }
    inline void SetCellScale(const FEPoint& stScale) { mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn].stScale = stScale; }
    inline void SetCellResource(unsigned long ulResHandle, unsigned long ulResParam, unsigned long ulResIndex) {
        FEListBoxCell* pCell = &mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn];
        pCell->stResource.Handle = ulResHandle;
        pCell->stResource.UserParam = ulResParam;
        pCell->stResource.ResourceIndex = ulResIndex;
    }
    inline void SetCellUV(const FERect& stUV) { *reinterpret_cast<FERect*>(&mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn].u.rect) = stUV; }
    inline unsigned long GetNumColumns() const { return mulNumColumns; }
    inline unsigned long GetNumRows() const { return mulNumRows; }
    inline const FEPoint& GetViewDimensions() const { return mstViewDimensions; }
    inline const FEPoint& GetCurrentLocation() const { return mstCurrentLocation; }
    inline const FEListEntryData* GetColumnData(unsigned long ulColumn) const { return &mpstColumnData[ulColumn]; }
    inline const FEListEntryData* GetRowData(unsigned long ulRow) const { return &mpstRowData[ulRow]; }
    inline const FEPoint& GetSelectionSpeed() const { return mstSelectionSpeed; }
    inline unsigned long GetCurrentColumn() const { return mulCurrentColumn; }
    inline unsigned long GetCurrentRow() const { return mulCurrentRow; }
    inline const FEListBoxCell* GetCellData(unsigned long ulColumn, unsigned long ulRow) const { return &mpstCells[ulRow * mulNumColumns + ulColumn]; }
    inline const FEListBoxCell* GetCurrentCellData() const { return &mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn]; }
    inline const FEListEntryData* GetCurrentColumnData() const { return &mpstColumnData[mulCurrentColumn]; }
    inline const FEListEntryData* GetCurrentRowData() const { return &mpstRowData[mulCurrentRow]; }
    inline float GetAlphaHilite() const { return mfCurrentAlpha; }
    inline bool IsCurrent(unsigned long ulColumn, unsigned long ulRow) const { return ulColumn == mulCurrentColumn && ulRow == mulCurrentRow; }
    inline bool IsAutoWrap() const { return (mulFlags & 1) != 0; }
    FEListBoxCell* GetPCellData(unsigned long ulColumn, unsigned long ulRow);
    inline FEListEntryData* GetPColumnData(unsigned long ulColumn) { return &mpstColumnData[ulColumn]; }
    inline FEListEntryData* GetPRowData(unsigned long ulRow) { return &mpstRowData[ulRow]; }
};

#endif
