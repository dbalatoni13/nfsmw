#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

unsigned long GetStringLength(const short* pString);

FEListBox::FEListBox()
    : mulFlags(4) //
    , mulNumColumns(0) //
    , mulNumRows(0) //
    , mstViewDimensions(0.0f, 0.0f) //
    , mstCurrentLocation(0.0f, 0.0f) //
    , mpstColumnData(nullptr) //
    , mpstRowData(nullptr) //
    , mstSelectionSpeed(1.0f, 1.0f) //
    , mulCurrentColumn(0) //
    , mulCurrentRow(0) //
    , mpstCells(nullptr) //
    , mstTargetLocation(0.0f, 0.0f) //
    , mstDirection(0.0f, 0.0f) //
    , mfCurrentAlpha(1.0f) //
    , mfAlphaDelta(-1.0f / 720.0f) {
    Type = FE_List;
}

FEListBox::~FEListBox() {
    Terminate();
}

void FEListBox::Terminate() {
    if (!(mulFlags & 1)) {
        return;
    }
    mulFlags &= ~1;
    CleanupColumns();
    CleanupRows();
    CleanupCells();
}

void FEListBox::SetAutoWrap(bool bStopWrap) {
    if (bStopWrap) {
        mulFlags &= ~4;
    } else {
        mulFlags |= 4;
    }
}

void FEListBox::InitializeListEntry(FEListEntryData* pstEntries, unsigned long ulNumEntries) {
    FEngMemSet(pstEntries, 0, ulNumEntries * sizeof(FEListEntryData));
}

void FEListBox::InitializeCell(FEListBoxCell* pstCells, unsigned long ulNumCells) {
    for (unsigned long i = 0; i < ulNumCells; i++) {
        pstCells[i].ulColor = 0xFFFFFFFF;
        pstCells[i].stScale = FEPoint(1.0f, 1.0f);
        pstCells[i].ulJustification = 0;
        pstCells[i].stResource.Handle = 0;
        pstCells[i].stResource.UserParam = 0;
        pstCells[i].stResource.ResourceIndex = 0;
        pstCells[i].ulType = 0;
        pstCells[i].u.rect.uv_left = 0.0f;
        pstCells[i].u.rect.uv_top = 0.0f;
        pstCells[i].u.rect.uv_right = 1.0f;
        pstCells[i].u.rect.uv_bottom = 1.0f;
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
    unsigned long numCells = mulNumRows * mulNumColumns;
    if (numCells != 0) {
        for (unsigned long i = 0; i < numCells; i++) {
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
    unsigned long i;
    float cumulative;

    i = 0;
    FEListEntryData* pCol = mpstColumnData;
    cumulative = 0.0f;
    while (i < mulNumColumns) {
        pCol->fCummulativeValue = cumulative;
        i++;
        float val = pCol->fValue;
        pCol += 1;
        cumulative = cumulative + val;
    }

    i = 0;
    FEListEntryData* pRow = mpstRowData;
    cumulative = 0.0f;
    while (i < mulNumRows) {
        pRow->fCummulativeValue = cumulative;
        i++;
        float val = pRow->fValue;
        pRow += 1;
        cumulative = cumulative + val;
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

void FEListBox::SetCellType(unsigned long ulType) {
    FEListBoxCell* pCell = &mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn];
    if (pCell->ulType != ulType) {
        if (pCell->ulType == 2 && pCell->u.string.pStr) {
            delete[] pCell->u.string.pStr;
            pCell->u.string.pStr = nullptr;
        }
        pCell->ulType = ulType;
    }
}

void FEListBox::SetCellString(const short* psString) {
    FEListBoxCell* pCell = &mpstCells[mulCurrentRow * mulNumColumns + mulCurrentColumn];
    if (pCell->u.string.pStr) {
        delete[] pCell->u.string.pStr;
        pCell->u.string.pStr = nullptr;
    }
    if (psString) {
        int len = (GetStringLength(psString) + 1) * 2;
        pCell->u.string.pStr = static_cast<short*>(FEngMalloc(len, 0, 0));
        FEngMemCpy(pCell->u.string.pStr, psString, len);
    }
}

void FEListBox::SetNumColumns(unsigned long ulNumColumns) {
    if (ulNumColumns == 0) {
        CleanupColumns();
        CleanupCells();
    } else {
        unsigned long ulNumCopy = 0;
        FEListEntryData* pstNewColumns = static_cast<FEListEntryData*>(FEngMalloc(ulNumColumns * sizeof(FEListEntryData), 0, 0));
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

        unsigned long ulNumCells = ulNumColumns * mulNumRows;
        FEListBoxCell* pstCells = nullptr;
        if (ulNumCells != 0) {
            pstCells = FENG_NEW FEListBoxCell[ulNumCells];
            if (mpstCells) {
                for (unsigned long c = 0; c < mulNumRows; c++) {
                    unsigned long dstOff = c * ulNumColumns;
                    unsigned long srcOff = c * mulNumColumns;
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
}

void FEListBox::SetNumRows(unsigned long ulNumRows) {
    if (ulNumRows == 0) {
        CleanupRows();
        CleanupCells();
    } else {
        unsigned long ulNumCopy = 0;
        FEListEntryData* pstNewRows = static_cast<FEListEntryData*>(FEngMalloc(ulNumRows * sizeof(FEListEntryData), 0, 0));
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

        unsigned long ulNumCells = mulNumColumns * ulNumRows;
        FEListBoxCell* pstCells = nullptr;
        if (ulNumCells != 0) {
            pstCells = FENG_NEW FEListBoxCell[ulNumCells];
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
}

void FEListBox::ScrollSelection(long lColumnNum, long lRowNum) {
    unsigned long flags = mulFlags;
    if ((flags & 0x60) == 0x60) {
        return;
    }
    unsigned long colDisabled = flags & 0x20;
    if (colDisabled) {
        lColumnNum = 0;
    }
    if ((flags & 0x40) != 0) {
        lRowNum = 0;
    }
    if (lColumnNum == 0) {
        if (lRowNum == 0) {
            return;
        }
    } else if (!colDisabled) {
        unsigned long ulCurrentColumn = mulCurrentColumn;
        unsigned long ulNewColumn = ulCurrentColumn + lColumnNum;
        if (flags & 4) {
            if (static_cast<long>(ulNewColumn) >= static_cast<long>(mulNumColumns)) {
                ulNewColumn = mulNumColumns - 1;
            }
            if (static_cast<long>(ulNewColumn) < 0) {
                ulNewColumn = 0;
            }
            goto set_column;
        } else {
            if (static_cast<long>(ulNewColumn) < 0) {
                unsigned long numCols = mulNumColumns;
                unsigned long i = numCols + ulNewColumn;
                float fCummulativeValue = mpstColumnData[i].fCummulativeValue;
                mstTargetLocation.h = fCummulativeValue;
                mstCurrentLocation.h = fCummulativeValue;
                do {
                    mstCurrentLocation.h = mstCurrentLocation.h + mpstColumnData[i].fValue;
                    unsigned long next = i + 1;
                    i = next - (next / numCols) * numCols;
                } while (i != ulCurrentColumn);
            } else {
                unsigned long numCols = mulNumColumns;
                if (static_cast<long>(ulNewColumn) < static_cast<long>(numCols)) {
                    goto set_column;
                }
                mstTargetLocation.h = mstCurrentLocation.h;
                do {
                    long idx = ulCurrentColumn - (ulCurrentColumn / numCols) * numCols;
                    ulCurrentColumn = ulCurrentColumn + 1;
                    mstTargetLocation.h = mstTargetLocation.h + mpstColumnData[idx].fValue;
                } while (ulCurrentColumn != ulNewColumn);
            }
            mulFlags = mulFlags | 8;
            mulCurrentColumn = ulNewColumn - (ulNewColumn / mulNumColumns) * mulNumColumns;
            goto end_column;
        }
    set_column:
        mulCurrentColumn = ulNewColumn;
        mstTargetLocation.h = mpstColumnData[ulNewColumn].fCummulativeValue;
    end_column:

        unsigned long i = mulCurrentColumn;
        float fNewWidth = 0.0f;
        if (i < mulNumColumns) {
            do {
                long idx = i * 0xC;
                i = i + 1;
                fNewWidth = fNewWidth + *reinterpret_cast<float*>(reinterpret_cast<char*>(mpstColumnData) + idx);
            } while (i < mulNumColumns);
        }

        if ((mulFlags & 4) != 0) {
            if (fNewWidth < mstViewDimensions.h) {
                mstTargetLocation.h = mstTargetLocation.h - (mstViewDimensions.h - fNewWidth);
            }
        } else {
            if (fNewWidth < mstViewDimensions.h) {
                mulFlags = mulFlags | 8;
            }
        }
        mulFlags = mulFlags | 0x20;
    }

    if (lRowNum == 0 || (mulFlags & 0x40) != 0) {
        goto compute_direction;
    }

    {
        unsigned long ulCurrentRow = mulCurrentRow;
        unsigned long ulNewRow = ulCurrentRow + lRowNum;
        if (mulFlags & 4) {
            if (static_cast<long>(ulNewRow) >= static_cast<long>(mulNumRows)) {
                ulNewRow = mulNumRows - 1;
            }
            if (static_cast<long>(ulNewRow) < 0) {
                ulNewRow = 0;
            }
            goto set_row;
        } else {
            if (static_cast<long>(ulNewRow) < 0) {
                unsigned long numRows = mulNumRows;
                unsigned long i = numRows + ulNewRow;
                float fCummulativeValue = mpstRowData[i].fCummulativeValue;
                mstTargetLocation.v = fCummulativeValue;
                mstCurrentLocation.v = fCummulativeValue;
                do {
                    mstCurrentLocation.v = mstCurrentLocation.v + mpstRowData[i].fValue;
                    unsigned long next = i + 1;
                    i = next - (next / numRows) * numRows;
                } while (i != ulCurrentRow);
            } else {
                unsigned long numRows = mulNumRows;
                if (static_cast<long>(ulNewRow) < static_cast<long>(numRows)) {
                    goto set_row;
                }
                mstTargetLocation.v = mstCurrentLocation.v;
                do {
                    long idx = ulCurrentRow - (ulCurrentRow / numRows) * numRows;
                    ulCurrentRow = ulCurrentRow + 1;
                    mstTargetLocation.v = mstTargetLocation.v + mpstRowData[idx].fValue;
                } while (ulCurrentRow != ulNewRow);
            }
            mulFlags = mulFlags | 8;
            mulCurrentRow = ulNewRow - (ulNewRow / mulNumRows) * mulNumRows;
            goto end_row;
        }
    set_row:
        mulCurrentRow = ulNewRow;
        mstTargetLocation.v = mpstRowData[ulNewRow].fCummulativeValue;
    end_row:

        unsigned long i = mulCurrentRow;
        float fNewHeight = 0.0f;
        if (i < mulNumRows) {
            do {
                long idx = i * 0xC;
                i = i + 1;
                fNewHeight = fNewHeight + *reinterpret_cast<float*>(reinterpret_cast<char*>(mpstRowData) + idx);
            } while (i < mulNumRows);
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
    FEVector2& obDirection = reinterpret_cast<FEVector2&>(mstDirection);
    obDirection = reinterpret_cast<FEVector2&>(mstTargetLocation) - reinterpret_cast<FEVector2&>(mstCurrentLocation);
    mulFlags = mulFlags | 2;
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
    if (mulFlags & 2) {
        FEVector2 obDirection(reinterpret_cast<FEVector2&>(mstDirection));
        FEVector2 obVelocity(obDirection);
        FEVector2& obSpeed = reinterpret_cast<FEVector2&>(mstSelectionSpeed);
        float fDot = obDirection.Dot(obSpeed);
        obVelocity *= FEngAbs(fDot) * fNumTicks;
        FEVector2& obCurrentLocation = reinterpret_cast<FEVector2&>(mstCurrentLocation);
        obCurrentLocation += obVelocity;
        FEVector2& obTargetLocation = reinterpret_cast<FEVector2&>(mstTargetLocation);
        if (obDirection.Dot(obTargetLocation) - obDirection.Dot(obCurrentLocation) < 0.0f) {
            CompleteScroll();
        }
    }
}

FEListBoxCell* FEListBox::GetPCellData(unsigned long ulColumn, unsigned long ulRow) {
    return &mpstCells[ulRow * mulNumColumns + ulColumn];
}
