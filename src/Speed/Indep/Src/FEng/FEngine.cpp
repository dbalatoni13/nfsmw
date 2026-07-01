#include "Speed/Indep/Src/FEng/FEngine.h"
#include "Speed/Indep/Src/FEng/FEColoredImage.h"
#include "Speed/Indep/Src/FEng/FEMessageNode.h"
#include "Speed/Indep/Src/FEng/FEMessageResponse.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEObjectCallback.h"
#include "Speed/Indep/Src/FEng/FEScript.h"
#include "Speed/Indep/Src/FEng/FEJoyPad.h"
#include "Speed/Indep/Src/FEng/FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEPackageReader.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEListBox.h"
#include "Speed/Indep/Src/FEng/FECodeListBox.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include <new>

extern "C" int printf(const char *, ...);

static const u32 FEAutoRepeatFrames = 6;       // size: 0x4, Decl: speed/indep/src/feng/FEngine.cpp:26
static const u32 FEAutoRepeatFirstFrames = 16; // size: 0x4, Decl: speed/indep/src/feng/FEngine.cpp:27

static const u32 Msg_Global_DisableInputs = 0x5D4CE32D; // size: 0x4, Decl: speed/indep/src/feng/FEngine.cpp:29
static const u32 Msg_Global_EnableInputs = 0x59BED120;  // size: 0x4, Decl: speed/indep/src/feng/FEngine.cpp:30

// total size: 0x4
// Decl: speed/indep/src/feng/FEngine.cpp:33
typedef struct {
    u8 Index1, Index2; // offset 0x0, size 0x1, Decl: speed/indep/src/feng/FEngine.cpp:34
    u16 Dir;           // offset 0x2, size 0x2, Decl: speed/indep/src/feng/FEngine.cpp:35
} PadDirImpulse;

static PadDirImpulse ImpulseDir[8] = {
    // size: 0x20, address: 0x8041D080, Decl: speed/indep/src/feng/FEngine.cpp:38
    {0x00, 0x02, 0x0007}, {0x00, 0x03, 0x0001}, {0x01, 0x02, 0x0005}, {0x01, 0x03, 0x0003},
    {0x00, 0xFF, 0x0000}, {0x02, 0xFF, 0x0006}, {0x01, 0xFF, 0x0004}, {0x03, 0xFF, 0x0002},
};

// total size: 0x4
// Decl: speed/indep/src/feng/FEngine.cpp:674
class FEngSetDirtyFlagsCallback : public FEObjectCallback {
  private:
    bool Callback(FEObject *pObj) override {} // Decl: speed/indep/src/feng/FEngine.cpp:675
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
    FEJoyPad **ppJoyPad = &pJoyPad;
    if (*ppJoyPad) {
        delete[] *ppJoyPad;
    }
    if (Count) {
        FEJoyPad *pPads = FNEW FEJoyPad[Count];
        long i = Count - 1;
        if (Count != 0) {
            FEJoyPad *pCur = pPads;
            do {
                new (pCur) FEJoyPad();
                pCur++;
            } while (i-- != 0);
        }
        *ppJoyPad = pPads;
    }
    NumJoyPads = Count;
    FEngMemSet(HoldDecrement, 0, sizeof(HoldDecrement));
}

void FEngine::SetExecution(bool bProcessEverything) {
    FEPackage *pPack = PackList.GetFirstPackage();
    while (pPack) {
        pPack->SetExecute(bProcessEverything);
        pPack = pPack->GetNext();
    }
}

void FEngine::SetProcessInput(FEPackage *pkg, bool bProcess) {
    if (!pkg) {
        return;
    }
    pkg->SetInputEnabled(bProcess);
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

FEPackage *FEngine::LoadPackage(const void *pPackageData, bool bLoadAsLibrary) {
    FEPackageReader reader;
    FEPackage *pPack = reader.Load(pPackageData, pInterface, this, bLoadObjectNames, bLoadScriptNames, bLoadAsLibrary);
    if (!pPack) {
        return nullptr;
    }
    return pPack;
}

bool FEngine::UnloadPackage(FEPackage *pPackage) {
    FEPackage *pPack = PackList.GetFirstPackage();
    while (pPack) {
        if (pPackage == pPack) {
            bool bDelete;
            if (pInterface) {
                bDelete = pInterface->PackageWillUnload(pPack);
            } else {
                bDelete = true;
            }
            PackList.RemovePackage(pPackage);
            FEPackageCommand *pTempNode = static_cast<FEPackageCommand *>(PackageCommands.GetHead());
            while (pTempNode) {
                FEPackageCommand *pNextNode = static_cast<FEPackageCommand *>(pTempNode->GetNext());
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
                FENode *pLibName = static_cast<FENode *>(pPack->GetLibraryList().GetHead());
                while (pLibName) {
                    FEPackage *pLibPack = FindLibraryPackage(pLibName->GetNameHash());
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

void FEngine::UnloadLibraryPackage(FEPackage *pLibPack) {
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

FEPackage *FEngine::PushPackage(const char *pPackageName, const u8 Level, const u32 ControlMask) {
    FEPackage *pPack = FindIdlePackage(pPackageName);
    if (!pPack) {
        char len = static_cast<char>(FEngStrLen(pPackageName));
        const char *pBaseName = pPackageName + len - 1;
        char c = *pBaseName;
        while (c != '/' && c != '\\' && len > 0) {
            c = *--pBaseName;
            len--;
        }
        if (len != 0) {
            pBaseName++;
        }
        pPack = FindIdlePackage(pBaseName);
    }
    if (pPack) {
        PackageInitStateCB cb;
        pPack->SetUseIdleList(true);
        pPack->ForAllObjects(cb);
        IdleList.RemNode(pPack);
    } else {
        u8 *pBlockStart;
        bool bDeleteBlock;
        u8 *pPackData = pInterface->GetPackageData(pPackageName, &pBlockStart, bDeleteBlock);
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
    pPack->SetControlMask(ControlMask);
    pPack->SetPriority(Level);
    pPack->SetExecute(bExecuting);
    if (pInterface) {
        pInterface->PackageWasLoaded(pPack);
    }
    PackList.AddPackage(pPack);
    return pPack;
}

void FEngine::AddToIdleList(FEPackage *pPack) {
    IdleList.AddNode(IdleList.GetTail(), pPack);
}

FEPackage *FEngine::FindIdlePackage(const char *pName) const {
    return static_cast<FEPackage *>(IdleList.FindNode(pName));
}

FEPackage *FEngine::GetFirstLibrary() const {
    return static_cast<FEPackage *>(LibraryList.GetHead());
}

void FEngine::AddToLibraryList(FEPackage *pPack) {
    LibraryList.AddNode(LibraryList.GetTail(), pPack);
}

void FEngine::RemoveFromLibraryList(FEPackage *pPack) {
    LibraryList.RemNode(pPack);
}

FEPackage *FEngine::FindLibraryPackage(u32 NameHash) const {
    FEPackage *pPack = GetFirstLibrary();
    while (pPack) {
        if (FEHashUpper(pPack->GetFilename() + 2) == NameHash) {
            return pPack;
        }
        pPack = pPack->GetNext();
    }
    return nullptr;
}

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
        for (unsigned char PadIndex = 0; PadIndex < NumJoyPads; PadIndex++) {
            pJoyPad[PadIndex].Update(pInterface->GetJoyPadMask(PadIndex), tDeltaTicks);
        }
        for (pPackage = PackList.GetFirstPackage(); pPackage; pPackage = pPackage->GetNext()) {
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
            iIterationTicks = 0;
            while (pPackage) {
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
            if (MsgQ.GetHead()) {
                ProcessMessageQueue();
            }
            bRenderedRecently = false;
            iTicksRemaining = iIterationTicks;
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

// size: 0x4C, address: 0x8041D0A0, Decl: speed/indep/src/feng/FEngine.cpp:873
static u32 PadButtonHash[19] = {
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0x406415E3u, 0x911AB364u, 0xB5AF2461u, 0x5073EF13u, 0xD9FEEC59u, 0xC519BFBFu,
    0xC519BFC0u, 0xC519BFC1u, 0xC519BFC2u, 0xC519BFC3u, 0xC519BFC4u, 0xC519BFC5u, 0xC519BFC6u, 0xC519BFC7u, 0xC519BFC8u,
};

// size: 0x8, address: 0x8041D0EC, Decl: speed/indep/src/feng/FEngine.cpp:881
static u32 PadButtonHeldHash[2] = {
    0x447315AFu,
    0x20AD4EB5u,
};

// size: 0x4C, address: 0x8041D0F4, Decl: speed/indep/src/feng/FEngine.cpp:886
static u32 PadReleasedHash[19] = {
    0x00000000u, 0x00000000u, 0x00000000u, 0x00000000u, 0xC12E9E27u, 0xC2F8FCC8u, 0xEBFCDA65u, 0x091DCD57u, 0x7A39195Du, 0xD4671F83u,
    0xD871B0A4u, 0xDC7C41C5u, 0xE086D2E6u, 0xE4916407u, 0xE89BF528u, 0xECA68649u, 0xF0B1176Au, 0xF4BBA88Bu, 0xF8C639ACu,
};

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
            if (pCurButton && pCurButton->FindResponse(0x0C407210u) != nullptr) {
                QueueMessage(0x0C407210u, nullptr, pPackage, pPackage->GetCurrentButton(), FromPadPressed[4]);
                QueueMessage(0x0C407210u, pPackage->GetCurrentButton(), pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), FromPadPressed[4]);
            } else if (pPackage->FindResponse(0x406415E3u) != nullptr) {
                QueueMessage(0x406415E3u, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFD), FromPadPressed[4]);
                QueueMessage(0x406415E3u, nullptr, pPackage, reinterpret_cast<FEObject *>(0xFFFFFFFB), FromPadPressed[4]);
            }
            goto check_released;

        held_handler:
            if ((Held & Mask) != 0) {
                u32 PadMask = FromPadPressed[i];
                u32 MsgID = PadButtonHeldHash[i - 7];
                if (pCurButton && pCurButton->FindResponse(MsgID) != nullptr) {
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
                if (pCurButton && pCurButton->FindResponse(MsgID) != nullptr) {
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
    if (JoyMask != 0 && ((JoyMask ^ 1) & 1) == 0 && pPackage->IsInputEnabled()) {
        float fMouseX = Mouse.GetXPos();
        float fMouseY = Mouse.GetYPos();
        int NumMO = pPackage->NumMouseObjects;
        FEObjectMouseState *pStates = pPackage->MouseObjectStates;
        for (int i = 0; i < NumMO; i++) {
            UpdateMouseState(pPackage, pStates + i, fMouseX, fMouseY);
        }
    }
}

void FEngine::Render() {
    FEMatrix4 mView;
    FEMatrix4 mIdentity;
    mIdentity.Identify();
    pInterface->GetViewTransformation(&mView);
    FEPackage *aPackages[32];
    int numPackages = 0;
    for (FEPackage *pPack = PackList.GetFirstPackage(); pPack; pPack = pPack->GetNext()) {
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
    FEPackage *pPack = PackList.GetFirstPackage();
    uGroupContext = 0;
    while (pPack) {
        pInterface->BeginPackageRendering(pPack);
        Sorter.Zero();
        FEObject *pObj = pPack->GetFirstObject();
        while (pObj) {
            if (pObj->Type == FE_Group) {
                RenderGroup(static_cast<FEGroup *>(pObj), mIdentity, mView, 0);
            } else {
                RenderObject(pObj, mView, 0);
            }
            pObj = pObj->GetNext();
        }
        Sorter.SortObjects();
        pInterface->RenderObjectList(reinterpret_cast<FEObjectListEntry *>(Sorter.GetListPtr()), Sorter.GetNumObjects());
        pInterface->EndPackageRendering(pPack);
        pPack = pPack->GetNext();
    }
    bRenderedRecently = bExecuting;
}

void FEngine::RenderGroup(FEGroup *pGroup, FEMatrix4 &mParent, FEMatrix4 &mAccum, u16 RenderContext) {
    FEObjData *pData = pGroup->GetObjData();
    FEMatrix4 stTemp;
    FEMatrix4 stContext;
    FEMatrix4 stContextView;
    FEVector3 stOffset(0.0f);
    FEVector3 stPivot(0.0f);
    if (pData->Col.a != 0) {
        if (bExecuting || static_cast<int>(pGroup->Flags) >= 0) {
            pData->Rot.GetMatrix(&stTemp);
            stPivot = pData->Pivot;
            stPivot *= -1.0f;
            FEMultMatrix(&stOffset, &stTemp, &stPivot);
            stTemp.m41 = stOffset.x + pData->Pivot.x + pData->Pos.x;
            stTemp.m42 = stOffset.y + pData->Pivot.y + pData->Pos.y;
            stTemp.m43 = stOffset.z + pData->Pivot.z + pData->Pos.z;
            FEMultMatrix(&stContext, &stTemp, &mParent);
            FEMultMatrix(&stContextView, &stContext, &mAccum);
            u16 ctx = uGroupContext + 1;
            uGroupContext = ctx;
            pGroup->RenderContext = RenderContext;
            pInterface->GenerateRenderContext(ctx, pGroup);
            FEObject *pObj = pGroup->GetFirstChild();
            while (pObj) {
                if (pObj->Type == FE_Group) {
                    RenderGroup(static_cast<FEGroup *>(pObj), stContext, mAccum, ctx);
                } else {
                    RenderObject(pObj, stContextView, ctx);
                }
                pObj = pObj->GetNext();
            }
        }
    }
}

void FEngine::RenderObject(FEObject *pObj, FEMatrix4 &mParent, u16 RenderContext) {
    FEObjData *pData = pObj->GetObjData();
    if (pData->Col.a != 0) {
        FEVector3 pos(pData->Pivot);
        FEVector3 result;
        result.z = 0.0f;
        result.y = 0.0f;
        result.x = 0.0f;
        pos.x = pos.x + pData->Pos.x;
        pos.y = pos.y + pData->Pos.y;
        pos.z = pData->Pos.z + pos.z;
        FEMultMatrix(&result, &mParent, &pos);
        pObj->RenderContext = RenderContext;
        if (result.z > 0.0f) {
            Sorter.AddObject(pObj, result.z);
        }
    }
}

bool FEngine::ForAllObjects(FEObjectCallback &Callback) {
    FEPackage *pPack = PackList.GetFirstPackage();
    while (pPack) {
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
        int iVar2 = *reinterpret_cast<int *>(pInterface);
        typedef void (*DebugFn)(void *, u32, FEPackage *, FEObject *, FEObject *, u32);
        (*reinterpret_cast<DebugFn *>(iVar2 + 0xB4))(
            reinterpret_cast<void *>(reinterpret_cast<char *>(pInterface) + *reinterpret_cast<short *>(iVar2 + 0xB0)), MsgID, pFromPackage, pTo,
            pFrom, ControlMask);
    }
    MsgQ.AddTail(pNode);
}

void FEngine::SendMessageToGame(u32 MsgID, FEObject *pFrom, FEPackage *pFromPackage, u32 uControlMask) {
    int iVar1 = *reinterpret_cast<int *>(pInterface);
    typedef void (*GameMsgFn)(void *, u32, FEObject *, u32, FEPackage *);
    GameMsgFn fn = *reinterpret_cast<GameMsgFn *>(iVar1 + 0x3C);
    void *adjusted = reinterpret_cast<void *>(reinterpret_cast<char *>(pInterface) + *reinterpret_cast<short *>(iVar1 + 0x38));
    fn(adjusted, MsgID, pFrom, uControlMask, pFromPackage);
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
    FEPackageCommand *pCom = nullptr;
    FEPackage *pPackageWithControl = FindPackageWithControl();
    FEPackageCommand *Node = FNEW FEPackageCommand();
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
        pCom = FindQueuedNodeWithControl();
        if (pCom) {
            if (ControlMask == 0) {
                Node->uControlMask = pCom->uControlMask;
            } else {
                Node->uControlMask = ControlMask;
            }
        } else {
            if (ControlMask == 0) {
                Node->uControlMask = 0xFF;
            } else {
                Node->uControlMask = ControlMask;
            }
        }
    }
    Node->iCommand = command;
    Node->SetName(pPackageName);
    PackageCommands.AddTail(Node);
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
    while (pNode) {
        if (bDebugMessages) {
            pInterface->DebugMessageProcessed(pNode->MsgID, pNode->pMsgTarget, pNode->pMsgFrom, pNode->pFromPackage, pNode->ControlMask);
        }
        switch (reinterpret_cast<u32>(pNode->pMsgTarget)) {
            case 0: {
                pPack = PackList.GetFirstPackage();
                while (pPack) {
                    ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                    FEMsgTargetList *pTargList = pPack->GetMessageTargets(pNode->MsgID);
                    if (pTargList) {
                        u32 Count = pTargList->GetCount();
                        u32 i = 0;
                        u32 MsgID = pNode->MsgID;
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
                while (pPack) {
                    ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                    pPack = pPack->GetNext();
                }
                break;
            case 0xFFFFFFFD:
                ProcessGlobalMessage(pNode->pFromPackage, pNode->MsgID, pNode->ControlMask);
                break;
            case 0xFFFFFFFC: {
                pPack = PackList.GetFirstPackage();
                while (pPack && pPack != pNode->pFromPackage) {
                    pPack = pPack->GetNext();
                }
                if (pPack) {
                    ProcessGlobalMessage(pPack, pNode->MsgID, pNode->ControlMask);
                    FEMsgTargetList *pTargList = pPack->GetMessageTargets(pNode->MsgID);
                    if (pTargList) {
                        u32 Count = pTargList->GetCount();
                        u32 i = 0;
                        u32 MsgID = pNode->MsgID;
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

bool FEngine::ProcessListBoxResponses(FEObject *pObj, FEPackage *pPack, u32 MsgID) {
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

bool FEngine::ProcessCodeListBoxResponses(FEObject *pObj, FEPackage *pPack, u32 MsgID) {
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

void FEngine::ProcessObjectMessage(FEObject *pObj, FEPackage *pPack, u32 MsgID, u32 uControlMask) {
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
    FEMessageResponse *pResp = pObj->FindResponse(MsgID);
    if (pResp) {
        ProcessResponses(pResp, pObj, pPack, uControlMask);
    }
}

void FEngine::ProcessGlobalMessage(FEPackage *pPack, u32 MsgID, u32 uControlMask) {
    FEMessageResponse *pResp = pPack->FindResponse(MsgID);
    if (pResp) {
        ProcessResponses(pResp, nullptr, pPack, uControlMask);
    }
}

void FEngine::ProcessResponses(FEMessageResponse *pRespList, FEObject *pObj, FEPackage *pPack, u32 ControlMask) {
    u32 Count = pRespList->GetCount();
    for (u32 i = 0; i < Count; i++) {
        FEResponse *pResp = pRespList->GetResponse(i);
        switch (pResp->ResponseID) {
            case 0:
                if (pObj) {
                    FEScript *pScript = pObj->FindScript(pResp->ResponseParam);
                    if (pScript) {
                        pObj->SetCurrentScript(pScript);
                        pScript->CurTime = 0;
                    }
                }
                break;
            case 1: {
                FEObject *pTo = reinterpret_cast<FEObject *>(pResp->ResponseTarget);
                if (reinterpret_cast<u32>(pTo) != 0xFFFFFFFC && reinterpret_cast<u32>(pTo) != 0xFFFFFFFF) {
                    pTo = pPack->FindObjectByGUID(pResp->ResponseTarget);
                }
                QueueMessage(pResp->ResponseParam, pObj, pPack, pTo, ControlMask);
                break;
            }
            case 2:
                QueueMessage(pResp->ResponseParam, pObj, pPack, reinterpret_cast<FEObject *>(0xFFFFFFFF), ControlMask);
                break;
            case 3:
                QueueMessage(pResp->ResponseParam, pObj, pPack, reinterpret_cast<FEObject *>(0xFFFFFFFB), ControlMask);
                break;
            case 0x100: {
                FEObject *pButton;
                if (pResp->ResponseParam != 0) {
                    pButton = pPack->FindObjectByGUID(pResp->ResponseParam);
                } else {
                    pButton = nullptr;
                }
                if (!pButton && pResp->ResponseParam != 0) {
                    break;
                }
                pPack->SetCurrentButton(pButton, pButton != nullptr);
                break;
            }
            case 0x101:
                SetProcessInput(pPack, pResp->ResponseParam == 1);
                break;
            case 0x102:
                if (pPack->GetCurrentButton()) {
                    RecordLastPackageButton(pPack->GetNameHash(), pPack->GetCurrentButton()->GUID);
                } else {
                    RecordLastPackageButton(pPack->GetNameHash(), 0);
                }
                break;
            case 0x103: {
                FEObject *pButton = nullptr;
                u32 recalled = RecallLastPackageButton(pPack->GetNameHash());
                if (recalled != 0) {
                    pButton = pPack->FindObjectByGUID(recalled);
                }
                if (!pButton) {
                    if (pResp->ResponseParam != 0) {
                        pButton = pPack->FindObjectByGUID(pResp->ResponseParam);
                    }
                    if (!pButton && pResp->ResponseParam != 0) {
                        break;
                    }
                }
                pPack->SetCurrentButton(pButton, pButton != nullptr);
                break;
            }
            case 0x105:
                QueuePackageUserTransfer(pPack, true, ControlMask);
                break;
            case 0x106:
                QueuePackageUserTransfer(pPack, true, 0xFF);
                break;
            case 0x107:
                QueuePackageUserTransfer(pPack, false, ControlMask);
                break;
            case 0x108:
                QueuePackageUserTransfer(pPack, false, 0xFF);
                break;
            case 0x200:
                QueuePackageSwitch(reinterpret_cast<const char *>(pResp->ResponseParam), pPack->GetControlMask());
                break;
            case 0x201:
                QueuePackagePush(reinterpret_cast<const char *>(pResp->ResponseParam), pPack->GetControlMask());
                break;
            case 0x202: {
                u32 pad = 0;
                do {
                    if (ControlMask & (1 << pad)) {
                        QueuePackagePush(reinterpret_cast<const char *>(pResp->ResponseParam), ControlMask);
                    }
                    pad++;
                } while (pad < 8);
                break;
            }
            case 0x204:
                QueuePackagePush(reinterpret_cast<const char *>(pResp->ResponseParam), 0);
                break;
            case 0x203:
                QueuePackagePop();
                break;
            case 0x2c0:
                RecordPackageMarker(pPack->GetName());
                break;
            case 0x2c1: {
                const char *pMarker = RecallPackageMarker();
                if (pMarker) {
                    QueuePackageSwitch(pMarker, pPack->GetControlMask());
                }
                break;
            }
            case 0x2c2:
                ClearPackageMarkers();
                break;
            case 0x300:
                if (pObj->pCurrentScript->ID != pResp->ResponseParam) {
                    i = pRespList->FindConditionBranchTarget(i);
                }
                break;
            case 0x301:
                if (pObj->pCurrentScript->ID == pResp->ResponseParam) {
                    i = pRespList->FindConditionBranchTarget(i);
                }
                break;
            case 0x500:
                i = pRespList->FindConditionBranchTarget(i);
                break;
        }
    }
}

FEPackage *FEngine::FindPackageWithControl() {
    FEPackage *pPack = PackList.GetLastPackage();
    while (pPack) {
        if (pPack->GetControlMask()) {
            return pPack;
        }
        pPack = pPack->GetPrev();
    }
    return nullptr;
}
FEPackageCommand *FEngine::FindQueuedNodeWithControl() {
    FEPackageCommand *pCmd = static_cast<FEPackageCommand *>(PackageCommands.GetTail());
    while (pCmd) {
        if (pCmd->iCommand & 2) {
            return pCmd;
        }
        pCmd = static_cast<FEPackageCommand *>(pCmd->GetPrev());
    }
    return nullptr;
}

void FEngine::ProcessPackageCommands() {
    FEPackage *pFixParentLink = nullptr;
    FEPackage *pNewParentLink = nullptr;

    do {
        FEPackageCommand *pNode = static_cast<FEPackageCommand *>(PackageCommands.RemHead());
        if (!pNode) {
            return;
        }

        int Level;
        if (pNode->pPackage) {
            Level = pNode->pPackage->Priority;
        } else {
            FEPackage *pPack = FindPackageWithControl();
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
                FEPackage *pParent = pNode->pPackage->pParentPackage;
                if (pParent) {
                    pParent->Controllers = pParent->OldControllers;
                }
                UnloadPackage(pNode->pPackage);
                Level--;
            }
        }

        if (pNode->iCommand & 2) {
            FEPackage *pPushed = PushPackage(pNode->GetName(), static_cast<unsigned char>(Level + 1), pNode->uControlMask);
            if (pPushed && (pNode->iCommand & 1) == 0 && Level >= 0) {
                pPushed->pParentPackage = pNode->pPackage;
            } else if (pNode->iCommand & 1) {
                pPushed->pParentPackage = pNewParentLink;
                PackList.ReplaceParentLinks(pFixParentLink, pPushed);
            }
        }

        if (pNode->iCommand & 4) {
            FEPackage *pPack = pNode->pPackage;
            FEPackage *pParent = pPack->pParentPackage;
            if (pParent) {
                u32 PassedMask = pPack->Controllers & pNode->uControlMask;
                pPack->Controllers &= ~PassedMask;
                pParent->Controllers |= PassedMask;
                QueueMessage(0x334c5493u, nullptr, pParent, reinterpret_cast<FEObject *>(0xFFFFFFFCu), pNode->uControlMask);
            }
        }

        if (pNode->iCommand & 8) {
            FEPackage *pChild = PackList.GetFirstPackage();
            while (true) {
                if (!pChild) {
                    break;
                }
                if (pChild->pParentPackage == pNode->pPackage)
                    break;
                pChild = pChild->GetNext();
            }
            if (pChild) {
                u32 PassedMask = pNode->pPackage->Controllers & pNode->uControlMask;
                pNode->pPackage->Controllers &= ~PassedMask;
                pChild->Controllers |= PassedMask;
                QueueMessage(0x334c5493u, nullptr, pChild, reinterpret_cast<FEObject *>(0xFFFFFFFCu), pNode->uControlMask);
            }
        }

        delete pNode;
    } while (true);
}

int FEngine::GetNumPackagesBelowPriority(u8 priority) {
    int count = 0;
    FEPackage *package = PackList.GetFirstPackage();
    while (package) {
        if (package->GetPriority() < priority) {
            count++;
        }
        package = package->GetNext();
    }
    FEPackageCommand *pNode = static_cast<FEPackageCommand *>(PackageCommands.GetHead());
    while (pNode) {
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

void FEngine::RecordLastPackageButton(u32 PackHash, u32 ButtonGUID) {
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

u32 FEngine::RecallLastPackageButton(u32 PackHash) {
    for (int i = 0; i < 32; i++) {
        if (RecordedPackageButtons[i].PackageHash == PackHash) {
            return RecordedPackageButtons[i].ButtonGUID;
        }
    }
    return 0;
}

bool FEngine::RecordPackageMarker(const char *pName) {
    int idx = CurrentPackageRecordIndex;
    if (idx == 16) {
        return false;
    }
    CurrentPackageRecordIndex = idx + 1;
    FEngStrCpy(RecordedPackageNames[idx], pName);
    return true;
}

const char *FEngine::RecallPackageMarker() {
    if (CurrentPackageRecordIndex == 0) {
        return nullptr;
    }
    return RecordedPackageNames[--CurrentPackageRecordIndex];
}

void FEngine::ClearPackageMarkers() {
    {
        u32 i = 0;
        do {
            RecordedPackageNames[i][0] = '\0';
            i++;
        } while (i < 16);
    }
    CurrentPackageRecordIndex = 0;
}
