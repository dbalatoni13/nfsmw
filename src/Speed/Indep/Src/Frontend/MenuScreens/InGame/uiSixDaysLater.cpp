#include "uiSixDaysLater.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMessageDone.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"

SixDaysLater::SixDaysLater(ScreenConstructorData *sd) : MenuScreen(sd) {
    mStringMode = sd->Arg;
    mpDataMainString = FEngFindString(GetPackageName(), 0xb769701e);
    FEngSetLanguageHash(mpDataMainString, FEngHashString("DDAY_TIMELAPSE_%d", mStringMode + 1));
    new EFadeScreenOff(0x14035fb);
}

void SixDaysLater::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    if (msg == 0xC98356BA) {
        if (FEngIsScriptSet(mpDataMainString, 0x5079c8f8)) {
            if (!FEngIsScriptSet(GetPackageName(), 0x53d9eb7e, 0x5079c8f8)) {
                FEngSetScript(GetPackageName(), 0x53d9eb7e, 0x5079c8f8, true);
            }
        }
        if (FEngIsScriptSet(mpDataMainString, 0x5a8e4ebe)) {
            if (!FEngIsScriptRunning(mpDataMainString, 0x5a8e4ebe)) {
                cFEng::Get()->QueuePackagePop(0);
                UCrc32 target(0x20d60dbf);
                MNotifyMessageDone done;
                done.Post(target);
            }
        }
    }
}
