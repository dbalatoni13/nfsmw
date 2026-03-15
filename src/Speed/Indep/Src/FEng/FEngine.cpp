#include "Speed/Indep/Src/FEng/fengine_full.h"
#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEPackageReader.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/FEng/cFEng.h"

void FEngGetCenter(FEObject* pObj, float& cx, float& cy);

extern "C" int printf(const char*, ...);

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

FEMultiPool ObjDataPool;
FEColoredImageData MaximumObjData;

FEngine::FEngine()
{
    bExecuting = true;
    bRenderedRecently = false;
    NumJoyPads = 0;
    pJoyPad = nullptr;
    FastRepCache = 0;
    FastRep = 0;
    WrapMode = Wrap_None;
    bMouseActive = false;
    bErrorScreenMode = false;
    bDebugMessages = false;
    bLoadObjectNames = true;
    bLoadScriptNames = true;
    FEngMemSet(HeldButtons, 0, sizeof(HeldButtons));
    CurrentPackageRecordIndex = 0;
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
        FEJoyPad* pPads = static_cast<FEJoyPad*>(FEngMalloc(Count * sizeof(FEJoyPad), nullptr, 0));
        long i = Count - 1;
        FEJoyPad* pCur = pPads;
        do {
            new (pCur) FEJoyPad();
            pCur++;
        } while (i-- != 0);
        pJoyPad = pPads;
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
    FEPackage* pPack = PackList.GetLastPackage();
    while (pPack) {
        if (pPack->Controllers) {
            return pPack;
        }
        pPack = pPack->GetPrev();
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

    ~FEMessageNode() override;
};

// total size: 0x20
struct FEPackageCommand : public FENode {
    int iCommand;               // offset 0x14, size 0x4
    unsigned long uControlMask; // offset 0x18, size 0x4
    FEPackage* pPackage;        // offset 0x1C, size 0x4
};

FEMessageNode::~FEMessageNode() {}

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
    FEMessageNode* pNode = FENG_NEW FEMessageNode();
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
    MsgQ.AddTail(pNode);
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
    NextButtonRecordIndex = (NextButtonRecordIndex + 1) % 32;
}

unsigned long FEngine::RecallLastPackageButton(unsigned long PackHash) {
    for (int i = 0; i < 32; i++) {
        if (RecordedPackageButtons[i].PackageHash == PackHash) {
            return RecordedPackageButtons[i].ButtonGUID;
        }
    }
    return 0;
}

bool FEngine::RecordPackageMarker(const char* pName) {
    int idx = CurrentPackageRecordIndex;
    if (idx == 16) {
        return false;
    }
    CurrentPackageRecordIndex = idx + 1;
    FEngStrCpy(RecordedPackageNames[idx], pName);
    return true;
}

const char* FEngine::RecallPackageMarker() {
    if (CurrentPackageRecordIndex == 0) {
        return nullptr;
    }
    int idx = CurrentPackageRecordIndex - 1;
    CurrentPackageRecordIndex = idx;
    return RecordedPackageNames[idx];
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
        if (pPackage == pPack) {
            bool bDelete;
            if (pInterface) {
                bDelete = pInterface->PackageWillUnload(pPack);
            } else {
                bDelete = true;
            }
            PackList.RemovePackage(pPackage);
            FEPackageCommand* pTempNode = static_cast<FEPackageCommand*>(PackageCommands.GetHead());
            while (pTempNode) {
                FEPackageCommand* pNextNode = static_cast<FEPackageCommand*>(pTempNode->GetNext());
                if (pTempNode->pPackage == pPackage) {
                    PackageCommands.RemNode(pTempNode);
                    if (pTempNode) {
                        delete pTempNode;
                    }
                }
                pTempNode = pNextNode;
            }
            if (pPack->UsesIdleList()) {
                AddToIdleList(pPackage);
            } else {
                FENode* pLibName = static_cast<FENode*>(pPack->GetLibraryList().GetHead());
                while (pLibName) {
                    FEPackage* pLibPack = FindLibraryPackage(pLibName->GetNameHash());
                    if (pLibPack) {
                        int Pri = pLibPack->GetPriority() - 1;
                        if (Pri < 1) {
                            UnloadLibraryPackage(pLibPack);
                        } else {
                            pLibPack->SetPriority(Pri);
                        }
                    }
                    pLibName = pLibName->GetNext();
                }
                pPack->Shutdown(pInterface);
                if (bDelete) {
                    if (pPack) {
                        delete pPack;
                    }
                }
            }
            return true;
        }
        pPack = pPack->GetNext();
    }
    return false;
}

FEPackage* FEngine::PushPackage(const char* pPackageName, const unsigned char Level, const unsigned long ControlMask) {
    FEPackage* pPack = FindIdlePackage(pPackageName);
    if (!pPack) {
        char len = static_cast<char>(FEngStrLen(pPackageName));
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
    }
    if (pPack) {
        PackageInitStateCB cb;
        pPack->bUseIdleList = true;
        pPack->ForAllObjects(cb);
        IdleList.RemNode(pPack);
    } else {
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
    }
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
    printf("If you get this, come see Gary or Lolley!\n");
    FEPackageCommand* pCmd = FENG_NEW FEPackageCommand();
    pCmd->uControlMask = 0;
    pCmd->iCommand = 0;
    pCmd->pPackage = pPack;
    pCmd->uControlMask = pPack->GetControlMask() & ControlMask;
    int cmd = 4;
    if (bPush) {
        cmd = 8;
    }
    pCmd->iCommand = cmd;
    PackageCommands.AddTail(pCmd);
}

int FEngine::GetNumPackagesBelowPriority(unsigned char priority) {
    int count = 0;
    FEPackage* package = PackList.GetFirstPackage();
    while (package) {
        if (package->GetPriority() < priority) {
            count++;
        }
        package = package->GetNext();
    }
    FEPackageCommand* pNode = static_cast<FEPackageCommand*>(PackageCommands.GetHead());
    while (pNode) {
        if (count == 0 && (pNode->iCommand & 3)) {
            count = 1;
        } else if (pNode->iCommand & 2) {
            count++;
        } else if (pNode->iCommand & 1) {
            count--;
        }
        pNode = static_cast<FEPackageCommand*>(pNode->GetNext());
    }
    return count;
}

void FEngine::ProcessObjectMessage(FEObject* pObj, FEPackage* pPack, unsigned long MsgID, unsigned long uControlMask) {
    if (pObj->Type == FE_List) {
        if (ProcessListBoxResponses(pObj, pPack, MsgID)) {
            return;
        }
    }
    if (pObj->Type == FE_CodeList) {
        if (ProcessCodeListBoxResponses(pObj, pPack, MsgID)) {
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

bool FEngine::ProcessListBoxResponses(FEObject* pObj, FEPackage* pPack, unsigned long MsgID) {
    FEListBox* pList = static_cast<FEListBox*>(pObj);
    switch (MsgID) {
        case 0xe10c4af9: pList->ScrollSelection(-1, 0); return true;
        case 0x030471ac: pList->ScrollSelection(1, 0); return true;
        case 0xfb814f13: pList->ScrollSelection(0, -1); return true;
        case 0xe10814a6: pList->ScrollSelection(0, 1); return true;
    }
    return false;
}

bool FEngine::ProcessCodeListBoxResponses(FEObject* pObj, FEPackage* pPack, unsigned long MsgID) {
    FECodeListBox* pList = static_cast<FECodeListBox*>(pObj);
    switch (MsgID) {
        case 0xe10c4af9: pList->ScrollSelection(-1, 0); return true;
        case 0x030471ac: pList->ScrollSelection(1, 0); return true;
        case 0xfb814f13: pList->ScrollSelection(0, -1); return true;
        case 0xe10814a6: pList->ScrollSelection(0, 1); return true;
    }
    return false;
}

void FEngine::UnloadLibraryPackage(FEPackage* pLibPack) {
    bool bDelete = pInterface->UnloadUnreferencedLibrary(pLibPack);
    if (bDelete) {
        RemoveFromLibraryList(pLibPack);
        bool bOwnsMemory;
        if (pInterface) {
            bOwnsMemory = pInterface->PackageWillUnload(pLibPack);
        } else {
            bOwnsMemory = true;
        }
        pLibPack->Shutdown(pInterface);
        if (bOwnsMemory && pLibPack) {
            delete pLibPack;
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
    FEPackageCommand* Node = FENG_NEW FEPackageCommand();
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
        for (unsigned char PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
            unsigned long mask = pInterface->GetJoyPadMask(PadIndex);
            pJoyPad[PadIndex].Update(mask, tDeltaTicks);
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
                for (unsigned char PadIdx = 0; PadIdx < NumJoyPads; PadIdx++) {
                    pJoyPad[PadIdx].DecrementHold(MaskBit, HoldDecrement[i]);
                }
            }
            HoldDecrement[i] = 0;
            i++;
            MaskBit <<= 1;
        } while (i < 19);
        FastRep = FastRepCache;
    }
    if (bExecuting) {
        int iTicksRemaining = tDeltaTicks;
        do {
            int iIterationTicks;
            if (!bRenderedRecently) {
                FEPackage::uHoldDirtyFlags = 0xFFFFFFFF;
            } else {
                FEPackage::uHoldDirtyFlags = 0;
            }
            pPackage = PackList.GetFirstPackage();
            while (pPackage) {
                FEPackage* pCachedNext = pPackage->GetNext();
                if (!bErrorScreenMode || pPackage->IsErrorScreen()) {
                    pPackage->Update(this, iTicksRemaining);
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
            iTicksRemaining = 0;
        } while (iTicksRemaining);
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

struct ImpulseDirEntry {
    unsigned char dir0;
    unsigned char dir1;
    unsigned short directionIndex;
};

static unsigned long PadButtonHash[19] = {
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0x406415E3u, 0x911AB364u, 0xB5AF2461u, 0x5073EF13u,
    0xD9FEEC59u, 0xC519BFBFu, 0xC519BFC0u, 0xC519BFC1u,
    0xC519BFC2u, 0xC519BFC3u, 0xC519BFC4u, 0xC519BFC5u,
    0xC519BFC6u, 0xC519BFC7u, 0xC519BFC8u,
};

static unsigned long PadButtonHeldHash[2] = {
    0x447315AFu, 0x20AD4EB5u,
};

static unsigned long PadReleasedHash[19] = {
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u,
    0xC12E9E27u, 0xC2F8FCC8u, 0xEBFCDA65u, 0x091DCD57u,
    0x7A39195Du, 0xD4671F83u, 0xD871B0A4u, 0xDC7C41C5u,
    0xE086D2E6u, 0xE4916407u, 0xE89BF528u, 0xECA68649u,
    0xF0B1176Au, 0xF4BBA88Bu, 0xF8C639ACu,
};

unsigned long FEDirection_Message[8] = {
    0x72619778u, 0x6FD81B16u, 0xB5971BF1u, 0xAB1A49C9u,
    0x911C0A4Bu, 0x79891376u, 0x9120409Eu, 0x6FFB6F23u,
};

static ImpulseDirEntry ImpulseDir[8] = {
    {0x00, 0x02, 0x0007}, {0x00, 0x03, 0x0001}, {0x01, 0x02, 0x0005}, {0x01, 0x03, 0x0003},
    {0x00, 0xFF, 0x0000}, {0x02, 0xFF, 0x0006}, {0x01, 0xFF, 0x0004}, {0x03, 0xFF, 0x0002},
};

inline int FEFramesToTicks(int Frames) { return Frames * 16; }

void FEngine::ProcessPadsForPackage(FEPackage* pPackage) {
    unsigned long Pressed;
    unsigned long Released;
    unsigned long Held;
    unsigned long Mask;
    unsigned long HeldFor[19];
    unsigned char FromPadHeld[19];
    unsigned char FromPadPressed[19];
    unsigned char FromPadReleased[19];
    unsigned char PadIndex;
    unsigned long i;
    unsigned long JoyMask;
    bool bSomethingActive;

    JoyMask = pPackage->GetControlMask();
    if (JoyMask == 0) {
        return;
    }

    bSomethingActive = false;
    PadIndex = 0;
    if (NumJoyPads != 0) {
        do {
            if ((JoyMask & (1 << (PadIndex & 0x3F))) != 0) {
                bSomethingActive = bSomethingActive | pJoyPad[PadIndex].WasActive();
            }
            PadIndex = (PadIndex + 1) & 0xFF;
        } while (PadIndex < NumJoyPads);
    }
    if (!bSomethingActive) {
        return;
    }

    FEngMemSet(HeldFor, 0, sizeof(HeldFor));
    FEngMemSet(FromPadHeld, 0, 19);
    FEngMemSet(FromPadPressed, 0, 19);
    FEngMemSet(FromPadReleased, 0, 19);

    i = 4;
    while (i < 19 && pPackage->IsInputEnabled()) {
        FEObject* pCurButton;
        JoyMask = pPackage->GetControlMask();
        Mask = 1 << (i & 0x3F);
        Pressed = 0;
        Released = 0;
        Held = 0;

        {
            unsigned char PadIndex = 0;
            if (NumJoyPads != 0) {
                do {
                    if ((JoyMask & (1 << (PadIndex & 0x3F))) != 0) {
                        if (pJoyPad[PadIndex].WasPressed(Mask)) {
                            Pressed = Pressed | Mask;
                            FromPadPressed[i] = FromPadPressed[i] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                        if (pJoyPad[PadIndex].WasReleased(Mask)) {
                            Released = Released | Mask;
                            FromPadReleased[i] = FromPadReleased[i] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                        if (pJoyPad[PadIndex].WasHeld(Mask)) {
                            Held = Held | Mask;
                            if (HeldFor[i] <= pJoyPad[PadIndex].HeldFor(Mask)) {
                                HeldFor[i] = pJoyPad[PadIndex].HeldFor(Mask);
                            }
                            FromPadHeld[i] = FromPadHeld[i] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                    }
                    PadIndex = (PadIndex + 1) & 0xFF;
                } while (PadIndex < NumJoyPads);
            }
        }

        if (i == 4 && pPackage->StartEqualsAccept()) {
            unsigned char PadIndex = 0;
            if (NumJoyPads != 0) {
                do {
                    if ((JoyMask & (1 << (PadIndex & 0x3F))) != 0) {
                        if (pJoyPad[PadIndex].WasPressed(0x40)) {
                            Pressed = Pressed | Mask;
                            FromPadPressed[4] = FromPadPressed[4] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                        if (pJoyPad[PadIndex].WasReleased(0x40)) {
                            Released = Released | Mask;
                            FromPadReleased[4] = FromPadReleased[4] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                        if (pJoyPad[PadIndex].WasHeld(0x40)) {
                            Held = Held | Mask;
                            if (HeldFor[4] <= pJoyPad[PadIndex].HeldFor(0x40)) {
                                HeldFor[4] = pJoyPad[PadIndex].HeldFor(0x40);
                            }
                            FromPadHeld[4] = FromPadHeld[4] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                    }
                    PadIndex = (PadIndex + 1) & 0xFF;
                } while (PadIndex < NumJoyPads);
            }
        }

        if ((Held | Released | Pressed) != 0) {
            pCurButton = pPackage->GetCurrentButton();

            if (i < 9) {
                if (i > 6) {
                    if ((Held & Mask) != 0) {
                        unsigned long PadMask = FromPadPressed[i];
                        unsigned long MsgID = PadButtonHeldHash[i - 7];
                        if (pCurButton == nullptr || pCurButton->FindResponse(MsgID) == nullptr) {
                            if (pPackage->FindResponse(MsgID) != nullptr) {
                                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFD), PadMask);
                                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                            }
                        } else {
                            QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                            QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                        }
                    }
                    goto check_released;
                }
                if (i == 4) {
                    if ((Pressed & 0x10) == 0) goto check_released;
                    HeldButtons[4] = pCurButton;
                    if (pCurButton == nullptr || pCurButton->FindResponse(0x0C407210u) == nullptr) {
                        if (pPackage->FindResponse(0x406415E3u) == nullptr) goto check_released;
                        QueueMessage(0x406415E3u, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFD), FromPadPressed[4]);
                        QueueMessage(0x406415E3u, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), FromPadPressed[4]);
                    } else {
                        QueueMessage(0x0C407210u, nullptr, pPackage, pPackage->GetCurrentButton(), FromPadPressed[4]);
                        QueueMessage(
                            0x0C407210u,
                            pPackage->GetCurrentButton(),
                            pPackage,
                            reinterpret_cast<FEObject*>(0xFFFFFFFB),
                            FromPadPressed[4]);
                    }
                    goto check_released;
                }
                if (i > 3) goto check_released;
            } else {
                if (i > 18) goto check_released;
            }

            if ((Pressed & Mask) != 0) {
                unsigned long PadMask = FromPadPressed[i];
                unsigned long MsgID = PadButtonHash[i];
                HeldButtons[i] = pCurButton;
                if (pCurButton == nullptr || pCurButton->FindResponse(MsgID) == nullptr) {
                    if (pPackage->FindResponse(MsgID) != nullptr) {
                        QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFD), PadMask);
                        QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                    }
                } else {
                    QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                    QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                }
            }

        check_released:
            if ((Released & Mask) != 0) {
                unsigned long PadMask = FromPadReleased[i];
                unsigned long MsgID = PadReleasedHash[i];
                if (HeldButtons[i] == pCurButton && pCurButton != nullptr) {
                    HeldButtons[i] = nullptr;
                    if (i == 4) {
                        MsgID = 0x936A6A7Fu;
                    }
                    if (pCurButton->FindResponse(MsgID) != nullptr) {
                        QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                        QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                    }
                }
                PadMask = FromPadReleased[i];
                if (pPackage->FindResponse(MsgID) != nullptr) {
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFD), PadMask);
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                }
            }

            if (MsgQ.GetNumElements() != 0) {
                ProcessMessageQueue();
            }
        }
        i = i + 1;
    }

    // Direction pad processing
    JoyMask = pPackage->GetControlMask();
    Pressed = 0;
    i = 0;
    while (i < 4 && pPackage->IsInputEnabled()) {
        Mask = 1 << (i & 0x3F);
        {
            unsigned char PadIndex = 0;
            if (NumJoyPads != 0) {
                do {
                    if ((JoyMask & (1 << (PadIndex & 0x3F))) != 0) {
                        if (pJoyPad[PadIndex].WasPressed(Mask)) {
                            Pressed = Pressed | Mask;
                            FromPadPressed[i] = FromPadPressed[i] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                        pJoyPad[PadIndex].WasReleased(Mask);
                        if (pJoyPad[PadIndex].WasHeld(Mask)) {
                            if (HeldFor[i] <= pJoyPad[PadIndex].HeldFor(Mask)) {
                                HeldFor[i] = pJoyPad[PadIndex].HeldFor(Mask);
                            }
                            FromPadHeld[i] = FromPadHeld[i] | static_cast<unsigned char>(1 << (PadIndex & 0x3F));
                        }
                    }
                    PadIndex = (PadIndex + 1) & 0xFF;
                } while (PadIndex < NumJoyPads);
            }
        }
        i = i + 1;
    }

    i = 0;
    {
        unsigned long Result;
        unsigned long Compare;
        unsigned long JustPressed;
        unsigned long PadMask;
        FEObject* pCurButton;
        while (true) {
            if (i > 7) {
                return;
            }
            if (!pPackage->IsInputEnabled()) {
                return;
            }

            pCurButton = pPackage->GetCurrentButton();
            if (ImpulseDir[i].dir1 == 0xFF) {
                JustPressed = Pressed >> (ImpulseDir[i].dir0 & 0x3F);
                Result = HeldFor[ImpulseDir[i].dir0];
                PadMask = FromPadPressed[ImpulseDir[i].dir0] | FromPadHeld[ImpulseDir[i].dir0];
            } else {
                Result = HeldFor[ImpulseDir[i].dir1];
                if (HeldFor[ImpulseDir[i].dir0] < HeldFor[ImpulseDir[i].dir1]) {
                    Result = HeldFor[ImpulseDir[i].dir0];
                }
                JustPressed =
                    (Pressed >> (ImpulseDir[i].dir0 & 0x3F)) & (Pressed >> (ImpulseDir[i].dir1 & 0x3F));
                PadMask =
                    (FromPadPressed[ImpulseDir[i].dir0] & FromPadPressed[ImpulseDir[i].dir1]) |
                    (FromPadHeld[ImpulseDir[i].dir0] & FromPadHeld[ImpulseDir[i].dir1]);
            }

            Compare = FEFramesToTicks(20);
            if ((FastRep & (1 << (i & 0x3F))) != 0) {
                Compare = 0x78;
            }
            if (Compare <= Result) {
                break;
            }
            if ((JustPressed & 1) == 0) {
                if (Result == 0) {
                    FastRepCache = FastRepCache & ~(1 << (i & 0x3F));
                }
            } else if (Result == 0) {
                break;
            }
            if (MsgQ.GetNumElements() != 0) {
                ProcessMessageQueue();
            }
            i = i + 1;
        }

        if (Result != 0) {
            FastRepCache = FastRepCache | (1 << (i & 0x3F));
        }
        HoldDecrement[ImpulseDir[i].dir0] = Compare;
        if (ImpulseDir[i].dir1 != 0xFF) {
            HoldDecrement[ImpulseDir[i].dir1] = Compare;
            {
                if (ImpulseDir[i].dir1 != 0xFF) {
                    HeldFor[ImpulseDir[i].dir0] = 0;
                    HeldFor[ImpulseDir[i].dir1] = 0;
                    PadHoldRegistered =
                        PadHoldRegistered |
                        (1 << (ImpulseDir[i].dir0 & 0x3F)) |
                        (1 << (ImpulseDir[i].dir1 & 0x3F));
                    goto fire_direction;
                }
            }
        }
        {
            HeldFor[ImpulseDir[i].dir0] = 0;
            PadHoldRegistered = PadHoldRegistered | (1 << (ImpulseDir[i].dir0 & 0x3F));
        }

    fire_direction:
        if (pCurButton == nullptr) {
            unsigned long MsgID = FEDirection_Message[ImpulseDir[i].directionIndex];
            if (pPackage->FindResponse(MsgID) != nullptr) {
                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFD), PadMask);
                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
            }
        } else {
            FEObject* pNewButton = nullptr;
            unsigned long MsgID = FEDirection_Message[ImpulseDir[i].directionIndex];
            FEMessageResponse* pResponse = pCurButton->FindResponse(MsgID);
            if (pResponse == nullptr) {
                if ((pCurButton->Flags & 0x80000) == 0) {
                    pNewButton = pPackage->GetButtonMap()->GetButtonFrom(pCurButton, ImpulseDir[i].directionIndex, pInterface, WrapMode);
                }
                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFD), PadMask);
            } else {
                QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                if ((pCurButton->Flags & 0x80000) == 0) {
                    if (pResponse->FindResponse(0x104) == -1) {
                        pNewButton = pPackage->GetButtonMap()->GetButtonFrom(pCurButton, ImpulseDir[i].directionIndex, pInterface, WrapMode);
                    }
                }
            }
            QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
            if (pNewButton != nullptr) {
                for (unsigned long j = 4; j < 19; j++) {
                    if (HeldButtons[j] != nullptr && pCurButton != nullptr) {
                        unsigned long PadMask;
                        unsigned long MsgID;
                        HeldButtons[j] = nullptr;
                        PadMask = FromPadReleased[j];
                        MsgID = PadReleasedHash[j];
                        if (j == 4) {
                            MsgID = 0x936A6A7Fu;
                        }
                        if (pCurButton->FindResponse(MsgID) != nullptr) {
                            QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                            QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject*>(0xFFFFFFFB), PadMask);
                        }
                    }
                }
                pPackage->SetCurrentButton(pNewButton, true);
            }
        }
    }
}

void FEngine::ProcessMouseForPackage(FEPackage* pPackage) {
    if (pPackage->Controllers != 0 && (pPackage->Controllers & 1) && pPackage->bInputEnabled) {
        float mx = static_cast<float>(Mouse.XPos);
        float my = static_cast<float>(Mouse.YPos);
        int NumMO = pPackage->NumMouseObjects;
        FEObjectMouseState* pStates = pPackage->MouseObjectStates;
        for (int i = 0; i < NumMO; i++) {
            UpdateMouseState(pPackage, pStates + i, mx, my);
        }
    }
}

void FEngine::UpdateMouseState(FEPackage* pPackage, FEObjectMouseState* pState, float mx, float my) {
    FEObject* pObj = pState->pObject;
    if (pObj && (pObj->Flags & 0x14000000) == 0x14000000) {
        return;
    }
    float cx, cy;
    FEngGetCenter(pObj, cx, cy);
    bool bTouching = pInterface->DoesPointTouchObject(
        mx - (static_cast<float>(pState->Offset.h) - cx),
        my - (static_cast<float>(pState->Offset.v) - cy),
        pObj);
    bool bLeftDown = Mouse.IsDown(1);
    bool bRightDown = Mouse.IsDown(2);
    unsigned long flags = pState->Flags;
    bool bWasOver = (flags & 1) != 0;
    bool bWasLeftDown = (flags & 2) != 0;
    bool bWasRightDown = (flags & 4) != 0;

    if (bTouching) {
        unsigned int msg = 0x13f4bd45;
        if (bWasOver) {
            msg = 0xb30d0683;
        }
        cFEng::mInstance->QueuePackageMessage(msg, pPackage->GetFilename(), pObj);
    } else {
        if (bWasOver) {
            cFEng::mInstance->QueuePackageMessage(0xb30793c1, pPackage->GetFilename(), pObj);
        }
    }

    if (bLeftDown) {
        if (!bWasLeftDown) {
            if (!bTouching) {
                goto skip_left;
            }
            cFEng::mInstance->QueuePackageMessage(0xf459b307, pPackage->GetFilename(), pObj);
        } else {
            cFEng::mInstance->QueuePackageMessage(0x1e646b2e, pPackage->GetFilename(), pObj);
        }
    } else {
        if (bWasLeftDown && bTouching) {
            cFEng::mInstance->QueuePackageMessage(0x7eabca56, pPackage->GetFilename(), pObj);
        }
    }
skip_left:

    if (bRightDown) {
        if (bWasRightDown) {
            cFEng::mInstance->QueuePackageMessage(0x0da2f4e1, pPackage->GetFilename(), pObj);
        } else if (bTouching) {
            cFEng::mInstance->QueuePackageMessage(0xce59c3da, pPackage->GetFilename(), pObj);
        } else {
            goto set_not_over;
        }
        if (!bTouching) {
            goto set_not_over;
        }
        flags = pState->Flags | 1;
        goto set_flags;
    } else {
        if (bWasRightDown) {
            if (!bTouching) {
                goto set_not_over;
            }
            cFEng::mInstance->QueuePackageMessage(0x98adf589, pPackage->GetFilename(), pObj);
        }
        if (!bTouching) {
            goto set_not_over;
        }
        flags = pState->Flags | 1;
    }
    goto set_flags;

set_not_over:
    flags = pState->Flags & ~1u;
set_flags:
    pState->Flags = flags;
    if (bLeftDown) {
        flags = pState->Flags | 2;
    } else {
        flags = pState->Flags & ~2u;
    }
    pState->Flags = flags;
    if (bRightDown) {
        flags = pState->Flags | 4;
    } else {
        flags = pState->Flags & ~4u;
    }
    pState->Flags = flags;
}

void FEngine::ProcessMessageQueue() {
    FEMessageNode* pNode = static_cast<FEMessageNode*>(MsgQ.RemHead());
    while (pNode) {
        if (bDebugMessages) {
            pInterface->DebugMessageProcessed(pNode->MsgID, pNode->pMsgTarget, pNode->pMsgFrom, pNode->pFromPackage, pNode->ControlMask);
        }
        FEObject* pTarget = pNode->pMsgTarget;
        unsigned long target = reinterpret_cast<unsigned long>(pTarget);
        switch (target) {
        case 0: {
            for (FEPackage* pPack = PackList.GetFirstPackage(); pPack; pPack = pPack->GetNext()) {
                ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                FEMsgTargetList* pTargets = pPack->GetMessageTargets(pNode->MsgID);
                if (pTargets) {
                    unsigned long Count = pTargets->Count;
                    unsigned long i = 0;
                    unsigned long MsgID = pNode->MsgID;
                    while (i < Count) {
                        ProcessObjectMessage(pTargets->pTargets[i], pPack, MsgID, pNode->ControlMask);
                        i++;
                    }
                }
            }
            break;
        }
        case 0xFFFFFFFF:
            pInterface->NotificationMessage(pNode->MsgID, pNode->pMsgFrom, pNode->ControlMask, reinterpret_cast<unsigned long>(pNode->pFromPackage));
            break;
        case 0xFFFFFFFE:
            for (FEPackage* pPack = PackList.GetFirstPackage(); pPack; pPack = pPack->GetNext()) {
                ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
            }
            break;
        case 0xFFFFFFFD:
            ProcessGlobalMessage(pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
            break;
        case 0xFFFFFFFC: {
            FEPackage* pPack = PackList.GetFirstPackage();
            while (pPack) {
                if (pPack == pNode->pFromPackage)
                    break;
                pPack = pPack->GetNext();
            }
            if (pPack) {
                ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                FEMsgTargetList* pTargets = pPack->GetMessageTargets(pNode->MsgID);
                if (pTargets) {
                    unsigned long Count = pTargets->Count;
                    unsigned long i = 0;
                    unsigned long MsgID = pNode->MsgID;
                    while (i < Count) {
                        ProcessObjectMessage(pTargets->pTargets[i], pPack, MsgID, pNode->ControlMask);
                        i++;
                    }
                }
            }
            break;
        }
        case 0xFFFFFFFB:
            pInterface->NotifySoundMessage(pNode->MsgID, pNode->pMsgFrom, pNode->ControlMask, reinterpret_cast<unsigned long>(pNode->pFromPackage));
            break;
        case 0xFFFFFFFA:
            if (pNode->MsgID == 0x59bed120) {
                SetProcessInput(pNode->pFromPackage, false);
            } else if (pNode->MsgID == 0x5d4ce32d) {
                SetProcessInput(pNode->pFromPackage, true);
            }
            break;
        default:
            ProcessObjectMessage(pTarget, pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
            break;
        }
        delete pNode;
        pNode = static_cast<FEMessageNode*>(MsgQ.RemHead());
    }
}

void FEngine::ProcessResponses(FEMessageResponse* pRespList, FEObject* pObj, FEPackage* pPack, unsigned long uControlMask) {
    unsigned long NumActions = pRespList->Count;
    for (unsigned long i = 0; i < NumActions; i++) {
        unsigned long Action = pRespList->pResponseList[i].ResponseID;
        FEResponse* pAction = &pRespList->pResponseList[i];
        switch (Action) {
        case 0:
            if (pObj) {
                FEScript* pScript = pObj->FindScript(pAction->ResponseParam);
                if (pScript) {
                    pObj->SetCurrentScript(pScript);
                    pScript->CurTime = 0;
                }
            }
            break;
        case 1: {
            FEObject* pTo = reinterpret_cast<FEObject*>(pAction->ResponseTarget);
            if (reinterpret_cast<unsigned long>(pTo) != 0xFFFFFFFC && reinterpret_cast<unsigned long>(pTo) != 0xFFFFFFFF) {
                pTo = pPack->FindObjectByGUID(pAction->ResponseTarget);
            }
            QueueMessage(pAction->ResponseParam, pObj, pPack, pTo, uControlMask);
            break;
        }
        case 2:
            QueueMessage(pAction->ResponseParam, pObj, pPack, reinterpret_cast<FEObject*>(0xFFFFFFFF), uControlMask);
            break;
        case 3:
            QueueMessage(pAction->ResponseParam, pObj, pPack, reinterpret_cast<FEObject*>(0xFFFFFFFB), uControlMask);
            break;
        case 0x100: {
            FEObject* pButton = nullptr;
            if (pAction->ResponseParam != 0) {
                pButton = pPack->FindObjectByGUID(pAction->ResponseParam);
            }
            bool bFound = pButton != nullptr;
            if (bFound || pAction->ResponseParam == 0) {
                pPack->SetCurrentButton(pButton, bFound);
            }
            break;
        }
        case 0x101:
            SetProcessInput(pPack, pAction->ResponseParam == 1);
            break;
        case 0x102:
            if (pPack->pCurrentButton) {
                RecordLastPackageButton(pPack->nameHash, pPack->pCurrentButton->GUID);
            } else {
                RecordLastPackageButton(pPack->nameHash, 0);
            }
            break;
        case 0x103: {
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
                    break;
                }
            }
            bFound = bFound;
            pPack->SetCurrentButton(pButton, bFound);
            break;
        }
        case 0x104:
            break;
        case 0x105:
            QueuePackageUserTransfer(pPack, true, uControlMask);
            break;
        case 0x106:
            QueuePackageUserTransfer(pPack, true, 0xFF);
            break;
        case 0x107:
            QueuePackageUserTransfer(pPack, false, uControlMask);
            break;
        case 0x108:
            QueuePackageUserTransfer(pPack, false, 0xFF);
            break;
        case 0x200:
            QueuePackageSwitch(reinterpret_cast<const char*>(pAction->ResponseParam), pPack->Controllers);
            break;
        case 0x201:
            QueuePackagePush(reinterpret_cast<const char*>(pAction->ResponseParam), pPack->Controllers);
            break;
        case 0x202: {
            unsigned long pad = 0;
            do {
                if (uControlMask & (1 << pad)) {
                    QueuePackagePush(reinterpret_cast<const char*>(pAction->ResponseParam), uControlMask);
                }
                pad++;
            } while (pad < 8);
            break;
        }
        case 0x204:
            QueuePackagePush(reinterpret_cast<const char*>(pAction->ResponseParam), 0);
            break;
        case 0x203:
            QueuePackagePop();
            break;
        case 0x2c0:
            RecordPackageMarker(pPack->name);
            break;
        case 0x2c1: {
            const char* pMarker = RecallPackageMarker();
            if (pMarker) {
                QueuePackageSwitch(pMarker, pPack->Controllers);
            }
            break;
        }
        case 0x2c2:
            ClearPackageMarkers();
            break;
        case 0x300:
            if (pObj->pCurrentScript->ID != pAction->ResponseParam) {
                i = pRespList->FindConditionBranchTarget(i);
            }
            break;
        case 0x301:
            if (pObj->pCurrentScript->ID == pAction->ResponseParam) {
                i = pRespList->FindConditionBranchTarget(i);
            }
            break;
        case 0x500:
            i = pRespList->FindConditionBranchTarget(i);
            break;
        }
    }
}

void FEngine::ProcessPackageCommands() {
    FEPackage* pFixParentLink = nullptr;
    FEPackage* pNewParentLink = nullptr;

    do {
        FEPackageCommand* pNode = static_cast<FEPackageCommand*>(PackageCommands.RemHead());
        if (!pNode) {
            return;
        }

        int Level;
        if (pNode->pPackage) {
            Level = pNode->pPackage->Priority;
        } else {
            FEPackage* pPack = FindPackageWithControl();
            pNode->pPackage = pPack;
            if (pPack) {
                Level = pPack->Priority;
                pPack->OldControllers = pPack->Controllers;
                pNode->pPackage->Controllers = 0;
            } else {
                Level = -1;
            }
        }

        if (pNode->iCommand & 1) {
            if (Level >= 0) {
                if (!(pNode->iCommand & 2)) {
                    PackList.ReplaceParentLinks(pNode->pPackage, pNode->pPackage->pParentPackage);
                } else {
                    pFixParentLink = pNode->pPackage;
                    pNewParentLink = pNode->pPackage->pParentPackage;
                }
                FEPackage* pParent = pNode->pPackage->pParentPackage;
                if (pParent) {
                    pParent->Controllers = pParent->OldControllers;
                }
                UnloadPackage(pNode->pPackage);
                Level--;
            }
        }

        if (pNode->iCommand & 2) {
            FEPackage* pPushed = PushPackage(pNode->GetName(),
                static_cast<unsigned char>(Level + 1), pNode->uControlMask);
            if (pPushed && !(pNode->iCommand & 1) && Level >= 0) {
                pPushed->pParentPackage = pNode->pPackage;
            } else if (pNode->iCommand & 1) {
                pPushed->pParentPackage = pNewParentLink;
                PackList.ReplaceParentLinks(pFixParentLink, pPushed);
            }
        }

        if (pNode->iCommand & 4) {
            FEPackage* pPack = pNode->pPackage;
            FEPackage* pParent = pPack->pParentPackage;
            if (pParent) {
                unsigned long PassedMask = pPack->Controllers & pNode->uControlMask;
                pPack->Controllers &= ~PassedMask;
                pParent->Controllers |= PassedMask;
                QueueMessage(0x334c5493u, nullptr, pParent,
                    reinterpret_cast<FEObject*>(0xFFFFFFFCu), pNode->uControlMask);
            }
        }

        if (pNode->iCommand & 8) {
            FEPackage* pChild = PackList.GetFirstPackage();
            while (pChild) {
                if (pChild->pParentPackage == pNode->pPackage) {
                    if (pChild) {
                        unsigned long PassedMask = pNode->pPackage->Controllers & pNode->uControlMask;
                        pNode->pPackage->Controllers &= ~PassedMask;
                        pChild->Controllers |= PassedMask;
                        QueueMessage(0x334c5493u, nullptr, pChild,
                            reinterpret_cast<FEObject*>(0xFFFFFFFCu), pNode->uControlMask);
                    }
                    break;
                }
                pChild = pChild->GetNext();
            }
        }

        delete pNode;
    } while (true);
}
