#ifndef PORT_ASSETS_H
#define PORT_ASSETS_H

#include <jni.h>

#ifdef __cplusplus
extern "C" {
#endif

void assets_bind(JNIEnv *env, jobject asset_manager);
const char *assets_status(void);
int assets_copy(const char *name, unsigned char **out_ptr, int *out_size);
int assets_decode_tga(const unsigned char *data, int size, unsigned char **out_rgba, int *out_w, int *out_h);

#ifdef __cplusplus
}
#endif

#endif
