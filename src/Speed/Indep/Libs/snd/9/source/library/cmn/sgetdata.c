#include "./sndcmn.h"

int SNDI_getb(void *pdata, int numbytes) {
    unsigned char *p = (unsigned char *)pdata;
    int val = 0;
    int i = numbytes;

    /* The original tests the old count before forming the loop remainder. */
    if (i-- != 0) {
        do {
            /* Four-byte fields wrap in the original 32-bit accumulator. */
            val = (int)((unsigned int)val * 256U + *p);
            p = p + 1;
        } while (i-- != 0);
    }

    if (numbytes == 1 && val > 0x7F) {
        val -= 0x100;
    } else if (numbytes == 2 && val > 0x7FFF) {
        val -= 0x10000;
    } else if (numbytes == 3 && val > 0x7FFFFF) {
        val -= 0x1000000;
    }

    return val;
}
