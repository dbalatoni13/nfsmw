#include "Speed/Indep/Src/FEng/FEButtonMap.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"

// size: 0x40, address: 0x80473CD0, Decl: 14
static FEVector2 DirectionVectors[8] = {
    FEVector2(0.0f, -1.0f), FEVector2(0.707110f, -0.707110f), FEVector2(1.0f, 0.0f),  FEVector2(0.707110f, 0.707110f),
    FEVector2(0.0f, 1.0f),  FEVector2(-0.707110f, 0.707110f), FEVector2(-1.0f, 0.0f), FEVector2(-0.707110f, -0.707110f),
};

// size: 0x20, address: 0x8041D040, Decl: 26
u32 FEDirection_Message[8] = {
    __PAD_UP__, __PAD_UPRIGHT__, __PAD_RIGHT__, __PAD_DOWNRIGHT__, __PAD_DOWN__, __PAD_DOWNLEFT__, __PAD_LEFT__, __PAD_UPLEFT__,
};

// Decl: 35
void FEButtonMap::SetCount(u32 NewCount) {
    if (pList != nullptr) {
        delete[] pList;
    }
    pList = nullptr;
    if (NewCount != 0) {
        pList = FNEW FEObject *[NewCount];
    }
    Count = NewCount;
}

// size: 0x28, address: 0x80473D10, Decl: 45
static FEVector2 PassOffsets[5] = {
    FEVector2(0.0f, 0.0f), FEVector2(-640.0f, 0.0f), FEVector2(640.0f, 0.0f), FEVector2(0.0f, -480.0f), FEVector2(0.0f, 480.0f),
};

// size: 0x14, address: 0x8041D060, Decl: 53
static u32 PassWrapMode[5] = {3, 1, 1, 2, 2};

// Decl: 75
FEObject *FEButtonMap::GetButtonFrom(FEObject *pButton, i32 Direction, FEGameInterface *pInterface, FEButtonWrapMode WrapMode) {
    float BestScore;
    u32 BestIndex = 0;
    FEVector2 VectOrig;
    FEVector2 VectFrom;
    FEVector2 VectTo;

    BestScore = 1e30f;

    ComputeButtonLocation(pButton, pInterface, VectOrig);

    u32 Pass = 0;
    while (Pass <= 4) {
        if (Pass == 0 || (PassWrapMode[Pass] & WrapMode) != 0) {
            VectFrom = VectOrig + PassOffsets[Pass];

            for (u32 i = 0; i < Count; i++) {
                if ((pList[i]->Flags & FF_IgnoreButton) == 0 && pButton != pList[i]) {
                    float Angle, Distance, Score;
                    FEVector2 Delta;
                    ComputeButtonLocation(pList[i], pInterface, VectTo);
                    Delta = VectTo - VectFrom;
                    Distance = Delta.Length();
                    if (Distance >= 0.0001f) {
                        Delta *= 1.0f / Distance;
                        Angle = Delta.Dot(DirectionVectors[Direction]);
                        if (Angle >= 0.0f) {
                            Angle = Angle * Angle;
                        }
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
            }
        }
        Pass++;
    }

    if (BestScore < 1500.0f) {
        return pList[BestIndex];
    }
    return nullptr;
}

// Decl: 141
void FEButtonMap::ComputeButtonLocation(FEObject *pButton, FEGameInterface *pInterface, FEVector2 &Dest) {
    if ((pInterface == nullptr) || pButton->RenderContext == 0) {
        Dest = reinterpret_cast<FEVector2 &>(pButton->GetObjData()->Pos);
    } else {
        FEMatrix4 Matrix;
        if (!pInterface->GetContextTransform(pButton->RenderContext, Matrix)) {
            Dest = reinterpret_cast<FEVector2 &>(pButton->GetObjData()->Pos);
        } else {
            FEVector3 Temp;
            FEMultMatrix(&Temp, &Matrix, &pButton->GetObjData()->Pos);
            Dest = reinterpret_cast<FEVector2 &>(Temp);
        }
    }
}

// total size: 0x8
// Decl: 161
class FEButtonCounter : public FEObjectCallback {
  public:
    u32 Count; // offset 0x4, size 0x4, Decl: 163

    bool Callback(FEObject *pObj) override { // Decl: 165
        if (pObj->Flags & FF_IsButton) {
            this->Count++;
        }
        return true;
    }
};

// total size: 0xC
// Decl: 175
class FEButtonEnumerator : public FEObjectCallback {
  public:
    FEButtonMap *pButtonMap; // offset 0x4, size 0x4, Decl: 177
    u32 Count;               // offset 0x8, size 0x4, Decl: 178

    bool Callback(FEObject *pObj) override { // Decl: 180
        if (pObj->Flags & FF_IsButton) {
            pButtonMap->SetButton(this->Count++, pObj);
        }
        return true;
    }
};
