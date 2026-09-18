#include "port_lz.h"

#include <stdlib.h>
#include <string.h>

int port_jdlz(const unsigned char *src, int src_sz, unsigned char **out_ptr, int *out_sz) {
    unsigned uncomp, comp;
    unsigned char *out;
    int size, di, si;
    unsigned control, runtype;
    if (!src || src_sz < 0x12 || !out_ptr || !out_sz) return 0;
    if (src[0] != 'J' || src[1] != 'D' || src[2] != 'L' || src[3] != 'Z') return 0;
    if (src[4] != 2) return 0;
    memcpy(&uncomp, src + 8, 4);
    memcpy(&comp, src + 12, 4);
    if (uncomp == 0 || uncomp > 8 * 1024 * 1024) return 0;
    out = (unsigned char *)malloc(uncomp);
    if (!out) return 0;
    size = (int)comp - 0x12;
    control = src[0x10] | 0x100;
    runtype = src[0x11] | 0x100;
    si = 0x12;
    di = 0;
    while (size != 0 && si < src_sz) {
        if (control & 1) {
            int run, offset;
            if (si + 1 >= src_sz) break;
            if (runtype & 1) {
                run = ((src[si] >> 4) << 8 | src[si + 1]) + 3;
                offset = (src[si] & 0xF) + 1;
            } else {
                offset = ((src[si] >> 5) << 8 | src[si + 1]) + 0x11;
                run = (src[si] & 0x1F) + 3;
            }
            si += 2;
            size -= 2;
            runtype >>= 1;
            while (run-- > 0 && di < (int)uncomp) {
                if (di - offset < 0) break;
                out[di] = out[di - offset];
                di++;
            }
        } else {
            if (di < (int)uncomp && si < src_sz) out[di++] = src[si++];
            else si++;
            size--;
        }
        control >>= 1;
        if (control == 1) {
            size--;
            if (si >= src_sz) break;
            control = src[si++] | 0x100;
        }
        if (runtype == 1) {
            size--;
            if (si >= src_sz) break;
            runtype = src[si++] | 0x100;
        }
    }
    *out_ptr = out;
    *out_sz = (int)uncomp;
    return 1;
}
