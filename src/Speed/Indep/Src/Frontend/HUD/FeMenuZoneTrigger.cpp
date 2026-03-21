#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Gameplay/GActivity.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Generated/Events/EQuitToFE.hpp"
#include "Speed/Indep/Src/Generated/Events/ERequestEventInfoDialog.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Messages/MEnterSafeHouse.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern cFrontendDatabase *FEDatabase;

FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
void FEngSetScript(FEObject *obj, unsigned int script_hash, bool play);
void FEngSetTextureHash(FEImage *img, unsigned int hash);
void BeginCarCustomize(eCustomizeEntryPoint entry, FECarRecord *record);

MenuZoneTrigger::MenuZoneTrigger(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x400000) //
    , IMenuZoneTrigger(pOutter)
{
    mCingularTimer = 0;
    mbInsideTrigger = false;
    mbCingularQueued = false;
    mpRaceActivity = nullptr;
    mZoneType = nullptr;
    mEngageMechanic = RegisterGroup(FEHashUpper("Engage_Mechanic"));
    mEventIcon = RegisterImage(FEHashUpper("EventIcon"));
    mCingularIcon = RegisterGroup(0xDA8141D4);
}

void MenuZoneTrigger::Update(IPlayer *player) {
    if (mbCingularQueued) {
        return;
    }
    if (mCingularTimer.IsSet()) {
        Timer diff = WorldTimer - mCingularTimer;
        if (diff.GetSeconds() >= 6.0f) {
            mCingularTimer.UnSet();
            if (!mbInsideTrigger) {
                HideDPadButton();
            } else {
                PulseDPadButton(ENGAGE_DPAD_ELEMENT_UP, mEventIcon);
            }
        }
    }
}

bool MenuZoneTrigger::IsPlayerInsideTrigger() {
    return FEngIsScriptSet(mEventIcon, 0x280164f);
}

void MenuZoneTrigger::ExitTrigger() {
    mpRaceActivity = nullptr;
    mbInsideTrigger = false;
    mZoneType = nullptr;
    HideDPadButton();
}

void MenuZoneTrigger::EnterTrigger(GRuntimeInstance *pRaceActivity) {
    mpRaceActivity = pRaceActivity;
    mbInsideTrigger = true;
    PulseDPadButton(ENGAGE_DPAD_ELEMENT_UP, mEventIcon);
    GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(static_cast<GActivity*>(mpRaceActivity));
    FEngSetTextureHash(mEventIcon, FEDatabase->GetRaceIconHash(parms->GetRaceType()));
}

void MenuZoneTrigger::EnterTrigger(const char *zoneType) {
    mZoneType = zoneType;
    mbInsideTrigger = true;
    PulseDPadButton(ENGAGE_DPAD_ELEMENT_UP, mEventIcon);
    FEngSetTextureHash(mEventIcon, FEDatabase->GetSafehouseIconHash(zoneType));
}

void MenuZoneTrigger::RequestEventInfoDialog(int port) {
    if (mpRaceActivity) {
        GRaceParameters *parms = GRaceDatabase::Get().GetRaceFromActivity(static_cast<GActivity*>(mpRaceActivity));
        if (!parms || !parms->GetIsBossRace()) {
            new ERequestEventInfoDialog(port, mpRaceActivity);
        } else {
            new ERaceSheetOn(3);
        }
    }
}

void MenuZoneTrigger::RequestZoneInfoDialog(int port) {
    if (bStrCmp(mZoneType, "safehouse") == 0 ||
        bStrCmp(mZoneType, "carlot") == 0 ||
        bStrCmp(mZoneType, "customshop") == 0) {
        MEnterSafeHouse msg(mZoneType);
        msg.Post(0x20D60DBF);
    }
}

void MenuZoneTrigger::RequestDoAction() {
    if (bStrCmp(mZoneType, "safehouse") == 0) {
        new EQuitToFE(static_cast<eGarageType>(2), "MainMenu_Sub.fng");
    } else if (bStrCmp(mZoneType, "carlot") == 0) {
        new EQuitToFE(static_cast<eGarageType>(5), "Car_Select.fng");
    } else if (bStrCmp(mZoneType, "customshop") == 0) {
        FECarRecord *rec = FEDatabase->GetPlayerCarStable(0)->GetCarRecordByHandle(FEDatabase->GetCareerSettings()->GetCurrentCar());
        if (rec->IsCustomized()) {
            BeginCarCustomize(static_cast<eCustomizeEntryPoint>(0), rec);
            new EQuitToFE(static_cast<eGarageType>(3), "CustomizeMain.fng");
        }
    }
}

void MenuZoneTrigger::HideDPadButton() {
    FEObject *objectPtr;
    if (!FEngIsScriptSet(mEventIcon, 0x33113AC) && !FEngIsScriptSet(mEventIcon, 0x16A259)) {
        FEngSetScript(mEventIcon, 0x33113AC, true);
    }
    if (!FEngIsScriptSet(mCingularIcon, 0x33113AC)) {
        if (!FEngIsScriptSet(mCingularIcon, 0x16A259)) {
            FEngSetScript(mCingularIcon, 0x33113AC, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0xA729B1B);
    if (objectPtr) {
        if (!FEngIsScriptSet(objectPtr, 0x33113AC) && !FEngIsScriptSet(objectPtr, 0x1744B3)) {
            FEngSetScript(objectPtr, 0x33113AC, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0x717C82AE);
    if (objectPtr) {
        if (!FEngIsScriptSet(objectPtr, 0x33113AC) && !FEngIsScriptSet(objectPtr, 0x1744B3)) {
            FEngSetScript(objectPtr, 0x33113AC, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0xA206A0B4);
    if (objectPtr) {
        if (!FEngIsScriptSet(objectPtr, 0x33113AC) && !FEngIsScriptSet(objectPtr, 0x1744B3)) {
            FEngSetScript(objectPtr, 0x33113AC, true);
        }
    }
    objectPtr = FEngFindObject(GetPackageName(), 0x7180B901);
    if (objectPtr) {
        if (!FEngIsScriptSet(objectPtr, 0x33113AC) && !FEngIsScriptSet(objectPtr, 0x1744B3)) {
            FEngSetScript(objectPtr, 0x33113AC, true);
        }
    }
    if (FEngIsScriptSet(mEngageMechanic, 0x5079C8F8)) {
        FEngSetScript(mEngageMechanic, 0x33113AC, true);
    }
}

void MenuZoneTrigger::PulseDPadButton(ENGAGE_DPAD_ELEMENT_DIRECTION direction, FEObject *iconToShow) {
    HideDPadButton();
    if (iconToShow && !FEngIsScriptSet(iconToShow, 0x5079C8F8) && !FEngIsScriptSet(iconToShow, 0x280164F)) {
        FEngSetScript(iconToShow, 0x5079C8F8, true);
    }
    if (direction == ENGAGE_DPAD_ELEMENT_NONE) {
        if (!FEngIsScriptSet(mEngageMechanic, 0x5079C8F8)) {
            FEngSetScript(mEngageMechanic, 0x5079C8F8, true);
            g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x13));
        }
    } else {
        unsigned int hash = 0;
        if (direction == ENGAGE_DPAD_ELEMENT_DOWN) {
            hash = 0x717C82AE;
        } else if (direction == ENGAGE_DPAD_ELEMENT_UP) {
            hash = 0xA729B1B;
        }
        if (hash) {
            FEObject *btn = FEngFindObject(pPackageName, hash);
            if (btn) {
                if (!FEngIsScriptSet(btn, 0x5079C8F8) && !FEngIsScriptSet(btn, 0x280164F)) {
                    FEngSetScript(btn, 0x5079C8F8, true);
                    g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0x13));
                }
            }
        }
    }
}

bool MenuZoneTrigger::IsType(const char *t) {
    return bStrCmp(mZoneType, t) == 0;
}

bool MenuZoneTrigger::ShouldSeeMenuZoneCluster() {
    return *reinterpret_cast<int *>(reinterpret_cast<char *>(&GRaceStatus::Get()) + 0x1AA4) == 0;
}
