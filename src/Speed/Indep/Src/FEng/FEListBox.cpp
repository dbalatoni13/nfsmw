#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

FEListBox::FEListBox()
    : mulFlags(FELISTBOX_FLAGS_DONTWRAP), //
      mulNumColumns(0),                   //
      mulNumRows(0),                      //
      mstViewDimensions(0.0f, 0.0f),      //
      mstCurrentLocation(0.0f, 0.0f),     //
      mpstColumnData(nullptr),            //
      mpstRowData(nullptr),               //
      mstSelectionSpeed(1.0f, 1.0f),      //
      mulCurrentColumn(0),                //
      mulCurrentRow(0),                   //
      mpstCells(nullptr),                 //
      mstTargetLocation(0.0f, 0.0f),      //
      mstDirection(0.0f, 0.0f),           //
      mfCurrentAlpha(1.0f),               //
      mfAlphaDelta(-1.0f / 720.0f) {
    Type = FE_List;
}

FEListBox::~FEListBox() {
    Terminate();
}

void FEListBox::Terminate() {
    if (!(mulFlags & FELISTBOX_FLAGS_INITIALIZED)) {
        return;
    }
    mulFlags &= ~FELISTBOX_FLAGS_INITIALIZED;
    CleanupColumns();
    CleanupRows();
    CleanupCells();
}

void FEListBox::SetNumColumns(u32 ulNumColumns) {
    if (ulNumColumns == 0) {
        CleanupColumns();
        CleanupCells();
        return;
    }

    FEListEntryData *pstNewColumns = FNEW FEListEntryData[ulNumColumns];
    u32 ulNumCopy = 0;
    if (mulNumColumns != 0) {
        ulNumCopy = ulNumColumns;
        if (ulNumColumns > mulNumColumns) {
            ulNumCopy = mulNumColumns;
        }
        FEngMemCpy(pstNewColumns, mpstColumnData, ulNumCopy * sizeof(FEListEntryData));
        if (mpstColumnData) {
            delete[] mpstColumnData;
        }
    }
    InitializeListEntry(pstNewColumns + ulNumCopy, ulNumColumns - ulNumCopy);

    u32 ulNumCells = ulNumColumns * mulNumRows;
    FEListBoxCell *pstCells = nullptr;
    if (ulNumCells != 0) {
        pstCells = FNEW FEListBoxCell[ulNumCells];
        if (mpstCells) {
            for (u32 c = 0; c < mulNumRows; c++) {
                u32 dstOff = c * ulNumColumns;
                u32 srcOff = c * mulNumColumns;
                FEngMemCpy(pstCells + dstOff, mpstCells + srcOff, ulNumCopy);
                InitializeCell(pstCells + dstOff + ulNumCopy, ulNumColumns - ulNumCopy);
            }
            if (mpstCells) {
                delete[] mpstCells;
            }
        } else {
            InitializeCell(pstCells, ulNumCells);
        }
    }
    mulNumColumns = ulNumColumns;
    mpstColumnData = pstNewColumns;
    mpstCells = pstCells;
}

void FEListBox::SetNumRows(u32 ulNumRows) {
    if (ulNumRows == 0) {
        CleanupRows();
        CleanupCells();
        return;
    }

    u32 ulNumCopy = 0;
    FEListEntryData *pstNewRows = FNEW FEListEntryData[ulNumRows];
    if (mulNumRows != 0) {
        ulNumCopy = ulNumRows;
        if (ulNumRows > mulNumRows) {
            ulNumCopy = mulNumRows;
        }
        FEngMemCpy(pstNewRows, mpstRowData, ulNumCopy * sizeof(FEListEntryData));
        if (mpstRowData) {
            delete[] mpstRowData;
        }
    }
    InitializeListEntry(pstNewRows + ulNumCopy, ulNumRows - ulNumCopy);

    u32 ulNumCells = mulNumColumns * ulNumRows;
    FEListBoxCell *pstCells = nullptr;
    if (ulNumCells != 0) {
        pstCells = FNEW FEListBoxCell[ulNumCells];
        if (mpstCells) {
            FEngMemCpy(pstCells, mpstCells, ulNumCopy * mulNumColumns);
            InitializeCell(pstCells + ulNumCopy * mulNumColumns, (ulNumRows - ulNumCopy) * mulNumColumns);
            if (mpstCells) {
                delete[] mpstCells;
            }
        } else {
            InitializeCell(pstCells, ulNumCells);
        }
    }
    mulNumRows = ulNumRows;
    mpstRowData = pstNewRows;
    mpstCells = pstCells;
}

void FEListBox::SetCellType(u32 ulType) {
    FEListBoxCell *pstCell = GetPCellData(mulCurrentColumn, mulCurrentRow);
    if (pstCell->ulType != ulType) {
        if (pstCell->ulType == 2 && pstCell->u.string.pStr) {
            delete[] pstCell->u.string.pStr;
            pstCell->u.string.pStr = nullptr;
        }
        pstCell->ulType = ulType;
    }
}

void FEListBox::SetCellString(const i16 *psString) {
    FEListBoxCell *pCell = GetPCellData(mulCurrentColumn, mulCurrentRow);
    if (pCell->u.string.pStr) {
        delete[] pCell->u.string.pStr;
        pCell->u.string.pStr = nullptr;
    }
    if (psString) {
        u32 ulNewLength = (GetStringLength(psString) + 1);
        pCell->u.string.pStr = FNEW i16[ulNewLength];
        FEngMemCpy(pCell->u.string.pStr, psString, ulNewLength * sizeof(i16));
    }
}

void FEListBox::IncrementCellByColumn() {
    mulCurrentColumn++;
    if (mulCurrentColumn >= mulNumColumns) {
        mulCurrentColumn = 0;
        mulCurrentRow++;
        if (mulCurrentRow >= mulNumRows) {
            mulCurrentRow = 0;
        }
    }
}

void FEListBox::ScrollSelection(i32 lColumnNum, i32 lRowNum) {
    u32 i;

    if ((mulFlags & 0x60) == 0x60) {
        return;
    }
    if (mulFlags & 0x20) {
        lColumnNum = 0;
    }
    if ((mulFlags & 0x40) != 0) {
        lRowNum = 0;
    }
    if (lColumnNum == 0) {
        if (lRowNum == 0) {
            return;
        }
    } else if ((mulFlags & 0x20) == 0) {
        i32 lColumn;
        float fNewWidth;
        lColumn = mulCurrentColumn + lColumnNum;

        if (mulFlags & 4) {
            if (lColumn >= static_cast<i32>(mulNumColumns)) {
                lColumn = mulNumColumns - 1;
            }
            if (lColumn < 0) {
                lColumn = 0;
            }
            mulCurrentColumn = lColumn;
            mstTargetLocation.h = GetCurrentColumnData()->fCummulativeValue;
            goto end_column;
        } else {

            if (lColumn < 0) {
                i32 i = mulNumColumns + lColumn;
                mstTargetLocation.h = GetColumnData(i)->fCummulativeValue;
                mstCurrentLocation.h = mstTargetLocation.h;
                do {
                    mstCurrentLocation.h = mstCurrentLocation.h + GetColumnData(i)->fValue;
                    i32 next = i + 1;
                    i = next - (next / mulNumColumns) * mulNumColumns;
                } while (i != static_cast<i32>(mulCurrentColumn));
            } else {
                if (lColumn < static_cast<i32>(mulNumColumns)) {
                    goto set_column;
                }
                mstTargetLocation.h = mstCurrentLocation.h;
                i32 i = mulCurrentColumn;
                do {
                    mstTargetLocation.h =
                        mstTargetLocation.h + GetColumnData(i - (i / mulNumColumns) * mulNumColumns)->fValue;
                    i = i + 1;
                } while (i != lColumn);
            }
            mulFlags = mulFlags | FELISTBOX_FLAGS_WRAPH;
            mulCurrentColumn = lColumn - (lColumn / mulNumColumns) * mulNumColumns;
            goto end_column;
        }
    set_column:
        mulCurrentColumn = lColumn;
        mstTargetLocation.h = GetCurrentColumnData()->fCummulativeValue;
    end_column:

        fNewWidth = 0.0f;
        for (i = mulCurrentColumn; i < mulNumColumns; i++) {
            fNewWidth = fNewWidth + *reinterpret_cast<float *>(reinterpret_cast<char *>(mpstColumnData) + i * 0xC);
        }

        if ((mulFlags & 4) != 0) {
            if (fNewWidth < mstViewDimensions.h) {
                mstTargetLocation.h = mstTargetLocation.h - (mstViewDimensions.h - fNewWidth);
            }
        } else {
            if (fNewWidth < mstViewDimensions.h) {
                mulFlags = mulFlags | FELISTBOX_FLAGS_WRAPH;
            }
        }
        mulFlags = mulFlags | 0x20;
    }

    if (lRowNum == 0 || (mulFlags & 0x40) != 0) {
        goto compute_direction;
    }

    {
        i32 lRow;
        float fNewHeight;
        lRow = mulCurrentRow + lRowNum;

        if (mulFlags & 4) {
            if (lRow >= static_cast<i32>(mulNumRows)) {
                lRow = mulNumRows - 1;
            }
            if (lRow < 0) {
                lRow = 0;
            }
            goto set_row;
        } else {
            if (lRow < 0) {
                i32 i = mulNumRows + lRow;
                mstTargetLocation.v = GetRowData(i)->fCummulativeValue;
                mstCurrentLocation.v = mstTargetLocation.v;
                do {
                    mstCurrentLocation.v = mstCurrentLocation.v + GetRowData(i)->fValue;
                    i32 next = i + 1;
                    i = next - (next / mulNumRows) * mulNumRows;
                } while (i != static_cast<i32>(mulCurrentRow));
            } else {
                if (lRow < static_cast<i32>(mulNumRows)) {
                    goto set_row;
                }
                mstTargetLocation.v = mstCurrentLocation.v;
                i32 i = mulCurrentRow;
                do {
                    mstTargetLocation.v = mstTargetLocation.v + GetRowData(i - (i / mulNumRows) * mulNumRows)->fValue;
                    i = i + 1;
                } while (i != lRow);
            }
            mulFlags = mulFlags | FELISTBOX_FLAGS_WRAPH;
            mulCurrentRow = lRow - (lRow / mulNumRows) * mulNumRows;
            goto end_row;
        }
    set_row:
        mulCurrentRow = lRow;
        mstTargetLocation.v = GetCurrentRowData()->fCummulativeValue;
    end_row:

        fNewHeight = 0.0f;
        for (i = mulCurrentRow; i < mulNumRows; i++) {
            fNewHeight = fNewHeight + *reinterpret_cast<float *>(reinterpret_cast<char *>(mpstRowData) + i * 0xC);
        }

        if ((mulFlags & 4) != 0) {
            if (fNewHeight < mstViewDimensions.v) {
                mstTargetLocation.v = mstTargetLocation.v - (mstViewDimensions.v - fNewHeight);
            }
        } else {
            if (fNewHeight < mstViewDimensions.v) {
                mulFlags = mulFlags | 0x10;
            }
        }
        mulFlags = mulFlags | 0x40;
    }

compute_direction:
    FEVector2 &obTargetLocation = reinterpret_cast<FEVector2 &>(mstTargetLocation);
    FEVector2 &obCurrentLocation = reinterpret_cast<FEVector2 &>(mstCurrentLocation);
    FEVector2 &obDirection = reinterpret_cast<FEVector2 &>(mstDirection);
    obDirection = obTargetLocation - obCurrentLocation;
    mulFlags = mulFlags | FELISTBOX_FLAGS_SCROLL;
    float fLength = obDirection.Length();
    if (fLength < 0.1f) {
        CompleteScroll();
    } else {
        obDirection *= 1.0f / obDirection.Length();
    }
}

void FEListBox::Update(float fNumTicks) {
    mfCurrentAlpha = mfCurrentAlpha + mfAlphaDelta * fNumTicks;
    if (mfCurrentAlpha < 0.0f) {
        mfCurrentAlpha = 0.0f;
        mfAlphaDelta = -mfAlphaDelta;
    } else if (mfCurrentAlpha > 1.0f) {
        mfCurrentAlpha = 1.0f;
        mfAlphaDelta = -mfAlphaDelta;
    }
    if (mulFlags & FELISTBOX_FLAGS_SCROLL) {
        FEVector2 &obTargetLocation = reinterpret_cast<FEVector2 &>(mstTargetLocation);
        FEVector2 &obCurrentLocation = reinterpret_cast<FEVector2 &>(mstCurrentLocation);
        FEVector2 &obSpeed = reinterpret_cast<FEVector2 &>(mstSelectionSpeed);
        FEVector2 obDirection(reinterpret_cast<FEVector2 &>(mstDirection));
        FEVector2 obVelocity(obDirection);
        float fDot = obDirection.Dot(obSpeed);

        obVelocity *= FEngAbs(fDot) * fNumTicks;
        obCurrentLocation += obVelocity;
        if (obDirection.Dot(obTargetLocation) - obDirection.Dot(obCurrentLocation) < 0.0f) {
            CompleteScroll();
        }
    }
}

void FEListBox::SetAutoWrap(bool bStopWrap) {
    if (bStopWrap) {
        mulFlags &= ~FELISTBOX_FLAGS_DONTWRAP;
    } else {
        mulFlags |= FELISTBOX_FLAGS_DONTWRAP;
    }
}

void FEListBox::InitializeListEntry(FEListEntryData *pstEntries, u32 ulNumEntries) {
    FEngMemSet(pstEntries, 0, ulNumEntries * sizeof(FEListEntryData));
}

void FEListBox::InitializeCell(FEListBoxCell *pstCells, u32 ulNumCells) {
    for (u32 i = 0; i < ulNumCells; i++) {
        pstCells[i].ulColor = 0xFFFFFFFF;
        pstCells[i].stScale = FEPoint(1.0f);
        pstCells[i].stResource.Set(0, 0, 0);
        pstCells[i].ulType = 0;
        pstCells[i].SetUV()(0.0f, 0.0f, 1.0f, 1.0f);
        pstCells[i].ulJustification = 0;
    }
}

void FEListBox::CleanupColumns() {
    if (mulNumColumns != 0) {
        if (mpstColumnData) {
            delete[] mpstColumnData;
        }
        mpstColumnData = nullptr;
        mulNumColumns = 0;
    }
}

void FEListBox::CleanupRows() {
    if (mulNumRows != 0) {
        if (mpstRowData) {
            delete[] mpstRowData;
        }
        mpstRowData = nullptr;
        mulNumRows = 0;
    }
}

void FEListBox::CleanupCells() {
    u32 numCells = mulNumRows * mulNumColumns;
    if (numCells != 0) {
        for (u32 i = 0; i < numCells; i++) {
            if (mpstCells[i].ulType == 2 && mpstCells[i].u.string.pStr) {
                delete[] mpstCells[i].u.string.pStr;
                mpstCells[i].u.string.pStr = nullptr;
            }
        }
        if (mpstCells) {
            delete[] mpstCells;
        }
        mpstCells = nullptr;
    }
}

void FEListBox::RecalculateCummulative() {
    FEListEntryData *pstColumn = mpstColumnData;
    f32 fCurrent = 0.0f;
    for (u32 ulColumn = 0; ulColumn < mulNumColumns; ulColumn++) {
        pstColumn->fCummulativeValue = fCurrent;
        fCurrent += pstColumn->fValue;
        pstColumn++;
    }

    FEListEntryData *pstRow = mpstRowData;
    fCurrent = 0.0f;
    for (u32 ulRow = 0; ulRow < mulNumRows; ulRow++) {
        pstRow->fCummulativeValue = fCurrent;
        fCurrent += pstRow->fValue;
        pstRow++;
    }
}

void FEListBox::CompleteScroll() {
    mstCurrentLocation = mstTargetLocation;
    mulFlags &= ~0x62;
    if (mulCurrentColumn == 0) {
        mulFlags &= ~0x08;
        mstCurrentLocation.h = 0.0f;
    }
    if (mulCurrentRow == 0) {
        mulFlags &= ~0x10;
        mstCurrentLocation.v = 0.0f;
    }
}
