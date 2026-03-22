#ifndef FRONTEND_MENUSCREENS_INGAME_FEPKG_CHYRON_H
#define FRONTEND_MENUSCREENS_INGAME_FEPKG_CHYRON_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct Chyron : public MenuScreen {
    Chyron(ScreenConstructorData *sd);
    ~Chyron() override;
    void NotificationMessage(unsigned long msg, FEObject *pobject, unsigned long param1, unsigned long param2) override;
    void Start();

    void *operator new(size_t, void *ptr) { return ptr; }

    static char *mTitle;
    static char *mArtist;
    static char *mAlbum;

    Timer mDelayTimer; // offset 0x2C
};

void SummonChyron(char *title, char *artist, char *album);
void DismissChyron();

#endif
