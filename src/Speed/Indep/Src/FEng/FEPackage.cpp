#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/fengine.h"
#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"

// Forward declarations for types only needed locally as pointer members.
// Their struct definitions come from FEngine.cpp earlier in the jumbo build.

// total size: 0xC
struct FELibraryRef {
    unsigned long ObjGUID;       // offset 0x0, size 0x4
    unsigned long PackNameHash;  // offset 0x4, size 0x4
    unsigned long LibGUID;       // offset 0x8, size 0x4
};

// total size: 0x10
struct FEMsgTargetList {
    unsigned long MsgID;       // offset 0x0, size 0x4
    unsigned long Alloc;       // offset 0x4, size 0x4
    unsigned long Count;       // offset 0x8, size 0x4
    FEObject** pTargets;       // offset 0xC, size 0x4

    inline FEMsgTargetList() : MsgID(0), Alloc(0), Count(0), pTargets(nullptr) {}
    inline ~FEMsgTargetList() {}
    inline void SetMsgID(unsigned long NewID) { MsgID = NewID; }
    inline unsigned long GetMsgID() const { return MsgID; }
    inline unsigned long GetCount() const { return Count; }
    inline FEObject* GetTarget(unsigned long Index) { return pTargets[Index]; }
    inline const FEObject* GetTarget(unsigned long Index) const { return pTargets[Index]; }

    void Allocate(unsigned long NewAlloc);
    void AppendTarget(FEObject* pObject);
};

// total size: 0x10
struct FEObjectMouseState {
    FEObject* pObject;   // offset 0x0, size 0x4
    FEPoint Offset;      // offset 0x4, size 0x8
    unsigned long Flags;  // offset 0xC, size 0x4

    FEObjectMouseState();
    ~FEObjectMouseState();

    inline bool GetBit(unsigned long bit) { return (Flags & bit) != 0; }
    inline void SetBit(unsigned long bit, bool state) {
        if (state) Flags |= bit;
        else Flags &= ~bit;
    }
};

// total size: 0x18
struct FEResourceRequest {
    unsigned long ID;             // offset 0x0, size 0x4
    const char* pFilename;        // offset 0x4, size 0x4
    unsigned long Type;           // offset 0x8, size 0x4
    unsigned long Flags;          // offset 0xC, size 0x4
    unsigned long Handle;         // offset 0x10, size 0x4
    unsigned long UserParam;      // offset 0x14, size 0x4
};

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
    , NumMouseObjects(0) //
    , NumMouseObjectsCounter(0) //
{
    VersionNumber = 0;
    pEnginePtr = nullptr;
    iTickIncrement = 0;
}

FEPackage::~FEPackage() {
    if (pFilename) {
        delete[] pFilename;
    }
    if (pRequests) {
        delete[] pRequests;
    }
    if (pMsgTargets) {
        FEMsgTargetList* pEnd = pMsgTargets + NumMsgTargets;
        FEMsgTargetList* p = pEnd;
        if (pMsgTargets != pEnd) {
            do {
                p--;
                if (p->pTargets) {
                    delete[] p->pTargets;
                }
            } while (pMsgTargets != p);
        }
        delete[] pMsgTargets;
    }
    if (pResourceNames) {
        delete[] pResourceNames;
    }
    if (MouseObjectStates) {
        FEObjectMouseState* pEnd = MouseObjectStates + NumMouseObjects;
        FEObjectMouseState* p = pEnd;
        if (MouseObjectStates != pEnd) {
            do {
                p--;
                p->~FEObjectMouseState();
            } while (MouseObjectStates != p);
        }
        delete[] reinterpret_cast<char*>(MouseObjectStates);
    }
    FEMinNode* node;
    while ((node = Comments.RemHead()) != nullptr) {
        // TODO: Comments node has a string field
        delete node;
    }
    if (pLibRefs) {
        delete[] pLibRefs;
    }
    if (ButtonMap.pList) {
        delete[] ButtonMap.pList;
    }
}

bool FEPackage::InitializePackage() {
    PackageInitStateCB cb;
    ForAllObjects(cb);
    return true;
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
    FEScript* pScript = pObj->FindScript(0x1744b3);
    pObj->SetCurrentScript(pScript);
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
    unsigned long Flags;

    if (!bExecuting) {
        if (pTracks && pTracks[0].LongOffset == 0) {
            FEKeyInterp(pTracks, CurTime, pData);
        }
        if (!*reinterpret_cast<int*>(pObj->pData + 0xC)) {
            Flags = pObj->Flags;
            goto setDirty;
        }
        {
            unsigned char TrackCount = static_cast<unsigned char>(pScript->TrackCount);
            for (unsigned char i = 0; i < TrackCount; i++) {
                FEKeyInterp(&pTracks[i], CurTime,
                            pData + pTracks[i].LongOffset * 4);
            }
        }
    } else {
        if (!pTracks || pTracks[0].LongOffset != 0) {
            pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFF7FFFFF;
        } else {
            if (!(pTracks[0].InterpAction & 0x80)) {
                pObj->Flags |= 0x800000;
            } else {
                pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFF7FFFFF;
            }
            FEKeyInterpFast(pTracks, CurTime, pData);
        }
        unsigned char bDone = 0x80;
        if (*reinterpret_cast<int*>(pObj->pData + 0xC)) {
            unsigned char TrackCount = static_cast<unsigned char>(pScript->TrackCount);
            for (unsigned char i = 0; i < TrackCount; i++) {
                bDone &= pTracks[i].InterpAction;
                FEKeyInterpFast(&pTracks[i], CurTime,
                                pData + pTracks[i].LongOffset * 4);
            }
        }
        if (bDone == 0) {
            pObj->Flags |= 0x1000000;
        } else {
            pObj->Flags &= FEPackage::uHoldDirtyFlags | 0xFEFFFFFF;
        }
    }
    Flags = pObj->Flags;
setDirty:
    if (Flags & 0x1C00000) {
        pObj->Flags = Flags | 0x2000000;
    }
}

void FEPackage::IssueScriptMessages(FEngine* pEngine, FEObject* pObj,
                                    FEScript* pScript, long tOldTime, long tNewTime) {
    int Count = pScript->Events.Count;
    if (Count == 0) {
        return;
    }

    FEEvent* pEvents = pScript->Events.pEvent;
    int i = 0;

    if (tOldTime < tNewTime) {
        while (i < Count) {
            if (pEvents[i].tTime >= static_cast<unsigned long>(tNewTime)) {
                break;
            }
            if (pEvents[i].tTime >= static_cast<unsigned long>(tOldTime)) {
                goto dispatch;
            }
            i++;
        }
        return;
    }

    while (i < Count) {
        if (pEvents[i].tTime < static_cast<unsigned long>(tOldTime)) {
            i++;
        } else {
            break;
        }
    }

    if (i < Count && pEvents[i].tTime < static_cast<unsigned long>(tNewTime)) {
    dispatch:
        unsigned long Target = pEvents[i].Target;
        for (;;) {
            if (Target == 0xFFFFFFFB) {
                pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                      reinterpret_cast<FEObject*>(0xFFFFFFFB), 0);
            } else if (Target < 0xFFFFFFFC) {
                if (Target == 0) {
                    if (pEvents[i].EventID != 0x1B3909AA) {
                        pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                              reinterpret_cast<FEObject*>(0), 0);
                    } else {
                        FEObject* pButton = FindObjectByGUID(0);
                        SetCurrentButton(pButton, true);
                    }
                } else if (Target == 0xFFFFFFFA) {
                    pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                          reinterpret_cast<FEObject*>(0xFFFFFFFA), 0);
                } else {
                    FEObject* pTarget = FindObjectByGUID(Target);
                    if (pEvents[i].EventID != 0x1B3909AA) {
                        if (pTarget) {
                            pEngine->QueueMessage(pEvents[i].EventID, pObj, this, pTarget, 0);
                        }
                        goto next;
                    }
                    FEObject* pButton = FindObjectByGUID(Target);
                    SetCurrentButton(pButton, true);
                }
            } else if (Target == 0xFFFFFFFC) {
                pEngine->QueueMessage(pEvents[i].EventID, pObj, this,
                                      reinterpret_cast<FEObject*>(0xFFFFFFFC), 0);
            } else if (Target == 0xFFFFFFFF) {
                pEngine->SendMessageToGame(pEvents[i].EventID, pObj, this, 0);
            } else {
                FEObject* pTarget = FindObjectByGUID(Target);
                if (pEvents[i].EventID != 0x1B3909AA) {
                    if (pTarget) {
                        pEngine->QueueMessage(pEvents[i].EventID, pObj, this, pTarget, 0);
                    }
                    goto next;
                }
                FEObject* pButton = FindObjectByGUID(Target);
                SetCurrentButton(pButton, true);
            }
        next:
            i++;
            if (i >= Count) {
                return;
            }
            if (pEvents[i].tTime >= static_cast<unsigned long>(tNewTime)) {
                return;
            }
            Target = pEvents[i].Target;
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
    if ((Flags & 0x1C00000) == 0) {
        Flags &= FEPackage::uHoldDirtyFlags | 0xFDFFFFFF;
    } else {
        Flags |= 0x2000000;
    }
    pObj->Flags = Flags;

    FEScript* pScript = pObj->pCurrentScript;
    int OldCurTime = pScript->CurTime;
    int Length = pScript->Length;
    int NewCurTime = OldCurTime + iTickIncrement;
    pScript->CurTime = NewCurTime;
    if (NewCurTime < 0) {
        pScript->CurTime = 0;
    }

    NewCurTime = pScript->CurTime;
    if (NewCurTime < Length) {
        if (bExecuting) {
            if (pScript->Events.Count != 0) {
                unsigned long PlayAction = pScript->Flags & 3;
                if (PlayAction == 1) {
                    if (NewCurTime < OldCurTime) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, Length);
                        NewCurTime = pScript->CurTime;
                        goto issueFrom0;
                    }
                    IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, NewCurTime);
                } else {
                    if (PlayAction == 0) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, NewCurTime);
                    } else if (PlayAction == 2) {
                        if (OldCurTime < Length) {
                            IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, NewCurTime);
                        } else {
                            IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime - Length, 0);
                            IssueScriptMessages(pEnginePtr, pObj, pScript, 0, pScript->CurTime);
                        }
                    }
                }
            }
            if (bExecuting && OldCurTime == pScript->CurTime &&
                OldCurTime == pScript->Length + 1 && !(pObj->Flags & 0x400000)) {
                goto finalize;
            }
        }
    } else if (bExecuting) {
        if (pScript->pChainTo) {
            UpdateObjectTracks(pObj, pScript);
            NewCurTime = pScript->CurTime - Length;
            pScript->CurTime = 0;
            if (pScript->Events.Count) {
                IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, Length);
            }
            pScript = pScript->pChainTo;
            pObj->SetCurrentScript(pScript);
            pScript->CurTime = NewCurTime;
            if (pScript->Events.Count) {
            issueFrom0:
                IssueScriptMessages(pEnginePtr, pObj, pScript, 0, NewCurTime);
            }
        } else {
            unsigned long PlayAction = pScript->Flags & 3;
            if (PlayAction == 1) {
                if (pScript->Length < 1) {
                    pScript->CurTime = 0;
                } else {
                    if (pScript->Events.Count) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, Length);
                    }
                    pScript->CurTime = pScript->CurTime -
                        (pScript->CurTime / pScript->Length) * pScript->Length;
                    if (pScript->Events.Count) {
                        IssueScriptMessages(pEnginePtr, pObj, pScript, 0, pScript->CurTime);
                    }
                    pObj->SetupMoveToTracks();
                }
            } else if (PlayAction == 0) {
                if (pScript->Events.Count) {
                    IssueScriptMessages(pEnginePtr, pObj, pScript, OldCurTime, Length);
                }
                pScript->CurTime = pScript->Length + 1;
            } else if (PlayAction == 2) {
                if (pScript->Length < 1) {
                    pScript->CurTime = 0;
                } else {
                    int doubleLen = pScript->Length * 2;
                    pScript->CurTime = NewCurTime - (NewCurTime / doubleLen) * doubleLen;
                }
            }
        }
        if (bExecuting && OldCurTime == pScript->CurTime &&
            OldCurTime == pScript->Length + 1 && !(pObj->Flags & 0x400000)) {
            goto finalize;
        }
    }

    UpdateObjectTracks(pObj, pScript);

finalize:
    if (pObj->Type == 5) {
        UpdateGroup(static_cast<FEGroup*>(pObj), tDeltaTicks);
    } else if (pObj->Type < 5) {
        if (pObj->Type == 4) {
            static_cast<FEListBox*>(pObj)->Update(static_cast<float>(tDeltaTicks));
        }
    } else if (pObj->Type == 6) {
        static_cast<FECodeListBox*>(pObj)->Update(static_cast<float>(tDeltaTicks));
    } else if (pObj->Type == 7 && bExecuting) {
        *reinterpret_cast<unsigned long*>(pObj->pData + 0x5C - 0x2C) += tDeltaTicks;
    }

    if (bExecuting == true && OldCurTime == pScript->CurTime &&
        OldCurTime == pScript->Length + 1 && !(pObj->Flags & 0x400000)) {
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
