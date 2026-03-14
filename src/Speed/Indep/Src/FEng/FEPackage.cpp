#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/fengine.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

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
