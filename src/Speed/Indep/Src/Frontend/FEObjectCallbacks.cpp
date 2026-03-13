#include "FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Misc/Config.h"

extern int SkipMovies;

enum eLanguages;

int GetMovieNameEnum(const char *name);
eLanguages GetCurrentLanguage();
void CalculateMovieFilename(char *buffer, int size, const char *name, eLanguages lang);
bool bFileExists(const char *filename);
void bStrNCpy(char *dst, const char *src, int size);

void FEngSetInvisible(FEObject *obj);
void FEngSetVisibility(FEObject *obj, bool visible);

bool FEngMovieStarter::Callback(FEObject *obj) {
    if (obj->Type == FE_Movie) {
        if (SkipMovies) {
            cFEng::Get()->QueueGameMessagePkg(0xC3960EB9, pPackage);
        }
        const char *movie_name = reinterpret_cast<const char*>(obj->Handle);
        char buffer[64];
        int movieID = GetMovieNameEnum(movie_name);
        eLanguages lang = GetCurrentLanguage();
        CalculateMovieFilename(buffer, 64, movie_name, lang);
        if (GetCurrentLanguage() != 0 && !bFileExists(buffer)) {
            CalculateMovieFilename(buffer, 64, movie_name, static_cast<eLanguages>(0));
        }
        if (!bFileExists(buffer)) {
            cFEng::Get()->QueueGameMessagePkg(0xC3960EB9, pPackage);
        } else {
            MoviePlayer_StartUp();
            MoviePlayer::Settings settings;
            settings.volume = 0;
            settings.bufferSize = 0x40000;
            settings.activeController = 0;
            settings.type = 0;
            settings.movieId = 0;
            settings.preload = false;
            settings.sound = IsSoundEnabled != false;
            settings.loop = false;
            settings.pal = false;
            settings.filename[0] = '\0';
            bStrNCpy(settings.filename, buffer, 256);
            settings.loop = true;
            settings.type = 0;
            settings.movieId = movieID;
            gMoviePlayer->Init(settings);
            MoviePlayer_Play();
        }
        return false;
    }
    return true;
}

bool FEngMovieStopper::Callback(FEObject *obj) {
    if (obj->Type == FE_Movie) {
        if (gMoviePlayer) {
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
        if (obj->Flags & 0x10000000) {
            obj->Flags &= ~0x10000000;
        }
        obj->Flags |= 0x400000;
    }
    return true;
}

bool FEngTransferFlagsToChildren::Callback(FEObject *obj) {
    if ((obj->Flags & FlagToTransfer) && obj->Type == FE_Group) {
        FEGroup *group = static_cast<FEGroup*>(obj);
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
    pObj->Cached = nullptr;
    return true;
}

bool ObjectDirtySetter::Callback(FEObject *obj) {
    obj->Cached = nullptr;
    return true;
}

