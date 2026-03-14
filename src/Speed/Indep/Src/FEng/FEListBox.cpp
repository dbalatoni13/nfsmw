#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

unsigned long GetStringLength(const short* pString);

FEListBox::FEListBox()
    : mulFlags(4) //
    , mulNumColumns(0) //
    , mulNumRows(0) //
    , mpstColumnData(nullptr) //
    , mpstRowData(nullptr) //
    , mulCurrentColumn(0) //
    , mulCurrentRow(0) //
    , mpstCells(nullptr) //
    , mfCurrentAlpha(0.0f) //
    , mfAlphaDelta(1.0f) {
    Type = FE_List;
    mstViewDimensions.h = 0.0f;
    mstViewDimensions.v = 0.0f;
    mstCurrentLocation.h = 0.0f;
    mstCurrentLocation.v = 0.0f;
    mstSelectionSpeed.h = 1.0f;
    mstSelectionSpeed.v = 1.0f;
    mstTargetLocation.h = 0.0f;
    mstTargetLocation.v = 0.0f;
    mstDirection.h = 0.0f;
    mstDirection.v = 0.0f;
}

FEListBox::~FEListBox() {
    Terminate();
}

void FEListBox::Terminate() {
    if (mulFlags & 1) {
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
        pstCells[i].stScale.h = 1.0f;
        pstCells[i].stScale.v = 1.0f;
        pstCells[i].stResource.Handle = 0;
        pstCells[i].stResource.UserParam = 0;
        pstCells[i].stResource.ResourceIndex = 0;
        pstCells[i].ulType = 0;
        pstCells[i].ulJustification = 0;
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
        mulNumColumns = 0;
        mpstColumnData = nullptr;
    }
}

void FEListBox::CleanupRows() {
    if (mulNumRows != 0) {
        if (mpstRowData) {
            delete[] mpstRowData;
        }
        mulNumRows = 0;
        mpstRowData = nullptr;
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
    if (mulNumColumns != 0) {
        do {
            pCol->fCummulativeValue = cumulative;
            i++;
            float val = pCol->fValue;
            pCol += 1;
            cumulative = cumulative + val;
        } while (i < mulNumColumns);
    }

    i = 0;
    FEListEntryData* pRow = mpstRowData;
    cumulative = 0.0f;
    if (mulNumRows == 0) {
        return;
    }
    do {
        pRow->fCummulativeValue = cumulative;
        i++;
        float val = pRow->fValue;
        pRow += 1;
        cumulative = cumulative + val;
    } while (i < mulNumRows);
}

void FEListBox::CompleteScroll() {
    mstCurrentLocation = mstTargetLocation;
    mulFlags &= ~0x62;
    if (mulCurrentColumn == 0) {
        mulFlags &= ~0x6A;
        mstCurrentLocation.h = 0.0f;
    }
    if (mulCurrentRow == 0) {
        mstCurrentLocation.v = 0.0f;
        mulFlags &= ~0x10;
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
