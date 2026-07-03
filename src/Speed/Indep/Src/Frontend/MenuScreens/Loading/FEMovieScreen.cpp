
// MovieScreen destructor
#include "Speed/Indep/Src/Frontend/MenuScreens/Loading/FEMovieScreen.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
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
    {"LS_EALogo.fng", "ealogo", 0x58BCF5B6, 0, 0},
    {"LS_EA_hidef.fng", "eahd_bumper", 0x58BCF5B6, 0, 0},
    {"LS_PSA.fng", "psa", 0x58BCF5B6, 1, 1},
    {"MW_LS_IntroFMV.fng", "intro_movie", 0x72CF9F38, 1, 0},
    {"MW_LS_AttractFMV.fng", "attract_movie", 0x72CF9F38, 1, 0},
    {"WS_LS_EALogo.fng", "ealogo", 0x58BCF5B6, 0, 0},
    {"WS_LS_EA_hidef.fng", "eahd_bumper", 0x58BCF5B6, 0, 0},
    {"WS_LS_PSA.fng", "psa", 0x58BCF5B6, 1, 1},
    {"WS_LS_IntroFMV.fng", "intro_movie", 0x58BCF5B6, 1, 0},
    {"WS_MW_LS_AttractFMV.fng", "attract_movie", 0x72CF9F38, 1, 0},
};

MovieScreen::MovieScreen(ScreenConstructorData *sd) : MenuScreen(sd), bByPassable(0), mSubtitler() {
    new ESndGameState(1, true);
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
    if (msg != 0xb5af2461) {
        if (msg > 0xb5af2461) {
            if (msg == 0xc3960eb9) {
                new ESndGameState(1, false);
                SetSoundControlState(false, SNDSTATE_FMV, "MovieScreen");
                if (!IsDebugPlayMovie) {
                    BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
                    movie_is_finished = true;
                } else {
                    cFEng::Get()->QueuePackagePop(1);
                    IsDebugPlayMovie = 0;
                    movie_is_finished = true;
                }
            }
            goto end;
        }
        if (msg != 0x406415e3)
            goto end;
    }
    if (bByPassable != 0) {
        new ESndGameState(1, false);
        SetSoundControlState(false, SNDSTATE_FMV, "MovieScreen");
        if (!IsDebugPlayMovie) {
            BootFlowManager::Get()->ChangeToNextBootFlowScreen(0xff);
            movie_is_finished = true;
        } else {
            movie_is_finished = true;
            cFEng::Get()->QueuePackagePop(1);
            IsDebugPlayMovie = 0;
        }
    }
end:
    if (movie_is_finished) {
        mSubtitler.Update(0xc3960eb9);
    }
}
