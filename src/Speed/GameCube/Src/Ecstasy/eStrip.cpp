#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eStrip.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/GameCube/Src/Ecstasy/eMatrixE.hpp"
#include "Speed/GameCube/Src/Ecstasy/eViewPlat.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include <dolphin.h>

// TODO poner en las cabeceras que toquen
int eSetTexture(TextureInfo *texture_info, int stage);
void eSetBlendMode(TextureInfo *texture_info, unsigned char opt);
int vsScreen(int opt);
int psModulate();

eStrip *e_current_strip = 0;
eStripVert *e_strip_verts;
TextureInfo *e_strip_texture_info = 0;
int e_current_strip_vert = 0;
int e_current_strip_uv = 0;
int e_current_strip_col = 0;
bMatrix4 e_strip_matrix;
eStrip eSTRIP;

bool eBeginStrip(TextureInfo *a, int b, bMatrix4 *c) {
    unsigned char *buf;
    e_current_strip = &eSTRIP;
    e_current_strip_vert = 0;
    e_current_strip_uv = 0;
    e_current_strip_col = 0;
    e_strip_texture_info = a;
    if (c) {
        bMemCpy(&e_strip_matrix, c, 0x40);
    } else {
        bIdentity(&e_strip_matrix);
    }
    return true;
}

bool eEndStrip(eView *view) {
    if (view == nullptr) {
        return false;
    }

    eViewPlatInfo *plat_view = view->GetPlatInfo();
    bMatrix4 mL2V; // r1+0x8
    int crtVtxFmt;
    int nVerts;

    e_current_strip->m_NumVerts = e_current_strip_vert;

    eMulMatrix(&mL2V, &e_strip_matrix, plat_view->GetWorldViewMatrix());
    eLoadPosMtxImm(mL2V, GX_PNMTX0);

    eSetTexture(e_strip_texture_info, 0);
    eSetBlendMode(e_strip_texture_info, 0);

    crtVtxFmt = vsScreen(0);
    psModulate();
    ps_NoLighting(1, 0);

    nVerts = e_current_strip->m_NumVerts;
    GXBegin(GX_TRIANGLESTRIP, (GXVtxFmt)crtVtxFmt, nVerts);

    for (int i = 0; i < nVerts; i++) {
        GXPosition3f32(e_current_strip->m_StripVerts[i].x, e_current_strip->m_StripVerts[i].y, e_current_strip->m_StripVerts[i].z);
        GXColor1u32(e_current_strip->m_StripVerts[i].col);
        GXTexCoord2f32(e_current_strip->m_StripVerts[i].u, e_current_strip->m_StripVerts[i].v);
    }

    GXEnd();

    ps_NoLighting(0, 0);
    psReset(RESET_OPTION_DEFAULT);

    return true;
}

static unsigned int rgbatoargb(unsigned char *c) {
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;

    a = c[3] >> 1;
    b = c[2] >> 1;
    g = c[1] >> 1;
    r = c[0] << 1;
    return (a << 24) | (b << 16) | (g << 8) | r;
}

void eAddColour(unsigned int colour) {
    e_current_strip->m_StripVerts[e_current_strip_col].col = rgbatoargb(reinterpret_cast<unsigned char *>(&colour));
    e_current_strip_col++;
}

void eAddUV(float u, float v) {
    e_current_strip->m_StripVerts[e_current_strip_uv].u = u;
    e_current_strip->m_StripVerts[e_current_strip_uv].v = v;
    e_current_strip_uv++;
}

void eAddVertex(const bVector3 &v) {
    e_current_strip->m_StripVerts[e_current_strip_vert].x = v.x;
    e_current_strip->m_StripVerts[e_current_strip_vert].y = v.y;
    e_current_strip->m_StripVerts[e_current_strip_vert].z = v.z;
    e_current_strip_vert++;
}

void exAddColour(unsigned int colour) {
    eAddColour(colour);
}

void exAddUV(float u, float v) {
    eAddUV(u, v);
}

void exAddVertex(const bVector3 &v) {
    eAddVertex(v);
}

bool exBeginStrip(TextureInfo *tex, int a, bMatrix4 *mat) {
    return eBeginStrip(tex, a, mat);
}

bool exEndStrip(eView *view) {
    return eEndStrip(view);
}
