
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
// c34ord3 f1: el objetivo emite ~FEString y FEString::Clone (FEString.h:69 y
// :108) JUSTO DETRAS de FEListBox::GetPCellData (FEListBox.h:235) y delante de
// FEGroup: o sea que FEString.h se parsea en los primeros ficheros de la unidad,
// no en FEPackageReader.cpp (el 21), que es de donde nos llegaba. NO ordenar
// alfabeticamente: es POSICION en el bloque diferido.
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"

void (*FECodeListBox::mpDefaultCallback)(FECodeListBox *) = FECodeListBox::DefaultSelectCallback;

// Decl: speed/indep/src/feng/FECodeListBox.cpp (line 42)
FECodeListBox::FECodeListBox()
    : mpobRenderer(nullptr),                  //
      mulNumVisibleColumns(0),                //
      mulNumVisibleRows(0),                   //
      mulFlags(2),                            //
      mulNumTotalColumns(0),                  //
      mulNumTotalRows(0),                     //
      mulCurrentVirtualColumn(0),             //
      mulCurrentVirtualRow(0),                //
      mulTargetColumn(0),                     //
      mulTargetRow(0),                        //
      mstViewDimensions(0.0f, 0.0f),          //
      mpstCells(nullptr),                     //
      mulNumStrings(0),                       //
      mulStringSize(0),                       //
      mulCurrentString(0),                    //
      mppsStringData(nullptr),                //
      mpsStrings(nullptr),                    //
      mfCurrentAlpha(1.0f),                   //
      mfAlphaDelta(-1.0f / 720.0f),               //
      mstSelectionColor(0xFFFFFFFF),          //
      mpSelectionCallback(mpDefaultCallback), //
      mpSetCellCallback(nullptr),             //
      mpvCallbackData(nullptr) {
    Type = FE_CodeList;
}

// Decl:: speed/indep/src/feng/FECodeListBox.cpp (line 69)
FECodeListBox::FECodeListBox(const FECodeListBox &Object, bool bReference)
    : FEObject(Object, bReference),                    //
      mpobRenderer(Object.mpobRenderer),               //
      mulNumVisibleColumns(0),                         //
      mulNumVisibleRows(0),                            //
      mulFlags(0),                                     //
      mulNumTotalColumns(Object.mulNumTotalColumns),   //
      mulNumTotalRows(Object.mulNumTotalRows),         //
      mulCurrentVirtualColumn(0),                      //
      mulCurrentVirtualRow(0),                         //
      mulTargetColumn(0),                              //
      mulTargetRow(0),                                 //
      mstViewDimensions(Object.mstViewDimensions),     //
      mpstCells(nullptr),                              //
      mulNumStrings(0),                                //
      mulStringSize(0),                                //
      mulCurrentString(0),                             //
      mppsStringData(nullptr),                         //
      mpsStrings(nullptr),                             //
      mfCurrentAlpha(1.0f),                            //
      mfAlphaDelta(-1.0f / 720.0f),                        //
      mstSelectionColor(0xFFFFFFFF),                   //
      mpSelectionCallback(Object.mpSelectionCallback), //
      mpSetCellCallback(Object.mpSetCellCallback),     //
      mpvCallbackData(Object.mpvCallbackData) {
    CopyProperties(Object);
}

void FECodeListBox::CopyProperties(const FECodeListBox &Object) {
    u32 ulNumCells;
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
    for (u32 i = 0; i < ulNumCells; i++) {
        mpstCells[i].ulColor = Object.mpstCells[i].ulColor;
        mpstCells[i].ulType = Object.mpstCells[i].ulType;
        mpstCells[i].stScale = Object.mpstCells[i].stScale;
        mpstCells[i].stResource.Handle = Object.mpstCells[i].stResource.Handle;
        mpstCells[i].stResource.ResourceIndex = Object.mpstCells[i].stResource.ResourceIndex;
        mpstCells[i].stResource.UserParam = Object.mpstCells[i].stResource.UserParam;
        mpstCells[i].ulJustification = Object.mpstCells[i].ulJustification;
        if (mpstCells[i].ulType == 2) {
            short *psString = Object.mpstCells[i].u.string.pStr;
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

void FECodeListBox::Initialize(u32 ulNumVisCols, u32 ulNumVisRows) {
    FEListBoxCell *pstOldCells = mpstCells;
    u32 ulOldNumVisibleColumns = mulNumVisibleColumns;
    u32 ulOldNumVisibleRows = mulNumVisibleRows;
    mulNumVisibleColumns = ulNumVisCols;
    mulNumVisibleRows = ulNumVisRows;
    i32 ulNumCells = ulNumVisRows * ulNumVisCols;
    mpstCells = FNEW FEListBoxCell[ulNumCells];
    FEListBox::InitializeCell(mpstCells, mulNumVisibleRows * mulNumVisibleColumns);
    SetTotalNumColumns(mulNumVisibleColumns);
    SetTotalNumRows(mulNumVisibleRows);
    if (mulFlags & 1) {
        u32 ulNumColumns = ulOldNumVisibleColumns;
        if (ulOldNumVisibleColumns > mulNumVisibleColumns) {
            ulNumColumns = mulNumVisibleColumns;
        }
        u32 ulNumRows = ulOldNumVisibleRows;
        if (ulOldNumVisibleRows > mulNumVisibleRows) {
            ulNumRows = mulNumVisibleRows;
        }
        if (pstOldCells) {
            u32 i;
            u32 j;
            for (i = 0; i < ulNumRows; i++) {
                FEngMemCpy(mpstCells + i * mulNumVisibleColumns, pstOldCells + i * ulOldNumVisibleColumns,
                           mulNumVisibleColumns * sizeof(FEListBoxCell));
                for (j = ulNumColumns; j < ulOldNumVisibleColumns; j++) {
                    FEListBoxCell *pOldCell = &pstOldCells[i * ulOldNumVisibleColumns + j];
                    if (pOldCell->ulType == 2) {
                        DeallocateString(pOldCell->u.string.pStr);
                    }
                }
            }
            for (i = ulNumRows; i < ulOldNumVisibleRows; i++) {
                for (j = 0; j < ulOldNumVisibleColumns; j++) {
                    FEListBoxCell *pOldCell = &pstOldCells[i * ulOldNumVisibleColumns + j];
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

FEObject *FECodeListBox::Clone(bool bReference) {
    return FNEW FECodeListBox(*this, bReference);
}

void FECodeListBox::FillAllCells() {
    if (!mulNumTotalColumns || !mulNumTotalRows || !mulNumVisibleRows || !mulNumVisibleColumns) {
        return;
    }
    i32 lRow = mulCurrentVirtualRow;
    i32 lStartColumn = mulCurrentVirtualColumn;

    u32 ulNumRows = mulNumVisibleRows;
    if (ulNumRows > mulNumTotalRows) {
        ulNumRows = mulNumTotalRows;
    }

    u32 ulNumColumns = mulNumVisibleColumns;
    if (ulNumColumns > mulNumTotalColumns) {
        ulNumColumns = mulNumTotalColumns;
    }

    if (mpSetCellCallback != nullptr) {
        for (u32 i = 0; i < ulNumRows; i++) {
            i32 lColumn = lStartColumn;

            for (u32 j = 0; j < ulNumColumns; j++) {
                mpSetCellCallback(mpvCallbackData, this, lColumn, lRow);
                lColumn = GetValidIndex(lColumn + 1, mulNumTotalColumns);
            }
            lRow = GetValidIndex(lRow + 1, mulNumTotalRows);
        }
    } else if (mpobRenderer != nullptr) {
        for (u32 i = 0; i < ulNumRows; i++) {
            i32 lColumn = lStartColumn;

            for (u32 j = 0; j < ulNumColumns; j++) {
                mpobRenderer->SetCellData(this, lColumn, lRow);
                lColumn = GetValidIndex(lColumn + 1, mulNumTotalColumns);
            }
            lRow = GetValidIndex(lRow + 1, mulNumTotalRows);
        }
    }
}

void FECodeListBox::SetTotalNumColumns(u32 ulNumColumns) {
    mulNumTotalColumns = ulNumColumns;
    mulCurrentVirtualColumn = CalculateCurrentFromTarget(mulTargetColumn, ulNumColumns, mulNumVisibleColumns);
}

void FECodeListBox::SetTotalNumRows(u32 ulNumRows) {
    mulNumTotalRows = ulNumRows;
    mulCurrentVirtualRow = CalculateCurrentFromTarget(mulTargetRow, ulNumRows, mulNumVisibleRows);
}

void FECodeListBox::AllocateStrings(u32 ulNumStrings, u32 ulStringSize) {
    short *psOldStrings = mpsStrings;
    short **ppsOldStringData = mppsStringData;
    u32 i = 0;
    u32 j = 0;
    mulNumStrings = 0;
    mulCurrentString = 0;
    mulStringSize = 0;
    mpsStrings = nullptr;
    mppsStringData = nullptr;
    if (ulNumStrings == 0 || ulStringSize == 0) {
        if (i < mulNumVisibleRows) {
            do {
                j = 0;
                if (j < mulNumVisibleColumns) {
                    do {
                        j++;
                    } while (j < mulNumVisibleColumns);
                }
                i++;
            } while (i < mulNumVisibleRows);
        }
    } else {
        mpsStrings = static_cast<short *>(FEngMalloc((ulNumStrings * ulStringSize) << 1, 0, 0));
        mppsStringData = static_cast<short **>(FEngMalloc(ulNumStrings * 4, 0, 0));
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
            for (i = 0; i < mulNumVisibleRows; i++) {
                for (j = 0; j < mulNumVisibleColumns; j++) {
                    FEListBoxCell *pstCell = GetRealCellData(j, i);
                    short *psString;
                    if (pstCell->ulType == 2) {
                        psString = pstCell->u.string.pStr;
                        pstCell->u.string.pStr = AllocateString();
                        CopyString(pstCell->u.string.pStr, psString, ulStringSize);
                    }
                }
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

void FECodeListBox::ScrollSelection(i32 lColumnNum, i32 lRowNum) {
    ScrollSelection(lColumnNum, mulCurrentVirtualColumn, mulTargetColumn, mulNumTotalColumns, mulNumVisibleColumns, true);
    ScrollSelection(lRowNum, mulCurrentVirtualRow, mulTargetRow, mulNumTotalRows, mulNumVisibleRows, false);
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

void FECodeListBox::DefaultSelectCallback(FECodeListBox *pList) {
    FEColor stColor = pList->GetSelectionColor();
    stColor.a = static_cast<int>(pList->GetAlphaHilite() * 255.0f);
    pList->SetSelectionColor(stColor);
}

short *FECodeListBox::AllocateString() {
    short *psRet = mppsStringData[mulCurrentString++];
    *psRet = 0;
    return psRet;
}

void FECodeListBox::DeallocateString(short *psString) {
    mulCurrentString--;
    mppsStringData[mulCurrentString] = psString;
}

inline i32 GetRealValue(i32 i, i32 lNumTotal, i32 lCurrentVirtual, i32 lNumVisible) { // Decl: speed/indep/src/feng/FECodeListBox.cpp:807
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

    lRet = GetValidIndex(lRet, lNumVisible);
    return lRet;
}

i32 FECodeListBox::GetRealColumn(i32 lColumn) const {
    return GetRealValue(lColumn, mulNumTotalColumns, mulCurrentVirtualColumn, mulNumVisibleColumns);
}

i32 FECodeListBox::GetRealRow(i32 lRow) const {
    return GetRealValue(lRow, mulNumTotalRows, mulCurrentVirtualRow, mulNumVisibleRows);
}

bool FECodeListBox::CheckMovement(i32 lNumMove, i32 lCurrentVirtual, i32 lTarget, i32 lNumTotal, i32 lNumVis) {
    if ((mulFlags & 4) && lNumTotal <= lNumVis) {
        mpobRenderer->NotificationMessage(FEHashUpper("CLB AT MIN"), this, 0xFF, 0);
        mpobRenderer->NotificationMessage(FEHashUpper("CLB AT MAX"), this, 0xFF, 0);
        return false;
    }
    if (!(mulFlags & 2)) {
        goto success;
    }
    if (mulFlags & 4) {
        if (lCurrentVirtual + lNumMove < 0) {
            mpobRenderer->NotificationMessage(FEHashUpper("CLB AT MIN"), this, 0xFF, 0);
            return false;
        }
        if (lCurrentVirtual + lNumMove < lNumTotal - lNumVis) {
            goto success;
        }
    } else {
        if (lNumMove + lTarget < 0) {
            mpobRenderer->NotificationMessage(FEHashUpper("CLB AT MIN"), this, 0xFF, 0);
            return false;
        }
        if (lNumMove + lTarget < lNumTotal) {
            goto success;
        }
    }
    mpobRenderer->NotificationMessage(FEHashUpper("CLB AT MAX"), this, 0xFF, 0);
    return false;

success:
    return true;
}

bool FECodeListBox::MakeMove(i32 lNumMove, u32 &ulCurrentVirtual, u32 &ulTarget, u32 ulNumTotal, u32 ulNumVis) {
    if (mulFlags & 8) {
        ulCurrentVirtual = GetValidIndex(static_cast<int>(ulCurrentVirtual) + lNumMove, ulNumTotal);
        ulTarget = GetValidIndex(static_cast<int>(ulTarget) + lNumMove, ulNumTotal);
    } else if ((mulFlags & 6) == 6) {
        ulCurrentVirtual = GetValidIndex(static_cast<int>(ulCurrentVirtual) + lNumMove, ulNumTotal);
        ulTarget = ulCurrentVirtual;
    } else {
        u32 ulOldTarget = ulTarget;
        ulTarget = GetValidIndex(static_cast<int>(ulOldTarget) + lNumMove, ulNumTotal);
        if (lNumMove < 0) {
            if (ulCurrentVirtual != ulOldTarget) {
                return false;
            }
            ulCurrentVirtual = ulTarget;
        } else {
            u32 ulDifference;
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
            ulCurrentVirtual = GetValidIndex(static_cast<int>(ulCurrentVirtual) + lNumMove, ulNumTotal);
        }
    }
    return true;
}

bool FECodeListBox::ScrollSelection(i32 lNumMove, u32 &ulCurrentVirtual, u32 &ulTarget, u32 ulNumTotal, u32 ulNumVis, bool bColumn) {
    if (lNumMove == 0)
        return false;
    if (!CheckMovement(lNumMove, ulCurrentVirtual, ulTarget, ulNumTotal, ulNumVis))
        return false;
    if (!MakeMove(lNumMove, ulCurrentVirtual, ulTarget, ulNumTotal, ulNumVis))
        return false;

    if (ulNumTotal != 0) {
        if (bColumn) {
            if (0 < lNumMove) {
                u32 NumColumns = mulNumVisibleColumns - 1;
                u32 ulFillCell = GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(mulNumVisibleColumns) - 1,
                                               static_cast<int>(mulNumTotalColumns));
                if (mpSetCellCallback != nullptr) {
                    u32 r = 0;
                    while (r < mulNumVisibleRows) {
                        short *psString = mpstCells[r * mulNumVisibleColumns].u.string.pStr;
                        u32 c = 0;
                        while (c < NumColumns) {
                            u32 Index = r * mulNumVisibleColumns + c;
                            c++;
                            FEngMemCpy(&mpstCells[Index], &mpstCells[Index + 1], sizeof(FEListBoxCell));
                        }
                        mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                        mpSetCellCallback(
                            mpvCallbackData, this, ulFillCell,
                            GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                        r++;
                    }
                } else if (mpobRenderer) {
                    u32 r = 0;
                    while (r < mulNumVisibleRows) {
                        short *psString = mpstCells[r * mulNumVisibleColumns].u.string.pStr;
                        u32 c = 0;
                        while (c < NumColumns) {
                            u32 Index = r * mulNumVisibleColumns + c;
                            c++;
                            FEngMemCpy(&mpstCells[Index], &mpstCells[Index + 1], sizeof(FEListBoxCell));
                        }
                        mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                        mpobRenderer->SetCellData(
                            this, ulFillCell,
                            GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                        r++;
                    }
                }
            } else if (mpSetCellCallback != nullptr) {
                u32 r = 0;
                while (r < mulNumVisibleRows) {
                    short *psString = mpstCells[mulNumVisibleColumns + r * mulNumVisibleColumns - 1].u.string.pStr;
                    u32 c = mulNumVisibleColumns;
                    while (--c != 0) {
                        u32 Index = r * mulNumVisibleColumns + c;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index - 1], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                    mpSetCellCallback(mpvCallbackData, this, mulCurrentVirtualColumn,
                                      GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                    r++;
                }
            } else if (mpobRenderer) {
                u32 r = 0;
                while (r < mulNumVisibleRows) {
                    short *psString = mpstCells[mulNumVisibleColumns + r * mulNumVisibleColumns - 1].u.string.pStr;
                    u32 c = mulNumVisibleColumns;
                    while (--c != 0) {
                        u32 Index = r * mulNumVisibleColumns + c;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index - 1], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                    mpobRenderer->SetCellData(
                        this, mulCurrentVirtualColumn,
                        GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                    r++;
                }
            }
        } else if (0 < lNumMove) {
            u32 NumRows = mulNumVisibleRows - 1;
            u32 ulFillCell =
                GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(mulNumVisibleRows) - 1, static_cast<int>(mulNumTotalRows));
            if (mpSetCellCallback != nullptr) {
                u32 c = 0;
                while (c < mulNumVisibleColumns) {
                    short *psString = mpstCells[c].u.string.pStr;
                    u32 r = 0;
                    while (r < NumRows) {
                        u32 Index = r * mulNumVisibleColumns + c;
                        r++;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index + mulNumVisibleColumns], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                    mpSetCellCallback(
                        mpvCallbackData, this,
                        GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(c), static_cast<int>(mulNumTotalColumns)),
                        ulFillCell);
                    c++;
                }
            } else if (mpobRenderer) {
                u32 c = 0;
                while (c < mulNumVisibleColumns) {
                    short *psString = mpstCells[c].u.string.pStr;
                    u32 r = 0;
                    while (r < NumRows) {
                        u32 Index = r * mulNumVisibleColumns + c;
                        r++;
                        FEngMemCpy(&mpstCells[Index], &mpstCells[Index + mulNumVisibleColumns], sizeof(FEListBoxCell));
                    }
                    mpstCells[r * mulNumVisibleColumns + c].u.string.pStr = psString;
                    mpobRenderer->SetCellData(
                        this, GetValidIndex(static_cast<int>(mulCurrentVirtualColumn) + static_cast<int>(c), static_cast<int>(mulNumTotalColumns)),
                        ulFillCell);
                    c++;
                }
            }
        } else if (mpSetCellCallback != nullptr) {
            u32 c = 0;
            while (c < mulNumVisibleColumns) {
                u32 r = mulNumVisibleRows - 1;
                short *psString = mpstCells[r * mulNumVisibleColumns + c].u.string.pStr;
                while (r != 0) {
                    u32 Index = r * mulNumVisibleColumns + c;
                    FEngMemCpy(&mpstCells[Index], &mpstCells[Index - mulNumVisibleColumns], sizeof(FEListBoxCell));
                    r--;
                }
                mpstCells[c].u.string.pStr = psString;
                mpSetCellCallback(
                    mpvCallbackData, this, mulCurrentVirtualColumn,
                    GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                c++;
            }
        } else if (mpobRenderer) {
            u32 c = 0;
            while (c < mulNumVisibleColumns) {
                u32 r = mulNumVisibleRows - 1;
                short *psString = mpstCells[r * mulNumVisibleColumns + c].u.string.pStr;
                while (r != 0) {
                    u32 Index = r * mulNumVisibleColumns + c;
                    FEngMemCpy(&mpstCells[Index], &mpstCells[Index - mulNumVisibleColumns], sizeof(FEListBoxCell));
                    r--;
                }
                mpstCells[c].u.string.pStr = psString;
                mpobRenderer->SetCellData(
                    this, mulCurrentVirtualColumn,
                    GetValidIndex(static_cast<int>(mulCurrentVirtualRow) + static_cast<int>(r), static_cast<int>(mulNumTotalRows)));
                c++;
            }
        }
    }
    return true;
}

u32 FECodeListBox::CalculateCurrentFromTarget(u32 ulTarget, u32 ulNumTotal, u32 ulNumVisible) {
    if (ulTarget >= ulNumTotal) {
        ulTarget = (ulNumTotal != 0) ? ulNumTotal - 1 : 0;
    }
    int lRet = static_cast<int>(ulTarget);
    if (mulFlags & 8) {
        lRet = GetValidIndex(lRet - static_cast<int>(ulNumVisible >> 1), static_cast<int>(ulNumTotal));
    }
    return static_cast<u32>(lRet);
}

void FECodeListBox::SetCellColor(u32 ulStartColumn, u32 ulStartRow, u32 ulColor, u32 ulNumColumns, u32 ulNumRows) {
    u32 endRow = ulNumRows + ulStartRow;
    u32 i = ulStartRow;
    u32 endColumn = ulNumColumns + ulStartColumn;
    while (i < endRow) {
        u32 j = ulStartColumn;
        while (j < endColumn) {
            i32 lCIndex = GetRealColumn(j);
            i32 lRIndex = GetRealRow(i);
            FEListBoxCell *pstCell = &mpstCells[lRIndex * mulNumVisibleColumns + lCIndex];
            pstCell->ulColor = ulColor;
            j++;
        }
        i++;
    }
}

void FECodeListBox::SetCellScale(u32 ulStartColumn, u32 ulStartRow, const FEPoint &stScale, u32 ulNumColumns, u32 ulNumRows) {
    u32 endRow = ulNumRows + ulStartRow;
    u32 i = ulStartRow;
    u32 endColumn = ulNumColumns + ulStartColumn;
    while (i < endRow) {
        u32 j = ulStartColumn;
        while (j < endColumn) {
            i32 lCIndex = GetRealColumn(j);
            i32 lRIndex = GetRealRow(i);
            FEListBoxCell *pstCell = &mpstCells[lRIndex * mulNumVisibleColumns + lCIndex];
            pstCell->stScale = stScale;
            j++;
        }
        i++;
    }
}

void FECodeListBox::SetCellJustification(u32 ulStartColumn, u32 ulStartRow, u32 ulJustification, u32 ulNumColumns, u32 ulNumRows) {
    u32 endRow = ulNumRows + ulStartRow;
    u32 i = ulStartRow;
    u32 endColumn = ulNumColumns + ulStartColumn;
    while (i < endRow) {
        u32 j = ulStartColumn;
        while (j < endColumn) {
            i32 lCIndex = GetRealColumn(j);
            i32 lRIndex = GetRealRow(i);
            FEListBoxCell *pstCell = &mpstCells[lRIndex * mulNumVisibleColumns + lCIndex];
            pstCell->ulJustification = ulJustification;
            j++;
        }
        i++;
    }
}
