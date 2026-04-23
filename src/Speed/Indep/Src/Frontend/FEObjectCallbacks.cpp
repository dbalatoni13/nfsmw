#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"

namespace BuildRegion {
bool IsPal();
}

extern int FEngStrICmp(const char*, const char*);
extern int FEngSNPrintf(char*, int, const char*, ...);
extern int bSPrintf(char*, const char*, ...);
extern const char* GetLanguageName(eLanguages language);
extern int SkipMovies;
extern bool IsSoundEnabled;
extern int GetCurrentLanguage();
extern int bFileExists(const char* f);
extern char* bStrNCpy(char* to, const char* from, int m);
extern void FEngSetVisible(FEObject* obj);
extern void FEngSetInvisible(FEObject* obj);

struct MovieNameMap {
    const char* movieName;
    int movieId;
};

static MovieNameMap sMovieNameMap[42];

static int GetMovieNameEnum(const char* movieName) {
    for (int i = 0; i < 42; i++) {
        if (FEngStrICmp(movieName, sMovieNameMap[i].movieName) == 0) {
            return sMovieNameMap[i].movieId;
        }
    }
    return -1;
}

static void CalculateMovieFilename(char* buffer, int bufsize, const char* basename,
                                   eLanguages cur_language) {
    const char* extension;
    const char* prefix = "";
    char language[64];
    const char* pal_or_ntsc;

    if (!BuildRegion::IsPal()) {
        pal_or_ntsc = "_ntsc";
    } else {
        pal_or_ntsc = "_pal";
    }

    bSPrintf(language, "_%s", GetLanguageName(cur_language));
    extension = ".vp6";
    FEngSNPrintf(buffer, bufsize, "%sMOVIES\\%s%s%s%s", prefix, basename, language, pal_or_ntsc,
                 extension);
}


bool FEngMovieStopper::Callback(FEObject* obj) {
    if (obj->Type == 7) {
        if (gMoviePlayer != nullptr) {
            gMoviePlayer->Stop();
        }
        MoviePlayer_ShutDown();
        return false;
    }
    return true;
}

bool FEngHidePCObjects::Callback(FEObject* obj) {
    if (obj->Flags & 0x8) {
        FEngSetInvisible(obj);
        if (obj->Flags & 0x10000000) {
            obj->Flags &= ~0x10000000;
        }
        obj->Flags |= 0x00400000;
    }
    return true;
}

bool RenderObjectDisconnect::Callback(FEObject* pObj) {
    pFEngRenderer->RemoveCachedRender(pObj, PkgRenderInfo);
    return true;
}

bool ObjectDirtySetter::Callback(FEObject* obj) {
    obj->Flags |= 0x00400000;
    cFEngRender::mInstance->RemoveCachedRender(obj, pRenderInfo);
    return true;
}

bool ObjectVisibilitySetter::Callback(FEObject* obj) {
    if (Visible) {
        FEngSetVisible(obj);
    } else {
        FEngSetInvisible(obj);
    }
    return true;
}

bool FEngMovieStarter::Callback(FEObject* obj) {
    if (obj->Type == FE_Movie) {
        if (SkipMovies) {
            cFEng::Get()->QueueGameMessagePkg(0xc3960eb9, pPackage);
        }

        const char* movie_name = reinterpret_cast<const char*>(obj->Handle);
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

bool FEngTransferFlagsToChildren::Callback(FEObject* obj) {
    if ((obj->Flags & FlagToTransfer) && obj->Type == FE_Group) {
        FEGroup* group = static_cast<FEGroup*>(obj);
        FEObject* child = group->GetFirstChild();
        int num = group->GetNumChildren();
        for (int i = 0; i < num; i++) {
            child->Flags |= FlagToTransfer;
            Callback(child);
            child = child->GetNext();
        }
    }
    return true;
}

static char* GetBaseName(char* dest, const char* filename) {
    long x = 0;
    long first = 0;
    long last;

    while (filename[x] != '\0') {
        int c = filename[x];
        x++;
        if (c == '\\' || c == '/') {
            first = x;
        }
    }

    last = x;
    if (x != 0) {
        if (filename[x] != '.') {
            while (--x != 0) {
                if (filename[x] == '.') {
                    last = x;
                    break;
                }
            }
        }
    }

    long y = 0;
    for (x = first; x < last; x++) {
        dest[y] = filename[x];
        y++;
    }
    dest[y] = '\0';
    return dest;
}
