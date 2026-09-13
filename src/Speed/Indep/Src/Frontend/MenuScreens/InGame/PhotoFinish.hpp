#ifndef PHOTOFINISH_HPP
#define PHOTOFINISH_HPP

#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"

struct load_info {
    FEImage *LoadIntoImage;
    uint32 LoadingTexture;
    bool IsLoaded;
};

class SillyTextureStreamerManager {
  public:
    SillyTextureStreamerManager(const char *stream_pack);
    ~SillyTextureStreamerManager();
    void Load(uint32 hash, FEImage *image);
    void Unload(uint32 hash);
    void UnloadAll();
    bool IsLoaded(uint32 hash);
    bool IsBusyLoading();

  private:
    static void MakeSpaceInPoolCallbackBridge(int param) {
        reinterpret_cast<SillyTextureStreamerManager *>(param)->MakeSpaceInPoolCallback();
    }
    void MakeSpaceInPoolCallback();
    static void LoadCallbackBridge(uint32 param) {
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
    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;
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
