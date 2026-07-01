#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FESplashScreen.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Generated/Messages/MControlPathfinder.h"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Frontend/FECarViewer.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feDialogBox.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"

SplashScreen::SplashScreen(ScreenConstructorData *sd)
    : MenuScreen(sd),        //
      bAllowContinue(false), //
      CopyrightNotice(0),    //
      SplashStartedTimer(0) {
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
        const unsigned long FEObj_ESRBicon = 0x43d41f73;
        FEngSetInvisible(GetPackageName(), FEObj_ESRBicon);
    }

    FEngSetVisible(FEngFindObject(GetPackageName(), FEObj_LicenseBlurb));
    FEngSetInvisible(GetPackageName(), FEObj_startclick);
    FEngSetInvisible(GetPackageName(), FEObj_mouseclick);

    pObject = FEngFindObject(GetPackageName(), FEObj_mouseclick);

    FEngSetInvisible(GetPackageName(), FEObj_mousebutton);
    FEngSetVisible(FEngFindObject(GetPackageName(), FEObj_mousebutton));

    FEngSetLanguageHash(GetPackageName(), FEObj_mousebutton, 0x9ba134fc);
    FEngSetLanguageHash(GetPackageName(), FEObj_LicenseBlurb, 0x9b580a55);

    if (pObject) {
        if ((pObject->Flags & FF_IsButton) != 0) {
            pObject->Flags &= ~FF_IsButton;
        }
        pObject->Flags |= FF_DirtyCode;
    }

    SplashStartedTimer = RealTimer;
    CopyrightNotice = RealTimer;

    {
        MControlPathfinder msg(false, 16, 0, 0);
        msg.Send("Event");
    }

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
    MControlPathfinder msg(false, 9, 0, 0);
    msg.Send("Event");
}

Timer SplashScreen::CalculateLastJoyEventTime() {
    Timer lowesttimer;
    lowesttimer.ResetLow();
    for (ActionQueue *const *iter = UTL::Collections::Listable<ActionQueue, 20>::GetList().begin();
         iter != UTL::Collections::Listable<ActionQueue, 20>::GetList().end(); ++iter) {
        ActionQueue *q = *iter;
        if (q->IsConnected() && q->IsEnabled() && bStrICmp(q->GetName(), "FEng") == 0) {
            if (!lowesttimer.IsSet() || q->LastActionTime() > lowesttimer) {
                lowesttimer = q->LastActionTime();
            }
        }
    }
    if (SplashStartedTimer > lowesttimer) {
        lowesttimer = SplashStartedTimer;
    }
    return lowesttimer;
}

extern float SplashScreenMovieTimeout;
extern float SplashScreenTotalTimeout;

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
        case 0xc98356ba: {
            Timer lastJoyTime = CalculateLastJoyEventTime();
            Timer elapsed = RealTimer - lastJoyTime;
            int timed_out = elapsed.GetSeconds() > SplashScreenMovieTimeout ||
                            (SplashScreenTotalTimeout != 0.0f && (RealTimer - SplashStartedTimer).GetSeconds() > SplashScreenTotalTimeout);
            int final_timed_out = timed_out;
            if (TheTrackStreamer.IsPermFileLoading()) {
                final_timed_out = 0;
            }
            if (final_timed_out != 0) {
                if (!BootFlowManager::Get()->DoAttract()) {
                    SplashStartedTimer.ResetHigh();
                }
            }
            break;
        }
        case 0x406415e3:
        case 0xb5af2461:
            if (bAllowContinue) {
                BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
            }
            break;
        case 0x35f8620b:
            bAllowContinue = true;
            break;
    }
}

float SplashScreenMovieTimeout = 8.0f;
float SplashScreenTotalTimeout = 0.0f;
