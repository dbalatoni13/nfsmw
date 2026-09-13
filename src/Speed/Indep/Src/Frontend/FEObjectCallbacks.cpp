#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

struct MovieNameMap {
    const char *movieName;
    int movieId;
};

static MovieNameMap sMovieNameMap[42] = {{"ealogo", 0},    {"THX_logo", 1},  {"ealogo_en", 2}, {"ealogo_fr", 3}, {"ealogo_gr", 4}, {"ealogo_sp", 5},
                                         {"07_god", 6},    {"08_edd", 7},    {"09_gbe", 8},    {"11_pnk", 9},    {"12_bud", 10},   {"13_kit", 11},
                                         {"14_mut", 12},   {"15_dik", 13},   {"16_res", 14},   {"17_met", 15},   {"19_fin", 16},   {"preopen", 17},
                                         {"psa", 18},      {"e3_title", 19}, {"drift", 20},    {"eng_drag", 21}, {"fre_drag", 22}, {"ger_drag", 23},
                                         {"spa_drag", 24}, {"dut_drag", 25}, {"ita_drag", 26}, {"swe_drag", 27}, {"ssx", 28},      {"intssx", 29},
                                         {"nfl", 30},      {"intnfl", 31},   {"nascar", 32},   {"thx_logo", 33}, {"logo_eng", 34}, {"logo_fre", 35},
                                         {"logo_ger", 36}, {"logo_ita", 37}, {"logo_dut", 38}, {"logo_spa", 39}, {"logo_swe", 40}, {"nfsgb", 41}};

static int GetMovieNameEnum(const char *movieName) {
    for (int i = 0; i < NUM_ENTRIES(sMovieNameMap); i++) {
        if (FEngStrICmp(movieName, sMovieNameMap[i].movieName) == 0) {
            return sMovieNameMap[i].movieId;
        }
    }
    return -1;
}

static void CalculateMovieFilename(char *buffer, int bufsize, const char *basename, eLanguages cur_language) {
    const char *extension;
    const char *prefix = "";
    char language[64];
    const char *pal_or_ntsc;

    if (!BuildRegion::IsPal()) {
        pal_or_ntsc = "_ntsc";
    } else {
        pal_or_ntsc = "_pal";
    }

    bSPrintf(language, "_%s", GetLanguageName(cur_language));
    extension = ".vp6";
    FEngSNPrintf(buffer, bufsize, "%sMOVIES\\%s%s%s%s", prefix, basename, language, pal_or_ntsc, extension);
}

bool FEngMovieStarter::Callback(FEObject *obj) {
    if (obj->Type == FE_Movie) {
        if (SkipMovies) {
            cFEng::Get()->QueueGameMessagePkg(0xc3960eb9, pPackage);
        }

        const char *movie_name = reinterpret_cast<const char *>(obj->Handle);
        char buffer[64];
        int movieID = GetMovieNameEnum(movie_name);
        CalculateMovieFilename(buffer, 0x40, movie_name, GetCurrentLanguage());

        if (GetCurrentLanguage() != 0 && !bFileExists(buffer)) {
            CalculateMovieFilename(buffer, 0x40, movie_name, eLANGUAGE_ENGLISH);
        }

        if (bFileExists(buffer)) {
            MoviePlayer_StartUp();
            {
                MoviePlayer::Settings settings;
                bStrNCpy(settings.filename, buffer, 0x100);
                settings.loop = true;
                settings.movieId = movieID;
                settings.type = 0;
                gMoviePlayer->Init(settings);
            }
            MoviePlayer_Play();
        } else {
            cFEng::Get()->QueueGameMessagePkg(0xc3960eb9, pPackage);
        }

        return false;
    }
    return true;
}

bool FEngMovieStopper::Callback(FEObject *obj) {
    if (obj->Type == FE_Movie) {
        if (gMoviePlayer != nullptr) {
            gMoviePlayer->Stop();
        }
        MoviePlayer_ShutDown();
        return false;
    }
    return true;
}

bool FEngHidePCObjects::Callback(FEObject *obj) {
    if (obj->Flags & 0x8) {
        FEngSetInvisible(obj);
        if (obj->Flags & FF_IsButton) {
            obj->Flags &= ~FF_IsButton;
        }
        obj->Flags |= FF_DirtyCode;
    }
    return true;
}

bool FEngTransferFlagsToChildren::Callback(FEObject *obj) {
    if ((obj->Flags & FlagToTransfer) && obj->Type == FE_Group) {
        FEGroup *group = static_cast<FEGroup *>(obj);
        FEObject *child = group->GetFirstChild();
        int num = group->GetNumChildren();
        for (int i = 0; i < num; i++) {
            child->Flags |= FlagToTransfer;
            Callback(child);
            child = child->GetNext();
        }
    }
    return true;
}

bool RenderObjectDisconnect::Callback(FEObject *pObj) {
    pFEngRenderer->RemoveCachedRender(pObj, PkgRenderInfo);
    return true;
}

bool ObjectDirtySetter::Callback(FEObject *obj) {
    obj->Flags |= FF_DirtyCode;
    cFEngRender::mInstance->RemoveCachedRender(obj, pRenderInfo);
    return true;
}

bool ObjectVisibilitySetter::Callback(FEObject *obj) {
    FEngSetVisibility(obj, Visible);
    return true;
}
