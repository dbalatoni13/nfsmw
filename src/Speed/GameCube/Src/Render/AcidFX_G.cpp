#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

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
