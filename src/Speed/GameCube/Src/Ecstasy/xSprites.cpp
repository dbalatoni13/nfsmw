#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

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

extern bMatrix4 eMathIdentityMatrix;

void RenderViewPolyEx(eView *view, ePoly *poly, TextureInfo *texture_info, bMatrix4 *matrix, int flags, float z_bias)
    __asm__("Render__18eViewPlatInterfaceP5ePolyP11TextureInfoP8bMatrix4if");

void XSpriteManager::AddSpark(const NGParticle &particle, TextureInfo *CurrentTexture) {
    if (this->position < 300) {
        UMath::Vector3 startPos;
        UMath::Vector3 endPos;
        float endAge = static_cast<float>(particle.length) * (1.0f / 2048.0f) + particle.age;
        float width = static_cast<float>(particle.width) * (1.0f / 2048.0f);
        SpriteDef *XSpriteBufferP = &this->XSpriteBuffer[this->position];

        UMath::ScaleAdd(particle.initialPos, particle.age, particle.vel, startPos);
        startPos.z += particle.gravity * particle.age * particle.age;
        UMath::ScaleAdd(particle.initialPos, endAge, particle.vel, endPos);
        endPos.z += particle.gravity * endAge * endAge;

        XSpriteBufferP->texture_info = CurrentTexture;
        XSpriteBufferP->color =
            particle.color >> 24 | particle.color >> 8 & 0xFF00 | (particle.color & 0xFF00) << 8 | particle.color << 24;
        XSpriteBufferP->width = width;
        XSpriteBufferP->startPos = startPos;
        XSpriteBufferP->EndPosPos = endPos;
        this->position++;
    }
}

void XSpriteManager::RenderAll(eView *view) {
    unsigned int i = 0;
    ePoly poly;

    if (this->position != 0) {
        do {
            SpriteDef *sprite = &this->XSpriteBuffer[i];

            poly.Vertices[0] = sprite->startPos;
            poly.Vertices[1] = sprite->startPos;
            poly.Vertices[1].z += sprite->width;
            poly.Vertices[2] = sprite->EndPosPos;
            poly.Vertices[2].z += sprite->width;
            poly.Vertices[3] = sprite->EndPosPos;

            *reinterpret_cast<uint32 *>(poly.Colours[0]) = sprite->color;
            *reinterpret_cast<uint32 *>(poly.Colours[1]) = sprite->color;
            *reinterpret_cast<uint32 *>(poly.Colours[2]) = sprite->color;
            *reinterpret_cast<uint32 *>(poly.Colours[3]) = sprite->color;

            i++;
            RenderViewPolyEx(view, &poly, sprite->texture_info, &eMathIdentityMatrix, 0, 0.0f);
        } while (i < this->position);
    }

    this->position = 0;
}
