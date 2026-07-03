#include "FEObject.h"
#include "FETypes.h"
#include "FEScript.h"
#include "FESlotPool.h"
#include "FEngStandard.h"
#include "FEMessageResponse.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEngine.h"

FEMultiPool ObjDataPool;                                   // size: 0x10, address: 0x80473D58, Decl: speed/indep/src/feng/FEObject.cpp:19
FEObjectDestructorCallback *FEObject::pDestructorCallback; // size: 0x4, address: 0x8041D154, Decl: speed/indep/src/feng/FEObject.cpp:20

static const float PositionEpsilon = 0.000001f; // size: 0x4, Decl: speed/indep/src/feng/FEObject.cpp:23
static const float SizeEpsilon = 0.000001f;     // size: 0x4, Decl: speed/indep/src/feng/FEObject.cpp:24
static const i32 ColorEpsilon = 1;              // size: 0x4, Decl: speed/indep/src/feng/FEObject.cpp:25

bool Close(float x, float y, float epsilon) {
    bool result = false;
    if (x + epsilon >= y) {
        result = x - epsilon <= y;
    }
    return result;
}

bool Close(i32 x, i32 y, i32 epsilon) {
    return x + epsilon >= y && x - epsilon <= y;
}

// Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEObject.cpp:37
inline bool CloseEnoughPosition(const FEVector3 &vector1, const FEVector3 &vector2) {
    if (!Close(vector1.x, vector2.x, PositionEpsilon))
        return false;
    if (!Close(vector1.y, vector2.y, PositionEpsilon))
        return false;
    return Close(vector1.z, vector2.z, PositionEpsilon);
}

// STRIPPED
//  Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEObject.cpp:47
inline bool CloseEnoughSize(const FEVector3 &size1, const FEVector3 &size2) {
    if (!Close(size1.x, size2.x, SizeEpsilon))
        return false;
    if (!Close(size1.y, size2.y, SizeEpsilon))
        return false;
    return Close(size1.z, size2.z, SizeEpsilon);
}

// Decl: d:/eax-build1-s04-ps3/carbon/branches/leadplat/speed/indep/src/feng/FEObject.cpp:57
inline bool CloseEnoughColor(const FEColor &color1, const FEColor &color2) {
    if (!Close(static_cast<long>(color1.r), static_cast<long>(color2.r), ColorEpsilon))
        return false;
    if (!Close(static_cast<long>(color1.g), static_cast<long>(color2.g), ColorEpsilon))
        return false;
    if (!Close(static_cast<long>(color1.b), static_cast<long>(color2.b), ColorEpsilon))
        return false;
    return Close(static_cast<long>(color1.a), static_cast<long>(color2.a), ColorEpsilon);
}

FEObject::FEObject()
    : NameHash(0),      //
      pName(nullptr),   //
      Flags(0),         //
      RenderContext(0), //
      Handle(0),        //
      UserParam(0),     //
      pData(nullptr),   //
      DataSize(0),      //
      Cached(nullptr)   //
{
    GUID = FEngine::GetNextGUID();
}

FEObject::FEObject(const FEObject &Object, bool bReference)
    : NameHash(0),    //
      pName(nullptr), //
      Flags(0),       //
      Handle(0),      //
      UserParam(0),   //
      pData(nullptr)  //
{
    GUID = FEngine::GetNextGUID();
    SetDataSize(Object.DataSize);
    FEngMemSet(pData, 0, DataSize);
    Type = Object.Type;
    Flags = Object.Flags;
    RenderContext = Object.RenderContext;
    ResourceIndex = Object.ResourceIndex;
    Handle = Object.Handle;
    SetName(Object.pName);

    FEMessageResponse *pSrcResp = static_cast<FEMessageResponse *>(Object.Responses.GetHead());
    while (pSrcResp) {
        FEMessageResponse *pNewResp = new FEMessageResponse();
        unsigned long count = pSrcResp->GetCount();
        pNewResp->SetCount(count);
        pNewResp->MsgID = pSrcResp->MsgID;
        for (unsigned long i = 0; i < count; i++) {
            pNewResp->pResponseList[i] = pSrcResp->pResponseList[i];
        }
        Responses.AddNode(Responses.GetTail(), pNewResp);
        pSrcResp = static_cast<FEMessageResponse *>(pSrcResp->GetNext());
    }

    for (FEScript *pSrcScript = static_cast<FEScript *>(Object.Scripts.GetHead()); pSrcScript;
         pSrcScript = static_cast<FEScript *>(pSrcScript->GetNext())) {
        FEScript *pNewScript = new FEScript(*pSrcScript, bReference);
        Scripts.AddNode(Scripts.GetTail(), pNewScript);
    }

    FEScript *pFoundScript = FindScript(Object.pCurrentScript->ID);
    SetCurrentScript(pFoundScript);

    FEScript *pMyScript = static_cast<FEScript *>(Scripts.GetHead());
    for (FEScript *pSrcScript = static_cast<FEScript *>(Object.Scripts.GetHead()); pSrcScript;
         pSrcScript = static_cast<FEScript *>(pSrcScript->GetNext())) {
        if (pSrcScript->pChainTo) {
            pMyScript->pChainTo = FindScript(pSrcScript->pChainTo->ID);
        }
        pMyScript = static_cast<FEScript *>(pMyScript->GetNext());
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

void FEObject::SetDataSize(u32 Size) {
    ObjDataPool.Free(pData);
    pData = nullptr;
    pData = ObjDataPool.Alloc(Size);
    DataSize = Size;
}

void FEObject::SetName(const char *pNewName) {
    if (pName) {
        delete[] pName;
        pName = nullptr;
    }
    NameHash = -1;
    if (pNewName) {
        int Len = FEngStrLen(pNewName);

        pName = FNEW char[Len + 1];
        FEngStrCpy(pName, pNewName);
        NameHash = FEHashUpper(pName);
    }
}

FEScript *FEObject::FindScript(u32 ID) const {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    if (pScript) {
        while (pScript->ID != ID) {
            pScript = pScript->GetNext();
            if (!pScript)
                break;
        }
    }
    return pScript;
}

void FEObject::SetupMoveToTracks() {
    u32 NumTracks = pCurrentScript->TrackCount;
    FEKeyTrack *pTrack = pCurrentScript->pTracks;

    for (u32 i = 0; i < NumTracks; i++) {
        pTrack[i].InterpAction &= 0x7F;

        if (static_cast<u32>(pTrack[i].InterpType - 3) < 2) {
            float *pfData = reinterpret_cast<float *>(pData + pTrack[i].LongOffset * 4);
            FEKeyNode *pBase = pTrack[i].GetBaseKey();
            FEKeyNode *pKey = pTrack[i].GetFirstDeltaKey();

            if (pKey) {
                switch (pTrack[i].ParamType) {
                    case 1: {
                        *static_cast<float *>(pKey->Val) =
                            *reinterpret_cast<long *>(pfData) - *reinterpret_cast<float *>(static_cast<unsigned char *>(pBase->Val));
                        break;
                    }
                    case 2: {
                        *reinterpret_cast<float *>(static_cast<unsigned char *>(pKey->Val)) =
                            *reinterpret_cast<float *>(pfData) - *reinterpret_cast<float *>(static_cast<unsigned char *>(pBase->Val));
                        break;
                    }
                    case 3: {
                        FEVector2 diff =
                            *reinterpret_cast<FEVector2 *>(pfData) - *reinterpret_cast<FEVector2 *>(static_cast<unsigned char *>(pBase->Val));
                        pKey->Val = diff;
                        break;
                    }
                    case 4: {
                        FEVector3 diff3 =
                            *reinterpret_cast<FEVector3 *>(pfData) - *reinterpret_cast<FEVector3 *>(static_cast<unsigned char *>(pBase->Val));
                        pKey->Val = diff3;
                        break;
                    }
                    case 5: {
                        FEQuaternion BaseQuat = *static_cast<FEQuaternion *>(pBase->Val);
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
                        FEColor colorDiff =
                            *reinterpret_cast<FEColor *>(pfData) - *reinterpret_cast<FEColor *>(static_cast<unsigned char *>(pBase->Val));
                        pKey->Val = colorDiff;
                        break;
                    }
                }
            }
        }
    }
}

void FEObject::SetCurrentScript(FEScript *pScript) {
    pCurrentScript = pScript;
    if (pScript) {
        SetupMoveToTracks();
    }
}

FEMessageResponse *FEObject::FindResponse(u32 MsgID) const {
    FEMessageResponse *pNode = GetFirstResponse();
    while (pNode) {
        if (pNode->GetMsgID() == MsgID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEVector3 &value, bool bRelative) {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack *pTrack = pScript->FindTrack(track);
        if (pTrack) {
            FEKeyNode *pKey = pTrack->GetBaseKey();
            if (bRelative) {
                *static_cast<FEVector3 *>(pKey->GetKeyData()->Val) += (value);
            } else {
                pKey->GetKeyData()->Val = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEVector3 *>(pData + offset) += (value);
    } else {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEVector3 *>(pData + offset) = value;
    }
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEVector2 &value, bool bRelative) {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack *pTrack = pScript->FindTrack(track);
        if (pTrack) {
            FEKeyNode *pKey = pTrack->GetBaseKey();
            if (bRelative) {
                *static_cast<FEVector2 *>(pKey->GetKeyData()->Val) += (value);
            } else {
                pKey->GetKeyData()->Val = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        unsigned long offset = GetDataOffset(track);
        reinterpret_cast<FEVector2 *>(pData + offset)->operator+=(value);
    } else {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEVector2 *>(pData + offset) = value;
    }
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEColor &value, bool bRelative) {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack *pTrack = pScript->FindTrack(track);
        if (pTrack) {
            FEKeyNode *pKey = pTrack->GetBaseKey();
            if (bRelative) {
                *static_cast<FEColor *>(pKey->GetKeyData()->Val) += value;
            } else {
                pKey->GetKeyData()->Val = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEColor *>(pData + offset) += value;
    } else {
        unsigned long offset = GetDataOffset(track);
        *reinterpret_cast<FEColor *>(pData + offset) = value;
    }
}

// STRIPPED
void FEObject::SetPivot(const FEVector3 &pivot, bool bRelative) {}

void FEObject::SetPosition(const FEVector3 &position, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    if (bRelative) {
        FEVector3 zero(0.0f, 0.0f, 0.0f);
        if (!CloseEnoughPosition(position, zero)) {
            Flags |= FF_DirtyCode;
        }
    } else {
        FEObjData *pData = GetObjData();
        if (!CloseEnoughPosition(position, pData->Pos)) {
            Flags |= FF_DirtyCode;
        }
    }
    SetTrackValue(FETrack_Position, position, bRelative);
}

void FEObject::SetRotation(const FEQuaternion &rotation, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    Flags |= FF_DirtyCode;
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    while (pScript) {
        FEKeyTrack *pTrack = pScript->FindTrack(FETrack_Rotation);
        if (pTrack) {
            FEKeyNode *pKey = pTrack->GetBaseKey();
            if (bRelative) {
                *static_cast<FEQuaternion *>(pKey->GetKeyData()->Val) *= (rotation);
            } else {
                pKey->GetKeyData()->Val = rotation;
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

// STRIPPED
void FEObject::SetSize(const FEVector3 &size, bool bRelative) {}

void FEObject::SetColor(const FEColor &color, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    bool bClose;
    if (bRelative) {
        FEColor zero(0);
        bClose = CloseEnoughColor(color, zero);
    } else {
        FEObjData *pData = GetObjData();
        bClose = CloseEnoughColor(color, pData->Col);
    }
    if (!bClose) {
        Flags |= FF_DirtyCode;
    }
    SetTrackValue(FETrack_Color, color, bRelative);
}

void FEObject::SetScript(u32 ID, bool bForce) {
    FEScript *pScript = FindScript(ID);
    SetScript(pScript, bForce);
}

void FEObject::SetScript(FEScript *pScript, bool bForce) {
    if (!bForce && pScript == pCurrentScript) {
        return;
    }
    SetCurrentScript(pScript);
    pCurrentScript->CurTime = 0;
}

u32 FEObject::GetDataOffset(FEKeyTrack_Indices track) {
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

FEObject *FEObject::Clone(bool bReference) {
    return FNEW FEObject(*this, bReference);
}

static const FEColoredImageData MaximumObjData; // size: 0x94, address: 0x80473D68, Decl: speed/indep/src/feng/FEObject.cpp:606
