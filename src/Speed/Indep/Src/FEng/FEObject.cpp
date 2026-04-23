#include <new>

#include "FEObject.h"
#include "FETypes.h"
#include "FEScript.h"
#include "FESlotPool.h"
#include "FEngStandard.h"
#include "FEMessageResponse.h"
#include "fengine.h"

extern FEMultiPool ObjDataPool;

inline bool CloseEnoughPosition(const FEVector3& vector1, const FEVector3& vector2) {
    if (!Close(vector1.x, vector2.x, 0.001f)) return false;
    if (!Close(vector1.y, vector2.y, 0.001f)) return false;
    return Close(vector1.z, vector2.z, 0.001f);
}

inline bool CloseEnoughColor(const FEColor& color1, const FEColor& color2) {
    if (!Close(static_cast<long>(color1.r), static_cast<long>(color2.r), 1L)) return false;
    if (!Close(static_cast<long>(color1.g), static_cast<long>(color2.g), 1L)) return false;
    if (!Close(static_cast<long>(color1.b), static_cast<long>(color2.b), 1L)) return false;
    return Close(static_cast<long>(color1.a), static_cast<long>(color2.a), 1L);
}

bool Close(float a, float b, float epsilon);
bool Close(long a, long b, long epsilon);

FEObjectDestructorCallback* FEObject::pDestructorCallback;

FEObject::FEObject()
    : NameHash(0) //
    , pName(nullptr) //
    , Flags(0) //
    , RenderContext(0) //
    , Handle(0) //
    , UserParam(0) //
    , pData(nullptr) //
    , DataSize(0) //
    , Cached(nullptr) //
{
    GUID = FEngine::GetNextGUID();
}

FEObject::FEObject(const FEObject& Object, bool bReference)
    : NameHash(0) //
    , pName(nullptr) //
    , Flags(0) //
    , Handle(0) //
    , UserParam(0) //
    , pData(nullptr) //
{
    GUID = FEngine::SysGUID++;
    SetDataSize(Object.DataSize);
    FEngMemSet(pData, 0, DataSize);
    Type = Object.Type;
    Flags = Object.Flags;
    RenderContext = Object.RenderContext;
    ResourceIndex = Object.ResourceIndex;
    Handle = Object.Handle;
    SetName(Object.pName);

    FEMessageResponse* pSrcResp = static_cast<FEMessageResponse*>(Object.Responses.GetHead());
    while (pSrcResp) {
        FEMessageResponse* pNewResp = new FEMessageResponse();
        unsigned long count = pSrcResp->GetCount();
        pNewResp->SetCount(count);
        pNewResp->MsgID = pSrcResp->MsgID;
        for (unsigned long i = 0; i < count; i++) {
            pNewResp->pResponseList[i] = pSrcResp->pResponseList[i];
        }
        Responses.AddNode(Responses.GetTail(), pNewResp);
        pSrcResp = static_cast<FEMessageResponse*>(pSrcResp->GetNext());
    }

    for (FEScript* pSrcScript = static_cast<FEScript*>(Object.Scripts.GetHead()); pSrcScript; pSrcScript = static_cast<FEScript*>(pSrcScript->GetNext())) {
        FEScript* pNewScript = new FEScript(*pSrcScript, bReference);
        Scripts.AddNode(Scripts.GetTail(), pNewScript);
    }

    FEScript* pFoundScript = FindScript(Object.pCurrentScript->ID);
    SetCurrentScript(pFoundScript);

    FEScript* pMyScript = static_cast<FEScript*>(Scripts.GetHead());
    for (FEScript* pSrcScript = static_cast<FEScript*>(Object.Scripts.GetHead()); pSrcScript; pSrcScript = static_cast<FEScript*>(pSrcScript->GetNext())) {
        if (pSrcScript->pChainTo) {
            pMyScript->pChainTo = FindScript(pSrcScript->pChainTo->ID);
        }
        pMyScript = static_cast<FEScript*>(pMyScript->GetNext());
    }
}

FEObject::~FEObject() {
    if (pDestructorCallback) {
        pDestructorCallback->OnDestroy(this);
    }
    ObjDataPool.Free(pData);
    if (pName) {
        delete[] pName;
    }
}

void FEObject::SetDataSize(unsigned long Size) {
    ObjDataPool.Free(pData);
    pData = nullptr;
    pData = ObjDataPool.Alloc(Size);
    DataSize = Size;
}

void FEObject::SetName(const char* pNewName) {
    if (pName) {
        delete[] pName;
        pName = nullptr;
    }
    NameHash = -1;
    if (pNewName) {
        int Len = FEngStrLen(pNewName);

        pName = FENG_NEW char[Len + 1];
        FEngStrCpy(pName, pNewName);
        NameHash = FEHashUpper(pName);
    }
}

FEScript* FEObject::FindScript(unsigned long ID) const {
    FEScript* pScript = static_cast<FEScript*>(Scripts.GetHead());
    if (pScript) {
        while (pScript->ID != ID) {
            pScript = pScript->GetNext();
            if (!pScript) break;
        }
    }
    return pScript;
}

void FEObject::SetCurrentScript(FEScript* pScript) {
    pCurrentScript = pScript;
    if (pScript) {
        SetupMoveToTracks();
    }
}

void FEObject::SetupMoveToTracks() {
    unsigned long NumTracks = pCurrentScript->TrackCount;
    FEKeyTrack* pTrack = pCurrentScript->pTracks;

    for (unsigned long i = 0; i < NumTracks; i++) {
        pTrack[i].InterpAction &= 0x7F;

        if (static_cast<unsigned long>(pTrack[i].InterpType - 3) < 2) {
            float* pfData = reinterpret_cast<float*>(pData + pTrack[i].LongOffset * 4);
            FEKeyNode* pBase = pTrack[i].GetBaseKey();
            FEKeyNode* pKey = pTrack[i].GetFirstDeltaKey();

            if (pKey) {
                switch (pTrack[i].ParamType) {
                case 1: {
                    *static_cast<long*>(pKey->Val) = *reinterpret_cast<long*>(pfData) - *reinterpret_cast<long*>(static_cast<unsigned char*>(pBase->Val));
                    break;
                }
                case 2: {
                    *reinterpret_cast<float*>(static_cast<unsigned char*>(pKey->Val)) = *reinterpret_cast<float*>(pfData) - *reinterpret_cast<float*>(static_cast<unsigned char*>(pBase->Val));
                    break;
                }
                case 3: {
                    FEVector2 diff = *reinterpret_cast<FEVector2*>(pfData) - *reinterpret_cast<FEVector2*>(static_cast<unsigned char*>(pBase->Val));
                    pKey->Val = diff;
                    break;
                }
                case 4: {
                    FEVector3 diff3 = *reinterpret_cast<FEVector3*>(pfData) - *reinterpret_cast<FEVector3*>(static_cast<unsigned char*>(pBase->Val));
                    pKey->Val = diff3;
                    break;
                }
                case 5: {
                    FEQuaternion BaseQuat = *static_cast<FEQuaternion*>(pBase->Val);
                    BaseQuat.Conjugate();
                    FEQuaternion qRet;
                    qRet.x = pfData[1] * BaseQuat.z - pfData[2] * BaseQuat.y;
                    qRet.y = pfData[2] * BaseQuat.x - pfData[0] * BaseQuat.z;
                    qRet.z = pfData[0] * BaseQuat.y - pfData[1] * BaseQuat.x;
                    qRet.x += pfData[0] * BaseQuat.w + BaseQuat.x * pfData[3];
                    qRet.y += pfData[1] * BaseQuat.w + BaseQuat.y * pfData[3];
                    qRet.z += pfData[2] * BaseQuat.w + BaseQuat.z * pfData[3];
                    qRet.w = pfData[3] * BaseQuat.w - (pfData[0] * BaseQuat.x + pfData[1] * BaseQuat.y + pfData[2] * BaseQuat.z);
                    pKey->Val = qRet;
                    break;
                }
                case 6: {
                    FEColor colorDiff = *reinterpret_cast<FEColor*>(pfData) - *reinterpret_cast<FEColor*>(static_cast<unsigned char*>(pBase->Val));
                    pKey->Val = colorDiff;
                    break;
                }
                }
            }
        }
    }
}

FEMessageResponse* FEObject::FindResponse(unsigned long MsgID) const {
    FEMessageResponse* pNode = GetFirstResponse();
    while (pNode) {
        if (pNode->GetMsgID() == MsgID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}

void FEObject::SetScript(unsigned long ID, bool bForce) {
    FEScript* pScript = FindScript(ID);
    SetScript(pScript, bForce);
}

void FEObject::SetScript(FEScript* pScript, bool bForce) {
    if (!bForce && pScript == pCurrentScript) {
        return;
    }
    SetCurrentScript(pScript);
    pCurrentScript->CurTime = 0;
}

unsigned long FEObject::GetDataOffset(FEKeyTrack_Indices track) {
    switch (track) {
    case FETrack_Color:
        return 0;
    case FETrack_Pivot:
        return 0x10;
    case FETrack_Position:
        return 0x1C;
    case FETrack_Rotation:
        return 0x28;
    case FETrack_Size:
        return 0x38;
    case FETrack_UpperLeft:
        return 0x44;
    case FETrack_LowerRight:
        return 0x4C;
    case FETrack_Color1:
        return 0x54;
    case FETrack_Color2:
        return 0x64;
    case FETrack_Color3:
        return 0x74;
    case FETrack_Color4:
        return 0x84;
    default:
        return 0;
    }
}

FEObject* FEObject::Clone(bool bReference) {
    return FENG_NEW FEObject(*this, bReference);
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEVector3& value, bool bRelative) {
    FEScript* pScript = static_cast<FEScript*>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack* pTrack = pScript->FindTrack(track);
        if (pTrack) {
            FEKeyNode* pKey = pTrack->GetBaseKey();
            if (bRelative) {
                reinterpret_cast<FEVector3*>(static_cast<unsigned char*>(*pKey->GetKeyData()))->operator+=(value);
            } else {
                *pKey->GetKeyData() = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        unsigned long offset = GetDataOffset(track);
        reinterpret_cast<FEVector3*>(pData + offset)->operator+=(value);
    } else {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEVector3*>(pData + offset) = value;
    }
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEVector2& value, bool bRelative) {
    FEScript* pScript = static_cast<FEScript*>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack* pTrack = pScript->FindTrack(track);
        if (pTrack) {
            FEKeyNode* pKey = pTrack->GetBaseKey();
            if (bRelative) {
                reinterpret_cast<FEVector2*>(static_cast<unsigned char*>(*pKey->GetKeyData()))->operator+=(value);
            } else {
                *pKey->GetKeyData() = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        unsigned long offset = GetDataOffset(track);
        reinterpret_cast<FEVector2*>(pData + offset)->operator+=(value);
    } else {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEVector2*>(pData + offset) = value;
    }
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEColor& value, bool bRelative) {
    FEScript* pScript = static_cast<FEScript*>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack* pTrack = pScript->FindTrack(track);
        if (pTrack) {
            FEKeyNode* pKey = pTrack->GetBaseKey();
            if (bRelative) {
                *reinterpret_cast<FEColor*>(static_cast<unsigned char*>(*pKey->GetKeyData())) += value;
            } else {
                *pKey->GetKeyData() = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEColor*>(pData + offset) += value;
    } else {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEColor*>(pData + offset) = value;
    }
}

void FEObject::SetPosition(const FEVector3& position, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    if (bRelative) {
        FEVector3 zero(0.0f, 0.0f, 0.0f);
        if (!CloseEnoughPosition(position, zero)) {
            Flags |= 0x400000;
        }
    } else {
        FEObjData* pData = GetObjData();
        if (!CloseEnoughPosition(position, pData->Pos)) {
            Flags |= 0x400000;
        }
    }
    SetTrackValue(FETrack_Position, position, bRelative);
}

void FEObject::SetRotation(const FEQuaternion& rotation, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    Flags |= 0x400000;
    FEScript* pScript = static_cast<FEScript*>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack* pTrack = pScript->FindTrack(FETrack_Rotation);
        if (pTrack) {
            FEKeyNode* pKey = pTrack->GetBaseKey();
            if (bRelative) {
                reinterpret_cast<FEQuaternion*>(static_cast<unsigned char*>(*pKey->GetKeyData()))->operator*=(rotation);
            } else {
                *pKey->GetKeyData() = rotation;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        GetObjData()->Rot *= rotation;
    } else {
        GetObjData()->Rot = rotation;
    }
}

void FEObject::SetColor(const FEColor& color, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    bool bClose;
    if (bRelative) {
        FEColor zero(0);
        bClose = CloseEnoughColor(color, zero);
    } else {
        FEObjData* pData = GetObjData();
        bClose = CloseEnoughColor(color, pData->Col);
    }
    if (!bClose) {
        Flags |= 0x400000;
    }
    SetTrackValue(FETrack_Color, color, bRelative);
}
