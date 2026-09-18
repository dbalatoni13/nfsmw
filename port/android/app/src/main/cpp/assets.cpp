#include "assets.h"

#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static AAssetManager *g_mgr;
static char g_status[160] = "assets: not bound";

void assets_bind(JNIEnv *env, jobject asset_manager) {
    if (!env || !asset_manager) {
        g_mgr = NULL;
        snprintf(g_status, sizeof(g_status), "assets: null");
        return;
    }
    g_mgr = AAssetManager_fromJava(env, asset_manager);
    AAsset *a = g_mgr ? AAssetManager_open(g_mgr, "probe.txt", AASSET_MODE_BUFFER) : NULL;
    if (!a) {
        snprintf(g_status, sizeof(g_status), "assets: probe.txt missing");
        return;
    }
    off_t sz = AAsset_getLength(a);
    const void *p = AAsset_getBuffer(a);
    char head[48];
    int n = sz > 40 ? 40 : (int)sz;
    if (p && n > 0) {
        memcpy(head, p, (size_t)n);
        head[n] = 0;
        for (int i = 0; i < n; i++) {
            if (head[i] == '\n' || head[i] == '\r') {
                head[i] = 0;
                break;
            }
        }
    } else {
        head[0] = 0;
    }
    AAsset_close(a);
    snprintf(g_status, sizeof(g_status), "assets: probe.txt %ldB \"%s\" + logo.tga", (long)sz, head);
}

const char *assets_status(void) {
    return g_status;
}

int assets_copy(const char *name, unsigned char **out_ptr, int *out_size) {
    if (!g_mgr || !name || !out_ptr || !out_size) return 0;
    AAsset *a = AAssetManager_open(g_mgr, name, AASSET_MODE_BUFFER);
    if (!a) return 0;
    int sz = (int)AAsset_getLength(a);
    const void *src = AAsset_getBuffer(a);
    if (!src || sz <= 0) {
        AAsset_close(a);
        return 0;
    }
    unsigned char *dst = (unsigned char *)malloc((size_t)sz);
    if (!dst) {
        AAsset_close(a);
        return 0;
    }
    memcpy(dst, src, (size_t)sz);
    AAsset_close(a);
    *out_ptr = dst;
    *out_size = sz;
    return 1;
}

int assets_decode_tga(const unsigned char *data, int size, unsigned char **out_rgba, int *out_w, int *out_h) {
    if (!data || size < 18 || !out_rgba || !out_w || !out_h) return 0;
    if (data[2] != 2) return 0;
    int w = data[12] | (data[13] << 8);
    int h = data[14] | (data[15] << 8);
    int bpp = data[16];
    int top = (data[17] & 0x20) != 0;
    if (w <= 0 || h <= 0 || (bpp != 24 && bpp != 32)) return 0;
    int src_bpp = bpp / 8;
    int need = 18 + w * h * src_bpp;
    if (size < need) return 0;
    unsigned char *rgba = (unsigned char *)malloc((size_t)(w * h * 4));
    if (!rgba) return 0;
    const unsigned char *src = data + 18;
    for (int y = 0; y < h; y++) {
        int dy = top ? y : (h - 1 - y);
        for (int x = 0; x < w; x++) {
            const unsigned char *p = src + ((dy * w + x) * src_bpp);
            unsigned char *d = rgba + ((y * w + x) * 4);
            d[0] = p[2];
            d[1] = p[1];
            d[2] = p[0];
            d[3] = (src_bpp == 4) ? p[3] : 255;
        }
    }
    *out_rgba = rgba;
    *out_w = w;
    *out_h = h;
    return 1;
}
