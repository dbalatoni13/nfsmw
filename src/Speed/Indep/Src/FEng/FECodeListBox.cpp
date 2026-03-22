#include <new>

#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"

template <class T> void CopyString(short* pDst, const T* pSrc);
template <class T> void CopyString(short* pDst, const T* pSrc, unsigned long ulMaxLength);

static inline int GetValidIndexListBox(int lIndex, int lRange) {
    if (lIndex >= 0) {
        int rem = lIndex - (lIndex / lRange) * lRange;
        return rem;
    }

    int posIndex = -lIndex;
    int rem = posIndex - (posIndex / lRange) * lRange;
    if (lRange <= 1) {
        return 0;
    }
    return lRange - rem;
}

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
    , mstViewDimensions(0.0f, 0.0f) //
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
    , mulFlags(0) //
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
    }
    if (mpsStrings) {
        delete[] mpsStrings;
    }
    if (mppsStringData) {
        delete[] mppsStringData;
    }
}

void FECodeListBox::CopyProperties(const FECodeListBox& Object) {
    unsigned long ulNumCells;
    mulFlags |= Object.mulFlags & 0xE;
    mstViewDimensions = Object.mstViewDimensions;
    mulNumTotalColumns = Object.mulNumTotalColumns;
    mulNumTotalRows = Object.mulNumTotalRows;
    Initialize(Object.mulNumVisibleColumns, Object.mulNumVisibleRows);
    if (mpsStrings) {
        delete[] mpsStrings;
        mpsStrings = nullptr;
    }
    if (mppsStringData) {
        delete[] mppsStringData;
        mppsStringData = nullptr;
    }
    mulNumStrings = 0;
    mulCurrentString = 0;
    mulStringSize = 0;
    AllocateStrings(Object.mulNumStrings, Object.mulStringSize);
    ulNumCells = mulNumVisibleColumns * mulNumVisibleRows;
    for (unsigned long i = 0; i < ulNumCells; i++) {
        mpstCells[i].ulColor = Object.mpstCells[i].ulColor;
        mpstCells[i].ulType = Object.mpstCells[i].ulType;
        mpstCells[i].stScale = Object.mpstCells[i].stScale;
        mpstCells[i].stResource.Handle = Object.mpstCells[i].stResource.Handle;
        mpstCells[i].stResource.ResourceIndex = Object.mpstCells[i].stResource.ResourceIndex;
        mpstCells[i].stResource.UserParam = Object.mpstCells[i].stResource.UserParam;
        mpstCells[i].ulJustification = Object.mpstCells[i].ulJustification;
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
    long ulNumCells = ulNumVisRows * ulNumVisCols;
    mpstCells = FENG_NEW FEListBoxCell[ulNumCells];
    FEListBox::InitializeCell(mpstCells, mulNumVisibleRows * mulNumVisibleColumns);
    SetTotalNumColumns(mulNumVisibleColumns);
    SetTotalNumRows(mulNumVisibleRows);
    if (mulFlags & 1) {
        unsigned long ulNumColumns = ulOldNumVisibleColumns;
        if (ulOldNumVisibleColumns > mulNumVisibleColumns) {
            ulNumColumns = mulNumVisibleColumns;
        }
        unsigned long ulNumRows = ulOldNumVisibleRows;
        if (ulOldNumVisibleRows > mulNumVisibleRows) {
            ulNumRows = mulNumVisibleRows;
        }
        if (pstOldCells) {
            for (unsigned long i = 0; i < ulNumRows; i++) {
                FEngMemCpy(mpstCells + i * mulNumVisibleColumns, pstOldCells + i * ulOldNumVisibleColumns, mulNumVisibleColumns * sizeof(FEListBoxCell));
                for (unsigned long j = ulNumColumns; j < ulOldNumVisibleColumns; j++) {
                    FEListBoxCell* pOldCell = &pstOldCells[i * ulOldNumVisibleColumns + j];
                    if (pOldCell->ulType == 2) {
                        DeallocateString(pOldCell->u.string.pStr);
                    }
                }
            }
            for (unsigned long i = ulNumRows; i < ulOldNumVisibleRows; i++) {
                for (unsigned long j = 0; j < ulOldNumVisibleColumns; j++) {
                    FEListBoxCell* pOldCell = &pstOldCells[i * ulOldNumVisibleColumns + j];
                    if (pOldCell->ulType == 2) {
                        DeallocateString(pOldCell->u.string.pStr);
                    }
                }
            }
            ulNumRows = ulOldNumVisibleRows;
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
    int lStartColumn = mulCurrentVirtualRow;
    int lRow = mulCurrentVirtualColumn;
    if (ulNumVisRows > mulNumTotalRows) {
        ulNumVisRows = mulNumTotalRows;
    }
    unsigned long ulNumVisCols = mulNumVisibleColumns;
    if (ulNumVisCols > mulNumTotalColumns) {
        ulNumVisCols = mulNumTotalColumns;
    }
    if (mpSetCellCallback) {
        unsigned long i = 0;
        if (i < ulNumVisRows) {
            do {
                int lColumn = lRow;
                unsigned long j = 0;
                if (j < ulNumVisCols) {
                    do {
                        mpSetCellCallback(mpvCallbackData, this, lColumn, lStartColumn);
                        lColumn = GetValidIndexListBox(lColumn + 1, mulNumTotalColumns);
                        j++;
                    } while (j < ulNumVisCols);
                }
                lStartColumn = GetValidIndexListBox(lStartColumn + 1, mulNumTotalRows);
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
                            lColumn = GetValidIndexListBox(lColumn + 1, mulNumTotalColumns);
                            j++;
                        } while (j < ulNumVisCols);
                    }
                    lStartColumn = GetValidIndexListBox(lStartColumn + 1, mulNumTotalRows);
                    i++;
                } while (i < ulNumVisRows);
            }
        }
    }
}

void FECodeListBox::AllocateStrings(unsigned long ulNumStrings, unsigned long ulStringSize) {
    short* psOldStrings = mpsStrings;
    short** ppsOldStringData = mppsStringData;
    unsigned long i = 0;
    unsigned long j = 0;
    mulNumStrings = 0;
    mulCurrentString = 0;
    mulStringSize = 0;
    mpsStrings = nullptr;
    mppsStringData = nullptr;
    if (ulNumStrings == 0 || ulStringSize == 0) {
        if (i < mulNumVisibleRows) {
            do {
                j = 0;
                i++;
                if (j < mulNumVisibleColumns) {
                    do {
                        j++;
                    } while (j < mulNumVisibleColumns);
                }
            } while (i < mulNumVisibleRows);
        }
    } else {
        mpsStrings = static_cast<short*>(FEngMalloc((ulNumStrings * ulStringSize) << 1, 0, 0));
        mppsStringData = static_cast<short**>(FEngMalloc(ulNumStrings * 4, 0, 0));
        FEngMemSet(mpsStrings, 0, ulNumStrings * (ulStringSize + ulStringSize));
        for (i = 0; i < ulNumStrings; i++) {
            mppsStringData[i] = mpsStrings + i * ulStringSize;
        }
        mulNumStrings = ulNumStrings;
        mulStringSize = ulStringSize;
        if (!psOldStrings) {
            goto cleanup_ptrs;
        }
        if (ppsOldStringData) {
            i = 0;
            if (i < mulNumVisibleRows) {
                do {
                    j = 0;
                    if (j < mulNumVisibleColumns) {
                        do {
                            FEListBoxCell* pstCell = GetCellData(j, i);
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
    return FENG_NEW FECodeListBox(*this, bReference);
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
            if (0 < lNumMove) {
                unsigned long NumColumns = mulNumVisibleColumns;
                unsigned long ulFillCell =
                    GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(NumColumns) - 1, static_cast<int>(mulNumTotalColumns));
                NumColumns--;
                if (mpSetCellCallback != nullptr) {
                    unsigned long r = 0;
                    while (r < mulNumVisibleRows) {
                        unsigned long c = 0;
                        short* psString = mpstCells[r * mulNumVisibleColumns].u.string.pStr;
                        while (c < NumColumns) {
                            unsigned long Index = r * mulNumVisibleColumns + c;
                            c++;
                            FEngMemCpy(&mpstCells[Index], &mpstCells[Index + 1], sizeof(FEListBoxCell));
                        }
                        mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                        mpSetCellCallback(
                            mpvCallbackData,
                            this,
                            ulFillCell,
                            GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                        r++;
                    }
                } else if (mpobRenderer) {
                    unsigned long r = 0;
                    while (r < mulNumVisibleRows) {
                        unsigned long c = 0;
                        short* psString = mpstCells[r * mulNumVisibleColumns].u.string.pStr;
                        while (c < NumColumns) {
                            unsigned long Index = r * mulNumVisibleColumns + c;
                            c++;
                            FEngMemCpy(&mpstCells[Index], &mpstCells[Index + 1], sizeof(FEListBoxCell));
                        }
                        mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                        mpobRenderer->SetCellData(
                            this,
                            ulFillCell,
                            GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                        r++;
                    }
                }
            } else if (mpSetCellCallback != nullptr) {
                unsigned long r = 0;
                while (r < mulNumVisibleRows) {
                    unsigned long NumColumns = mulNumVisibleColumns;
                    long c = NumColumns;
                    short* psString = mpstCells[NumColumns + r * NumColumns - 1].u.string.pStr;
                    while (--c != 0) {
                        unsigned long Index = r * mulNumVisibleColumns + c;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index - 1], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns].u.string.pStr = psString;
                    mpSetCellCallback(mpvCallbackData, this, mulCurrentVirtualColumn, GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                    r++;
                }
            } else if (mpobRenderer) {
                unsigned long r = 0;
                while (r < mulNumVisibleRows) {
                    unsigned long NumColumns = mulNumVisibleColumns;
                    long c = NumColumns;
                    short* psString = mpstCells[NumColumns + r * NumColumns - 1].u.string.pStr;
                    while (--c != 0) {
                        unsigned long Index = r * mulNumVisibleColumns + c;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index - 1], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns].u.string.pStr = psString;
                    mpobRenderer->SetCellData(this, mulCurrentVirtualColumn, GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                    r++;
                }
            }
        } else if (0 < lNumMove) {
            unsigned long NumRows = mulNumVisibleRows;
            unsigned long ulFillCell =
                GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(NumRows) - 1, static_cast<int>(mulNumTotalRows));
            NumRows--;
            if (mpSetCellCallback != nullptr) {
                unsigned long c = 0;
                while (c < mulNumVisibleColumns) {
                    unsigned long r = 0;
                    short* psString = mpstCells[c].u.string.pStr;
                    while (r < NumRows) {
                        unsigned long Index = r * mulNumVisibleColumns + c;
                        r++;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index + mulNumVisibleColumns], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                    mpSetCellCallback(
                        mpvCallbackData,
                        this,
                        GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(c), static_cast<int>(mulNumTotalColumns)),
                        ulFillCell);
                    c++;
                }
            } else if (mpobRenderer) {
                unsigned long c = 0;
                while (c < mulNumVisibleColumns) {
                    unsigned long r = 0;
                    short* psString = mpstCells[c].u.string.pStr;
                    while (r < NumRows) {
                        unsigned long Index = r * mulNumVisibleColumns + c;
                        r++;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index + mulNumVisibleColumns], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                    mpobRenderer->SetCellData(
                        this,
                        GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(c), static_cast<int>(mulNumTotalColumns)),
                        ulFillCell);
                    c++;
                }
            }
        } else if (mpSetCellCallback != nullptr) {
            unsigned long NumColumns = mulNumVisibleColumns;
            unsigned long c = 0;
            while (c < NumColumns) {
                long r = mulNumVisibleRows - 1;
                short* psString = mpstCells[r * NumColumns + c].u.string.pStr;
                while (r != 0) {
                    unsigned long Index = r * mulNumVisibleColumns + c;
                    FEngMemCpy(&mpstCells[Index], &mpstCells[Index - mulNumVisibleColumns], sizeof(FEListBoxCell));
                    r--;
                }
                mpstCells[c].u.string.pStr = psString;
                mpSetCellCallback(mpvCallbackData, this, mulCurrentVirtualColumn, GetValidIndex(static_cast<int>(mulCurrentVirtualRow), static_cast<int>(mulNumTotalRows)));
                NumColumns = mulNumVisibleColumns;
                c++;
            }
        } else if (mpobRenderer) {
            unsigned long NumColumns = mulNumVisibleColumns;
            unsigned long c = 0;
            while (c < NumColumns) {
                long r = mulNumVisibleRows - 1;
                short* psString = mpstCells[r * NumColumns + c].u.string.pStr;
                while (r != 0) {
                    unsigned long Index = r * mulNumVisibleColumns + c;
                    FEngMemCpy(&mpstCells[Index], &mpstCells[Index - mulNumVisibleColumns], sizeof(FEListBoxCell));
                    r--;
                }
                mpstCells[c].u.string.pStr = psString;
                mpobRenderer->SetCellData(this, mulCurrentVirtualColumn, GetValidIndex(static_cast<int>(mulCurrentVirtualRow), static_cast<int>(mulNumTotalRows)));
                NumColumns = mulNumVisibleColumns;
                c++;
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
    short* psRet = mppsStringData[mulCurrentString++];
    *psRet = 0;
    return psRet;
}

void FECodeListBox::DeallocateString(short* psString) {
    mulCurrentString--;
    mppsStringData[mulCurrentString] = psString;
}

void FECodeListBox::DefaultSelectCallback(FECodeListBox* pList) {
    FEColor stColor = pList->GetSelectionColor();
    stColor.a = static_cast<int>(pList->GetAlphaHilite() * 255.0f);
    pList->SetSelectionColor(stColor);
}

long FECodeListBox::GetRealColumn(long lColumn) const {
    return GetRealValue(lColumn, mulNumTotalColumns, mulCurrentVirtualColumn, mulNumVisibleColumns);
}

long FECodeListBox::GetRealRow(long lRow) const {
    return GetRealValue(lRow, mulNumTotalRows, mulCurrentVirtualRow, mulNumVisibleRows);
}

unsigned long FECodeListBox::CalculateCurrentFromTarget(unsigned long ulTarget, unsigned long ulNumTotal, unsigned long ulNumVisible) {
    if (ulTarget >= ulNumTotal) {
        ulTarget = 0;
        if (ulNumTotal != 0) {
            ulTarget = ulNumTotal - 1;
        }
    }
    int lRet = static_cast<int>(ulTarget);
    if (mulFlags & 8) {
        lRet = GetValidIndex(lRet - static_cast<int>(ulNumVisible >> 1), static_cast<int>(ulNumTotal));
    }
    return static_cast<unsigned long>(lRet);
}

void FECodeListBox::Update(float fNumTicks) {
    if (mpSelectionCallback) {
        mpSelectionCallback(this);
    }
    float fAlpha = mfCurrentAlpha + mfAlphaDelta * fNumTicks;
    mfCurrentAlpha = fAlpha;
    if (fAlpha < 0.0f) {
        mfCurrentAlpha = 0.0f;
        mfAlphaDelta = -mfAlphaDelta;
    } else if (fAlpha > 1.0f) {
        mfCurrentAlpha = 1.0f;
        mfAlphaDelta = -mfAlphaDelta;
    }
}

void FECodeListBox::SetCellColor(unsigned long ulStartColumn, unsigned long ulStartRow, unsigned long ulColor, unsigned long ulNumColumns, unsigned long ulNumRows) {
    unsigned long endRow = ulNumRows + ulStartRow;
    unsigned long i = ulStartRow;
    unsigned long endColumn = ulNumColumns + ulStartColumn;
    while (i < endRow) {
        unsigned long j = ulStartColumn;
        while (j < endColumn) {
            long lCIndex = GetRealColumn(j);
            long lRIndex = GetRealRow(i);
            FEListBoxCell* pstCell = &mpstCells[lRIndex * mulNumVisibleColumns + lCIndex];
            pstCell->ulColor = ulColor;
            j++;
        }
        i++;
    }
}

void FECodeListBox::SetCellScale(unsigned long ulStartColumn, unsigned long ulStartRow, const FEPoint& stScale, unsigned long ulNumColumns, unsigned long ulNumRows) {
    unsigned long endRow = ulNumRows + ulStartRow;
    unsigned long i = ulStartRow;
    unsigned long endColumn = ulNumColumns + ulStartColumn;
    while (i < endRow) {
        unsigned long j = ulStartColumn;
        while (j < endColumn) {
            long lCIndex = GetRealColumn(j);
            long lRIndex = GetRealRow(i);
            FEListBoxCell* pstCell = &mpstCells[lRIndex * mulNumVisibleColumns + lCIndex];
            pstCell->stScale = stScale;
            j++;
        }
        i++;
    }
}

void FECodeListBox::SetCellJustification(unsigned long ulStartColumn, unsigned long ulStartRow, unsigned long ulJustification, unsigned long ulNumColumns, unsigned long ulNumRows) {
    unsigned long endRow = ulNumRows + ulStartRow;
    unsigned long i = ulStartRow;
    unsigned long endColumn = ulNumColumns + ulStartColumn;
    while (i < endRow) {
        unsigned long j = ulStartColumn;
        while (j < endColumn) {
            long lCIndex = GetRealColumn(j);
            long lRIndex = GetRealRow(i);
            FEListBoxCell* pstCell = &mpstCells[lRIndex * mulNumVisibleColumns + lCIndex];
            pstCell->ulJustification = ulJustification;
            j++;
        }
        i++;
    }
}

bool FECodeListBox::CheckMovement(long lNumMove, long lCurrentVirtual, long lTarget, long lNumTotal, long lNumVis) {
    if ((mulFlags & 4) && lNumTotal <= lNumVis) {
        mpobRenderer->NotificationMessage(FEHashUpper("ListBound"), this, 0xFF, 0);
        mpobRenderer->NotificationMessage(FEHashUpper("ListEnd"), this, 0xFF, 0);
        return false;
    }
    if (!(mulFlags & 2)) {
        goto success;
    }
    if (mulFlags & 4) {
        if (lCurrentVirtual + lNumMove < 0) {
            mpobRenderer->NotificationMessage(FEHashUpper("ListBound"), this, 0xFF, 0);
            return false;
        }
        if (lCurrentVirtual + lNumMove < lNumTotal - lNumVis) {
            goto success;
        }
    } else {
        if (lNumMove + lTarget < 0) {
            mpobRenderer->NotificationMessage(FEHashUpper("ListBound"), this, 0xFF, 0);
            return false;
        }
        if (lNumMove + lTarget < lNumTotal) {
            goto success;
        }
    }
    mpobRenderer->NotificationMessage(FEHashUpper("ListEnd"), this, 0xFF, 0);
    return false;

success:
    return true;
}

bool FECodeListBox::MakeMove(long lNumMove, unsigned long& ulCurrentVirtual, unsigned long& ulTarget, unsigned long ulNumTotal, unsigned long ulNumVis) {
    if (mulFlags & 8) {
        ulCurrentVirtual = GetValidIndexListBox(static_cast<int>(ulCurrentVirtual) + lNumMove, ulNumTotal);
        ulTarget = GetValidIndexListBox(static_cast<int>(ulTarget) + lNumMove, ulNumTotal);
    } else if ((mulFlags & 6) == 6) {
        ulCurrentVirtual = GetValidIndexListBox(static_cast<int>(ulCurrentVirtual) + lNumMove, ulNumTotal);
        ulTarget = ulCurrentVirtual;
    } else {
        unsigned long ulOldTarget = ulTarget;
        ulTarget = GetValidIndexListBox(static_cast<int>(ulOldTarget) + lNumMove, ulNumTotal);
        if (lNumMove < 0) {
            if (ulCurrentVirtual != ulOldTarget) {
                return false;
            }
            ulCurrentVirtual = ulTarget;
        } else {
            unsigned long ulDifference;
            if (ulCurrentVirtual == ulOldTarget) {
                return false;
            }
            if (ulCurrentVirtual < ulTarget) {
                ulDifference = ulTarget - ulCurrentVirtual;
            } else {
                ulDifference = ulTarget + ulNumTotal - ulCurrentVirtual;
            }
            if (ulDifference < ulNumVis) {
                return false;
            }
            ulDifference = GetValidIndexListBox(static_cast<int>(ulCurrentVirtual) + lNumMove, ulNumTotal);
            ulCurrentVirtual = ulDifference;
        }
    }
    return true;
}
