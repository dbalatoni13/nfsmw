#ifndef ECSTASY_XSPRITES_H
#define ECSTASY_XSPRITES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/GameCube/Src/xSparks.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

struct TextureInfo;

struct SpriteDef {
    TextureInfo *texture_info;
    unsigned int color;
    float width;
    bVector3 startPos;
    bVector3 EndPosPos;
};

struct XSpriteManager {
    void AddSpark(const NGParticle &particle, TextureInfo *CurrentTexture);
    void RenderAll(eView *view);

    unsigned int position;
    SpriteDef XSpriteBuffer[300];
};

extern XSpriteManager NGSpriteManager;

#endif
