#include "Speed/Indep/Src/Frontend/HUD/FeShiftUpdater.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"

extern float warningPulseMinRpm;

ShiftUpdater::ShiftUpdater(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x20000000), IShiftUpdater(pOutter) //
{
    mGear = G_NEUTRAL;
    mShiftPotential = SHIFT_POTENTIAL_NONE;
    mGearChanged = 0;
    mLastShiftStatus = SHIFT_STATUS_NONE;
    mIsEngineBlown = false;
    mEngineTemp = 0.0f;
    pShiftIndicator = RegisterImage(FEHashUpper("Shift_light"));
    pShiftIndicatorOverheatGroup = RegisterGroup(FEHashUpper("ENGINE_HEAT_SHIFTLIGHT_GROUP"));
    pShiftMsgGroup = RegisterGroup(FEHashUpper("SHIFT_GROUP"));
    pShiftMsg = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("ShiftMessage")));
    pShiftMsgShadow = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("ShiftMessage_Shadow")));
}

void ShiftUpdater::Update(IPlayer *player) {
    ShiftPotential potential = static_cast<ShiftPotential>(mShiftPotential);
    if (potential < SHIFT_POTENTIAL_GOOD || mIsEngineBlown) {
        FEngSetInvisible(reinterpret_cast<FEObject *>(pShiftIndicator));
        FEngSetInvisible(pShiftIndicatorOverheatGroup);
    } else if (potential == SHIFT_POTENTIAL_GOOD) {
        const char *scriptName = "PulseBlue";
        FEObject *obj = reinterpret_cast<FEObject *>(pShiftIndicator);
        unsigned long hash = FEHashUpper(scriptName);
        if (!FEngIsScriptSet(obj, hash)) {
            FEngSetScript(reinterpret_cast<FEObject *>(pShiftIndicator), FEHashUpper(scriptName), true);
        }
        FEngSetVisible(reinterpret_cast<FEObject *>(pShiftIndicator));
        FEngSetInvisible(reinterpret_cast<FEObject *>(pShiftIndicatorOverheatGroup));
    } else if (potential == SHIFT_POTENTIAL_PERFECT) {
        const char *scriptName = "PulseGreen";
        FEObject *obj = reinterpret_cast<FEObject *>(pShiftIndicator);
        unsigned long hash = FEHashUpper(scriptName);
        if (!FEngIsScriptSet(obj, hash)) {
            FEngSetScript(reinterpret_cast<FEObject *>(pShiftIndicator), FEHashUpper(scriptName), true);
        }
        FEngSetVisible(reinterpret_cast<FEObject *>(pShiftIndicator));
        FEngSetInvisible(pShiftIndicatorOverheatGroup);
    } else if (potential == SHIFT_POTENTIAL_MISS) {
        if (mEngineTemp <= warningPulseMinRpm) {
            const char *scriptName = "PulseRed";
            FEObject *obj = reinterpret_cast<FEObject *>(pShiftIndicator);
            unsigned long hash = FEHashUpper(scriptName);
            if (!FEngIsScriptSet(obj, hash)) {
                FEngSetScript(reinterpret_cast<FEObject *>(pShiftIndicator), FEHashUpper(scriptName), true);
            }
            FEngSetVisible(reinterpret_cast<FEObject *>(pShiftIndicator));
            FEngSetInvisible(pShiftIndicatorOverheatGroup);
        } else {
            unsigned long hash = FEHashUpper("OVERHEAT_PULSE");
            if (!FEngIsScriptSet(pShiftIndicatorOverheatGroup, hash)) {
                FEngSetScript(pShiftIndicatorOverheatGroup, FEHashUpper("OVERHEAT_PULSE"), true);
            }
            FEngSetInvisible(reinterpret_cast<FEObject *>(pShiftIndicator));
            FEngSetVisible(pShiftIndicatorOverheatGroup);
        }
    }

    if (mGear < G_SECOND) {
        return;
    }
    if (mGearChanged < 1) {
        return;
    }
    ShiftStatus status = static_cast<ShiftStatus>(mLastShiftStatus);
    if (status < SHIFT_STATUS_NORMAL) {
        return;
    }
    if (mIsEngineBlown) {
        return;
    }

    if (status == SHIFT_STATUS_NORMAL) {
        FEngSetLanguageHash(pShiftMsg, 0x2202b5b9);
        FEngSetLanguageHash(pShiftMsgShadow, 0x2202b5b9);
        FEngSetScript(pShiftMsgGroup, FEHashUpper("EARLY"), true);
    } else if (status == SHIFT_STATUS_GOOD) {
        FEngSetLanguageHash(pShiftMsg, 0x27d2dd45);
        FEngSetLanguageHash(pShiftMsgShadow, 0x27d2dd45);
        FEngSetScript(pShiftMsgGroup, FEHashUpper("GOOD"), true);
    } else if (status == SHIFT_STATUS_PERFECT) {
        FEngSetLanguageHash(pShiftMsg, 0x598b065);
        FEngSetLanguageHash(pShiftMsgShadow, 0x598b065);
        FEngSetScript(pShiftMsgGroup, FEHashUpper("PERFECT"), true);
    } else if (status == SHIFT_STATUS_MISSED) {
        FEngSetLanguageHash(pShiftMsg, 0xdf61b3e5);
        FEngSetLanguageHash(pShiftMsgShadow, 0xdf61b3e5);
        FEngSetScript(pShiftMsgGroup, FEHashUpper("OVERREV"), true);
    }

    FEngSetInvisible(reinterpret_cast<FEObject *>(pShiftIndicator));
    mGearChanged = 0;
}
