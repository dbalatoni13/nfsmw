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

u32 FEPackage::uHoldDirtyFlags = 0xFFFFFFFF; // size: 0x4, address: 0x8041D158, Decl: speed/indep/src/feng/FEPackage.cpp:26

bool PackageInitStateCB::Callback(FEObject *pObj) {
    pObj->SetCurrentScript(pObj->FindScript(0x1744b3));
    pObj->pCurrentScript->CurTime = 0;
    pObj->Flags |= FEngDirtyFlagsMask;
    return true;
}

FEPackage::FEPackage()
    : bExecuting(false), bUseIdleList(false), bIsLibrary(false), bStartEqualsAccept(false), bErrorScreen(false), Priority(0), Controllers(0xff),
      OldControllers(0xff), bInputEnabled(false), pFilename(nullptr), pParentPackage(nullptr), UserParam(0), NumRequests(0), pRequests(nullptr),
      NumMsgTargets(0), pMsgTargets(nullptr), NumLibRefs(0), pLibRefs(nullptr), pCurrentButton(nullptr), pResourceNames(nullptr),
      MouseObjectStates(nullptr), NumMouseObjects(0) {}

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
    FEObjectComment *pComment;
    while ((pComment = static_cast<FEObjectComment *>(Comments.RemHead())) != nullptr) {
        if (pComment->pStr) {
            delete[] pComment->pStr;
        }
        delete pComment;
    }
    if (pLibRefs) {
        delete[] pLibRefs;
    }
}

void FEPackage::SetFilename(const char *pName) {
    if (pFilename) {
        delete[] pFilename;
    }
    pFilename = nullptr;
    if (pName) {
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
    if (pGameInterface) {
        pGameInterface->UnloadResources(this, NumRequests, pRequests);
    }
}

bool FEPackage::InitializePackage() {
    PackageInitStateCB cb;
    return ForAllObjects(cb);
}

FEMessageResponse *FEPackage::FindResponse(u32 MsgID) {
    FEMessageResponse *pNode = GetFirstResponse();
    while (pNode) {
        if (pNode->GetMsgID() == MsgID) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return pNode;
}

bool FEPackage::ForAllChildren(FEGroup *pGroup, FEObjectCallback &Callback) {
    FEObject *pObj = pGroup->GetFirstChild();
    while (pObj) {
        if (!Callback.Callback(pObj)) {
            return false;
        }
        if (pObj->Type == 5 && !ForAllChildren(static_cast<FEGroup *>(pObj), Callback)) {
            return false;
        }
        pObj = static_cast<FEObject *>(pObj->GetNext());
    }
    return true;
}

bool FEPackage::ForAllObjects(FEObjectCallback &Callback) {
    FEObject *pObj = static_cast<FEObject *>(Objects.GetHead());
    while (pObj) {
        if (!Callback.Callback(pObj)) {
            return false;
        }
        if (pObj->Type == 5 && !ForAllChildren(static_cast<FEGroup *>(pObj), Callback)) {
            return false;
        }
        pObj = static_cast<FEObject *>(pObj->GetNext());
    }
    return true;
}

// File: speed/indep/src/feng/FEPackage.cpp
// total size: 0xC
// Decl: speed/indep/src/feng/FEPackage.cpp:211
class FEFindByHash : public FEObjectCallback {
  public:
    u32 Hash;         // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:213
    FEObject *pFound; // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:214

    bool Callback(FEObject *pObj) override { // Decl: speed/indep/src/feng/FEPackage.cpp:216
        if (pObj->NameHash == Hash) {
            pFound = pObj;
            return false;
        }
        return true;
    }
};

FEObject *FEPackage::FindObjectByName(const char *pName) {}

FEObject *FEPackage::FindObjectByHash(u32 NameHash) {
    FEFindByHash finder;
    finder.Hash = NameHash;
    finder.pFound = nullptr;
    ForAllObjects(finder);
    return finder.pFound;
}

// total size: 0xC
// Decl: speed/indep/src/feng/FEPackage.cpp:257
class FEFindByGUID : public FEObjectCallback {
  public:
    u32 GUID;         // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:259
    FEObject *pFound; // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:260

    bool Callback(FEObject *pObj) override { // Decl: speed/indep/src/feng/FEPackage.cpp:262
        if (pObj->GUID == GUID) {
            pFound = pObj;
            return false;
        }
        return true;
    }
};

FEObject *FEPackage::FindObjectByGUID(u32 GUID) {
    FEFindByGUID finder;
    finder.GUID = GUID;
    finder.pFound = nullptr;
    ForAllObjects(finder);
    return finder.pFound;
}

void FEPackage::IssueScriptMessages(FEngine *pEngine, FEObject *pObj, FEScript *pScript, i32 tOldTime, i32 tNewTime) {
    FEEvent *pEvents = &pScript->Events[0];
    int Count = pScript->Events.GetCount();

    if (tNewTime < tOldTime) {
        return;
    }

    if (tNewTime == pScript->Length) {
        tNewTime++;
    }

    int i = 0;
    while (i < Count) {
        if (pEvents[i].tTime >= static_cast<u32>(tOldTime)) {
            break;
        }
        i++;
    }

    if (i < Count && pEvents[i].tTime < static_cast<u32>(tNewTime)) {
        for (;;) {
            switch (pEvents[i].Target) {
                case 0:
                    if (pEvents[i].EventID == 0x1B3909AA) {
                        FEObject *pButton = FindObjectByGUID(0);
                        SetCurrentButton(pButton, true);
                    } else {
                        pEngine->QueueMessage(pEvents[i].EventID, pObj, this, reinterpret_cast<FEObject *>(0), 0);
                    }
                    break;
                case 0xFFFFFFFF:
                    pEngine->SendMessageToGame(pEvents[i].EventID, pObj, this, 0);
                    break;
                case 0xFFFFFFFC:
                    pEngine->QueueMessage(pEvents[i].EventID, pObj, this, reinterpret_cast<FEObject *>(0xFFFFFFFC), 0);
                    break;
                case 0xFFFFFFFB:
                    pEngine->QueueMessage(pEvents[i].EventID, pObj, this, reinterpret_cast<FEObject *>(0xFFFFFFFB), 0);
                    break;
                case 0xFFFFFFFA:
                    pEngine->QueueMessage(pEvents[i].EventID, pObj, this, reinterpret_cast<FEObject *>(0xFFFFFFFA), 0);
                    break;
                default: {
                    FEObject *pTargetPtr = FindObjectByGUID(pEvents[i].Target);
                    if (pEvents[i].EventID == 0x1B3909AA) {
                        FEObject *pTargetPtr = FindObjectByGUID(pEvents[i].Target);
                        SetCurrentButton(pTargetPtr, true);
                        break;
                    }
                    if (pObj) {
                        pEngine->QueueMessage(pEvents[i].EventID, pObj, this, pTargetPtr, 0);
                    }
                    break;
                }
            }
            i++;
            if (i >= Count) {
                return;
            }
            if (pEvents[i].tTime >= static_cast<u32>(tNewTime)) {
                return;
            }
        }
    }
}

void FEPackage::UpdateGroup(FEGroup *pGroup, long tDeltaTicks) {
    FEObject *pChild = pGroup->GetFirstChild();
    while (pChild) {
        UpdateObject(pChild, tDeltaTicks);
        pChild->Flags |= pGroup->Flags & FEngDirtyFlagsMask;
        pChild = static_cast<FEObject *>(pChild->next);
    }
}

uint32 eFrameCounter = 0; // size: 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:400
uint32 eFrameCounterOLD;  // size: 0x4, address: 0x80473E0C, Decl: speed/indep/src/feng/FEPackage.cpp:401
uint32 objCount;          // size: 0x4, address: 0x80473E10, Decl: speed/indep/src/feng/FEPackage.cpp:402

void FEPackage::UpdateObject(FEObject *pObj, long tDeltaTicks) {
    if (eFrameCounterOLD == eFrameCounter) {
        objCount++;
    } else {
        objCount = 0;
        eFrameCounterOLD = eFrameCounter;
    }

    unsigned long Flags = pObj->Flags;
    if (Flags & FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform) {
        pObj->Flags = Flags | FF_Dirty;
    } else {
        pObj->Flags = Flags & (FEPackage::uHoldDirtyFlags | ~FF_Dirty);
    }

    FEScript *pScript = pObj->pCurrentScript;
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
                if (pScript->Events.GetCount()) {
                    IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                }
                pScript = pScript->pChainTo;
                pObj->SetCurrentScript(pScript);
                pScript->CurTime = tOverTime;
                if (pScript->Events.GetCount()) {
                    goto issueFrom0;
                }
            } else {
                PlayAction = pScript->Flags & 3;
                switch (PlayAction) {
                    case 0:
                        if (pScript->Events.GetCount()) {
                            IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                        }
                        pScript->CurTime = pScript->Length + 1;
                        break;
                    case 1:
                        if (pScript->Length > 0) {
                            if (pScript->Events.GetCount()) {
                                IssueScriptMessages(pEnginePtr, pObj, pScript, tPrevTime, ScrLength);
                            }
                            pScript->CurTime = pScript->CurTime - (pScript->CurTime / pScript->Length) * pScript->Length;
                            if (pScript->Events.GetCount()) {
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
            if (bExecuting && tPrevTime == pScript->CurTime && tPrevTime == pScript->Length + 1 && !(pObj->Flags & FF_DirtyCode)) {
                goto finalize;
            }
        }
    } else {
        if (bExecuting) {
            if (pScript->Events.GetCount() != 0) {
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
            if (bExecuting && tPrevTime == pScript->CurTime && tPrevTime == pScript->Length + 1 && !(pObj->Flags & FF_DirtyCode)) {
                goto finalize;
            }
        }
    }

    UpdateObjectTracks(pObj, pScript);

finalize:
    switch (pObj->Type) {
        case FE_Group:
            UpdateGroup(static_cast<FEGroup *>(pObj), tDeltaTicks);
            break;
        case FE_List:
            static_cast<FEListBox *>(pObj)->Update(static_cast<float>(tDeltaTicks));
            break;
        case FE_CodeList:
            static_cast<FECodeListBox *>(pObj)->Update(static_cast<float>(tDeltaTicks));
            break;
        case FE_Movie:
            if (bExecuting) {
                static_cast<FEMovie *>(pObj)->Update(tDeltaTicks);
            }
            break;
    }

    if (bExecuting == true && tPrevTime == pScript->CurTime && tPrevTime == pScript->Length + 1 && !(pObj->Flags & FF_DirtyCode)) {
        pObj->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyTransform;
    }

    Flags = pObj->Flags & (FEPackage::uHoldDirtyFlags | ~FF_DirtyCode);
    pObj->Flags = Flags;
    if (Flags & FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform) {
        pObj->Flags = Flags | FF_Dirty;
    }
}

void FEPackage::UpdateObjectTracks(FEObject *pObj, FEScript *pScript) {
    unsigned char *pData = pObj->pData;
    int CurTime = pScript->CurTime;
    FEKeyTrack *pTracks = pScript->pTracks;

    if (bExecuting) {
        if (pTracks && pTracks->LongOffset == 0) {
            if (pTracks->InterpAction & 0x80) {
                pObj->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyColor;
            } else {
                pObj->Flags |= FF_DirtyColor;
            }
            FEKeyInterpFast(pTracks, CurTime, pData);
        } else {
            pObj->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyColor;
        }
        unsigned char bDone = 0x80;
        if (*reinterpret_cast<int *>(pObj->pData + 0xC)) {
            unsigned char TrackCount = static_cast<unsigned char>(pScript->TrackCount);
            for (unsigned char i = 0; i < TrackCount; i++, pTracks++) {
                bDone = pTracks->InterpAction & bDone;
                FEKeyInterpFast(pTracks, CurTime, pData + pTracks->LongOffset * 4);
            }
        }
        if (bDone) {
            pObj->Flags &= FEPackage::uHoldDirtyFlags | ~FF_DirtyTransform;
        } else {
            pObj->Flags |= FF_DirtyTransform;
        }
    } else {
        if (pTracks && pTracks->LongOffset == 0) {
            FEKeyInterp(pTracks, CurTime, pData);
        }
        if (*reinterpret_cast<int *>(pObj->pData + 0xC)) {
            unsigned char TrackCount = static_cast<unsigned char>(pScript->TrackCount);
            for (unsigned char i = 0; i < TrackCount; i++, pTracks++) {
                FEKeyInterp(pTracks, CurTime, pData + pTracks->LongOffset * 4);
            }
        }
    }

    unsigned long Flags = pObj->Flags;
    if (Flags & FF_DirtyCode | FF_DirtyColor | FF_DirtyTransform) {
        pObj->Flags = Flags | FF_Dirty;
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

// Decl: speed/indep/src/feng/FEPackage.cpp:765
void FEPackage::Update(FEngine *pEngine, long tDeltaTicks) {
    FEObject *pObject = static_cast<FEObject *>(Objects.GetHead());
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

void FEPackage::SetCurrentButton(FEObject *pNewButton, bool bSendMsgs) {
    if (bSendMsgs) {
        if (pCurrentButton) {
            pEnginePtr->QueueMessage(0x55d1e635, nullptr, this, pCurrentButton, 0);
            pEnginePtr->QueueMessage(0x55d1e635, pCurrentButton, this, reinterpret_cast<FEObject *>(0xfffffffb), 0);
        }
        if (pNewButton) {
            pEnginePtr->QueueMessage(0xabc08912, nullptr, this, pNewButton, 0);
            pEnginePtr->QueueMessage(0xabc08912, pNewButton, this, reinterpret_cast<FEObject *>(0xfffffffb), 0);
        }
    }
    pCurrentButton = pNewButton;
}

// total size: 0xC
// Decl: speed/indep/src/feng/FEPackage.cpp:888
class FEGetNumSpawnResponses : public FEObjectCallback {
  public:
    ~FEGetNumSpawnResponses() override {} // Decl: speed/indep/src/feng/FEPackage.cpp:888

    u32 *pNumLoad;      // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:890
    u32 *pStringLength; // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:891

    FEGetNumSpawnResponses(u32 &TheNumLoad, u32 &TheStringLength) {} // Decl: speed/indep/src/feng/FEPackage.cpp:893

    bool Callback(FEObject *pObj) override {} // Decl: speed/indep/src/feng/FEPackage.cpp:896
};

// total size: 0x10
// Decl: speed/indep/src/feng/FEPackage.cpp:904
class FESetSpawnResponses : public FEObjectCallback {
  public:
    ~FESetSpawnResponses() override {} // Decl: speed/indep/src/feng/FEPackage.cpp:904

    u32 *pNumLoad;                 // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:906
    u32 *pStringLength;            // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:907
    FELoadPackageDescArray *pDesc; // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:908

    FESetSpawnResponses(u32 &TheNumLoad, u32 &TheStringLength, FELoadPackageDescArray *pTheDesc) {} // Decl: speed/indep/src/feng/FEPackage.cpp:910

    bool Callback(FEObject *pObj) override {} // Decl: speed/indep/src/feng/FEPackage.cpp:913
};

// total size: 0xC
// Decl: speed/indep/src/feng/FEPackage.cpp:968
class ResourceConnector : public FEObjectCallback {
  public:
    FEPackage *pPack;             // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:970
    FEResourceRequest **pReqList; // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:971

    bool Callback(FEObject *pObj) override; // Decl: speed/indep/src/feng/FEPackage.cpp:978

    void ConnectListBoxResources(FEListBox *pList); // Decl: speed/indep/src/feng/FEPackage.cpp:1006
};

bool ResourceConnector::Callback(FEObject *pObj) override { // Decl: speed/indep/src/feng/FEPackage.cpp:978
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

void ResourceConnector::ConnectListBoxResources(FEListBox *pList) { // Decl: speed/indep/src/feng/FEPackage.cpp:1006
    pList->SetCurrentColumn(0);
    pList->SetCurrentRow(0);

    u32 Rows = pList->GetNumRows();
    u32 Cols = pList->GetNumColumns();
    for (u32 j = 0; j < Rows; j++) {
        for (u32 i = 0; i < Cols; i++) {
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
// Decl: speed/indep/src/feng/FEPackage.cpp:1074
class MouseStateObjectCounter : public FEObjectCallback {
  public:
    int NumMouseObjects; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:1076

    bool Callback(FEObject *pObj) override { // Decl: speed/indep/src/feng/FEPackage.cpp:1078
        if (pObj->Flags & FF_MouseObject) {
            NumMouseObjects++;
        }
        return true;
    }
};

// total size: 0x8
// Decl: speed/indep/src/feng/FEPackage.cpp:1089
class MouseStateArrayBuilder : public FEObjectCallback {
  public:
    FEPackage *pPack; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackage.cpp:1091

    bool Callback(struct FEObject *pObj) override { // Decl: speed/indep/src/feng/FEPackage.cpp:1093
        if (pObj->Flags & FF_MouseObject) {
            pPack->AddMouseObjectState(pObj);
        }
        return true;
    }
};

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

bool OffsetCalculatron(u32 NameHash, FEObject *pObj, FEPoint &Offset) {
    if (NameHash == pObj->NameHash) {
        FEObjData *pData = pObj->GetObjData();
        Offset.h += pData->Pos.x;
        Offset.v += pData->Pos.y;
        return true;
    } else if (pObj->Type == FE_Group && static_cast<FEGroup *>(pObj)->FindChildRecursive(NameHash)) {
        FEObjData *pData = pObj->GetObjData();
        Offset.h += pData->Pos.x;
        Offset.v += pData->Pos.y;
        FEObject *pChild = static_cast<FEGroup *>(pObj)->GetFirstChild();
        while (pChild) {
            OffsetCalculatron(NameHash, pChild, Offset);
            pChild = static_cast<FEObject *>(pChild->GetNext());
        }
        return true;
    }
    return false;
}

void FEPackage::AddMouseObjectState(FEObject *obj) {
    if (!obj) {
        return;
    }
    FEObject *pObj = GetFirstObject();
    u32 mouseable = obj->NameHash;
    while (pObj) {
        if (pObj->Type == FE_Group) {
            if (static_cast<FEGroup *>(pObj)->FindChildRecursive(mouseable) || mouseable == pObj->NameHash) {
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
    if (!obj) {
        return;
    }
    FEObject *pObj = GetFirstObject();
    u32 mouseable = obj->NameHash;
    while (pObj) {
        if (pObj->Type == FE_Group) {
            if (static_cast<FEGroup *>(pObj)->FindChildRecursive(mouseable) || mouseable == pObj->NameHash) {
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
        if (pLibRefs) {
            delete[] reinterpret_cast<char *>(pLibRefs);
        }
        pLibRefs = nullptr;
    } else {
        FELibraryRef *pNewList = FNEW FELibraryRef[NewCount];
        u32 CopyCount = NewCount;
        if (NewCount > NumLibRefs) {
            CopyCount = NumLibRefs;
        }
        if (CopyCount != 0) {
            FEngMemCpy(pNewList, pLibRefs, CopyCount * sizeof(FELibraryRef));
        }
        if (pLibRefs) {
            delete[] reinterpret_cast<char *>(pLibRefs);
        }
        pLibRefs = pNewList;
        NumLibRefs = NewCount;
    }
}

FELibraryRef *FEPackage::FindLibraryReference(u32 ObjGUID) const {
    for (u32 i = 0; i < NumLibRefs; i++) {
        if (pLibRefs[i].ObjGUID == ObjGUID) {
            return &pLibRefs[i];
        }
    }
    return nullptr;
}
