#include "uiRepSheetRivalStreamer.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

struct FEObject;

FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);
void FEngSetTextureHash(FEImage *image, unsigned int hash);
unsigned int FEngHashString(const char *format, ...);
void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(void *), void *param, int pool);
void eUnloadStreamingTexture(unsigned int *textures, int count);
void eUnloadStreamingTexturePack(const char *name);
void eWaitForStreamingTexturePackLoading(const char *name);
struct TextureInfo;
TextureInfo *GetTextureInfo(unsigned int hash, int, int);

uiRepSheetRivalStreamer::uiRepSheetRivalStreamer(const char *name, bool in_game) {
    pkg_name = name;
    MemPoolNum = 0;
    bInGame = in_game;
    DesiredBin = -1;
    LoadedBin = -1;
    LoadingInProgress = true;
    bMakeSpaceInPoolComplete = false;
    NumLoadedTextures = 0;
    Rival = nullptr;
    Tag = nullptr;
    BG = nullptr;
    if (bInGame) {
        MemPoolNum = 7;
        TheTrackStreamer.DisableZoneSwitching();
        TheTrackStreamer.MakeSpaceInPool(0x30000, MakeSpaceInPoolCallbackBridge, reinterpret_cast<int>(this));
    } else {
        eLoadStreamingTexturePack("BL_RIVAL_PACK", TexturePackLoadedCallbackBridge, this, 0);
    }
}

uiRepSheetRivalStreamer::~uiRepSheetRivalStreamer() {
    if (bInGame) {
        if (!bMakeSpaceInPoolComplete) {
            TheTrackStreamer.WaitForCurrentLoadingToComplete();
        }
        TheTrackStreamer.EnableZoneSwitching();
        TheTrackStreamer.RefreshLoading();
    }
    eWaitForStreamingTexturePackLoading("BL_RIVAL_PACK");
    UnloadTextures();
    eUnloadStreamingTexturePack("BL_RIVAL_PACK");
}

void uiRepSheetRivalStreamer::MakeSpaceInPoolCallback() {
    bMakeSpaceInPoolComplete = true;
    eLoadStreamingTexturePack("BL_RIVAL_PACK", TexturePackLoadedCallbackBridge, this, 0);
}

void uiRepSheetRivalStreamer::TexturePackLoadedCallback() {
    LoadingInProgress = false;
    LoadTextures();
}

void uiRepSheetRivalStreamer::Init(unsigned int the_bin, FEImage *the_rival, FEImage *the_tag, FEImage *the_bg) {
    DesiredBin = the_bin;
    Rival = the_rival;
    Tag = the_tag;
    BG = the_bg;
    FEngSetInvisible(reinterpret_cast<FEObject *>(Rival));
    FEngSetInvisible(reinterpret_cast<FEObject *>(Tag));
    FEngSetInvisible(reinterpret_cast<FEObject *>(BG));
    if (!LoadingInProgress) {
        LoadTextures();
    }
}

void uiRepSheetRivalStreamer::LoadTextures() {
    if (LoadedBin != DesiredBin) {
        if (NumLoadedTextures != 0) {
            UnloadTextures();
        }
        LoadingInProgress = true;
        LoadedBin = DesiredBin;
        NumLoadedTextures = CalcTexturesToLoad(LoadedTextures, DesiredBin);
        eLoadStreamingTexture(LoadedTextures, NumLoadedTextures, TexturesLoadedCallbackBridge, this, MemPoolNum);
    }
}

void uiRepSheetRivalStreamer::UnloadTextures() {
    eUnloadStreamingTexture(LoadedTextures, NumLoadedTextures);
    NumLoadedTextures = 0;
    LoadedBin = -1;
}

int uiRepSheetRivalStreamer::CalcTexturesToLoad(unsigned int *temp, int bin) {
    int count = 0;
    if (Rival != nullptr) {
        if (bInGame) {
            temp[count++] = FEngHashString("BL_INGAME_RIVAL_%d", bin);
        } else {
            temp[count++] = FEngHashString("BL_RIVAL_%d", bin);
        }
    }
    if (Tag != nullptr) {
        if (bInGame) {
            temp[count++] = FEngHashString("BL_INGAME_TAG_%d", bin);
        } else {
            temp[count++] = FEngHashString("BL_TAG_%d", bin);
        }
    }
    if (BG != nullptr) {
        if (bInGame) {
            temp[count++] = FEngHashString("BL_INGAME_BG_%d", bin);
        } else {
            temp[count++] = FEngHashString("BL_BG_%d", bin);
        }
    }
    return count;
}

void uiRepSheetRivalStreamer::TexturesLoadedCallback() {
    int idx;
    LoadingInProgress = false;
    if (LoadedBin != DesiredBin) {
        LoadTextures();
        return;
    }
    idx = 0;
    if (Rival != nullptr) {
        cFEng::Get()->QueuePackageMessage(0xC0942E85, pkg_name, nullptr);
        GetTextureInfo(LoadedTextures[0], false, false);
        FEngSetTextureHash(Rival, LoadedTextures[0]);
        FEngSetVisible(reinterpret_cast<FEObject *>(Rival));
        idx = 1;
    }
    if (Tag != nullptr) {
        cFEng::Get()->QueuePackageMessage(0x8C9D4547, pkg_name, nullptr);
        FEngSetTextureHash(Tag, LoadedTextures[idx]);
        idx++;
        FEngSetVisible(reinterpret_cast<FEObject *>(Tag));
    }
    if (BG != nullptr) {
        cFEng::Get()->QueuePackageMessage(0xD22B95D0, pkg_name, nullptr);
        FEngSetTextureHash(BG, LoadedTextures[idx]);
        FEngSetVisible(reinterpret_cast<FEObject *>(BG));
    }
}

void uiRepSheetRivalStreamer::MakeSpaceInPoolCallbackBridge(int param) {
    reinterpret_cast<uiRepSheetRivalStreamer *>(param)->MakeSpaceInPoolCallback();
}

void uiRepSheetRivalStreamer::TexturePackLoadedCallbackBridge(void *param) {
    static_cast<uiRepSheetRivalStreamer *>(param)->TexturePackLoadedCallback();
}

void uiRepSheetRivalStreamer::TexturesLoadedCallbackBridge(void *param) {
    static_cast<uiRepSheetRivalStreamer *>(param)->TexturesLoadedCallback();
}
