#ifndef FRONTEND_MENUSCREENS_INGAME_PHOTOFINISH_H
#define FRONTEND_MENUSCREENS_INGAME_PHOTOFINISH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/FEng/feimage.h"

struct load_info {
    FEImage *LoadIntoImage;
    uint32 LoadingTexture;
    bool IsLoaded;
};

class SillyTextureStreamerManager {
  public:
    SillyTextureStreamerManager(const char *stream_pack);
    ~SillyTextureStreamerManager();
    void Load(unsigned int hash, FEImage *image);
    void Unload(unsigned int hash);
    void UnloadAll();
    bool IsLoaded(unsigned int hash);
    bool IsBusyLoading();

  private:
    static void MakeSpaceInPoolCallbackBridge(int param) {
        reinterpret_cast<SillyTextureStreamerManager *>(param)->MakeSpaceInPoolCallback();
    }
    void MakeSpaceInPoolCallback();
    static void LoadCallbackBridge(unsigned int param) {
        reinterpret_cast<SillyTextureStreamerManager *>(param)->LoadCallback();
    }
    void LoadCallback();

    char BundleFileName[256];
    load_info LoadInfos[4];
    bool mCurrentlyLoading;
    bool mMakeSpaceInPoolComplete;
    int mCurrentLoadingIndex;
};

class PhotoFinishScreen : public MenuScreen {
  public:
    PhotoFinishScreen(ScreenConstructorData *sd);
    ~PhotoFinishScreen() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    static MenuScreen *Create(ScreenConstructorData *sd);
    static bool mRestartSelected;

  private:
    void Setup();

    static float mSpeedtrapSpeed;
    static float mSpeedtrapBounty;
    static bool mActive;

    Timer mIceCamTimer;
    Timer mSlowdownTimer;
    FERESULTTYPE fResultType;
    int mPhotoHash;
    SillyTextureStreamerManager StreamTex;
};

#endif
