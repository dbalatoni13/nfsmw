#ifndef _DOLPHIN_OSCACHE_SHIM_H_
#define _DOLPHIN_OSCACHE_SHIM_H_
// Shim del SDK: cache de datos/instrucciones. En arm64 coherente, no-ops.
inline void DCFlushRange(void *start, unsigned long len) { (void)start; (void)len; }
inline void DCInvalidateRange(void *start, unsigned long len) { (void)start; (void)len; }
inline void ICInvalidateRange(void *start, unsigned long len) { (void)start; (void)len; }
#endif
