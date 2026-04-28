int netsize = 0x100;
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

static int contest(int b, int g, int r, int aa);
static void altersingle(int alpha, int i, int b, int g, int r, int aa);
static void alterneigh(int rad, int i, int b, int g, int r, int aa);

void initnet(unsigned char *thepic, int len, int num_colours, int sample) {
    netsize = num_colours;
    thepicture = thepic;
    lengthcount = len;
    samplefac = sample;

    int i = 0;
    int *p;
    if (i >= num_colours) {
        return;
    }

    do {
        p = network[i];
        freq[i] = 0x10000 / num_colours;
        bias[i] = 0;
        p[3] = (i << 12) / num_colours;
        p[2] = (i << 12) / num_colours;
        p[1] = (i << 12) / num_colours;
        p[0] = (i << 12) / num_colours;
        i++;
    } while (i < num_colours);
}

void unbiasnet() {
    for (int i = 0; i < netsize; i++) {
        for (int j = 0; j < 4; j++) {
            network[i][j] >>= 4;
        }
        network[i][4] = i;
    }
}

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

void inxbuild() {
    int previouscol = 0;
    int startpos = 0;
    int i = 0;

    if (i < netsize) {
        do {
            int *pi = network[i];
            int smallpos = i;
            int smallval = pi[1];
            int j = i + 1;

            if (j < netsize) {
                do {
                    int value = network[j][1];
                    if (value < smallval) {
                        smallval = value;
                        smallpos = j;
                    }
                    j++;
                } while (j < netsize);
            }

            if (i != smallpos) {
                int *ps = network[smallpos];
                int temp;
                temp = ps[0]; ps[0] = pi[0]; pi[0] = temp;
                temp = ps[1]; ps[1] = pi[1]; pi[1] = temp;
                temp = ps[2]; ps[2] = pi[2]; pi[2] = temp;
                temp = ps[3]; ps[3] = pi[3]; pi[3] = temp;
                temp = ps[4]; ps[4] = pi[4]; pi[4] = temp;
            }

            if (smallval != previouscol) {
                netindex[previouscol] = (startpos + i) >> 1;
                int j = previouscol + 1;
                if (j < smallval) {
                    do {
                        netindex[j] = i;
                        j++;
                    } while (j < smallval);
                }
                previouscol = smallval;
                startpos = i;
            }
            i++;
        } while (i < netsize);
    }

    netindex[previouscol] = (startpos + netsize - 1) >> 1;
    int i2 = previouscol + 1;
    if (i2 < 0x100) {
        do {
            netindex[i2] = netsize - 1;
            i2++;
        } while (i2 < 0x100);
    }
}

int inxsearch(int b, int g, int r, int aa) {
    int best = -1;
    int i = netindex[g];
    int j = i - 1;
    int bestd = 0x400;

    while (true) {
        if (i < netsize) {
            int *p = network[i];
            int dist = p[1] - g;
            int next = netsize;

            if (dist < bestd) {
                next = i + 1;
                if (dist < 0) {
                    dist = -dist;
                }

                int a = p[0] - b;
                if (a < 0) {
                    a = -a;
                }

                if (dist + a < bestd) {
                    int value = p[2] - r;
                    if (value < 0) {
                        value = -value;
                    }
                    value = dist + a + value;
                    if (value < bestd) {
                        dist = p[3] - aa;
                        if (dist < 0) {
                            dist = -dist;
                        }
                        value += dist;
                        if (value < bestd) {
                            best = p[4];
                            bestd = value;
                        }
                    }
                }
            }

            i = next;
        } else if (j < 0) {
            return best;
        }

        if (j > -1) {
            int *p = network[j];
            int dist = g - p[1];

            if (dist < bestd) {
                if (dist < 0) {
                    dist = -dist;
                }

                int a = p[0] - b;
                if (a < 0) {
                    a = -a;
                }

                j--;
                if (dist + a < bestd) {
                    int value = p[2] - r;
                    if (value < 0) {
                        value = -value;
                    }
                    value = dist + a + value;
                    if (value < bestd) {
                        dist = p[3] - aa;
                        if (dist < 0) {
                            dist = -dist;
                        }
                        value += dist;
                        if (value < bestd) {
                            best = p[4];
                            bestd = value;
                        }
                    }
                }
            } else {
                j = -1;
            }
        }
    }
}

static int contest(int b, int g, int r, int aa) {
    int bestd = 0x7FFFFFFF;
    int bestbiasd = 0x7FFFFFFF;
    int bestpos = -1;
    int bestbiaspos = -1;
    int *bptr = bias;
    int *f = freq;
    int i = 0;

    if (i < netsize) {
        do {
        int *n = network[i];
        int dist = n[0] - b;
        if (dist < 0) {
            dist = -dist;
        }

        int value = n[1] - g;
        if (value < 0) {
            value = -value;
        }
        dist += value;

        value = n[2] - r;
        if (value < 0) {
            value = -value;
        }
        dist += value;

        value = n[3] - aa;
        if (value < 0) {
            value = -value;
        }
        dist += value;

        if (dist < bestd) {
            bestd = dist;
            bestpos = i;
        }

        value = dist - (*bptr >> 12);
        if (value < bestbiasd) {
            bestbiasd = value;
            bestbiaspos = i;
        }

        value = *f >> 10;
        *f -= value;
        *bptr += value << 10;
        i++;
        f++;
        bptr++;
        } while (i < netsize);
    }

    freq[bestpos] += 0x40;
    bias[bestpos] -= 0x10000;
    return bestbiaspos;
}

static void altersingle(int alpha, int i, int b, int g, int r, int aa) {
    int *p = &network[i][0];
    int current = p[0];
    int delta = alpha * (current - b);
    if (delta < 0) {
        delta += 0x3FF;
    }
    p[0] = current - (delta >> 10);

    current = *++p;
    delta = alpha * (current - g);
    if (delta < 0) {
        delta += 0x3FF;
    }
    p[0] = current - (delta >> 10);

    current = *++p;
    delta = alpha * (current - r);
    if (delta < 0) {
        delta += 0x3FF;
    }
    p[0] = current - (delta >> 10);

    current = *++p;
    alpha *= current - aa;
    if (alpha < 0) {
        alpha += 0x3FF;
    }
    p[0] = current - (alpha >> 10);
}

static void alterneigh(int rad, int i, int b, int g, int r, int aa) {
    int lo = i - rad;
    if (lo < -1) {
        lo = -1;
    }

    int hi = i + rad;
    if (hi > netsize) {
        hi = netsize;
    }

    int j = i + 1;
    i--;
    int *q = radpower;

    while (j < hi || lo < i) {
        int a = *++q;

        if (j < hi) {
            int *p = &network[j][0];
            int current = p[0];
            int delta = a * (current - b);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            p[0] = current - (delta >> 18);

            current = p[1];
            delta = a * (current - g);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            p[1] = current - (delta >> 18);

            current = p[2];
            delta = a * (current - r);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            p[2] = current - (delta >> 18);

            current = p[3];
            delta = a * (current - aa);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            j++;
            p[3] = current - (delta >> 18);
        }

        if (lo < i) {
            int *p = &network[i][0];
            int current = p[0];
            int delta = a * (current - b);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            p[0] = current - (delta >> 18);

            current = p[1];
            delta = a * (current - g);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            p[1] = current - (delta >> 18);

            current = p[2];
            delta = a * (current - r);
            if (delta < 0) {
                delta += 0x3FFFF;
            }
            p[2] = current - (delta >> 18);

            current = p[3];
            a *= current - aa;
            if (a < 0) {
                a += 0x3FFFF;
            }
            i--;
            p[3] = current - (a >> 18);
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
