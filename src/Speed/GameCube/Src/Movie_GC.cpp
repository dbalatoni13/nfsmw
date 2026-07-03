#include "G.hpp"

// void MoviePlayer::FillInTextureInfo(unsigned int *framer_address /* r4 */, struct TextureInfo *texture_info /* r5 */,
//                                     struct Shape *yuv_shape /* r6 */) {
//     // Range: 0x8026DB58 -> 0x8026DB5C
//     // inline void GCHW_VD::SetFrame(struct Shape * yuvshp) {}
// }

void GCDrawMovie() {}

void PlatSetFirstMovieFrame(struct TextureInfo *texture_info /* r3 */, struct Shape *yuv_shape /* r30 */, bool isVP6Movie /* r31 */) {
    // // Range: 0x8026DBB8 -> 0x8026DBB8
    // inline void *operator new(unsigned int size, const char *file, int line) {}
}

unsigned int RCMP_GetMaxFramesOutStanding() {}

void PlatFinishMovie() {}

// GCHW_VD::GCHW_VD(struct Shape *yuvshp /* r30 */, bool isVP6Movie /* r5 */) {
//     // Local variables
//     int w; // r24
//     int h; // r31

//     // Range: 0x8026DC40 -> 0x8026DC40
//     inline int Shape::GetWidth() const {
//         // Range: 0x8026DC40 -> 0x8026DC40
//         inline int TextureElement::GetWidth() const {}
//     }

//     // Range: 0x8026DC40 -> 0x8026DC40
//     inline int Shape::GetHeight() const {
//         // Range: 0x8026DC40 -> 0x8026DC40
//         inline int TextureElement::GetHeight() const {}
//     }
// }

// GCHW_VD::~GCHW_VD() {}

// void GCHW_VD::iDraw() {
// // Local variables
// char *y;            // r23
// int w;              // r24
// int h;              // r31
// char *cb;           // r22
// char *cr;           // r20
// unsigned long size; // r29
// float u0;           // f27
// float u1;           // f28
// float v0;           // f26
// float v1;           // f25

// // Range: 0x8026DE48 -> 0x8026DE5C
// inline const void *ShapeElement::GetData() const {}

// // Range: 0x8026DE5C -> 0x8026DE5C
// inline int Shape::GetWidth() const {
//     // Range: 0x8026DE5C -> 0x8026DE5C
//     inline int TextureElement::GetWidth() const {}
// }

// // Range: 0x8026DE5C -> 0x8026DE5C
// inline int Shape::GetHeight() const {
//     // Range: 0x8026DE5C -> 0x8026DE5C
//     inline int TextureElement::GetHeight() const {}
// }

// /* anonymous block */ {
//     // Range: 0x8026DE7C -> 0x8026DE7C
//     const int vp6Border;
//     int dataOfs; // r10
// }

// // Range: 0x8026DEF0 -> 0x8026DEF0
// inline void GXSetTexCoordGen(enum _GXTexCoordID dst_coord, enum _GXTexGenType func, enum _GXTexGenSrc src_param, unsigned int mtx) {}

// /* anonymous block */ {
//     // Range: 0x8026E198 -> 0x8026E198
//     float m_l; // f29
//     float m_t;
//     float m_r; // f30
//     float m_b; // f31
//     float m_z;

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXPosition3f32(const float x, const float y, const float z) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXColor1u32(const unsigned long x) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXTexCoord2f32(const float x, const float y) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXPosition3f32(const float x, const float y, const float z) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXColor1u32(const unsigned long x) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXTexCoord2f32(const float x, const float y) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXPosition3f32(const float x, const float y, const float z) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXColor1u32(const unsigned long x) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXTexCoord2f32(const float x, const float y) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXPosition3f32(const float x, const float y, const float z) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXColor1u32(const unsigned long x) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXTexCoord2f32(const float x, const float y) {}

//     // Range: 0x8026E198 -> 0x8026E198
//     inline void GXEnd() {}
// }
// }
