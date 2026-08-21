unsigned int seedPATH[6] = {
    0xF22D0E56,
    0x883126E9,
    0xC624DD2F,
    0x0702C49C,
    0x9E353F7D,
    0x6FDF3B64,
};

unsigned int PATHI_random() {
    unsigned int ax;
    unsigned int c;

    c = 0;
    ax = seedPATH[5] + seedPATH[4];
    if (ax < seedPATH[5] || ax < seedPATH[4]) {
        c = 1;
    }
    seedPATH[4] = ax;
    ax += seedPATH[3] + c;
    c = ax < seedPATH[3];
    seedPATH[3] = ax;
    ax += seedPATH[2] + c;
    c = ax < seedPATH[2];
    seedPATH[2] = ax;
    ax += seedPATH[1] + c;
    c = ax < seedPATH[1];
    seedPATH[1] = ax;
    ax += seedPATH[0] + c;
    seedPATH[5]++;
    seedPATH[0] = ax;
    if (seedPATH[5] != 0) {
        return ax;
    }
    seedPATH[4]++;
    if (seedPATH[4] != 0) {
        return ax;
    }
    seedPATH[3]++;
    if (seedPATH[3] != 0) {
        return ax;
    }
    seedPATH[2]++;
    if (seedPATH[2] != 0) {
        return ax;
    }
    seedPATH[1]++;
    if (seedPATH[1] != 0) {
        return ax;
    }
    seedPATH[0]++;
    return seedPATH[0];
}
