#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FESplashScreen.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"

SplashScreen::SplashScreen(ScreenConstructorData *sd)
    : MenuScreen(sd),        //
      bAllowContinue(false), //
      CopyrightNotice(),     //
      SplashStartedTimer() {
    const u32 FEObj_HDGROUP = 0x534cc377;
    const u32 FEObj_startclick = 0x13cf446d;
    const u32 FEObj_mouseclick = 0x8c0bd743;
    const u32 FEObj_mousebutton = 0x4b98c4b9;
    FEObject *pObject;
    const u32 FEObj_LicenseBlurb = 0xc4df3ff2;

    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }

    FEngSetInvisible(GetPackageName(), FEObj_HDGROUP);

    if (GetVideoMode() == 0) {
        const u32 FEObj_ESRBicon = 0x43d41f73;
        FEngSetInvisible(GetPackageName(), FEObj_ESRBicon);
    }

    FEngSetVisible(GetPackageName(), FEObj_LicenseBlurb);
    FEngSetInvisible(GetPackageName(), FEObj_startclick);
    FEngSetInvisible(GetPackageName(), FEObj_mouseclick);

    pObject = FEngFindObject(GetPackageName(), FEObj_mouseclick);

    FEngSetInvisible(GetPackageName(), FEObj_mousebutton);
    FEngSetVisible(GetPackageName(), FEObj_mousebutton);

    FEngSetLanguageHash(GetPackageName(), FEObj_mousebutton, 0x9ba134fc);
    FEngSetLanguageHash(GetPackageName(), FEObj_LicenseBlurb, 0x9b580a55);

    if (pObject != nullptr) {
        if ((pObject->Flags & FF_IsButton) != 0) {
            pObject->Flags &= ~FF_IsButton;
        }
        pObject->Flags |= FF_DirtyCode;
    }

    SplashStartedTimer = RealTimer;
    CopyrightNotice = RealTimer;

    MControlPathfinder(false, 16, 0, 0).Send("Event");

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
    for (ActionQueue *const *iter = UTL::Collections::Listable<ActionQueue, 20>::GetList().begin();
         iter != UTL::Collections::Listable<ActionQueue, 20>::GetList().end(); iter++) {
        ActionQueue *q = *iter;
        if (q->IsConnected() && q->IsEnabled() && bStrICmp(q->GetName(), "FEng") == 0) {
            if ((lowesttimer.IsSet() == 0) || ((q->LastActionTime() > lowesttimer) != 0)) {
                lowesttimer = q->LastActionTime();
            }
        }
    }
    if (SplashStartedTimer > lowesttimer) {
        lowesttimer = SplashStartedTimer;
    }
    return lowesttimer;
}

float SplashScreenMovieTimeout = 30.0f;
float SplashScreenTotalTimeout = 0.0f;

// UNSOLVED
void SplashScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    switch (msg) {
        case 0x98257537:
            DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, 0x1fab5998, 0x53f13fd1);
            break;
        case 0x6521e5c2:
            DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, 0x1fab5998, 0x6521e5c2);
            break;
        case 0xa6813b08:
            DialogInterface::ShowOneButton(GetPackageName(), "", dialog_alert, 0x417b2601, 0x1fab5998, 0xa1161aaf);
            break;
        case FEMSG_SCREEN_TICK: {
            bool timed_out = ((RealTimer - CalculateLastJoyEventTime()).GetSeconds() > SplashScreenMovieTimeout ||
                              (SplashScreenTotalTimeout != 0 && (RealTimer - SplashStartedTimer).GetSeconds() > SplashScreenTotalTimeout));

            if (TheTrackStreamer.IsPermFileLoading()) {
                timed_out = false;
            }

            if (timed_out) {
                if (!BootFlowManager::Get()->DoAttract()) {
                    SplashStartedTimer.ResetHigh();
                }
            }
            break;
        }
        case __PAD_ACCEPT__:
        case __PAD_START__:
            if (bAllowContinue) {
                BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
            }
            break;
        case FEHASH_INITCOMPLETE:
            bAllowContinue = true;
            break;
    }
}
