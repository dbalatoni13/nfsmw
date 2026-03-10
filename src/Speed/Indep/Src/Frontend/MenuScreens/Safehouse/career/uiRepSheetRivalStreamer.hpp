#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRIVALSTREAMER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UIREPSHEETRIVALSTREAMER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

struct FEImage;

// total size: 0x3C
struct uiRepSheetRivalStreamer {
    int MemPoolNum;                  // offset 0x0, size 0x4
    bool bInGame;                    // offset 0x4, size 0x1
    bool bMakeSpaceInPoolComplete;   // offset 0x8, size 0x1
    const char* pkg_name;            // offset 0xC, size 0x4
    int DesiredBin;                  // offset 0x10, size 0x4
    int LoadedBin;                   // offset 0x14, size 0x4
    bool LoadingInProgress;          // offset 0x18, size 0x1
    int NumLoadedTextures;           // offset 0x1C, size 0x4
    unsigned int LoadedTextures[3];  // offset 0x20, size 0xC
    FEImage* Rival;                  // offset 0x2C, size 0x4
    FEImage* Tag;                    // offset 0x30, size 0x4
    FEImage* BG;                     // offset 0x34, size 0x4
    // vtable at 0x38

    static void MakeSpaceInPoolCallbackBridge(int param);
    static void TexturePackLoadedCallbackBridge(void* param);
    static void TexturesLoadedCallbackBridge(void* param);

    uiRepSheetRivalStreamer(const char* pkg_name, bool in_game);
    virtual ~uiRepSheetRivalStreamer();

    void MakeSpaceInPoolCallback();
    void TexturePackLoadedCallback();
    void Init(unsigned int the_bin, FEImage* the_rival, FEImage* the_tag, FEImage* the_bg);
    void LoadTextures();
    void UnloadTextures();
    int CalcTexturesToLoad(unsigned int* temp, int bin);
    void TexturesLoadedCallback();
};

#endif
