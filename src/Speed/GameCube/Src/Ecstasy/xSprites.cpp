#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"

struct NGParticle {
    // total size: 0x30
    UMath::Vector3 initialPos; // offset 0x0, size 0xC
    uint32 color;              // offset 0xC, size 0x4
    UMath::Vector3 vel;        // offset 0x10, size 0xC
    float gravity;             // offset 0x1C, size 0x4
    uint16 life;               // offset 0x20, size 0x2
    uint8 length;              // offset 0x22, size 0x1
    uint8 width;               // offset 0x23, size 0x1
    unsigned char uv[1];       // offset 0x24, size 0x1
    unsigned char pad[3];      // offset 0x25, size 0x3
    float padAlign;            // offset 0x28, size 0x4
    float age;                 // offset 0x2C, size 0x4
};

struct SpriteDef {
    // total size: 0x2C
    TextureInfo *texture_info; // offset 0x0, size 0x4
    uint32 color;              // offset 0x4, size 0x4
    float width;               // offset 0x8, size 0x4
    bVector3 startPos;         // offset 0xC, size 0x10
    bVector3 EndPosPos;        // offset 0x1C, size 0x10
};

struct XSpriteManager {
    // total size: 0x3394
    uint32 position;             // offset 0x0, size 0x4
    SpriteDef XSpriteBuffer[300]; // offset 0x4, size 0x3390

    void AddSpark(const NGParticle &particle, TextureInfo *CurrentTexture);
    void RenderAll(eView *view);
};

void RenderViewPolyEx(eView *view, ePoly *poly, TextureInfo *texture_info, bMatrix4 *matrix, int flags, float z_bias)
    __asm__("Render__18eViewPlatInterfaceP5ePolyP11TextureInfoP8bMatrix4if");

void XSpriteManager::AddSpark(const NGParticle &particle, TextureInfo *CurrentTexture) {
    if (this->position < 300) {
        UMath::Vector3 startPos;
        UMath::Vector3 endPos;

        UMath::ScaleAdd(particle.vel, particle.age, particle.initialPos, startPos);
        startPos.z += particle.gravity * particle.age * particle.age;
        float endAge = static_cast<float>(static_cast<int>(particle.length)) * (1.0f / 2048.0f) + particle.age;

        UMath::ScaleAdd(particle.vel, endAge, particle.initialPos, endPos);

        SpriteDef *XSpriteBufferP = &this->XSpriteBuffer[this->position];
        endPos.z += particle.gravity * endAge * endAge;

        XSpriteBufferP->texture_info = CurrentTexture;
        XSpriteBufferP->color =
            particle.color >> 24 | particle.color >> 8 & 0xFF00 | (particle.color & 0xFF00) << 8 | particle.color << 24;
        XSpriteBufferP->startPos = startPos;
        XSpriteBufferP->EndPosPos = endPos;

        float width = static_cast<float>(static_cast<int>(particle.width)) * (1.0f / 2048.0f);
        XSpriteBufferP->width = width;
        this->position++;
    }
}

void XSpriteManager::RenderAll(eView *view) {
    ePoly pPoly;
    SpriteDef *XSpriteBufferP = this->XSpriteBuffer;

    {
        uint32 i;
        bMatrix4 *identity = eGetIdentityMatrix();

        for (i = 0; i < this->position; i++) {
            pPoly.Vertices[0] = XSpriteBufferP->startPos;
            pPoly.Vertices[1] = XSpriteBufferP->startPos;
            pPoly.Vertices[1].z += XSpriteBufferP->width;
            pPoly.Vertices[2] = XSpriteBufferP->EndPosPos;
            pPoly.Vertices[2].z += XSpriteBufferP->width;
            pPoly.Vertices[3] = XSpriteBufferP->EndPosPos;

            *reinterpret_cast<uint32 *>(pPoly.Colours[0]) = XSpriteBufferP->color;
            *reinterpret_cast<uint32 *>(pPoly.Colours[1]) = XSpriteBufferP->color;
            *reinterpret_cast<uint32 *>(pPoly.Colours[2]) = XSpriteBufferP->color;
            *reinterpret_cast<uint32 *>(pPoly.Colours[3]) = XSpriteBufferP->color;

            RenderViewPolyEx(view, &pPoly, XSpriteBufferP->texture_info, identity, 0, 0.0f);
            XSpriteBufferP++;
        }
    }

    this->position = 0;
}
