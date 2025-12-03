#ifndef ECSTASY_ECSTASYE_H
#define ECSTASY_ECSTASYE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"

struct eRenderTarget {
    // total size: 0x40
    TARGET_ID ID;             // offset 0x0, size 0x4
    const char *Name;         // offset 0x4, size 0x4
    int Active;               // offset 0x8, size 0x4
    int ScissorX;             // offset 0xC, size 0x4
    int ScissorY;             // offset 0x10, size 0x4
    int ScissorW;             // offset 0x14, size 0x4
    int ScissorH;             // offset 0x18, size 0x4
    int FrameAddress;         // offset 0x1C, size 0x4
    int FrameWidth;           // offset 0x20, size 0x4
    int FrameHeight;          // offset 0x24, size 0x4
    FILTER_ID CopyFilterID;   // offset 0x28, size 0x4
    GXColor BackgroundColour; // offset 0x2C, size 0x4
    int ClearBackground;      // offset 0x30, size 0x4
    bMatrix4 *WorldClip;      // offset 0x34, size 0x4
    bMatrix4 *WorldView;      // offset 0x38, size 0x4
    bMatrix4 *ViewScreen;     // offset 0x3C, size 0x4

    FILTER_ID GetCopyFilter() {
        return CopyFilterID;
    }

    TextureInfo *GetTextureInfo();
};

void __InitGXlite(void);

#endif
