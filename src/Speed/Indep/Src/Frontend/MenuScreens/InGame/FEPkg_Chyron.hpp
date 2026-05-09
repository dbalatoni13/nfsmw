#ifndef FRONTEND_MENUSCREENS_INGAME_FEPKG_CHYRON_H
#define FRONTEND_MENUSCREENS_INGAME_FEPKG_CHYRON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

static MenuScreen *ChyronScreenPtr;

class Chyron : public MenuScreen {
  public:
    void *operator new(size_t size) {
        return ChyronScreenPtr;
    }
    void *operator new(size_t size, char *file, int line) {}
    void *operator new(size_t size, char *msg) {}
    void operator delete(void *ptr) {}

    Chyron(ScreenConstructorData *sd);
    ~Chyron() override {};
    void NotificationMessage(u32 msg, FEObject *pobject, u32 param1, u32 param2) override;
    void Start();

    static char *mTitle;
    static char *mArtist;
    static char *mAlbum;

    Timer mDelayTimer; // offset 0x2C
};

void SummonChyron(char *title, char *artist, char *album);
void DismissChyron();
void InitChyron();

#endif
