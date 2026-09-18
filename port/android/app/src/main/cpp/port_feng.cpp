#include "port_feng.h"
#include "assets.h"
#include "gx2d.h"
#include "port_epoly.h"
#include "port_font.h"
#include "port_pixfont.h"

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FE_ID(a, b, c, d) ((unsigned)(a) | ((unsigned)(b) << 8) | ((unsigned)(c) << 16) | ((unsigned)(d) << 24))
#define FE_TAG(a, b) ((unsigned)(a) | ((unsigned)(b) << 8))
#define CH_NESTED 0x80000000u
#define CH_FENG (FE_ID('F', 'E', 'n', 'g') | CH_NESTED)
#define CH_PKHD FE_ID('P', 'k', 'H', 'd')
#define CH_RESL (FE_ID('R', 'e', 's', 'L') | CH_NESTED)
#define CH_RSNM FE_ID('R', 's', 'N', 'm')
#define CH_RSRQ FE_ID('R', 's', 'R', 'q')
#define CH_OBJL (FE_ID('O', 'b', 'j', 'L') | CH_NESTED)
#define CH_FOBJ (FE_ID('F', 'O', 'b', 'j') | CH_NESTED)
#define CH_OBJD FE_ID('O', 'b', 'j', 'D')
#define TG_OT FE_TAG('O', 't')
#define TG_OP FE_TAG('O', 'P')
#define TG_PA FE_TAG('P', 'A')
#define TG_SA FE_TAG('S', 'A')
#define TG_ST FE_TAG('S', 't')
#define FE_IMAGE 1
#define FE_STRING 2
#define FE_GROUP 5
#define FE_COLOREDIMAGE 9
#define PORT_FENG_MAX_OBJ 128
#define PORT_FENG_MAX_RES 32

typedef struct {
    unsigned type;
    unsigned flags;
    unsigned guid;
    unsigned parent;
    unsigned res;
    int col_b, col_g, col_r, col_a;
    float px, py, sx, sy;
    float u0, v0, u1, v1;
    char text[80];
} PortFEObj;

static unsigned char *g_pkg;
static int g_pkg_sz;
static PortFEObj g_objs[PORT_FENG_MAX_OBJ];
static int g_nobj;
static unsigned int g_logo_tex;
static unsigned int g_circle_tex;
static unsigned int g_res_tex[PORT_FENG_MAX_RES];
static char g_res_name[PORT_FENG_MAX_RES][40];
static int g_nres;
static char g_status[96] = "feng: off";
static char g_name[32];

static unsigned ru32(const unsigned char *p) {
    return (unsigned)p[0] | ((unsigned)p[1] << 8) | ((unsigned)p[2] << 16) | ((unsigned)p[3] << 24);
}

static unsigned ru16(const unsigned char *p) {
    return (unsigned)p[0] | ((unsigned)p[1] << 8);
}

static float rf32(const unsigned char *p) {
    unsigned u = ru32(p);
    float f;
    memcpy(&f, &u, 4);
    return f;
}

static int ri32(const unsigned char *p) {
    return (int)ru32(p);
}

static int clamp8(int v) {
    if (v < 0) return 0;
    if (v > 255) return 255;
    return v;
}

static const unsigned char *find_child(const unsigned char *chunk, unsigned want) {
    unsigned sz = ru32(chunk + 4);
    const unsigned char *p = chunk + 8;
    const unsigned char *end = chunk + 8 + sz;
    while (p + 8 <= end) {
        unsigned id = ru32(p);
        unsigned cs = ru32(p + 4);
        if (p + 8 + cs > end) break;
        if (id == want) return p;
        p += 8 + cs;
    }
    return NULL;
}

static int parse_object(const unsigned char *fobj) {
    const unsigned char *objd = find_child(fobj, CH_OBJD);
    PortFEObj *o;
    const unsigned char *p;
    const unsigned char *end;
    if (!objd || g_nobj >= PORT_FENG_MAX_OBJ) return 0;
    o = &g_objs[g_nobj];
    memset(o, 0, sizeof(*o));
    o->res = 0xFFFF;
    o->col_a = 255;
    o->sx = 1.0f;
    o->sy = 1.0f;
    o->u1 = 1.0f;
    o->v1 = 1.0f;
    p = objd + 8;
    end = objd + 8 + ru32(objd + 4);
    while (p + 4 <= end) {
        unsigned id = ru16(p);
        unsigned sz = ru16(p + 2);
        const unsigned char *d = p + 4;
        if (d + sz > end) break;
        if (id == TG_OT && sz >= 4) {
            o->type = ru32(d);
        } else if (id == TG_OP && sz >= 16) {
            o->guid = ru32(d);
            o->flags = ru32(d + 8);
            o->res = ru32(d + 12);
        } else if (id == TG_PA && sz >= 4) {
            o->parent = ru32(d);
        } else if (id == TG_ST && sz >= 2) {
            unsigned n = 0;
            unsigned k;
            for (k = 0; k + 1 < sz && n + 1 < sizeof(o->text); k += 2) {
                unsigned c = ru16(d + k);
                if (c == 0) break;
                o->text[n++] = (c < 128) ? (char)c : '?';
            }
            o->text[n] = 0;
        } else if (id == TG_SA && sz >= 0x44) {
            o->col_b = ri32(d + 0);
            o->col_g = ri32(d + 4);
            o->col_r = ri32(d + 8);
            o->col_a = ri32(d + 12);
            o->px = rf32(d + 0x1C);
            o->py = rf32(d + 0x20);
            o->sx = rf32(d + 0x38);
            o->sy = rf32(d + 0x3C);
            if (sz >= 0x54) {
                o->u0 = rf32(d + 0x44);
                o->v0 = rf32(d + 0x48);
                o->u1 = rf32(d + 0x4C);
                o->v1 = rf32(d + 0x50);
            }
        }
        p += 4 + sz;
    }
    g_nobj++;
    return 1;
}

static int load_named(const char *name, unsigned char **data, int *size) {
    return assets_copy(name, data, size) && *data && *size >= 16;
}

static const char *kPkgs[] = {
    "fng/EA_TRAX.fng",
    "fng/PressStart.fng",
    "fng/PC_Loading.fng",
    "fng/FadeScreen.fng",
    "fng/DiscErrorPC.fng",
    "fng/Loading.fng",
    NULL
};
static int g_pkg_i;

int port_feng_load(void) {
    const unsigned char *pkhd;
    const unsigned char *objl;
    const unsigned char *p;
    const unsigned char *end;
    unsigned char *data = NULL;
    int size = 0;
    int i;
    g_nobj = 0;
    g_nres = 0;
    memset(g_res_name, 0, sizeof(g_res_name));
    memset(g_res_tex, 0, sizeof(g_res_tex));
    g_name[0] = 0;
    if (g_pkg) {
        free(g_pkg);
        g_pkg = NULL;
        g_pkg_sz = 0;
    }
    if (!kPkgs[g_pkg_i] || !load_named(kPkgs[g_pkg_i], &data, &size)) {
        for (i = 0; kPkgs[i]; i++) {
            if (load_named(kPkgs[i], &data, &size)) {
                g_pkg_i = i;
                break;
            }
            data = NULL;
            size = 0;
        }
    }
    if (!data) {
        snprintf(g_status, sizeof(g_status), "feng: no package");
        return 0;
    }
    if (ru32(data) != CH_FENG) {
        snprintf(g_status, sizeof(g_status), "feng: bad root %08x", ru32(data));
        free(data);
        return 0;
    }
    pkhd = find_child(data, CH_PKHD);
    if (!pkhd || ru32(pkhd + 4) < 24) {
        snprintf(g_status, sizeof(g_status), "feng: no PkHd");
        free(data);
        return 0;
    }
    {
        const unsigned char *hd = pkhd + 8;
        unsigned ver = ru32(hd);
        unsigned nlen = ru32(hd + 16);
        if (ver < 0x20000) {
            snprintf(g_status, sizeof(g_status), "feng: ver %u", ver);
            free(data);
            return 0;
        }
        if (nlen > 0 && nlen < sizeof(g_name) && 24 + nlen <= ru32(pkhd + 4)) {
            memcpy(g_name, hd + 24, nlen);
            g_name[sizeof(g_name) - 1] = 0;
        }
    }
    {
        const unsigned char *resl = find_child(data, CH_RESL);
        const unsigned char *rsnm;
        const unsigned char *rsrq;
        if (resl) {
            rsnm = find_child(resl, CH_RSNM);
            rsrq = find_child(resl, CH_RSRQ);
            if (rsnm && rsrq && ru32(rsrq + 4) >= 4) {
                const unsigned char *names = rsnm + 8;
                unsigned namesz = ru32(rsnm + 4);
                const unsigned char *rp = rsrq + 8;
                unsigned nreq = ru32(rp);
                unsigned ri;
                rp += 4;
                for (ri = 0; ri < nreq && g_nres < PORT_FENG_MAX_RES; ri++) {
                    unsigned off;
                    if (rp + 24 > rsrq + 8 + ru32(rsrq + 4)) break;
                    off = ru32(rp + 4);
                    if (off < namesz) {
                        unsigned n = 0;
                        while (n < sizeof(g_res_name[0]) - 1 && off + n < namesz && names[off + n]) {
                            g_res_name[g_nres][n] = (char)names[off + n];
                            n++;
                        }
                        g_res_name[g_nres][n] = 0;
                    }
                    g_nres++;
                    rp += 24;
                }
            }
        }
    }
    objl = find_child(data, CH_OBJL);
    if (!objl) {
        snprintf(g_status, sizeof(g_status), "feng: no ObjL");
        free(data);
        return 0;
    }
    p = objl + 8;
    end = objl + 8 + ru32(objl + 4);
    while (p + 8 <= end) {
        unsigned id = ru32(p);
        unsigned sz = ru32(p + 4);
        if (p + 8 + sz > end) break;
        if (id == CH_FOBJ) parse_object(p);
        p += 8 + sz;
    }
    g_pkg = data;
    g_pkg_sz = size;
    snprintf(g_status, sizeof(g_status), "feng: %s %d obj %dB", g_name[0] ? g_name : "?", g_nobj, size);
    return g_nobj > 0;
}

void port_feng_bind_logo(unsigned int tex) {
    g_logo_tex = tex;
}

static unsigned int load_tga_tex(const char *path) {
    unsigned char *raw = NULL;
    unsigned char *rgba = NULL;
    int sz = 0, w = 0, h = 0;
    unsigned int tex = 0;
    if (!assets_copy(path, &raw, &sz) || !raw) return 0;
    if (assets_decode_tga(raw, sz, &rgba, &w, &h) && rgba) {
        tex = gx2d_upload_rgba(rgba, w, h);
        free(rgba);
    }
    free(raw);
    return tex;
}

static void res_stem(const char *name, char *out, int outsz) {
    const char *s = name;
    const char *e;
    const char *slash;
    int n;
    for (slash = name; *slash; slash++) {
        if (*slash == '/' || *slash == '\\') s = slash + 1;
    }
    e = s;
    while (*e && *e != '.') e++;
    n = (int)(e - s);
    if (n >= outsz) n = outsz - 1;
    memcpy(out, s, (size_t)n);
    out[n] = 0;
}

void port_feng_upload_placeholders(void) {
    unsigned char px[32 * 32 * 4];
    int x, y, i;
    for (y = 0; y < 32; y++) {
        for (x = 0; x < 32; x++) {
            float dx = (float)x - 15.5f;
            float dy = (float)y - 15.5f;
            float d = sqrtf(dx * dx + dy * dy);
            int a = 0;
            if (d <= 13.0f) a = 255;
            else if (d < 16.0f) a = (int)((16.0f - d) * (255.0f / 3.0f));
            i = (y * 32 + x) * 4;
            px[i + 0] = 255;
            px[i + 1] = 255;
            px[i + 2] = 255;
            px[i + 3] = (unsigned char)a;
        }
    }
    g_circle_tex = gx2d_upload_rgba(px, 32, 32);
    for (i = 0; i < g_nres; i++) {
        char stem[40];
        char path[80];
        unsigned int tex = 0;
        res_stem(g_res_name[i], stem, sizeof(stem));
        if (stem[0]) {
            snprintf(path, sizeof(path), "tpk/%s.tga", stem);
            tex = load_tga_tex(path);
        }
        if (!tex && (strstr(g_res_name[i], "ircle") || strstr(g_res_name[i], "Circle") || strstr(g_res_name[i], "IRCLE"))) {
            tex = load_tga_tex("tpk/U2_loading_Circle.tga");
            if (!tex) tex = g_circle_tex;
        }
        g_res_tex[i] = tex;
    }
}

static PortFEObj *find_guid(unsigned guid) {
    int i;
    if (!guid) return NULL;
    for (i = 0; i < g_nobj; i++) {
        if (g_objs[i].guid == guid) return &g_objs[i];
    }
    return NULL;
}

static void world_xf(const PortFEObj *o, float *x, float *y, float *sx, float *sy) {
    const PortFEObj *p;
    int guard = 0;
    *x = o->px;
    *y = o->py;
    *sx = o->sx;
    *sy = o->sy;
    p = find_guid(o->parent);
    while (p && guard++ < 8) {
        *x = p->px + *x * p->sx;
        *y = p->py + *y * p->sy;
        *sx *= p->sx;
        *sy *= p->sy;
        p = find_guid(p->parent);
    }
}

static void draw_one(PortFEObj *o, int strings_only) {
    ePoly poly;
    unsigned colors[4];
    unsigned argb;
    float cx, cy, hw, hh, wx, wy, wsx, wsy;
    unsigned tex;
    int a, r, g, b;
    if (strings_only) {
        if (o->type != FE_STRING) return;
    } else if (o->type != FE_IMAGE && o->type != FE_COLOREDIMAGE) {
        return;
    }
    if (o->flags & 1) return;
    if (o->flags & 8) return;
    a = o->col_a;
    r = o->col_r;
    g = o->col_g;
    b = o->col_b;
    if (a <= 0) a = 220;
    a = clamp8(a);
    r = clamp8(r);
    g = clamp8(g);
    b = clamp8(b);
    argb = ((unsigned)a << 24) | ((unsigned)r << 16) | ((unsigned)g << 8) | (unsigned)b;
    colors[0] = colors[1] = colors[2] = colors[3] = argb;
    world_xf(o, &wx, &wy, &wsx, &wsy);
    cx = wx + 320.0f;
    cy = wy + 240.0f;
    hw = wsx * 0.5f;
    hh = wsy * 0.5f;
    if (hw < 0.0f) hw = -hw;
    if (hh < 0.0f) hh = -hh;
    if (o->type == FE_STRING) {
        if (o->text[0]) {
            float ps = wsx * 3.0f;
            if (ps < 2.0f) ps = 2.0f;
            port_pixfont_draw(cx, cy, ps, (float)r / 255.0f, (float)g / 255.0f, (float)b / 255.0f, o->text);
        }
        return;
    }
    tex = 0;
    if (o->res < (unsigned)g_nres) tex = g_res_tex[o->res];
    epoly_init(&poly);
    epoly_add(&poly, cx - hw, cy - hh, cx + hw, cy + hh, 1.0f, o->u0, o->v0, o->u1, o->v1, colors);
    gx2d_epoly(&poly, tex);
}

void port_feng_draw(void) {
    int i;
    for (i = 0; i < g_nobj; i++) draw_one(&g_objs[i], 0);
    for (i = 0; i < g_nobj; i++) draw_one(&g_objs[i], 1);
}

int port_feng_cycle(void) {
    int n = 0;
    while (kPkgs[n]) n++;
    if (n < 1) return 0;
    g_pkg_i = (g_pkg_i + 1) % n;
    return port_feng_load();
}

const char *port_feng_name(void) {
    return g_name[0] ? g_name : (kPkgs[g_pkg_i] ? kPkgs[g_pkg_i] : "?");
}

const char *port_feng_status(void) {
    return g_status;
}
