#include "Speed/Indep/Src/FEng/FEKeyInterpolators.h"
#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

void FEInterpLinear(FEScript *pScript, u8 TrackNum, i32 tTime, void *pOutData) {
    FEKeyTrack *pTrack = &pScript->pTracks[TrackNum];
    u8 *pOutDataPtr = reinterpret_cast<u8 *>(pOutData);

    u32 OutOffset = pTrack->LongOffset;

    FEInterpLinear(pTrack, tTime, pOutDataPtr + OutOffset * sizeof(void *));
}

void FEInterpLinear(FEKeyTrack *pTrack, i32 tTime, void *pOutDataPtr) {
    u32 KeySize;
    float t = 0.0f;
    FEKeyNode *pKey = nullptr;
    FEKeyNode *pPrevKey = nullptr;
    FEKeyNode *pBaseKey = pTrack->GetBaseKey();
    u8 *pBaseValue = pBaseKey->GetKeyData()->Val;

    if (pTrack->DeltaKeys.GetNumElements() != 0) {
        switch (pTrack->InterpAction & 0x7f) {
            case 0: {
                // Standard (clamp)
                pKey = pTrack->GetDeltaKeyAt(tTime);
                if (!pKey)
                    goto write_base;
                pPrevKey = pKey->GetPrev();
                if (pPrevKey && tTime < pKey->tTime) {
                    float div = static_cast<float>(pKey->tTime - pPrevKey->tTime);
                    if (div > 0.00001f) {
                        t = static_cast<float>(tTime - pPrevKey->tTime) / div;
                    }
                } else {
                    t = 1.0f;
                }
                break;
            }
            case 1: {
                // Loop
                pKey = pTrack->GetDeltaKeyAt(tTime);
                if (!pKey)
                    goto write_base;
                pPrevKey = pKey;
                if (pKey->tTime < tTime) {
                    FEKeyNode *pFirstKey = pTrack->GetFirstDeltaKey();
                    float div = static_cast<float>((pTrack->Length - pKey->tTime) + pFirstKey->tTime);
                    if (div > 0.00001f) {
                        t = static_cast<float>(tTime - pKey->tTime) / div;
                    }
                    pKey = pFirstKey;
                } else if (pKey->tTime != tTime) {
                    pPrevKey = pKey->GetPrev();
                    if (!pPrevKey) {
                        pPrevKey = pTrack->GetKeyAt(pTrack->Length);
                        float div = static_cast<float>((pTrack->Length - pPrevKey->tTime) + pKey->tTime);
                        if (div > 0.00001f) {
                            t = static_cast<float>((tTime + pTrack->Length) - pPrevKey->tTime) / div;
                        }
                    } else {
                        float div = static_cast<float>(pKey->tTime - pPrevKey->tTime);
                        if (div > 0.00001f) {
                            t = static_cast<float>(tTime - pPrevKey->tTime) / div;
                        }
                    }
                }
                break;
            }
            case 2: {
                // Ping-pong
                if (tTime > pTrack->Length) {
                    tTime = pTrack->Length * 2 - tTime;
                }
                pKey = pTrack->GetDeltaKeyAt(tTime);
                break;
            }
        }
    }

write_base:
    if (!pKey) {
        switch (pTrack->ParamType) {
            case 1:
            case 2: {
                i32 *pSrc = pBaseKey->GetKeyData()->Val;
                *reinterpret_cast<i32 *>(pOutDataPtr) = pSrc[0];
                break;
            }
            case 3: {
                i32 *pSrc = pBaseKey->GetKeyData()->Val;
                i32 val0 = pSrc[0];
                i32 val1 = pSrc[1];
                reinterpret_cast<i32 *>(pOutDataPtr)[0] = val0;
                reinterpret_cast<i32 *>(pOutDataPtr)[1] = val1;
                break;
            }
            case 4: {
                i32 *pSrc = pBaseKey->GetKeyData()->Val;
                i32 val0 = pSrc[0];
                i32 val1 = pSrc[1];
                i32 val2 = pSrc[2];
                reinterpret_cast<i32 *>(pOutDataPtr)[0] = val0;
                reinterpret_cast<i32 *>(pOutDataPtr)[1] = val1;
                reinterpret_cast<i32 *>(pOutDataPtr)[2] = val2;
                break;
            }
            case 5:
            case 6: {
                i32 *pSrc = reinterpret_cast<i32 *>(pBaseValue);
                i32 val0 = pSrc[0];
                i32 val1 = pSrc[1];
                i32 val2 = pSrc[2];
                i32 val3 = pSrc[3];
                reinterpret_cast<i32 *>(pOutDataPtr)[0] = val0;
                reinterpret_cast<i32 *>(pOutDataPtr)[1] = val1;
                reinterpret_cast<i32 *>(pOutDataPtr)[2] = val2;
                reinterpret_cast<i32 *>(pOutDataPtr)[3] = val3;
                break;
            }
        }
        return;
    }

    if (t == 0.0f || t == 1.0f) {
        void *pValPtr = pKey->GetKeyData();
        if (t == 0.0f) {
            pValPtr = pPrevKey->GetKeyData();
        }
        switch (pTrack->ParamType) {
            case 1: {
                *reinterpret_cast<i32 *>(pOutDataPtr) = *reinterpret_cast<i32 *>(pBaseValue) + *reinterpret_cast<i32 *>(pValPtr);
                break;
            }
            case 2: {
                *reinterpret_cast<float *>(pOutDataPtr) = *reinterpret_cast<float *>(pBaseValue) + *reinterpret_cast<float *>(pValPtr);
                break;
            }
            case 3: {
                reinterpret_cast<float *>(pOutDataPtr)[0] = reinterpret_cast<float *>(pBaseValue)[0] + reinterpret_cast<float *>(pValPtr)[0];
                reinterpret_cast<float *>(pOutDataPtr)[1] = reinterpret_cast<float *>(pBaseValue)[1] + reinterpret_cast<float *>(pValPtr)[1];
                break;
            }
            case 4: {
                reinterpret_cast<float *>(pOutDataPtr)[0] = reinterpret_cast<float *>(pBaseValue)[0] + reinterpret_cast<float *>(pValPtr)[0];
                reinterpret_cast<float *>(pOutDataPtr)[1] = reinterpret_cast<float *>(pBaseValue)[1] + reinterpret_cast<float *>(pValPtr)[1];
                reinterpret_cast<float *>(pOutDataPtr)[2] = reinterpret_cast<float *>(pBaseValue)[2] + reinterpret_cast<float *>(pValPtr)[2];
                break;
            }
            case 5: {
                FEQuaternion *pBaseQuat = reinterpret_cast<FEQuaternion *>(pBaseValue);
                FEQuaternion *pKeyQuat = reinterpret_cast<FEQuaternion *>(pValPtr);
                FEQuaternion *pDestQuat = reinterpret_cast<FEQuaternion *>(pOutDataPtr);
                *pDestQuat = *pBaseQuat * *pKeyQuat;
                break;
            }
            case 6: {
                reinterpret_cast<i32 *>(pOutDataPtr)[2] = reinterpret_cast<i32 *>(pBaseValue)[2] + reinterpret_cast<i32 *>(pValPtr)[2];
                reinterpret_cast<i32 *>(pOutDataPtr)[1] = reinterpret_cast<i32 *>(pBaseValue)[1] + reinterpret_cast<i32 *>(pValPtr)[1];
                reinterpret_cast<i32 *>(pOutDataPtr)[0] = reinterpret_cast<i32 *>(pBaseValue)[0] + reinterpret_cast<i32 *>(pValPtr)[0];
                reinterpret_cast<i32 *>(pOutDataPtr)[3] = reinterpret_cast<i32 *>(pBaseValue)[3] + reinterpret_cast<i32 *>(pValPtr)[3];
                break;
            }
        }
    } else {
        switch (pTrack->ParamType) {
            case 1:
                FELerpInteger(*reinterpret_cast<i32 *>(pPrevKey->GetKeyData()), *reinterpret_cast<i32 *>(pKey->GetKeyData()), t,
                              reinterpret_cast<i32 *>(pBaseValue), reinterpret_cast<i32 *>(pOutDataPtr));
                break;
            case 2:
                FELerpFloat(*reinterpret_cast<float *>(pPrevKey->GetKeyData()), *reinterpret_cast<float *>(pKey->GetKeyData()), t,
                            reinterpret_cast<float *>(pBaseValue), reinterpret_cast<float *>(pOutDataPtr));
                break;
            case 3:
                FELerpVector2(*reinterpret_cast<FEVector2 *>(pPrevKey->GetKeyData()), *reinterpret_cast<FEVector2 *>(pKey->GetKeyData()), t,
                              reinterpret_cast<FEVector2 *>(pBaseValue), reinterpret_cast<FEVector2 *>(pOutDataPtr));
                break;
            case 4:
                FELerpVector3(*reinterpret_cast<FEVector3 *>(pPrevKey->GetKeyData()), *reinterpret_cast<FEVector3 *>(pKey->GetKeyData()), t,
                              reinterpret_cast<FEVector3 *>(pBaseValue), reinterpret_cast<FEVector3 *>(pOutDataPtr));
                break;
            case 5:
                FELerpQuaternion(*reinterpret_cast<FEQuaternion *>(pPrevKey->GetKeyData()), *reinterpret_cast<FEQuaternion *>(pKey->GetKeyData()), t,
                                 reinterpret_cast<FEQuaternion *>(pBaseValue), reinterpret_cast<FEQuaternion *>(pOutDataPtr));
                break;
            case 6:
                FELerpColor(*reinterpret_cast<FEColor *>(pPrevKey->GetKeyData()), *reinterpret_cast<FEColor *>(pKey->GetKeyData()), t,
                            reinterpret_cast<FEColor *>(pBaseValue), reinterpret_cast<FEColor *>(pOutDataPtr));
                break;
        }
    }
}

void FELerpInteger(i32 n1, i32 n2, float t, i32 *pOffset, i32 *pDest) {
    *pDest = *pOffset + n1 + static_cast<int>((n2 - n1) * t + 0.5f);
}

void FELerpFloat(float n1, float n2, float t, float *pOffset, float *pDest) {
    *pDest = *pOffset + n1 + (n2 - n1) * t;
}

void FELerpVector2(FEVector2 &v1, FEVector2 &v2, float t, FEVector2 *pOffset, FEVector2 *pDest) {
    pDest->x = pOffset->x + v1.x + (v2.x - v1.x) * t;
    pDest->y = pOffset->y + v1.y + (v2.y - v1.y) * t;
}

void FELerpVector3(FEVector3 &v1, FEVector3 &v2, float t, FEVector3 *pOffset, FEVector3 *pDest) {
    pDest->x = pOffset->x + v1.x + (v2.x - v1.x) * t;
    pDest->y = pOffset->y + v1.y + (v2.y - v1.y) * t;
    pDest->z = pOffset->z + v1.z + (v2.z - v1.z) * t;
}

void FELerpQuaternion(FEQuaternion &q1, FEQuaternion &q2, float t, FEQuaternion *pOffset, FEQuaternion *pDest) {
    FEQuaternion q = q2;
    float Dot = QuaternionDot(q1, q);

    if (Dot < 0.0f) {
        q.x = -q.x;
        q.y = -q.y;
        q.z = -q.z;
        q.w = -q.w;
        Dot = -Dot;
    }

    if (Dot < 0.999f) {
        float Angle = FEngACos(Dot);
        float SinA = FEngSin(Angle);
        float SinAT = FEngSin(Angle * t);
        float SinAInvT = FEngSin(Angle * (1.0f - t));

        q = ((q1 * SinAInvT) + (q * SinAT)) * (1.0f / SinA);
    } else {
        FEQuaternion r = (q1 + ((q * q1) * t));
        NormalizeQuaternion(r);
        q = r;
    }

    FEQuaternion qRet = *pOffset * q;
    *pDest = qRet;
}

void FELerpColor(FEColor &c1, FEColor &c2, float t, FEColor *pOffset, FEColor *pDest) {
    pDest->b = pOffset->b + c1.b + static_cast<int>((c2.b - c1.b) * t + 0.5f);
    pDest->g = pOffset->g + c1.g + static_cast<int>((c2.g - c1.g) * t + 0.5f);
    pDest->r = pOffset->r + c1.r + static_cast<int>((c2.r - c1.r) * t + 0.5f);
    pDest->a = pOffset->a + c1.a + static_cast<int>((c2.a - c1.a) * t + 0.5f);
}
