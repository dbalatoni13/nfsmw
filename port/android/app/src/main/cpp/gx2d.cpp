#include "gx2d.h"

#include <GLES2/gl2.h>
#include <string.h>

#define GX2D_MAX_QUADS 1024
#define GX2D_VW 640.0f
#define GX2D_VH 480.0f

static const char *kVert =
    "attribute vec2 aPos;\n"
    "attribute vec4 aCol;\n"
    "attribute vec2 aUv;\n"
    "varying vec4 vCol;\n"
    "varying vec2 vUv;\n"
    "void main(){\n"
    "  vCol = aCol;\n"
    "  vUv = aUv;\n"
    "  gl_Position = vec4(aPos, 0.0, 1.0);\n"
    "}\n";
static const char *kFrag =
    "precision mediump float;\n"
    "varying vec4 vCol;\n"
    "varying vec2 vUv;\n"
    "uniform sampler2D uTex;\n"
    "void main(){\n"
    "  vec4 t = texture2D(uTex, vUv);\n"
    "  gl_FragColor = vec4(vCol.rgb * t.rgb, vCol.a * t.a);\n"
    "}\n";

static GLuint g_prog;
static GLint g_aPos;
static GLint g_aCol;
static GLint g_aUv;
static GLint g_uTex;
static GLint g_uUseTex;
static GLuint g_white;
static int g_fb_w = 640;
static int g_fb_h = 480;
static int g_nverts;
static float g_pos[GX2D_MAX_QUADS * 6 * 2];
static float g_col[GX2D_MAX_QUADS * 6 * 4];
static float g_uv[GX2D_MAX_QUADS * 6 * 2];
static GLuint g_texid[GX2D_MAX_QUADS * 6];

static GLuint compile(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        glDeleteShader(s);
        return 0;
    }
    return s;
}

int gx2d_init(void) {
    GLuint vs = compile(GL_VERTEX_SHADER, kVert);
    GLuint fs = compile(GL_FRAGMENT_SHADER, kFrag);
    if (!vs || !fs) return 0;
    g_prog = glCreateProgram();
    glAttachShader(g_prog, vs);
    glAttachShader(g_prog, fs);
    glBindAttribLocation(g_prog, 0, "aPos");
    glBindAttribLocation(g_prog, 1, "aCol");
    glBindAttribLocation(g_prog, 2, "aUv");
    glLinkProgram(g_prog);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = 0;
    glGetProgramiv(g_prog, GL_LINK_STATUS, &ok);
    if (!ok) {
        glDeleteProgram(g_prog);
        g_prog = 0;
        return 0;
    }
    g_aPos = 0;
    g_aCol = 1;
    g_aUv = 2;
    g_uTex = glGetUniformLocation(g_prog, "uTex");
    g_uUseTex = -1;
    g_white = gx2d_make_solid_tex(255, 255, 255, 255);
    g_nverts = 0;
    return g_prog && g_white ? 1 : 0;
}

void gx2d_shutdown(void) {
    if (g_white) {
        glDeleteTextures(1, &g_white);
        g_white = 0;
    }
    if (g_prog) {
        glDeleteProgram(g_prog);
        g_prog = 0;
    }
    g_nverts = 0;
}

void gx2d_resize(int fb_w, int fb_h) {
    if (fb_w > 0) g_fb_w = fb_w;
    if (fb_h > 0) g_fb_h = fb_h;
}

void gx2d_begin(void) {
    g_nverts = 0;
}

static void push_vert(float x, float y, float u, float v, float r, float g, float b, float a, GLuint tex) {
    if (g_nverts >= GX2D_MAX_QUADS * 6) return;
    float ndc_x = (x / GX2D_VW) * 2.0f - 1.0f;
    float ndc_y = 1.0f - (y / GX2D_VH) * 2.0f;
    int i = g_nverts;
    g_pos[i * 2 + 0] = ndc_x;
    g_pos[i * 2 + 1] = ndc_y;
    g_uv[i * 2 + 0] = u;
    g_uv[i * 2 + 1] = v;
    g_col[i * 4 + 0] = r;
    g_col[i * 4 + 1] = g;
    g_col[i * 4 + 2] = b;
    g_col[i * 4 + 3] = a;
    g_texid[i] = tex ? tex : g_white;
    g_nverts++;
}

void gx2d_quad(float x0, float y0, float x1, float y1, float r, float g, float b, float a) {
    gx2d_quad_tex(x0, y0, x1, y1, 0, r, g, b, a);
}

static unsigned int upload_rgba(const unsigned char *rgba, int w, int h, int linear) {
    GLuint tex = 0;
    GLint filt;
    if (!rgba || w <= 0 || h <= 0) return 0;
    filt = linear ? GL_LINEAR : GL_NEAREST;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filt);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, rgba);
    return tex;
}

unsigned int gx2d_upload_rgba(const unsigned char *rgba, int w, int h) {
    return upload_rgba(rgba, w, h, 0);
}

unsigned int gx2d_upload_rgba_linear(const unsigned char *rgba, int w, int h) {
    return upload_rgba(rgba, w, h, 1);
}

unsigned int gx2d_make_solid_tex(unsigned char r, unsigned char g, unsigned char b, unsigned char a) {
    unsigned char px[4] = { r, g, b, a };
    GLuint tex = 0;
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, px);
    return tex;
}

static void unpack_fe_color(unsigned int c, float *r, float *g, float *b, float *a) {
    *a = ((c >> 24) & 0xff) / 255.0f;
    *r = ((c >> 16) & 0xff) / 255.0f;
    *g = ((c >> 8) & 0xff) / 255.0f;
    *b = (c & 0xff) / 255.0f;
    if (*a <= 0.0f) *a = 1.0f;
}

void gx2d_fe_poly(float x0, float y0, float x1, float y1, float s0, float t0, float s1, float t1,
                  const unsigned int colors[4], unsigned int tex) {
    float r[4], g[4], b[4], a[4];
    int i;
    for (i = 0; i < 4; i++) unpack_fe_color(colors[i], &r[i], &g[i], &b[i], &a[i]);
    GLuint t = tex ? (GLuint)tex : g_white;
    push_vert(x0, y0, s0, t0, r[0], g[0], b[0], a[0], t);
    push_vert(x1, y0, s1, t0, r[1], g[1], b[1], a[1], t);
    push_vert(x1, y1, s1, t1, r[2], g[2], b[2], a[2], t);
    push_vert(x0, y0, s0, t0, r[0], g[0], b[0], a[0], t);
    push_vert(x1, y1, s1, t1, r[2], g[2], b[2], a[2], t);
    push_vert(x0, y1, s0, t1, r[3], g[3], b[3], a[3], t);
}

void gx2d_quad_tex(float x0, float y0, float x1, float y1, unsigned int tex, float r, float g, float b, float a) {
    GLuint t = tex ? (GLuint)tex : g_white;
    push_vert(x0, y0, 0.0f, 0.0f, r, g, b, a, t);
    push_vert(x1, y0, 1.0f, 0.0f, r, g, b, a, t);
    push_vert(x1, y1, 1.0f, 1.0f, r, g, b, a, t);
    push_vert(x0, y0, 0.0f, 0.0f, r, g, b, a, t);
    push_vert(x1, y1, 1.0f, 1.0f, r, g, b, a, t);
    push_vert(x0, y1, 0.0f, 1.0f, r, g, b, a, t);
}

void gx2d_flush(void) {
    if (!g_prog || g_nverts <= 0) return;
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glUseProgram(g_prog);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);
    glActiveTexture(GL_TEXTURE0);
    glUniform1i(g_uTex, 0);
    glEnableVertexAttribArray((GLuint)g_aPos);
    glEnableVertexAttribArray((GLuint)g_aCol);
    if (g_aUv >= 0) glEnableVertexAttribArray((GLuint)g_aUv);
    glVertexAttribPointer((GLuint)g_aPos, 2, GL_FLOAT, GL_FALSE, 0, g_pos);
    glVertexAttribPointer((GLuint)g_aCol, 4, GL_FLOAT, GL_FALSE, 0, g_col);
    if (g_aUv >= 0) glVertexAttribPointer((GLuint)g_aUv, 2, GL_FLOAT, GL_FALSE, 0, g_uv);
    int i = 0;
    while (i < g_nverts) {
        GLuint tex = g_texid[i];
        int j = i + 1;
        while (j < g_nverts && g_texid[j] == tex) j++;
        glBindTexture(GL_TEXTURE_2D, tex);
        glDrawArrays(GL_TRIANGLES, i, j - i);
        i = j;
    }
    glDisableVertexAttribArray((GLuint)g_aPos);
    glDisableVertexAttribArray((GLuint)g_aCol);
    if (g_aUv >= 0) glDisableVertexAttribArray((GLuint)g_aUv);
    g_nverts = 0;
}
