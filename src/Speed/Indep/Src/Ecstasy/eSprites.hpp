#ifndef ECSTASY_ESPRITES_H
#define ECSTASY_ESPRITES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bMath.hpp"

class eView;
struct TextureInfo;
struct eSprite;

// total size: 0x1C
struct eSpriteManager {
    bool bBatching;            // offset 0x0
    unsigned int bufferLength; // offset 0x4
    unsigned int packetLength; // offset 0x8
    unsigned int *pBuffer;     // offset 0xC
    unsigned int *baseBuffer;  // offset 0x10
    unsigned int *endBuffer;   // offset 0x14
    unsigned int *lastInline;  // offset 0x18

    eSpriteManager() { bBatching = false; bufferLength = 0; packetLength = 0; pBuffer = 0; baseBuffer = 0; endBuffer = 0; lastInline = 0; }
    ~eSpriteManager() {}

    bool StartBatch(eView *view, TextureInfo *texture_info, bMatrix4 *local_world, int numElements);
    void AddSprite(eSprite *sprite);
    void EndBatch();
    void RenderSprite(eSprite *sprite, TextureInfo *texture_info, bMatrix4 *local_world);
};

#endif
