#include <new>

#include "FEObject.h"
#include "FETypes.h"
#include "FEScript.h"
#include "FESlotPool.h"
#include "FEngStandard.h"
#include "FEMessageResponse.h"
#include "fengine.h"

extern FEMultiPool ObjDataPool;

FEObjectDestructorCallback* FEObject::pDestructorCallback;

FEObject::FEObject()
    : GUID(FEngine::SysGUID++) //
    , NameHash(0) //
    , pName(nullptr) //
    , Flags(0) //
    , RenderContext(0) //
    , Handle(0) //
    , UserParam(0) //
    , pData(nullptr) //
    , DataSize(0) //
    , Cached(nullptr) //
{
}

FEObject::FEObject(const FEObject& Object, bool bReference)
    : GUID(FEngine::SysGUID++) //
    , NameHash(0) //
    , pName(nullptr) //
    , Flags(0) //
    , RenderContext(0) //
    , Handle(0) //
    , UserParam(0) //
    , pData(nullptr) //
    , DataSize(0) //
    , pCurrentScript(nullptr) //
    , Cached(nullptr) //
{
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
        pNewResp->SetCount(pSrcResp->GetCount());
        pNewResp->MsgID = pSrcResp->MsgID;
        unsigned long count = pSrcResp->GetCount();
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
    DataSize = Size;
    if (Size != 0) {
        pData = ObjDataPool.Alloc(Size);
    }
}

void FEObject::SetName(const char* pNewName) {
    if (pName) {
        delete[] pName;
        pName = nullptr;
    }
    if (pNewName) {
        int len = FEngStrLen(pNewName);
        pName = new char[len + 1];
        FEngStrCpy(pName, pNewName);
    }
}

FEScript* FEObject::FindScript(unsigned long ID) const {
    FEScript* pScript = static_cast<FEScript*>(Scripts.GetHead());
    while (pScript) {
        if (pScript->ID == ID) {
            return pScript;
        }
        pScript = pScript->GetNext();
    }
    return pScript;
}

void FEObject::SetCurrentScript(FEScript* pScript) {
    pCurrentScript = pScript;
    if (pScript) {
        SetupMoveToTracks();
    }
}

FEMessageResponse* FEObject::FindResponse(unsigned long MsgID) const {
    FEMessageResponse* pResp = static_cast<FEMessageResponse*>(Responses.GetHead());
    while (pResp) {
        if (pResp->MsgID == MsgID) {
            return pResp;
        }
        pResp = pResp->GetNext();
    }
    return pResp;
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
    default:
        if (track >= FETrack_Color1 && track <= FETrack_Color4) {
            return (track - FETrack_Color1) * 0x10;
        }
        return 0;
    }
}

FEObject* FEObject::Clone(bool bReference) {
    FEObject* pObject = static_cast<FEObject*>(FEngMalloc(sizeof(FEObject), 0, 0));
    new (pObject) FEObject(*this, bReference);
    return pObject;
}
