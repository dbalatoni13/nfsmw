#ifndef FRONTEND_MENUSCREENS_INGAME_PHOTOFINISH_H
#define FRONTEND_MENUSCREENS_INGAME_PHOTOFINISH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"

struct load_info {
    FEImage *LoadIntoImage;
    uint32 LoadingTexture;
    int IsLoaded;
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
    static void MakeSpaceInPoolCallbackBridge(int param);
    void MakeSpaceInPoolCallback();
    static void LoadCallbackBridge(unsigned int param);
    void LoadCallback();

    char BundleFileName[256];
    load_info LoadInfos[4];
    int mCurrentlyLoading;
    int mMakeSpaceInPoolComplete;
    int mCurrentLoadingIndex;
};

class PhotoFinishScreen : public MenuScreen {
  public:
    PhotoFinishScreen(ScreenConstructorData *sd);
    ~PhotoFinishScreen() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    static MenuScreen *Create(ScreenConstructorData *sd);

    static void SetTrapSpeed(float mps) {
        mSpeedtrapSpeed = mps;
    }

    static void SetTrapBounty(float bounty) {
        mSpeedtrapBounty = bounty;
    }

    static void SetActive() {
        mActive = true;
    }

    static bool mRestartSelected;

  private:
    void Setup();

    static float mSpeedtrapSpeed;
    static float mSpeedtrapBounty;
    static bool mActive;

    Timer mSlowdownTimer;
    Timer mIceCamTimer;
    FERESULTTYPE fResultType;
    int mPhotoHash;
    SillyTextureStreamerManager StreamTex;
};

#endif
