#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct eStripVert {
    // total size: 0x18
    float x;          // offset 0x0, size 0x4
    float y;          // offset 0x4, size 0x4
    float z;          // offset 0x8, size 0x4
    unsigned int col; // offset 0xC, size 0x4
    float u;          // offset 0x10, size 0x4
    float v;          // offset 0x14, size 0x4
};

struct eStrip {
    // total size: 0x604
    int m_NumVerts;              // offset 0x0, size 0x4
    eStripVert m_StripVerts[64]; // offset 0x4, size 0x600
};

eStrip *e_current_strip;
eStripVert *e_strip_verts;
TextureInfo *e_strip_texture_info;
int e_current_strip_vert;
int e_current_strip_uv;
int e_current_strip_col;
bMatrix4 e_strip_matrix;
eStrip eSTRIP;

bool eBeginStrip(TextureInfo *a, int b, bMatrix4 *c) {
    unsigned char *buf;
    e_current_strip = &eSTRIP;
    e_current_strip_col = 0;
    e_current_strip_vert = 0;
    e_current_strip_uv = 0;
    e_strip_texture_info = a;
    if (c != nullptr) {
        bMemCpy(&e_strip_matrix, c, 0x40);
    } else {
        bIdentity(&e_strip_matrix);
    }
    return true;
}

bool eEndStrip(eView *view) {
    // Local variables
    eViewPlatInfo *plat_view;
    bMatrix4 mL2V; // r1+0x8
    int crtVtxFmt; // r30
    int nVerts;    // r30

    //   // Blocks
    //   /* anonymous block */ {
    //     // Range: 0x8010E6B8 -> 0x8010E7AC
    //     int i; // r10

    //     inline void GXPosition3f32(const float x, const float y, const float z, const float x /* f0 */, const float y /* f13 */,
    //                                const float z /* f12 */) {}
    //     inline void GXColor1u32(const unsigned long x, const unsigned long x /* r0 */) {}
    //     inline void GXTexCoord2f32(const float x, const float y, const float x /* f0 */, const float y /* f13 */) {}
    //   }

    //   inline struct eViewPlatInfo *eViewPlatInterface::GetPlatInfo(struct eViewPlatInterface *const this,
    //                                                                struct eViewPlatInterface *const this /* r3 */) {}
    //   inline struct bMatrix4 *bMatrix4::bMatrix4(struct bMatrix4 *const this, struct bMatrix4 *const this /* r29 */) {
    //     inline struct bVector4 *bVector4::bVector4(struct bVector4 *const this, struct bVector4 *const this /* r29 */) {}
    //     inline struct bVector4 *bVector4::bVector4(struct bVector4 *const this, struct bVector4 *const this) {}
    //     inline struct bVector4 *bVector4::bVector4(struct bVector4 *const this, struct bVector4 *const this) {}
    //     inline struct bVector4 *bVector4::bVector4(struct bVector4 *const this, struct bVector4 *const this) {}
    //   }
    //   inline struct bMatrix4 *eViewPlatInfo::GetWorldViewMatrix(struct eViewPlatInfo *const this, struct eViewPlatInfo *const this) {}
    //   inline void GXEnd() {}
}

bool exBeginStrip(TextureInfo *tex, int a, bMatrix4 *mat) {
    return eBeginStrip(tex, a, mat);
}

bool exEndStrip(eView *view) {
    return eEndStrip(view);
}
