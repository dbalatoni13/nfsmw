#include "FEObject.h"
#include "FETypes.h"
#include "FEScript.h"
#include "FESlotPool.h"
#include "FEngStandard.h"
#include "FEMessageResponse.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEngine.h"

FEMultiPool ObjDataPool;                                   // size: 0x10, address: 0x80473D58, Decl: 19
FEObjectDestructorCallback *FEObject::pDestructorCallback; // size: 0x4, address: 0x8041D154, Decl: 20

static const float PositionEpsilon = 0.000001f; // size: 0x4, Decl: 23
static const float SizeEpsilon = 0.000001f;     // size: 0x4, Decl: 24
static const i32 ColorEpsilon = 1;              // size: 0x4, Decl: 25

static bool Close(float x, float y, float epsilon) {
    return x + epsilon >= y && x - epsilon <= y;
}

static bool Close(i32 x, i32 y, i32 epsilon) {
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

// UNSOLVED
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
    FEScript *pSrcScript;
    FEScript *pScript;
    FEMessageResponse *pSrcResp = static_cast<FEMessageResponse *>(Object.Responses.GetHead());
    FEMessageResponse *pResp = static_cast<FEMessageResponse *>(Object.Responses.GetHead());
    while (pSrcResp != nullptr) {
        pResp = new FEMessageResponse();
        u32 i = 0;
        u32 Count = pSrcResp->GetCount();
        pResp->SetCount(Count);
        pResp->SetMsgID(pSrcResp->GetMsgID());
        for (i = 0; i < Count; i++) {
            *pResp->GetResponse(i) = *pSrcResp->GetResponse(i);
        }
        Responses.AddTail(pResp);
        pSrcResp = static_cast<FEMessageResponse *>(pSrcResp->GetNext());
    }

    for (pSrcScript = static_cast<FEScript *>(Object.Scripts.GetHead()); pSrcScript != nullptr;
         pSrcScript = static_cast<FEScript *>(pSrcScript->GetNext())) {
        Scripts.AddTail(new FEScript(*pSrcScript, bReference));
    }

    SetCurrentScript(FindScript(Object.pCurrentScript->ID));

    pSrcScript = static_cast<FEScript *>(Object.Scripts.GetHead());
    pScript = GetFirstScript();
    while (pSrcScript != nullptr) {
        if (pSrcScript->pChainTo != nullptr) {
            pScript->pChainTo = FindScript(pSrcScript->pChainTo->ID);
        }
        pScript = pScript->GetNext();
        pSrcScript = static_cast<FEScript *>(pSrcScript->GetNext());
    }
}

FEObject::~FEObject() {
    if (pDestructorCallback != nullptr) {
        pDestructorCallback->OnDestroy(this);
    }
    ObjDataPool.Free(pData);
    if (pName != nullptr) {
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
    if (pName != nullptr) {
        delete[] pName;
        pName = nullptr;
    }
    NameHash = -1;
    if (pNewName != nullptr) {
        int Len = FEngStrLen(pNewName);

        pName = FNEW char[Len + 1];
        FEngStrCpy(pName, pNewName);
        NameHash = FEHashUpper(pName);
    }
}

FEScript *FEObject::FindScript(u32 ID) const {
    FEScript *pScript = GetFirstScript();
    if (pScript != nullptr) {
        while (pScript->ID != ID) {
            pScript = pScript->GetNext();
            if (pScript == nullptr)
                break;
        }
    }
    return pScript;
}

// UNSOLVED
void FEObject::SetupMoveToTracks() {
    FEKeyTrack *pTrack = pCurrentScript->pTracks;
    u32 NumTracks = pCurrentScript->TrackCount;
    u8 *pDataPtr;
    FEKeyNode *pBase;
    FEKeyNode *pKey;

    for (u32 i = 0; i < NumTracks; i++) {
        pTrack[i].InterpAction &= 0x7F;

        if (pTrack[i].InterpType >= 3 && pTrack[i].InterpType < 5) {
            pDataPtr = reinterpret_cast<u8 *>(pData + pTrack[i].LongOffset * 4);
            pBase = pTrack[i].GetBaseKey();
            pKey = pTrack[i].GetFirstDeltaKey();

            if (pKey != nullptr) {
                switch (pTrack[i].ParamType) {
                    case PT_Int: {
                        reinterpret_cast<i32 &>(pKey->Val.Data) = reinterpret_cast<i32 &>(pDataPtr) - reinterpret_cast<i32 &>(pBase->Val);
                        break;
                    }
                    case PT_Float: {
                        reinterpret_cast<float &>(pKey->Val.Data) = reinterpret_cast<float &>(pDataPtr) - reinterpret_cast<float &>(pBase->Val.Data);
                        break;
                    }
                    case PT_Vector2:
                        reinterpret_cast<FEVector2 &>(pKey->Val.Data) =
                            reinterpret_cast<FEVector2 &>(pDataPtr) - reinterpret_cast<FEVector2 &>(pBase->Val.Data);
                        break;

                    case PT_Vector3:
                        reinterpret_cast<FEVector3 &>(pKey->Val.Data) =
                            reinterpret_cast<FEVector3 &>(pDataPtr) - reinterpret_cast<FEVector3 &>(pBase->Val.Data);
                        break;

                    case PT_Quaternion: {
                        FEQuaternion BaseQuat = reinterpret_cast<FEQuaternion &>(pBase->Val);
                        BaseQuat.Conjugate();
                        pKey->Val = reinterpret_cast<FEQuaternion &>(pDataPtr) * BaseQuat;
                        break;
                    }
                    case PT_Color:
                        pKey->Val = reinterpret_cast<FEColor &>(pDataPtr) - reinterpret_cast<FEColor &>(pBase->Val.Data);
                        break;
                }
            }
        }
    }
}

void FEObject::SetCurrentScript(FEScript *pScript) {
    pCurrentScript = pScript;
    if (pScript != nullptr) {
        SetupMoveToTracks();
    }
}

FEMessageResponse *FEObject::FindResponse(u32 MsgID) const {
    FEMessageResponse *pNode = GetFirstResponse();
    while (pNode != nullptr) {
        if (pNode->GetMsgID() == MsgID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEVector3 &value, bool bRelative) {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    FEKeyTrack *pTrack;
    FEKeyNode *pKey;

    while (pScript != nullptr) {
        pTrack = pScript->FindTrack(track);
        if (pTrack != nullptr) {
            pKey = &pTrack->BaseKey;
            if (bRelative) {
                reinterpret_cast<FEVector3 &>(pKey->Val) += value;
            } else {
                pKey->Val = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }

    if (bRelative) {
        *reinterpret_cast<FEVector3 *>(pData + GetDataOffset(track)) += value;
    } else {
        *reinterpret_cast<FEVector3 *>(pData + GetDataOffset(track)) = value;
    }
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEVector2 &value, bool bRelative) {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    FEKeyTrack *pTrack;
    FEKeyNode *pKey;

    while (pScript != nullptr) {
        pTrack = pScript->FindTrack(track);
        if (pTrack != nullptr) {
            pKey = &pTrack->BaseKey;
            if (bRelative) {
                reinterpret_cast<FEVector2 &>(pKey->Val) += value;
            } else {
                pKey->Val = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        *reinterpret_cast<FEVector2 *>(pData + GetDataOffset(track)) += value;
    } else {
        *reinterpret_cast<FEVector2 *>(pData + GetDataOffset(track)) = value;
    }
}

void FEObject::SetTrackValue(FEKeyTrack_Indices track, const FEColor &value, bool bRelative) {
    FEScript *pScript = static_cast<FEScript *>(Scripts.GetHead());
    FEKeyTrack *pTrack;
    FEKeyNode *pKey;

    while (pScript != nullptr) {
        pTrack = pScript->FindTrack(track);
        if (pTrack != nullptr) {
            pKey = &pTrack->BaseKey;
            if (bRelative) {
                *reinterpret_cast<FEColor *>(&pKey->Val) += value;
            } else {
                pKey->Val = value;
            }
            pTrack->InterpAction &= 0x7F;
        }
        pScript = pScript->GetNext();
    }
    if (bRelative) {
        *reinterpret_cast<FEColor *>(pData + GetDataOffset(track)) += value;
    } else {
        *reinterpret_cast<FEColor *>(pData + GetDataOffset(track)) = value;
    }
}

// STRIPPED
void FEObject::SetPivot(const FEVector3 &pivot, bool bRelative) {}

void FEObject::SetPosition(const FEVector3 &position, bool bRelative) {
    if (Type > 0xFF) {
        return;
    }
    if (bRelative) {
        if (!CloseEnoughPosition(position, FEVector3(0.0f, 0.0f, 0.0f))) {
            Flags |= FF_DirtyCode;
        }
    } else {
        if (!CloseEnoughPosition(position, GetObjData()->Pos)) {
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
    FEKeyTrack *pTrack;
    FEKeyNode *pKey;
    while (pScript != nullptr) {
        pTrack = pScript->FindTrack(FETrack_Rotation);
        if (pTrack != nullptr) {
            pKey = pTrack->GetBaseKey();
            if (bRelative) {
                *static_cast<FEQuaternion *>(pKey->Val) *= rotation;
            } else {
                pKey->Val = rotation;
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
    if (bRelative) {
        if (!CloseEnoughColor(color, FEColor(0))) {
            Flags |= FF_DirtyCode;
        }
    } else {
        if (!CloseEnoughColor(color, GetObjData()->Col)) {
            Flags |= FF_DirtyCode;
        }
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

static const FEColoredImageData MaximumObjData; // size: 0x94, address: 0x80473D68, Decl: 606
