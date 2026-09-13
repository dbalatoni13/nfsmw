#ifndef LOADINGSCREEN_HPP
#define LOADINGSCREEN_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// total size: 0x2C
// Decl: 15
class LoadingScreen : public MenuScreen {
  public:
    static void *operator new(size_t size) {
        return mLoadingScreenPtr;
    }

    static void *operator new(size_t size, char *file, int line) {
        return mLoadingScreenPtr;
    }

    static void *operator new(size_t size, char *msg) {
        return mLoadingScreenPtr;
    }

    static void operator delete(void *ptr) {} // Decl: 21

    static void operator delete(void *ptr, char *msg) {} // Decl: 22

    enum LoadingScreenTypes {
        LS_LOADING_FE = 0,
        LS_LOADING_GAME_FROM_FE = 1,
    };

    LoadingScreen(ScreenConstructorData *sd);
    ~LoadingScreen() override;

    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override {}

    static void InitLoadingScreen();
    static void CloseLoadingScreen();

  private:
    static void *mLoadingScreenPtr; // size: 0x4, address: 0x8041C194, Decl: 20
};

#endif
