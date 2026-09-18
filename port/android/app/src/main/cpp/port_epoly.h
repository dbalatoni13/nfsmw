#ifndef PORT_EPOLY_H
#define PORT_EPOLY_H

#ifdef __cplusplus
extern "C" {
#endif

enum {
    EPOLY_APPLYASPECT = 1,
    EPOLY_APPLYZSORT = 2,
    EPOLY_MULTI_TEXT_MASK = 4
};

typedef struct {
    float x, y, z, pad;
} ePolyVec3;

typedef struct {
    ePolyVec3 Vertices[4];
    float UVs[2][4];
    float UVsMask[2][4];
    unsigned char Colours[4][4];
    unsigned char flags;
    unsigned char Flailer;
} ePoly;

void epoly_init(ePoly *p);
void epoly_add(ePoly *p, float x0, float y0, float x1, float y1, float z,
               float s0, float t0, float s1, float t1, const unsigned int colors[4]);
void gx2d_epoly(const ePoly *p, unsigned int tex);

#ifdef __cplusplus
}
#endif

#endif
