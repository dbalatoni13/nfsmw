extern void *memset(void *dest, int value, unsigned int size);

static const unsigned int dequant_index[64] = {
    0, 1, 8, 16, 9, 2, 3, 10,
    17, 24, 32, 25, 18, 11, 4, 5,
    12, 19, 26, 33, 40, 48, 41, 34,
    27, 20, 13, 6, 7, 14, 21, 28,
    35, 42, 49, 56, 57, 50, 43, 36,
    29, 22, 15, 23, 30, 37, 44, 51,
    58, 59, 52, 45, 38, 31, 39, 46,
    53, 60, 61, 54, 47, 55, 62, 63
};

#define DEQUANT(index) \
    DCT_block[dequant_index[index]] = quantized_list[index] * dequant_coeffs[index]

void dequant_slow10(short *dequant_coeffs, short *quantized_list, int *DCT_block) {
    memset(DCT_block, 0, 128);
    DEQUANT(0);
    DEQUANT(1);
    DEQUANT(2);
    DEQUANT(3);
    DEQUANT(4);
    DEQUANT(5);
    DEQUANT(6);
    DEQUANT(7);
    DEQUANT(8);
    DEQUANT(9);
    DEQUANT(10);
}

#undef DEQUANT

#define C1 64277
#define C2 60547
#define C3 54491
#define C4 46341
#define C5 36410
#define C6 25080
#define C7 12785

/* NON_MATCHING: normalized DWARF matches, but the compiler still folds two
 * product-result copies that are separate instructions in the retail routine.
 */
void IDct10(short *InputData, short *QuantMatrix, short *OutputData) {
    int loop;
    int t1;
    int t2;
    int IntermediateData[64];
    int _A;
    int _B;
    int _C;
    int _D;
    int _Ad;
    int _Bd;
    int _Cd;
    int _Dd;
    int _E;
    int _F;
    int _G;
    int _H;
    int _Ed;
    int _Gd;
    int _Add;
    int _Bdd;
    int _Fd;
    int _Hd;
    int *ip;
    short *op;

    ip = IntermediateData;
    op = OutputData;
    dequant_slow10(QuantMatrix, InputData, ip);
    for (loop = 4; loop; loop--) {
        if (ip[0] | ip[1] | ip[2] | ip[3]) {
            t1 = ip[1] * C1;
            _A = (t1 >> 16);
            t1 = ip[1] * C7;
            _B = (t1 >> 16);
            t1 = ip[3] * C3;
            t2 = ip[3] * C5;
            t2 >>= 16;
            _C = -t2;
            _D = t1 >> 16;
            _Cd = _A + _D;
            _Dd = _B + _C;
            t1 = (_A - _D) * C4;
            _Ad = t1 >> 16;
            t1 = (_B - _C) * C4;
            _Bd = t1 >> 16;
            t1 = ip[0] * C4;
            _E = t1 >> 16;
            _F = _E;
            t1 = ip[2] * C2;
            _G = t1 >> 16;
            t1 = ip[2] * C6;
            _H = t1 >> 16;
            _Ed = _E - _G;
            _Gd = _E + _G;
            _Add = _F + _Ad;
            _Bdd = _Bd - _H;
            _Fd = _F - _Ad;
            _Hd = _Bd + _H;
            ip[0] = (short)(_Gd + _Cd);
            ip[7] = (short)(_Gd - _Cd);
            ip[1] = (short)(_Add + _Hd);
            ip[2] = (short)(_Add - _Hd);
            ip[3] = (short)(_Ed + _Dd);
            ip[4] = (short)(_Ed - _Dd);
            ip[5] = (short)(_Fd + _Bdd);
            ip[6] = (short)(_Fd - _Bdd);
        }
        ip += 8;
    }
    ip = IntermediateData;
    for (loop = 8; loop; loop--) {
        if (ip[0] | ip[8] | ip[16] | ip[24]) {
            t1 = ip[8] * C1;
            _A = (t1 >> 16);
            t1 = ip[8] * C7;
            _B = (t1 >> 16);
            t1 = ip[24] * C3;
            t2 = ip[24] * C5;
            t2 >>= 16;
            _C = -t2;
            _D = t1 >> 16;
            _Cd = _A + _D;
            _Dd = _B + _C;
            t1 = (_A - _D) * C4;
            _Ad = t1 >> 16;
            t1 = (_B - _C) * C4;
            _Bd = t1 >> 16;
            t1 = ip[0] * C4;
            _E = t1 >> 16;
            _F = _E;
            t1 = ip[16] * C2;
            _G = t1 >> 16;
            t1 = ip[16] * C6;
            _H = t1 >> 16;
            _Ed = _E - _G;
            _Gd = _E + _G;
            _Add = _F + _Ad;
            _Bdd = _Bd - _H;
            _Fd = _F - _Ad;
            _Hd = _Bd + _H;
            _Add += 8;
            _Fd += 8;
            _Ed += 8;
            _Gd += 8;
            op[0] = (_Gd + _Cd) >> 4;
            op[56] = (_Gd - _Cd) >> 4;
            op[8] = (_Add + _Hd) >> 4;
            op[16] = (_Add - _Hd) >> 4;
            op[24] = (_Ed + _Dd) >> 4;
            op[32] = (_Ed - _Dd) >> 4;
            op[40] = (_Fd + _Bdd) >> 4;
            op[48] = (_Fd - _Bdd) >> 4;
        } else {
            op[0] = 0;
            op[56] = 0;
            op[8] = 0;
            op[16] = 0;
            op[24] = 0;
            op[32] = 0;
            op[40] = 0;
            op[48] = 0;
        }
        ip += 1;
        op++;
    }
}

#undef C1
#undef C2
#undef C3
#undef C4
#undef C5
#undef C6
#undef C7
