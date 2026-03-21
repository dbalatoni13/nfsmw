#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/EmitterSystem.h"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include <dolphin/gx/GXGeometry.h>
#include <dolphin/gx/GXVert.h>
#include <dolphin/mtx44_ext.h>

bVector4 BillboardedParticleBasisX;
bVector4 BillboardedParticleBasisY;
int crtVtxFmt;

void eLoadPosMtxImm(bMatrix4 &matrix, GXPosNrmMtx id);
int vsModel(int flags, int matrix_blend);
void ps_Model(int shader_id, void *light_material, int flags);
void ps_NoLighting(int lighting, int flags);
int eSetTexture(TextureInfo *texture_info, int stage);
int eSetBlendMode(TextureInfo *texture_info, unsigned char alpha_mode);

inline bMatrix4 *eViewPlatInfo::GetWorldViewMatrix() {
    return &WorldViewMatrix;
}

int afxBeginBillboardedParticles(eView *view) {
    eViewPlatInfo *plat_info = view->GetPlatInfo();
    Camera *camera = view->GetCamera();
    bMatrix4 *world_view = camera->GetCameraMatrix();
    bMatrix4 *world_view_gc = plat_info->GetWorldViewMatrix();

    {
        eLoadPosMtxImm(*world_view_gc, GX_PNMTX0);
        crtVtxFmt = vsModel(0, -1);
        ps_Model(5, 0, 0);
        ps_NoLighting(1, 0);
    }

    BillboardedParticleBasisX.x = world_view->v0.x;
    BillboardedParticleBasisX.y = world_view->v1.x;
    BillboardedParticleBasisX.z = world_view->v2.x;
    BillboardedParticleBasisX.w = 0.0f;
    BillboardedParticleBasisY.x = world_view->v0.y;
    BillboardedParticleBasisY.y = world_view->v1.y;
    BillboardedParticleBasisY.z = world_view->v2.y;
    BillboardedParticleBasisY.w = 0.0f;
    return 1;
}

int afxBeginBillboardedParticleBatch(TextureInfo *texture_info) {
    eSetTexture(texture_info, 0);
    eSetBlendMode(texture_info, 0);
    return 1;
}

bool PlatStartParticleRender(eView *view, TextureInfo *mTextureInfo, unsigned int mNumParticles) {
    afxBeginBillboardedParticles(view);
    afxBeginBillboardedParticleBatch(mTextureInfo);
    return 1;
}

int afxEndBillboardedParticleBatch(TextureInfo *texture_info, float f, int i) {
    return 1;
}

int afxEndBillboardedParticles() {
    return 1;
}

void PlatEndParticleRender() {
    afxEndBillboardedParticleBatch(0, 0.0f, 0);
    afxEndBillboardedParticles();
}

void PlatAddParticle(const EmitterParticle &particle, const UMath::Vector3 &upVec, const UMath::Vector3 &rightVec,
                     unsigned int hack_flags, bVector4 *x_constrain_basis, bVector4 *y_constrain_basis) {
    float particle_scale_factor = particle.mSize;
    bVector3 bx(BillboardedParticleBasisX.x * particle.mSize,
                BillboardedParticleBasisX.y * particle.mSize,
                BillboardedParticleBasisX.z * particle.mSize);
    bVector3 by(BillboardedParticleBasisY.x * particle.mSize,
                BillboardedParticleBasisY.y * particle.mSize,
                BillboardedParticleBasisY.z * particle.mSize);
    unsigned int colour = particle.mColour;
    const float fs0 = static_cast<float>(particle.mUVStart >> 16) * (1.0f / 65535.0f);
    const float ft0 = static_cast<float>(particle.mUVStart & 0xFFFF) * (1.0f / 65535.0f);
    const float fs1 = static_cast<float>(particle.mUVEnd >> 16) * (1.0f / 65535.0f);
    const float ft1 = static_cast<float>(particle.mUVEnd & 0xFFFF) * (1.0f / 65535.0f);

    GXBegin(GX_QUADS, static_cast<GXVtxFmt>(crtVtxFmt), 4);

    GXPosition3f32(particle.mPosX + bx.x + by.x,
                   particle.mPosY + bx.y + by.y,
                   particle.mPosZ + bx.z + by.z);
    GXColor1u32(colour);
    GXTexCoord2f32(fs1, ft1);

    GXPosition3f32(particle.mPosX - bx.x + by.x,
                   particle.mPosY - bx.y + by.y,
                   particle.mPosZ - bx.z + by.z);
    GXColor1u32(colour);
    GXTexCoord2f32(fs0, ft1);

    GXPosition3f32(particle.mPosX - bx.x - by.x,
                   particle.mPosY - bx.y - by.y,
                   particle.mPosZ - bx.z - by.z);
    GXColor1u32(colour);
    GXTexCoord2f32(fs0, ft0);

    GXPosition3f32(particle.mPosX + bx.x - by.x,
                   particle.mPosY + bx.y - by.y,
                   particle.mPosZ + bx.z - by.z);
    GXColor1u32(colour);
    GXTexCoord2f32(fs1, ft0);
    GXEnd();
}

void PlatGetViewVectors(eView *view, UMath::Vector3 &right, UMath::Vector3 &up, UMath::Vector3 &forward) {
    eViewPlatInfo *plat_info = view->GetPlatInfo();
    Mtx44 local_matrix;
    PSMTX44Copy(reinterpret_cast<const float(*)[4]>(&plat_info->WorldViewMatrix), local_matrix);
    right.x = local_matrix[0][0];
    right.y = local_matrix[1][1];
    right.z = local_matrix[2][2];
    up.x = local_matrix[0][0];
    up.y = local_matrix[1][1];
    up.z = local_matrix[2][2];
    forward.x = local_matrix[0][0];
    forward.y = local_matrix[1][1];
    forward.z = local_matrix[2][2];
}
