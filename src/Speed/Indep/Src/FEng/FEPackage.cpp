#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/fengine.h"
#include "Speed/Indep/Src/FEng/FEResourceRequest.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEMovie.h"

// Forward declarations for types only needed locally as pointer members.
// Their struct definitions come from FEngine.cpp earlier in the jumbo build.

// total size: 0xC
struct FELibraryRef {
    unsigned long ObjGUID;       // offset 0x0, size 0x4
    unsigned long PackNameHash;  // offset 0x4, size 0x4
    unsigned long LibGUID;       // offset 0x8, size 0x4

    FELibraryRef()
        : ObjGUID(0) //
        , PackNameHash(0xFFFFFFFF) //
        , LibGUID(0) {}
};

// total size: 0x14
struct FEObjectComment : public FEMinNode {
    unsigned long ObjectGUID; // offset 0xC, size 0x4
    char* pStr;               // offset 0x10, size 0x4

    inline ~FEObjectComment() override {}
};

// FEMsgTargetList defined in FEPackage.h

unsigned long FEPackage::uHoldDirtyFlags;

FEPackage::FEPackage()
    : bExecuting(false) //
    , bUseIdleList(false) //
    , bIsLibrary(false) //
    , bStartEqualsAccept(false) //
    , bErrorScreen(false) //
    , Priority(0) //
    , Controllers(0xff) //
    , OldControllers(0xff) //
    , bInputEnabled(false) //
    , pFilename(nullptr) //
    , pParentPackage(nullptr) //
    , UserParam(0) //
    , NumRequests(0) //
    , pRequests(nullptr) //
    , NumMsgTargets(0) //
    , pMsgTargets(nullptr) //
    , NumLibRefs(0) //
    , pLibRefs(nullptr) //
    , pCurrentButton(nullptr) //
    , pResourceNames(nullptr) //
    , MouseObjectStates(nullptr) //
    , NumMouseObjects(0)
{
}

FEPackage::~FEPackage() {
    if (pFilename) {
        delete[] pFilename;
    }
    if (pRequests) {
        delete[] pRequests;
    }
    if (pMsgTargets) {
        delete[] pMsgTargets;
    }
    if (pResourceNames) {
        delete[] pResourceNames;
    }
    if (MouseObjectStates) {
        delete[] MouseObjectStates;
    }
    FEObjectComment* pComment;
    while ((pComment = static_cast<FEObjectComment*>(Comments.RemHead())) != nullptr) {
        if (pComment->pStr) {
            delete[] pComment->pStr;
        }
        delete pComment;
    }
    if (pLibRefs) {
        delete[] pLibRefs;
    }
}

bool FEPackage::InitializePackage() {
    PackageInitStateCB cb;
    return ForAllObjects(cb);
}

FEObject* FEPackage::FindObjectByHash(unsigned long NameHash) {
    FEFindByHash finder;
    finder.Hash = NameHash;
    finder.pFound = nullptr;
    ForAllObjects(finder);
    return finder.pFound;
}

FEObject* FEPackage::FindObjectByGUID(unsigned long GUID) {
    FEFindByGUID finder;
    finder.GUID = GUID;
    finder.pFound = nullptr;
    ForAllObjects(finder);
    return finder.pFound;
}

bool FEPackage::ForAllChildren(FEGroup* pGroup, FEObjectCallback& Callback) {
    FEObject* pObj = static_cast<FEObject*>(pGroup->Children.GetHead());
    while (pObj) {
        if (!Callback.Callback(pObj)) {
            return false;
        }
        if (pObj->Type == 5 && !ForAllChildren(static_cast<FEGroup*>(pObj), Callback)) {
            return false;
        }
        pObj = static_cast<FEObject*>(pObj->GetNext());
    }
    return true;
}

bool FEPackage::ForAllObjects(FEObjectCallback& Callback) {
    FEObject* pObj = static_cast<FEObject*>(Objects.GetHead());
    while (pObj) {
        if (!Callback.Callback(pObj)) {
            return false;
        }
        if (pObj->Type == 5 && !ForAllChildren(static_cast<FEGroup*>(pObj), Callback)) {
            return false;
        }
        pObj = static_cast<FEObject*>(pObj->GetNext());
    }
    return true;
}

void FEPackage::SetCurrentButton(FEObject* pNewButton, bool bSendMsgs) {
    if (bSendMsgs) {
        if (pCurrentButton) {
            pEnginePtr->QueueMessage(0x55d1e635, nullptr, this, pCurrentButton, 0);
            pEnginePtr->QueueMessage(0x55d1e635, pCurrentButton, this, reinterpret_cast<FEObject*>(0xfffffffb), 0);
        }
        if (pNewButton) {
            pEnginePtr->QueueMessage(0xabc08912, nullptr, this, pNewButton, 0);
            pEnginePtr->QueueMessage(0xabc08912, pNewButton, this, reinterpret_cast<FEObject*>(0xfffffffb), 0);
        }
    }
    pCurrentButton = pNewButton;
}

bool PackageInitStateCB::Callback(FEObject* pObj) {
    pObj->SetCurrentScript(pObj->FindScript(0x1744b3));
    pObj->pCurrentScript->CurTime = 0;
    pObj->Flags |= 0x3c00000;
    return true;
}

bool FEFindByHash::Callback(FEObject* pObj) {
    if (pObj->NameHash == Hash) {
        pFound = pObj;
        return false;
    }
    return true;
}

bool FEFindByGUID::Callback(FEObject* pObj) {
    if (pObj->GUID == GUID) {
        pFound = pObj;
        return false;
    }
    return true;
}

bool MouseStateObjectCounter::Callback(FEObject* pObj) {
    if (pObj->Flags & 0x20000) {
        NumMouseObjects++;
    }
    return true;
}

bool MouseStateArrayBuilder::Callback(FEObject* pObj) {
    if (pObj->Flags & 0x20000) {
        pPack->AddMouseObjectState(pObj);
    }
    return true;
}

bool MouseStateArrayOffsetUpdater::Callback(FEObject* pObj) {
    if (pObj->Flags & 0x20000) {
        pPack->UpdateMouseObjectOffsets(pObj);
    }
    return true;
}

extern unsigned int eFrameCounter;
extern unsigned int eFrameCounterOLD;
extern unsigned int objCount;

void FEKeyInterp(FEKeyTrack* pTrack, long tTime, void* pOutData);
void FEKeyInterpFast(FEKeyTrack* pTrack, long tTime, void* pOutData);

void FEPackage::UpdateGroup(FEGroup* pGroup, long tDeltaTicks) {
    FEObject* pChild = pGroup->GetFirstChild();
    while (pChild) {
        UpdateObject(pChild, tDeltaTicks);
        pChild->Flags |= pGroup->Flags & 0x3C00000;
        pChild = static_cast<FEObject*>(pChild->next);
    }
}

void FEPackage::UpdateObjectTracks(FEObject* pObj, FEScript* pScript) {
    unsigned char* pData = pObj->pData;
    int CurTime = pScript->CurTime;
    FEKeyTrack* pTracks = pScript->pTracks;

    if (bExecuting) {
        if (pTracks && pTracks->LongOffset == 0) {
            if (pTracks->InterpAction & 0x80) {
                pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFF7FFFFF;
            } else {
                pObj->Flags |= 0x800000;
            }
            FEKeyInterpFast(pTracks, CurTime, pData);
        } else {
            pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFF7FFFFF;
        }
        unsigned char bDone = 0x80;
        if (*reinterpret_cast<int*>(pObj->pData + 0xC)) {
            unsigned char TrackCount = static_cast<unsigned char>(pScript->TrackCount);
            for (unsigned char i = 0; i < TrackCount; i++, pTracks++) {
                bDone = pTracks->InterpAction & bDone;
                FEKeyInterpFast(pTracks, CurTime,
                                pData + pTracks->LongOffset * 4);
            }
        }
        if (bDone) {
            pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFEFFFFFF;
        } else {
            pObj->Flags |= 0x1000000;
        }
    } else {
        if (pTracks && pTracks->LongOffset == 0) {
            FEKeyInterp(pTracks, CurTime, pData);
        }
        if (*reinterpret_cast<int*>(pObj->pData + 0xC)) {
            unsigned char TrackCount = static_cast<unsigned char>(pScript->TrackCount);
            for (unsigned char i = 0; i < TrackCount; i++, pTracks++) {
                FEKeyInterp(pTracks, CurTime,
                            pData + pTracks->LongOffset * 4);
            }
        }
    }

    unsigned long Flags = pObj->Flags;
    if (Flags & 0x1C00000) {
        pObj->Flags = Flags | 0x2000000;
    }
}

void FEPackage::IssueScriptMessages(FEngine* pEngine, FEObject* pObj,
                                    FEScript* pScript, long tOldTime, long tNewTime) {
    FEEvent* pEvents = pScript->Events.pEvent;
    int Count = pScript->Events.Count;

    if (tNewTime < tOldTime) {
        return;
    }

    if (tNewTime == pScript->Length) {
        tNewTime++;
    }

    int i = 0;
    while (i < Count) {
        if (pEvents[i].tTime >= static_cast<unsigned long>(tOldTime)) {
            break;
        }
        i++;
    }

    if (i < Count && pEvents[i].tTime < static_cast<unsigned long>(tNewTime)) {
        for (;;) {
            switch (pEvents[i].Target) {
            case 0:
                if (pEvents[i].EventID == 0x1B3909AA) {
                    FEObject* pButton = FindObjectByGUID(0);
                    SetCurrentButton(pButton, true);
                } else {
                    pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                          reinterpret_cast<FEObject*>(0), 0);
                }
                break;
            case 0xFFFFFFFF:
                pEngine->SendMessageToGame(pEvents[i].EventID, pObj, this, 0);
                break;
            case 0xFFFFFFFC:
                pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                      reinterpret_cast<FEObject*>(0xFFFFFFFC), 0);
                break;
            case 0xFFFFFFFB:
                pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                      reinterpret_cast<FEObject*>(0xFFFFFFFB), 0);
                break;
            case 0xFFFFFFFA:
                pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                      reinterpret_cast<FEObject*>(0xFFFFFFFA), 0);
                break;
            default: {
                FEObject* pTarget = FindObjectByGUID(pEvents[i].Target);
                if (pEvents[i].EventID == 0x1B3909AA) {
                    FEObject* pButton = FindObjectByGUID(pEvents[i].Target);
                    SetCurrentButton(pButton, true);
                    break;
                }
                if (pObj) {
                    pEngine->QueueMessage(pEvents[i].EventID, pObj, this, pTarget, 0);
                }
                break;
            }
            }
            i++;
            if (i >= Count) {
                return;
            }
            if (pEvents[i].tTime >= static_cast<unsigned long>(tNewTime)) {
                return;
            }
        }
    }
}

void FEPackage::UpdateObject(FEObject* pObj, long tDeltaTicks) {
    if (eFrameCounterOLD == eFrameCounter) {
        objCount++;
    } else {
        objCount = 0;
        eFrameCounterOLD = eFrameCounter;
    }

    unsigned long Flags = pObj->Flags;
    if (Flags & 0x1C00000) {
        pObj->Flags = Flags | 0x2000000;
    } else {
        pObj->Flags = Flags & (FEPackage::uHoldDirtyFlags | 0xFDFFFFFF);
    }

    FEScript* pScript = pObj->pCurrentScript;
    int tPrevTime = pScript->CurTime;
    int ScrLength = pScript->Length;
    pScript->CurTime = tPrevTime + iTickIncrement;
    if (pScript->CurTime < 0) {
        pScript->CurTime = 0;
    }

    unsigned long PlayAction;
    if (pScript->CurTime >= ScrLength) {
        if (bExecuting) {
            if (pScript->pChainTo) {
                UpdateObjectTracks(pObj, pScript);
                int tOverTime = pScript->CurTime - ScrLength;
                pScript->CurTime = 0;
                if (pScript->Events.Count) {
                    IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                }
                pScript = pScript->pChainTo;
                pObj->SetCurrentScript(pScript);
                pScript->CurTime = tOverTime;
                if (pScript->Events.Count) {
                    goto issueFrom0;
                }
            } else {
                PlayAction = pScript->Flags & 3;
                switch (PlayAction) {
                case 0:
                    if (pScript->Events.Count) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                    }
                    pScript->CurTime = pScript->Length + 1;
                    break;
                case 1:
                    if (pScript->Length > 0) {
                        if (pScript->Events.Count) {
                            IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                        }
                        pScript->CurTime = pScript->CurTime -
                            (pScript->CurTime / pScript->Length) * pScript->Length;
                        if (pScript->Events.Count) {
                            IssueScriptMessages(pEnginePtr, pObj, pScript, 0, pScript->CurTime);
                        }
                        pObj->SetupMoveToTracks();
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
            if (bExecuting && tPrevTime == pScript->CurTime &&
                tPrevTime == pScript->Length + 1 && !(pObj->Flags & 0x400000)) {
                goto finalize;
            }
        }
    } else {
        if (bExecuting) {
            if (pScript->Events.Count != 0) {
                PlayAction = pScript->Flags & 3;
                switch (PlayAction) {
                case 0:
                    IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, pScript->CurTime);
                    break;
                case 1:
                    if (pScript->CurTime < tPrevTime) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                    issueFrom0:
                        IssueScriptMessages(pEnginePtr, pObj, pScript, 0, pScript->CurTime);
                        break;
                    }
                    IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, pScript->CurTime);
                    break;
                case 2:
                    if (tPrevTime < ScrLength) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, pScript->CurTime);
                    } else {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime - ScrLength, 0);
                        IssueScriptMessages(pEnginePtr, pObj, pScript, 0, pScript->CurTime);
                    }
                    break;
                }
            }
            if (bExecuting && tPrevTime == pScript->CurTime &&
                tPrevTime == pScript->Length + 1 && !(pObj->Flags & 0x400000)) {
                goto finalize;
            }
        }
    }

    UpdateObjectTracks(pObj, pScript);

finalize:
    switch (pObj->Type) {
    case FE_Group:
        UpdateGroup(static_cast<FEGroup*>(pObj), tDeltaTicks);
        break;
    case FE_List:
        static_cast<FEListBox*>(pObj)->Update(static_cast<float>(tDeltaTicks));
        break;
    case FE_CodeList:
        static_cast<FECodeListBox*>(pObj)->Update(static_cast<float>(tDeltaTicks));
        break;
    case FE_Movie:
        if (bExecuting) {
            static_cast<FEMovie*>(pObj)->Update(tDeltaTicks);
        }
        break;
    }

    if (bExecuting == true && tPrevTime == pScript->CurTime &&
        tPrevTime == pScript->Length + 1 && !(pObj->Flags & 0x400000)) {
        pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFE7FFFFF;
    }

    Flags = pObj->Flags & (FEPackage::uHoldDirtyFlags | 0xFFBFFFFF);
    pObj->Flags = Flags;
    if (Flags & 0x1C00000) {
        pObj->Flags = Flags | 0x2000000;
    }
}

void FEPackage::Update(FEngine* pEngine, long tDeltaTicks) {
    FEObject* pObject = static_cast<FEObject*>(Objects.GetHead());
    pEnginePtr = pEngine;
    iTickIncrement = tDeltaTicks;
    while (pObject) {
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

void FEPackage::SetFilename(const char* pName) {
    if (pFilename) {
        delete[] pFilename;
    }
    pFilename = nullptr;
    if (pName) {
        int Len = FEngStrLen(pName);

        pFilename = FENG_NEW char[Len + 1];
        FEngStrCpy(pFilename, pName);
    }
}

bool FEPackage::Startup(FEGameInterface* pGameInterface) {
    bool bResult = true;
    if (!pGameInterface->LoadResources(this, NumRequests, pRequests)) {
        bResult = false;
    }
    ConnectObjectResources();
    BuildMouseObjectStateList();
    return bResult;
}

void FEPackage::Shutdown(FEGameInterface* pGameInterface) {
    if (pGameInterface) {
        pGameInterface->UnloadResources(this, NumRequests, pRequests);
    }
}

FEMessageResponse* FEPackage::FindResponse(unsigned long MsgID) {
    FEMessageResponse* pNode = GetFirstResponse();
    while (pNode) {
        if (pNode->GetMsgID() == MsgID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}

bool ResourceConnector::Callback(FEObject* pObj) {
    if (pObj->Type == FE_List)
        goto connect;
    if (pObj->Type < FE_List)
        goto check_resource;
    if (pObj->Type > FE_CodeList)
        goto check_resource;
    goto done;
connect:
    ConnectListBoxResources(static_cast<FEListBox*>(pObj));
    goto done;
check_resource:
    if (pObj->ResourceIndex != 0xFFFF) {
        unsigned long idx = static_cast<unsigned long>(pObj->ResourceIndex);
        pObj->UserParam = (*pReqList)[idx].UserParam;
        pObj->Handle = (*pReqList)[idx].Handle;
    }
done:
    return true;
}

void ResourceConnector::ConnectListBoxResources(FEListBox* pList) {
    {
        unsigned long* pCurrentColumn = &pList->mulCurrentColumn;
        unsigned long col;
        if (pList->mulNumColumns == 0) {
            col = 0;
        } else {
            col = 0;
            if (col >= pList->mulNumColumns) {
                col = pList->mulNumColumns - 1;
            }
        }
        *pCurrentColumn = col;
    }
    {
        unsigned long* pCurrentRow = &pList->mulCurrentRow;
        unsigned long row;
        if (pList->mulNumRows == 0) {
            row = 0;
        } else {
            row = 0;
            if (row >= pList->mulNumRows) {
                row = pList->mulNumRows - 1;
            }
        }
        *pCurrentRow = row;
    }
    unsigned long Rows = pList->GetNumRows();
    unsigned long Cols = pList->GetNumColumns();
    for (unsigned long j = 0; j < Rows; j++) {
        unsigned long i = 0;
        while (i < Cols) {
            const FEListBoxCell* pCellData = pList->GetCurrentCellData();
            unsigned long resIdx = pCellData->stResource.ResourceIndex;
            if (resIdx != 0xFFFFFFFF) {
                FEResourceRequest* pReq = &(*pReqList)[resIdx];
                unsigned long handle = pReq->Handle;
                unsigned long userParam = pReq->UserParam;
                FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
                pCell->stResource.Handle = handle;
                pCell->stResource.UserParam = userParam;
                pCell->stResource.ResourceIndex = resIdx;
            } else {
                FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
                pCell->stResource.Handle = 0;
                pCell->stResource.UserParam = 0;
                pCell->stResource.ResourceIndex = 0xFFFFFFFF;
            }
            i++;
            pList->IncrementCellByColumn();
        }
    }
}

void FEPackage::ConnectObjectResources() {
    ResourceConnector resConnector;
    resConnector.pPack = this;
    resConnector.pReqList = &pRequests;
    ForAllObjects(resConnector);
}

void FEPackage::SetNumLibraryRefs(unsigned long NewCount) {
    if (NewCount == 0) {
        if (pLibRefs) {
            delete[] reinterpret_cast<char*>(pLibRefs);
        }
        pLibRefs = nullptr;
    } else {
        FELibraryRef* pNewList = FENG_NEW FELibraryRef[NewCount];
        unsigned long CopyCount = NewCount;
        if (NewCount > NumLibRefs) {
            CopyCount = NumLibRefs;
        }
        if (CopyCount != 0) {
            FEngMemCpy(pNewList, pLibRefs, CopyCount * sizeof(FELibraryRef));
        }
        if (pLibRefs) {
            delete[] reinterpret_cast<char*>(pLibRefs);
        }
        pLibRefs = pNewList;
        NumLibRefs = NewCount;
    }
}

FELibraryRef* FEPackage::FindLibraryReference(unsigned long ObjGUID) const {
    for (unsigned long i = 0; i < NumLibRefs; i++) {
        if (pLibRefs[i].ObjGUID == ObjGUID) {
            return &pLibRefs[i];
        }
    }
    return nullptr;
}

void FEPackage::BuildMouseObjectStateList() {
    if (MouseObjectStates) {
        delete[] MouseObjectStates;
        MouseObjectStates = nullptr;
        NumMouseObjects = 0;
    }
    MouseStateObjectCounter the_counter;
    the_counter.NumMouseObjects = 0;
    ForAllObjects(the_counter);
    if (the_counter.NumMouseObjects > 0) {
        MouseObjectStates = new FEObjectMouseState[the_counter.NumMouseObjects];
        MouseStateArrayBuilder the_builder;
        the_builder.pPack = this;
        ForAllObjects(the_builder);
    }
}

FEObjectMouseState::FEObjectMouseState() {
    pObject = nullptr;
    Offset.h = 0.0f;
    Offset.v = 0.0f;
    Flags = 0;
}

FEObjectMouseState::~FEObjectMouseState() {}

FEMsgTargetList* FEPackage::GetMessageTargets(unsigned long MsgID) {
    for (unsigned long i = 0; i < NumMsgTargets; i++) {
        if (pMsgTargets[i].GetMsgID() == MsgID) {
            return &pMsgTargets[i];
        }
    }
    return nullptr;
}

bool OffsetCalculatron(unsigned long NameHash, FEObject* pObj, FEPoint& Offset) {
    if (NameHash == pObj->NameHash) {
        FEObjData* pData = pObj->GetObjData();
        Offset.h += pData->Pos.x;
        Offset.v += pData->Pos.y;
        return true;
    } else if (pObj->Type == FE_Group && static_cast<FEGroup*>(pObj)->FindChildRecursive(NameHash)) {
        FEObjData* pData = pObj->GetObjData();
        Offset.h += pData->Pos.x;
        Offset.v += pData->Pos.y;
        FEObject* pChild = static_cast<FEGroup*>(pObj)->GetFirstChild();
        while (pChild) {
            OffsetCalculatron(NameHash, pChild, Offset);
            pChild = static_cast<FEObject*>(pChild->GetNext());
        }
        return true;
    }
    return false;
}

void FEPackage::AddMouseObjectState(FEObject* obj) {
    if (!obj) {
        return;
    }
    FEObject* pObj = GetFirstObject();
    unsigned long mouseable = obj->NameHash;
    while (pObj) {
        if (pObj->Type == FE_Group) {
            if (static_cast<FEGroup*>(pObj)->FindChildRecursive(mouseable) || mouseable == pObj->NameHash) {
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
        pObj = static_cast<FEObject*>(pObj->GetNext());
    }
    MouseObjectStates[NumMouseObjects].pObject = obj;
    NumMouseObjects++;
}

void FEPackage::UpdateMouseObjectOffsets(FEObject* obj) {
    if (!obj) {
        return;
    }
    FEObject* pObj = GetFirstObject();
    unsigned long mouseable = obj->NameHash;
    while (pObj) {
        if (pObj->Type == FE_Group) {
            if (static_cast<FEGroup*>(pObj)->FindChildRecursive(mouseable) || mouseable == pObj->NameHash) {
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
        pObj = static_cast<FEObject*>(pObj->GetNext());
    }
}
