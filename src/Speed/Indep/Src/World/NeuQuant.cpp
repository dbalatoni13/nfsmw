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
    thepicture = thepic;
    lengthcount = len;
    samplefac = sample;
    netsize = num_colours;

    if (num_colours < 1) {
        return;
    }

    for (int i = 0; i < num_colours; i++) {
        int value = (i << 12) / num_colours;
        network[i][0] = value;
        network[i][1] = value;
        network[i][2] = value;
        network[i][3] = value;
        freq[i] = 0x10000 / num_colours;
        bias[i] = 0;
    }
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

    for (int i = 0; i < netsize; i++) {
        int smallpos = i;
        int smallval = network[i][1];

        for (int j = i + 1; j < netsize; j++) {
            int value = network[j][1];
            if (value < smallval) {
                smallval = value;
                smallpos = j;
            }
        }

        if (i != smallpos) {
            int temp = network[smallpos][0];
            network[smallpos][0] = network[i][0];
            network[i][0] = temp;

            temp = network[smallpos][1];
            network[smallpos][1] = network[i][1];
            network[i][1] = temp;

            temp = network[smallpos][2];
            network[smallpos][2] = network[i][2];
            network[i][2] = temp;

            temp = network[smallpos][3];
            network[smallpos][3] = network[i][3];
            network[i][3] = temp;

            temp = network[smallpos][4];
            network[smallpos][4] = network[i][4];
            network[i][4] = temp;
        }

        if (smallval != previouscol) {
            netindex[previouscol] = (startpos + i) >> 1;
            for (int j = previouscol + 1; j < smallval; j++) {
                netindex[j] = i;
            }
            previouscol = smallval;
            startpos = i;
        }
    }

    netindex[previouscol] = (startpos + netsize - 1) >> 1;
    for (int i = previouscol + 1; i < 0x100; i++) {
        netindex[i] = netsize - 1;
    }
}

int inxsearch(int b, int g, int r, int aa) {
    int best = -1;
    int i = netindex[g];
    int j = netindex[g] - 1;
    int bestd = 0x400;

    while (true) {
        if (i < netsize) {
            int dist = network[i][1] - g;
            int next = netsize;

            if (dist < bestd) {
                next = i + 1;
                if (dist < 0) {
                    dist = -dist;
                }

                int a = network[i][0] - b;
                if (a < 0) {
                    a = -a;
                }

                if (dist + a < bestd) {
                    int value = network[i][2] - r;
                    if (value < 0) {
                        value = -value;
                    }
                    value = dist + a + value;
                    if (value < bestd) {
                        dist = network[i][3] - aa;
                        if (dist < 0) {
                            dist = -dist;
                        }
                        value += dist;
                        if (value < bestd) {
                            best = network[i][4];
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
            int dist = g - network[j][1];

            if (dist < bestd) {
                if (dist < 0) {
                    dist = -dist;
                }

                int a = network[j][0] - b;
                if (a < 0) {
                    a = -a;
                }

                j--;
                if (dist + a < bestd) {
                    int value = network[j + 1][2] - r;
                    if (value < 0) {
                        value = -value;
                    }
                    value = dist + a + value;
                    if (value < bestd) {
                        dist = network[j + 1][3] - aa;
                        if (dist < 0) {
                            dist = -dist;
                        }
                        value += dist;
                        if (value < bestd) {
                            best = network[j + 1][4];
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

    for (int i = 0; i < netsize; i++) {
        int dist = network[i][0] - b;
        if (dist < 0) {
            dist = -dist;
        }

        int value = network[i][1] - g;
        if (value < 0) {
            value = -value;
        }
        dist += value;

        value = network[i][2] - r;
        if (value < 0) {
            value = -value;
        }
        dist += value;

        value = network[i][3] - aa;
        if (value < 0) {
            value = -value;
        }
        dist += value;

        if (dist < bestd) {
            bestd = dist;
            bestpos = i;
        }

        value = dist - (bias[i] >> 12);
        if (value < bestbiasd) {
            bestbiasd = value;
            bestbiaspos = i;
        }

        value = freq[i] >> 10;
        freq[i] -= value;
        bias[i] += value << 10;
    }

    freq[bestpos] += 0x40;
    bias[bestpos] -= 0x10000;
    return bestbiaspos;
}

static void altersingle(int alpha, int i, int b, int g, int r, int aa) {
    i *= 0x14;

    int value = alpha * (network[0][0] - b);
    if (value < 0) {
        value += 0x3FF;
    }
    *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i) = network[i / 0x14][0] - (value >> 10);

    int current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i + 4);
    value = alpha * (current - g);
    if (value < 0) {
        value += 0x3FF;
    }
    *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i + 4) = current - (value >> 10);

    current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i + 8);
    value = alpha * (current - r);
    if (value < 0) {
        value += 0x3FF;
    }
    *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i + 8) = current - (value >> 10);

    current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i + 12);
    alpha *= current - aa;
    if (alpha < 0) {
        alpha += 0x3FF;
    }
    *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + i + 12) = current - (alpha >> 10);
}

static void alterneigh(int rad, int i, int b, int g, int r, int aa) {
    int lo = i - rad;
    if (lo < -1) {
        lo = -1;
    }

    int hi = i + rad;
    if (netsize < hi) {
        hi = netsize;
    }

    int j = i + 1;
    i--;
    int *q = radpower;

    while (j < hi || lo < i) {
        int a = *++q;

        if (j < hi) {
            int offset = j * 0x14;
            int value = a * (*reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset) - b);
            if (value < 0) {
                value += 0x3FFFF;
            }
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset) -= value >> 18;

            int current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 4);
            value = a * (current - g);
            if (value < 0) {
                value += 0x3FFFF;
            }
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 4) = current - (value >> 18);

            current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 8);
            value = a * (current - r);
            if (value < 0) {
                value += 0x3FFFF;
            }
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 8) = current - (value >> 18);

            current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 12);
            value = a * (current - aa);
            if (value < 0) {
                value += 0x3FFFF;
            }
            j++;
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 12) = current - (value >> 18);
        }

        if (lo < i) {
            int offset = i * 0x14;
            int value = a * (*reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset) - b);
            if (value < 0) {
                value += 0x3FFFF;
            }
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset) -= value >> 18;

            int current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 4);
            value = a * (current - g);
            if (value < 0) {
                value += 0x3FFFF;
            }
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 4) = current - (value >> 18);

            current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 8);
            value = a * (current - r);
            if (value < 0) {
                value += 0x3FFFF;
            }
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 8) = current - (value >> 18);

            current = *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 12);
            a *= current - aa;
            if (a < 0) {
                a += 0x3FFFF;
            }
            i--;
            *reinterpret_cast<int *>(reinterpret_cast<unsigned char *>(network) + offset + 12) = current - (a >> 18);
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
