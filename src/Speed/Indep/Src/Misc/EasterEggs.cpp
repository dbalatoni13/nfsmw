#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/cFEng.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

extern int UnlockAllThings;
extern int SkipCareerIntro;
extern int SkipDDayRaces;
extern int MikeMannBuild;
extern EAXSound *g_pEAXSound;
extern EasterEggs::EasterEggsData StaticEasterEggsTable[];

EasterEggs::EasterEggs() {
    EasterEggsTable = StaticEasterEggsTable;
    NumberOfEasterEggs = 2;
    CurrentStartButton = 0;
    NumberOfCurrentButtons = 0;
    UnlockMessage = 0;
    HaveUnlockMessage = false;
    ButtonsEnabled = false;
}

EasterEggs::~EasterEggs() {
}

void EasterEggs::Activate() {
    for (int i = 0; i < 2; i++) {
        EasterEggActionQ[i] = new ActionQueue(i, 0x82d21520, "", false);
        EasterEggActionQ[i]->Enable(true);
    }
    ButtonsEnabled = true;
    HaveUnlockMessage = false;
    UnlockMessage = 0;
    ClearButtons();
}

void EasterEggs::UnActivate() {
    for (int i = 0; i < 2; i++) {
        if (EasterEggActionQ[i] != nullptr) {
            delete EasterEggActionQ[i];
        }
    }
    ButtonsEnabled = false;
    HaveUnlockMessage = false;
    UnlockMessage = 0;
    ClearButtons();
}

void EasterEggs::ClearNonPersistent() {
    for (unsigned int i = 0; i < NumberOfEasterEggs; i++) {
        if (EasterEggsTable[i].persistent == false) {
            EasterEggsTable[i].unlocked = false;
        }
    }
}

void EasterEggs::ActivateEasterEgg(int egg) {
    EasterEggsData *data = &EasterEggsTable[egg];
    if (data->enabled == false) {
        return;
    }
    ClearGroup(data->group);
    data = &EasterEggsTable[egg];
    int wasUnlocked = data->unlocked;
    data->unlocked = true;
    data = &EasterEggsTable[egg];
    if (data->type == EASTER_EGG_SPECIAL) {
        TriggerSpecial(data->item);
    }
    HaveUnlockMessage = true;
    UnlockMessage = EasterEggsTable[egg].unlock_message;
    ClearButtons();
    g_pEAXSound->PlayUISoundFX(4);
    if (wasUnlocked == 0) {
        unsigned int type = EasterEggsTable[egg].type;
        if (type == EASTER_EGG_CARS) {
            UnlockAllThings = 1;
        } else if (type == EASTER_EGG_VISUAL_PARTS) {
            SkipCareerIntro = 1;
            SkipDDayRaces = 1;
        } else if (type == EASTER_EGG_PERF_PARTS) {
            MikeMannBuild = 0;
        } else if (type == EASTER_EGG_DRIFT_PHYSICS) {
            _5cFEng_mInstance->QueueGameMessage(0x98257537, nullptr, 0xff);
        } else if (type == EASTER_EGG_TRACK) {
            _5cFEng_mInstance->QueueGameMessage(0x6521e5c2, nullptr, 0xff);
            FEDatabase->GetPlayerCarStable(0)->AwardBonusCars();
        } else if (type == EASTER_EGG_SPECIAL) {
            _5cFEng_mInstance->QueueGameMessage(0xa6813b08, nullptr, 0xff);
            FEDatabase->GetCareerSettings()->TryAwardDemoMarker();
        }
    }
}

void EasterEggs::HandleJoy() {
    if (ButtonsEnabled == false) {
        return;
    }
    for (int i = 0; i < 2; i++) {
        if (EasterEggActionQ[i] == nullptr) {
            continue;
        }
        while (!EasterEggActionQ[i]->IsEmpty()) {
            ActionRef action = EasterEggActionQ[i]->GetAction();
            float data = action.Data();
            if (data == 1.0f && NumberOfCurrentButtons < 8) {
                int id = action.ID();
                ButtonBuffer[(CurrentStartButton + NumberOfCurrentButtons) & 7] =
                    static_cast<EasterEggButtons>(id);
                NumberOfCurrentButtons++;
                if (NumberOfCurrentButtons == 8) {
                    bool found = false;
                    unsigned int j = 0;
                    while (j < NumberOfEasterEggs) {
                        found = true;
                        unsigned int k = 0;
                        while (EasterEggsTable[j].buttons[k] ==
                               ButtonBuffer[(CurrentStartButton + k) & 7]) {
                            k++;
                            if (k > 7) {
                                goto matched;
                            }
                        }
                        found = false;
                    matched:
                        if (found) {
                            ActivateEasterEgg(j);
                            break;
                        }
                        j++;
                    }
                    if (!found) {
                        CurrentStartButton = (CurrentStartButton + 1) & 7;
                        NumberOfCurrentButtons--;
                    }
                }
            }
            EasterEggActionQ[i]->PopAction();
        }
    }
}

void EasterEggs::ClearButtons() {
    NumberOfCurrentButtons = 0;
}

bool EasterEggs::IsEasterEggUnlocked(unsigned int type, unsigned int item) {
    for (unsigned int i = 0; i < NumberOfEasterEggs; i++) {
        if (EasterEggsTable[i].type == type && EasterEggsTable[i].item == item) {
            return EasterEggsTable[i].unlocked;
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
    for (unsigned int i = 0; i < NumberOfEasterEggs; i++) {
        if (EasterEggsTable[i].group == group) {
            EasterEggsTable[i].unlocked = false;
        }
    }
}

void EasterEggs::TriggerSpecial(unsigned int special) {
}
