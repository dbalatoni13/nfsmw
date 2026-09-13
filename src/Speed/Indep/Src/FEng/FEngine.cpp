#include "Speed/Indep/Src/FEng/FEngine.h"
#include "Speed/Indep/Src/FEng/FEKeyTrack.h"
#include "Speed/Indep/Src/FEng/FEMessageNode.h"
#include "Speed/Indep/Src/FEng/FEMessageResponse.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEPackageReader.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"

extern "C" int printf(const char *, ...);

static const u32 FEAutoRepeatFrames = 6;       // size: 0x4, Decl: 26
static const u32 FEAutoRepeatFirstFrames = 16; // size: 0x4, Decl: 27

static const u32 Msg_Global_DisableInputs = __DISABLE_ALL_INPUTS__; // size: 0x4, Decl: 29
static const u32 Msg_Global_EnableInputs = __ENABLE_ALL_INPUTS__;   // size: 0x4, Decl: 30

// total size: 0x4
// Decl: 33
typedef struct {
    u8 Index1, Index2; // offset 0x0, size 0x1, Decl: 34
    u16 Dir;           // offset 0x2, size 0x2, Decl: 35
} PadDirImpulse;

static PadDirImpulse ImpulseDir[8] = {
    // size: 0x20, address: 0x8041D080, Decl: 38
    {0x00, 0x02, 0x0007}, {0x00, 0x03, 0x0001}, {0x01, 0x02, 0x0005}, {0x01, 0x03, 0x0003},
    {0x00, 0xFF, 0x0000}, {0x02, 0xFF, 0x0006}, {0x01, 0xFF, 0x0004}, {0x03, 0xFF, 0x0002},
};

// total size: 0x4
// Decl: 674
class FEngSetDirtyFlagsCallback : public FEObjectCallback {
  private:
    bool Callback(FEObject *pObj) override {} // Decl: 675
};

FEngine::FEngine() {
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

void FEngine::SetNumJoyPads(u8 Count) {
    if (pJoyPad != nullptr) {
        delete[] pJoyPad;
    }

    if (Count) {
        pJoyPad = FNEW FEJoyPad[Count];
    }

    NumJoyPads = Count;
    FEngMemSet(HoldDecrement, 0, sizeof(HoldDecrement));
}

void FEngine::SetExecution(bool bProcessEverything) {
    FEPackage *pPackage = PackList.GetFirstPackage();
    bExecuting = bProcessEverything;
    while (pPackage != nullptr) {
        pPackage->SetExecute(bExecuting);
        pPackage = pPackage->GetNext();
    }
}

void FEngine::SetProcessInput(FEPackage *pkg, bool bProcess) {
    if (pkg == nullptr) {
        return;
    }
    pkg->SetInputEnabled(bProcess);
}

void FEngine::SetInitialState() {
    PackageInitStateCB InitState;
    ForAllObjects(InitState);
    for (u8 i = 0; i < NumJoyPads; i++) {
        pJoyPad[i].Reset();
    }

    if (bMouseActive) {
        Mouse.Reset();
    }
}

FEPackage *FEngine::LoadPackage(const void *pPackageData, bool bLoadAsLibrary) {
    FEPackage *pPack;
    FEPackageReader Reader;
    pPack = Reader.Load(pPackageData, pInterface, this, bLoadObjectNames, bLoadScriptNames, bLoadAsLibrary);
    if (pPack == nullptr) {
        return nullptr;
    }
    return pPack;
}

bool FEngine::UnloadPackage(FEPackage *pPackage) {
    FEPackage *pPack = PackList.GetFirstPackage();
    while (pPack != nullptr) {
        if (pPackage == pPack) {
            bool bDelete;
            if (pInterface != nullptr) {
                bDelete = pInterface->PackageWillUnload(pPack);
            } else {
                bDelete = true;
            }
            PackList.RemovePackage(pPackage);
            FEPackageCommand *pTempNode = static_cast<FEPackageCommand *>(PackageCommands.GetHead());
            while (pTempNode != nullptr) {
                FEPackageCommand *pNextNode = static_cast<FEPackageCommand *>(pTempNode->GetNext());
                if (pTempNode->pPackage == pPackage) {
                    PackageCommands.RemNode(pTempNode);
                    if (pTempNode != nullptr) {
                        delete pTempNode;
                    }
                }
                pTempNode = pNextNode;
            }
            if (pPack->UsesIdleList()) {
                AddToIdleList(pPackage);
            } else {
                FENode *pLibName = static_cast<FENode *>(pPack->GetLibraryList().GetHead());
                while (pLibName != nullptr) {
                    FEPackage *pLibPack = FindLibraryPackage(pLibName->GetNameHash());
                    if (pLibPack != nullptr) {
                        i32 Pri = pLibPack->GetPriority() - 1;
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
                    if (pPack != nullptr) {
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

// UNSOLVED (regswap)
void FEngine::UnloadLibraryPackage(FEPackage *pLibPack) {
    if (!pInterface->UnloadUnreferencedLibrary()) {
        return;
    }
    RemoveFromLibraryList(pLibPack);
    bool bDelete;
    if (pInterface != nullptr) {
        bDelete = pInterface->PackageWillUnload(pLibPack);
    } else {
        bDelete = true;
    }
    pLibPack->Shutdown(pInterface);
    if (bDelete && (pLibPack != nullptr)) {
        delete pLibPack;
    }
}

FEPackage *FEngine::PushPackage(const char *pPackageName, const u8 Level, const u32 ControlMask) {
    FEPackage *pPack = FindIdlePackage(pPackageName);
    bool bDeleteBlock;
    u8 *pBlockStart;
    if (pPack == nullptr) {
        char length = static_cast<char>(FEngStrLen(pPackageName));
        char *s = const_cast<char *>(pPackageName + length - 1);
        while (*s != '/' && *s != '\\' && length > 0) {
            s--;
            length--;
        }
        if (length != 0) {
            s++;
        }
        pPack = FindIdlePackage(s);
    }
    if (pPack != nullptr) {
        PackageInitStateCB InitCB;
        pPack->SetUseIdleList(true);
        pPack->ForAllObjects(InitCB);
        IdleList.RemNode(pPack);
    } else {
        u8 *pData = pInterface->GetPackageData(pPackageName, &pBlockStart, bDeleteBlock);
        if (pData == nullptr) {
            return nullptr;
        }
        pPack = LoadPackage(pData, false);
        if (bDeleteBlock && (pBlockStart != nullptr)) {
            delete[] pBlockStart;
        }
        if (pPack == nullptr) {
            return nullptr;
        }
    }
    pPack->SetControlMask(ControlMask);
    pPack->SetPriority(Level);
    pPack->SetExecute(bExecuting);
    if (pInterface != nullptr) {
        pInterface->PackageWasLoaded(pPack);
    }
    PackList.AddPackage(pPack);
    return pPack;
}

void FEngine::AddToIdleList(FEPackage *pPack) {
    IdleList.AddTail(pPack);
}

FEPackage *FEngine::FindIdlePackage(const char *pName) const {
    return static_cast<FEPackage *>(IdleList.FindNode(pName));
}

FEPackage *FEngine::GetFirstLibrary() const {
    return static_cast<FEPackage *>(LibraryList.GetHead());
}

void FEngine::AddToLibraryList(FEPackage *pPack) {
    LibraryList.AddTail(pPack);
}

void FEngine::RemoveFromLibraryList(FEPackage *pPack) {
    LibraryList.RemNode(pPack);
}

FEPackage *FEngine::FindLibraryPackage(u32 NameHash) const {
    FEPackage *pNode = GetFirstLibrary();
    while (pNode != nullptr) {
        if (FEHashUpper(pNode->GetFilename() + 2) == NameHash) {
            return pNode;
        }
        pNode = pNode->GetNext();
    }
    return nullptr;
}

// UNSOLVED
void FEngine::Update(const i32 tDeltaTicks, uint32 lock) {
    FEPackage *pPackage;
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
        for (u8 PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
            pJoyPad[PadIndex].Update(pInterface->GetJoyPadMask(PadIndex), tDeltaTicks);
        }
        for (pPackage = PackList.GetFirstPackage(); pPackage != nullptr; pPackage = pPackage->GetNext()) {
            if (pPackage->IsInputEnabled() && (!bErrorScreenMode || pPackage->IsErrorScreen())) {
                ProcessPadsForPackage(pPackage);
                if (bMouseActive) {
                    ProcessMouseForPackage(pPackage);
                }
            }
        }
        u32 i = 0;
        u32 MaskBit = 1;
        do {
            if ((PadHoldRegistered & MaskBit) != 0) {
                for (u8 PadIdx = 0; PadIdx < NumJoyPads; PadIdx++) {
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
            iIterationTicks = 0;
            while (pPackage != nullptr) {
                FEPackage *pCachedNext = pPackage->GetNext();
                if (!bErrorScreenMode || pPackage->IsErrorScreen()) {
                    pPackage->Update(this, iTicksRemaining);
                }
                pPackage = pCachedNext;
            }
            ProcessMessageQueue();
            if (!bErrorScreenMode) {
                ProcessPackageCommands();
            }
            if (MsgQ.GetHead() != nullptr) {
                ProcessMessageQueue();
            }
            bRenderedRecently = false;
            iTicksRemaining = iIterationTicks;
        } while (iTicksRemaining);
    } else {
        for (pPackage = PackList.GetFirstPackage(); pPackage != nullptr; pPackage = pPackage->GetNext()) {
            if (!bErrorScreenMode || pPackage->IsErrorScreen()) {
                pPackage->Update(this, tDeltaTicks);
            }
        }
    }
    if (bDebugMessages) {
        pInterface->DebugMessageEndUpdate();
    }
}

// size: 0x4C, address: 0x8041D0A0, Decl: 873
static u32 PadButtonHash[19] = {
    0x00000000,       0x00000000,       0x00000000,      0x00000000,      __PAD_ACCEPT__,  __PAD_BACK__,    __PAD_START__,
    __PAD_LTRIGGER__, __PAD_RTRIGGER__, __PAD_BUTTON0__, __PAD_BUTTON1__, __PAD_BUTTON2__, __PAD_BUTTON3__, __PAD_BUTTON4__,
    __PAD_BUTTON5__,  __PAD_BUTTON6__,  __PAD_BUTTON7__, __PAD_BUTTON8__, __PAD_BUTTON9__,
};

// size: 0x8, address: 0x8041D0EC, Decl: 881
static u32 PadButtonHeldHash[2] = {
    __PAD_LTRIGGER_HELD__,
    __PAD_RTRIGGER_HELD__,
};

// size: 0x4C, address: 0x8041D0F4, Decl: 886
static u32 PadReleasedHash[19] = {
    0x00000000,
    0x00000000,
    0x00000000,
    0x00000000,
    __PAD_ACCEPT_RELEASED__,
    __PAD_BACK_RELEASED__,
    __PAD_START_RELEASED__,
    __PAD_LTRIGGER_RELEASED__,
    __PAD_RTRIGGER_RELEASED__,
    __PAD_BUTTON0_RELEASED__,
    __PAD_BUTTON1_RELEASED__,
    __PAD_BUTTON2_RELEASED__,
    __PAD_BUTTON3_RELEASED__,
    __PAD_BUTTON4_RELEASED__,
    __PAD_BUTTON5_RELEASED__,
    __PAD_BUTTON6_RELEASED__,
    __PAD_BUTTON7_RELEASED__,
    __PAD_BUTTON8_RELEASED__,
    __PAD_BUTTON9_RELEASED__,
};

// UNSOLVED
void FEngine::ProcessPadsForPackage(FEPackage *pPackage) {
    u32 Pressed;
    u32 Released;
    u32 Held;
    u32 Mask;
    u32 HeldFor[19];
    u8 FromPadHeld[19];
    u8 FromPadPressed[19];
    u8 FromPadReleased[19];
    u8 PadIndex;
    u32 i;
    u32 JoyMask;
    bool bSomethingActive;

    JoyMask = pPackage->GetControlMask();
    if (JoyMask == 0) {
        return;
    }

    bSomethingActive = false;
    for (PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
        if ((JoyMask & (1 << PadIndex)) != 0) {
            bSomethingActive = bSomethingActive | pJoyPad[PadIndex].WasActive();
        }
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
        FEObject *pCurButton;
        JoyMask = pPackage->GetControlMask();
        Mask = 1 << i;
        Pressed = 0;
        Released = 0;
        Held = 0;

        {
            for (unsigned char PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
                if ((JoyMask & (1 << PadIndex)) != 0) {
                    if (pJoyPad[PadIndex].WasPressed(Mask)) {
                        Pressed = Pressed | Mask;
                        FromPadPressed[i] = FromPadPressed[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                    if (pJoyPad[PadIndex].WasReleased(Mask)) {
                        Released = Released | Mask;
                        FromPadReleased[i] = FromPadReleased[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                    if (pJoyPad[PadIndex].WasHeld(Mask)) {
                        Held = Held | Mask;
                        HeldFor[i] = HeldFor[i] > pJoyPad[PadIndex].HeldFor(Mask) ? HeldFor[i] : pJoyPad[PadIndex].HeldFor(Mask);
                        FromPadHeld[i] = FromPadHeld[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                }
            }
        }

        if (i == 4 && pPackage->StartEqualsAccept()) {
            for (unsigned char PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
                if ((JoyMask & (1 << PadIndex)) != 0) {
                    if (pJoyPad[PadIndex].WasPressed(0x40)) {
                        Pressed = Pressed | Mask;
                        FromPadPressed[i] = FromPadPressed[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                    if (pJoyPad[PadIndex].WasReleased(0x40)) {
                        Released = Released | Mask;
                        FromPadReleased[i] = FromPadReleased[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                    if (pJoyPad[PadIndex].WasHeld(0x40)) {
                        Held = Held | Mask;
                        HeldFor[i] = HeldFor[i] > pJoyPad[PadIndex].HeldFor(0x40) ? HeldFor[i] : pJoyPad[PadIndex].HeldFor(0x40);
                        FromPadHeld[i] = FromPadHeld[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                }
            }
        }

        if ((Held | Released | Pressed) != 0) {
            pCurButton = pPackage->GetCurrentButton();

            if (i > 8)
                goto outer_range;
            if (i >= 7)
                goto held_handler;
            if (i == 4)
                goto accept_handler;
            if (i < 4)
                goto check_released;
            goto default_press;

        outer_range:
            if (i > 18)
                goto check_released;
            goto default_press;

        accept_handler:
            if ((Pressed & 0x10) == 0)
                goto check_released;
            HeldButtons[4] = pCurButton;
            if ((pCurButton != nullptr) && pCurButton->FindResponse(__BUTTON_PRESSED__) != nullptr) {
                QueueMessage(__BUTTON_PRESSED__, nullptr, pPackage, pPackage->GetCurrentButton(), FromPadPressed[4]);
                QueueMessage(__BUTTON_PRESSED__, pPackage->GetCurrentButton(), pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), FromPadPressed[4]);
            } else if (pPackage->FindResponse(__PAD_ACCEPT__) != nullptr) {
                QueueMessage(__PAD_ACCEPT__, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), FromPadPressed[4]);
                QueueMessage(__PAD_ACCEPT__, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), FromPadPressed[4]);
            }
            goto check_released;

        held_handler:
            if ((Held & Mask) != 0) {
                u32 PadMask = FromPadPressed[i];
                u32 MsgID = PadButtonHeldHash[i - 7];
                if ((pCurButton != nullptr) && pCurButton->FindResponse(MsgID) != nullptr) {
                    QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                    QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
                } else if (pPackage->FindResponse(MsgID) != nullptr) {
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), PadMask);
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
                }
            }

        default_press:
            if ((Pressed & Mask) != 0) {
                u32 PadMask = FromPadPressed[i];
                HeldButtons[i] = pCurButton;
                u32 MsgID = PadButtonHash[i];
                if ((pCurButton != nullptr) && pCurButton->FindResponse(MsgID) != nullptr) {
                    QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                    QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
                } else if (pPackage->FindResponse(MsgID) != nullptr) {
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), PadMask);
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
                }
            }

        check_released:
            if ((Released & Mask) != 0) {
                u32 PadMask = FromPadReleased[i];
                u32 MsgID = PadReleasedHash[i];
                if (HeldButtons[i] == pCurButton && pCurButton != nullptr) {
                    HeldButtons[i] = nullptr;
                    if (i == 4) {
                        MsgID = 0x936A6A7Fu;
                    }
                    if (pCurButton->FindResponse(MsgID) != nullptr) {
                        QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                        QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
                    }
                }
                PadMask = FromPadReleased[i];
                if (pPackage->FindResponse(MsgID) != nullptr) {
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), PadMask);
                    QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
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
        Mask = 1 << i;
        {
            for (unsigned char PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
                if ((JoyMask & (1 << PadIndex)) != 0) {
                    if (pJoyPad[PadIndex].WasPressed(Mask)) {
                        Pressed = Pressed | Mask;
                        FromPadPressed[i] = FromPadPressed[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                    pJoyPad[PadIndex].WasReleased(Mask);
                    if (pJoyPad[PadIndex].WasHeld(Mask)) {
                        HeldFor[i] = HeldFor[i] > pJoyPad[PadIndex].HeldFor(Mask) ? HeldFor[i] : pJoyPad[PadIndex].HeldFor(Mask);
                        FromPadHeld[i] = FromPadHeld[i] | static_cast<unsigned char>(1 << PadIndex);
                    }
                }
            }
        }
        i = i + 1;
    }

    i = 0;
    {
        u32 Result;
        u32 Compare;
        u32 JustPressed;
        u32 PadMask;
        FEObject *pCurButton;
        while (true) {
            if (i > 7) {
                return;
            }
            if (!pPackage->IsInputEnabled()) {
                return;
            }

            pCurButton = pPackage->GetCurrentButton();
            if (ImpulseDir[i].Index2 != 0xFF) {
                Result = HeldFor[ImpulseDir[i].Index2];
                if (HeldFor[ImpulseDir[i].Index1] < HeldFor[ImpulseDir[i].Index2]) {
                    Result = HeldFor[ImpulseDir[i].Index1];
                }
                JustPressed = (Pressed >> ImpulseDir[i].Index1) & (Pressed >> ImpulseDir[i].Index2);
                PadMask = (FromPadPressed[ImpulseDir[i].Index1] & FromPadPressed[ImpulseDir[i].Index2]) |
                          (FromPadHeld[ImpulseDir[i].Index1] & FromPadHeld[ImpulseDir[i].Index2]);
            } else {
                JustPressed = Pressed >> ImpulseDir[i].Index1;
                Result = HeldFor[ImpulseDir[i].Index1];
                PadMask = FromPadPressed[ImpulseDir[i].Index1] | FromPadHeld[ImpulseDir[i].Index1];
            }

            Compare = FEFramesToTicks(20);
            if ((FastRep & (1 << i)) != 0) {
                Compare = 0x78;
            }
            if (Compare <= Result) {
                break;
            }
            if ((JustPressed & 1) == 0) {
                if (Result == 0) {
                    FastRepCache = FastRepCache & ~(1 << i);
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
            FastRepCache = FastRepCache | (1 << i);
        }
        HoldDecrement[ImpulseDir[i].Index1] = Compare;
        if (ImpulseDir[i].Index2 != 0xFF) {
            HoldDecrement[ImpulseDir[i].Index2] = Compare;
            HeldFor[ImpulseDir[i].Index1] = 0;
            HeldFor[ImpulseDir[i].Index2] = 0;
            PadHoldRegistered = PadHoldRegistered | (1 << ImpulseDir[i].Index1) | (1 << ImpulseDir[i].Index2);
            goto fire_direction;
        }
        {
            HeldFor[ImpulseDir[i].Index1] = 0;
            PadHoldRegistered = PadHoldRegistered | (1 << ImpulseDir[i].Index1);
        }

    fire_direction:
        if (pCurButton) {
            FEObject *pNewButton = nullptr;
            u32 MsgID = FEDirection_Message[ImpulseDir[i].Dir];
            FEMessageResponse *pResponse = pCurButton->FindResponse(MsgID);
            if (pResponse) {
                QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                if ((pCurButton->Flags & FF_DontNavigate) == 0) {
                    if (pResponse->FindResponse(0x104) == -1) {
                        pNewButton = pPackage->GetButtonMap()->GetButtonFrom(pCurButton, ImpulseDir[i].Dir, pInterface, WrapMode);
                    }
                }
            } else {
                if ((pCurButton->Flags & FF_DontNavigate) == 0) {
                    pNewButton = pPackage->GetButtonMap()->GetButtonFrom(pCurButton, ImpulseDir[i].Dir, pInterface, WrapMode);
                }
                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), PadMask);
            }
            QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
            if (pNewButton != nullptr) {
                for (u32 j = 4; j < 19; j++) {
                    if (HeldButtons[j] != nullptr && pCurButton != nullptr) {
                        u32 PadMask;
                        u32 MsgID;
                        HeldButtons[j] = nullptr;
                        PadMask = FromPadReleased[j];
                        MsgID = PadReleasedHash[j];
                        if (j == 4) {
                            MsgID = 0x936A6A7Fu;
                        }
                        if (pCurButton->FindResponse(MsgID) != nullptr) {
                            QueueMessage(MsgID, nullptr, pPackage, pCurButton, PadMask);
                            QueueMessage(MsgID, pCurButton, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
                        }
                    }
                }
                pPackage->SetCurrentButton(pNewButton, true);
            }
        } else {
            u32 MsgID = FEDirection_Message[ImpulseDir[i].Dir];
            if (pPackage->FindResponse(MsgID) != nullptr) {
                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), PadMask);
                QueueMessage(MsgID, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), PadMask);
            }
        }
    }
}

// UNSOLVED
void FEngine::UpdateMouseState(FEPackage *pkg, FEObjectMouseState *state, float mx, float my) {
    FEObject *obj = state->pObject;
    if (obj && (obj->Flags & (FF_IgnoreButton | FF_IsButton)) == (FF_IgnoreButton | FF_IsButton)) {
        return;
    }
    float objX, objY;
    FEngGetCenter(obj, objX, objY);
    bool is_mouse_over = pInterface->DoesPointTouchObject(mx - (state->Offset.h - objX), my - (state->Offset.v - objY), obj);
    bool is_left_down = Mouse.IsDown(1);
    bool is_right_down = Mouse.IsDown(2);
    bool was_mouse_over = state->GetBit(1);
    bool was_mouse_left_down = state->GetBit(2);
    bool was_mouse_right_down = state->GetBit(4);

    if (is_mouse_over) {
        unsigned int msg = 0x13f4bd45;
        if (was_mouse_over) {
            msg = 0xb30d0683;
        }
        cFEng::Get()->QueuePackageMessage(msg, pkg->name, obj);
    } else {
        if (was_mouse_over) {
            cFEng::Get()->QueuePackageMessage(0xb30793c1, pkg->name, obj);
        }
    }

    if (is_left_down) {
        if (was_mouse_left_down) {
            cFEng::Get()->QueuePackageMessage(0x1e646b2e, pkg->name, obj);
        } else {
            if (!is_mouse_over) {
                goto skip_left;
            }
            cFEng::Get()->QueuePackageMessage(0xf459b307, pkg->name, obj);
        }
    } else {
        if (was_mouse_left_down && is_mouse_over) {
            cFEng::Get()->QueuePackageMessage(0x7eabca56, pkg->name, obj);
        }
    }
skip_left:

    if (is_right_down) {
        if (was_mouse_right_down) {
            cFEng::Get()->QueuePackageMessage(0x0da2f4e1, pkg->name, obj);
        } else if (is_mouse_over) {
            cFEng::Get()->QueuePackageMessage(0xce59c3da, pkg->name, obj);
        } else {
            goto set_bits;
        }
        if (!is_mouse_over) {
            goto set_bits;
        }
    } else {
        if (was_mouse_right_down) {
            if (!is_mouse_over) {
                goto set_bits;
            }
            cFEng::Get()->QueuePackageMessage(0x98adf589, pkg->name, obj);
        }
        if (!is_mouse_over) {
            goto set_bits;
        }
    }
set_bits:
    state->SetBit(FEMouseFlag_MouseOver, is_mouse_over);
    state->SetBit(FEMouseFlag_MouseLeftPressed, is_left_down);
    state->SetBit(FEMouseFlag_MouseRightPressed, is_right_down);
}

void FEngine::ProcessMouseForPackage(FEPackage *pPackage) {
    u32 JoyMask = pPackage->GetControlMask();

    if (JoyMask == 0 || !(JoyMask & 1) || !pPackage->IsInputEnabled()) {
        return;
    }

    float fMouseX = Mouse.GetXPos();
    float fMouseY = Mouse.GetYPos();
    int num_mouseables = pPackage->GetNumMouseObjects();
    FEObjectMouseState *the_states = pPackage->GetMouseObjectStates();
    for (int j = 0; j < num_mouseables; j++) {
        UpdateMouseState(pPackage, the_states + j, fMouseX, fMouseY);
    }
}

void FEngine::Render() {
    FEPackage *pPack;
    FEObject *pObj;
    ProfileNode profile_node2("FEngine::Render()", 0);
    ProfileNode profile_node;
    profile_node.Begin("Setup", 0);
    FEMatrix4 stView;
    FEMatrix4 stParent;
    stParent.Identify();
    pInterface->GetViewTransformation(&stView);
    int pkg = 0;
    FEPackage *hack[32];
    FEPackage *z;

    for (z = PackList.GetFirstPackage(); z != nullptr; z = z->GetNext()) {
        hack[pkg] = z;
        pkg++;
    }
    for (int i = 0; i < pkg; i++) {
        PackList.RemovePackage(hack[i]);
    }
    for (int i = 0; i < pkg; i++) {
        PackList.AddPackage(hack[i]);
    }

    uGroupContext = 0;
    pPack = PackList.GetFirstPackage();
    while (pPack != nullptr) {
        pInterface->BeginPackageRendering(pPack);
        Sorter.Zero();
        profile_node.Begin("Object traversal", 0);
        for (pObj = pPack->GetFirstObject(); pObj != nullptr; pObj = pObj->GetNext()) {
            if (pObj->Type == FE_Group) {
                RenderGroup(static_cast<FEGroup *>(pObj), stParent, stView, 0);
            } else {
                RenderObject(pObj, stView, 0);
            }
        }
        profile_node.Begin("SortObjects()", 0);
        Sorter.SortObjects();
        profile_node.Begin("RenderObjectList()", 0);
        pInterface->RenderObjectList(reinterpret_cast<FEObjectListEntry *>(Sorter.GetListPtr()), Sorter.GetNumObjects());
        pInterface->EndPackageRendering(pPack);
        pPack = pPack->GetNext();
    }
    bRenderedRecently = bExecuting;
}

void FEngine::RenderGroup(FEGroup *pGroup, FEMatrix4 &stParent, FEMatrix4 &stView, u16 uContext) {
    FEObject *pObj;
    FEMatrix4 stTemp;
    FEMatrix4 stContext;
    FEMatrix4 stContextView;
    FEVector3 stOffset;
    FEVector3 stPivot;
    FEObjData *pData = pGroup->GetObjData();

    if (pData->Col.a == 0) {
        return;
    }
    if (!bExecuting && pGroup->Flags == 0) {
        return;
    }

    pData->Rot.GetMatrix(&stTemp);
    stPivot = pData->Pivot;
    stPivot *= -1.0f;
    FEMultMatrix(&stOffset, &stTemp, &stPivot);
    stTemp.m41 = stOffset.x + pData->Pivot.x + pData->Pos.x;
    stTemp.m42 = stOffset.y + pData->Pivot.y + pData->Pos.y;
    stTemp.m43 = stOffset.z + pData->Pivot.z + pData->Pos.z;
    FEMultMatrix(&stContext, &stTemp, &stParent);
    FEMultMatrix(&stContextView, &stContext, &stView);
    uGroupContext++;
    u16 uLocalGroupContext = uGroupContext;
    pGroup->RenderContext = uContext;
    pInterface->GenerateRenderContext(uLocalGroupContext, pGroup);
    pObj = pGroup->GetFirstChild();
    while (pObj != nullptr) {
        if (pObj->Type == FE_Group) {
            RenderGroup(static_cast<FEGroup *>(pObj), stContext, stView, uLocalGroupContext);
        } else {
            RenderObject(pObj, stContextView, uLocalGroupContext);
        }
        pObj = pObj->GetNext();
    }
}

void FEngine::RenderObject(FEObject *pObj, FEMatrix4 &stView, u16 uContext) {
    FEObjData *pData = pObj->GetObjData();
    if (pData->Col.a == 0) {
        return;
    }

    FEVector3 Pivot(pData->Pivot);
    FEVector3 ResultVect;

    Pivot += pData->Pos;

    FEMultMatrix(&ResultVect, &stView, &Pivot);

    pObj->RenderContext = uContext;

    if (ResultVect.z > 0.0f) {
        Sorter.AddObject(pObj, ResultVect.z);
    }
}

bool FEngine::ForAllObjects(FEObjectCallback &Callback) {
    FEPackage *pPack = PackList.GetFirstPackage();
    while (pPack != nullptr) {
        if (!pPack->ForAllObjects(Callback)) {
            return false;
        }
        pPack = pPack->GetNext();
    }
    return true;
}

void FEngine::QueueMessage(u32 MsgID, FEObject *pFrom, FEPackage *pFromPackage, FEObject *pTo, u32 ControlMask) {
    FEMessageNode *pNode = FNEW FEMessageNode();
    pNode->MsgID = MsgID;
    pNode->pMsgFrom = pFrom;
    pNode->pFromPackage = pFromPackage;
    pNode->pMsgTarget = pTo;
    pNode->ControlMask = ControlMask;

    if (bDebugMessages) {
        pInterface->DebugMessageQueued(MsgID, pFrom, pFromPackage, pTo, ControlMask);
    }
    MsgQ.AddTail(pNode);
}

void FEngine::SendMessageToGame(u32 MsgID, FEObject *pFrom, FEPackage *pFromPackage, u32 uControlMask) {
    pInterface->NotificationMessage(MsgID, pFrom, uControlMask, reinterpret_cast<u32>(pFromPackage));
}

void FEngine::QueuePackageSwitch(const char *pPackageName, u32 ControlMask) {
    QueuePackageCommand(3, ControlMask, pPackageName);
}

void FEngine::QueuePackagePush(const char *pPackageName, u32 ControlMask) {
    QueuePackageCommand(2, ControlMask, pPackageName);
}

void FEngine::QueuePackagePop() {
    QueuePackageCommand(1, 0, nullptr);
}

void FEngine::QueuePackageCommand(i32 command, u32 ControlMask, const char *pPackageName) {
    FEPackageCommand *Node = nullptr;
    FEPackage *pPackageWithControl = FindPackageWithControl();
    FEPackageCommand *pCom = FNEW FEPackageCommand();
    pCom->iCommand = 0;
    pCom->uControlMask = 0;
    pCom->pPackage = pPackageWithControl;
    if (pPackageWithControl != nullptr) {
        if (ControlMask == 0) {
            pCom->uControlMask = pPackageWithControl->GetControlMask();
        } else {
            pCom->uControlMask = ControlMask;
        }
        pPackageWithControl->SetOldControlMask(pPackageWithControl->GetControlMask());
        pPackageWithControl->SetControlMask(0);
    } else {
        Node = FindQueuedNodeWithControl();
        if (Node != nullptr) {
            if (ControlMask == 0) {
                pCom->uControlMask = Node->uControlMask;
            } else {
                pCom->uControlMask = ControlMask;
            }
        } else {
            if (ControlMask == 0) {
                pCom->uControlMask = 0xFF;
            } else {
                pCom->uControlMask = ControlMask;
            }
        }
    }
    pCom->iCommand = command;
    pCom->SetName(pPackageName);
    PackageCommands.AddTail(pCom);
}

void FEngine::QueuePackageUserTransfer(FEPackage *pFrom, bool bToChild, u32 ControlMask) {
    printf("If you get this, come see Gary or Lolley!\n");
    FEPackageCommand *pCom = FNEW FEPackageCommand();
    pCom->iCommand = 0;
    pCom->uControlMask = 0;
    pCom->pPackage = pFrom;
    pCom->uControlMask = pFrom->GetControlMask() & ControlMask;
    pCom->iCommand = bToChild ? 8 : 4;
    PackageCommands.AddTail(pCom);
}

void FEngine::ProcessMessageQueue() {
    FEMessageNode *pNode = static_cast<FEMessageNode *>(MsgQ.RemHead());
    FEPackage *pPack;
    while (pNode != nullptr) {
        if (bDebugMessages) {
            pInterface->DebugMessageProcessed(pNode->MsgID, pNode->pMsgTarget, pNode->pMsgFrom, pNode->pFromPackage, pNode->ControlMask);
        }
        switch (reinterpret_cast<u32>(pNode->pMsgTarget)) {
            case 0: {
                pPack = PackList.GetFirstPackage();
                while (pPack != nullptr) {
                    ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                    FEMsgTargetList *pTargList = pPack->GetMessageTargets(pNode->MsgID);
                    if (pTargList != nullptr) {
                        u32 i = 0;
                        u32 MsgID = pNode->MsgID;
                        u32 Count = pTargList->GetCount();
                        while (i < Count) {
                            ProcessObjectMessage(pTargList->GetTarget(i), pPack, MsgID, pNode->ControlMask);
                            i++;
                        }
                    }
                    pPack = pPack->GetNext();
                }
                break;
            }
            case 0xFFFFFFFF:
                pInterface->NotificationMessage(pNode->MsgID, pNode->pMsgFrom, pNode->ControlMask, reinterpret_cast<u32>(pNode->pFromPackage));
                break;
            case 0xFFFFFFFE:
                pPack = PackList.GetFirstPackage();
                while (pPack != nullptr) {
                    ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                    pPack = pPack->GetNext();
                }
                break;
            case 0xFFFFFFFD:
                ProcessGlobalMessage(pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
                break;
            case 0xFFFFFFFC: {
                pPack = PackList.GetFirstPackage();
                while ((pPack != nullptr) && pPack != pNode->pFromPackage) {
                    pPack = pPack->GetNext();
                }
                if (pPack != nullptr) {
                    ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                    FEMsgTargetList *pTargList = pPack->GetMessageTargets(pNode->MsgID);
                    if (pTargList != nullptr) {
                        u32 i = 0;
                        u32 MsgID = pNode->MsgID;
                        u32 Count = pTargList->GetCount();
                        while (i < Count) {
                            ProcessObjectMessage(pTargList->GetTarget(i), pPack, MsgID, pNode->ControlMask);
                            i++;
                        }
                    }
                }
                break;
            }
            case 0xFFFFFFFB:
                pInterface->NotifySoundMessage(pNode->MsgID, pNode->pMsgFrom, pNode->ControlMask, reinterpret_cast<u32>(pNode->pFromPackage));
                break;
            case 0xFFFFFFFA: {
                switch (pNode->MsgID) {
                    case Msg_Global_DisableInputs:
                        SetProcessInput(pNode->pFromPackage, false);
                        break;
                    case Msg_Global_EnableInputs:
                        SetProcessInput(pNode->pFromPackage, true);
                        break;
                }
                break;
            }
            default:
                ProcessObjectMessage(pNode->pMsgTarget, pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
                break;
        }
        delete pNode;
        pNode = static_cast<FEMessageNode *>(MsgQ.RemHead());
    }
}

bool FEngine::ProcessListBoxResponses(FEObject *pObj, FEPackage *, u32 MsgID) {
    FEListBox *pList = static_cast<FEListBox *>(pObj);
    switch (MsgID) {
        case 0xe10c4af9:
            pList->ScrollSelection(-1, 0);
            return true;
        case 0x030471ac:
            pList->ScrollSelection(1, 0);
            return true;
        case 0xfb814f13:
            pList->ScrollSelection(0, -1);
            return true;
        case 0xe10814a6:
            pList->ScrollSelection(0, 1);
            return true;
    }
    return false;
}

bool FEngine::ProcessCodeListBoxResponses(FEObject *pObj, FEPackage *, u32 MsgID) {
    FECodeListBox *pList = static_cast<FECodeListBox *>(pObj);
    switch (MsgID) {
        case 0xe10c4af9:
            pList->ScrollSelection(-1, 0);
            return true;
        case 0x030471ac:
            pList->ScrollSelection(1, 0);
            return true;
        case 0xfb814f13:
            pList->ScrollSelection(0, -1);
            return true;
        case 0xe10814a6:
            pList->ScrollSelection(0, 1);
            return true;
    }
    return false;
}

void FEngine::ProcessObjectMessage(FEObject *pObj, FEPackage *pPack, u32 MsgID, u32 ControlMask) {
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
    FEMessageResponse *pRespList = pObj->FindResponse(MsgID);
    if (pRespList != nullptr) {
        ProcessResponses(pRespList, pObj, pPack, ControlMask);
    }
}

void FEngine::ProcessGlobalMessage(FEPackage *pPack, u32 MsgID, u32 ControlMask) {
    FEMessageResponse *pRespList = pPack->FindResponse(MsgID);
    if (pRespList != nullptr) {
        ProcessResponses(pRespList, nullptr, pPack, ControlMask);
    }
}

// UNSOLVED
void FEngine::ProcessResponses(FEMessageResponse *pRespList, FEObject *pObj, FEPackage *pPack, u32 ControlMask) {
    u32 i = 0;
    u32 Count = pRespList->GetCount();
    FEResponse *pResp;
    for (i = 0; i < Count; i++) {
        pResp = pRespList->GetResponse(i);
        switch (pResp->ResponseID) {
            case MR_SetScript:
                if (pObj != nullptr) {
                    FEScript *pScript = pObj->FindScript(pResp->ResponseParam);
                    if (pScript != nullptr) {
                        pObj->SetCurrentScript(pScript);
                        pScript->CurTime = 0;
                    }
                }
                break;
            case MR_PostMessageToFEng: {
                FEObject *pTo = reinterpret_cast<FEObject *>(pResp->ResponseTarget);
                if (reinterpret_cast<u32>(pTo) != 0xFFFFFFFC && reinterpret_cast<u32>(pTo) != 0xFFFFFFFF) {
                    pTo = pPack->FindObjectByGUID(pResp->ResponseTarget);
                }
                QueueMessage(pResp->ResponseParam, pObj, pPack, pTo, ControlMask);
                break;
            }
            case MR_PostMessageToGame:
                QueueMessage(pResp->ResponseParam, pObj, pPack, reinterpret_cast<FEObject *>(0xFFFFFFFF), ControlMask);
                break;
            case MR_PostMessageToSound:
                QueueMessage(pResp->ResponseParam, pObj, pPack, reinterpret_cast<FEObject *>(0xFFFFFFFB), ControlMask);
                break;
            case MR_SetActiveButton: {
                FEObject *pButton;
                if (pResp->ResponseParam != 0) {
                    pButton = pPack->FindObjectByGUID(pResp->ResponseParam);
                } else {
                    pButton = nullptr;
                }
                if ((pButton == nullptr) && pResp->ResponseParam != 0) {
                    break;
                }
                pPack->SetCurrentButton(pButton, pButton != nullptr);
                break;
            }
            case MR_SetInputProcessing:
                SetProcessInput(pPack, pResp->ResponseParam == 1);
                break;
            case MR_RecordCurrentButton:
                if (pPack->GetCurrentButton() != nullptr) {
                    RecordLastPackageButton(pPack->GetNameHash(), pPack->GetCurrentButton()->GUID);
                } else {
                    RecordLastPackageButton(pPack->GetNameHash(), 0);
                }
                break;
            case MR_RecallRecordedButton: {
                FEObject *pButton = nullptr;
                u32 recalled = RecallLastPackageButton(pPack->GetNameHash());
                if (recalled != 0) {
                    pButton = pPack->FindObjectByGUID(recalled);
                }
                if (pButton == nullptr) {
                    if (pResp->ResponseParam != 0) {
                        pButton = pPack->FindObjectByGUID(pResp->ResponseParam);
                    }
                    if ((pButton == nullptr) && pResp->ResponseParam != 0) {
                        break;
                    }
                }
                pPack->SetCurrentButton(pButton, pButton != nullptr);
                break;
            }
            case MR_DontNavigate:
                QueuePackageUserTransfer(pPack, true, ControlMask);
                break;
            case MR_PassControlToChildGlobal:
                QueuePackageUserTransfer(pPack, true, 0xFF);
                break;
            case MR_PassControlToParentCurrent:
                QueuePackageUserTransfer(pPack, false, ControlMask);
                break;
            case MR_PassControlToParentGlobal:
                QueuePackageUserTransfer(pPack, false, 0xFF);
                break;
            case MR_SwitchToPackage:
                QueuePackageSwitch(reinterpret_cast<const char *>(pResp->ResponseParam), pPack->GetControlMask());
                break;
            case MR_PushPackageGlobal:
                QueuePackagePush(reinterpret_cast<const char *>(pResp->ResponseParam), pPack->GetControlMask());
                break;
            case MR_PushPackageCurrent: {
                u32 pad = 0;
                do {
                    if (ControlMask & (1 << pad)) {
                        QueuePackagePush(reinterpret_cast<const char *>(pResp->ResponseParam), ControlMask);
                    }
                    pad++;
                } while (pad < 8);
                break;
            }
            case MR_PushPackageNone:
                QueuePackagePush(reinterpret_cast<const char *>(pResp->ResponseParam), 0);
                break;
            case MR_PopPackage:
                QueuePackagePop();
                break;
            case MR_RecordPackageMarker:
                RecordPackageMarker(pPack->GetName());
                break;
            case MR_SwitchToPackageMarker: {
                const char *pMarker = RecallPackageMarker();
                if (pMarker != nullptr) {
                    QueuePackageSwitch(pMarker, pPack->GetControlMask());
                }
                break;
            }
            case MR_ClearPackageMarkers:
                ClearPackageMarkers();
                break;
            case MR_IfScriptEquals:
                if (pObj->pCurrentScript->ID != pResp->ResponseParam) {
                    i = pRespList->FindConditionBranchTarget(i);
                }
                break;
            case MR_IfScriptNotEquals:
                if (pObj->pCurrentScript->ID == pResp->ResponseParam) {
                    i = pRespList->FindConditionBranchTarget(i);
                }
                break;
            case MR_Else:
                i = pRespList->FindConditionBranchTarget(i);
                break;
        }
    }
}

FEPackage *FEngine::FindPackageWithControl() {
    FEPackage *package = PackList.GetLastPackage();
    while (package != nullptr) {
        if (package->GetControlMask()) {
            return package;
        }
        package = package->GetPrev();
    }
    return nullptr;
}

FEPackageCommand *FEngine::FindQueuedNodeWithControl() {
    FEPackageCommand *pNode = static_cast<FEPackageCommand *>(PackageCommands.GetTail());
    while (pNode != nullptr) {
        if (pNode->iCommand & 2) {
            return pNode;
        }
        pNode = static_cast<FEPackageCommand *>(pNode->GetPrev());
    }
    return nullptr;
}

void FEngine::ProcessPackageCommands() {
    FEPackageCommand *pNode;
    i32 Level;
    FEPackage *pFixParentLink = nullptr;
    FEPackage *pNewParentLink = nullptr;

    while (true) {
        pNode = static_cast<FEPackageCommand *>(PackageCommands.RemHead());
        if (pNode == nullptr) {
            return;
        }

        if (pNode->pPackage != nullptr) {
            Level = pNode->pPackage->GetPriority();
        } else {
            pNode->pPackage = FindPackageWithControl();
            if (pNode->pPackage != nullptr) {
                Level = pNode->pPackage->GetPriority();
                pNode->pPackage->SetOldControlMask(pNode->pPackage->GetControlMask());
                pNode->pPackage->SetControlMask(0);
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
                if (pNode->pPackage->GetParentPackage() != nullptr) {
                    pNode->pPackage->GetParentPackage()->SetControlMask(pNode->pPackage->GetParentPackage()->GetOldControlMask());
                }
                UnloadPackage(pNode->pPackage);
                Level--;
            }
        }

        if (pNode->iCommand & 2) {
            FEPackage *pPushed = PushPackage(pNode->GetName(), static_cast<unsigned char>(Level + 1), pNode->uControlMask);
            if ((pPushed != nullptr) && (pNode->iCommand & 1) == 0 && Level >= 0) {
                pPushed->SetParentPackage(pNode->pPackage);
            } else if (pNode->iCommand & 1) {
                pPushed->SetParentPackage(pNewParentLink);
                PackList.ReplaceParentLinks(pFixParentLink, pPushed);
            }
        }

        if (pNode->iCommand & 4) {
            FEPackage *pParent = pNode->pPackage->GetParentPackage();
            if (pParent != nullptr) {
                u32 PassedMask = pNode->pPackage->GetControlMask() & pNode->uControlMask;
                pNode->pPackage->SetControlMask(pNode->pPackage->GetControlMask() & ~PassedMask);
                pParent->SetControlMask(pParent->GetControlMask() | PassedMask);
                QueueMessage(0x334c5493, nullptr, pParent, reinterpret_cast<FEObject *>(0xFFFFFFFC), pNode->uControlMask);
            }
        }

        if (pNode->iCommand & 8) {
            FEPackage *pChild = PackList.GetFirstPackage();
            while (true) {
                if (pChild == nullptr) {
                    break;
                }
                if (pChild->GetParentPackage() == pNode->pPackage)
                    break;
                pChild = pChild->GetNext();
            }
            if (pChild != nullptr) {
                u32 PassedMask = pNode->pPackage->GetControlMask() & pNode->uControlMask;
                pNode->pPackage->SetControlMask(pNode->pPackage->GetControlMask() & ~PassedMask);
                pChild->SetControlMask(pChild->GetControlMask() | PassedMask);
                QueueMessage(0x334c5493, nullptr, pChild, reinterpret_cast<FEObject *>(0xFFFFFFFCu), pNode->uControlMask);
            }
        }

        delete pNode;
    }
}

int FEngine::GetNumPackagesBelowPriority(u8 priority) {
    i32 count = 0;
    FEPackage *package = PackList.GetFirstPackage();
    while (package != nullptr) {
        if (package->GetPriority() < priority) {
            count++;
        }
        package = package->GetNext();
    }
    FEPackageCommand *pNode = static_cast<FEPackageCommand *>(PackageCommands.GetHead());
    while (pNode != nullptr) {
        if (count == 0 && (pNode->iCommand & 3)) {
            count = 1;
        } else if (pNode->iCommand & 2) {
            count++;
        } else if (pNode->iCommand & 1) {
            count--;
        }
        pNode = static_cast<FEPackageCommand *>(pNode->GetNext());
    }
    return count;
}

void FEngine::RecordLastPackageButton(u32 PackageHash, u32 ButtonGUID) {
    for (i32 i = 0; i < 32; i++) {
        if (RecordedPackageButtons[i].PackageHash == PackageHash) {
            RecordedPackageButtons[i].PackageHash = 0;
        }
    }
    RecordedPackageButtons[NextButtonRecordIndex].PackageHash = PackageHash;
    RecordedPackageButtons[NextButtonRecordIndex].ButtonGUID = ButtonGUID;
    NextButtonRecordIndex = (NextButtonRecordIndex + 1) % 32;
}

u32 FEngine::RecallLastPackageButton(u32 PackageHash) {
    for (i32 i = 0; i < 32; i++) {
        if (RecordedPackageButtons[i].PackageHash == PackageHash) {
            return RecordedPackageButtons[i].ButtonGUID;
        }
    }
    return 0;
}

bool FEngine::RecordPackageMarker(const char *pPackageName) {
    if (CurrentPackageRecordIndex == 16) {
        return false;
    }

    FEngStrCpy(RecordedPackageNames[CurrentPackageRecordIndex++], pPackageName);
    return true;
}

const char *FEngine::RecallPackageMarker() {
    if (CurrentPackageRecordIndex == 0) {
        return nullptr;
    }
    return RecordedPackageNames[--CurrentPackageRecordIndex];
}

void FEngine::ClearPackageMarkers() {
    for (u32 i = 0; i < 16; i++) {
        RecordedPackageNames[i][0] = '\0';
    }

    CurrentPackageRecordIndex = 0;
}
