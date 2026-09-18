#include "gx2d.h"
#include "assets.h"
#include "port_aaudio.h"
#include "port_epoly.h"
#include "port_feng.h"
#include "port_font.h"
#include "port_pixfont.h"
#include <stdlib.h>

#include <jni.h>
#include <android/native_window.h>
#include <android/native_window_jni.h>
#include <android/log.h>
#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <pthread.h>
#include <unistd.h>
#include <time.h>
#include <math.h>
#include <string.h>

#define LOGI(...) __android_log_print(ANDROID_LOG_INFO, "nfsmw", __VA_ARGS__)
#define LOGE(...) __android_log_print(ANDROID_LOG_ERROR, "nfsmw", __VA_ARGS__)

static pthread_mutex_t g_lock = PTHREAD_MUTEX_INITIALIZER;
static pthread_t g_thread;
static ANativeWindow *g_window = NULL;
static volatile int g_running = 0;
static volatile int g_paused = 0;
static volatile int g_has_thread = 0;
static volatile int g_width = 1;
static volatile int g_height = 1;
static volatile int g_tap = 0;

static const char *kVert =
    "attribute vec4 aPos;\n"
    "void main(){ gl_Position = aPos; }\n";
static const char *kFrag =
    "precision mediump float;\n"
    "uniform vec4 uColor;\n"
    "void main(){ gl_FragColor = uColor; }\n";

static GLuint compile_shader(GLenum type, const char *src) {
    GLuint s = glCreateShader(type);
    glShaderSource(s, 1, &src, NULL);
    glCompileShader(s);
    GLint ok = 0;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok) {
        char log[256];
        glGetShaderInfoLog(s, sizeof(log), NULL, log);
        LOGE("shader: %s", log);
        glDeleteShader(s);
        return 0;
    }
    return s;
}

static GLuint make_program() {
    GLuint vs = compile_shader(GL_VERTEX_SHADER, kVert);
    GLuint fs = compile_shader(GL_FRAGMENT_SHADER, kFrag);
    if (!vs || !fs) return 0;
    GLuint p = glCreateProgram();
    glAttachShader(p, vs);
    glAttachShader(p, fs);
    glBindAttribLocation(p, 0, "aPos");
    glLinkProgram(p);
    glDeleteShader(vs);
    glDeleteShader(fs);
    GLint ok = 0;
    glGetProgramiv(p, GL_LINK_STATUS, &ok);
    if (!ok) {
        LOGE("link failed");
        glDeleteProgram(p);
        return 0;
    }
    return p;
}

static int egl_init(ANativeWindow *win, EGLDisplay *out_dpy, EGLSurface *out_surf,
                    EGLContext *out_ctx) {
    EGLDisplay dpy = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    if (dpy == EGL_NO_DISPLAY) return 0;
    if (!eglInitialize(dpy, NULL, NULL)) return 0;
    const EGLint cfg_attr[] = {
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_RED_SIZE, 8,
        EGL_GREEN_SIZE, 8,
        EGL_BLUE_SIZE, 8,
        EGL_NONE
    };
    EGLConfig cfg;
    EGLint n = 0;
    if (!eglChooseConfig(dpy, cfg_attr, &cfg, 1, &n) || n < 1) return 0;
    EGLint fmt = 0;
    eglGetConfigAttrib(dpy, cfg, EGL_NATIVE_VISUAL_ID, &fmt);
    ANativeWindow_setBuffersGeometry(win, 0, 0, fmt);
    EGLSurface surf = eglCreateWindowSurface(dpy, cfg, win, NULL);
    if (surf == EGL_NO_SURFACE) return 0;
    const EGLint ctx_attr[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };
    EGLContext ctx = eglCreateContext(dpy, cfg, EGL_NO_CONTEXT, ctx_attr);
    if (ctx == EGL_NO_CONTEXT) {
        eglDestroySurface(dpy, surf);
        return 0;
    }
    if (!eglMakeCurrent(dpy, surf, surf, ctx)) {
        eglDestroyContext(dpy, ctx);
        eglDestroySurface(dpy, surf);
        return 0;
    }
    eglSwapInterval(dpy, 1);
    *out_dpy = dpy;
    *out_surf = surf;
    *out_ctx = ctx;
    return 1;
}

static void egl_shutdown(EGLDisplay dpy, EGLSurface surf, EGLContext ctx) {
    if (dpy != EGL_NO_DISPLAY) {
        eglMakeCurrent(dpy, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (ctx != EGL_NO_CONTEXT) eglDestroyContext(dpy, ctx);
        if (surf != EGL_NO_SURFACE) eglDestroySurface(dpy, surf);
        eglTerminate(dpy);
    }
}

static void *render_thread(void *) {
    pthread_mutex_lock(&g_lock);
    ANativeWindow *win = g_window;
    pthread_mutex_unlock(&g_lock);
    if (!win) {
        LOGI("gl: no window");
        return NULL;
    }

    EGLDisplay dpy = EGL_NO_DISPLAY;
    EGLSurface surf = EGL_NO_SURFACE;
    EGLContext ctx = EGL_NO_CONTEXT;
    if (!egl_init(win, &dpy, &surf, &ctx)) {
        LOGE("egl init failed err=0x%x", eglGetError());
        return NULL;
    }

    GLuint prog = make_program();
    GLint uColor = prog ? glGetUniformLocation(prog, "uColor") : -1;
    static const float verts[] = {
        0.0f,  0.55f,
       -0.55f, -0.5f,
        0.55f, -0.5f
    };
    GLuint vbo = 0;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    if (!gx2d_init()) {
        LOGE("gx2d_init failed");
    }
    unsigned char checker[8 * 8 * 4];
    for (int y = 0; y < 8; y++) {
        for (int x = 0; x < 8; x++) {
            int on = ((x >> 1) ^ (y >> 1)) & 1;
            int i = (y * 8 + x) * 4;
            checker[i + 0] = on ? 255 : 40;
            checker[i + 1] = on ? 90 : 40;
            checker[i + 2] = on ? 10 : 40;
            checker[i + 3] = 255;
        }
    }
    GLuint check_tex = 0;
    glGenTextures(1, &check_tex);
    glBindTexture(GL_TEXTURE_2D, check_tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 8, 8, 0, GL_RGBA, GL_UNSIGNED_BYTE, checker);

    GLuint logo_tex = 0;
    int logo_w = 0, logo_h = 0;
    unsigned char *tga = NULL;
    int tga_sz = 0;
    if (assets_copy("logo.tga", &tga, &tga_sz)) {
        unsigned char *rgba = NULL;
        if (assets_decode_tga(tga, tga_sz, &rgba, &logo_w, &logo_h)) {
            logo_tex = gx2d_upload_rgba(rgba, logo_w, logo_h);
            LOGI("gl: logo.tga %dx%d tex=%u", logo_w, logo_h, (unsigned)logo_tex);
            free(rgba);
        } else {
            LOGE("gl: logo.tga decode failed size=%d", tga_sz);
        }
        free(tga);
    } else {
        LOGE("gl: logo.tga missing");
    }
    port_feng_bind_logo(logo_tex);
    port_feng_upload_placeholders();
    port_font_load();

    LOGI("gl: loop start");
    while (1) {
        pthread_mutex_lock(&g_lock);
        int running = g_running;
        int paused = g_paused;
        int w = g_width;
        int h = g_height;
        pthread_mutex_unlock(&g_lock);
        if (!running) break;
        if (paused) {
            usleep(16000);
            continue;
        }

        EGLint ew = w, eh = h;
        eglQuerySurface(dpy, surf, EGL_WIDTH, &ew);
        eglQuerySurface(dpy, surf, EGL_HEIGHT, &eh);
        if (ew < 1) ew = 1;
        if (eh < 1) eh = 1;
        glViewport(0, 0, ew, eh);

        struct timespec ts;
        clock_gettime(CLOCK_MONOTONIC, &ts);
        float t = (float)ts.tv_sec + (float)ts.tv_nsec * 1e-9f;
        float pulse = 0.5f + 0.5f * sinf(t * 2.2f);
        glClearColor(0.06f + 0.28f * pulse, 0.03f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        if (prog) {
            glUseProgram(prog);
            glUniform4f(uColor, 1.0f, 0.45f + 0.4f * pulse, 0.05f, 1.0f);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const void *)0);
            glDrawArrays(GL_TRIANGLES, 0, 3);
            glDisableVertexAttribArray(0);
            glBindBuffer(GL_ARRAY_BUFFER, 0);
        }
        if (g_tap) {
            g_tap = 0;
            port_feng_cycle();
            port_feng_upload_placeholders();
        }
        gx2d_resize(ew, eh);
        gx2d_begin();
        port_feng_draw();
        gx2d_quad(8.0f, 452.0f, 400.0f, 476.0f, 0.0f, 0.0f, 0.0f, 0.55f);
        port_pixfont_draw(12.0f, 456.0f, 2.0f, 1.0f, 1.0f, 1.0f, port_feng_name());
        gx2d_flush();
        eglSwapBuffers(dpy, surf);
    }

    if (logo_tex) glDeleteTextures(1, &logo_tex);
    if (check_tex) glDeleteTextures(1, &check_tex);
    gx2d_shutdown();
    if (vbo) glDeleteBuffers(1, &vbo);
    if (prog) glDeleteProgram(prog);
    egl_shutdown(dpy, surf, ctx);
    LOGI("gl: loop end");
    return NULL;
}

extern "C" JNIEXPORT void JNICALL
Java_dev_nfsmw_port_MainActivity_nativeSetSurface(JNIEnv *env, jobject, jobject surface) {
    if (g_has_thread) {
        pthread_mutex_lock(&g_lock);
        g_running = 0;
        pthread_mutex_unlock(&g_lock);
        pthread_join(g_thread, NULL);
        g_has_thread = 0;
    }

    pthread_mutex_lock(&g_lock);
    if (g_window) {
        ANativeWindow_release(g_window);
        g_window = NULL;
    }
    if (surface) {
        g_window = ANativeWindow_fromSurface(env, surface);
        g_running = 1;
        pthread_mutex_unlock(&g_lock);
        if (pthread_create(&g_thread, NULL, render_thread, NULL) == 0) {
            g_has_thread = 1;
        } else {
            LOGE("pthread_create failed");
            pthread_mutex_lock(&g_lock);
            g_running = 0;
            if (g_window) {
                ANativeWindow_release(g_window);
                g_window = NULL;
            }
            pthread_mutex_unlock(&g_lock);
        }
        return;
    }
    pthread_mutex_unlock(&g_lock);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_nfsmw_port_MainActivity_nativeSetSize(JNIEnv *, jobject, jint w, jint h) {
    pthread_mutex_lock(&g_lock);
    if (w > 0) g_width = w;
    if (h > 0) g_height = h;
    pthread_mutex_unlock(&g_lock);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_nfsmw_port_MainActivity_nativeSetPaused(JNIEnv *, jobject, jboolean paused) {
    pthread_mutex_lock(&g_lock);
    g_paused = paused ? 1 : 0;
    pthread_mutex_unlock(&g_lock);
    port_audio_set_paused(paused ? 1 : 0);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_nfsmw_port_MainActivity_nativeOnTouch(JNIEnv *, jobject, jfloat x, jfloat y, jint action) {
    if (action == 0) g_tap = 1;
}
