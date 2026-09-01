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
        _A = ip[0];
        _B = ip[1];
        _C = ip[2];
        _D = ip[3];
        if (_A | _B | _C | _D) {
        _Ad = (_B * C1) >> 16;
        _Bd = (_B * C7) >> 16;
        _Cd = -((_D * C5) >> 16);
        _Dd = (_D * C3) >> 16;
        _Ed = _Bd - _Cd;
        _Gd = _Bd + _Cd;
        _Fd = _Ad - _Dd;
        _Hd = _Ad + _Dd;
        _Add = (_Fd * C4) >> 16;
        _Bdd = (_Ed * C4) >> 16;
        _F = (_A * C4) >> 16;
        _G = (_C * C2) >> 16;
        _Ad = _F + _Add;
        _Bd = _F - _Add;
        _E = (_C * C6) >> 16;
        _Cd = _F - _G;
        _Dd = _F + _G;
        _F = _Dd - _Hd;
        _G = _Cd - _Gd;
        _Dd += _Hd;
        _Cd += _Gd;
        _H = _Bdd + _E;
        _E = _Bdd - _E;

        ip[0] = (short)_Dd;
        ip[7] = (short)_F;
        ip[1] = (short)(_Ad + _H);
        ip[2] = (short)(_Ad - _H);
        ip[3] = (short)_Cd;
        ip[4] = (short)_G;
        ip[5] = (short)(_Bd + _E);
        ip[6] = (short)(_Bd - _E);
        }
        ip += 8;
    }

    ip = IntermediateData;
    for (loop = 8; loop; loop--) {
        _A = ip[0];
        _B = ip[8];
        _C = ip[16];
        _D = ip[24];
        if (_A | _B | _C | _D) {
            _Ad = (_B * C1) >> 16;
            _Bd = (_B * C7) >> 16;
            _Cd = -((_D * C5) >> 16);
            _Dd = (_D * C3) >> 16;
            _Ed = _Bd - _Cd;
            _Gd = _Bd + _Cd;
            _Fd = _Ad - _Dd;
            _Hd = _Ad + _Dd;
            _Add = (_Fd * C4) >> 16;
            _Bdd = (_Ed * C4) >> 16;
            _F = (_A * C4) >> 16;
            _G = (_C * C2) >> 16;
            _Ad = _F + _Add + 8;
            _Bd = _F - _Add + 8;
            _E = (_C * C6) >> 16;
            _Cd = _F - _G + 8;
            _Dd = _F + _G + 8;
            _F = _Dd - _Hd;
            _G = _Cd - _Gd;
            _Dd += _Hd;
            _Cd += _Gd;
            _H = _Bdd + _E;
            _E = _Bdd - _E;

            op[0] = _Dd >> 4;
            op[56] = _F >> 4;
            op[8] = (_Ad + _H) >> 4;
            op[16] = (_Ad - _H) >> 4;
            op[24] = _Cd >> 4;
            op[32] = _G >> 4;
            op[40] = (_Bd + _E) >> 4;
            op[48] = (_Bd - _E) >> 4;
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
        ip++;
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
