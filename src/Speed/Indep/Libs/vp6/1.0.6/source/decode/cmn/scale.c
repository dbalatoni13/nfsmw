void NullScale(unsigned char *dest, unsigned int destPitch, unsigned int destWidth) {}

extern void *memcpy(void *dest, const void *source, unsigned int size);

void HorizontalLine_Copy(const unsigned char *source, unsigned int sourceWidth, unsigned char *dest, unsigned int destWidth) {
    memcpy(dest, source, sourceWidth);
}

void HorizontalLine_1_2_Scale_C(const unsigned char *source, unsigned int sourceWidth,
                                unsigned char *dest, unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned char *des;
    const unsigned char *src;

    src = source;
    des = dest;
    sourceWidth--;
    i = 0;
    while (i < sourceWidth) {
        a = src[0];
        b = src[1];
        i++;
        des[0] = a;
        des[1] = (a + b + 1) >> 1;
        src++;
        des += 2;
    }
    a = src[0];
    des[0] = a;
    des[1] = a;
}

void VerticalBand_1_2_Scale_C(unsigned char *dest, unsigned int destPitch,
                              unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        a = des[0];
        b = *(des + (destPitch << 1));
        i++;
        des[destPitch] = (a + b + 1) >> 1;
        des++;
    }
}

void LastVerticalBand_1_2_Scale_C(unsigned char *dest, unsigned int destPitch,
                                  unsigned int destWidth) {
    unsigned int i;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        des[destPitch] = des[0];
        des++;
        i++;
    }
}

void HorizontalLine_4_5_Scale_C(const unsigned char *source, unsigned int sourceWidth,
                                unsigned char *dest, unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned char *des;
    const unsigned char *src;

    src = source;
    des = dest;
    sourceWidth -= 4;
    i = 0;
    while (i < sourceWidth) {
        b = src[1];
        a = src[0];
        des[0] = a;
        des[1] = (51 * a + 205 * b + 128) >> 8;
        a = src[3];
        c = src[2];
        des[2] = (102 * b + 154 * c + 128) >> 8;
        des[3] = (154 * c + 102 * a + 128) >> 8;
        b = src[4];
        des[4] = (205 * a + 51 * b + 128) >> 8;
        src += 4;
        des += 5;
        i += 4;
    }
    a = src[0];
    b = src[1];
    des[0] = a;
    des[1] = (51 * a + 205 * b + 128) >> 8;
    c = src[2];
    a = src[3];
    des[4] = a;
    des[3] = (154 * c + 102 * a + 128) >> 8;
    des[2] = (102 * b + 154 * c + 128) >> 8;
}

void VerticalBand_4_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                              unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        a = des[0];
        b = des[destPitch];
        c = des[destPitch * 2];
        d = des[destPitch * 3];
        des[destPitch] = (51 * a + 205 * b + 128) >> 8;
        des[destPitch * 2] = (102 * b + 154 * c + 128) >> 8;
        des[destPitch * 3] = (154 * c + 102 * d + 128) >> 8;
        a = des[destPitch * 4];
        des[destPitch * 4] = (205 * d + 51 * a + 128) >> 8;
        des++;
        i++;
    }
}

void LastVerticalBand_4_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                                  unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned int d;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        a = des[0];
        b = des[destPitch];
        c = des[destPitch * 2];
        d = des[destPitch * 3];
        des[destPitch] = (51 * a + 205 * b + 128) >> 8;
        des[destPitch * 2] = (102 * b + 154 * c + 128) >> 8;
        des[destPitch * 3] = (154 * c + 102 * d + 128) >> 8;
        des[destPitch * 4] = d;
        des++;
        i++;
    }
}

void HorizontalLine_3_5_Scale_C(const unsigned char *source, unsigned int sourceWidth,
                                unsigned char *dest, unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned char *des;
    const unsigned char *src;

    src = source;
    des = dest;
    sourceWidth -= 3;
    i = 0;
    while (i < sourceWidth) {
        a = src[0];
        b = src[1];
        des[0] = a;
        des[1] = (102 * a + 154 * b + 128) >> 8;
        c = src[2];
        des[2] = (205 * b + 51 * c + 128) >> 8;
        des[3] = (51 * b + 205 * c + 128) >> 8;
        a = src[3];
        des[4] = (154 * c + 102 * a + 128) >> 8;
        src += 3;
        des += 5;
        i += 3;
    }
    a = src[0];
    b = src[1];
    des[0] = a;
    des[1] = (102 * a + 154 * b + 128) >> 8;
    c = src[2];
    des[2] = (205 * b + 51 * c + 128) >> 8;
    des[3] = (51 * b + 205 * c + 128) >> 8;
    a = src[3];
    des[4] = (154 * c + 102 * a + 128) >> 8;
}

void VerticalBand_3_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                              unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        a = des[0];
        b = des[destPitch];
        c = des[destPitch * 2];
        des[destPitch] = (102 * a + 154 * b + 128) >> 8;
        des[destPitch * 2] = (205 * b + 51 * c + 128) >> 8;
        des[destPitch * 3] = (51 * b + 205 * c + 128) >> 8;
        a = des[destPitch * 3];
        des[destPitch * 4] = (154 * c + 102 * a + 128) >> 8;
        des++;
        i++;
    }
}

void LastVerticalBand_3_5_Scale_C(unsigned char *dest, unsigned int destPitch,
                                  unsigned int destWidth) {
    unsigned int i;
    unsigned int a;
    unsigned int b;
    unsigned int c;
    unsigned char *des;

    des = dest;
    i = 0;
    while (i < destWidth) {
        a = des[0];
        b = des[destPitch];
        c = des[destPitch * 2];
        des[destPitch] = (102 * a + 154 * b + 128) >> 8;
        des[destPitch * 2] = (205 * b + 51 * c + 128) >> 8;
        des[destPitch * 3] = (51 * b + 205 * c + 128) >> 8;
        des[destPitch * 4] = c;
        des++;
        i++;
    }
}
