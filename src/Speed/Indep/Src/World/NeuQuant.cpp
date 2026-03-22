extern unsigned char *thepicture;
extern int lengthcount;
extern int samplefac;
extern int alphadec;
extern int netsize;
extern int radpower[32];

int contest(int b, int g, int r, int aa);
void altersingle(int alpha, int i, int b, int g, int r, int aa);
void alterneigh(int rad, int i, int b, int g, int r, int aa);

void learn() {
    int i;
    int j;
    int b;
    int g;
    int r;
    int a;
    int radius;
    int rad;
    int alpha;
    int step;
    int delta;
    int samplepixels;
    unsigned char *p;
    unsigned char *lim;

    p = thepicture;
    samplepixels = lengthcount / (samplefac << 2);
    lim = thepicture + lengthcount;
    alphadec = (samplefac - 1) / 3 + 30;

    if (samplepixels < 101) {
        delta = 1;
    } else {
        delta = samplepixels / 100;
    }

    alpha = 1024;
    radius = (netsize << 3) & ~0x3F;
    rad = radius >> 6;

    if (rad < 2) {
        rad = 0;
    }

    j = 0;
    if (rad > 0) {
        do {
            radpower[j] = (((rad * rad - j * j) * 0x100) / (rad * rad)) << 10;
            j++;
        } while (j < rad);
    }

    if (lengthcount == lengthcount / 499 * 499) {
        if (lengthcount == lengthcount / 0x1EB * 0x1EB) {
            step = 0x7DC;
            if (lengthcount != lengthcount / 0x1E7 * 0x1E7) {
                step = 0x79C;
            }
        } else {
            step = 0x7AC;
        }
    } else {
        step = 0x7CC;
    }

    i = 0;
    if (samplepixels > 0) {
        do {
            b = static_cast<unsigned int>(*p) << 4;
            g = static_cast<unsigned int>(p[1]) << 4;
            r = static_cast<unsigned int>(p[2]) << 4;
            a = static_cast<unsigned int>(p[3]) << 4;
            j = contest(b, g, r, a);
            altersingle(alpha, j, b, g, r, a);

            if (rad != 0) {
                alterneigh(rad, j, b, g, r, a);
            }

            i++;

            for (p += step; lim <= p; p -= lengthcount) {
            }

            if (i == i / delta * delta) {
                radius = radius - radius / 0x1E;
                rad = radius >> 6;
                alpha = alpha - alpha / alphadec;

                if (rad < 2) {
                    rad = 0;
                }

                j = 0;
                if (rad > 0) {
                    do {
                        radpower[j] = alpha * (((rad * rad - j * j) * 0x100) / (rad * rad));
                        j++;
                    } while (j < rad);
                }
            }
        } while (i < samplepixels);
    }
}
