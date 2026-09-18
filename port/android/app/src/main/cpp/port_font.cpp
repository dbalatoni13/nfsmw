#include "port_font.h"
#include "assets.h"
#include "gx2d.h"
#include "port_epoly.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FONT_COLS 16
#define FONT_CELL 64.0f
#define FONT_TEX 1024.0f

typedef struct {
    unsigned short uni;
    signed char offx, offy;
    unsigned char w, h;
    short advx;
    unsigned short idx;
} GridGlyph;

static GridGlyph *g_glyphs;
static int g_nglyph;
static unsigned int g_atlas;
static char g_status[80] = "font: off";

static unsigned ru16(const unsigned char *p) {
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

static unsigned ru32(const unsigned char *p) {
    return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24);
}

static const GridGlyph *find_glyph(unsigned uni) {
    int lo = 0, hi = g_nglyph - 1;
    while (lo <= hi) {
        int mid = (lo + hi) >> 1;
        if (g_glyphs[mid].uni == uni) return &g_glyphs[mid];
        if (g_glyphs[mid].uni < uni) lo = mid + 1;
        else hi = mid - 1;
    }
    return NULL;
}

int port_font_load(void) {
    unsigned char *bin = NULL, *tga = NULL, *rgba = NULL;
    int bsz = 0, tsz = 0, w = 0, h = 0, n, i;
    if (g_glyphs) return 1;
    if (!assets_copy("font/font_grid.bin", &bin, &bsz) || bsz < 4) {
        snprintf(g_status, sizeof(g_status), "font: no grid.bin");
        return 0;
    }
    n = (int)ru32(bin);
    if (n <= 0 || n > 512 || 4 + n * 14 > bsz) {
        snprintf(g_status, sizeof(g_status), "font: bad bin");
        free(bin);
        return 0;
    }
    g_glyphs = (GridGlyph *)malloc((size_t)n * sizeof(GridGlyph));
    if (!g_glyphs) {
        free(bin);
        return 0;
    }
    g_nglyph = n;
    for (i = 0; i < n; i++) {
        const unsigned char *p = bin + 4 + i * 14;
        g_glyphs[i].uni = (unsigned short)ru16(p);
        g_glyphs[i].offx = (signed char)p[2];
        g_glyphs[i].offy = (signed char)p[3];
        g_glyphs[i].w = p[4];
        g_glyphs[i].h = p[5];
        g_glyphs[i].advx = (short)ru16(p + 6);
        g_glyphs[i].idx = (unsigned short)ru32(p + 10);
    }
    free(bin);
    if (!assets_copy("font/font_grid.tga", &tga, &tsz) || !tga) {
        snprintf(g_status, sizeof(g_status), "font: no grid.tga");
        return 0;
    }
    if (!assets_decode_tga(tga, tsz, &rgba, &w, &h) || !rgba) {
        free(tga);
        snprintf(g_status, sizeof(g_status), "font: tga decode");
        return 0;
    }
    free(tga);
    g_atlas = gx2d_upload_rgba(rgba, w, h);
    free(rgba);
    snprintf(g_status, sizeof(g_status), "font: grid %d %dx%d", g_nglyph, w, h);
    return g_atlas ? 1 : 0;
}

static void draw_text(float x, float y, float sx, float sy, unsigned int argb, const char *text) {
    float cx = x;
    const char *p;
    unsigned colors[4];
    if (!g_atlas || !g_glyphs || !text) return;
    colors[0] = colors[1] = colors[2] = colors[3] = argb;
    for (p = text; *p; p++) {
        const GridGlyph *g = find_glyph((unsigned char)*p);
        ePoly poly;
        float col, row, s0, t0, s1, t1, x0, y0, x1, y1, adv;
        if (!g) continue;
        adv = (g->advx != 0) ? (float)g->advx : (float)g->w;
        if (g->w == 0 || g->h == 0) {
            cx += adv * sx;
            continue;
        }
        col = (float)(g->idx % FONT_COLS);
        row = (float)(g->idx / FONT_COLS);
        s0 = (col * FONT_CELL) / FONT_TEX;
        t0 = (row * FONT_CELL) / FONT_TEX;
        s1 = (col * FONT_CELL + (float)g->w) / FONT_TEX;
        t1 = (row * FONT_CELL + (float)g->h) / FONT_TEX;
        x0 = cx + (float)g->offx * sx;
        y0 = y + (float)g->offy * sy;
        x1 = x0 + (float)g->w * sx;
        y1 = y0 + (float)g->h * sy;
        epoly_init(&poly);
        epoly_add(&poly, x0, y0, x1, y1, 1.0f, s0, t0, s1, t1, colors);
        gx2d_epoly(&poly, g_atlas);
        cx += adv * sx;
    }
}

void port_font_draw(float x, float y, float sx, float sy, unsigned int argb, const char *text) {
    draw_text(x, y, sx, sy, argb, text);
}

void port_font_draw_raw(float x, float y, float sx, float sy, unsigned int argb, const char *text) {
    draw_text(x, y, sx, sy, argb, text);
}

unsigned int port_font_atlas(void) {
    return g_atlas;
}

const char *port_font_status(void) {
    return g_status;
}
