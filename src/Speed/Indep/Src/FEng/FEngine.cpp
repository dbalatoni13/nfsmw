#include "Speed/Indep/Src/FEng/fengine.h"
#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"

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
};

// total size: 0x20
struct FEPackageCommand : public FENode {
    int iCommand;               // offset 0x14, size 0x4
    unsigned long uControlMask; // offset 0x18, size 0x4
    FEPackage* pPackage;        // offset 0x1C, size 0x4
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

FEPackage* FEngine::FindQueuedNodeWithControl() {
    FEPackageCommand* pCmd = static_cast<FEPackageCommand*>(PackageCommands.GetTail());
    while (pCmd) {
        if (pCmd->iCommand & 2) {
            return pCmd->pPackage;
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
