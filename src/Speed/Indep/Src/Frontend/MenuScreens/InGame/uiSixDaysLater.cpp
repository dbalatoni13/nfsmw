#include "uiSixDaysLater.hpp"

#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMessageDone.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

SixDaysLater::SixDaysLater(ScreenConstructorData *sd) : MenuScreen(sd), mStringMode(sd->Arg) {
    mpDataMainString = FEngFindString(GetPackageName(), 0xb769701e);
    FEngSetLanguageHash(mpDataMainString, FEngHashString("DDAY_TIMELAPSE_%d", mStringMode + 1));
    new EFadeScreenOff(FEHASH_15_IN);
}

void SixDaysLater::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == FEMSG_SCREEN_TICK) {
        if (FEngIsScriptSet(mpDataMainString, FEHASH_APPEAR)) {
            if (!FEngIsScriptSet(GetPackageName(), 0x53d9eb7e, FEHASH_APPEAR)) {
                FEngSetScript(GetPackageName(), 0x53d9eb7e, FEHASH_APPEAR, true);
            }
        }
        if (FEngIsScriptSet(mpDataMainString, FEHASH_ANIMATE)) {
            if (!FEngIsScriptRunning(mpDataMainString, FEHASH_ANIMATE)) {
                cFEng::Get()->QueuePackagePop(0);
                MNotifyMessageDone().Post(0x20d60dbf);
            }
        }
    }
}
