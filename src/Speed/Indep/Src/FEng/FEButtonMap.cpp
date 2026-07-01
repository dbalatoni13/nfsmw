#include "Speed/Indep/Src/FEng/FEButtonMap.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

// size: 0x40, address: 0x80473CD0, Decl: speed/indep/src/feng/FEButtonMap.cpp:14
static FEVector2 DirectionVectors[8] = {
    FEVector2(0.0f, -1.0f), FEVector2(0.707110f, -0.707110f), FEVector2(1.0f, 0.0f),  FEVector2(0.707110f, 0.707110f),
    FEVector2(0.0f, 1.0f),  FEVector2(-0.707110f, 0.707110f), FEVector2(-1.0f, 0.0f), FEVector2(-0.707110f, -0.707110f),
};

// size: 0x20, address: 0x8041D040, Decl: speed/indep/src/feng/FEButtonMap.cpp:26
u32 FEDirection_Message[8] = {
    0x72619778u, 0x6FD81B16u, 0xB5971BF1u, 0xAB1A49C9u, 0x911C0A4Bu, 0x79891376u, 0x9120409Eu, 0x6FFB6F23u,
};

// Decl: speed/indep/src/feng/FEButtonMap.cpp:35
void FEButtonMap::SetCount(u32 NewCount) {
    if (pList) {
        delete[] pList;
    }
    pList = nullptr;
    if (NewCount != 0) {
        pList = FNEW FEObject *[NewCount];
    }
    Count = NewCount;
}

// size: 0x28, address: 0x80473D10, Decl: speed/indep/src/feng/FEButtonMap.cpp:45
static FEVector2 PassOffsets[5] = {
    FEVector2(0.0f, 0.0f), FEVector2(-640.0f, 0.0f), FEVector2(640.0f, 0.0f), FEVector2(0.0f, -480.0f), FEVector2(0.0f, 480.0f),
};

// size: 0x14, address: 0x8041D060, Decl: speed/indep/src/feng/FEButtonMap.cpp:53
static u32 PassWrapMode[5] = {3, 1, 1, 2, 2};

// Decl: speed/indep/src/feng/FEButtonMap.cpp:75
FEObject *FEButtonMap::GetButtonFrom(FEObject *pButton, i32 Direction, FEGameInterface *pInterface, FEButtonWrapMode WrapMode) {
    float BestScore; // = 1e30f;
    u32 BestIndex = 0;
    FEVector2 VectOrig;
    FEVector2 VectFrom;
    FEVector2 VectTo;

    BestScore = 1e30f;

    ComputeButtonLocation(pButton, pInterface, VectOrig);

    for (u32 Pass = 0; Pass <= 4; Pass++) {
        if (Pass == 0 || (PassWrapMode[Pass] & WrapMode) != 0) {
            VectFrom = VectOrig + PassOffsets[Pass];

            for (u32 i = 0; i < Count; i++) {
                if ((pList[i]->Flags & FF_IgnoreButton) == 0 && pButton != pList[i]) {
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
            }
        }
    }

    if (BestScore < 1500.0f) {
        return pList[BestIndex];
    }
    return nullptr;
}

// Decl: speed/indep/src/feng/FEButtonMap.cpp:141
void FEButtonMap::ComputeButtonLocation(FEObject *pButton, FEGameInterface *pInterface, FEVector2 &Dest) {
    if (!pInterface || pButton->RenderContext == 0) {
        Dest = static_cast<FEVector2>(pButton->GetObjData()->Pos);
    } else {
        FEMatrix4 Matrix;
        if (!pInterface->GetContextTransform(pButton->RenderContext, Matrix)) {
            Dest = static_cast<FEVector2>(pButton->GetObjData()->Pos);
        } else {
            FEVector3 Temp;
            FEMultMatrix(&Temp, &Matrix, &pButton->GetObjData()->Pos);
            Dest = static_cast<FEVector2>(Temp);
        }
    }
}

// File: speed/indep/src/feng/FEButtonMap.cpp
// total size: 0x8
// Decl: speed/indep/src/feng/FEButtonMap.cpp:161
class FEButtonCounter : public FEObjectCallback {
  public:
    u32 Count; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEButtonMap.cpp:163

    bool Callback(FEObject *pObj) override {} // Decl: speed/indep/src/feng/FEButtonMap.cpp:165
};

// total size: 0xC
// Decl: speed/indep/src/feng/FEButtonMap.cpp:175
class FEButtonEnumerator : public FEObjectCallback {
  public:
    FEButtonMap *pButtonMap; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEButtonMap.cpp:177
    u32 Count;               // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEButtonMap.cpp:178

    bool Callback(FEObject *pObj) override {} // Decl: speed/indep/src/feng/FEButtonMap.cpp:180
};
