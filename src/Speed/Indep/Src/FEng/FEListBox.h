#ifndef FELISTBOX_H_
#define FELISTBOX_H_

#include "FEObject.h"
#include "FETypes.h"
#include "Speed/Indep/Src/FEng/FEWideString.h"

// File: speed/indep/src/feng/FEListBox.h
static const u32 INVALID_RESOURCE_HANDLE = 0xFFFFFFFF;   // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:26
static const u16 INVALID_RESOURCE_HANDLE_SHORT = 0xFFFF; // size: 0x2, Decl: speed/indep/src/feng/FEListBox.h:27
static const u32 INVALID_LIST_ENTRY = 0xFFFFFFFF;        // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:28

static const u32 FELISTBOX_FLAGS_INITIALIZED = 1; // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:42
static const u32 FELISTBOX_FLAGS_SCROLL = 2;      // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:43
static const u32 FELISTBOX_FLAGS_DONTWRAP = 4;    // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:44
static const u32 FELISTBOX_FLAGS_WRAPH = 8;       // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:45
static const u32 FELISTBOX_FLAGS_WRAPV = 16;      // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:46
static const u32 FELISTBOX_FLAGS_SCROLLH = 32;    // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:47
static const u32 FELISTBOX_FLAGS_SCROLLV = 64;    // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:48

static const u32 FENG_HASH_SCROLLLEFT = 0xE10C4AF9;  // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:55
static const u32 FENG_HASH_SCROLLRIGHT = 0x030471AC; // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:56
static const u32 FENG_HASH_SCROLLUP = 0xFB814F13;    // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:57
static const u32 FENG_HASH_SCROLLDOWN = 0xE10814A6;  // size: 0x4, Decl: speed/indep/src/feng/FEListBox.h:58

#define FENG_MESSAGE_SCROLLLEFT "Scroll Left"   // :50
#define FENG_MESSAGE_SCROLLRIGHT "Scroll Right" // :51
#define FENG_MESSAGE_SCROLLUP "Scroll Up"       // :52
#define FENG_MESSAGE_SCROLLDOWN "Scroll Down"   // :53

// File: speed/indep/src/feng/FEListBox.h
// total size: 0xC
// Decl: speed/indep/src/feng/FEListBox.h:62
struct ListBoxResource {
    u32 Handle;        // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:63
    u32 UserParam;     // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:64
    u32 ResourceIndex; // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:65

    ListBoxResource() {} // Decl: speed/indep/src/feng/FEListBox.h:67

    void Set(u32 ulHandle, u32 ulUserParam, u32 ulResourceIndex) {
        this->Handle = ulHandle;
        this->UserParam = ulUserParam;
        this->ResourceIndex = ulResourceIndex;
    }
};

// total size: 0x30
// Decl: speed/indep/src/feng/FEListBox.h:79
struct FEListBoxCell {
    u32 ulColor;                // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:80
    FEPoint stScale;            // offset 0x4, size 0x8, Decl: speed/indep/src/feng/FEListBox.h:81
    ListBoxResource stResource; // offset 0xC, size 0xC, Decl: speed/indep/src/feng/FEListBox.h:82
    u32 ulType;                 // offset 0x18, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:83
    u32 ulJustification;        // offset 0x1C, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:84

    union _u {
        struct r {
            float uv_left;   // offset 0x0
            float uv_top;    // offset 0x4
            float uv_right;  // offset 0x8
            float uv_bottom; // offset 0xC
        } rect;              // offset 0x0, size 0x10
        struct string {
            i16 *pStr; // offset 0x0, size 0x4
            u32 Label; // offset 0x4, size 0x4
        } string;      // offset 0x0, size 0x8
    };
    _u u; // offset 0x20, size 0x10, Decl: speed/indep/src/feng/FEListBox.h:101

    FEListBoxCell() : ulColor(0), stScale(1.0f, 1.0f) { // Decl: speed/indep/src/feng/FEListBox.h:103
        stResource.Handle = 0;
        stResource.UserParam = 0;
        stResource.ResourceIndex = 0;
        ulType = 0;
        u.string.pStr = nullptr;
        u.string.Label = 0xFFFFFFFF;
    }

    u32 GetLabelHash() const {
        return u.string.Label;
    }

    const i16 *GetStringPtr() const {
        return u.string.pStr;
    }

    FEWideString GetString() const {}

    const FERect &GetUV() const {
        return *reinterpret_cast<const FERect *>(&u.rect);
    }

    FERect &SetUV() { // Decl: speed/indep/src/feng/FEListBox.h:128
        return *reinterpret_cast<FERect *>(&u.rect);
    }
};

// total size: 0xC
// Decl: speed/indep/src/feng/FEListBox.h:135
struct FEListEntryData {
    float fValue;            // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:136
    float fCummulativeValue; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:137
    u32 ulJustification;     // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:138
};

// total size: 0xAC
// Decl: speed/indep/src/feng/FEListBox.h:142
class FEListBox : public FEObject {
  public:
    u32 mulFlags;                    // offset 0x5C, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:145
    u32 mulNumColumns;               // offset 0x60, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:146
    u32 mulNumRows;                  // offset 0x64, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:147
    FEPoint mstViewDimensions;       // offset 0x68, size 0x8, Decl: speed/indep/src/feng/FEListBox.h:148
    FEPoint mstCurrentLocation;      // offset 0x70, size 0x8, Decl: speed/indep/src/feng/FEListBox.h:149
    FEListEntryData *mpstColumnData; // offset 0x78, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:150
    FEListEntryData *mpstRowData;    // offset 0x7C, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:151
    FEPoint mstSelectionSpeed;       // offset 0x80, size 0x8, Decl: speed/indep/src/feng/FEListBox.h:152
    u32 mulCurrentColumn;            // offset 0x88, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:153
    u32 mulCurrentRow;               // offset 0x8C, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:154
    FEListBoxCell *mpstCells;        // offset 0x90, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:155
    FEPoint mstTargetLocation;       // offset 0x94, size 0x8, Decl: speed/indep/src/feng/FEListBox.h:156
    FEPoint mstDirection;            // offset 0x9C, size 0x8, Decl: speed/indep/src/feng/FEListBox.h:157

    float mfCurrentAlpha; // offset 0xA4, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:160
    float mfAlphaDelta;   // offset 0xA8, size 0x4, Decl: speed/indep/src/feng/FEListBox.h:161

    FEListBox();
    FEListBox(const FEListBox &Object);
    ~FEListBox() override;

    void Initialize(u32 ulNumColumns, u32 ulNumRows);

    void Terminate();

    FEObject *Clone() { // Decl: speed/indep/src/feng/FEListBox.h:174
        FEListBox *pNew = new FEListBox(*this);
        return pNew;
    }

    void SetNumColumns(u32 ulNumColumns);

    void SetNumRows(u32 ulNumRows);

    void SetViewDimensions(const FEPoint &stViewDimensions) {
        mstViewDimensions = stViewDimensions;
    }

    void SetCurrentLocation(const FEPoint &stCurrentLocation) {
        mstCurrentLocation = stCurrentLocation;
    }

    void SetSelectionSpeed(const FEPoint &stSelectionSpeed) {
        mstSelectionSpeed = stSelectionSpeed;
    }

    void SetCurrentColumn(u32 ulCurrentColumn) {
        mulCurrentColumn = mulNumColumns == 0 ? 0 : ulCurrentColumn < mulNumColumns ? ulCurrentColumn : mulNumColumns - 1;
    }

    void SetCurrentRow(u32 ulCurrentRow) {
        mulCurrentRow = mulNumRows == 0 ? 0 : ulCurrentRow < mulNumRows ? ulCurrentRow : mulNumRows - 1;
    }

    void IncrementCellByRow();

    void IncrementCellByColumn();

    void SetColumnWidth(float fWidth);

    void SetColumnJustification(u32 ulJustification) {
        mpstColumnData[mulCurrentColumn].ulJustification = ulJustification;
    }

    void SetRowHeight(float fHeight);

    void SetRowJustification(u32 ulJustification) {
        mpstRowData[mulCurrentRow].ulJustification = ulJustification;
    }

    void SetCellColor(const FEColor &stColor) {
        GetPCellData(mulCurrentColumn, mulCurrentRow)->ulColor = stColor;
    }

    void SetCellScale(const FEPoint &stScale) {
        GetPCellData(mulCurrentColumn, mulCurrentRow)->stScale = stScale;
    }

    void SetCellResource(u32 ulResHandle, u32 ulResParam, u32 ulResIndex) {
        GetPCellData(mulCurrentColumn, mulCurrentRow)->stResource.Set(ulResHandle, ulResParam, ulResIndex);
    }

    void SetCellType(u32 ulType);

    void SetCellUV(const FERect &stUV) {
        *reinterpret_cast<FERect *>(&GetPCellData(mulCurrentColumn, mulCurrentRow)->u.rect) = stUV;
    }

    void SetCellString(const i16 *psString);

    u32 GetNumColumns() const {
        return mulNumColumns;
    }

    u32 GetNumRows() const {
        return mulNumRows;
    }

    const FEPoint &GetViewDimensions() const {
        return mstViewDimensions;
    }

    const FEPoint &GetCurrentLocation() const {
        return mstCurrentLocation;
    }

    const FEListEntryData *GetColumnData(u32 ulColumn) const {
        return &mpstColumnData[ulColumn];
    }

    const FEListEntryData *GetRowData(u32 ulRow) const {
        return &mpstRowData[ulRow];
    }

    const FEPoint &GetSelectionSpeed() const {
        return mstSelectionSpeed;
    }

    u32 GetCurrentColumn() const {
        return mulCurrentColumn;
    }

    u32 GetCurrentRow() const {
        return mulCurrentRow;
    }

    const FEListBoxCell *GetCellData(u32 ulColumn, u32 ulRow) const {
        return &mpstCells[ulRow * mulNumColumns + ulColumn];
    }

    const FEListBoxCell *GetCurrentCellData() const {
        return GetCellData(mulCurrentColumn, mulCurrentRow);
    }

    const FEListEntryData *GetCurrentColumnData() const {
        return &mpstColumnData[mulCurrentColumn];
    }

    const FEListEntryData *GetCurrentRowData() const {
        return GetRowData(mulCurrentRow);
    }

    u32 GetFirstVisibleColumn() const;

    u32 GetFirstVisibleRow() const;

    u32 GetLastVisibleColumn() const;

    u32 GetLastVisibleRow() const;

    float GetAlphaHilite() const {
        return mfCurrentAlpha;
    }

    bool GetCellInfo(u32 ulColumn, u32 ulRow, FERect &stCellRect, FERect &stClippedCellRect, FEListBoxCell &stCellInfo, u32 &ulJustification) const;

    bool IsCurrent(u32 ulColumn, u32 ulRow) const {
        return ulColumn == mulCurrentColumn && ulRow == mulCurrentRow;
    }

    void ScrollSelection(i32 lColumnNum, i32 lRowNum);

    void Update(float fNumTicks);

    void SetAutoWrap(bool bStopWrap);

    bool IsAutoWrap() const {
        return (mulFlags & 1) != 0;
    }

  private:
    FEListBoxCell *GetPCellData(u32 ulColumn, u32 ulRow) {
        return &mpstCells[ulRow * mulNumColumns + ulColumn];
    }

    FEListEntryData *GetPColumnData(u32 ulColumn) {
        return &mpstColumnData[ulColumn];
    }

    FEListEntryData *GetPRowData(u32 ulRow) {
        return &mpstRowData[ulRow];
    }

    static void InitializeListEntry(FEListEntryData *pstEntries, u32 ulNumEntries);

    static void InitializeCell(FEListBoxCell *pstCells, u32 ulNumCells);

    void CleanupColumns();

    void CleanupRows();

    void CleanupCells();

    void RecalculateCummulative();

    void CompleteScroll();

    friend class FECodeListBox;
    friend class FEPackageReader;
};

#endif
