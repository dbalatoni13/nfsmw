#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

EasterEggs gEasterEggs;

EasterEggs::EasterEggs() {
    static EasterEggsData StaticEasterEggsTable[2] = {
        {{UP, DN, UP, DN, LT, RT, LT, RT}, 3, 0, 0, 0, false, true, true},
        {{LT, RT, LT, RT, UP, DN, UP, DN}, 4, 0, 0, 0, false, true, true},
    };
    ButtonsEnabled = false;
    HaveUnlockMessage = false;
    UnlockMessage = 0;
    NumberOfCurrentButtons = 0;
    CurrentStartButton = 0;
    EasterEggsTable = StaticEasterEggsTable;
    NumberOfEasterEggs = 2;
}

EasterEggs::~EasterEggs() {}

void EasterEggs::Activate() {
    for (int i = 0; i < 2; i++) {
        EasterEggActionQ[i] = new ((const char *)nullptr) ActionQueue(i, 0x82d21520, "movie q", false);
        EasterEggActionQ[i]->Enable(true);
    }
    ButtonsEnabled = true;
    HaveUnlockMessage = false;
    UnlockMessage = 0;
    ClearButtons();
}

void EasterEggs::UnActivate() {
    for (int i = 0; i < 2; i++) {
        if (EasterEggActionQ[i]) {
            delete EasterEggActionQ[i];
        }
    }
    ButtonsEnabled = false;
    HaveUnlockMessage = false;
    UnlockMessage = 0;
    ClearButtons();
}

void EasterEggs::ClearNonPersistent() {
    for (unsigned int egg = 0; egg < NumberOfEasterEggs; egg++) {
        if (EasterEggsTable[egg].persistent == false) {
            EasterEggsTable[egg].unlocked = false;
        }
    }
}

void EasterEggs::ActivateEasterEgg(int egg) {
    if (EasterEggsTable[egg].enabled == false) {
        return;
    }
    ClearGroup(EasterEggsTable[egg].group);
    bool was_unlocked = EasterEggsTable[egg].unlocked;
    EasterEggsTable[egg].unlocked = true;
    if (EasterEggsTable[egg].type == EASTER_EGG_SPECIAL) {
        TriggerSpecial(EasterEggsTable[egg].item);
    }
    HaveUnlockMessage = true;
    UnlockMessage = EasterEggsTable[egg].unlock_message;
    ClearButtons();
    g_pEAXSound->PlayUISoundFX(UISND_COMMON_SELECT);
    if (!was_unlocked) {
        if (EasterEggsTable[egg].type == EASTER_EGG_CARS) {
            UnlockAllThings = 1;
        } else if (EasterEggsTable[egg].type == EASTER_EGG_VISUAL_PARTS) {
            SkipDDayRaces = 1;
            SkipCareerIntro = 1;
        } else if (EasterEggsTable[egg].type == EASTER_EGG_PERF_PARTS) {
            MikeMannBuild = 0;
        } else if (EasterEggsTable[egg].type == EASTER_EGG_DRIFT_PHYSICS) {
            // TODO magic
            cFEng::Get()->QueueGameMessage(0x98257537, nullptr, 0xff);
        } else if (EasterEggsTable[egg].type == EASTER_EGG_TRACK) {
            // TODO magic
            cFEng::Get()->QueueGameMessage(0x6521e5c2, nullptr, 0xff);
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            stable->AwardBonusCars();
        } else if (EasterEggsTable[egg].type == EASTER_EGG_SPECIAL) {
            // TODO magic
            cFEng::Get()->QueueGameMessage(0xa6813b08, nullptr, 0xff);
            FEDatabase->GetCareerSettings()->TryAwardDemoMarker();
        }
    }
}

void EasterEggs::HandleJoy() {
    if (!ButtonsEnabled) {
        return;
    }
    for (int port = 0; port < 2; port++) {
        if (!EasterEggActionQ[port]) {
            continue;
        }
        while (!EasterEggActionQ[port]->IsEmpty()) {
            ActionRef aRef = EasterEggActionQ[port]->GetAction();
            if (aRef.Data() == 1.0f && NumberOfCurrentButtons < 8) {
                unsigned int button_buffer_index = (CurrentStartButton + NumberOfCurrentButtons) & 7;
                ButtonBuffer[button_buffer_index] = static_cast<EasterEggButtons>(aRef.ID());
                NumberOfCurrentButtons++;
                if (NumberOfCurrentButtons == 8) {
                    bool found = false;
                    for (unsigned int egg = 0; egg < NumberOfEasterEggs; egg++) {
                        found = true;
                        for (unsigned int button = 0; button < 8; button++) {
                            if (EasterEggsTable[egg].buttons[button] != ButtonBuffer[(CurrentStartButton + button) & 7]) {
                                found = false;
                                break;
                            }
                        }
                        if (found) {
                            ActivateEasterEgg(egg);
                            break;
                        }
                    }
                    if (!found) {
                        CurrentStartButton = (CurrentStartButton + 1) & 7;
                        NumberOfCurrentButtons--;
                    }
                }
            }
            EasterEggActionQ[port]->PopAction();
        }
    }
}

void EasterEggs::ClearButtons() {
    NumberOfCurrentButtons = 0;
}

bool EasterEggs::IsEasterEggUnlocked(unsigned int type, unsigned int item) {
    for (unsigned int egg = 0; egg < NumberOfEasterEggs; egg++) {
        if (EasterEggsTable[egg].type == type && EasterEggsTable[egg].item == item) {
            return EasterEggsTable[egg].unlocked;
        }
    }
    return false;
}

bool EasterEggs::IsEasterEggUnlocked(EasterEggsSpecial egg) {
    return IsEasterEggUnlocked(static_cast<unsigned int>(egg), 0);
}

void EasterEggs::ClearGroup(unsigned int group) {
    if (group == 0) {
        return;
    }
    for (unsigned int egg = 0; egg < NumberOfEasterEggs; egg++) {
        if (EasterEggsTable[egg].group == group) {
            EasterEggsTable[egg].unlocked = false;
        }
    }
}

void EasterEggs::TriggerSpecial(unsigned int special) {}
