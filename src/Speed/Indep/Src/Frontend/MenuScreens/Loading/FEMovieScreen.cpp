
// MovieScreen destructor
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEMovieScreen.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEMovies.hpp"
#include "Speed/Indep/Src/Generated/Events/ESndGameState.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEBootFlowManager.hpp"

static int IsDebugPlayMovie;

struct MovieScreenData {
    char *PackageName;
    char *MovieToPlay;
    uint32 ObjectHash;
    bool Bypassable;
    bool HasSubtitles;
};

static MovieScreenData MovieData[10] = {
    {"LS_EALogo.fng", "ealogo", 0x58BCF5B6, false, false},
    {"LS_EA_hidef.fng", "eahd_bumper", 0x58BCF5B6, false, false},
    {"LS_PSA.fng", "psa", 0x58BCF5B6, true, true},
    {"MW_LS_IntroFMV.fng", "intro_movie", 0x72CF9F38, true, false},
    {"MW_LS_AttractFMV.fng", "attract_movie", 0x72CF9F38, true, false},
    {"WS_LS_EALogo.fng", "ealogo", 0x58BCF5B6, false, false},
    {"WS_LS_EA_hidef.fng", "eahd_bumper", 0x58BCF5B6, false, false},
    {"WS_LS_PSA.fng", "psa", 0x58BCF5B6, true, true},
    {"WS_LS_IntroFMV.fng", "intro_movie", 0x58BCF5B6, true, false},
    {"WS_MW_LS_AttractFMV.fng", "attract_movie", 0x72CF9F38, true, false},
};

MovieScreen::MovieScreen(ScreenConstructorData *sd) : MenuScreen(sd), bByPassable(false), mSubtitler() {
    new ESndGameState(1, true);

    void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);
    SetSoundControlState(true, SNDSTATE_FMV, "MovieScreen");
    for (int i = 0; i < 10; i++) {
        if (bStrICmp(GetPackageName(), MovieData[i].PackageName) == 0) {
            bByPassable = MovieData[i].Bypassable;
            FEngSetMovieName(GetPackageName(), MovieData[i].ObjectHash, MovieData[i].MovieToPlay);
            if (MovieData[i].HasSubtitles) {
                mSubtitler.BeginningMovie(MovieData[i].MovieToPlay, GetPackageName());
            }
        }
    }
}

void MovieScreen::NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) {
    mSubtitler.Update(msg);
    bool movie_is_finished = false;

    void SetSoundControlState(bool bON, eSNDCTLSTATE esndstate, const char *Reason);
    switch (msg) {
        case 0xc3960eb9:
            new ESndGameState(1, false);
            SetSoundControlState(false, SNDSTATE_FMV, "MovieScreen");
            if (IsDebugPlayMovie) {
                cFEng::Get()->QueuePackagePop(1);
                IsDebugPlayMovie = 0;
                movie_is_finished = true;
            } else {
                movie_is_finished = true;
                BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
            }
            break;
        case __PAD_START__:
        case __PAD_ACCEPT__: {
            bool bypassable = bByPassable;

            if (bypassable) {
                new ESndGameState(1, false);
                SetSoundControlState(false, SNDSTATE_FMV, "MovieScreen");
                if (IsDebugPlayMovie) {
                    movie_is_finished = true;
                    cFEng::Get()->QueuePackagePop(1);
                    IsDebugPlayMovie = 0;
                } else {
                    movie_is_finished = true;
                    BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
                }
            }

            break;
        }
    }

    if (movie_is_finished) {
        mSubtitler.Update(0xc3960eb9);
    }
}
