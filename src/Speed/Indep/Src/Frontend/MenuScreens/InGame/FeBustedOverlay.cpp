#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeBustedOverlay.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

BustedOverlayScreen::BustedOverlayScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    if (INIS::Get() != nullptr) {
        if (INIS::Get()->GetType() == CAnimChooser::Arrest) {

            int languageHashToUse = 0x626ac043;
            if (PostRacePursuitScreen::GetPursuitData().mNumRoadblocksDodged >= 2) {
                languageHashToUse = 0x626ac042;
                if (PostRacePursuitScreen::GetPursuitData().mNumSpikeStripsDodged > 1) {
                    languageHashToUse = 0x626ac045;
                }
            } else if (PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed >= 2) {
                languageHashToUse = 0x626ac044;
            } else if (static_cast<float>(PostRacePursuitScreen::GetPursuitData().mCostToStateAchieved) > 10000.0f) {
                languageHashToUse = 0x626ac046;
            }
            char finishTimerString[32];
            Timer finishTimer(PostRacePursuitScreen::GetPursuitData().mPursuitLength);
            finishTimer.PrintToString(finishTimerString, 0);
            switch (languageHashToUse) {
                case 0x626ac042:
                    FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a), GetLocalizedString(0x626ac042),
                             PostRacePursuitScreen::GetPursuitData().mNumCopsDamaged, PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed,
                             PostRacePursuitScreen::GetPursuitData().mNumRoadblocksDodged,
                             PostRacePursuitScreen::GetPursuitData().mCostToStateAchieved, finishTimerString);
                    break;
                case 0x626ac043:
                    FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a), GetLocalizedString(0x626ac043), finishTimerString,
                             GInfractionManager::Get().GetNumInfractions());
                    break;
                case 0x626ac044:
                    FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a), GetLocalizedString(0x626ac044), finishTimerString,
                             PostRacePursuitScreen::GetPursuitData().mNumCopsDamaged, PostRacePursuitScreen::GetPursuitData().mNumCopsDestroyed,
                             GInfractionManager::Get().GetNumInfractions());
                    break;
                case 0x626ac045:
                    FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a), GetLocalizedString(0x626ac045), finishTimerString,
                             PostRacePursuitScreen::GetPursuitData().mNumSpikeStripsDodged,
                             PostRacePursuitScreen::GetPursuitData().mNumRoadblocksDodged, GInfractionManager::Get().GetNumInfractions());
                    break;
                case 0x626ac046:
                    FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a), GetLocalizedString(0x626ac046), finishTimerString,
                             PostRacePursuitScreen::GetPursuitData().mCostToStateAchieved, GInfractionManager::Get().GetNumInfractions());
                    break;
            }
        } else if (INIS::Get()->GetType() == CAnimChooser::Intro) {
            FEngSetLanguageHash(GetPackageName(), 0xf1ed545a, 0x626ac047);
        }
    }
}

BustedOverlayScreen::~BustedOverlayScreen() {}
