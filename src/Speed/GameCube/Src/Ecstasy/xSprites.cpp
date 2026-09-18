#include "Speed/GameCube/Src/Ecstasy/xSprites.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

XSpriteManager NGSpriteManager;

void XSpriteManager::AddSpark(const NGParticle &particle, TextureInfo *CurrentTexture) {
    if (this->position > 299) {
        return;
    }

    {
        UMath::Vector3 startPos;
        UMath::ScaleAdd(particle.vel, particle.age, particle.initialPos, startPos);
        startPos.z += particle.gravity * particle.age * particle.age;

        float endAge = static_cast<float>(static_cast<int>(particle.length)) * (1.0f / 2048.0f) + particle.age;
        UMath::Vector3 endPos;
        UMath::ScaleAdd(particle.vel, endAge, particle.initialPos, endPos);
        endPos.z += particle.gravity * endAge * endAge;

        SpriteDef *XSpriteBufferP = &this->XSpriteBuffer[this->position];
        XSpriteBufferP->texture_info = CurrentTexture;
        XSpriteBufferP->color = (particle.color >> 24) | ((particle.color >> 8) & 0xFF00) |
                                ((particle.color << 8) & 0xFF0000) | (particle.color << 24);
        XSpriteBufferP->startPos = startPos;
        XSpriteBufferP->EndPosPos = endPos;
        XSpriteBufferP->width = static_cast<float>(static_cast<int>(particle.width)) * (1.0f / 2048.0f);
        this->position++;
    }
}

void XSpriteManager::RenderAll(eView *view) {
    ePoly pPoly;
    SpriteDef *XSpriteBufferP = this->XSpriteBuffer;

    for (unsigned int i = 0; i < this->position; i++) {
        *reinterpret_cast<unsigned int *>(&pPoly.Colours[0][0]) = XSpriteBufferP->color;
        *reinterpret_cast<unsigned int *>(&pPoly.Colours[1][0]) = XSpriteBufferP->color;
        *reinterpret_cast<unsigned int *>(&pPoly.Colours[2][0]) = XSpriteBufferP->color;
        *reinterpret_cast<unsigned int *>(&pPoly.Colours[3][0]) = XSpriteBufferP->color;

        pPoly.Vertices[0] = XSpriteBufferP->startPos;
        pPoly.Vertices[1] = XSpriteBufferP->startPos;
        pPoly.Vertices[1].z = XSpriteBufferP->startPos.z + XSpriteBufferP->width;

        pPoly.Vertices[3] = XSpriteBufferP->EndPosPos;
        pPoly.Vertices[2] = XSpriteBufferP->EndPosPos;
        pPoly.Vertices[2].z = XSpriteBufferP->EndPosPos.z + XSpriteBufferP->width;

        bMatrix4 *identity = eGetIdentityMatrix();

        view->Render(&pPoly, XSpriteBufferP->texture_info, identity, 0, 0.0f);
        XSpriteBufferP++;
    }

    this->position = 0;
}
