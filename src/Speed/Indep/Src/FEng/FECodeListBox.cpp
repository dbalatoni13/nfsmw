#include <new>

#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"

template <class T> void CopyString(short* pDst, const T* pSrc);
template <class T> void CopyString(short* pDst, const T* pSrc, unsigned long ulMaxLength);

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

FECodeListBox::FECodeListBox(const FECodeListBox& Object, bool bReference)
    : FEObject(Object, bReference) //
    , mpobRenderer(Object.mpobRenderer) //
    , mulNumVisibleColumns(0) //
    , mulNumVisibleRows(0) //
    , mulFlags(Object.mulFlags) //
    , mulNumTotalColumns(Object.mulNumTotalColumns) //
    , mulNumTotalRows(Object.mulNumTotalRows) //
    , mulCurrentVirtualColumn(0) //
    , mulCurrentVirtualRow(0) //
    , mulTargetColumn(0) //
    , mulTargetRow(0) //
    , mstViewDimensions(Object.mstViewDimensions) //
    , mpstCells(nullptr) //
    , mulNumStrings(0) //
    , mulStringSize(0) //
    , mulCurrentString(0) //
    , mppsStringData(nullptr) //
    , mpsStrings(nullptr) //
    , mfCurrentAlpha(1.0f) //
    , mfAlphaDelta(-0.001389f) //
    , mstSelectionColor(0xFFFFFFFF) //
    , mpSelectionCallback(Object.mpSelectionCallback) //
    , mpSetCellCallback(Object.mpSetCellCallback) //
    , mpvCallbackData(Object.mpvCallbackData) {
    CopyProperties(Object);
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

void FECodeListBox::CopyProperties(const FECodeListBox& Object) {
    unsigned long ulNumCells;
    mulFlags |= Object.mulFlags & 0xE;
    mstViewDimensions = Object.mstViewDimensions;
    mulNumTotalColumns = Object.mulNumTotalColumns;
    mulNumTotalRows = Object.mulNumTotalRows;
    Initialize(Object.mulNumVisibleColumns, Object.mulNumVisibleRows);
    if (mppsStringData) {
        delete[] mppsStringData;
        mppsStringData = nullptr;
    }
    if (mpsStrings) {
        delete[] mpsStrings;
        mpsStrings = nullptr;
    }
    mulStringSize = 0;
    mulNumStrings = 0;
    mulCurrentString = 0;
    AllocateStrings(Object.mulNumStrings, Object.mulStringSize);
    ulNumCells = mulNumVisibleColumns * mulNumVisibleRows;
    for (unsigned long i = 0; i < ulNumCells; i++) {
        mpstCells[i].ulColor = Object.mpstCells[i].ulColor;
        mpstCells[i].ulJustification = Object.mpstCells[i].ulJustification;
        mpstCells[i].stScale = Object.mpstCells[i].stScale;
        mpstCells[i].stResource = Object.mpstCells[i].stResource;
        mpstCells[i].ulType = Object.mpstCells[i].ulType;
        if (mpstCells[i].ulType == 2) {
            short* psString = Object.mpstCells[i].u.string.pStr;
            if (!psString) {
                return;
            }
            mpstCells[i].u.string.pStr = AllocateString();
            CopyString(mpstCells[i].u.string.pStr, psString);
        }
        if (mpstCells[i].ulType == 1) {
            mpstCells[i].SetUV() = Object.mpstCells[i].GetUV();
        }
    }
}

void FECodeListBox::Initialize(unsigned long ulNumVisCols, unsigned long ulNumVisRows) {
    FEListBoxCell* pstOldCells = mpstCells;
    unsigned long ulOldNumVisibleColumns = mulNumVisibleColumns;
    unsigned long ulOldNumVisibleRows = mulNumVisibleRows;
    mulNumVisibleColumns = ulNumVisCols;
    mulNumVisibleRows = ulNumVisRows;
    long ulNumCells = ulNumVisCols * ulNumVisRows;
    mpstCells = static_cast<FEListBoxCell*>(FEngMalloc(ulNumCells * sizeof(FEListBoxCell), 0, 0));
    unsigned long* puVar3 = reinterpret_cast<unsigned long*>(mpstCells);
    for (long n = ulNumCells; n != 0; n--) {
        puVar3[0] = 0;
        puVar3[1] = 0x3F800000;
        puVar3[2] = 0x3F800000;
        puVar3[3] = 0;
        puVar3[4] = 0;
        puVar3[5] = 0;
        puVar3[6] = 0;
        puVar3[8] = 0;
        puVar3[9] = 0xFFFFFFFF;
        puVar3 += 12;
    }
    FEListBox::InitializeCell(mpstCells, mulNumVisibleRows * mulNumVisibleColumns);
    SetTotalNumColumns(mulNumVisibleColumns);
    SetTotalNumRows(mulNumVisibleRows);
    if (mulFlags & 1) {
        unsigned long ulNumColumns = ulOldNumVisibleColumns;
        if (mulNumVisibleColumns < ulOldNumVisibleColumns) {
            ulNumColumns = mulNumVisibleColumns;
        }
        unsigned long ulNumRows = ulOldNumVisibleRows;
        if (mulNumVisibleRows < ulOldNumVisibleRows) {
            ulNumRows = mulNumVisibleRows;
        }
        if (pstOldCells) {
            unsigned long i = 0;
            if (ulNumRows != 0) {
                do {
                    FEngMemCpy(mpstCells + i * mulNumVisibleColumns, pstOldCells + i * ulOldNumVisibleColumns, mulNumVisibleColumns * sizeof(FEListBoxCell));
                    for (unsigned long j = ulNumColumns; j < ulOldNumVisibleColumns; j++) {
                        FEListBoxCell* pOldCell = &pstOldCells[i * ulOldNumVisibleColumns + j];
                        if (pOldCell->ulType == 2) {
                            DeallocateString(pOldCell->u.string.pStr);
                        }
                    }
                    i++;
                } while (i < ulNumRows);
            }
            while (ulNumRows < ulOldNumVisibleRows) {
                unsigned long j = 0;
                unsigned long ulNextRow = ulNumRows + 1;
                if (ulOldNumVisibleColumns != 0) {
                    do {
                        FEListBoxCell* pOldCell = &pstOldCells[ulNumRows * ulOldNumVisibleColumns + j];
                        if (pOldCell->ulType == 2) {
                            DeallocateString(pOldCell->u.string.pStr);
                        }
                        j++;
                    } while (j < ulOldNumVisibleColumns);
                }
                ulNumRows = ulNextRow;
            }
            if (pstOldCells) {
                delete[] pstOldCells;
            }
        }
    }
    mulFlags |= 1;
}

void FECodeListBox::FillAllCells() {
    if (!mulNumTotalColumns || !mulNumTotalRows || !mulNumVisibleRows || !mulNumVisibleColumns) {
        return;
    }
    unsigned long ulNumVisRows = mulNumVisibleRows;
    if (ulNumVisRows > mulNumTotalRows) {
        ulNumVisRows = mulNumTotalRows;
    }
    unsigned long ulNumVisCols = mulNumVisibleColumns;
    if (ulNumVisCols > mulNumTotalColumns) {
        ulNumVisCols = mulNumTotalColumns;
    }
    int lStartColumn = mulCurrentVirtualColumn;
    int lRow = mulCurrentVirtualRow;
    if (mpSetCellCallback) {
        unsigned long i = 0;
        if (i < ulNumVisRows) {
            do {
                int lColumn = lRow;
                unsigned long j = 0;
                if (j < ulNumVisCols) {
                    do {
                        mpSetCellCallback(mpvCallbackData, this, lColumn, lStartColumn);
                        lColumn = GetValidIndex(lColumn + 1, mulNumTotalColumns);
                        j++;
                    } while (j < ulNumVisCols);
                }
                lStartColumn = GetValidIndex(lStartColumn + 1, mulNumTotalRows);
                i++;
            } while (i < ulNumVisRows);
        }
    } else {
        if (mpobRenderer) {
            unsigned long i = 0;
            if (i < ulNumVisRows) {
                do {
                    int lColumn = lRow;
                    unsigned long j = 0;
                    if (j < ulNumVisCols) {
                        do {
                            mpobRenderer->SetCellData(this, lColumn, lStartColumn);
                            lColumn = GetValidIndex(lColumn + 1, mulNumTotalColumns);
                            j++;
                        } while (j < ulNumVisCols);
                    }
                    lStartColumn = GetValidIndex(lStartColumn + 1, mulNumTotalRows);
                    i++;
                } while (i < ulNumVisRows);
            }
        }
    }
}

void FECodeListBox::AllocateStrings(unsigned long ulNumStrings, unsigned long ulStringSize) {
    short* psOldStrings = mpsStrings;
    short** ppsOldStringData = mppsStringData;
    mulNumStrings = 0;
    mulCurrentString = 0;
    mulStringSize = 0;
    mppsStringData = nullptr;
    mpsStrings = nullptr;
    if (ulNumStrings == 0 || ulStringSize == 0) {
        unsigned long i = 0;
        if (mulNumVisibleRows != 0) {
            do {
                unsigned long j = 0;
                i++;
                if (mulNumVisibleColumns != 0) {
                    do {
                        j++;
                    } while (j < mulNumVisibleColumns);
                }
            } while (i < mulNumVisibleRows);
        }
    } else {
        mpsStrings = static_cast<short*>(FEngMalloc(ulNumStrings * ulStringSize * 2, 0, 0));
        mppsStringData = static_cast<short**>(FEngMalloc(ulNumStrings * 4, 0, 0));
        FEngMemSet(mpsStrings, 0, ulNumStrings * ulStringSize * 2);
        for (unsigned long i = 0; i < ulNumStrings; i++) {
            mppsStringData[i] = mpsStrings + i * ulStringSize;
        }
        mulNumStrings = ulNumStrings;
        mulStringSize = ulStringSize;
        if (!psOldStrings) {
            goto cleanup_ptrs;
        }
        if (ppsOldStringData) {
            unsigned long i = 0;
            if (mulNumVisibleRows != 0) {
                do {
                    unsigned long j = 0;
                    if (mulNumVisibleColumns != 0) {
                        do {
                            FEListBoxCell* pstCell = GetRealCellData(j, i);
                            if (pstCell->ulType == 2) {
                                short* psString = pstCell->u.string.pStr;
                                pstCell->u.string.pStr = AllocateString();
                                CopyString(pstCell->u.string.pStr, psString, ulStringSize);
                            }
                            j++;
                        } while (j < mulNumVisibleColumns);
                    }
                    i++;
                } while (i < mulNumVisibleRows);
            }
        }
    }
    if (psOldStrings) {
        delete[] psOldStrings;
    }
cleanup_ptrs:
    if (ppsOldStringData) {
        delete[] ppsOldStringData;
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
    mulCurrentVirtualColumn = CalculateCurrentFromTarget(mulTargetColumn, ulNumColumns, mulNumVisibleColumns);
}

void FECodeListBox::SetTotalNumRows(unsigned long ulNumRows) {
    mulNumTotalRows = ulNumRows;
    mulCurrentVirtualRow = CalculateCurrentFromTarget(mulTargetRow, ulNumRows, mulNumVisibleRows);
}

bool FECodeListBox::ScrollSelection(long lNumMove, unsigned long& ulCurrentVirtual, unsigned long& ulTarget, unsigned long ulNumTotal, unsigned long ulNumVis, bool bColumn) {
    if (lNumMove == 0) return false;
    if (!CheckMovement(lNumMove, ulCurrentVirtual, ulTarget, ulNumTotal, ulNumVis)) return false;
    if (!MakeMove(lNumMove, ulCurrentVirtual, ulTarget, ulNumTotal, ulNumVis)) return false;

    if (ulNumTotal != 0) {
        if (bColumn) {
            if (lNumMove < 0) {
                // Column scrolling left
                if (mpSetCellCallback == nullptr) {
                    if (mpobRenderer) {
                        unsigned long NumColumns = mulNumVisibleColumns;
                        unsigned long ulFillCell;
                        for (unsigned long r = 0; r < mulNumVisibleRows; r++) {
                            ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[NumColumns + r * NumColumns - 1])[8];
                            long c = NumColumns - 1;
                            while (c != 0) {
                                unsigned long idx = r * mulNumVisibleColumns + c;
                                FEngMemCpy(&mpstCells[idx], &mpstCells[idx - 1], sizeof(FEListBoxCell));
                                c--;
                            }
                            reinterpret_cast<unsigned long*>(&mpstCells[r * mulNumVisibleColumns])[8] = ulFillCell;
                            int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows));
                            mpobRenderer->SetCellData(this, mulCurrentVirtualColumn, rowIdx);
                        }
                    }
                } else {
                    if (mulNumVisibleRows != 0) {
                        unsigned long NumColumns = mulNumVisibleColumns;
                        unsigned long ulFillCell;
                        for (unsigned long r = 0; r < mulNumVisibleRows; r++) {
                            ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[NumColumns + r * NumColumns - 1])[8];
                            long c = NumColumns - 1;
                            while (c != 0) {
                                unsigned long idx = r * mulNumVisibleColumns + c;
                                FEngMemCpy(&mpstCells[idx], &mpstCells[idx - 1], sizeof(FEListBoxCell));
                                c--;
                            }
                            reinterpret_cast<unsigned long*>(&mpstCells[r * mulNumVisibleColumns])[8] = ulFillCell;
                            int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows));
                            mpSetCellCallback(mpvCallbackData, this, mulCurrentVirtualColumn, rowIdx);
                        }
                    }
                }
            } else {
                // Column scrolling right
                unsigned long NumColumns = mulNumVisibleColumns;
                int colIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(NumColumns) - 1, static_cast<int>(mulNumTotalColumns));
                if (mpSetCellCallback == nullptr) {
                    if (mpobRenderer && mulNumVisibleRows != 0) {
                        for (unsigned long r = 0; r < mulNumVisibleRows; r++) {
                            unsigned long c = 0;
                            unsigned long ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[r * mulNumVisibleColumns])[8];
                            if (NumColumns != 1) {
                                do {
                                    unsigned long idx = r * mulNumVisibleColumns + c;
                                    c++;
                                    FEngMemCpy(&mpstCells[idx], &mpstCells[idx + 1], sizeof(FEListBoxCell));
                                } while (c < NumColumns - 1u);
                            }
                            reinterpret_cast<unsigned long*>(&mpstCells[c * 1 + r * mulNumVisibleColumns])[8] = ulFillCell;
                            int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows));
                            mpobRenderer->SetCellData(this, colIdx, rowIdx);
                        }
                    }
                } else if (mulNumVisibleRows != 0) {
                    for (unsigned long r = 0; r < mulNumVisibleRows; r++) {
                        unsigned long c = 0;
                        unsigned long ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[r * mulNumVisibleColumns])[8];
                        if (NumColumns != 1) {
                            do {
                                unsigned long idx = r * mulNumVisibleColumns + c;
                                c++;
                                FEngMemCpy(&mpstCells[idx], &mpstCells[idx + 1], sizeof(FEListBoxCell));
                            } while (c < NumColumns - 1u);
                        }
                        reinterpret_cast<unsigned long*>(&mpstCells[c * 1 + r * mulNumVisibleColumns])[8] = ulFillCell;
                        int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows));
                        mpSetCellCallback(mpvCallbackData, this, colIdx, rowIdx);
                    }
                }
            }
        } else {
            if (lNumMove < 0) {
                // Row scrolling down
                if (mpSetCellCallback == nullptr) {
                    if (mpobRenderer) {
                        unsigned long NumColumns = mulNumVisibleColumns;
                        unsigned long ulFillCell;
                        for (unsigned long c = 0; c < NumColumns; c++) {
                            long r = mulNumVisibleRows - 1;
                            ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[r * NumColumns + c])[8];
                            for (; r != 0; r--) {
                                unsigned long idx = r * mulNumVisibleColumns + c;
                                FEngMemCpy(&mpstCells[idx], &mpstCells[idx - mulNumVisibleColumns], sizeof(FEListBoxCell));
                            }
                            reinterpret_cast<unsigned long*>(&mpstCells[c])[8] = ulFillCell;
                            int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow), static_cast<int>(mulNumTotalRows));
                            mpobRenderer->SetCellData(this, mulCurrentVirtualColumn, rowIdx);
                            NumColumns = mulNumVisibleColumns;
                        }
                    }
                } else {
                    unsigned long NumColumns = mulNumVisibleColumns;
                    if (NumColumns != 0) {
                        unsigned long ulFillCell;
                        for (unsigned long c = 0; c < NumColumns; c++) {
                            long r = mulNumVisibleRows - 1;
                            ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[r * NumColumns + c])[8];
                            for (; r != 0; r--) {
                                unsigned long idx = r * mulNumVisibleColumns + c;
                                FEngMemCpy(&mpstCells[idx], &mpstCells[idx - mulNumVisibleColumns], sizeof(FEListBoxCell));
                            }
                            reinterpret_cast<unsigned long*>(&mpstCells[c])[8] = ulFillCell;
                            int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow), static_cast<int>(mulNumTotalRows));
                            mpSetCellCallback(mpvCallbackData, this, mulCurrentVirtualColumn, rowIdx);
                            NumColumns = mulNumVisibleColumns;
                        }
                    }
                }
            } else {
                // Row scrolling up
                unsigned long NumRows = mulNumVisibleRows;
                int rowIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(NumRows) - 1, static_cast<int>(mulNumTotalRows));
                if (mpSetCellCallback == nullptr) {
                    if (mpobRenderer && mulNumVisibleColumns != 0) {
                        for (unsigned long c = 0; c < mulNumVisibleColumns; c++) {
                            unsigned long r = 0;
                            unsigned long ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[c])[8];
                            if (NumRows != 1) {
                                do {
                                    unsigned long idx = r * mulNumVisibleColumns + c;
                                    r++;
                                    FEngMemCpy(&mpstCells[idx], &mpstCells[idx + mulNumVisibleColumns], sizeof(FEListBoxCell));
                                } while (r < NumRows - 1u);
                            }
                            reinterpret_cast<unsigned long*>(&mpstCells[r * mulNumVisibleColumns + c])[8] = ulFillCell;
                            int colIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(c), static_cast<int>(mulNumTotalColumns));
                            mpobRenderer->SetCellData(this, colIdx, rowIdx);
                        }
                    }
                } else if (mulNumVisibleColumns != 0) {
                    for (unsigned long c = 0; c < mulNumVisibleColumns; c++) {
                        unsigned long r = 0;
                        unsigned long ulFillCell = reinterpret_cast<unsigned long*>(&mpstCells[c])[8];
                        if (NumRows != 1) {
                            do {
                                unsigned long idx = r * mulNumVisibleColumns + c;
                                r++;
                                FEngMemCpy(&mpstCells[idx], &mpstCells[idx + mulNumVisibleColumns], sizeof(FEListBoxCell));
                            } while (r < NumRows - 1u);
                        }
                        reinterpret_cast<unsigned long*>(&mpstCells[r * mulNumVisibleColumns + c])[8] = ulFillCell;
                        int colIdx = GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(c), static_cast<int>(mulNumTotalColumns));
                        mpSetCellCallback(mpvCallbackData, this, colIdx, rowIdx);
                    }
                }
            }
        }
    }
    return true;
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
    if (col >= pList->mulNumVisibleColumns) {
        col = col % pList->mulNumVisibleColumns;
    }
    if (row >= pList->mulNumVisibleRows) {
        row = row % pList->mulNumVisibleRows;
    }
    FEListBoxCell* pCell = &pList->mpstCells[row * pList->mulNumVisibleColumns + col];
    pCell->ulColor = static_cast<unsigned long>(pList->mstSelectionColor);
}

long FECodeListBox::GetRealColumn(long lColumn) const {
    if (mulNumTotalColumns == 0) return -1;
    if (lColumn >= static_cast<long>(mulNumTotalColumns)) {
        lColumn = lColumn % static_cast<long>(mulNumTotalColumns);
    }
    long diff = lColumn - static_cast<long>(mulCurrentVirtualColumn);
    if (diff < 0) {
        diff += static_cast<long>(mulNumTotalColumns);
    }
    return GetValidIndex(static_cast<int>(diff), static_cast<int>(mulNumVisibleColumns));
}

long FECodeListBox::GetRealRow(long lRow) const {
    if (mulNumTotalRows == 0) return -1;
    if (lRow >= static_cast<long>(mulNumTotalRows)) {
        lRow = lRow % static_cast<long>(mulNumTotalRows);
    }
    long diff = lRow - static_cast<long>(mulCurrentVirtualRow);
    if (diff < 0) {
        diff += static_cast<long>(mulNumTotalRows);
    }
    return GetValidIndex(static_cast<int>(diff), static_cast<int>(mulNumVisibleRows));
}

unsigned long FECodeListBox::CalculateCurrentFromTarget(unsigned long ulTarget, unsigned long ulTotal, unsigned long ulVisible) {
    if (ulTarget >= ulTotal) {
        if (ulTotal == 0) {
            ulTarget = 0;
        } else {
            ulTarget = ulTotal - 1;
        }
    }
    if (!(mulFlags & 8)) {
        return ulTarget;
    }
    return static_cast<unsigned long>(GetValidIndex(static_cast<int>(ulTarget) - static_cast<int>(ulVisible >> 1), static_cast<int>(ulTotal)));
}

void FECodeListBox::Update(float fNumTicks) {
    if (mpSelectionCallback) {
        mpSelectionCallback(this);
    }
    float fAlpha = mfCurrentAlpha + mfAlphaDelta * fNumTicks;
    mfCurrentAlpha = fAlpha;
    if (fAlpha < 0.0f || fAlpha > 1.0f) {
        mfCurrentAlpha = fAlpha < 0.0f ? 0.0f : 1.0f;
        mfAlphaDelta = -mfAlphaDelta;
    }
}

void FECodeListBox::SetCellColor(unsigned long ulStartColumn, unsigned long ulStartRow, unsigned long ulColor, unsigned long ulNumColumns, unsigned long ulNumRows) {
    for (unsigned long i = ulStartRow; i < ulStartRow + ulNumRows; i++) {
        for (unsigned long j = ulStartColumn; j < ulStartColumn + ulNumColumns; j++) {
            long lCIndex = GetRealColumn(j);
            long lRIndex = GetRealRow(i);
            mpstCells[lRIndex * mulNumVisibleColumns + lCIndex].ulColor = ulColor;
        }
    }
}

void FECodeListBox::SetCellScale(unsigned long ulStartColumn, unsigned long ulStartRow, const FEPoint& stScale, unsigned long ulNumColumns, unsigned long ulNumRows) {
    for (unsigned long i = ulStartRow; i < ulStartRow + ulNumRows; i++) {
        for (unsigned long j = ulStartColumn; j < ulStartColumn + ulNumColumns; j++) {
            long lCIndex = GetRealColumn(j);
            long lRIndex = GetRealRow(i);
            mpstCells[lRIndex * mulNumVisibleColumns + lCIndex].stScale = stScale;
        }
    }
}

void FECodeListBox::SetCellJustification(unsigned long ulStartColumn, unsigned long ulStartRow, unsigned long ulJustification, unsigned long ulNumColumns, unsigned long ulNumRows) {
    for (unsigned long i = ulStartRow; i < ulStartRow + ulNumRows; i++) {
        for (unsigned long j = ulStartColumn; j < ulStartColumn + ulNumColumns; j++) {
            long lCIndex = GetRealColumn(j);
            long lRIndex = GetRealRow(i);
            mpstCells[lRIndex * mulNumVisibleColumns + lCIndex].ulJustification = ulJustification;
        }
    }
}

bool FECodeListBox::CheckMovement(long lNumMove, long lCurrentVirtual, long lTarget, long lNumTotal, long lNumVis) {
    if ((mulFlags & 4) && lNumVis >= lNumTotal) {
        mpobRenderer->NotificationMessage(FEHashUpper("ListBound"), this, 0xFF, 0);
    } else if (mulFlags & 2) {
        if (!(mulFlags & 4)) {
            if (lCurrentVirtual + lNumMove < 0) {
                mpobRenderer->NotificationMessage(FEHashUpper("ListBound"), this, 0xFF, 0);
            } else if (lCurrentVirtual + lNumMove < lNumTotal) {
                return true;
            }
        } else {
            if (lTarget + lNumMove < 0) {
                mpobRenderer->NotificationMessage(FEHashUpper("ListBound"), this, 0xFF, 0);
            } else if (lTarget + lNumMove < lNumTotal - lNumVis) {
                return true;
            }
        }
    } else {
        return true;
    }
    mpobRenderer->NotificationMessage(FEHashUpper("ListEnd"), this, 0xFF, 0);
    return false;
}

bool FECodeListBox::MakeMove(long lNumMove, unsigned long& ulCurrentVirtual, unsigned long& ulTarget, unsigned long ulNumTotal, unsigned long ulNumVis) {
    if (mulFlags & 8) {
        long lIndex = static_cast<long>(ulCurrentVirtual) + lNumMove;
        ulCurrentVirtual = GetValidIndex(lIndex, ulNumTotal);
    } else if ((mulFlags & 6) == 6) {
        long lIndex = static_cast<long>(ulCurrentVirtual) + lNumMove;
        ulCurrentVirtual = GetValidIndex(lIndex, ulNumTotal);
    } else {
        unsigned long ulOldTarget = ulTarget;
        long lIndex = static_cast<long>(ulTarget) + lNumMove;
        ulTarget = GetValidIndex(lIndex, ulNumTotal);
        if (lNumMove < 0) {
            if (ulCurrentVirtual == ulOldTarget) {
                ulCurrentVirtual = GetValidIndex(static_cast<long>(ulCurrentVirtual) + lNumMove, ulNumTotal);
                return true;
            }
        } else {
            unsigned long ulDifference;
            if (ulCurrentVirtual < ulTarget) {
                ulDifference = ulTarget - ulCurrentVirtual;
            } else {
                ulDifference = ulTarget + ulNumTotal - ulCurrentVirtual;
            }
            if (ulDifference < ulNumVis) {
                return false;
            }
            long lNewIndex = static_cast<long>(ulCurrentVirtual) + lNumMove;
            ulCurrentVirtual = GetValidIndex(lNewIndex, ulNumTotal);
            return true;
        }
        ulCurrentVirtual = ulTarget;
    }
    return true;
}
