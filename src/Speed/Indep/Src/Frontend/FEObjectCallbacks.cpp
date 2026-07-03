#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
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

static MovieNameMap sMovieNameMap[42];

static int GetMovieNameEnum(const char *movieName) {
    for (int i = 0; i < 42; i++) {
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
        eLanguages lang = static_cast<eLanguages>(GetCurrentLanguage());
        CalculateMovieFilename(buffer, 0x40, movie_name, lang);

        if (GetCurrentLanguage() != 0 && !bFileExists(buffer)) {
            CalculateMovieFilename(buffer, 0x40, movie_name, eLANGUAGE_ENGLISH);
        }

        if (bFileExists(buffer)) {
            MoviePlayer_StartUp();
            {
                MoviePlayer::Settings settings;
                settings.bufferSize = 0x40000;
                settings.activeController = 0;
                settings.preload = false;
                settings.volume = 0;
                settings.filename[0] = '\0';
                settings.sound = IsSoundEnabled != 0;
                settings.loop = false;
                settings.pal = false;
                settings.type = 0;
                settings.movieId = 0;
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
    if (Visible) {
        FEngSetVisible(obj);
    } else {
        FEngSetInvisible(obj);
    }
    return true;
}
