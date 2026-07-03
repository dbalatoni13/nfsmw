#ifndef _ESTRIPH_
#define _ESTRIPH_

#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"

// total size: 0x18
struct eStripVert {
    float x;          // offset 0x0, size 0x4
    float y;          // offset 0x4, size 0x4
    float z;          // offset 0x8, size 0x4
    unsigned int col; // offset 0xC, size 0x4
    float u;          // offset 0x10, size 0x4
    float v;          // offset 0x14, size 0x4
};

// total size: 0x604
struct eStrip {
    int m_NumVerts;              // offset 0x0, size 0x4
    eStripVert m_StripVerts[64]; // offset 0x4, size 0x600
};

bool eBeginStrip(TextureInfo *a, int b, bMatrix4 *c);
bool eEndStrip(eView *view);
void eAddVertex(const bVector3 &v);
void eAddColour(unsigned int colour);
void eAddUV(float u, float v);
bool exBeginStrip(TextureInfo *tex, int a, bMatrix4 *mat);
void exAddVertex(const bVector3 &v);
void exAddColour(unsigned int colour);
void exAddUV(float u, float v);
bool exEndStrip(eView *view);

#endif
