#include "NeuQant.hpp"

int netsize = 256;
int alphadec;
static unsigned char *thepicture;
static int lengthcount;
static int samplefac;
typedef int pixel[5];
static pixel network[256];
static int netindex[256];
static int bias[256];
static int freq[256];
static int radpower[32];

void initnet(unsigned char *thepic, int len, int num_colours, int sample) {
    int i;
    int *p;
    netsize = num_colours;
    thepicture = thepic;
    lengthcount = len;
    samplefac = sample;

    for (i = 0; i < num_colours; i++) {
        p = network[i];
        freq[i] = 0x10000 / num_colours;
        bias[i] = 0;
        p[3] = (i << 12) / num_colours;
        p[2] = (i << 12) / num_colours;
        p[1] = (i << 12) / num_colours;
        p[0] = (i << 12) / num_colours;
    }
}

void unbiasnet() {
    int i, j;
    for (i = 0; i < netsize; i++) {
        for (j = 0; j < 4; j++) {
            network[i][j] >>= 4;
        }
        network[i][4] = i;
    }
}

void inxbuild() {
    int i;
    int j;
    int smallpos;
    int smallval;
    int *p;
    int *q;
    int previouscol;
    int startpos;

    previouscol = 0;
    startpos = 0;

    for (i = 0; i < netsize; i++) {
        p = network[i];
        smallpos = i;
        smallval = p[1];

        for (j = i + 1; j < netsize; j++) {
            q = network[j];
            if (q[1] < smallval) {
                smallpos = j;
                smallval = q[1];
            }
        }

        q = network[smallpos];
        if (i != smallpos) {
            j = q[0];
            q[0] = p[0];
            p[0] = j;
            j = q[1];
            q[1] = p[1];
            p[1] = j;
            j = q[2];
            q[2] = p[2];
            p[2] = j;
            j = q[3];
            q[3] = p[3];
            p[3] = j;
            j = q[4];
            q[4] = p[4];
            p[4] = j;
        }

        if (smallval != previouscol) {
            netindex[previouscol] = (startpos + i) >> 1;
            for (j = previouscol + 1; j < smallval; j++) {
                netindex[j] = i;
            }
            previouscol = smallval;
            startpos = i;
        }
    }

    netindex[previouscol] = (startpos + (netsize - 1)) >> 1;
    for (j = previouscol + 1; j < 256; j++) {
        netindex[j] = netsize - 1;
    }
}

int inxsearch(int b, int g, int r, int aa) {
    int i;
    int j;
    int dist;
    int a;
    int bestd;
    int *p;
    int best;

    i = netindex[g];
    bestd = 1024;
    best = -1;
    j = i - 1;

    while (i < netsize || j >= 0) {
        if (i < netsize) {
            p = network[i];
            dist = p[1] - g;
            if (dist >= bestd) {
                i = netsize;
            } else {
                i++;
                if (dist < 0) {
                    dist = -dist;
                }
                a = p[0] - b;
                if (a < 0) {
                    a = -a;
                }
                dist += a;
                if (dist < bestd) {
                    a = p[2] - r;
                    if (a < 0) {
                        a = -a;
                    }
                    dist += a;
                    if (dist < bestd) {
                        a = p[3] - aa;
                        if (a < 0) {
                            a = -a;
                        }
                        dist += a;
                        if (dist < bestd) {
                            bestd = dist;
                            best = p[4];
                        }
                    }
                }
            }
        }

        if (j >= 0) {
            p = network[j];
            dist = g - p[1];
            if (dist >= bestd) {
                j = -1;
            } else {
                j--;
                if (dist < 0) {
                    dist = -dist;
                }
                a = p[0] - b;
                if (a < 0) {
                    a = -a;
                }
                dist += a;
                if (dist < bestd) {
                    a = p[2] - r;
                    if (a < 0) {
                        a = -a;
                    }
                    dist += a;
                    if (dist < bestd) {
                        a = p[3] - aa;
                        if (a < 0) {
                            a = -a;
                        }
                        dist += a;
                        if (dist < bestd) {
                            bestd = dist;
                            best = p[4];
                        }
                    }
                }
            }
        }
    }

    return best;
}

static int contest(int b, int g, int r, int aa) {
    int i;
    int dist;
    int a;
    int biasdist;
    int betafreq;
    int bestpos;
    int bestbiaspos;
    int bestd;
    int bestbiasd;
    int *p;
    int *f;
    int *n;

    bestd = 0x7fffffff;
    bestbiasd = 0x7fffffff;
    bestpos = -1;
    bestbiaspos = -1;
    p = bias;
    f = freq;

    for (i = 0; i < netsize; i++) {
        n = network[i];

        dist = n[0] - b;
        if (dist < 0) {
            dist = -dist;
        }

        a = n[1] - g;
        if (a < 0) {
            a = -a;
        }
        dist += a;

        a = n[2] - r;
        if (a < 0) {
            a = -a;
        }
        dist += a;

        a = n[3] - aa;
        if (a < 0) {
            a = -a;
        }
        dist += a;

        if (dist < bestd) {
            bestd = dist;
            bestpos = i;
        }

        biasdist = dist - (*p >> 12);
        if (biasdist < bestbiasd) {
            bestbiasd = biasdist;
            bestbiaspos = i;
        }

        betafreq = *f >> 10;
        *f -= betafreq;
        *p += betafreq << 10;
        f++;
        p++;
    }

    freq[bestpos] += 0x40;
    bias[bestpos] -= 0x10000;
    return bestbiaspos;
}

// UNSOLVED regswapsd
static void altersingle(int alpha, int i, int b, int g, int r, int aa) {
    int *n = &network[i][0];

    b = alpha * (*n - b);
    if (b < 0) {
        b += 0x3ff;
    }
    *n -= b >> 10;

    g = alpha * (*++n - g);
    if (g < 0) {
        g += 0x3ff;
    }
    *n -= g >> 10;

    r = alpha * (*++n - r);
    if (r < 0) {
        r += 0x3ff;
    }
    *n -= r >> 10;

    aa = alpha * (*++n - aa);
    if (aa < 0) {
        aa += 0x3ff;
    }
    *n -= aa >> 10;
}

// UNSOLVED regswaps
static void alterneigh(int rad, int i, int b, int g, int r, int aa) {
    int j;
    int k;
    int lo;
    int hi;
    int a;
    int *p;
    int *q;

    lo = i - rad;
    if (lo < -1) {
        lo = -1;
    }

    hi = i + rad;
    if (hi > netsize) {
        hi = netsize;
    }

    j = i + 1;
    k = i - 1;
    q = radpower;

    while (j < hi || k > lo) {
        a = *++q;

        if (j < hi) {
            p = &network[j][0];

            rad = a * (*p - b);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            *p -= rad >> 18;

            rad = a * (*++p - g);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            *p -= rad >> 18;

            rad = a * (*++p - r);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            *p -= rad >> 18;

            rad = a * (*++p - aa);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            j++;
            *p -= rad >> 18;
        }

        if (k > lo) {
            p = &network[k][0];

            rad = a * (*p - b);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            *p -= rad >> 18;

            rad = a * (*++p - g);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            *p -= rad >> 18;

            rad = a * (*++p - r);
            if (rad < 0) {
                rad += 0x3FFFF;
            }
            *p -= rad >> 18;

            a *= *++p - aa;
            if (a < 0) {
                a += 0x3FFFF;
            }
            k--;
            *p -= a >> 18;
        }
    }
}

// UNSOLVED regswaps
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

    if (samplepixels > 100) {
        delta = samplepixels / 100;
    } else {
        delta = 1;
    }

    alpha = 1024;
    radius = (netsize << 3) & ~0x3F;
    rad = radius >> 6;

    if (rad < 2) {
        rad = 0;
    }

    for (j = 0; j < rad; j++) {
        radpower[j] = (((rad * rad - j * j) * 256) / (rad * rad)) << 10;
    }

    if (lengthcount != lengthcount / 499 * 499) {
        step = 1996;
    } else {
        if (lengthcount != lengthcount / 491 * 491) {
            step = 1964;
        } else {
            if (lengthcount != lengthcount / 487 * 487) {
                step = 1948;
            } else {
                step = 2012;
            }
        }
    }

    for (i = 0; i < samplepixels;) {
        b = static_cast<unsigned int>(*p) << 4;
        g = static_cast<unsigned int>(p[1]) << 4;
        r = static_cast<unsigned int>(p[2]) << 4;
        a = static_cast<unsigned int>(p[3]) << 4;
        j = contest(b, g, r, a);
        altersingle(alpha, j, b, g, r, a);

        if (rad != 0) {
            alterneigh(rad, j, b, g, r, a);
        }

        for (p += step; p >= lim; p -= lengthcount) {
        }

        i++;
        if (i == i / delta * delta) {
            radius = radius - radius / 30;
            rad = radius >> 6;
            alpha = alpha - alpha / alphadec;

            if (rad < 2) {
                rad = 0;
            }

            for (j = 0; j < rad; j++) {
                radpower[j] = alpha * (((rad * rad - j * j) * 256) / (rad * rad));
            }
        }
    }
}

void nqGetPaletteEntry(int i, unsigned char &r, unsigned char &g, unsigned char &b, unsigned char &a) {
    if (i > -1 && i < netsize) {
        r = static_cast<unsigned char>(network[i][2]);
        g = static_cast<unsigned char>(network[i][1]);
        b = static_cast<unsigned char>(network[i][0]);
        a = static_cast<unsigned char>(network[i][3]);
        return;
    }

    r = 0;
    g = 0;
    b = 0;
    a = 0;
}
