#ifndef FECODELISTBOX_H_
#define FECODELISTBOX_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEListBox.h"

class FEGameInterface;

static const i32 INVALID_REAL_INDEX = -1; // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:28

static const u32 FECODELISTBOX_FLAGS_INITIALIZED = 1;           // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:44
static const u32 FECODELISTBOX_FLAGS_DONTWRAP = 2;              // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:45
static const u32 FECODELISTBOX_FLAGS_SELECTIONLESS = 4;         // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:46
static const u32 FECODELISTBOX_FLAGS_SCROLLFROMCENTER = 8;      // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:47
static const u32 FECODELISTBOX_FLAGS_IGNORESELECTIONCOLOR = 16; // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:48
static const u32 FECODELISTBOX_FLAGS_SCROLLH = 32;              // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:49
static const u32 FECODELISTBOX_FLAGS_SCROLLV = 64;              // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:50

static const u32 FECODELISTBOX_PUBLICFLAGS_MASK = 0xFFFFFFFE; // size: 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:52

inline i32 GetValidIndex(i32 lIndex, i32 lRange) { // Decl: speed/indep/src/feng/FECodeListBox.h:58
    if (lIndex >= 0) {
        return lIndex - (lIndex / lRange) * lRange;
    }

    int posIndex = -lIndex;
    int rem = posIndex - (posIndex / lRange) * lRange;
    if (lRange > 1) {
        return lRange - rem;
    }
    return 0;
}

class FECodeListBox;

typedef void (*FECodeListBoxSelectCB)(FECodeListBox *);                    // Decl: speed/indep/src/feng/FECodeListBox.h:86
typedef void (*FECodeListBoxSetCellCB)(void *, FECodeListBox *, u32, u32); // Decl: speed/indep/src/feng/FECodeListBox.h:87

// total size: 0xC8
// Decl: speed/indep/src/feng/FECodeListBox.h:96
class FECodeListBox : public FEObject {
  private:
    FEGameInterface *mpobRenderer; // offset 0x5C, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:98

    u32 mulNumVisibleColumns; // offset 0x60, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:100
    u32 mulNumVisibleRows;    // offset 0x64, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:101

    u32 mulFlags;                // offset 0x68, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:103
    u32 mulNumTotalColumns;      // offset 0x6C, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:104
    u32 mulNumTotalRows;         // offset 0x70, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:105
    u32 mulCurrentVirtualColumn; // offset 0x74, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:106
    u32 mulCurrentVirtualRow;    // offset 0x78, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:107
    u32 mulTargetColumn;         // offset 0x7C, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:108
    u32 mulTargetRow;            // offset 0x80, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:109
    FEPoint mstViewDimensions;   // offset 0x84, size 0x8, Decl: speed/indep/src/feng/FECodeListBox.h:110

    FEListBoxCell *mpstCells; // offset 0x8C, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:112

    u32 mulNumStrings;    // offset 0x90, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:115
    u32 mulStringSize;    // offset 0x94, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:116
    u32 mulCurrentString; // offset 0x98, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:117
    i16 **mppsStringData; // offset 0x9C, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:118
    i16 *mpsStrings;      // offset 0xA0, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:119

    float mfCurrentAlpha; // offset 0xA4, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:121
    float mfAlphaDelta;   // offset 0xA8, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:122

    FEColor mstSelectionColor; // offset 0xAC, size 0x10, Decl: speed/indep/src/feng/FECodeListBox.h:124

    void *mpvCallbackData; // offset 0xC4, size 0x4, Decl: speed/indep/src/feng/FECodeListBox.h:128

  public:
    static void (*mpDefaultCallback)(FECodeListBox *);            // size: 0x4, address: 0x8041D074
    void (*mpSelectionCallback)(FECodeListBox *);                 // offset 0xBC, size 0x4
    void (*mpSetCellCallback)(void *, FECodeListBox *, u32, u32); // offset 0xC0, size 0x4

    FECodeListBox();
    FECodeListBox(const FECodeListBox &Src, bool bReference);
    ~FECodeListBox() override;

    void Initialize(u32 ulNumVisCols, u32 ulNumVisRows);

    FEObject *Clone(bool bReference) override;

    void CopyProperties(const FECodeListBox &Object);

    void SetGameInterface(FEGameInterface *pobGameInterface) { // Decl: speed/indep/src/feng/FECodeListBox.h:144
        this->mpobRenderer = pobGameInterface;
    }

    void FillAllCells();

    void SetTotalNumColumns(u32 ulNumTotalColumns);

    void SetTotalNumRows(u32 ulNumTotalRows);

    void SetCurrent(u32 ulCurrentColumn, u32 ulCurrentRow);

    void SetCurrentColumn(u32 ulVCurrentColumn);

    void SetCurrentRow(u32 ulVCurrentRow);

    void SetViewDimensions(const FEPoint &stViewDimensions) {
        mstViewDimensions.h = stViewDimensions.h;
        mstViewDimensions.v = stViewDimensions.v;
    }

    u32 GetTotalNumColumns() const {}

    u32 GetTotalNumRows() const {}

    u32 GetNumVisColumns() const {
        return mulNumVisibleColumns;
    }

    u32 GetNumVisRows() const {
        return mulNumVisibleRows;
    }

    u32 GetCurrentColumn() const {
        return mulCurrentVirtualColumn;
    }

    u32 GetCurrentRow() const {
        return mulCurrentVirtualRow;
    }

    const FEPoint &GetViewDimensions() const {
        return mstViewDimensions;
    }

    void SetCellColor(u32 ulStartColumn, u32 ulStartRow, u32 ulColor, u32 ulNumColumns, u32 ulNumRows);

    void SetCellScale(u32 ulStartColumn, u32 ulStartRow, const FEPoint &stScale, u32 ulNumColumns, u32 ulNumRows);

    void SetCellResource(u32 ulStartColumn, u32 ulStartRow, u32 ulResHandle, u32 ulResParam, u32 ulResIndex, u32 ulNumColumns, u32 ulNumRows);

    void SetCellJustification(u32 ulStartColumn, u32 ulStartRow, u32 ulJustification, u32 ulNumColumns, u32 ulNumRows);

    void SetCellType(u32 ulStartColumn, u32 ulStartRow, u32 ulType, u32 ulNumColumns, u32 ulNumRows);

    void SetCellUV(u32 ulColumn, u32 ulRow, const FERect &stUV) {}

    void SetCellString(u32 ulColumn, u32 ulRow, const i16 *psString, u32 ulLabelHash) {}

    void SetCellData(u32 ulStartColumn, u32 ulStartRow, u32 ulColor, const FEPoint &stScale, u32 ulJustification, u32 ulType, u32 ulNumColumns,
                     u32 ulNumRows) {}

    void SetAllTypeResource(u32 ulType, u32 ulResHandle, u32 ulResParam, u32 ulResIndex);

    void SetAllType(u32 ulType);

    void SetAllColor(u32 ulColor);

    void SetAllScale(const FEPoint &stScale);

    void SetAllJustication(u32 ulJustification);

    const FEListBoxCell *GetCellData(u32 ulColumnIndex, u32 ulRowIndex) const {}

    void AllocateStrings(u32 ulNumStrings, u32 ulStringSize);

    u32 GetNumStrings() const {}

    u32 GetSizeStrings() const {}

    u32 GetFirstVisibleColumn() const {}

    u32 GetFirstVisibleRow() const {}

    u32 GetLastVisibleColumn() const {}

    u32 GetLastVisibleRow() const {}

    bool GetCellInfo(u32 ulColumn, u32 ulRow, FERect &stCellRect, FEListBoxCell &stCellInfo) const;

    bool IsCurrent(u32 ulColumn, u32 ulRow) const;

    void ScrollSelection(i32 lColumnNum, i32 lRowNum);

    bool ScrollSelection(i32 lNumMove, u32 &ulCurrentVirtual, u32 &ulTarget, u32 ulNumTotal, u32 ulNumVis, bool bColumn);

    void Update(float fNumTicks);

    f32 GetAlphaHilite() const {
        return mfCurrentAlpha;
    }

    void SetFlags(u32 ulFlag, bool bSet) {
        if (bSet) {
            Flags |= ulFlag;
        } else {
            Flags &= ~ulFlag;
        }
    }

    u32 GetFlags() const {
        return Flags;
    }

    void SetSelectionColor(const FEColor &stColor) {
        mstSelectionColor = stColor;
    }

    FEColor &GetSelectionColor() const {
        return const_cast<FEColor &>(mstSelectionColor);
    }

    void Reset();

    void SetSetCellCallback(FECodeListBoxSetCellCB pCallback, void *pvData) {
        mpSetCellCallback = pCallback;
        mpvCallbackData = pData;
    }

    static void DefaultSelectCallback(FECodeListBox *pobListBox);

    void SetSelectionCallback(FECodeListBoxSelectCB pSelectionCallback) {
        mpSelectionCallback = pSelectionCallback;
    }

    static void SetDefaultSelectionCallback(FECodeListBoxSelectCB pSelectionCallback) {}

    u32 GetVisualSelectionColumn() { // Decl: speed/indep/src/feng/FECodeListBox.h:224
        return mulCurrentVirtualColumn % mulNumVisibleColumns;
    }
    u32 GetVisualSelectionRow() { // Decl: speed/indep/src/feng/FECodeListBox.h:225
        return mulCurrentVirtualRow % mulNumVisibleRows;
    }

  private:
    i16 *AllocateString();

    void DeallocateString(i16 *psString);

    FEListBoxCell *GetPCellData(u32 ulColumnIndex, u32 ulRowIndex) const {
        return &mpstCells[ulRowIndex * mulNumVisibleColumns + ulColumnIndex];
    }

    FEListBoxCell *GetRealCellData(i32 lColumnIndex, i32 lRowIndex) {
        return &mpstCells[GetRealRow(lRowIndex) * mulNumVisibleColumns + GetRealColumn(lColumnIndex)];
    }

    i32 GetRealColumn(i32 i) const;

    i32 GetRealRow(i32 i) const;

    i32 GetNewVirtual(i32 lNum, i32 lCurrent, i32 lNumTotal, u32 ulBuildFlag0, u32 ulBuildFlag1);

    bool CheckMovement(i32 lNumMove, i32 lCurrentVirtual, i32 lTarget, i32 lNumTotal, i32 lNumVis);

    bool MakeMove(i32 lNumMove, u32 &ulCurrentVirtual, u32 &ulTarget, u32 ulNumTotal, u32 ulNumVis);

    u32 CalculateCurrentFromTarget(u32 ulTarget, u32 ulNumTotal, u32 ulNumVisible);

    void SetPCellType(FEListBoxCell *pstCell, u32 ulType) {}
};

#endif
