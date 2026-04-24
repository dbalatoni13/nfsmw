#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/SimTypes.h"

#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"

namespace Sim {
Sim::eUserMode GetUserMode();
}

struct FEngTextInputObject;

struct KeyboardEditString {
    char InitialString[256];
    unsigned short EditStringUCS2[256];
    int CursorPosUCS2;
    char EditStringPacked[256];
    unsigned int ModeFlags;
    int KeysProcessed;
    int MaxTextLength;
    bool mEnabled;
    FEngTextInputObject* TextInputObject;

    bool IsCapturing() {
        return mEnabled && TextInputObject != nullptr;
    }
};

extern KeyboardEditString gKeyboardManager;

struct FEJoyMapping {
    int eventID;
    unsigned long padMask;
    const char* name;
    int state[4];
};

static FEJoyMapping MapJoyEventToFEPad[16] = {
    {FRONTENDACTION_UP, 0x00000001, "FEPad_Up", {0, 0, 0, 0}},
    {FRONTENDACTION_DOWN, 0x00000002, "FEPad_Down", {0, 0, 0, 0}},
    {FRONTENDACTION_LEFT, 0x00000004, "FEPad_Left", {0, 0, 0, 0}},
    {FRONTENDACTION_RIGHT, 0x00000008, "FEPad_Right", {0, 0, 0, 0}},
    {FRONTENDACTION_ACCEPT, 0x00000010, "FEPad_Accept", {0, 0, 0, 0}},
    {FRONTENDACTION_CANCEL, 0x00000020, "FEPad_Back", {0, 0, 0, 0}},
    {FRONTENDACTION_CANCEL_ALT, 0x00000020, "FEPad_Back", {0, 0, 0, 0}},
    {FRONTENDACTION_START, 0x00000040, "FEPad_Start", {0, 0, 0, 0}},
    {FRONTENDACTION_BUTTON0, 0x00000200, "FEPad_Button0", {0, 0, 0, 0}},
    {FRONTENDACTION_BUTTON1, 0x00000400, "FEPad_Button1", {0, 0, 0, 0}},
    {FRONTENDACTION_BUTTON2, 0x00000800, "FEPad_Button2", {0, 0, 0, 0}},
    {FRONTENDACTION_BUTTON3, 0x00001000, "FEPad_Button3", {0, 0, 0, 0}},
    {FRONTENDACTION_BUTTON4, 0x00002000, "FEPad_Button4", {0, 0, 0, 0}},
    {FRONTENDACTION_BUTTON5, 0x00004000, "FEPad_Button5", {0, 0, 0, 0}},
    {FRONTENDACTION_LTRIGGER, 0x00000080, "FEPad_LeftTrigger", {0, 0, 0, 0}},
    {FRONTENDACTION_RTRIGGER, 0x00000100, "FEPad_RightTrigger", {0, 0, 0, 0}},
};

cFEngJoyInput* cFEngJoyInput::mInstance;

cFEngJoyInput::cFEngJoyInput() {
    for (int i = 0; i < 2; i++) {
        mActionQ[i] = new ActionQueue(i, 0x82d21520, "FEng", false);
        mActionQ[i]->Enable(true);
        mActionQ[i]->IsConnected();
    }
}

void cFEngJoyInput::FlushActions() {
    for (int port = 0; port < 2; port++) {
        if (mActionQ[port] != nullptr) {
            mActionQ[port]->Flush();
        }
        for (int i = 0; i < 16; i++) {
            MapJoyEventToFEPad[i].state[port] = 0;
        }
    }
}

void cFEngJoyInput::JoyDisable(JoystickPort port, bool do_flush) {
    if (port == kJP_NumPorts) {
        for (int i = 0; i < 2; i++) {
            mActionQ[i]->Enable(false);
            if (do_flush) {
                mActionQ[i]->Flush();
            }
        }
    } else {
        mActionQ[port]->Enable(false);
        if (do_flush) {
            mActionQ[port]->Flush();
        }
    }
}

bool cFEngJoyInput::IsJoyPluggedIn(JoystickPort port) {
    return mActionQ[port]->IsConnected();
}

void cFEngJoyInput::JoyEnable(JoystickPort port, bool do_flush) {
    if (port == kJP_NumPorts) {
        for (int i = 0; i < 2; i++) {
            if (!mActionQ[i]->IsEnabled()) {
                mActionQ[i]->Enable(true);
                if (do_flush) {
                    mActionQ[i]->Flush();
                }
            }
        }
    } else if (port != static_cast<JoystickPort>(-1)) {
        if (!mActionQ[port]->IsEnabled()) {
            mActionQ[port]->Enable(true);
            if (do_flush) {
                mActionQ[port]->Flush();
            }
        }
    }
}

bool cFEngJoyInput::IsJoyEnabled(JoystickPort port) {
    if (port == kJP_NumPorts) {
        for (int i = 0; i < 2; i++) {
            if (!mActionQ[i]->IsEnabled()) {
                return false;
            }
        }
    } else {
        if (!mActionQ[port]->IsEnabled()) {
            return false;
        }
    }
    return true;
}

void cFEngJoyInput::SetRequiredJoy(JoystickPort port, bool required) {
    if (port == kJP_NumPorts) {
        for (int i = 0; i < 2; i++) {
            mActionQ[i]->SetRequired(required);
        }
        return;
    }
    mActionQ[port]->SetRequired(required);
}

bool cFEngJoyInput::CheckUnplugged() {
    bool unplugged = false;
    if (!TheGameFlowManager.IsInGame() && !FEManager::Get()->IsAllowingControllerError()) {
        SetRequiredJoy(kJP_NumPorts, false);
    } else {
        int is_splitscreen = false;
        if (FEDatabase->IsSplitScreenMode()) {
            is_splitscreen = FEDatabase->iNumPlayers == 2;
        }
        bool bIsSplit;
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            bIsSplit = true;
        } else {
            bIsSplit = is_splitscreen ? true : false;
        }
        JoystickPort player_port2 = static_cast<JoystickPort>(-1);
        JoystickPort player_port1 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
        if (player_port1 == static_cast<JoystickPort>(-1)) {
            return false;
        }
        if (bIsSplit) {
            player_port2 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(1));
        }
        SetRequiredJoy(player_port1, true);
        if (player_port2 != static_cast<JoystickPort>(-1)) {
            SetRequiredJoy(player_port2, true);
        }
        FEManager* feManager = FEManager::Get();
        if (!IsJoyPluggedIn(player_port1)) {
            feManager->WantControllerError(player_port1);
            unplugged = true;
        } else if (!bIsSplit && !cFEng::Get()->IsPackagePushed("ControllerUnplugged.fng")) {
            feManager->ClearControllerError(player_port1);
        }
        if (player_port2 != static_cast<JoystickPort>(-1) && !IsJoyPluggedIn(player_port2)) {
            feManager->WantControllerError(player_port2);
            unplugged = true;
        }
    }
    return unplugged;
}

void cFEngJoyInput::HandleJoy() {
    for (int port = 0; port < 2; port++) {
        if (mActionQ[port] != nullptr) {
            while (!mActionQ[port]->IsEmpty()) {
                ActionRef aRef = mActionQ[port]->GetAction();
                if (aRef.ID() == ACTION_PLUGGED) {
                    bool is_splitscreen = false;
                    if (FEDatabase->IsSplitScreenMode()) {
                        is_splitscreen = FEDatabase->iNumPlayers == 2;
                    }
                    bool bIsSplit;
                    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                        bIsSplit = true;
                    } else if (!is_splitscreen) {
                        bIsSplit = false;
                    } else {
                        bIsSplit = true;
                    }
                    JoystickPort player_port2 = static_cast<JoystickPort>(-1);
                    JoystickPort player_port1 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
                    if (bIsSplit) {
                        player_port2 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(1));
                    }
                    if (port == player_port1) {
                        if (bIsSplit && player_port2 != static_cast<JoystickPort>(-1)) {
                            JoyEnable(player_port2, false);
                        }
                    } else if (port == player_port2 && bIsSplit && player_port1 != static_cast<JoystickPort>(-1)) {
                        JoyEnable(player_port1, false);
                    }
                    JoyEnable(static_cast<JoystickPort>(port), false);
                    mActionQ[port]->PopAction();
                } else {
                    mActionQ[port]->IsEnabled();
                    for (int j = 0; j < 16; j++) {
                        if (mActionQ[port]->IsConnected()) {
                            if (MapJoyEventToFEPad[j].eventID == aRef.ID()) {
                                MapJoyEventToFEPad[j].state[port] = static_cast<int>(aRef.Data() + 0.5f);
                                if (!gKeyboardManager.IsCapturing()) {
                                    if (aRef.ID() == FRONTENDACTION_BUTTON2) {
                                        if (aRef.Data() == 1.0f) {
                                            FEManager::Get()->SetEATraxSecondButton();
                                        }
                                    } else if (aRef.ID() == FRONTENDACTION_BUTTON3) {
                                        if (aRef.Data() == 1.0f) {
                                            FEManager::Get()->SetEATraxFirstButton(true);
                                        } else {
                                            FEManager::Get()->SetEATraxFirstButton(false);
                                        }
                                    }
                                }
                                break;
                            }
                        } else {
                            MapJoyEventToFEPad[j].state[port] = 0;
                        }
                    }
                    mActionQ[port]->PopAction();
                }
            }
        }
    }
    CheckUnplugged();
}

unsigned long cFEngJoyInput::GetJoyPadMask(unsigned char pPadIndex) {
    unsigned int buttons = 0;
    for (int i = 0; i < 16; i++) {
        if (MapJoyEventToFEPad[i].state[pPadIndex] != 0) {
            buttons |= MapJoyEventToFEPad[i].padMask;
        }
    }
    return buttons;
}

void MUTEX_lock(MUTEX* m);
void MUTEX_unlock(MUTEX* m);

void MyMutex::Lock() {
    MUTEX_lock(&mMutex);
}

void MyMutex::Unlock() {
    MUTEX_unlock(&mMutex);
}

int MyMutex::AddRef() {
    return ++mRefcount;
}

int MyMutex::Release() {
    int ref = --mRefcount;
    if (ref > 0) {
        return ref;
    }
    if (this != nullptr) {
        delete this;
    }
    return 0;
}

IMutex* MyMutex::CreateInstance() {
    return new MyMutex();
}

int MyThread::AddRef() {
    return ++mRefcount;
}

int MyThread::Release() {
    int ref = --mRefcount;
    if (ref > 0) {
        return ref;
    }
    if (this != nullptr) {
        delete this;
    }
    return 0;
}

IThread* MyThread::CreateInstance() {
    return new MyThread();
}

void THREAD_sleep(int ticks);
void THREAD_create(THREAD* thread, int (*func)(void*), void* param, void* stack, int stackSize, int priority);
void THREAD_waitexit(THREAD* thread, int status);
void THREAD_setpriority(THREAD* thread, int priority);
void THREAD_yield(int ticks);

void MyThread::Sleep(int ticks) {
    THREAD_sleep(ticks);
}

void MyThread::Begin(int (*func)(void*)) {
    mEntryFunc = func;
    mStackBuffer = new char[mStackSize];
    THREAD_create(&mThreadData, EntryProc, this, mStackBuffer, mStackSize, mPriority);
    mActive = true;
}

void MyThread::WaitForEnd(int) {
    THREAD_waitexit(&mThreadData, 0);
    if (mStackBuffer != nullptr) {
        delete[] static_cast< char* >(mStackBuffer);
    }
    mActive = false;
}

void MyThread::SetPriority(int priority) {
    mPriority = 0;
    THREAD_setpriority(&mThreadData, 0);
}

bool MyThread::IsActive() { return mActive; }

int (*MyThread::GetEntryFunc())(void*) { return mEntryFunc; }

int MyThread::EntryProc(void* pContext) {
    MyThread* pThread = static_cast< MyThread* >(pContext);
    while (!pThread->MyThread::IsActive()) {
        THREAD_yield(1);
    }
    pThread->MyThread::GetEntryFunc()(pContext);
    return 0;
}
