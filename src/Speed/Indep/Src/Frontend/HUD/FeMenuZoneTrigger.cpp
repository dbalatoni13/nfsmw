#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/FECustomize.hpp"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERequestEventInfoDialog.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Messages/MEnterSafeHouse.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

MenuZoneTrigger::MenuZoneTrigger(UTL::COM::Object *pOuter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x400000), IMenuZoneTrigger(pOuter), mCingularTimer(), mbInsideTrigger(false), mbCingularQueued(false),
      mpRaceActivity(nullptr), mZoneType(nullptr) {
    mEngageMechanic = RegisterGroup(FEHashUpper("Engage_Mechanic"));
    mEventIcon = RegisterImage(FEHashUpper("EventIcon"));
    mCingularIcon = RegisterGroup(0xDA8141D4);
}

void MenuZoneTrigger::Update(IPlayer *player) {
    if (mbCingularQueued) {
        mbCingularQueued = false;
        mCingularTimer = WorldTimer;
        PulseDPadButton(ENGAGE_DPAD_ELEMENT_RIGHT, mCingularIcon);
    } else {
        if (mCingularTimer.IsSet()) {
            if ((WorldTimer - mCingularTimer).GetSeconds() >= 6.0f) {
                mCingularTimer.UnSet();
                if (mbInsideTrigger) {
                    PulseDPadButton(ENGAGE_DPAD_ELEMENT_UP, mEventIcon);
                } else {
                    HideDPadButton();
                }
            }
        }
    }
}

bool MenuZoneTrigger::ShouldSeeMenuZoneCluster() {
    return GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming;
}

bool MenuZoneTrigger::IsPlayerInsideTrigger() {
    return FEngIsScriptSet(mEventIcon, 0x280164f);
}

void MenuZoneTrigger::EnterTrigger(GRuntimeInstance *pRaceActivity) {
    mpRaceActivity = pRaceActivity;
    mbInsideTrigger = true;
    PulseDPadButton(ENGAGE_DPAD_ELEMENT_UP, mEventIcon);
    GActivity *activity = static_cast<GActivity *>(mpRaceActivity);
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(activity);
    FEngSetTextureHash(mEventIcon, FEDatabase->GetRaceIconHash(parms->GetRaceType()));
}

void MenuZoneTrigger::EnterTrigger(const char *zoneType) {
    mZoneType = zoneType;
    mbInsideTrigger = true;
    PulseDPadButton(ENGAGE_DPAD_ELEMENT_UP, mEventIcon);
    FEngSetTextureHash(mEventIcon, FEDatabase->GetSafehouseIconHash(zoneType));
}

void MenuZoneTrigger::ExitTrigger() {
    mbInsideTrigger = false;
    mZoneType = nullptr;
    mpRaceActivity = nullptr;
    HideDPadButton();
}

void MenuZoneTrigger::RequestEventInfoDialog(int port) {
    if (mpRaceActivity != nullptr) {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(static_cast<GActivity *>(mpRaceActivity));
        if ((parms != nullptr) && parms->GetIsBossRace()) {
            new ERaceSheetOn(3);
        } else {
            new ERequestEventInfoDialog(port, mpRaceActivity);
        }
    }
}

void MenuZoneTrigger::RequestZoneInfoDialog(int port) {
    if (bStrCmp(mZoneType, "safehouse") == 0 || bStrCmp(mZoneType, "carlot") == 0 || bStrCmp(mZoneType, "customshop") == 0) {
        MEnterSafeHouse(mZoneType).Post(0x20D60DBF);
    }
}

bool MenuZoneTrigger::IsType(const char *t) {
    return bStrCmp(mZoneType, t) == 0;
}

void MenuZoneTrigger::RequestDoAction() {
    if (bStrCmp(mZoneType, "safehouse") == 0) {
        new EQuitToFE(GARAGETYPE_CAREER_SAFEHOUSE, "MainMenu_Sub.fng");
    } else if (bStrCmp(mZoneType, "carlot") == 0) {
        new EQuitToFE(GARAGETYPE_CAR_LOT, "Car_Select.fng");
    } else if (bStrCmp(mZoneType, "customshop") == 0) {
        uint32 player_car = FEDatabase->GetCareerSettings()->GetCurrentCar();
        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
        FECarRecord *record = stable->GetCarRecordByHandle(player_car);
        if (record->IsCustomized()) {
            BeginCarCustomize(CEP_GAMEPLAY, record);
            new EQuitToFE(GARAGETYPE_CUSTOMIZATION_SHOP, "CustomizeMain.fng");
        }
    }
}

void MenuZoneTrigger::HideDPadButton() {
    FEObject *objectPtr;
    if (!FEngIsScriptSet(mEventIcon, FEHASH_LEAVE) && !FEngIsScriptSet(mEventIcon, FEHASH_HIDE)) {
        FEngSetScript(mEventIcon, FEHASH_LEAVE, true);
    }
    if (!FEngIsScriptSet(mCingularIcon, FEHASH_LEAVE)) {
        if (!FEngIsScriptSet(mCingularIcon, FEHASH_HIDE)) {
            FEngSetScript(mCingularIcon, FEHASH_LEAVE, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0xA729B1B);
    if (objectPtr != nullptr) {
        if (!FEngIsScriptSet(objectPtr, FEHASH_LEAVE) && !FEngIsScriptSet(objectPtr, FEHASH_INIT)) {
            FEngSetScript(objectPtr, FEHASH_LEAVE, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0x717C82AE);
    if (objectPtr != nullptr) {
        if (!FEngIsScriptSet(objectPtr, FEHASH_LEAVE) && !FEngIsScriptSet(objectPtr, FEHASH_INIT)) {
            FEngSetScript(objectPtr, FEHASH_LEAVE, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0xA206A0B4);
    if (objectPtr != nullptr) {
        if (!FEngIsScriptSet(objectPtr, FEHASH_LEAVE) && !FEngIsScriptSet(objectPtr, FEHASH_INIT)) {
            FEngSetScript(objectPtr, FEHASH_LEAVE, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0x7180B901);
    if (objectPtr != nullptr) {
        if (!FEngIsScriptSet(objectPtr, FEHASH_LEAVE) && !FEngIsScriptSet(objectPtr, FEHASH_INIT)) {
            FEngSetScript(objectPtr, FEHASH_LEAVE, true);
        }
    }
    if (FEngIsScriptSet(mEngageMechanic, FEHASH_APPEAR)) {
        FEngSetScript(mEngageMechanic, FEHASH_LEAVE, true);
    }
}

void MenuZoneTrigger::PulseDPadButton(ENGAGE_DPAD_ELEMENT_DIRECTION direction, FEObject *iconToShow) {
    HideDPadButton();
    if ((iconToShow != nullptr) && !FEngIsScriptSet(iconToShow, FEHASH_APPEAR) && !FEngIsScriptSet(iconToShow, 0x280164F)) {
        FEngSetScript(iconToShow, FEHASH_APPEAR, true);
    }
    if (direction != ENGAGE_DPAD_ELEMENT_NONE) {
        uint32 objectHash = 0;

        switch (direction) {
            case ENGAGE_DPAD_ELEMENT_UP:
                objectHash = 0xA729B1B;
                break;
            case ENGAGE_DPAD_ELEMENT_DOWN:
                objectHash = 0x717C82AE;
                break;
            case ENGAGE_DPAD_ELEMENT_RIGHT:
                objectHash = 0xa206a0b4;
                break;
            case ENGAGE_DPAD_ELEMENT_LEFT:
                objectHash = 0x7180b901;
                break;
        }

        FEObject *objectPtr = FEngFindObject(GetPackageName(), objectHash);
        if (objectPtr != nullptr) {
            if (!FEngIsScriptSet(objectPtr, FEHASH_APPEAR)) {
                FEngSetScript(objectPtr, FEHASH_APPEAR, true);
                g_pEAXSound->PlayUISoundFX(UISND_ENTER_TRIGGER);
            }
        }
    } else {
        if (!FEngIsScriptSet(mEngageMechanic, FEHASH_APPEAR)) {
            FEngSetScript(mEngageMechanic, FEHASH_APPEAR, true);
            g_pEAXSound->PlayUISoundFX(UISND_ENTER_TRIGGER);
        }
    }
}
