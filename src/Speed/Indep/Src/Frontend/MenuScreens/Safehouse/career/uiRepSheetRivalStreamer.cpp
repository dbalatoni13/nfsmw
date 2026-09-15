#include "uiRepSheetRivalStreamer.hpp"

#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

uiRepSheetRivalStreamer::uiRepSheetRivalStreamer(const char *pkg_name, bool in_game) {
    this->pkg_name = pkg_name;
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
        int mem_needed = 0x30000;
        TheTrackStreamer.MakeSpaceInPool(mem_needed, MakeSpaceInPoolCallbackBridge, reinterpret_cast<int>(this));
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

void uiRepSheetRivalStreamer::Init(uint32 the_bin, FEImage *the_rival, FEImage *the_tag, FEImage *the_bg) {
    DesiredBin = the_bin;
    Rival = the_rival;
    Tag = the_tag;
    BG = the_bg;
    FEngSetInvisible(Rival);
    FEngSetInvisible(Tag);
    FEngSetInvisible(BG);
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

int uiRepSheetRivalStreamer::CalcTexturesToLoad(uint32 *temp, int bin) {
    int index = 0;
    if (Rival != nullptr) {
        if (bInGame) {
            temp[index++] = FEngHashString("RIVAL_%02d_IG", bin);
        } else {
            temp[index++] = FEngHashString("RIVAL_%02d", bin);
        }
    }
    if (Tag != nullptr) {
        if (bInGame) {
            temp[index++] = FEngHashString("RIVAL_%02d_IG_GRAF", bin);
        } else {
            temp[index++] = FEngHashString("RIVAL_%02d_GRAF", bin);
        }
    }
    if (BG != nullptr) {
        if (bInGame) {
            temp[index++] = FEngHashString("RIVAL_%02d_IG_BG", bin);
        } else {
            temp[index++] = FEngHashString("RIVAL_%02d_BG", bin);
        }
    }
    return index;
}

void uiRepSheetRivalStreamer::TexturesLoadedCallback() {
    LoadingInProgress = false;
    if (LoadedBin != DesiredBin) {
        LoadTextures();
    } else {
        int index = 0;
        if (Rival != nullptr) {
            const u32 FEObj_RIVALLOADED = 0xC0942E85;
            cFEng::Get()->QueuePackageMessage(FEObj_RIVALLOADED, pkg_name, nullptr);
            TextureInfo *ti = GetTextureInfo(LoadedTextures[0], 0, 0);
            FEngSetTextureHash(Rival, LoadedTextures[0]);
            FEngSetVisible(Rival);
            index = 1;
        }
        if (Tag != nullptr) {
            const u32 FEObj_GRAFLOADED = 0x8C9D4547;
            cFEng::Get()->QueuePackageMessage(FEObj_GRAFLOADED, pkg_name, nullptr);
            FEngSetTextureHash(Tag, LoadedTextures[index]);
            index++;
            FEngSetVisible(Tag);
        }
        if (BG != nullptr) {
            const u32 FEObj_BGLOADED = 0xD22B95D0;
            cFEng::Get()->QueuePackageMessage(FEObj_BGLOADED, pkg_name, nullptr);
            FEngSetTextureHash(BG, LoadedTextures[index]);
            FEngSetVisible(BG);
        }
    }
}
