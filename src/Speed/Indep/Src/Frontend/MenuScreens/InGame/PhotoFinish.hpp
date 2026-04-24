#ifndef FRONTEND_MENUSCREENS_INGAME_PHOTOFINISH_H
#define FRONTEND_MENUSCREENS_INGAME_PHOTOFINISH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

struct FEImage;

struct load_info {
    FEImage *LoadIntoImage;
    unsigned int LoadingTexture;
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
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;
    void Setup();
    static MenuScreen *Create(ScreenConstructorData *sd);
    static bool mRestartSelected;

  protected:
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
