#ifndef PORT_LZ_H
#define PORT_LZ_H

#ifdef __cplusplus
extern "C" {
#endif

int port_jdlz(const unsigned char *src, int src_sz, unsigned char **out_ptr, int *out_sz);

#ifdef __cplusplus
}
#endif

#endif
