#include "../../../include/common/realcore/std.h"

void MEM_copy(void *dest, const void *source, int count) {
    int si;
    int di;
    int difference;

    si = reinterpret_cast<int>(source);
    di = reinterpret_cast<int>(dest);
    difference = reinterpret_cast<int>(source) - reinterpret_cast<int>(dest);
    if (count == 0) {
        return;
    }
    if (difference == 0) {
        return;
    }

    if ((difference & 1) != 0) {
        {
            char *sb = reinterpret_cast<char *>(const_cast<void *>(source));
            char *db = reinterpret_cast<char *>(dest);
            char t0;
            char t1;
            char t2;
            char t3;

            for (; count > 3; count -= 4) {
                t0 = sb[0];
                t1 = sb[1];
                t2 = sb[2];
                t3 = sb[3];
                db[0] = t0;
                sb += 4;
                db[1] = t1;
                db[2] = t2;
                db[3] = t3;
                db += 4;
            }
            if (count > 0) {
                for (; count > 0; count--) {
                    t0 = sb[0];
                    sb++;
                    db[0] = t0;
                    db++;
                }
            }
            return;
        }
    }

    if ((difference & 2) != 0) {
        {
            short *ss;
            short *ds;
            short t0;
            short t1;
            short t2;
            short t3;

            ss = reinterpret_cast<short *>(const_cast<void *>(source));
            ds = reinterpret_cast<short *>(dest);

            if ((reinterpret_cast<int>(ss) & 1) != 0 && count > 0) {
                *reinterpret_cast<unsigned char *>(ds) = *reinterpret_cast<unsigned char *>(ss);
                ds = reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(ds) + 1);
                ss = reinterpret_cast<short *>(reinterpret_cast<unsigned char *>(ss) + 1);
                count--;
            }
            for (; count > 7; count -= 8) {
                t0 = ss[0];
                t1 = ss[1];
                t2 = ss[2];
                t3 = ss[3];
                ds[0] = t0;
                ss += 4;
                ds[1] = t1;
                ds[2] = t2;
                ds[3] = t3;
                ds += 4;
            }
            for (; count > 1; count -= 2) {
                *ds = *ss;
                ss++;
                ds++;
            }
            if (count > 0) {
                *reinterpret_cast<unsigned char *>(ds) = *reinterpret_cast<unsigned char *>(ss);
            }
            return;
        }
    }

    {
        {
            int *sw;
            int *dw;
            int t0;
            int t1;
            int t2;
            int t3;

            dw = reinterpret_cast<int *>(dest);
            sw = reinterpret_cast<int *>(const_cast<void *>(source));

            if ((reinterpret_cast<int>(sw) & 1) != 0 && count > 0) {
                *reinterpret_cast<unsigned char *>(dw) = *reinterpret_cast<unsigned char *>(sw);
                sw = reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(sw) + 1);
                dw = reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(dw) + 1);
                count--;
            }
            if ((reinterpret_cast<int>(sw) & 2) != 0 && count > 1) {
                *reinterpret_cast<unsigned short *>(dw) = *reinterpret_cast<unsigned short *>(sw);
                sw = reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(sw) + 2);
                dw = reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(dw) + 2);
                count -= 2;
            }
            for (; count > 15; count -= 16) {
                t0 = sw[0];
                t1 = sw[1];
                t2 = sw[2];
                t3 = sw[3];
                dw[0] = t0;
                sw += 4;
                dw[1] = t1;
                dw[2] = t2;
                dw[3] = t3;
                dw += 4;
            }
            for (; count > 3; count -= 4) {
                *dw = *sw;
                sw++;
                dw++;
            }
            if (count > 1) {
                *reinterpret_cast<unsigned short *>(dw) = *reinterpret_cast<unsigned short *>(sw);
                count -= 2;
                sw = reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(sw) + 2);
                dw = reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(dw) + 2);
            }
            if (count > 0) {
                *reinterpret_cast<unsigned char *>(dw) = *reinterpret_cast<unsigned char *>(sw);
            }
        }
    }
}
