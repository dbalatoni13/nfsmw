#include "Speed/Indep/Libs/snd/9/source/library/cmn/sndcmn.h"

namespace Snd {
    void *Util::MemCpy(void *pDst, const void *pSrc, unsigned int bytes) {
        int si = (int)pSrc;
        int di = (int)pDst;
        int difference = si - di;
        if (bytes == 0 || difference == 0) {
            return NULL;
        }

        if ((difference & 1) != 0) {
            char *sb = (char *)pSrc;
            char *db = (char *)pDst;
            char t0, t1, t2, t3;

            while (bytes > 3) {
                bytes -= 4;

                t0 = sb[0];
                t1 = sb[1];
                t2 = sb[2];
                t3 = sb[3];

                sb += 4;
                db[0] = t0;
                db[1] = t1;
                db[2] = t2;
                db[3] = t3;
                db += 4;
            }

            while (bytes != 0) {
                *db = *sb;
                bytes--;
                sb++;
                db++;
            }
        } else if ((difference & 2) != 0) {
            short *ss = (short *)si;
            short *ds = (short *)di;
            short t0, t1, t2, t3;

            if ((si & 1) != 0) {
                ds = (short *)(di + 1);
                ss = (short *)(si + 1);
                bytes--;
                *(char *)pDst = *(char *)pSrc;
            }

            while (bytes > 7) {
                bytes -= 8;

                t0 = ss[0];
                t1 = ss[1];
                t2 = ss[2];
                t3 = ss[3];
                ss += 4;

                ds[0] = t0;
                ds[1] = t1;
                ds[2] = t2;
                ds[3] = t3;
                ds += 4;
            }

            while (bytes > 1) {
                *ds = *ss;
                bytes -= 2;
                ss++;
                ds++;
            }

            if (bytes != 0) {
                *(char *)ds = *(char *)ss;
            }
        } else {
            int t0, t1, t2, t3;

            if ((si & 1) != 0) {
                *(char *)di = *(char *)si;
                si++;
                di++;
                bytes--;
            }

            if ((si & 2) != 0 && bytes > 1) {
                *(short *)di = *(short *)si;
                si += 2;
                di += 2;
                bytes -= 2;
            }

            int *sw = (int *)si;
            int *dw = (int *)di;

            while (bytes > 15) {
                bytes -= 16;

                t0 = sw[0];
                t1 = sw[1];
                t2 = sw[2];
                t3 = sw[3];
                sw += 4;

                dw[0] = t0;
                dw[1] = t1;
                dw[2] = t2;
                dw[3] = t3;
                dw += 4;
            }

            while (bytes > 3) {
                *dw = *sw;
                bytes -= 4;
                sw++;
                dw++;
            }

            if (bytes > 1) {
                *(short *)dw = *(short *)sw;
                sw = (int *)((int)sw + 2);
                dw = (int *)((int)dw + 2);
                bytes -= 2;
            }

            if (bytes != 0) {
                *(char *)dw = *(char *)sw;
            }
        }

        return NULL;
    }
}
