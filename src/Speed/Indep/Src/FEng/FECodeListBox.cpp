#include <new>

#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void (*FECodeListBox::mpDefaultCallback)(FECodeListBox*) = FECodeListBox::DefaultSelectCallback;

FECodeListBox::FECodeListBox()
    : mpobRenderer(nullptr) //
    , mulNumVisibleColumns(0) //
    , mulNumVisibleRows(0) //
    , mulFlags(2) //
    , mulNumTotalColumns(0) //
    , mulNumTotalRows(0) //
    , mulCurrentVirtualColumn(0) //
    , mulCurrentVirtualRow(0) //
    , mulTargetColumn(0) //
    , mulTargetRow(0) //
    , mstViewDimensions(0.0f) //
    , mpstCells(nullptr) //
    , mulNumStrings(0) //
    , mulStringSize(0) //
    , mulCurrentString(0) //
    , mppsStringData(nullptr) //
    , mpsStrings(nullptr) //
    , mfCurrentAlpha(1.0f) //
    , mfAlphaDelta(-0.000694f) //
    , mstSelectionColor(0xFFFFFFFF) //
    , mpSelectionCallback(mpDefaultCallback) //
    , mpSetCellCallback(nullptr) //
    , mpvCallbackData(nullptr) {
    Type = FE_CodeList;
}

FECodeListBox::~FECodeListBox() {
    if (mpstCells) {
        delete[] mpstCells;
        mpstCells = nullptr;
    }
    if (mppsStringData) {
        delete[] mppsStringData;
        mppsStringData = nullptr;
    }
    if (mpsStrings) {
        delete[] mpsStrings;
        mpsStrings = nullptr;
    }
}

FEObject* FECodeListBox::Clone(bool bReference) {
    FECodeListBox* pNew = static_cast<FECodeListBox*>(FEngMalloc(sizeof(FECodeListBox), 0, 0));
    if (pNew) {
        new (pNew) FECodeListBox(*this, bReference);
    }
    return pNew;
}

void FECodeListBox::SetTotalNumColumns(unsigned long ulNumColumns) {
    mulNumTotalColumns = ulNumColumns;
    if (mulCurrentVirtualColumn >= ulNumColumns) {
        mulCurrentVirtualColumn = ulNumColumns - 1;
    }
    mulTargetColumn = mulCurrentVirtualColumn;
}

void FECodeListBox::SetTotalNumRows(unsigned long ulNumRows) {
    mulNumTotalRows = ulNumRows;
    if (mulCurrentVirtualRow >= ulNumRows) {
        mulCurrentVirtualRow = ulNumRows - 1;
    }
    mulTargetRow = mulCurrentVirtualRow;
}

void FECodeListBox::ScrollSelection(long lColumnNum, long lRowNum) {
    ScrollSelection(lColumnNum, mulCurrentVirtualColumn, mulTargetColumn, mulNumTotalColumns, mulNumVisibleColumns, true);
    ScrollSelection(lRowNum, mulCurrentVirtualRow, mulTargetRow, mulNumTotalRows, mulNumVisibleRows, false);
}

short* FECodeListBox::AllocateString() {
    short* psRet = mppsStringData[mulCurrentString];
    mulCurrentString++;
    return psRet;
}

void FECodeListBox::DeallocateString(short* psString) {
    mulCurrentString--;
    mppsStringData[mulCurrentString] = psString;
}

void FECodeListBox::DefaultSelectCallback(FECodeListBox* pList) {
    unsigned long col = pList->mulCurrentVirtualColumn;
    unsigned long row = pList->mulCurrentVirtualRow;
    unsigned long visCol = col;
    unsigned long visRow = row;
    if (col >= pList->mulNumVisibleColumns) {
        visCol = col % pList->mulNumVisibleColumns;
    }
    if (row >= pList->mulNumVisibleRows) {
        visRow = row % pList->mulNumVisibleRows;
    }
    FEListBoxCell* pCell = &pList->mpstCells[visRow * pList->mulNumVisibleColumns + visCol];
    pCell->ulColor = static_cast<unsigned long>(pList->mstSelectionColor);
}

long FECodeListBox::GetRealColumn(long lColumn) const {
    if (lColumn < 0) {
        lColumn += static_cast<long>(mulNumTotalColumns);
        if (lColumn < 0) {
            lColumn = 0;
        }
    } else if (lColumn >= static_cast<long>(mulNumTotalColumns)) {
        lColumn -= static_cast<long>(mulNumTotalColumns);
        if (lColumn >= static_cast<long>(mulNumTotalColumns)) {
            lColumn = static_cast<long>(mulNumTotalColumns) - 1;
        }
    }
    return lColumn;
}

long FECodeListBox::GetRealRow(long lRow) const {
    if (lRow < 0) {
        lRow += static_cast<long>(mulNumTotalRows);
        if (lRow < 0) {
            lRow = 0;
        }
    } else if (lRow >= static_cast<long>(mulNumTotalRows)) {
        lRow -= static_cast<long>(mulNumTotalRows);
        if (lRow >= static_cast<long>(mulNumTotalRows)) {
            lRow = static_cast<long>(mulNumTotalRows) - 1;
        }
    }
    return lRow;
}

void FECodeListBox::CalculateCurrentFromTarget(unsigned long ulTarget, unsigned long ulTotal, unsigned long ulVisible) {
    if (ulTarget < ulVisible) {
        return;
    }
    unsigned long ulMax = ulTotal - ulVisible;
    if (ulTarget > ulMax) {
        ulTarget = ulMax;
    }
}
