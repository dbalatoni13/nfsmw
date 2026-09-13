#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

// total size: 0x1C
// Decl: 30
struct cMapJoyEventToFEPad {
    ActionID Event;    // offset 0x0, size 0x4, Decl: 31
    uint32 FEPadValue; // offset 0x4, size 0x4, Decl: 32
    char *Name;        // offset 0x8, size 0x4, Decl: 33
    int State[4];      // offset 0xC, size 0x10, Decl: 34

    void ResetState() {} // Decl: 36
};

static cMapJoyEventToFEPad MapJoyEventToFEPad[16] = {
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

cFEngJoyInput *cFEngJoyInput::mInstance = nullptr;

cFEngJoyInput::cFEngJoyInput() {
    for (int i = 0; i < NUM_JOYSTICK_PORTS; i++) {
        mActionQ[i] = new ("cFEngJoyInput::ActionQueue") ActionQueue(i, 0x82d21520, "FEng", false);
        mActionQ[i]->Enable(true);
        mActionQ[i]->IsConnected();
    }
}

void cFEngJoyInput::FlushActions() {
    mActionQ[0]->Size(); // unknown purpose

    for (int port = 0; port < NUM_JOYSTICK_PORTS; port++) {
        if (mActionQ[port] != nullptr) {
            mActionQ[port]->Flush();
        }
        for (int i = 0; i < NUM_ELEMENTS(MapJoyEventToFEPad); i++) {
            MapJoyEventToFEPad[i].State[port] = 0;
        }
    }

    mActionQ[1]->Size(); // unknown purpose
}

void cFEngJoyInput::JoyDisable(JoystickPort port, bool do_flush) {
    if (port == JOYSTICK_PORT_ALL) {
        for (int i = 0; i < NUM_JOYSTICK_PORTS; i++) {
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
    if (port == JOYSTICK_PORT_ALL) {
        for (int i = 0; i < NUM_JOYSTICK_PORTS; i++) {
            if (!mActionQ[i]->IsEnabled()) {
                mActionQ[i]->Enable(true);
                if (do_flush) {
                    mActionQ[i]->Flush();
                }
            }
        }
    } else if (port != JOYSTICK_PORT_NONE) {
        if (!mActionQ[port]->IsEnabled()) {
            mActionQ[port]->Enable(true);
            if (do_flush) {
                mActionQ[port]->Flush();
            }
        }
    }
}

bool cFEngJoyInput::IsJoyEnabled(JoystickPort port) {
    if (port == JOYSTICK_PORT_ALL) {
        for (int i = 0; i < NUM_JOYSTICK_PORTS; i++) {
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
    if (port == JOYSTICK_PORT_ALL) {
        for (int i = 0; i < NUM_JOYSTICK_PORTS; i++) {
            mActionQ[i]->SetRequired(required);
        }
        return;
    }
    mActionQ[port]->SetRequired(required);
}

// UNSOLVED
bool cFEngJoyInput::CheckUnplugged() {
    bool unplugged = false;
    if (!TheGameFlowManager.IsInGame() && !FEManager::Get()->IsAllowingControllerError()) {
        SetRequiredJoy(JOYSTICK_PORT_NONE, false);
    } else {
        bool is_splitscreen = FEDatabase->IsSplitScreenMode();

        bool bIsSplit;
        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
            bIsSplit = true;
        } else if (!is_splitscreen) {
            bIsSplit = false;
        } else {
            bIsSplit = true;
        }

        JoystickPort player_port1 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
        JoystickPort player_port2 = JOYSTICK_PORT_NONE;
        if (player_port1 == JOYSTICK_PORT_NONE) {
            return false;
        }
        if (bIsSplit) {
            player_port2 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(1));
        }
        SetRequiredJoy(player_port1, true);
        if (player_port2 != JOYSTICK_PORT_NONE) {
            SetRequiredJoy(player_port2, true);
        }
        FEManager *feManager = FEManager::Get();
        if (!IsJoyPluggedIn(player_port1)) {
            feManager->WantControllerError(player_port1);
            unplugged = true;
        } else if (!bIsSplit && !cFEng::Get()->IsPackagePushed("ControllerUnplugged.fng")) {
            feManager->ClearControllerError(player_port1);
        }
        if (player_port2 != JOYSTICK_PORT_NONE && !IsJoyPluggedIn(player_port2)) {
            feManager->WantControllerError(player_port2);
            unplugged = true;
        }
    }
    return unplugged;
}

// UNSOLVED
void cFEngJoyInput::HandleJoy() {
    for (int port = 0; port < 2; port++) {
        if (mActionQ[port] != nullptr) {
            while (!mActionQ[port]->IsEmpty()) {
                ActionRef aRef = mActionQ[port]->GetAction();
                if (aRef.ID() == ACTION_PLUGGED) {
                    bool is_splitscreen = FEDatabase->IsSplitScreenMode();

                    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
                        is_splitscreen = true;
                    } else if (is_splitscreen) {
                        is_splitscreen = false;
                    } else {
                        is_splitscreen = true;
                    }

                    JoystickPort player_port1 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(0));
                    JoystickPort player_port2 = JOYSTICK_PORT_NONE;
                    if (is_splitscreen) {
                        player_port2 = static_cast<JoystickPort>(FEDatabase->GetPlayersJoystickPort(1));
                    }
                    if (port == player_port1) {
                        if (is_splitscreen && player_port2 != JOYSTICK_PORT_NONE) {
                            JoyEnable(player_port2, false);
                        }
                    } else if (port == player_port2 && is_splitscreen && player_port1 != JOYSTICK_PORT_NONE) {
                        JoyEnable(player_port1, false);
                    }
                    JoyEnable(static_cast<JoystickPort>(port), false);
                    mActionQ[port]->PopAction();
                } else {
                    mActionQ[port]->IsEnabled();
                    for (int j = 0; j < 16; j++) {
                        if (mActionQ[port]->IsConnected()) {
                            if (MapJoyEventToFEPad[j].Event == aRef.ID()) {
                                aRef.Data();
                                MapJoyEventToFEPad[j].State[port] = static_cast<int>(aRef.Data() + 0.5f);
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
                            MapJoyEventToFEPad[j].State[port] = 0;
                        }
                    }
                    mActionQ[port]->PopAction();
                }
            }
        }
    }
    CheckUnplugged();
}

u32 cFEngJoyInput::GetJoyPadMask(u8 pPadIndex) {
    uint32 buttons = 0;
    JoystickPort port;
    for (int i = 0; i < 16; i++) {
        if (MapJoyEventToFEPad[i].State[pPadIndex] != 0) {
            buttons |= MapJoyEventToFEPad[i].FEPadValue;
        }
    }
    return buttons;
}

// STRIPPED
uint32 cFEngJoyInput::GetJoyPadTexture(const char *eventString, JoystickPort port) {}
