#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/GameCube/Src/Ecstasy/eMatrixE.hpp"
#include "Speed/GameCube/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#include <dolphin.h>

struct eLightMaterial;

enum eModelPixelShaderId {
    EPSHADER_PARTICLE = 5,
};

int afxBeginBillboardedParticles(eView *view);
int afxBeginBillboardedParticleBatch(TextureInfo *texture);
int afxEndBillboardedParticleBatch(TextureInfo *texture, float f, int n);
int afxEndBillboardedParticles();
int eSetTexture(TextureInfo *texture_info, int stage);
void eSetBlendMode(TextureInfo *texture_info, unsigned char opt);
int vsModel(int opt, int unused);
int ps_Model(eModelPixelShaderId id, eLightMaterial *lm, int opt);

int crtVtxFmt;
bVector4 BillboardedParticleBasisX;
bVector4 BillboardedParticleBasisY;

int afxBeginBillboardedParticles(eView *view) {
    Camera *camera = view->GetCamera();
    eLoadPosMtxImm(*reinterpret_cast<bMatrix4 *>(view->GetPlatInfo()), GX_PNMTX0);
    crtVtxFmt = vsModel(0, -1);
    ps_Model(EPSHADER_PARTICLE, 0, 0);
    ps_NoLighting(1, 0);

    bMatrix4 *m = camera->GetCameraMatrix();
    BillboardedParticleBasisX.x = m->v0.x;
    BillboardedParticleBasisX.y = m->v1.x;
    BillboardedParticleBasisX.z = m->v2.x;
    BillboardedParticleBasisX.w = 0.0f;
    BillboardedParticleBasisY.x = m->v0.y;
    BillboardedParticleBasisY.y = m->v1.y;
    BillboardedParticleBasisY.z = m->v2.y;
    BillboardedParticleBasisY.w = 0.0f;
    return 1;
}

int afxBeginBillboardedParticleBatch(TextureInfo *texture) {
    eSetTexture(texture, 0);
    eSetBlendMode(texture, 0);
    return 1;
}

int afxEndBillboardedParticleBatch(TextureInfo *texture, float f, int n) {
    return 1;
}

int afxEndBillboardedParticles() {
    return 1;
}
// Los tres vectores salen iguales: columnas swizzleadas de la WorldViewMatrix
// tal cual las consume el render de particulas billboard en GC.
void PlatGetViewVectors(eView *view, UMath::Vector3 &right, UMath::Vector3 &up, UMath::Vector3 &fwd) {
    Mtx44 m;
    eViewPlatInfo *plat_info = view->GetPlatInfo();

    PSMTX44Copy(reinterpret_cast<const float(*)[4]>(&plat_info->WorldViewMatrix), m);
    right.x = m[0][0];
    right.y = m[1][1];
    right.z = m[2][2];
    up.x = m[0][0];
    up.y = m[1][1];
    up.z = m[2][2];
    fwd.x = m[0][0];
    fwd.y = m[1][1];
    fwd.z = m[2][2];
}

bool PlatStartParticleRender(eView *view, TextureInfo *texture, uint32 flags) {
    afxBeginBillboardedParticles(view);
    afxBeginBillboardedParticleBatch(texture);
    return 1;
}

void PlatEndParticleRender() {
    afxEndBillboardedParticleBatch(0, 0.0f, 0);
    afxEndBillboardedParticles();
}


// Un quad por particula, en modo inmediato sobre el FIFO. Las UV llegan en dos
// palabras de 16 bits (mUVStart/mUVEnd: u0v0 y u1v1) y se normalizan dividiendo
// por 65535. Los cuatro vertices combinan la posicion de la particula con las
// bases billboard escaladas por su tamano; el color se copia tal cual.
void PlatAddParticle(const EmitterParticle &particle, const UMath::Vector3 &upVec, const UMath::Vector3 &rightVec, unsigned int hack_flags,
                     bVector4 *x_constrain_basis, bVector4 *y_constrain_basis) {
    unsigned int color = particle.mColour;
    float u0 = (float)(particle.mUVStart >> 16) / 65535.0f;
    float v0 = (float)(particle.mUVStart & 0xFFFF) / 65535.0f;
    float u1 = (float)(particle.mUVEnd >> 16) / 65535.0f;
    float v1 = (float)(particle.mUVEnd & 0xFFFF) / 65535.0f;

    // Los escalados de las dos bases viven en pila y se releen por vertice,
    // igual que en el original.
    float xbasis[3];
    float ybasis[3];
    xbasis[0] = BillboardedParticleBasisX.x * particle.mSize;
    xbasis[1] = BillboardedParticleBasisX.y * particle.mSize;
    xbasis[2] = BillboardedParticleBasisX.z * particle.mSize;
    ybasis[0] = BillboardedParticleBasisY.x * particle.mSize;
    ybasis[1] = BillboardedParticleBasisY.y * particle.mSize;
    ybasis[2] = BillboardedParticleBasisY.z * particle.mSize;

    GXBegin(GX_QUADS, (GXVtxFmt)crtVtxFmt, 4);

    GXPosition3f32(particle.mPosX + xbasis[0] + ybasis[0], particle.mPosY + xbasis[1] + ybasis[1], particle.mPosZ + xbasis[2] + ybasis[2]);
    GXColor1u32(color);
    GXTexCoord2f32(u1, v1);

    GXPosition3f32(particle.mPosX - xbasis[0] + ybasis[0], particle.mPosY - xbasis[1] + ybasis[1], particle.mPosZ - xbasis[2] + ybasis[2]);
    GXColor1u32(color);
    GXTexCoord2f32(u0, v1);

    GXPosition3f32(particle.mPosX - xbasis[0] - ybasis[0], particle.mPosY - xbasis[1] - ybasis[1], particle.mPosZ - xbasis[2] - ybasis[2]);
    GXColor1u32(color);
    GXTexCoord2f32(u0, v0);

    GXPosition3f32(particle.mPosX + xbasis[0] - ybasis[0], particle.mPosY + xbasis[1] - ybasis[1], particle.mPosZ + xbasis[2] - ybasis[2]);
    GXColor1u32(color);
    GXTexCoord2f32(u1, v0);
}
