#include "Speed/Indep/Src/FEng/fengine_full.h"
#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEPackageReader.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"

// Callback structs used by both FEngine and FEPackage.
// Defined here because FEngine.cpp comes before FEPackage.cpp in the jumbo build.

// total size: 0x4
struct PackageInitStateCB : public FEObjectCallback {
    bool Callback(FEObject* pObj) override;
};

// total size: 0xC
struct FEFindByHash : public FEObjectCallback {
    unsigned long Hash;      // offset 0x4, size 0x4
    FEObject* pFound;        // offset 0x8, size 0x4

    bool Callback(FEObject* pObj) override;
};

// total size: 0xC
struct FEFindByGUID : public FEObjectCallback {
    unsigned long GUID;      // offset 0x4, size 0x4
    FEObject* pFound;        // offset 0x8, size 0x4

    bool Callback(FEObject* pObj) override;
};

// total size: 0x8
struct MouseStateObjectCounter : public FEObjectCallback {
    int NumMouseObjects;     // offset 0x4, size 0x4

    bool Callback(FEObject* pObj) override;
};

// total size: 0x8
struct MouseStateArrayBuilder : public FEObjectCallback {
    FEPackage* pPack;        // offset 0x4, size 0x4

    bool Callback(FEObject* pObj) override;
};

// total size: 0x8
struct MouseStateArrayOffsetUpdater : public FEObjectCallback {
    FEPackage* pPack;        // offset 0x4, size 0x4

    bool Callback(FEObject* pObj) override;
};

// total size: 0xC
struct ResourceConnector : public FEObjectCallback {
    FEPackage* pPack;                // offset 0x4, size 0x4
    FEResourceRequest** pReqList;    // offset 0x8, size 0x4

    bool Callback(FEObject* pObj) override;
    void ConnectListBoxResources(FEListBox* pList);
};

unsigned long FEngine::SysGUID;

FEngine::FEngine()
    : bExecuting(true) //
    , bMouseActive(false) //
    , bLoadObjectNames(false) //
    , bLoadScriptNames(false) //
    , pJoyPad(nullptr) //
    , FastRep(0) //
    , FastRepCache(0) //
    , PadHoldRegistered(0) //
    , WrapMode(kFBW_Wrap) //
    , NumJoyPads(0) //
    , pInterface(nullptr) //
    , CurrentPackageRecordIndex(0) //
    , NextButtonRecordIndex(0) //
    , bErrorScreenMode(false) //
    , bRenderedRecently(false) //
    , bDebugMessages(false) //
{
    bExecuting = true;
    bMouseActive = false;
    FEngMemSet(HoldDecrement, 0, sizeof(HoldDecrement));
    FEngMemSet(HeldButtons, 0, sizeof(HeldButtons));
    Sorter.Zero();
    FEngMemSet(RecordedPackageNames, 0, sizeof(RecordedPackageNames));
    NextButtonRecordIndex = 0;
    FEngMemSet(RecordedPackageButtons, 0, sizeof(RecordedPackageButtons));
    TypeLib.Startup();
}

void FEngine::SetNumJoyPads(unsigned char Count) {
    if (pJoyPad) {
        delete[] pJoyPad;
    }
    if (Count) {
        pJoyPad = new FEJoyPad[Count];
    }
    NumJoyPads = Count;
    FEngMemSet(HoldDecrement, 0, sizeof(HoldDecrement));
}

void FEngine::SetExecution(bool bProcessEverything) {
    FEPackage* pPack = PackList.GetFirstPackage();
    bExecuting = bProcessEverything;
    while (pPack) {
        pPack->bExecuting = bExecuting;
        pPack = pPack->GetNext();
    }
}

void FEngine::SetInitialState() {
    PackageInitStateCB cb;
    ForAllObjects(cb);
    unsigned char i = 0;
    while (i < NumJoyPads) {
        pJoyPad[i].Reset();
        i++;
    }
    if (bMouseActive) {
        Mouse.Reset();
    }
}

FEPackage* FEngine::FindIdlePackage(const char* pName) const {
    return static_cast<FEPackage*>(IdleList.FindNode(pName));
}

FEPackage* FEngine::FindPackageWithControl() {
    FEPackage* pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (pPack->Controllers) {
            return pPack;
        }
        pPack = pPack->GetNext();
    }
    return nullptr;
}

bool FEngine::ForAllObjects(FEObjectCallback& Callback) {
    FEPackage* pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (!pPack->ForAllObjects(Callback)) {
            return false;
        }
        pPack = pPack->GetNext();
    }
    return true;
}

// total size: 0x20
struct FEMessageNode : public FEMinNode {
    FEObject* pMsgFrom;         // offset 0xC, size 0x4
    FEObject* pMsgTarget;       // offset 0x10, size 0x4
    FEPackage* pFromPackage;    // offset 0x14, size 0x4
    unsigned long MsgID;        // offset 0x18, size 0x4
    unsigned long ControlMask;  // offset 0x1C, size 0x4

    inline ~FEMessageNode() override {}
};

// total size: 0x20
struct FEPackageCommand : public FENode {
    int iCommand;               // offset 0x14, size 0x4
    unsigned long uControlMask; // offset 0x18, size 0x4
    FEPackage* pPackage;        // offset 0x1C, size 0x4

    inline ~FEPackageCommand() override {}
};

void FEngine::SetProcessInput(FEPackage* pkg, bool bProcess) {
    if (!pkg) {
        return;
    }
    pkg->bInputEnabled = bProcess;
}

FEPackage* FEngine::GetFirstLibrary() const {
    return static_cast<FEPackage*>(LibraryList.GetHead());
}

void FEngine::AddToIdleList(FEPackage* pPack) {
    IdleList.AddNode(IdleList.GetTail(), pPack);
}

void FEngine::AddToLibraryList(FEPackage* pPack) {
    LibraryList.AddNode(LibraryList.GetTail(), pPack);
}

void FEngine::RemoveFromLibraryList(FEPackage* pPack) {
    LibraryList.RemNode(pPack);
}

FEPackage* FEngine::FindLibraryPackage(unsigned long NameHash) const {
    FEPackage* pPack = GetFirstLibrary();
    while (pPack) {
        if (FEHashUpper(pPack->pFilename + 2) == NameHash) {
            return pPack;
        }
        pPack = pPack->GetNext();
    }
    return nullptr;
}

void FEngine::QueueMessage(unsigned long MsgID, FEObject* pFrom, FEPackage* pFromPackage, FEObject* pTo, unsigned long ControlMask) {
    FEMessageNode* pNode = static_cast<FEMessageNode*>(FEngMalloc(sizeof(FEMessageNode), nullptr, 0));
    pNode->prev = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    pNode->next = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    pNode->MsgID = MsgID;
    pNode->pMsgFrom = pFrom;
    pNode->pFromPackage = pFromPackage;
    pNode->pMsgTarget = pTo;
    pNode->ControlMask = ControlMask;
    if (bDebugMessages) {
        int iVar2 = *reinterpret_cast<int*>(pInterface);
        typedef void (*DebugFn)(void*, unsigned long, FEPackage*, FEObject*, FEObject*, unsigned long);
        DebugFn fn = *reinterpret_cast<DebugFn*>(iVar2 + 0xB4);
        void* adjusted = reinterpret_cast<void*>(reinterpret_cast<char*>(pInterface) + *reinterpret_cast<short*>(iVar2 + 0xB0));
        fn(adjusted, MsgID, pFromPackage, pTo, pFrom, ControlMask);
    }
    MsgQ.AddNode(MsgQ.GetTail(), pNode);
}

void FEngine::SendMessageToGame(unsigned long MsgID, FEObject* pFrom, FEPackage* pFromPackage, unsigned long uControlMask) {
    int iVar1 = *reinterpret_cast<int*>(pInterface);
    typedef void (*GameMsgFn)(void*, unsigned long, FEObject*, unsigned long, FEPackage*);
    GameMsgFn fn = *reinterpret_cast<GameMsgFn*>(iVar1 + 0x3C);
    void* adjusted = reinterpret_cast<void*>(reinterpret_cast<char*>(pInterface) + *reinterpret_cast<short*>(iVar1 + 0x38));
    fn(adjusted, MsgID, pFrom, uControlMask, pFromPackage);
}

void FEngine::QueuePackageSwitch(const char* pPackageName, unsigned long ControlMask) {
    QueuePackageCommand(3, ControlMask, pPackageName);
}

void FEngine::QueuePackagePush(const char* pPackageName, unsigned long ControlMask) {
    QueuePackageCommand(2, ControlMask, pPackageName);
}

void FEngine::QueuePackagePop() {
    QueuePackageCommand(1, 0, nullptr);
}

FEPackageCommand* FEngine::FindQueuedNodeWithControl() {
    FEPackageCommand* pCmd = static_cast<FEPackageCommand*>(PackageCommands.GetTail());
    while (pCmd) {
        if (pCmd->iCommand & 2) {
            return pCmd;
        }
        pCmd = static_cast<FEPackageCommand*>(pCmd->GetPrev());
    }
    return nullptr;
}

void FEngine::RecordLastPackageButton(unsigned long PackHash, unsigned long ButtonGUID) {
    int i = 0;
    do {
        if (RecordedPackageButtons[i].PackageHash == PackHash) {
            RecordedPackageButtons[i].PackageHash = 0;
        }
        i++;
    } while (i < 32);
    RecordedPackageButtons[NextButtonRecordIndex].PackageHash = PackHash;
    RecordedPackageButtons[NextButtonRecordIndex].ButtonGUID = ButtonGUID;
    int next = NextButtonRecordIndex + 1;
    int tmp = next;
    if (next < 0) {
        tmp = NextButtonRecordIndex + 32;
    }
    NextButtonRecordIndex = tmp - (tmp / 32) * 32;
}

unsigned long FEngine::RecallLastPackageButton(unsigned long PackHash) {
    int i = 0;
    do {
        if (RecordedPackageButtons[i].PackageHash == PackHash) {
            return RecordedPackageButtons[i].ButtonGUID;
        }
        i++;
    } while (i < 32);
    return 0;
}

bool FEngine::RecordPackageMarker(const char* pName) {
    int idx = CurrentPackageRecordIndex;
    if (idx != 16) {
        CurrentPackageRecordIndex = idx + 1;
        FEngStrCpy(RecordedPackageNames[idx], pName);
    }
    return idx != 16;
}

const char* FEngine::RecallPackageMarker() {
    if (CurrentPackageRecordIndex != 0) {
        int idx = CurrentPackageRecordIndex - 1;
        CurrentPackageRecordIndex = idx;
        return RecordedPackageNames[idx];
    }
    return nullptr;
}

void FEngine::ClearPackageMarkers() {
    unsigned long i = 0;
    do {
        RecordedPackageNames[i][0] = '\0';
        i++;
    } while (i < 16);
    CurrentPackageRecordIndex = 0;
}

FEPackage* FEngine::LoadPackage(const void* pPackageData, bool bLoadAsLibrary) {
    FEPackageReader reader;
    FEPackage* pPack = reader.Load(pPackageData, pInterface, this, bLoadObjectNames, bLoadScriptNames, bLoadAsLibrary);
    if (!pPack) {
        return nullptr;
    }
    return pPack;
}

bool FEngine::UnloadPackage(FEPackage* pPackage) {
    FEPackage* pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (pPack == pPackage) {
            break;
        }
        pPack = pPack->GetNext();
    }
    if (!pPack) {
        return false;
    }
    bool bOwnsMemory;
    if (!pInterface) {
        bOwnsMemory = true;
    } else {
        bOwnsMemory = pInterface->PackageWillUnload(pPack);
    }
    PackList.RemovePackage(pPackage);
    FEPackageCommand* pCmd = static_cast<FEPackageCommand*>(PackageCommands.GetHead());
    while (pCmd) {
        FEPackageCommand* pNext = static_cast<FEPackageCommand*>(pCmd->GetNext());
        if (pCmd->pPackage == pPackage) {
            PackageCommands.RemNode(pCmd);
            if (pCmd) {
                delete pCmd;
            }
        }
        pCmd = pNext;
    }
    if (!pPack->bIsLibrary) {
        FENode* pLibNode = static_cast<FENode*>(pPack->LibrariesUsed.GetHead());
        while (pLibNode) {
            FEPackage* pLib = FindLibraryPackage(pLibNode->GetNameHash());
            if (pLib) {
                int RefCount = pLib->NumLibRefs - 1;
                if (RefCount < 1) {
                    UnloadLibraryPackage(pLib);
                } else {
                    pLib->NumLibRefs = RefCount;
                }
            }
            pLibNode = pLibNode->GetNext();
        }
        pPack->Shutdown(pInterface);
        if (bOwnsMemory && pPack) {
            delete pPack;
        }
    } else {
        AddToIdleList(pPackage);
    }
    return true;
}

FEPackage* FEngine::PushPackage(const char* pPackageName, const unsigned char Level, const unsigned long ControlMask) {
    FEPackage* pPack = FindIdlePackage(pPackageName);
    if (!pPack) {
        int len = FEngStrLen(pPackageName);
        const char* pBaseName = pPackageName + len - 1;
        char c = *pBaseName;
        while (c != '/' && c != '\\' && len > 0) {
            len--;
            pBaseName--;
            c = *pBaseName;
        }
        if (len != 0) {
            pBaseName++;
        }
        pPack = FindIdlePackage(pBaseName);
        if (!pPack) {
            unsigned char* pBlockStart;
            bool bDeleteBlock;
            unsigned char* pPackData = pInterface->GetPackageData(pPackageName, &pBlockStart, bDeleteBlock);
            if (!pPackData) {
                return nullptr;
            }
            pPack = LoadPackage(pPackData, false);
            if (bDeleteBlock && pBlockStart) {
                delete[] pBlockStart;
            }
            if (!pPack) {
                return nullptr;
            }
            goto loaded;
        }
    }
    {
        PackageInitStateCB cb;
        pPack->bUseIdleList = true;
        pPack->ForAllObjects(cb);
        IdleList.RemNode(pPack);
    }
loaded:
    pPack->Controllers = ControlMask;
    pPack->Priority = Level;
    pPack->bExecuting = bExecuting;
    if (pInterface) {
        pInterface->PackageWasLoaded(pPack);
    }
    PackList.AddPackage(pPack);
    return pPack;
}

void FEngine::QueuePackageUserTransfer(FEPackage* pPack, bool bPush, unsigned long ControlMask) {
    FEPackageCommand* pCmd = static_cast<FEPackageCommand*>(FEngMalloc(sizeof(FEPackageCommand), nullptr, 0));
    pCmd->prev = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    pCmd->next = reinterpret_cast<FEMinNode*>(0xABADCAFE);
    pCmd->uControlMask = 0;
    pCmd->iCommand = 0;
    pCmd->pPackage = pPack;
    pCmd->uControlMask = pPack->Controllers & ControlMask;
    int cmd = 4;
    if (bPush) {
        cmd = 8;
    }
    pCmd->iCommand = cmd;
    PackageCommands.AddNode(PackageCommands.GetTail(), pCmd);
}

int FEngine::GetNumPackagesBelowPriority(unsigned char priority) {
    int count = 0;
    FEPackage* pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (pPack->Priority < priority) {
            count++;
        }
        pPack = pPack->GetNext();
    }
    FEPackageCommand* pCmd = static_cast<FEPackageCommand*>(PackageCommands.GetHead());
    while (pCmd) {
        unsigned long cmd = pCmd->iCommand;
        if (count == 0 && (cmd & 3)) {
            count = 1;
        } else if (cmd & 2) {
            count++;
        } else if (cmd & 1) {
            count--;
        }
        pCmd = static_cast<FEPackageCommand*>(pCmd->GetNext());
    }
    return count;
}

void FEngine::ProcessObjectMessage(FEObject* pObj, FEPackage* pPack, unsigned long MsgID, unsigned long uControlMask) {
    if (pObj->Type == FE_List) {
        if (ProcessListBoxResponses(pObj, MsgID)) {
            return;
        }
    }
    if (pObj->Type == FE_CodeList) {
        if (ProcessCodeListBoxResponses(pObj, MsgID)) {
            return;
        }
    }
    FEMessageResponse* pResp = pObj->FindResponse(MsgID);
    if (pResp) {
        ProcessResponses(pResp, pObj, pPack, uControlMask);
    }
}

void FEngine::ProcessGlobalMessage(FEPackage* pPack, unsigned long MsgID, unsigned long uControlMask) {
    FEMessageResponse* pResp = pPack->FindResponse(MsgID);
    if (pResp) {
        ProcessResponses(pResp, nullptr, pPack, uControlMask);
    }
}

bool FEngine::ProcessListBoxResponses(FEObject* pObj, unsigned long MsgID) {
    FEListBox* pList = static_cast<FEListBox*>(pObj);
    long lCol;
    long lRow;
    if (MsgID == 0xe10814a6) {
        lCol = 0;
        lRow = 1;
    } else if (MsgID == 0x030471ac) {
        lCol = 1;
        lRow = 0;
    } else if (MsgID == 0xe10c4af9) {
        lCol = -1;
        lRow = 0;
    } else if (MsgID == 0xfb814f13) {
        lCol = 0;
        lRow = -1;
    } else {
        return false;
    }
    pList->ScrollSelection(lCol, lRow);
    return true;
}

bool FEngine::ProcessCodeListBoxResponses(FEObject* pObj, unsigned long MsgID) {
    FECodeListBox* pList = static_cast<FECodeListBox*>(pObj);
    long lCol;
    long lRow;
    if (MsgID == 0xe10814a6) {
        lCol = 0;
        lRow = 1;
    } else if (MsgID == 0x030471ac) {
        lCol = 1;
        lRow = 0;
    } else if (MsgID == 0xe10c4af9) {
        lCol = -1;
        lRow = 0;
    } else if (MsgID == 0xfb814f13) {
        lCol = 0;
        lRow = -1;
    } else {
        return false;
    }
    pList->ScrollSelection(lCol, lRow);
    return true;
}

bool FEngine::ProcessListBoxResponses(FEObject* pObj, FEPackage* pPack, unsigned long MsgID) {
    FEListBox* pList = static_cast<FEListBox*>(pObj);
    long lCol;
    long lRow;
    if (MsgID == 0xe10814a6) {
        lCol = 0;
        lRow = 1;
    } else if (MsgID == 0x030471ac) {
        lCol = 1;
        lRow = 0;
    } else if (MsgID == 0xe10c4af9) {
        lCol = -1;
        lRow = 0;
    } else if (MsgID == 0xfb814f13) {
        lCol = 0;
        lRow = -1;
    } else {
        return false;
    }
    pList->ScrollSelection(lCol, lRow);
    return true;
}

bool FEngine::ProcessCodeListBoxResponses(FEObject* pObj, FEPackage* pPack, unsigned long MsgID) {
    FECodeListBox* pList = static_cast<FECodeListBox*>(pObj);
    long lCol;
    long lRow;
    if (MsgID == 0xe10814a6) {
        lCol = 0;
        lRow = 1;
    } else if (MsgID == 0x030471ac) {
        lCol = 1;
        lRow = 0;
    } else if (MsgID == 0xe10c4af9) {
        lCol = -1;
        lRow = 0;
    } else if (MsgID == 0xfb814f13) {
        lCol = 0;
        lRow = -1;
    } else {
        return false;
    }
    pList->ScrollSelection(lCol, lRow);
    return true;
}

void FEngine::UnloadLibraryPackage(FEPackage* pLibPack) {
    bool bDelete = pInterface->UnloadUnreferencedLibrary(pLibPack);
    if (bDelete) {
        RemoveFromLibraryList(pLibPack);
        bool bOwnsMemory;
        if (!pInterface) {
            bOwnsMemory = true;
        } else {
            bOwnsMemory = pInterface->PackageWillUnload(pLibPack);
        }
        pLibPack->Shutdown(pInterface);
        if (bOwnsMemory && pLibPack) {
            pLibPack->~FEPackage();
        }
    }
}

void FEngine::Render() {
    FEMatrix4 mIdentity;
    mIdentity.Identify();
    FEMatrix4 mView;
    pInterface->GetViewTransformation(&mView);
    FEPackage* aPackages[32];
    int numPackages = 0;
    for (FEPackage* pPack = PackList.GetFirstPackage(); pPack; pPack = pPack->GetNext()) {
        aPackages[numPackages] = pPack;
        numPackages++;
    }
    int i;
    for (i = 0; i < numPackages; i++) {
        PackList.RemovePackage(aPackages[i]);
    }
    for (i = 0; i < numPackages; i++) {
        PackList.AddPackage(aPackages[i]);
    }
    FEPackage* pPack = PackList.GetFirstPackage();
    uGroupContext = 0;
    while (pPack) {
        pInterface->BeginPackageRendering(pPack);
        FEObject* pObj = pPack->GetFirstObject();
        Sorter.Zero();
        while (pObj) {
            if (pObj->Type == FE_Group) {
                RenderGroup(static_cast<FEGroup*>(pObj), mIdentity, mView, 0);
            } else {
                RenderObject(pObj, mView, 0);
            }
            pObj = pObj->GetNext();
        }
        Sorter.SortObjects();
        pInterface->RenderObjectList(reinterpret_cast<FEObjectListEntry*>(Sorter.GetListPtr()), Sorter.GetNumObjects());
        pInterface->EndPackageRendering(pPack);
        pPack = pPack->GetNext();
    }
    bRenderedRecently = bExecuting;
}

void FEngine::RenderGroup(FEGroup* pGroup, FEMatrix4& mParent, FEMatrix4& mAccum, unsigned short RenderContext) {
    FEObjData* pData = pGroup->GetObjData();
    FEVector3 pivot(0.0f);
    FEVector3 neg(0.0f);
    if (pData->Col.a != 0) {
        if (bExecuting || static_cast<int>(pGroup->Flags) > -1) {
            FEMatrix4 mRot;
            pData->Rot.GetMatrix(&mRot);
            neg.x = -pData->Pivot.x;
            neg.y = -pData->Pivot.y;
            neg.z = -pData->Pivot.z;
            FEMultMatrix(&pivot, &mRot, &neg);
            FEMatrix4 mLocal;
            mLocal.m41 = pivot.x + pData->Pivot.x + pData->Pos.x;
            mLocal.m42 = pivot.y + pData->Pivot.y + pData->Pos.y;
            mLocal.m43 = pivot.z + pData->Pivot.z + pData->Pos.z;
            FEMatrix4 mCombined;
            FEMultMatrix(&mCombined, &mRot, &mParent);
            FEMatrix4 mFinal;
            FEMultMatrix(&mFinal, &mCombined, &mAccum);
            unsigned short ctx = uGroupContext + 1;
            uGroupContext = ctx;
            pGroup->RenderContext = RenderContext;
            pInterface->GenerateRenderContext(ctx, pGroup);
            FEObject* pObj = pGroup->GetFirstChild();
            while (pObj) {
                if (pObj->Type == FE_Group) {
                    RenderGroup(static_cast<FEGroup*>(pObj), mCombined, mAccum, ctx);
                } else {
                    RenderObject(pObj, mFinal, ctx);
                }
                pObj = pObj->GetNext();
            }
        }
    }
}

void FEngine::RenderObject(FEObject* pObj, FEMatrix4& mParent, unsigned short RenderContext) {
    FEObjData* pData = pObj->GetObjData();
    if (pData->Col.a != 0) {
        FEVector3 pos;
        pos = pData->Pivot;
        FEVector3 result(0.0f);
        pos.x = pos.x + pData->Pos.x;
        pos.y = pos.y + pData->Pos.y;
        pos.z = pos.z + pData->Pos.z;
        FEMultMatrix(&result, &mParent, &pos);
        pObj->RenderContext = RenderContext;
        if (result.z > 0.0f) {
            Sorter.AddObject(pObj, result.z);
        }
    }
}

void FEngine::QueuePackageCommand(long command, unsigned long ControlMask, const char* pPackageName) {
    FEPackage* pPackageWithControl = FindPackageWithControl();
    FEPackageCommand* Node = static_cast<FEPackageCommand*>(
        static_cast<FENode*>(new (FEngMalloc(sizeof(FEPackageCommand), nullptr, 0)) FENode()));
    Node->iCommand = 0;
    Node->uControlMask = 0;
    Node->pPackage = pPackageWithControl;
    if (pPackageWithControl) {
        if (ControlMask == 0) {
            Node->uControlMask = pPackageWithControl->GetControlMask();
        } else {
            Node->uControlMask = ControlMask;
        }
        pPackageWithControl->SetOldControlMask(pPackageWithControl->GetControlMask());
        pPackageWithControl->SetControlMask(0);
    } else {
        FEPackageCommand* pCom = FindQueuedNodeWithControl();
        if (pCom) {
            if (ControlMask == 0) {
                Node->uControlMask = pCom->uControlMask;
            }
        } else {
            if (ControlMask == 0) {
                Node->uControlMask = 0xFF;
            }
        }
        if (ControlMask != 0) {
            Node->uControlMask = ControlMask;
        }
    }
    Node->iCommand = command;
    Node->SetName(pPackageName);
    PackageCommands.AddNode(static_cast<FEMinNode*>(PackageCommands.GetTail()), static_cast<FEMinNode*>(static_cast<FENode*>(Node)));
}

void FEngine::Update(const long tDeltaTicks, unsigned int lock) {
    FEPackage* pPackage;
    if (bDebugMessages) {
        pInterface->DebugMessageBeginUpdate();
    }
    if (bExecuting) {
        PadHoldRegistered = 0;
        if (bMouseActive) {
            FEMouseInfo Info;
            pInterface->GetMouseInfo(Info);
            Mouse.Update(Info, tDeltaTicks);
        }
        unsigned char PadIndex = 0;
        if (NumJoyPads != 0) {
            do {
                unsigned long mask = pInterface->GetJoyPadMask(PadIndex);
                pJoyPad[PadIndex].Update(mask, tDeltaTicks);
                PadIndex = PadIndex + 1;
            } while (PadIndex < NumJoyPads);
        }
        for (pPackage = PackList.GetFirstPackage(); pPackage; pPackage = pPackage->GetNext()) {
            if (pPackage->IsInputEnabled() &&
                (!bErrorScreenMode || pPackage->IsErrorScreen())) {
                ProcessPadsForPackage(pPackage);
                if (bMouseActive) {
                    ProcessMouseForPackage(pPackage);
                }
            }
        }
        unsigned long i = 0;
        unsigned long MaskBit = 1;
        do {
            if ((PadHoldRegistered & MaskBit) != 0) {
                unsigned char PadIdx = 0;
                if (NumJoyPads != 0) {
                    do {
                        pJoyPad[PadIdx].DecrementHold(MaskBit, HoldDecrement[i]);
                        PadIdx = PadIdx + 1;
                    } while (PadIdx < NumJoyPads);
                }
            }
            HoldDecrement[i] = 0;
            i++;
            MaskBit <<= 1;
        } while (i < 19);
        FastRep = FastRepCache;
    }
    if (bExecuting) {
        if (!bRenderedRecently) {
            FEPackage::uHoldDirtyFlags = 0xFFFFFFFF;
        } else {
            FEPackage::uHoldDirtyFlags = 0;
        }
        pPackage = PackList.GetFirstPackage();
        while (pPackage) {
            FEPackage* pCachedNext = pPackage->GetNext();
            if (!bErrorScreenMode || pPackage->IsErrorScreen()) {
                pPackage->Update(this, tDeltaTicks);
            }
            pPackage = pCachedNext;
        }
        ProcessMessageQueue();
        if (!bErrorScreenMode) {
            ProcessPackageCommands();
        }
        if (MsgQ.GetHead()) {
            ProcessMessageQueue();
        }
        bRenderedRecently = false;
    } else {
        for (pPackage = PackList.GetFirstPackage(); pPackage; pPackage = pPackage->GetNext()) {
            if (!bErrorScreenMode || pPackage->IsErrorScreen()) {
                pPackage->Update(this, tDeltaTicks);
            }
        }
    }
    if (bDebugMessages) {
        pInterface->DebugMessageEndUpdate();
    }
}

void FEngine::ProcessMouseForPackage(FEPackage* pPackage) {
    if (pPackage->Controllers != 0 && (pPackage->Controllers & 1) && pPackage->NumMouseObjects != 0) {
        int NumMO = pPackage->NumMouseObjects;
        int i = 0;
        float mx = static_cast<float>(Mouse.XPos);
        float my = static_cast<float>(Mouse.YPos);
        if (NumMO > 0) {
            do {
                UpdateMouseState(pPackage, pPackage->MouseObjectStates + i, mx, my);
                i++;
            } while (i < NumMO);
        }
    }
}

void FEngine::ProcessMessageQueue() {
    FEMessageNode* pNode = static_cast<FEMessageNode*>(MsgQ.RemHead());
    while (pNode) {
        if (bDebugMessages) {
            pInterface->DebugMessageProcessed(pNode->MsgID, pNode->pMsgTarget, pNode->pMsgFrom, pNode->pFromPackage, pNode->ControlMask);
        }
        FEObject* pTarget = pNode->pMsgTarget;
        unsigned long target = reinterpret_cast<unsigned long>(pTarget);
        if (target == 0xFFFFFFFC) {
            FEPackage* pPack = PackList.GetFirstPackage();
            while (pPack) {
                if (pPack == pNode->pFromPackage) {
                    if (pPack) {
                        ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                        FEMsgTargetList* pTargets = pPack->GetMessageTargets(pNode->MsgID);
                        if (pTargets) {
                            unsigned long Count = pTargets->Count;
                            unsigned long i = 0;
                            unsigned long MsgID = pNode->MsgID;
                            if (Count != 0) {
                                do {
                                    ProcessObjectMessage(pTargets->pTargets[i], pPack, MsgID, pNode->ControlMask);
                                    i++;
                                } while (i < Count);
                            }
                        }
                    }
                    break;
                }
                pPack = pPack->GetNext();
            }
        } else if (target == 0) {
            for (FEPackage* pPack = PackList.GetFirstPackage(); pPack; pPack = pPack->GetNext()) {
                ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                FEMsgTargetList* pTargets = pPack->GetMessageTargets(pNode->MsgID);
                if (pTargets) {
                    unsigned long Count = pTargets->Count;
                    unsigned long i = 0;
                    unsigned long MsgID = pNode->MsgID;
                    if (Count != 0) {
                        do {
                            ProcessObjectMessage(pTargets->pTargets[i], pPack, MsgID, pNode->ControlMask);
                            i++;
                        } while (i < Count);
                    }
                }
            }
        } else if (target == 0xFFFFFFFB) {
            pInterface->NotificationMessage(pNode->MsgID, pNode->pMsgFrom, pNode->ControlMask, reinterpret_cast<unsigned long>(pNode->pFromPackage));
        } else if (target == 0xFFFFFFFE) {
            for (FEPackage* pPack = PackList.GetFirstPackage(); pPack; pPack = pPack->GetNext()) {
                ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
            }
        } else if (target == 0xFFFFFFFF) {
            pInterface->NotifySoundMessage(pNode->MsgID, pNode->pMsgFrom, pNode->ControlMask, reinterpret_cast<unsigned long>(pNode->pFromPackage));
        } else if (target == 0xFFFFFFFD) {
            ProcessGlobalMessage(pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
        } else if (target == 0xFFFFFFFA) {
            if (pNode->MsgID == 0x59bed120) {
                SetProcessInput(pNode->pFromPackage, true);
            } else if (pNode->MsgID == 0x5d4ce32d) {
                SetProcessInput(pNode->pFromPackage, false);
            }
        } else {
            ProcessObjectMessage(pTarget, pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
        }
        if (pNode) {
            delete pNode;
        }
        pNode = static_cast<FEMessageNode*>(MsgQ.RemHead());
    }
}

void FEngine::ProcessResponses(FEMessageResponse* pRespList, FEObject* pObj, FEPackage* pPack, unsigned long uControlMask) {
    unsigned long i = 0;
    unsigned long NumActions = pRespList->Count;
    if (NumActions == 0) {
        return;
    }
    do {
        unsigned long Action = pRespList->pResponseList[i].ResponseID;
        FEResponse* pAction = &pRespList->pResponseList[i];
        if (Action == 0x108) {
            QueuePackageUserTransfer(pPack, false, 0xFF);
        } else if (Action == 0) {
            if (pObj) {
                FEScript* pScript = pObj->FindScript(pAction->ResponseParam);
                if (pScript) {
                    pObj->SetCurrentScript(pScript);
                    pScript->CurTime = 0;
                }
            }
        } else if (Action == 1) {
            FEObject* pTo = reinterpret_cast<FEObject*>(pAction->ResponseTarget);
            if (reinterpret_cast<unsigned long>(pTo) != 0xFFFFFFFC && reinterpret_cast<unsigned long>(pTo) != 0xFFFFFFFF) {
                pTo = pPack->FindObjectByGUID(pAction->ResponseTarget);
            }
            QueueMessage(pAction->ResponseParam, pObj, pPack, pTo, uControlMask);
        } else if (Action == 2) {
            QueueMessage(pAction->ResponseParam, pObj, pPack, reinterpret_cast<FEObject*>(0xFFFFFFFF), uControlMask);
        } else if (Action == 3) {
            QueueMessage(pAction->ResponseParam, pObj, pPack, reinterpret_cast<FEObject*>(0xFFFFFFFB), uControlMask);
        } else if (Action == 0x100) {
            FEObject* pButton = nullptr;
            if (pAction->ResponseParam != 0) {
                pButton = pPack->FindObjectByGUID(pAction->ResponseParam);
            }
            bool bFound = pButton != nullptr;
            if (bFound || pAction->ResponseParam == 0) {
                pPack->SetCurrentButton(pButton, bFound);
            }
        } else if (Action == 0x101) {
            SetProcessInput(pPack, pAction->ResponseParam == 1);
        } else if (Action == 0x102) {
            if (!pPack->pCurrentButton) {
                RecordLastPackageButton(pPack->nameHash, 0);
            } else {
                RecordLastPackageButton(pPack->nameHash, pPack->pCurrentButton->GUID);
            }
        } else if (Action == 0x103) {
            FEObject* pButton = nullptr;
            unsigned long recalled = RecallLastPackageButton(pPack->nameHash);
            if (recalled != 0) {
                pButton = pPack->FindObjectByGUID(recalled);
            }
            bool bFound = pButton != nullptr;
            if (!bFound) {
                if (pAction->ResponseParam != 0) {
                    pButton = pPack->FindObjectByGUID(pAction->ResponseParam);
                }
                bFound = pButton != nullptr;
                if (!bFound && pAction->ResponseParam != 0) {
                    goto next;
                }
            }
            bFound = bFound;
            pPack->SetCurrentButton(pButton, bFound);
        } else if (Action == 0x104) {
        } else if (Action == 0x106) {
            QueuePackageUserTransfer(pPack, true, 0xFF);
        } else if (Action == 0x105 || Action == 0x107) {
            QueuePackageUserTransfer(pPack, Action < 0x107, uControlMask);
        } else if (Action == 0x200) {
            QueuePackageSwitch(reinterpret_cast<const char*>(pAction->ResponseParam), pPack->Controllers);
        } else if (Action == 0x201) {
            QueuePackagePush(reinterpret_cast<const char*>(pAction->ResponseParam), pPack->Controllers);
        } else if (Action == 0x202) {
            unsigned long pad = 0;
            do {
                if (uControlMask & (1 << (pad & 0x3f))) {
                    QueuePackagePush(reinterpret_cast<const char*>(pAction->ResponseParam), uControlMask);
                }
                pad++;
            } while (pad < 8);
        } else if (Action == 0x203) {
            QueuePackagePop();
        } else if (Action == 0x204) {
            QueuePackagePush(reinterpret_cast<const char*>(pAction->ResponseParam), 0);
        } else if (Action == 0x2c0) {
            RecordPackageMarker(pPack->pFilename);
        } else if (Action == 0x2c1) {
            const char* pMarker = RecallPackageMarker();
            if (pMarker) {
                QueuePackageSwitch(pMarker, pPack->Controllers);
            }
        } else if (Action == 0x2c2) {
            ClearPackageMarkers();
        } else if (Action == 0x300) {
            if (pObj->pCurrentScript->CurTime != static_cast<int>(pAction->ResponseParam)) {
                i = pRespList->FindConditionBranchTarget(i);
            }
        } else if (Action == 0x301) {
            if (pObj->pCurrentScript->CurTime == static_cast<int>(pAction->ResponseParam)) {
                i = pRespList->FindConditionBranchTarget(i);
            }
        } else if (Action == 0x500) {
            i = pRespList->FindConditionBranchTarget(i);
        }
    next:
        i++;
    } while (i < NumActions);
}

void FEngine::ProcessPackageCommands() {
    FEPackageCommand* pCmd = static_cast<FEPackageCommand*>(PackageCommands.GetHead());
    while (pCmd) {
        FEPackageCommand* pNext = static_cast<FEPackageCommand*>(pCmd->GetNext());
        long cmd = pCmd->iCommand;
        if (cmd & 1) {
            UnloadPackage(pCmd->pPackage);
            PackageCommands.RemNode(pCmd);
            if (pCmd) {
                delete pCmd;
            }
        } else if (cmd & 2) {
            FEPackage* pPack = PushPackage(pCmd->GetName(), 0, pCmd->uControlMask);
            if (pPack) {
                PackageCommands.RemNode(pCmd);
                if (pCmd) {
                    delete pCmd;
                }
            }
        } else if (cmd & 4) {
            FEPackage* pPack = pCmd->pPackage;
            if (pPack) {
                PackageCommands.RemNode(pCmd);
                UnloadPackage(pPack);
                if (pCmd) {
                    delete pCmd;
                }
            }
        } else if (cmd & 8) {
            FEPackage* pPack = pCmd->pPackage;
            if (pPack) {
                PackageCommands.RemNode(pCmd);
                IdleList.RemNode(pPack);
                pPack->bExecuting = bExecuting;
                pPack->Controllers = pCmd->uControlMask;
                PackList.AddPackage(pPack);
                if (pCmd) {
                    delete pCmd;
                }
            }
        }
        pCmd = pNext;
    }
}
