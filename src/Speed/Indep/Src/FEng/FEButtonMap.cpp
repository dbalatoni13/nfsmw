#include <new>

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"

static unsigned long PassWrapMode[5] = { 3, 1, 1, 2, 2 };
static FEVector2 DirectionVectors[8] = {
    FEVector2(0.0f, -1.0f),
    FEVector2(0.707110f, -0.707110f),
    FEVector2(1.0f, 0.0f),
    FEVector2(0.707110f, 0.707110f),
    FEVector2(0.0f, 1.0f),
    FEVector2(-0.707110f, 0.707110f),
    FEVector2(-1.0f, 0.0f),
    FEVector2(-0.707110f, -0.707110f),
};
static FEVector2 PassOffsets[5] = {
    FEVector2(0.0f, 0.0f),
    FEVector2(-640.0f, 0.0f),
    FEVector2(640.0f, 0.0f),
    FEVector2(0.0f, -480.0f),
    FEVector2(0.0f, 480.0f),
};

void FEButtonMap::SetCount(unsigned long NewCount) {
    if (pList) {
        delete[] pList;
    }
    pList = nullptr;
    if (NewCount != 0) {
        pList = reinterpret_cast<FEObject**>(FENG_NEW char[NewCount * sizeof(FEObject*)]);
    }
    Count = NewCount;
}

void FEButtonMap::ComputeButtonLocation(FEObject* pButton, FEGameInterface* pInterface, FEVector2& Dest) {
    if (!pInterface || pButton->RenderContext == 0) {
        FEObjData* pData = pButton->GetObjData();
        Dest.x = pData->Pos.x;
        Dest.y = pData->Pos.y;
    } else {
        FEMatrix4 Matrix;
        if (!pInterface->GetContextTransform(pButton->RenderContext, Matrix)) {
            FEObjData* pData = pButton->GetObjData();
            Dest.x = pData->Pos.x;
            Dest.y = pData->Pos.y;
        } else {
            FEVector3 Temp;
            FEMultMatrix(&Temp, &Matrix, &pButton->GetObjData()->Pos);
            Dest.x = Temp.x;
            Dest.y = Temp.y;
        }
    }
}

FEObject* FEButtonMap::GetButtonFrom(FEObject* pButton, long Direction, FEGameInterface* pInterface, FEButtonWrapMode WrapMode) {
    FEVector2 VectOrig;
    FEVector2 VectFrom;
    FEVector2 VectTo;
    float BestScore = 1e30f;
    unsigned long BestIndex = 0;

    ComputeButtonLocation(pButton, pInterface, VectOrig);

    unsigned long Pass = 0;
    do {
        if (Pass == 0 || (PassWrapMode[Pass] & WrapMode) != 0) {
            VectFrom = VectOrig + PassOffsets[Pass];
            unsigned long i = 0;
            if (i < Count) {
                do {
                    FEObject* pObj = pList[i];
                    if ((pObj->Flags & 0x4000000) == 0 && pButton != pObj) {
                        FEVector2 Delta;
                        ComputeButtonLocation(pList[i], pInterface, VectTo);
                        Delta = VectTo - VectFrom;
                        float Distance = Delta.Length();
                        if (Distance >= 0.0001f) {
                            Delta *= 1.0f / Distance;
                            float Angle = Delta.Dot(DirectionVectors[Direction]);
                            if (Angle >= 0.0f) {
                                Angle = Angle * Angle;
                            }
                            float Score;
                            if (Angle >= 0.25f) {
                                Score = (1.0f - Angle) * 200.0f + Distance;
                            } else {
                                Score = 1500.0f;
                            }
                            if (Score < BestScore) {
                                BestScore = Score;
                                BestIndex = i;
                            }
                        }
                    }
                    i++;
                } while (i < Count);
            }
        }
        Pass++;
    } while (Pass <= 4);

    if (BestScore < 1500.0f) {
        return pList[BestIndex];
    }
    return nullptr;
}
