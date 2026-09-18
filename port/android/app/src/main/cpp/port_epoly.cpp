#include "port_epoly.h"
#include "gx2d.h"

#include <string.h>

void epoly_init(ePoly *p) {
    memset(p, 0, sizeof(*p));
    p->Vertices[0].x = 0.0f;
    p->Vertices[0].y = 0.0f;
    p->Vertices[1].x = 64.0f;
    p->Vertices[1].y = 0.0f;
    p->Vertices[2].x = 64.0f;
    p->Vertices[2].y = 64.0f;
    p->Vertices[3].x = 0.0f;
    p->Vertices[3].y = 64.0f;
    p->Colours[0][0] = p->Colours[0][1] = p->Colours[0][2] = p->Colours[0][3] = 0x80;
    p->Colours[1][0] = p->Colours[1][1] = p->Colours[1][2] = p->Colours[1][3] = 0x80;
    p->Colours[2][0] = p->Colours[2][1] = p->Colours[2][2] = p->Colours[2][3] = 0x80;
    p->Colours[3][0] = p->Colours[3][1] = p->Colours[3][2] = p->Colours[3][3] = 0x80;
    p->UVs[0][0] = 0.0f;
    p->UVs[0][1] = 0.0f;
    p->UVs[0][2] = 1.0f;
    p->UVs[0][3] = 0.0f;
    p->UVs[1][0] = 1.0f;
    p->UVs[1][1] = 1.0f;
    p->UVs[1][2] = 0.0f;
    p->UVs[1][3] = 1.0f;
    p->Flailer = 0;
}

static void store_argb(unsigned char out[4], unsigned int c) {
    out[0] = (unsigned char)((c >> 24) & 0xff);
    out[1] = (unsigned char)((c >> 16) & 0xff);
    out[2] = (unsigned char)((c >> 8) & 0xff);
    out[3] = (unsigned char)(c & 0xff);
    if (out[0] == 0) out[0] = 0xff;
}

void epoly_add(ePoly *p, float x0, float y0, float x1, float y1, float z,
               float s0, float t0, float s1, float t1, const unsigned int colors[4]) {
    p->Vertices[0].x = x0;
    p->Vertices[0].y = y0;
    p->Vertices[0].z = z;
    p->Vertices[1].x = x1;
    p->Vertices[1].y = y0;
    p->Vertices[1].z = z;
    p->Vertices[2].x = x1;
    p->Vertices[2].y = y1;
    p->Vertices[2].z = z;
    p->Vertices[3].x = x0;
    p->Vertices[3].y = y1;
    p->Vertices[3].z = z;
    p->UVs[0][0] = s0;
    p->UVs[0][1] = t0;
    p->UVs[0][2] = s1;
    p->UVs[0][3] = t0;
    p->UVs[1][0] = s1;
    p->UVs[1][1] = t1;
    p->UVs[1][2] = s0;
    p->UVs[1][3] = t1;
    store_argb(p->Colours[0], colors[0]);
    store_argb(p->Colours[1], colors[1]);
    store_argb(p->Colours[2], colors[2]);
    store_argb(p->Colours[3], colors[3]);
    p->Flailer = 1;
    p->flags = (unsigned char)EPOLY_APPLYASPECT;
}

void gx2d_epoly(const ePoly *p, unsigned int tex) {
    unsigned int colors[4];
    int i;
    if (!p) return;
    for (i = 0; i < 4; i++) {
        colors[i] = ((unsigned int)p->Colours[i][0] << 24)
                  | ((unsigned int)p->Colours[i][1] << 16)
                  | ((unsigned int)p->Colours[i][2] << 8)
                  | ((unsigned int)p->Colours[i][3]);
    }
    gx2d_fe_poly(p->Vertices[0].x, p->Vertices[0].y, p->Vertices[2].x, p->Vertices[2].y,
                 p->UVs[0][0], p->UVs[0][1], p->UVs[1][0], p->UVs[1][1], colors, tex);
}
