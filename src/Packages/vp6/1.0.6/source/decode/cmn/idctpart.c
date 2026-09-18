extern void *memset();

#define IdctAdjustBeforeShift 8

#define xC1S7 64277
#define xC2S6 60547
#define xC3S5 54491
#define xC4S4 46341
#define xC5S3 36410
#define xC6S2 25080
#define xC7S1 12785

static const int dequant_index[64] = {
    0,  1,  8,  16, 9,  2,  3,  10, 17, 24, 32, 25, 18, 11, 4,  5,
    12, 19, 26, 33, 40, 48, 41, 34, 27, 20, 13, 6,  7,  14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36, 29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46, 53, 60, 61, 54, 47, 55, 62, 63
};

void dequant_slow10(short *dequant_coeffs, short *quantized_list, int *DCT_block) {
    memset(DCT_block, 0, 128);

    DCT_block[dequant_index[0]] = quantized_list[0] * dequant_coeffs[0];
    DCT_block[dequant_index[1]] = quantized_list[1] * dequant_coeffs[1];
    DCT_block[dequant_index[2]] = quantized_list[2] * dequant_coeffs[2];
    DCT_block[dequant_index[3]] = quantized_list[3] * dequant_coeffs[3];
    DCT_block[dequant_index[4]] = quantized_list[4] * dequant_coeffs[4];
    DCT_block[dequant_index[5]] = quantized_list[5] * dequant_coeffs[5];
    DCT_block[dequant_index[6]] = quantized_list[6] * dequant_coeffs[6];
    DCT_block[dequant_index[7]] = quantized_list[7] * dequant_coeffs[7];
    DCT_block[dequant_index[8]] = quantized_list[8] * dequant_coeffs[8];
    DCT_block[dequant_index[9]] = quantized_list[9] * dequant_coeffs[9];
    DCT_block[dequant_index[10]] = quantized_list[10] * dequant_coeffs[10];
}

void IDct10(short *InputData, short *QuantMatrix, short *OutputData) {
    int IntermediateData[64];
    int *ip = IntermediateData;
    short *op = OutputData;

    int _A, _B, _C, _D, _Ad, _Bd, _Cd, _Dd, _E, _F, _G, _H;
    int _Ed, _Gd, _Add, _Bdd, _Fd, _Hd;
    int t1, t2;

    int loop;

    dequant_slow10(QuantMatrix, InputData, IntermediateData);

    /* Inverse DCT on the rows now */
    for (loop = 0; loop < 4; loop++) {
        /* Check for non-zero values */
        if (ip[0] | ip[1] | ip[2] | ip[3]) {
            t1 = (int)(xC1S7 * ip[1]);
            t1 >>= 16;
            _A = t1;

            t1 = (int)(xC7S1 * ip[1]);
            t1 >>= 16;
            _B = t1;

            t1 = (int)(xC3S5 * ip[3]);
            t2 = (int)(xC5S3 * ip[3]);
            t1 >>= 16;
            t2 >>= 16;
            _C = t1;
            _D = -t2;

            t1 = (int)(xC4S4 * (_A - _C));
            t1 >>= 16;
            _Ad = t1;

            t1 = (int)(xC4S4 * (_B - _D));
            t1 >>= 16;
            _Bd = t1;

            _Cd = _A + _C;
            _Dd = _B + _D;

            t1 = (int)(xC4S4 * ip[0]);
            t1 >>= 16;
            _E = t1;
            _F = t1;

            t1 = (int)(xC2S6 * ip[2]);
            t1 >>= 16;
            _G = t1;

            t1 = (int)(xC6S2 * ip[2]);
            t1 >>= 16;
            _H = t1;

            _Ed = _E - _G;
            _Gd = _E + _G;

            _Add = _F + _Ad;
            _Bdd = _Bd - _H;

            _Fd = _F - _Ad;
            _Hd = _Bd + _H;

            /* Final sequence of operations over-write original inputs. */
            ip[0] = (short)((_Gd + _Cd) >> 0);
            ip[7] = (short)((_Gd - _Cd) >> 0);

            ip[1] = (short)((_Add + _Hd) >> 0);
            ip[2] = (short)((_Add - _Hd) >> 0);

            ip[3] = (short)((_Ed + _Dd) >> 0);
            ip[4] = (short)((_Ed - _Dd) >> 0);

            ip[5] = (short)((_Fd + _Bdd) >> 0);
            ip[6] = (short)((_Fd - _Bdd) >> 0);
        }

        ip += 8; /* next row */
    }

    ip = IntermediateData;

    for (loop = 0; loop < 8; loop++) {
        /* Check for non-zero values (bitwise or faster than ||) */
        if (ip[0 * 8] | ip[1 * 8] | ip[2 * 8] | ip[3 * 8]) {
            t1 = (int)(xC1S7 * ip[1 * 8]);
            t1 >>= 16;
            _A = t1;

            t1 = (int)(xC7S1 * ip[1 * 8]);
            t1 >>= 16;
            _B = t1;

            t1 = (int)(xC3S5 * ip[3 * 8]);
            t2 = (int)(xC5S3 * ip[3 * 8]);
            t1 >>= 16;
            t2 >>= 16;
            _C = t1;
            _D = -t2;

            t1 = (int)(xC4S4 * (_A - _C));
            t1 >>= 16;
            _Ad = t1;

            t1 = (int)(xC4S4 * (_B - _D));
            t1 >>= 16;
            _Bd = t1;

            _Cd = _A + _C;
            _Dd = _B + _D;

            t1 = (int)(xC4S4 * ip[0 * 8]);
            t1 >>= 16;
            _E = t1;
            _F = t1;

            t1 = (int)(xC2S6 * ip[2 * 8]);
            t1 >>= 16;
            _G = t1;

            t1 = (int)(xC6S2 * ip[2 * 8]);
            t1 >>= 16;
            _H = t1;

            _Ed = _E - _G;
            _Gd = _E + _G;

            _Add = _F + _Ad;
            _Bdd = _Bd - _H;

            _Fd = _F - _Ad;
            _Hd = _Bd + _H;

            _Gd += IdctAdjustBeforeShift;
            _Add += IdctAdjustBeforeShift;
            _Ed += IdctAdjustBeforeShift;
            _Fd += IdctAdjustBeforeShift;

            /* Final sequence of operations over-write original inputs. */
            op[0 * 8] = (short)((_Gd + _Cd) >> 4);
            op[7 * 8] = (short)((_Gd - _Cd) >> 4);

            op[1 * 8] = (short)((_Add + _Hd) >> 4);
            op[2 * 8] = (short)((_Add - _Hd) >> 4);

            op[3 * 8] = (short)((_Ed + _Dd) >> 4);
            op[4 * 8] = (short)((_Ed - _Dd) >> 4);

            op[5 * 8] = (short)((_Fd + _Bdd) >> 4);
            op[6 * 8] = (short)((_Fd - _Bdd) >> 4);
        } else {
            op[0 * 8] = 0;
            op[7 * 8] = 0;

            op[1 * 8] = 0;
            op[2 * 8] = 0;

            op[3 * 8] = 0;
            op[4 * 8] = 0;

            op[5 * 8] = 0;
            op[6 * 8] = 0;
        }

        ip++; /* next column */
        op++;
    }
}
