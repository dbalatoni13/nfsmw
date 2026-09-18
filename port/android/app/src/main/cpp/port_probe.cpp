// Sonda de portabilidad Fase 0/1: ejercita codigo portable del decomp en Android.
#include "UMath.h"
#include "assets.h"
#include "port_aaudio.h"
#include "port_feng.h"
#include "port_font.h"
#include "port_boot.h"

#include <jni.h>
#include <cstdio>

extern "C" JNIEXPORT jstring JNICALL
Java_dev_nfsmw_port_MainActivity_probe(JNIEnv *env, jobject) {
    UMath::Vector3 a; a.x = 3.0f; a.y = 4.0f; a.z = 0.0f;
    UMath::Vector3 b; b.x = 0.0f; b.y = 0.0f; b.z = 1.0f;
    UMath::Vector3 c;
    UMath::Cross(a, b, c);
    float len = UMath::Length(a);
    char buf[256];
    snprintf(buf, sizeof(buf), "UMath OK: |(3,4,0)|=%.1f  |  %s  |  %s",
             len, port_boot_status(), port_feng_status());
    return env->NewStringUTF(buf);
}

extern "C" JNIEXPORT void JNICALL
Java_dev_nfsmw_port_MainActivity_nativeSetAssets(JNIEnv *env, jobject, jobject mgr) {
    assets_bind(env, mgr);
    port_audio_start();
    port_feng_load();
    port_boot_scan();
}
