#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeBustedOverlay.hpp"

#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GInfractionManager.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"

extern int FEPrintf(const char *pkg_name, int object_hash, const char *fmt, ...);
extern void FEngSetLanguageHash(const char *pkg_name, unsigned int object_hash, unsigned int language_hash);
const char *GetLocalizedString(unsigned int hash);

BustedOverlayScreen::BustedOverlayScreen(ScreenConstructorData *sd) : MenuScreen(sd) {
    if (INIS::Get()) {
        if (INIS::Get()->GetType() == CAnimChooser::Arrest) {
            PursuitData &pd = PostRacePursuitScreen::GetPursuitData();
            unsigned int languageHashToUse = 0x626ac043;
            if (pd.mNumRoadblocksDodged >= 2) {
                languageHashToUse = 0x626ac042;
                if (pd.mNumSpikeStripsDodged > 1) {
                    languageHashToUse = 0x626ac045;
                }
            } else if (pd.mNumCopsDestroyed >= 2) {
                languageHashToUse = 0x626ac044;
            } else if (static_cast<float>(pd.mCostToStateAchieved) > 10000.0f) {
                languageHashToUse = 0x626ac046;
            }
            Timer finishTimer(pd.mPursuitLength);
            char finishTimerString[32];
            finishTimer.PrintToString(finishTimerString, 0);
            switch (languageHashToUse) {
            case 0x626ac042:
                FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a),
                         GetLocalizedString(0x626ac042),
                         pd.mNumCopsDamaged, pd.mNumCopsDestroyed,
                         pd.mNumRoadblocksDodged, pd.mCostToStateAchieved,
                         finishTimerString);
                break;
            case 0x626ac043:
                FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a),
                         GetLocalizedString(0x626ac043),
                         finishTimerString, GInfractionManager::Get().GetNumInfractions());
                break;
            case 0x626ac044:
                FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a),
                         GetLocalizedString(0x626ac044),
                         finishTimerString, pd.mNumCopsDamaged, pd.mNumCopsDestroyed,
                         GInfractionManager::Get().GetNumInfractions());
                break;
            case 0x626ac045:
                FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a),
                         GetLocalizedString(0x626ac045),
                         finishTimerString, pd.mNumSpikeStripsDodged, pd.mNumRoadblocksDodged,
                         GInfractionManager::Get().GetNumInfractions());
                break;
            case 0x626ac046:
                FEPrintf(GetPackageName(), static_cast<int>(0xf1ed545a),
                         GetLocalizedString(0x626ac046),
                         finishTimerString, pd.mCostToStateAchieved,
                         GInfractionManager::Get().GetNumInfractions());
                break;
            }
        } else if (INIS::Get()->GetType() == CAnimChooser::Intro) {
            FEngSetLanguageHash(GetPackageName(), 0xf1ed545a, 0x626ac047);
        }
    }
}

BustedOverlayScreen::~BustedOverlayScreen() {}