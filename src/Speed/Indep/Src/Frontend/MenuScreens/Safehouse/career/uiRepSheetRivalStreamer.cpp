#include "uiRepSheetRivalStreamer.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"

struct FEObject;

void FEngSetTextureHash(FEImage *image, unsigned int hash);
unsigned int FEngHashString(const char *format, ...);
void eLoadStreamingTexture(unsigned int *textures, int count, void (*callback)(void *), void *param, int pool);
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
        eLoadStreamingTexturePack("Global\\Rivals.bin", TexturePackLoadedCallbackBridge, this, 0);
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
    eWaitForStreamingTexturePackLoading("Global\\Rivals.bin");
    UnloadTextures();
    eUnloadStreamingTexturePack("Global\\Rivals.bin");
}

void uiRepSheetRivalStreamer::MakeSpaceInPoolCallback() {
    bMakeSpaceInPoolComplete = true;
    eLoadStreamingTexturePack("Global\\Rivals.bin", TexturePackLoadedCallbackBridge, this, 0);
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
            temp[count++] = FEngHashString("RIVAL_%02d_IG", bin);
        } else {
            temp[count++] = FEngHashString("RIVAL_%02d", bin);
        }
    }
    if (Tag != nullptr) {
        if (bInGame) {
            temp[count++] = FEngHashString("RIVAL_%02d_IG_GRAF", bin);
        } else {
            temp[count++] = FEngHashString("RIVAL_%02d_GRAF", bin);
        }
    }
    if (BG != nullptr) {
        if (bInGame) {
            temp[count++] = FEngHashString("RIVAL_%02d_IG_BG", bin);
        } else {
            temp[count++] = FEngHashString("RIVAL_%02d_BG", bin);
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
        TextureInfo *ti = GetTextureInfo(LoadedTextures[0], 0, 0);
        FEngSetTextureHash(Rival, LoadedTextures[idx++]);
        FEngSetVisible(reinterpret_cast<FEObject *>(Rival));
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
