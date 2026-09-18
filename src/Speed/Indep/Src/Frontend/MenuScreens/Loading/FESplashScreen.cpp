#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FESplashScreen.hpp"

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Generated/LanguageHashes.hpp"

float SplashScreenMovieTimeout = 30.0f;
float SplashScreenTotalTimeout = 0.0f;

SplashScreen::SplashScreen(ScreenConstructorData *sd)
    : MenuScreen(sd), bAllowContinue(false), CopyrightNotice(), SplashStartedTimer() {
    const u32 FEObj_HDGROUP = 0x534CC377;
    const u32 FEObj_startclick = 0xC4DF3FF2;
    const u32 FEObj_mouseclick = 0x13CF446D;
    const u32 FEObj_mousebutton = 0x8C0BD743;
    const u32 FEObj_LicenseBlurb = 0x4B98C4B9;
    FEObject *pObject;

    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }

    FEngSetInvisible(FEngFindObject(GetPackageName(), FEObj_HDGROUP));

    {
        const u32 FEObj_ESRBicon = 0x43D41F73;
        if (GetVideoMode() == MODE_PAL) {
            FEngSetInvisible(FEngFindObject(GetPackageName(), FEObj_ESRBicon));
        }
    }

    FEngSetVisible(FEngFindObject(GetPackageName(), FEObj_startclick));
    FEngSetInvisible(FEngFindObject(GetPackageName(), FEObj_mouseclick));
    FEngSetInvisible(FEngFindObject(GetPackageName(), FEObj_mousebutton));
    pObject = FEngFindObject(GetPackageName(), FEObj_mousebutton);
    FEngSetInvisible(FEngFindObject(GetPackageName(), FEObj_LicenseBlurb));
    FEngSetVisible(FEngFindObject(GetPackageName(), FEObj_LicenseBlurb));
    FEngSetLanguageHash(GetPackageName(), FEObj_LicenseBlurb, 0x9BA134FC);
    FEngSetLanguageHash(GetPackageName(), FEObj_startclick, 0x9B580A55);

    if (pObject != nullptr) {
        if (pObject->Flags & FF_IsButton) {
            pObject->Flags &= ~FF_IsButton;
        }
        pObject->Flags |= FF_DirtyCode;
    }

    SplashStartedTimer = RealTimer;
    CopyrightNotice = RealTimer;
    MControlPathfinder(false, 0x10, 0, 0).Send("Event");
    gEasterEggs.Activate();

    if (!CarViewer::haveLoadedOnce) {
        RideInfo ride;
        FEDatabase->BuildCurrentRideForPlayer(0, &ride);
        CarViewer::SetRideInfo(&ride, SET_RIDE_INFO_REASON_CATCHALL, eCARVIEWER_PLAYER1_CAR);
        CarViewer::ShowCarScreen();
        CarViewer::haveLoadedOnce = true;
    }
}

SplashScreen::~SplashScreen() {
    gEasterEggs.UnActivate();
    MControlPathfinder(false, 9, 0, 0).Send("Event");
}

Timer SplashScreen::CalculateLastJoyEventTime() {
    Timer lowesttimer;
    lowesttimer.ResetLow();

    for (ActionQueue *const *iter = ActionQueue::GetList().begin(); iter != ActionQueue::GetList().end(); iter++) {
        ActionQueue *q = *iter;
        if (q->IsConnected() && q->IsEnabled() &&
            bStrICmp(*reinterpret_cast<char **>(reinterpret_cast<char *>(q) + 0x27C), "FEng") == 0) {
            if (!lowesttimer.IsSet() || (*reinterpret_cast<int *>(reinterpret_cast<char *>(q) + 0x28C)) > lowesttimer.GetPackedTime()) {
                lowesttimer.SetPackedTime(*reinterpret_cast<int *>(reinterpret_cast<char *>(q) + 0x28C));
            }
        }
    }

    if (SplashStartedTimer > lowesttimer) {
        lowesttimer = SplashStartedTimer;
    }
    return lowesttimer;
}

void SplashScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
    case FEHASH_BK_UNLOCK:
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x1FAB5998, 0x53F13FD1);
        break;
    case FEHASH_CASTROL_UNLOCK:
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x1FAB5998, FEHASH_CASTROL_UNLOCK);
        break;
    case FEHASH_DEMO_CHEAT:
        DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, LANGUAGE_COMMON_OK, 0x1FAB5998, 0xA1161AAF);
        break;
    case 0xC98356BA: {
        Timer last = CalculateLastJoyEventTime();
        bool bTimedOut = (RealTimer - last).GetSeconds() > SplashScreenMovieTimeout ||
                         (SplashScreenTotalTimeout != 0.0f &&
                          (RealTimer - SplashStartedTimer).GetSeconds() > SplashScreenTotalTimeout);
        int timed_out = bTimedOut;
#ifndef EA_BUILD_A124 // la alpha 124 no tiene TrackStreamer::IsPermFileLoading
        if (TheTrackStreamer.IsPermFileLoading()) {
            timed_out = 0;
        }
#endif
        if (timed_out != 0) {
            if (!BootFlowManager::Get()->DoAttract()) {
                SplashStartedTimer.ResetHigh();
            }
        }
        break;
    }
    case 0x406415E3:
    case 0xB5AF2461:
        if (bAllowContinue) {
            BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xFF);
        }
        break;
    case 0x35F8620B:
        bAllowContinue = true;
        break;
    default:
        break;
    }
}
