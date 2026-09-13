#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEKeyInterpolators.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEngine.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEMovie.h"

u32 FEPackage::uHoldDirtyFlags = 0xFFFFFFFF; // size: 0x4, address: 0x8041D158, Decl: 26

bool PackageInitStateCB::Callback(FEObject *pObj) {
    pObj->SetCurrentScript(pObj->FindScript(FEHASH_INIT));
    pObj->pCurrentScript->CurTime = 0;
    pObj->Flags |= FEngDirtyFlagsMask;
    return true;
}

// UNSOLVED
FEPackage::FEPackage()
    : bExecuting(false), bUseIdleList(false), bIsLibrary(false), bStartEqualsAccept(false), bErrorScreen(false), Priority(0), Controllers(0xff),
      OldControllers(0xff), bInputEnabled(false), pFilename(nullptr), pParentPackage(nullptr), UserParam(0), NumRequests(0), pRequests(nullptr),
      NumMsgTargets(0), pMsgTargets(nullptr), NumLibRefs(0), pLibRefs(nullptr), pCurrentButton(nullptr), pResourceNames(nullptr),
      MouseObjectStates(nullptr), NumMouseObjects(0) {}

FEPackage::~FEPackage() {
    if (pFilename != nullptr) {
        delete[] pFilename;
    }
    if (pRequests != nullptr) {
        delete[] pRequests;
    }
    if (pMsgTargets != nullptr) {
        delete[] pMsgTargets;
    }
    if (pResourceNames != nullptr) {
        delete[] pResourceNames;
    }
    if (MouseObjectStates != nullptr) {
        delete[] MouseObjectStates;
    }

    FEObjectComment *pComment = static_cast<FEObjectComment *>(Comments.RemHead());
    while (pComment != nullptr) {
        if (pComment->pStr != nullptr) {
            delete[] pComment->pStr;
        }
        delete pComment;

        pComment = static_cast<FEObjectComment *>(Comments.RemHead());
    }

    if (pLibRefs != nullptr) {
        delete[] pLibRefs;
    }
}

void FEPackage::SetFilename(const char *pName) {
    if (pFilename != nullptr) {
        delete[] pFilename;
    }
    pFilename = nullptr;
    if (pName != nullptr) {
        int Len = FEngStrLen(pName);

        pFilename = FNEW char[Len + 1];
        FEngStrCpy(pFilename, pName);
    }
}

bool FEPackage::Startup(FEGameInterface *pGameInterface) {
    bool bResult = true;
    if (!pGameInterface->LoadResources(this, NumRequests, pRequests)) {
        bResult = false;
    }
    ConnectObjectResources();
    BuildMouseObjectStateList();
    return bResult;
}

void FEPackage::Shutdown(FEGameInterface *pGameInterface) {
    if (pGameInterface != nullptr) {
        pGameInterface->UnloadResources(this, NumRequests, pRequests);
    }
}

bool FEPackage::InitializePackage() {
    PackageInitStateCB initState;
    return ForAllObjects(initState);
}

FEMessageResponse *FEPackage::FindResponse(u32 MsgID) {
    FEMessageResponse *pNode = GetFirstResponse();
    while (pNode != nullptr) {
        if (pNode->GetMsgID() == MsgID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}

bool FEPackage::ForAllChildren(FEGroup *pGroup, FEObjectCallback &Callback) {
    FEObject *pObj = pGroup->GetFirstChild();
    while (pObj != nullptr) {
        if (!Callback.Callback(pObj)) {
            return false;
        }
        if (pObj->Type == FE_Group && !ForAllChildren(static_cast<FEGroup *>(pObj), Callback)) {
            return false;
        }
        pObj = pObj->GetNext();
    }
    return true;
}

bool FEPackage::ForAllObjects(FEObjectCallback &Callback) {
    FEObject *pObj = GetFirstObject();
    while (pObj != nullptr) {
        if (!Callback.Callback(pObj)) {
            return false;
        }
        if (pObj->Type == FE_Group && !ForAllChildren(static_cast<FEGroup *>(pObj), Callback)) {
            return false;
        }
        pObj = static_cast<FEObject *>(pObj->GetNext());
    }
    return true;
}

// total size: 0xC
// Decl: 211
class FEFindByHash : public FEObjectCallback {
  public:
    u32 Hash;         // offset 0x4, size 0x4, Decl: 213
    FEObject *pFound; // offset 0x8, size 0x4, Decl: 214

    bool Callback(FEObject *pObj) override { // Decl: 216
        if (pObj->NameHash == Hash) {
            pFound = pObj;
            return false;
        }
        return true;
    }
};

// STRIPPED
FEObject *FEPackage::FindObjectByName(const char *pName) {}

FEObject *FEPackage::FindObjectByHash(u32 NameHash) {
    FEFindByHash Find;
    Find.Hash = NameHash;
    Find.pFound = nullptr;
    ForAllObjects(Find);
    return Find.pFound;
}

// total size: 0xC
// Decl: 257
class FEFindByGUID : public FEObjectCallback {
  public:
    u32 GUID;         // offset 0x4, size 0x4, Decl: 259
    FEObject *pFound; // offset 0x8, size 0x4, Decl: 260

    bool Callback(FEObject *pObj) override { // Decl: 262
        if (pObj->GUID == GUID) {
            pFound = pObj;
            return false;
        }
        return true;
    }
};

FEObject *FEPackage::FindObjectByGUID(u32 GUID) {
    FEFindByGUID Find;
    Find.GUID = GUID;
    Find.pFound = nullptr;
    ForAllObjects(Find);
    return Find.pFound;
}

void FEPackage::IssueScriptMessages(FEngine *pEngine, FEObject *pObjPtr, FEScript *pScript, i32 tFrom, i32 tTo) {
    FEEvent *pEvents = &pScript->Events[0];
    int i;
    int Count = pScript->Events.GetCount();

    if (tTo < tFrom) {
        return;
    }

    if (tTo == pScript->Length) {
        tTo++;
    }

    i = 0;
    while (i < Count) {
        if (pEvents[i].tTime >= static_cast<u32>(tFrom)) {
            break;
        }
        i++;
    }

    while (i < Count && pEvents[i].tTime < static_cast<u32>(tTo)) {
        switch (pEvents[i].Target) {
            case 0:
                if (pEvents[i].EventID == 0x1B3909AA) {
                    FEObject *pTargetPtr = FindObjectByGUID(0);
                    SetCurrentButton(pTargetPtr, true);
                } else {
                    pEngine->QueueMessage(pEvents[i].EventID, pObjPtr, this, reinterpret_cast<FEObject *>(0), 0);
                }
                break;
            case 0xFFFFFFFF:
                pEngine->SendMessageToGame(pEvents[i].EventID, pObjPtr, this, 0);
                break;
            case 0xFFFFFFFC:
                pEngine->QueueMessage(pEvents[i].EventID, pObjPtr, this, reinterpret_cast<FEObject *>(0xFFFFFFFC), 0);
                break;
            case 0xFFFFFFFB:
                pEngine->QueueMessage(pEvents[i].EventID, pObjPtr, this, reinterpret_cast<FEObject *>(0xFFFFFFFB), 0);
                break;
            case 0xFFFFFFFA:
                pEngine->QueueMessage(pEvents[i].EventID, pObjPtr, this, reinterpret_cast<FEObject *>(0xFFFFFFFA), 0);
                break;
            default: {
                FEObject *pTargetPtr = FindObjectByGUID(pEvents[i].Target);
                if (pEvents[i].EventID == 0x1B3909AA) {
                    FEObject *pTargetPtr = FindObjectByGUID(pEvents[i].Target);
                    SetCurrentButton(pTargetPtr, true);
                    break;
                }
                if (pObjPtr != nullptr) {
                    pEngine->QueueMessage(pEvents[i].EventID, pObjPtr, this, pTargetPtr, 0);
                }
                break;
            }
        }
        i++;
    }
}

void FEPackage::UpdateGroup(FEGroup *pGroup, const i32 tDeltaTicks) {
    FEObject *pObj = pGroup->GetFirstChild();
    while (pObj != nullptr) {
        UpdateObject(pObj, tDeltaTicks);
        pObj->Flags |= pGroup->Flags & FEngDirtyFlagsMask;
        pObj = pObj->GetNext();
    }
}

uint32 eFrameCounter = 0; // size: 0x4, Decl: 400
uint32 eFrameCounterOLD;  // size: 0x4, address: 0x80473E0C, Decl: 401
uint32 objCount;          // size: 0x4, address: 0x80473E10, Decl: 402

// UNSOLVED
void FEPackage::UpdateObject(FEObject *pObject, const i32 tDeltaTicks) {
    if (eFrameCounterOLD == eFrameCounter) {
        objCount++;
    } else {
        objCount = 0;
        eFrameCounterOLD = eFrameCounter;
    }

    if (pObject->Flags & (FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform)) {
        pObject->Flags = pObject->Flags | FF_Dirty;
    } else {
        pObject->Flags = pObject->Flags & (FEPackage::uHoldDirtyFlags | ~FF_Dirty);
    }

    FEScript *pScript = pObject->pCurrentScript;
    int tPrevTime = pScript->CurTime;
    int ScrLength = pScript->Length;
    pScript->CurTime = tPrevTime + iTickIncrement;
    if (pScript->CurTime < 0) {
        pScript->CurTime = 0;
    }

    u32 PlayAction;
    if (pScript->CurTime >= ScrLength) {
        if (bExecuting) {
            if (pScript->pChainTo != nullptr) {
                UpdateObjectTracks(pObject, pScript);
                int tOverTime = pScript->CurTime - ScrLength;
                pScript->CurTime = 0;
                if (pScript->Events.GetCount()) {
                    IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, ScrLength);
                }
                pScript = pScript->pChainTo;
                pObject->SetCurrentScript(pScript);
                pScript->CurTime = tOverTime;
                if (pScript->Events.GetCount()) {
                    goto issueFrom0;
                }
            } else {
                PlayAction = pScript->Flags & 3;
                switch (PlayAction) {
                    case 0:
                        if (pScript->Events.GetCount()) {
                            IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, ScrLength);
                        }
                        pScript->CurTime = pScript->Length + 1;
                        break;
                    case 1:
                        if (pScript->Length > 0) {
                            if (pScript->Events.GetCount()) {
                                IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, ScrLength);
                            }
                            pScript->CurTime = pScript->CurTime - (pScript->CurTime / pScript->Length) * pScript->Length;
                            if (pScript->Events.GetCount()) {
                                IssueScriptMessages(pEnginePtr, pObject, pScript, 0, pScript->CurTime);
                            }
                            pObject->SetupMoveToTracks();
                        } else {
                            pScript->CurTime = 0;
                        }
                        break;
                    case 2:
                        if (pScript->Length > 0) {
                            int doubleLen = pScript->Length * 2;
                            pScript->CurTime = pScript->CurTime - (pScript->CurTime / doubleLen) * doubleLen;
                        } else {
                            pScript->CurTime = 0;
                        }
                        break;
                }
            }
            if (bExecuting && tPrevTime == pScript->CurTime && tPrevTime == pScript->Length + 1 && !(pObject->Flags & FF_DirtyCode)) {
                goto finalize;
            }
        }
    } else {
        if (bExecuting) {
            if (pScript->Events.GetCount() != 0) {
                PlayAction = pScript->Flags & 3;
                switch (PlayAction) {
                    case 0:
                        IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, pScript->CurTime);
                        break;
                    case 1:
                        if (pScript->CurTime < tPrevTime) {
                            IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, ScrLength);
                        issueFrom0:
                            IssueScriptMessages(pEnginePtr, pObject, pScript, 0, pScript->CurTime);
                            break;
                        }
                        IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, pScript->CurTime);
                        break;
                    case 2:
                        if (tPrevTime < ScrLength) {
                            IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime, pScript->CurTime);
                        } else {
                            IssueScriptMessages(pEnginePtr, pObject, pScript, tPrevTime - ScrLength, 0);
                            IssueScriptMessages(pEnginePtr, pObject, pScript, 0, pScript->CurTime);
                        }
                        break;
                }
            }
            if (bExecuting && tPrevTime == pScript->CurTime && tPrevTime == pScript->Length + 1 && !(pObject->Flags & FF_DirtyCode)) {
                goto finalize;
            }
        }
    }

    UpdateObjectTracks(pObject, pScript);

finalize:
    switch (pObject->Type) {
        case FE_Group:
            UpdateGroup(static_cast<FEGroup *>(pObject), tDeltaTicks);
            break;
        case FE_List:
            static_cast<FEListBox *>(pObject)->Update(static_cast<float>(tDeltaTicks));
            break;
        case FE_CodeList:
            static_cast<FECodeListBox *>(pObject)->Update(static_cast<float>(tDeltaTicks));
            break;
        case FE_Movie:
            if (bExecuting) {
                static_cast<FEMovie *>(pObject)->Update(tDeltaTicks);
            }
            break;
    }

    if (bExecuting == true && tPrevTime == pScript->CurTime && tPrevTime == pScript->Length + 1 && !(pObject->Flags & FF_DirtyCode)) {
        pObject->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyTransform;
    }

    pObject->Flags = pObject->Flags & (FEPackage::uHoldDirtyFlags | ~FF_DirtyCode);
    if (pObject->Flags & FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform) {
        pObject->Flags = pObject->Flags | FF_Dirty;
    }
}

void FEPackage::UpdateObjectTracks(FEObject *pObject, FEScript *pScript) {
    int tTime = pScript->CurTime;
    u8 TrackCount;
    u8 Track;
    FEKeyTrack *pTrack = pScript->pTracks;
    u8 *pDestPtr = pObject->pData;

    if (bExecuting) {
        if ((pTrack != nullptr) && pTrack->LongOffset == 0) {
            if (pTrack->InterpAction & 0x80) {
                pObject->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyColor;
            } else {
                pObject->Flags |= FF_DirtyColor;
            }
            FEKeyInterpFast(pTrack, tTime, pDestPtr);
        } else {
            pObject->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyColor;
        }
        u32 UpdateMask = 0x80;
        if (pObject->GetObjData()->Col.a) {
            TrackCount = pScript->TrackCount;
            for (Track = 0; Track < TrackCount; Track++) {
                UpdateMask = pTrack->InterpAction & UpdateMask;
                FEKeyInterpFast(pTrack, tTime, pDestPtr + pTrack->LongOffset * 4);
                pTrack++;
            }
        }
        if (UpdateMask) {
            pObject->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyTransform;
        } else {
            pObject->Flags |= FF_DirtyTransform;
        }
    } else {
        if ((pTrack != nullptr) && pTrack->LongOffset == 0) {
            FEKeyInterp(pTrack, tTime, pDestPtr);
        }
        if (pObject->GetObjData()->Col.a) {
            TrackCount = pScript->TrackCount;

            for (Track = 0; Track < TrackCount; Track++) {
                FEKeyInterp(pTrack, tTime, pDestPtr + pTrack->LongOffset * 4);
                pTrack++;
            }
        }
    }

    if (pObject->Flags & (FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform)) {
        pObject->Flags = pObject->Flags | FF_Dirty;
    }
}

// total size: 0x8
class MouseStateArrayOffsetUpdater : public FEObjectCallback {
  public:
    FEPackage *pPack; // offset 0x4, size 0x4

    bool Callback(FEObject *pObj) override {
        if (pObj->Flags & FF_MouseObject) {
            pPack->UpdateMouseObjectOffsets(pObj);
        }
        return true;
    };
};

// Decl: 765
void FEPackage::Update(FEngine *pEngine, const i32 tDeltaTicks) {
    FEObject *pObject = static_cast<FEObject *>(Objects.GetHead());
    pEnginePtr = pEngine;
    iTickIncrement = tDeltaTicks;

    while (pObject != nullptr) {
        UpdateObject(pObject, tDeltaTicks);
        pObject = pObject->GetNext();
    }

    if (NumMouseObjects > 0) {
        NumMouseObjectsCounter = 0;
        MouseStateArrayOffsetUpdater the_udater;
        the_udater.pPack = this;
        ForAllObjects(the_udater);
    }
}

void FEPackage::SetCurrentButton(FEObject *pNewButton, bool bSendMsgs) {
    if (bSendMsgs) {
        if (pCurrentButton != nullptr) {
            pEnginePtr->QueueMessage(0x55d1e635, nullptr, this, pCurrentButton, 0);
            pEnginePtr->QueueMessage(0x55d1e635, pCurrentButton, this, reinterpret_cast<FEObject *>(0xfffffffb), 0);
        }
        if (pNewButton != nullptr) {
            pEnginePtr->QueueMessage(0xabc08912, nullptr, this, pNewButton, 0);
            pEnginePtr->QueueMessage(0xabc08912, pNewButton, this, reinterpret_cast<FEObject *>(0xfffffffb), 0);
        }
    }
    pCurrentButton = pNewButton;
}

// total size: 0xC
// Decl: 888
class FEGetNumSpawnResponses : public FEObjectCallback {
  public:
    ~FEGetNumSpawnResponses() override {} // Decl: 888

    u32 *pNumLoad;      // offset 0x4, size 0x4, Decl: 890
    u32 *pStringLength; // offset 0x8, size 0x4, Decl: 891

    FEGetNumSpawnResponses(u32 &TheNumLoad, u32 &TheStringLength) {} // Decl: 893

    bool Callback(FEObject *pObj) override {} // Decl: 896
};

// total size: 0x10
// Decl: 904
class FESetSpawnResponses : public FEObjectCallback {
  public:
    ~FESetSpawnResponses() override {} // Decl: 904

    u32 *pNumLoad;                 // offset 0x4, size 0x4, Decl: 906
    u32 *pStringLength;            // offset 0x8, size 0x4, Decl: 907
    FELoadPackageDescArray *pDesc; // offset 0xC, size 0x4, Decl: 908

    FESetSpawnResponses(u32 &TheNumLoad, u32 &TheStringLength, FELoadPackageDescArray *pTheDesc) {} // Decl: 910

    bool Callback(FEObject *pObj) override {} // Decl: 913
};

// total size: 0xC
// Decl: 968
class ResourceConnector : public FEObjectCallback {
  public:
    FEPackage *pPack;             // offset 0x4, size 0x4, Decl: 970
    FEResourceRequest **pReqList; // offset 0x8, size 0x4, Decl: 971

    bool Callback(FEObject *pObj) override; // Decl: 978

    void ConnectListBoxResources(FEListBox *pList); // Decl: 1006
};

bool ResourceConnector::Callback(FEObject *pObj) { // Decl: 978
    switch (pObj->Type) {
        case FE_List:
            ConnectListBoxResources(static_cast<FEListBox *>(pObj));
        case FE_CodeList:
        case FE_Group:
            break;
        default:
            if (pObj->ResourceIndex != 0xFFFF) {
                pObj->UserParam = (*pReqList)[pObj->ResourceIndex].UserParam;
                pObj->Handle = (*pReqList)[pObj->ResourceIndex].Handle;
            }
    }
    return true;
};

void ResourceConnector::ConnectListBoxResources(FEListBox *pList) { // Decl: 1006
    pList->SetCurrentColumn(0);
    pList->SetCurrentRow(0);

    u32 Rows = pList->GetNumRows();
    u32 Cols = pList->GetNumColumns();
    u32 i;
    u32 j;

    for (j = 0; j < Rows; j++) {
        for (i = 0; i < Cols; i++) {
            u32 ulIndex = pList->GetCurrentCellData()->stResource.ResourceIndex;
            if (ulIndex != INVALID_LIST_ENTRY) {
                pList->SetCellResource((*pReqList)[ulIndex].Handle, (*pReqList)[ulIndex].UserParam, ulIndex);
            } else {
                pList->SetCellResource(0, 0, INVALID_LIST_ENTRY);
            }
            pList->IncrementCellByColumn();
        }
    }
};

void FEPackage::ConnectObjectResources() {
    ResourceConnector resConnector;
    resConnector.pPack = this;
    resConnector.pReqList = &pRequests;
    ForAllObjects(resConnector);
}

FEObjectMouseState::FEObjectMouseState() {
    pObject = nullptr;
    Offset.h = 0.0f;
    Offset.v = 0.0f;
    Flags = 0;
}

FEObjectMouseState::~FEObjectMouseState() {}

// total size: 0x8
// Decl: 1074
class MouseStateObjectCounter : public FEObjectCallback {
  public:
    int NumMouseObjects; // offset 0x4, size 0x4, Decl: 1076

    bool Callback(FEObject *pObj) override { // Decl: 1078
        if (pObj->Flags & FF_MouseObject) {
            NumMouseObjects++;
        }
        return true;
    }
};

// total size: 0x8
// Decl: 1089
class MouseStateArrayBuilder : public FEObjectCallback {
  public:
    FEPackage *pPack; // offset 0x4, size 0x4, Decl: 1091

    bool Callback(struct FEObject *pObj) override { // Decl: 1093
        if (pObj->Flags & FF_MouseObject) {
            pPack->AddMouseObjectState(pObj);
        }
        return true;
    }
};

void FEPackage::BuildMouseObjectStateList() {
    if (MouseObjectStates != nullptr) {
        delete[] MouseObjectStates;
        MouseObjectStates = nullptr;
        NumMouseObjects = 0;
    }
    MouseStateObjectCounter the_counter;
    the_counter.NumMouseObjects = 0;
    ForAllObjects(the_counter);
    if (the_counter.NumMouseObjects > 0) {
        MouseObjectStates = FNEW FEObjectMouseState[the_counter.NumMouseObjects];
        MouseStateArrayBuilder the_builder;
        the_builder.pPack = this;
        ForAllObjects(the_builder);
    }
}

bool OffsetCalculatron(u32 mouseable, FEObject *obj, FEPoint &Offset) {
    if (mouseable == obj->NameHash) {
        FEObjData *objData = obj->GetObjData();
        Offset.h += objData->Pos.x;
        Offset.v += objData->Pos.y;
        return true;
    } else if (obj->Type == FE_Group && (static_cast<FEGroup *>(obj)->FindChildRecursive(mouseable) != nullptr)) {
        FEObjData *objData = obj->GetObjData();
        Offset.h += objData->Pos.x;
        Offset.v += objData->Pos.y;
        FEObject *pChild = static_cast<FEGroup *>(obj)->GetFirstChild();
        while (pChild != nullptr) {
            OffsetCalculatron(mouseable, pChild, Offset);
            pChild = static_cast<FEObject *>(pChild->GetNext());
        }
        return true;
    }
    return false;
}

void FEPackage::AddMouseObjectState(FEObject *obj) {
    if (obj == nullptr) {
        return;
    }
    FEObject *pObj = GetFirstObject();
    u32 mouseable = obj->NameHash;
    while (pObj != nullptr) {
        if (pObj->Type == FE_Group) {
            if ((static_cast<FEGroup *>(pObj)->FindChildRecursive(mouseable) != nullptr) || mouseable == pObj->NameHash) {
                FEPoint p;
                if (OffsetCalculatron(mouseable, pObj, p)) {
                    MouseObjectStates[NumMouseObjects].Offset = p;
                    break;
                }
            }
        } else if (mouseable == pObj->NameHash) {
            FEPoint p;
            if (OffsetCalculatron(mouseable, pObj, p)) {
                MouseObjectStates[NumMouseObjects].Offset = p;
                break;
            }
        }
        pObj = static_cast<FEObject *>(pObj->GetNext());
    }
    MouseObjectStates[NumMouseObjects].pObject = obj;
    NumMouseObjects++;
}

void FEPackage::UpdateMouseObjectOffsets(FEObject *obj) {
    if (obj == nullptr) {
        return;
    }
    FEObject *pObj = GetFirstObject();
    u32 mouseable = obj->NameHash;
    while (pObj != nullptr) {
        if (pObj->Type == FE_Group) {
            if ((static_cast<FEGroup *>(pObj)->FindChildRecursive(mouseable) != nullptr) || mouseable == pObj->NameHash) {
                FEPoint p;
                if (OffsetCalculatron(mouseable, pObj, p)) {
                    MouseObjectStates[NumMouseObjectsCounter++].Offset = p;
                    break;
                }
            }
        } else if (mouseable == pObj->NameHash) {
            FEPoint p;
            if (OffsetCalculatron(mouseable, pObj, p)) {
                MouseObjectStates[NumMouseObjectsCounter++].Offset = p;
                break;
            }
        }
        pObj = static_cast<FEObject *>(pObj->GetNext());
    }
}

void FEPackage::SetNumLibraryRefs(u32 NewCount) {
    if (NewCount == 0) {
        if (pLibRefs != nullptr) {
            delete[] pLibRefs;
        }
        pLibRefs = nullptr;
        return;
    }
    FELibraryRef *pNewList = FNEW FELibraryRef[NewCount];
    u32 CopyCount = NewCount;
    if (NewCount > NumLibRefs) {
        CopyCount = NumLibRefs;
    }
    if (CopyCount != 0) {
        FEngMemCpy(pNewList, pLibRefs, CopyCount * sizeof(FELibraryRef));
    }
    if (pLibRefs != nullptr) {
        delete[] pLibRefs;
    }
    pLibRefs = pNewList;
    NumLibRefs = NewCount;
}

FELibraryRef *FEPackage::FindLibraryReference(u32 ObjGUID) const {
    for (u32 i = 0; i < NumLibRefs; i++) {
        if (pLibRefs[i].ObjGUID == ObjGUID) {
            return &pLibRefs[i];
        }
    }
    return nullptr;
}
