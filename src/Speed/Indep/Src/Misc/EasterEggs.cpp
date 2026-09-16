#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/controller_hash.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"

// TODO
// static const float nRenderEggChance;
// static const float fRenderEggMinOn;
// static const float fRenderEggMinOff;
Timer tRenderEggTimer;

EasterEggs gEasterEggs;

EasterEggs::EasterEggs() {
    static EasterEggsData StaticEasterEggsTable[2] = {
        {{UP, DN, UP, DN, LT, RT, LT, RT}, 3, 0, 0, 0, false, true, true},
        {{LT, RT, LT, RT, UP, DN, UP, DN}, 4, 0, 0, 0, false, true, true},
    };
    this->ButtonsEnabled = false;
    this->HaveUnlockMessage = false;
    this->UnlockMessage = 0;
    this->NumberOfCurrentButtons = 0;
    this->CurrentStartButton = 0;
    this->EasterEggsTable = StaticEasterEggsTable;
    this->NumberOfEasterEggs = 2;
}

EasterEggs::~EasterEggs() {}

void EasterEggs::Activate() {
    for (int i = 0; i < 2; i++) {
        this->EasterEggActionQ[i] = new (static_cast<const char *>(nullptr)) ActionQueue(i, Attrib::Hash::controller::key_fe, "movie q", false);
        this->EasterEggActionQ[i]->Enable(true);
    }
    this->ButtonsEnabled = true;
    this->HaveUnlockMessage = false;
    this->UnlockMessage = 0;
    this->ClearButtons();
}

void EasterEggs::UnActivate() {
    for (int i = 0; i < NUM_ELEMENTS(this->EasterEggActionQ); i++) {
        if (this->EasterEggActionQ[i] != nullptr) {
            delete this->EasterEggActionQ[i];
        }
    }
    this->ButtonsEnabled = false;
    this->HaveUnlockMessage = false;
    this->UnlockMessage = 0;
    this->ClearButtons();
}

void EasterEggs::ClearNonPersistent() {
    for (unsigned int egg = 0; egg < this->NumberOfEasterEggs; egg++) {
        if (this->EasterEggsTable[egg].persistent == false) {
            this->EasterEggsTable[egg].unlocked = false;
        }
    }
}

void EasterEggs::KeyboardInput(char key) {}

void EasterEggs::ActivateEasterEgg(int egg) {
    if (this->EasterEggsTable[egg].enabled == false) {
        return;
    }
    this->ClearGroup(this->EasterEggsTable[egg].group);
    bool was_unlocked = this->EasterEggsTable[egg].unlocked;
    this->EasterEggsTable[egg].unlocked = true;
    if (this->EasterEggsTable[egg].type == EASTER_EGG_SPECIAL) {
        this->TriggerSpecial(this->EasterEggsTable[egg].item);
    }
    this->HaveUnlockMessage = true;
    this->UnlockMessage = this->EasterEggsTable[egg].unlock_message;
    this->ClearButtons();
    g_pEAXSound->PlayUISoundFX(UISND_COMMON_SELECT);
    if (!was_unlocked) {
        if (this->EasterEggsTable[egg].type == EASTER_EGG_UNLOCK_ALL_THINGS) {
            UnlockAllThings = 1;
        } else if (this->EasterEggsTable[egg].type == EASTER_EGG_SKIP_DDAY) {
            SkipDDayRaces = 1;
            SkipCareerIntro = 1;
        } else if (this->EasterEggsTable[egg].type == EASTER_EGG_DISABLE_MIKE_MANN_BUILD) {
            extern int MikeMannBuild;
            MikeMannBuild = 0;
        } else if (this->EasterEggsTable[egg].type == EASTER_EGG_BURGER_KING) {
            cFEng::Get()->QueueGameMessage(FEHASH_BK_UNLOCK, nullptr, 0xff);
        } else if (this->EasterEggsTable[egg].type == EASTER_EGG_CASTROL) {
            cFEng::Get()->QueueGameMessage(FEHASH_CASTROL_UNLOCK, nullptr, 0xff);
            FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
            stable->AwardBonusCars();
        } else if (this->EasterEggsTable[egg].type == EASTER_EGG_DEMO_CHEAT) {
            cFEng::Get()->QueueGameMessage(FEHASH_DEMO_CHEAT, nullptr, 0xff);
            FEDatabase->GetCareerSettings()->TryAwardDemoMarker();
        }
    }
}

void EasterEggs::HandleJoy() {
    if (!this->ButtonsEnabled) {
        return;
    }
    for (int port = 0; port < NUM_ELEMENTS(this->EasterEggActionQ); port++) {
        if (this->EasterEggActionQ[port] == nullptr) {
            continue;
        }
        while (!this->EasterEggActionQ[port]->IsEmpty()) {
            ActionRef aRef = this->EasterEggActionQ[port]->GetAction();
            if (aRef.Data() == 1.0f && this->NumberOfCurrentButtons < 8) {
                uint32 button_buffer_index = (this->CurrentStartButton + this->NumberOfCurrentButtons) & 7;
                this->ButtonBuffer[button_buffer_index] = static_cast<EasterEggButtons>(aRef.ID());
                this->NumberOfCurrentButtons++;
                if (this->NumberOfCurrentButtons == 8) {
                    bool found = false;
                    for (uint32 egg = 0; egg < this->NumberOfEasterEggs; egg++) {
                        found = true;
                        for (uint32 button = 0; button < 8; button++) {
                            if (this->EasterEggsTable[egg].buttons[button] != this->ButtonBuffer[(this->CurrentStartButton + button) & 7]) {
                                found = false;
                                break;
                            }
                        }
                        if (found) {
                            this->ActivateEasterEgg(egg);
                            break;
                        }
                    }
                    if (!found) {
                        this->CurrentStartButton = (this->CurrentStartButton + 1) & 7;
                        this->NumberOfCurrentButtons--;
                    }
                }
            }
            this->EasterEggActionQ[port]->PopAction();
        }
    }
}

void EasterEggs::ClearButtons() {
    this->NumberOfCurrentButtons = 0;
}

// STRIPPED
bool EasterEggs::IsStockCarTypeUnlocked(int cartype) {}

// STRIPPED
bool EasterEggs::AreContestVinylsUnlocked() {}

// STRIPPED
bool EasterEggs::IsPresetUnlocked(unsigned int presethash) {}

// STRIPPED
bool EasterEggs::IsPartUnlocked(int level) {}

// STRIPPED
bool EasterEggs::IsPerfUnlocked(int level) {}

// STRIPPED
bool EasterEggs::IsManufacturerVinylUnlocked(unsigned int partnamehash) {}

// STRIPPED
void EasterEggs::PrintEasterEggsTable() {}

bool EasterEggs::IsEasterEggUnlocked(uint32 type, uint32 item) {
    for (uint32 egg = 0; egg < this->NumberOfEasterEggs; egg++) {
        if (this->EasterEggsTable[egg].type == type && this->EasterEggsTable[egg].item == item) {
            return this->EasterEggsTable[egg].unlocked;
        }
    }
    return false;
}

bool EasterEggs::IsEasterEggUnlocked(EasterEggsSpecial egg) {
    return this->IsEasterEggUnlocked(static_cast<uint32>(egg), 0);
}

void EasterEggs::ClearGroup(uint32 group) {
    if (group == 0) {
        return;
    }
    for (uint32 egg = 0; egg < this->NumberOfEasterEggs; egg++) {
        if (this->EasterEggsTable[egg].group == group) {
            this->EasterEggsTable[egg].unlocked = false;
        }
    }
}

void EasterEggs::TriggerSpecial(uint32 special) {}
