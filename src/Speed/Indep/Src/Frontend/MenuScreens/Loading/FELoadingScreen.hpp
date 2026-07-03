#ifndef LOADINGSCREEN_HPP
#define LOADINGSCREEN_HPP

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

// File: speed/indep/src/frontend/menuscreens/loading/FELoadingScreen.hpp
// total size: 0x2C
// Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingScreen.hpp:15
class LoadingScreen : public MenuScreen {
  public:
    static void *operator new(size_t size) {}

    static void *operator new(size_t size, char *file, int line) {}

    static void *operator new(size_t size, char *msg) {}

    static void operator delete(void *ptr) {} // Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingScreen.hpp:21

    static void operator delete(void *ptr, char *msg) {} // Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingScreen.hpp:22

    typedef enum { LS_LOADING_FE = 0, LS_LOADING_GAME_FROM_FE = 1 } LoadingScreenTypes;

    LoadingScreen(ScreenConstructorData *sd);
    ~LoadingScreen() override;

    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override {}

    static void InitLoadingScreen();
    static void CloseLoadingScreen();

  private:
    static void *mLoadingScreenPtr; // size: 0x4, address: 0x8041C194, Decl: speed/indep/src/frontend/menuscreens/loading/FELoadingScreen.cpp:20
};

#endif
