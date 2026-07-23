#include "LZCompress.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstring>

static int Compare(unsigned char *a, unsigned char *b, int max) {
    int Result = 0;
    while (Result < max && a[Result] == b[Result]) {
        Result++;
    }
    return Result;
}

static void ShortMove(unsigned char *pDest, uint8 *pSrc, int Len) {
    int Run = 0;
    do {
        pDest[Run] = pSrc[Run];
        Run++;
    } while (Run < Len);
}

// total size: 0x10
class JLZHash {
  public:
    int32 Offset;
    uint8 *pData;
    JLZHash *pPrev;
    JLZHash *pNext;
};

// total size: 0xC
struct JLZHashPool {
  public:
    JLZHashPool(int WindowSize);
    ~JLZHashPool();
    void Update(uint8 *pData, int32 Offset);
    JLZHash *FindList(uint8 *pData);

  private:
    JLZHash *pPool;
    JLZHash **pHead;
    int32 PoolSize;
};

JLZHashPool::JLZHashPool(int WindowSize) {
    PoolSize = WindowSize;
    pPool = new ("JLZHash", 0) JLZHash[WindowSize];
    pHead = new ("JLZHash", 0) JLZHash *[0x2000];
    bMemSet(pPool, 0, PoolSize << 4);
    bMemSet(pHead, 0, 0x8000);
}

JLZHashPool::~JLZHashPool() {
    delete[] pPool;
    delete[] pHead;
}

static int32 GetTableHash(uint8 *pData) {
    unsigned int Value = (((pData[2] << 8) ^ (pData[1] << 4) ^ pData[0]) * 0x9e5f);
    return Value & 0x1fff;
}

void JLZHashPool::Update(uint8 *pData, int32 Offset) {
    int WindowPos = Offset - (Offset / PoolSize) * PoolSize;
    JLZHash *pHash = &pPool[WindowPos];
    int TableNum;
    if (pHash->pData != nullptr) {
        if (pHash->pNext != nullptr) {
            pHash->pNext->pPrev = pHash->pPrev;
        }
        if (pHash->pPrev != nullptr) {
            pHash->pPrev->pNext = pHash->pNext;
        }
        TableNum = GetTableHash(pHash->pData);
        if (pHead[TableNum] == pHash) {
            pHead[TableNum] = pHash->pNext;
        }
    }
    pHash->Offset = Offset;
    pHash->pData = pData;
    TableNum = GetTableHash(pData);
    pHash->pPrev = nullptr;
    pHash->pNext = pHead[TableNum];
    pHead[TableNum] = pHash;
    if (pHash->pNext != nullptr) {
        pHash->pNext->pPrev = pHash;
    }
}

JLZHash *JLZHashPool::FindList(unsigned char *pData) {
    return pHead[GetTableHash(pData)];
}

int JLZCompress(uint8 *pSrc, int32 Size, uint8 *pDest) {
    JLZHashPool Pool(0x810);
    int Pos = 0;
    uint8 *pOut;
    JLZHash *pHash;
    JLZHash *pLongRun;
    int LongRun;
    int MaxLen;
    int Run;
    int RunOffset;
    int OrigSize = Size;
    int Iteration = 0;
    unsigned short Control;
    unsigned short RunType;
    uint8 *pControl;
    uint8 *pRunType;
    LZHeader *header = reinterpret_cast<LZHeader *>(pDest);
    int previous_percent;

    header->ID = JLZCOMP_ID;
    header->Version = JLZCOMP_VERSION;
    header->HeaderSize = sizeof(LZHeader);
    header->Flags = 0;
    header->UncompressedSize = Size;
    header->CompressedSize = 0;
    previous_percent = 0;
    Control = 0xff00;
    RunType = 0xff00;
    pControl = pDest + sizeof(LZHeader);
    pRunType = pControl + 1;
    pOut = pRunType + 1;

    while (Size >= 0) {
        if (Size > 0x1001) {
            MaxLen = 0x1002;
        } else {
            MaxLen = Size;
        }

        pHash = Pool.FindList(pSrc);
        LongRun = 2;
        pLongRun = nullptr;

        if (pHash != nullptr) {
            do {
                Run = Compare(pHash->pData, pSrc, MaxLen);
                if (Run > LongRun) {
                    RunOffset = Pos - pHash->Offset;
                    if (Run < 0x23 || RunOffset < 0x10 || LongRun < 0x23) {
                        LongRun = Run;
                        pLongRun = pHash;
                    }
                }
                pHash = pHash->pNext;
            } while (pHash != nullptr && LongRun < 0x1002);
        }

        if (LongRun > 2) {
            Control >>= 1;
            RunOffset = Pos - pLongRun->Offset - 1;
            if (RunOffset > 0xf) {
                if (LongRun > 0x22) {
                    LongRun = 0x22;
                }
                RunType = (RunType >> 1) & 0x7f7f;
                pOut[0] = (((RunOffset - 0x10) >> 8) << 5) | (LongRun - 3);
                pOut[1] = RunOffset - 0x10;
                pOut += 2;
            } else {
                RunType >>= 1;
                pOut[0] = (((LongRun - 3) >> 8) << 4) | RunOffset;
                pOut[1] = LongRun - 3;
                pOut += 2;
            }
            Size -= LongRun;
            do {
                Pool.Update(pSrc, Pos);
                pSrc++;
                Pos++;
                LongRun--;
            } while (LongRun != 0);
        } else {
            Pool.Update(pSrc, Pos);
            Control = (Control >> 1) & 0x7f7f;
            Size--;
            *pOut = *pSrc;
            pOut++;
            Pos++;
            pSrc++;
        }

        if (Control < 0x100) {
            *pControl = Control;
            pControl = pOut;
            Control = 0xff00;
            pOut++;
        }
        if (RunType < 0x100) {
            *pRunType = RunType;
            pRunType = pOut;
            RunType = 0xff00;
            pOut++;
        }

        Iteration++;
        if ((Iteration & 0x1fff) == 0) {
            int current_percent = 10 - (Size * 10) / OrigSize;
            if (current_percent != previous_percent) {
                previous_percent = current_percent;
            }
        }
    }

    while (Control & 0xff00) {
        Control >>= 1;
    }
    *pControl = Control;
    while (RunType & 0xff00) {
        RunType >>= 1;
    }
    *pRunType = RunType;
    header->CompressedSize = pOut - pDest;
    return pOut - pDest;
}

int JLZDecompress(uint8 *pSrc, uint8 *pDest) {
    uint8 *pOut = pDest;
    uint8 *pEnd;
    int Offset;
    int Run;
    short Control;
    short RunType;
    LZHeader *header = reinterpret_cast<LZHeader *>(pSrc);
    int Size = header->CompressedSize;

    if (header->ID != JLZCOMP_ID) {
        return 0;
    }
    if (header->Version != JLZCOMP_VERSION) {
        return 0;
    }
    Size -= 0x12;
    pEnd = pOut + header->UncompressedSize;
    Control = pSrc[0x10] | 0x100;
    RunType = pSrc[0x11] | 0x100;
    pSrc += 0x12;
    while (Size != 0) {
        if ((Control & 1) != 0) {
            if ((RunType & 1) != 0) {
                Run = (((pSrc[0]) >> 4 << 8) | pSrc[1]) + 3;
                Offset = ((pSrc[0] & 0xf) + 1);

                ShortMove(pOut, pOut - Offset, Run);
            } else {
                Offset = (((pSrc[0]) >> 5 << 8) | pSrc[1]) + 0x11;
                Run = ((pSrc[0] & 0x1f) + 3);

                ShortMove(pOut, pOut - Offset, Run);
            }
            pOut += Run;
            pSrc += 2;
            Size -= 2;
            RunType >>= 1;
        } else {
            if (pOut < pEnd) {
                *pOut = *pSrc++;
                pOut++;
            }
            Size--;
        }
        Control >>= 1;
        if (Control == 1) {
            Size--;
            Control = *pSrc++ | 0x100;
        }
        if (RunType == 1) {
            Size--;
            RunType = *pSrc++ | 0x100;
        }
    }
    return header->UncompressedSize;
}

static int ZERO = 0;

// Decl: 444
#define GET16BITS()                                                                                                                                  \
    bitsunshifted = qs[0] | (bitsunshifted << 8);                                                                                                    \
    bitsunshifted = qs[1] | (bitsunshifted << 8);                                                                                                    \
    qs += 2;

// Decl: 450
#define SQgetbits(v, n)                                                                                                                              \
    if (n) {                                                                                                                                         \
        v = bits >> (32 - (n));                                                                                                                      \
        bits <<= (n);                                                                                                                                \
        bitsleft -= (n);                                                                                                                             \
    }                                                                                                                                                \
    if (bitsleft < 0) {                                                                                                                              \
        GET16BITS() bits = bitsunshifted << (-bitsleft);                                                                                             \
        bitsleft += 16;                                                                                                                              \
    }

// Decl: 467
#define SQgetnum(v)                                                                                                                                  \
    if ((int)bits < 0) {                                                                                                                             \
        SQgetbits(v, 3);                                                                                                                             \
        v -= 4;                                                                                                                                      \
    } else {                                                                                                                                         \
        int n;                                                                                                                                       \
        unsigned int v1;                                                                                                                             \
        if (bits >> 16) {                                                                                                                            \
            n = 2;                                                                                                                                   \
            do {                                                                                                                                     \
                bits <<= 1;                                                                                                                          \
                ++n;                                                                                                                                 \
            } while ((int)bits >= 0);                                                                                                                \
            bits <<= 1;                                                                                                                              \
            bitsleft -= (n - 1);                                                                                                                     \
            SQgetbits(v, ZERO);                                                                                                                      \
        } else {                                                                                                                                     \
            n = 2;                                                                                                                                   \
            do {                                                                                                                                     \
                ++n;                                                                                                                                 \
                SQgetbits(v, 1);                                                                                                                     \
            } while (!v);                                                                                                                            \
        }                                                                                                                                            \
        if (n > 16) {                                                                                                                                \
            SQgetbits(v, n - 16);                                                                                                                    \
            SQgetbits(v1, 16);                                                                                                                       \
            v = (v1 | (v << 16)) + (1 << n) - 4;                                                                                                     \
        } else {                                                                                                                                     \
            SQgetbits(v, n);                                                                                                                         \
            v = (v + (1 << n) - 4);                                                                                                                  \
        }                                                                                                                                            \
    }

// Decl: 515
#define SQmemset(ptr, val, amt)                                                                                                                      \
    {                                                                                                                                                \
        int i = amt / 16;                                                                                                                            \
        int intval = (val << 24) | (val << 16) | (val << 8) | val;                                                                                   \
        int *lptr = (int *)ptr;                                                                                                                      \
        while (i) {                                                                                                                                  \
            lptr[0] = intval;                                                                                                                        \
            lptr[1] = intval;                                                                                                                        \
            lptr[2] = intval;                                                                                                                        \
            lptr[3] = intval;                                                                                                                        \
            lptr += 4;                                                                                                                               \
            --i;                                                                                                                                     \
        }                                                                                                                                            \
    }

static int HUFF_decompress(unsigned char *packbuf, unsigned char *unpackbuf) {
    unsigned int type;
    unsigned char clue;
    int ulen;
    unsigned int cmp;
    int bitnum = 0;
    int cluelen = 0;
    unsigned char *qs;
    unsigned char *qd;
    unsigned int bits;
    unsigned int bitsunshifted = 0;
    int numbits;
    int bitsleft;
    unsigned int v;

    qs = packbuf;
    qd = unpackbuf;
    ulen = 0L;

    if (qs != nullptr) {
        {
            int mostbits;
            int i;
            int bitnumtbl[16];
            unsigned int deltatbl[16];
            unsigned int cmptbl[16];
            unsigned char codetbl[256];
            unsigned char quickcodetbl[256];
            unsigned char quicklentbl[256];

            bitsleft = -16;
            bits = 0;
            SQgetbits(v, ZERO);
            SQgetbits(type, 16);

            if (type & 0x8000) {
                if (type & 0x100) {
                    SQgetbits(v, 16);
                    SQgetbits(v, 16);
                }
                type &= ~0x100;
                SQgetbits(v, 16);
                SQgetbits(ulen, 16);
                ulen |= (v << 16);
            } else {
                if (type & 0x100) {
                    SQgetbits(v, 8);
                    SQgetbits(v, 16);
                }
                type &= ~0x100;
                SQgetbits(v, 8);
                SQgetbits(ulen, 16);
                ulen |= (v << 16);
            }

            {
                {
                    int numchars;
                    {
                        unsigned int basecmp;
                        {
                            unsigned int t;
                            SQgetbits(t, 8);
                            clue = (unsigned char)t;
                        }
                        numchars = 0;
                        numbits = 1;
                        basecmp = (unsigned int)0;
                        do {
                            basecmp <<= 1;
                            deltatbl[numbits] = basecmp - numchars;
                            SQgetnum(bitnum);
                            bitnumtbl[numbits] = bitnum;
                            numchars += bitnum;
                            basecmp += bitnum;
                            cmp = 0;
                            if (bitnum)
                                cmp = (basecmp << (16 - numbits) & 0xffff);
                            cmptbl[numbits++] = cmp;
                        } while (!bitnum || cmp);
                    }
                    cmptbl[numbits - 1] = 0xffffffff;
                    mostbits = numbits - 1;
                    {
                        signed char leap[256];
                        unsigned char nextchar;
                        SQmemset(leap, 0, 256);
                        nextchar = (unsigned char)-1;
                        for (i = 0; i < numchars; ++i) {
                            int leapdelta = 0;
                            SQgetnum(leapdelta);
                            ++leapdelta;
                            do {
                                ++nextchar;
                                if (!leap[nextchar])
                                    --leapdelta;
                            } while (leapdelta);
                            leap[nextchar] = 1;
                            codetbl[i] = nextchar;
                        }
                    }
                }

                SQmemset(quicklentbl, 64, 256);
                {
                    int bits;
                    int bitnum;
                    int numbitentries;
                    int nextcode;
                    int nextlen;
                    int i;
                    unsigned char *codeptr;
                    unsigned char *quickcodeptr;
                    unsigned char *quicklenptr;
                    codeptr = codetbl;
                    quickcodeptr = quickcodetbl;
                    quicklenptr = quicklentbl;
                    for (bits = 1; bits <= mostbits; ++bits) {
                        bitnum = bitnumtbl[bits];
                        if (bits >= 9)
                            break;
                        numbitentries = 1 << (8 - bits);
                        while (bitnum--) {
                            nextcode = *codeptr++;
                            nextlen = bits;
                            if (nextcode == clue) {
                                cluelen = bits;
                                nextlen = 96;
                            }
                            for (i = 0; i < numbitentries; ++i) {
                                *quickcodeptr++ = (unsigned char)nextcode;
                                *quicklenptr++ = (unsigned char)nextlen;
                            }
                        }
                    }
                }
            }

            for (;;) {
                unsigned char *quickcodeptr = quickcodetbl;
                unsigned char *quicklenptr = quicklentbl;
                goto nextloop;
                do {
                    *qd++ = quickcodeptr[bits >> 24];
                    GET16BITS();
                    bits = bitsunshifted << (16 - bitsleft);
                nextloop:
                    numbits = quicklenptr[bits >> 24];
                    bitsleft -= numbits;
                    if (bitsleft >= 0) {
                        do {
                            *qd++ = quickcodeptr[bits >> 24];
                            bits <<= numbits;
                            numbits = quicklenptr[bits >> 24];
                            bitsleft -= numbits;
                            if (bitsleft < 0)
                                break;
                            *qd++ = quickcodeptr[bits >> 24];
                            bits <<= numbits;
                            numbits = quicklenptr[bits >> 24];
                            bitsleft -= numbits;
                            if (bitsleft < 0)
                                break;
                            *qd++ = quickcodeptr[bits >> 24];
                            bits <<= numbits;
                            numbits = quicklenptr[bits >> 24];
                            bitsleft -= numbits;
                            if (bitsleft < 0)
                                break;
                            *qd++ = quickcodeptr[bits >> 24];
                            bits <<= numbits;
                            numbits = quicklenptr[bits >> 24];
                            bitsleft -= numbits;
                        } while (bitsleft >= 0);
                    }
                    bitsleft += 16;
                } while (bitsleft >= 0);

                bitsleft = bitsleft - 16 + numbits;
                {
                    unsigned char code;
                    if (numbits != 96) {
                        cmp = (unsigned int)(bits >> 16);
                        numbits = 8;
                        do {
                            ++numbits;
                        } while (cmp >= cmptbl[numbits]);
                    } else
                        numbits = cluelen;

                    cmp = bits >> (32 - (numbits));
                    bits <<= (numbits);
                    bitsleft -= (numbits);
                    code = codetbl[cmp - deltatbl[numbits]];

                    if (code != clue && bitsleft >= 0) {
                        *qd++ = code;
                        goto nextloop;
                    }

                    if (bitsleft < 0) {
                        GET16BITS();
                        bits = bitsunshifted << -bitsleft;
                        bitsleft += 16;
                    }

                    if (code != clue) {
                        *qd++ = code;
                        goto nextloop;
                    }

                    {
                        int runlen = 0;
                        unsigned char *d = qd;
                        unsigned char *dest;
                        SQgetnum(runlen);
                        if (runlen) {
                            dest = d + runlen;
                            code = *(d - 1);
                            do {
                                *d++ = code;
                            } while (d < dest);
                            qd = d;
                            goto nextloop;
                        }
                    }

                    SQgetbits(v, 1);
                    if (v)
                        break;
                    {
                        unsigned int t;
                        SQgetbits(t, 8);
                        code = (unsigned char)t;
                    }
                    *qd++ = code;
                    goto nextloop;
                }
            }

            {
                int i;
                int nextchar;
                if (type == 0x32fb || type == 0xb2fb) {
                    i = 0;
                    qd = unpackbuf;
                    while (qd < unpackbuf + ulen) {
                        i += (int)*qd;
                        *qd++ = (unsigned char)i;
                    }
                } else if (type == 0x34fb || type == 0xb4fb) {
                    i = 0;
                    nextchar = 0;
                    qd = unpackbuf;
                    while (qd < unpackbuf + ulen) {
                        i += (int)*qd;
                        nextchar += i;
                        *qd++ = (unsigned char)nextchar;
                    }
                }
            }
        }
    }
    return ulen;
}

void HUFF_decode(void *dest, const void *src) {
    HUFF_decompress(static_cast<unsigned char *>(const_cast<void *>(src)), static_cast<unsigned char *>(dest));
}

// total size: 0x8
// Decl: 936
struct HUFFMemStruct {
    char *ptr;
    int len;
};

#define HUFFBIGNUM 32000 // Decl: 941
#define HUFFTREESIZE 520 // Decl: 942
#define HUFFCODES 256    // Decl: 943
#define HUFFMAXBITS 16   // Decl: 944
#define HUFFREPTBL 252   // Decl: 945

// total size: 0x31EC
// Decl: 948
struct HuffEncodeContext {
    char qleapcode[HUFFCODES];             // offset 0x0, size 0x100
    unsigned int count[768];               // offset 0x100, size 0xC00
    unsigned int bitnum[HUFFMAXBITS + 1];  // offset 0xD00, size 0x44
    unsigned int repbits[HUFFREPTBL];      // offset 0xD44, size 0x3F0
    unsigned int repbase[HUFFREPTBL];      // offset 0x1134, size 0x3F0
    unsigned int tree_left[HUFFTREESIZE];  // offset 0x1524, size 0x820
    unsigned int tree_right[HUFFTREESIZE]; // offset 0x1D44, size 0x820
    unsigned int bitsarray[HUFFCODES];     // offset 0x2564, size 0x400
    unsigned int patternarray[HUFFCODES];  // offset 0x2964, size 0x400
    unsigned int masks[17];                // offset 0x2D64, size 0x44
    unsigned int packbits;                 // offset 0x2DA8, size 0x4
    unsigned int workpattern;              // offset 0x2DAC, size 0x4
    unsigned char *buffer;                 // offset 0x2DB0, size 0x4
    unsigned char *bufptr;                 // offset 0x2DB4, size 0x4
    int flen;                              // offset 0x2DB8, size 0x4
    unsigned int csum;                     // offset 0x2DBC, size 0x4
    unsigned int mostbits;                 // offset 0x2DC0, size 0x4
    unsigned int codes;                    // offset 0x2DC4, size 0x4
    unsigned int chainused;                // offset 0x2DC8, size 0x4
    unsigned int clue;                     // offset 0x2DCC, size 0x4
    unsigned int dclue;                    // offset 0x2DD0, size 0x4
    unsigned int clues;                    // offset 0x2DD4, size 0x4
    unsigned int dclues;                   // offset 0x2DD8, size 0x4
    int mindelta;                          // offset 0x2DDC, size 0x4
    int maxdelta;                          // offset 0x2DE0, size 0x4
    unsigned int plen;                     // offset 0x2DE4, size 0x4
    unsigned int ulen;                     // offset 0x2DE8, size 0x4
    unsigned int sortptr[HUFFCODES];       // offset 0x2DEC, size 0x400
};

// Decl: 998
static void HUFF_writebits(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int bitpattern, unsigned int len) {
    if (len > 16) {
        HUFF_writebits(EC, dest, bitpattern >> 16, len - 16);
        HUFF_writebits(EC, dest, bitpattern, 16);
    } else {
        EC->packbits = EC->packbits + len;
        EC->workpattern = EC->workpattern + ((bitpattern & EC->masks[len]) << (0x18 - EC->packbits));
        while (EC->packbits > 7) {
            *(dest->ptr + dest->len) = static_cast<unsigned char>(EC->workpattern >> 16);
            dest->len++;
            EC->workpattern <<= 8;
            EC->packbits -= 8;
            EC->plen++;
        }
    }
}

static void HUFF_treechase(HuffEncodeContext *EC, unsigned int node, unsigned int bits) {
    if (node < HUFFCODES) {
        EC->bitsarray[node] = bits;
    } else {
        HUFF_treechase(EC, EC->tree_left[node], bits + 1);
        HUFF_treechase(EC, EC->tree_right[node], bits + 1);
    }
}

static void HUFF_maketree(HuffEncodeContext *EC) {
    unsigned int i;
    unsigned int i1;
    unsigned int ptr1;
    unsigned int val1;
    unsigned int val2;
    unsigned int ptr2;
    unsigned int nodes = HUFFCODES;
    unsigned int list_count[HUFFCODES + 2];
    unsigned int list_ptr[HUFFCODES + 2];

    i1 = 0;
    list_count[i1++] = 0;

    for (i = 0; i < HUFFCODES; ++i) {
        EC->bitsarray[i] = 99;

        if (EC->count[i]) {
            list_count[i1] = EC->count[i];
            list_ptr[i1++] = i;
        }
    }

    EC->codes = i1 - 1;

    if (i1 > 2) {
        while (i1 > 2) {
            i = i1;
            val1 = list_count[--i];
            ptr1 = i;
            val2 = list_count[--i];
            ptr2 = i;

            if (val1 < val2) {
                val2 = val1;
                ptr2 = ptr1;
                val1 = list_count[i];
                ptr1 = i;
            }

            while (i != 0) {
                i--;

                while (list_count[i] > val1) {
                    i--;
                }

                if (i != 0) {
                    val1 = list_count[i];
                    ptr1 = i;

                    if (val1 <= val2) {
                        val1 = val2;
                        ptr1 = ptr2;
                        val2 = list_count[i];
                        ptr2 = i;
                    }
                }
            }

            EC->tree_left[nodes] = list_ptr[ptr1];
            EC->tree_right[nodes] = list_ptr[ptr2];
            list_count[ptr1] = val1 + val2;
            list_ptr[ptr1] = nodes;
            list_count[ptr2] = list_count[--i1];
            list_ptr[ptr2] = list_ptr[i1];
            nodes++;
        }

        HUFF_treechase(EC, nodes - 1, 0);
    } else {
        HUFF_treechase(EC, list_ptr[EC->codes], 1);
    }
}

static int HUFF_minrep(HuffEncodeContext *EC, unsigned int remaining, unsigned int r) {
    int min;
    int min1;
    int use;
    int newremaining;

    if (r != 0) {
        min = HUFF_minrep(EC, remaining, r - 1);

        if (EC->count[EC->clue + r]) {
            use = remaining / r;
            newremaining = remaining - use * r;
            min1 = HUFF_minrep(EC, newremaining, r - 1) + EC->bitsarray[EC->clue + r] * use;

            if (min1 < min) {
                min = min1;
            }
        }
    } else {
        min = 0;

        if (remaining != 0) {
            min = 20;

            if (remaining < HUFFREPTBL) {
                min = EC->bitsarray[EC->clue] + 3 + EC->repbits[remaining] * 2;
            }
        }
    }

    return min;
}

static void HUFF_writenum(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int num) {
    int dphuf;
    int dbase;
    if (num < 0xfc) {
        dphuf = EC->repbits[num];
        dbase = EC->repbase[num];
    } else if (num < 0x1fc) {
        dphuf = 6;
        dbase = 0xfc;
    } else if (num < 0x3fc) {
        dphuf = 7;
        dbase = 0x1fc;
    } else if (num < 0x7fc) {
        dphuf = 8;
        dbase = 0x3fc;
    } else if (num < 0xffc) {
        dphuf = 9;
        dbase = 0x7fc;
    } else if (num < 0x1ffc) {
        dphuf = 10;
        dbase = 0xffc;
    } else if (num < 0x3ffc) {
        dphuf = 0xb;
        dbase = 0x1ffc;
    } else if (num < 0x7ffc) {
        dphuf = 0xc;
        dbase = 0x3ffc;
    } else if (num < 0xfffc) {
        dphuf = 0xd;
        dbase = 0x7ffc;
    } else if (num < 0x1fffc) {
        dphuf = 0xe;
        dbase = 0xfffc;
    } else if (num < 0x3fffc) {
        dphuf = 0xf;
        dbase = 0x1fffc;
    } else if (num < 0x80000) {
        dphuf = 0x10;
        dbase = 0x3fffc;
    } else if (num < 0x100000) {
        dphuf = 0x11;
        dbase = 0x80000;
    } else {
        dphuf = 0x12;
        dbase = 0x100000;
    }
    HUFF_writebits(EC, dest, 1, dphuf + 1);
    HUFF_writebits(EC, dest, num - dbase, dphuf + 2);
}

static void HUFF_writeexp(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int code) {
    HUFF_writebits(EC, dest, EC->patternarray[EC->clue], EC->bitsarray[EC->clue]);
    HUFF_writenum(EC, dest, 0);
    HUFF_writebits(EC, dest, code, 9);
}

static void HUFF_writecode(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int code) {
    if (code == EC->clue) {
        HUFF_writeexp(EC, dest, code);
    } else {
        HUFF_writebits(EC, dest, EC->patternarray[code], EC->bitsarray[code]);
    }
}

static void HUFF_init(HuffEncodeContext *EC) {
    unsigned int i = 0;
    for (; i < 4; i++) {
        EC->repbits[i] = 0;
        EC->repbase[i] = 0;
    }
    for (; i < 12; i++) {
        EC->repbits[i] = 1;
        EC->repbase[i] = 4;
    }
    for (; i < 28; i++) {
        EC->repbits[i] = 2;
        EC->repbase[i] = 0xc;
    }
    for (; i < 60; i++) {
        EC->repbits[i] = 3;
        EC->repbase[i] = 0x1c;
    }
    for (; i < 124; i++) {
        EC->repbits[i] = 4;
        EC->repbase[i] = 0x3c;
    }
    for (; i < 252; i++) {
        EC->repbits[i] = 5;
        EC->repbase[i] = 0x7c;
    }
}

static void HUFF_analysis(struct HuffEncodeContext *EC, unsigned int opt, unsigned int chainsaw) {
    unsigned char *bptr1;
    unsigned char *bptr2;
    unsigned int i;
    unsigned int i1;
    unsigned int i2 = 0;
    unsigned int i3;
    unsigned int thres = 0;
    int di;
    unsigned int pattern;
    unsigned int rep1;
    unsigned int repn;
    unsigned int ncode;
    unsigned int irep;
    unsigned int remaining;
    unsigned int count2[HUFFCODES];
    unsigned int dcount[HUFFCODES];

    for (i = 0; i < 768; i++) {
        EC->count[i] = 0;
    }

    bptr1 = EC->buffer;
    i1 = 256;
    i3 = 0;
    while (bptr1 < EC->bufptr) {
        i = static_cast<unsigned int>(*bptr1++);
        i3 = i3 + i;
        if (i == i1) {
            i2 = 0;
            bptr2 = bptr1 + 30000;
            if (bptr2 > EC->bufptr) {
                bptr2 = EC->bufptr;
            }
            while ((i == i1) && (bptr1 < bptr2)) {
                i2++;
                i = static_cast<unsigned int>(*bptr1++);
                i3 = i3 + i;
            }
            if (i2 < 255) {
                EC->count[512 + i2]++;
            } else {
                EC->count[512]++;
            }
        }
        EC->count[i]++;
        EC->count[((i + 256 - i1) & 255) + 256]++;
        i1 = i;
    }
    EC->csum = i3;
    if (!EC->count[512]) {
        EC->count[512]++;
    }

    EC->clues = 0;
    EC->dclues = 0;
    i3 = 0;
    for (i = 0; i < HUFFCODES; ++i) {
        i1 = i;
        i2 = 0;
        if (EC->count[i] < EC->count[i3]) {
            i3 = i;
        }
        while (!EC->count[i] && (i < 256)) {
            i2++;
            i++;
        }
        if (i2 >= EC->dclues) {
            EC->dclue = i1;
            EC->dclues = i2;
            if (EC->dclues >= EC->clues) {
                EC->dclue = EC->clue;
                EC->dclues = EC->clues;
                EC->clue = i1;
                EC->clues = i2;
            }
        }
    }

    if (opt & 32) {
        if (!EC->clues) {
            EC->clues = 1;
            EC->clue = i3;
        }
    }

    if ((~opt) & 2) {
        if (EC->clues > 1) {
            EC->clues = 1;
        }
        if ((~opt) & 1) {
            EC->clues = 0;
        }
    }
    if ((~opt) & 4) {
        EC->dclues = 0;
    } else {
        if (EC->dclues > 10) {
            i1 = EC->clue;
            i2 = EC->clues;
            EC->clue = EC->dclue;
            EC->clues = EC->dclues;
            EC->dclue = i1;
            EC->dclues = i2;
        }
        if ((EC->clues * 4) < EC->dclues) {
            EC->clues = EC->dclues / 4;
            EC->dclues = EC->dclues - EC->clues;
            EC->clue = EC->dclue + EC->dclues;
        }
    }

    if (EC->dclues) {
        EC->mindelta = -(static_cast<int>(EC->dclues) / 2);
        EC->maxdelta = EC->dclues + EC->mindelta;
        thres = static_cast<int>(EC->ulen / 25);
        for (i = 1; i <= EC->maxdelta; i++) {
            if (EC->count[256 + i] > thres) {
                EC->count[EC->dclue + (i - 1) * 2] = EC->count[256 + i];
            }
        }
        for (i = 1; i <= -EC->mindelta; i++) {
            if (EC->count[512 - i] > thres) {
                EC->count[EC->dclue + (i - 1) * 2 + 1] = EC->count[512 - i];
            }
        }
        for (i = 0, i2 = 0; i < EC->dclues; i++) {
            if (EC->count[EC->dclue + i] != 0) {
                i2 = i;
            }
        }
        di = EC->dclues - i2 - 1;
        EC->dclues -= di;
        if (EC->clue == (EC->dclue + EC->dclues + di)) {
            EC->clue -= di;
            EC->clues += di;
        }
        EC->mindelta = -(static_cast<int>(EC->dclues) / 2);
        EC->maxdelta = EC->dclues + EC->mindelta;
    }

    if (EC->clues != 0) {
        for (i = 0; i < EC->clues; i++) {
            EC->count[EC->clue + i] = EC->count[512 + i];
        }
    }

    HUFF_maketree(EC);

    if (EC->clues > 1) {
        for (i = 1; i < EC->clues; i++) {
            i1 = i - 1;
            if (i1 > 8) {
                i1 = 8;
            }
            if (EC->count[EC->clue + i]) {
                i1 = HUFF_minrep(EC, i, i1);
                if ((i1 <= EC->bitsarray[EC->clue + i]) || (EC->count[EC->clue + i] * (i1 - EC->bitsarray[EC->clue + i]) < (i / 2))) {
                    EC->count[EC->clue + i] = 0;
                }
            }
        }
    }

    for (i = 0; i < HUFFCODES; i++) {
        count2[i] = EC->count[i];
        dcount[i] = 0;
        EC->count[i] = 0;
        EC->count[256 + i] = 0;
        EC->count[512 + i] = 0;
    }

    i = 1;
    i1 = 256;
    bptr1 = EC->buffer;
    while (bptr1 < EC->bufptr) {
        i = static_cast<unsigned int>(*bptr1++);
        if (i == i1) {
            i2 = 0;
            bptr2 = bptr1 + 30000;
            if (bptr2 > EC->bufptr) {
                bptr2 = EC->bufptr;
            }
            while ((i == i1) && (bptr1 < bptr2)) {
                i = static_cast<unsigned int>(*bptr1++);
                i2++;
            }
            repn = HUFFBIGNUM;
            irep = HUFFBIGNUM;
            ncode = i2 * EC->bitsarray[i1];
            if (EC->clues != 0) {
                if (count2[EC->clue] != 0) {
                    repn = 20;
                    if (i2 < HUFFREPTBL) {
                        repn = EC->bitsarray[EC->clue] + 3 + EC->repbits[i2] * 2;
                    }
                }
                if (EC->clues > 1) {
                    remaining = i2;
                    irep = 0;
                    for (i3 = EC->clues - 1; i3 != 0; i3--) {
                        if (count2[EC->clue + i3]) {
                            rep1 = remaining / i3;
                            irep = irep + rep1 * EC->bitsarray[EC->clue + i3];
                            remaining = remaining - rep1 * i3;
                        }
                    }
                    if (remaining != 0) {
                        irep = HUFFBIGNUM;
                    }
                }
            }
            if ((ncode <= repn) && (ncode <= irep)) {
                EC->count[i1] += i2;
            } else {
                if (repn < irep) {
                    EC->count[EC->clue]++;
                } else {
                    remaining = i2;
                    irep = 0;
                    for (i3 = EC->clues - 1; i3 != 0; i3--) {
                        if (count2[EC->clue + i3]) {
                            rep1 = remaining / i3;
                            irep = irep + rep1 * EC->bitsarray[EC->clue + i3];
                            remaining = remaining - rep1 * i3;
                            EC->count[EC->clue + i3] += rep1;
                        }
                    }
                }
            }
        }
        if (EC->dclues != 0) {
            i3 = 0;
            di = i - i1;
            if (di <= EC->maxdelta) {
                if (di >= EC->mindelta) {
                    di = (i - i1 - 1) * 2 + EC->dclue;
                    if (i < i1) {
                        di = (i1 - i - 1) * 2 + EC->dclue + 1;
                    }
                    if (count2[di] > thres) {
                        if (count2[i] < 4) {
                            i3++;
                        }
                        if (EC->bitsarray[di] < EC->bitsarray[i]) {
                            i3++;
                        }
                        if (EC->bitsarray[di] == EC->bitsarray[i]) {
                            if (EC->count[di] > EC->count[i]) {
                                i3++;
                            }
                        }
                    }
                }
            }
            if (i3 != 0) {
                EC->count[di]++;
            } else {
                EC->count[i]++;
            }
        } else {
            EC->count[i]++;
        }
        i1 = i;
    }

    if (opt & 32) {
        EC->count[EC->clue]++;
    }

    HUFF_maketree(EC);

    EC->chainused = 0;
    i1 = 99;
    while (i1 > chainsaw) {
        i1 = 0;
        for (i = 0; i < HUFFCODES; i++) {
            if (EC->count[i] != 0) {
                if (EC->bitsarray[i] >= i1) {
                    i3 = i2;
                    i2 = i;
                    i1 = EC->bitsarray[i];
                }
            }
        }
        if (i1 > chainsaw) {
            i1 = 0;
            while (i1 < HUFFCODES) {
                if (EC->count[i1] != 0) {
                    if (EC->bitsarray[i1] < chainsaw) {
                        break;
                    }
                }
                i1++;
            }
            for (i = i1; i < HUFFCODES; i++) {
                if (EC->count[i] != 0) {
                    if ((EC->bitsarray[i] < chainsaw) && (EC->bitsarray[i] > EC->bitsarray[i1])) {
                        i1 = i;
                    }
                }
            }
            i = EC->bitsarray[i1] + 1;
            EC->bitsarray[i1] = i;
            EC->bitsarray[i2] = i;
            EC->bitsarray[i3] = EC->bitsarray[i3] - 1;
            EC->chainused = chainsaw;
            i1 = 99;
        }
    }

    if ((~opt) & 8) {
        i1 = 8;
        for (i = 0; i < HUFFCODES; i++) {
            EC->bitsarray[i] = i1;
        }
    }

    for (i = 0; i <= HUFFMAXBITS; i++) {
        EC->bitnum[i] = 0;
    }
    for (i = 0; i < HUFFCODES; i++) {
        if (EC->bitsarray[i] <= HUFFMAXBITS) {
            EC->bitnum[EC->bitsarray[i]]++;
        }
    }

    i = 0;
    i3 = 0;
    for (i2 = 1; i2 <= HUFFMAXBITS; i2++) {
        if (EC->bitnum[i2] != 0) {
            for (i1 = 0; i1 < HUFFCODES; i1++) {
                if (EC->bitsarray[i1] == i2) {
                    EC->sortptr[i++] = i1;
                }
            }
            i3 = i2;
        }
    }
    EC->mostbits = i3;

    pattern = 0;
    i2 = 0;
    for (i = 0; i < EC->codes; i++) {
        i1 = EC->sortptr[i];
        while (i2 < EC->bitsarray[i1]) {
            i2++;
            pattern = pattern << 1;
        }
        EC->patternarray[i1] = pattern;
        pattern++;
    }
}

static void HUFF_pack(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int opt) {
    unsigned char *bptr1;
    unsigned char *bptr2;
    unsigned int i;
    unsigned int i1;
    unsigned int i2;
    unsigned int i3;
    int uptype;
    unsigned int hlen;
    unsigned int ibits;
    unsigned int rladjust;
    int di;
    int firstcode;
    int firstbits;
    unsigned int rep1;
    unsigned int repn;
    unsigned int ncode;
    unsigned int irep;
    unsigned int remaining;
    unsigned int curpc;

    curpc = 0;
    uptype = 38;
    rladjust = 1;

    if (uptype == 38) {
        if (uptype == 34) {
            HUFF_writenum(EC, dest, EC->ulen);

            ibits = 0;
            if ((opt & 16) && EC->chainused == 0) {
                ibits = 1;
            }

            i = 0;
            if (EC->clues != 0) {
                i = 1;
            }
            if (ibits != 0) {
                i += 2;
            }
            if (EC->dclues != 0) {
                i += 4;
            }

            HUFF_writenum(EC, dest, i);

            if (EC->clues != 0) {
                HUFF_writenum(EC, dest, EC->clue);
                HUFF_writenum(EC, dest, EC->clues);
            }

            if (EC->dclues != 0) {
                HUFF_writenum(EC, dest, EC->dclue);
                HUFF_writenum(EC, dest, EC->dclues);
            }

            if (ibits == 0) {
                HUFF_writenum(EC, dest, EC->mostbits);
            }
        } else {
            HUFF_writebits(EC, dest, EC->clue, 8);
            rladjust = 0;
        }

        for (i = 1; i <= EC->mostbits; i++) {
            HUFF_writenum(EC, dest, EC->bitnum[i]);
        }

        for (i = 0; i < HUFFCODES; i++) {
            EC->qleapcode[i] = 0;
        }

        i = 0;
        i2 = 255;
        firstbits = 0;
        firstcode = -1;

        while (i < EC->codes) {
            i1 = EC->sortptr[i];
            di = -1;

            do {
                i2 = (i2 + 1) & 255;
                if (EC->qleapcode[i2] == 0) {
                    di++;
                }
            } while (i1 != i2);

            EC->qleapcode[i2] = 1;
            HUFF_writenum(EC, dest, di);
            i++;
        }
    }

    hlen = EC->plen + 1;

    if (EC->clues == 0) {
        EC->clue = HUFFBIGNUM;
    }

    i = 1;
    i1 = 256;
    bptr1 = EC->buffer;

    while (bptr1 < EC->bufptr) {
        i = *bptr1++;

        if (i == i1) {
            i2 = 0;
            bptr2 = bptr1 + 30000;

            if (bptr2 > EC->bufptr) {
                bptr2 = EC->bufptr;
            }

            while (i == i1 && bptr1 < bptr2) {
                i = *bptr1++;
                i2++;
            }

            repn = HUFFBIGNUM;
            irep = HUFFBIGNUM;
            ncode = i2 * EC->bitsarray[i1];

            if (EC->clues != 0) {
                if (EC->count[EC->clue] != 0) {
                    repn = 20;

                    if (i2 < HUFFREPTBL) {
                        repn = EC->bitsarray[EC->clue] + 3 + EC->repbits[i2] * 2;
                    }
                }

                if (EC->clues > 1) {
                    remaining = i2;
                    irep = 0;
                    i3 = EC->clues - 1;

                    while (i3 != 0) {
                        if (EC->count[EC->clue + i3]) {
                            rep1 = remaining / i3;
                            irep += rep1 * EC->bitsarray[EC->clue + i3];
                            remaining -= rep1 * i3;
                        }

                        i3--;
                    }

                    if (remaining != 0) {
                        irep = HUFFBIGNUM;
                    }
                }
            }

            if (ncode <= repn && ncode <= irep) {
                while (i2-- != 0) {
                    HUFF_writecode(EC, dest, i1);
                }
            } else {
                if (repn < irep) {
                    HUFF_writebits(EC, dest, EC->patternarray[EC->clue], EC->bitsarray[EC->clue]);
                    HUFF_writenum(EC, dest, i2 - rladjust);
                } else {
                    remaining = i2;
                    irep = 0;
                    i3 = EC->clues - 1;

                    while (i3 != 0) {
                        if (EC->count[EC->clue + i3]) {
                            rep1 = remaining / i3;
                            irep += rep1 * EC->bitsarray[EC->clue + i3];
                            remaining -= rep1 * i3;

                            while (rep1--) {
                                HUFF_writecode(EC, dest, EC->clue + i3);
                            }
                        }

                        i3--;
                    }
                }
            }
        }

        i3 = 0;
        if (EC->dclues) {
            di = i - i1;

            if (di <= EC->maxdelta && di >= EC->mindelta) {
                di = (i - i1 - 1) * 2 + EC->dclue;

                if (i < i1) {
                    di = (i1 - i - 1) * 2 + EC->dclue + 1;
                }
                if (EC->bitsarray[di] < EC->bitsarray[i]) {
                    HUFF_writebits(EC, dest, EC->patternarray[di], EC->bitsarray[di]);
                    i3++;
                }
            }
        }

        i1 = i;
        if (i3 == 0) {
            HUFF_writecode(EC, dest, i);
        }
        if (reinterpret_cast<intptr_t>(bptr1) - reinterpret_cast<intptr_t>(EC->buffer) >= static_cast<intptr_t>(EC->plen + curpc)) {
            curpc = reinterpret_cast<intptr_t>(bptr1) - reinterpret_cast<intptr_t>(EC->buffer) - EC->plen;
        }
    }

    HUFF_writebits(EC, dest, EC->patternarray[EC->clue], EC->bitsarray[EC->clue]);
    HUFF_writenum(EC, dest, 0);
    HUFF_writebits(EC, dest, 2, 2);
    HUFF_writebits(EC, dest, 0, 7);

    curpc += 2;
}

static int HUFF_packfile(HuffEncodeContext *EC, HUFFMemStruct *infile, HUFFMemStruct *outfile, int ulen) {
    unsigned int i;
    unsigned int uptype;
    unsigned int chainsaw;
    unsigned int opt;

    EC->packbits = 0;
    EC->workpattern = 0;
    chainsaw = 15;
    EC->masks[0] = 0;

    for (i = 1; i < 17; i++) {
        EC->masks[i] = (EC->masks[i - 1] << 1) + 1;
    }

    HUFF_init(EC);

    EC->buffer = reinterpret_cast<unsigned char *>(infile->ptr);
    EC->flen = infile->len;
    EC->ulen = EC->flen;
    EC->bufptr = EC->buffer + EC->flen;

    outfile->len = 0;
    EC->packbits = 0;
    EC->workpattern = 0;
    EC->plen = 0;

    opt = 0x39;
    HUFF_analysis(EC, opt, chainsaw);

    if (ulen > 0xffffff) {
        if (ulen == infile->len) {
            uptype = 0xb0fb;
            HUFF_writebits(EC, outfile, uptype, 16);
            HUFF_writebits(EC, outfile, infile->len, 32);
        } else {
            uptype = 0xb1fb;
            HUFF_writebits(EC, outfile, uptype, 16);
            HUFF_writebits(EC, outfile, ulen, 32);
            HUFF_writebits(EC, outfile, infile->len, 32);
        }
    } else {
        if (ulen == infile->len) {
            uptype = 0x30fb;
            HUFF_writebits(EC, outfile, uptype, 16);
            HUFF_writebits(EC, outfile, infile->len, 24);
        } else {
            uptype = 0x31fb;
            HUFF_writebits(EC, outfile, uptype, 16);
            HUFF_writebits(EC, outfile, ulen, 24);
            HUFF_writebits(EC, outfile, infile->len, 24);
        }
    }

    HUFF_pack(EC, outfile, opt);

    return outfile->len;
}

int HUFF_encode(void *compresseddata, const void *source, int sourcesize) {
    int plen = 0;
    HUFFMemStruct infile;
    HUFFMemStruct outfile;
    HuffEncodeContext *EC = static_cast<HuffEncodeContext *>(bMalloc(sizeof(HuffEncodeContext), "HuffEncodeContext", 0, 0));
    if (EC != nullptr) {
        infile.ptr = const_cast<char *>(static_cast<const char *>(source));
        infile.len = sourcesize;
        outfile.ptr = static_cast<char *>(compresseddata);
        outfile.len = sourcesize;
        plen = HUFF_packfile(EC, &infile, &outfile, sourcesize);
        bFree(EC);
    }
    return plen;
}

int HUFFDecompress(uint8 *pSrc, uint8 *pDest) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pSrc);
    unsigned int error;
    if (header->ID != HUFFCOMP_ID || header->Version != HUFFCOMP_VERSION) {
        return 0;
    }
    HUFF_decode(pDest, &header[1]);
    return header->UncompressedSize;
}

int HUFFCompress(uint8 *pSrc, int32 Size, uint8 *pDest) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pDest);
    header->ID = HUFFCOMP_ID;
    header->Version = HUFFCOMP_VERSION;
    header->HeaderSize = sizeof(*header);
    header->Flags = 0;
    header->UncompressedSize = Size;
    header->CompressedSize = HUFF_encode(&header[1], pSrc, Size);
    return header->CompressedSize + sizeof(*header);
}

int RAWDecompress(uint8 *pSrc, uint8 *pDest) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pSrc);
    if (header->ID != RAWCOMP_ID || header->Version != RAWCOMP_VERSION) {
        return 0;
    }
    memcpy(pDest, &header[1], header->UncompressedSize);
    return header->UncompressedSize;
}

int RAWCompress(uint8 *pSrc, int32 Size, uint8 *pDest) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pDest);
    header->ID = RAWCOMP_ID;
    header->Version = RAWCOMP_VERSION;
    header->HeaderSize = sizeof(*header);
    header->Flags = 0;
    header->UncompressedSize = Size;
    header->CompressedSize = Size;
    memcpy(&header[1], pSrc, Size);
    return header->CompressedSize + sizeof(*header);
}

#define PS *p++ != *p_src++ // Decl: 2152
#define ITEM_MAX 18         // Decl: 2155

#define TOP_WORD 0xFFFF0000 // Decl: 2158

// Decl: 2290
uint32 OldLZDecompress(uint8 *p_src_first, uint8 *p_dst_first) {
    LZHeader *header = reinterpret_cast<LZHeader *>(p_src_first);

    if (header == nullptr || p_dst_first == nullptr) {
        return 0;
    }

    uint32 src_len = header->CompressedSize;

    if (header->ID != LCZOMP_ID) {
        return 0;
    }

    uint8 *p_src = p_src_first + sizeof(LZHeader);
    uint8 *p_dst = p_dst_first;
    uint8 *p_src_post = p_src_first + src_len;
    uint8 *p_src_max16 = p_src_first + src_len - 32;
    uint32 control = 1;

    if (header->Flags == 1) {
        memcpy(p_dst_first, p_src, src_len - sizeof(LZHeader));
        return src_len - sizeof(LZHeader);
    }

    while (p_src != p_src_post) {
        int unroll;

        if (control == 1) {
            control = 0x10000 | *p_src++;
            control |= *p_src++ << 8;
        }

        unroll = p_src <= p_src_max16 ? 16 : 1;

        while (unroll--) {
            if (control & 1) {
                int lenmt;
                uint8 *p;

                lenmt = *p_src++;
                p = p_dst - (((lenmt & 0xF0) << 4) | *p_src++);

                *p_dst++ = *p++;
                *p_dst++ = *p++;
                *p_dst++ = *p++;

                lenmt &= 0xF;

                if (lenmt) {
                    do {
                        *p_dst++ = *p++;
                    } while (--lenmt);
                }
            } else {
                *p_dst++ = *p_src++;
            }

            control >>= 1;
        }
    }

    return p_dst - p_dst_first;
}

unsigned int LZGetMaxCompressedSize(unsigned int source_data_size) {
    return source_data_size * 2 + ITEM_MAX;
}

void LZByteSwapHeader(LZHeader *header) {
    bPlatEndianSwap(&header->ID);
    bPlatEndianSwap(&header->Version);
    bPlatEndianSwap(&header->HeaderSize);
    bPlatEndianSwap(&header->Flags);
    bPlatEndianSwap(&header->UncompressedSize);
    bPlatEndianSwap(&header->CompressedSize);
}

int32 LZValidHeader(LZHeader *header) {
    switch (header->ID) {
        case LCZOMP_ID:
        case HUFFCOMP_ID:
        case RAWCOMP_ID:
            return static_cast<int32>(header->Version == 1);
        case JLZCOMP_ID:
            return static_cast<int32>(header->Version == JLZCOMP_VERSION);
        default:
            return 0;
    }
}

uint32 LZCompress(uint8 *pSrc, uint32 SrcSize, uint8 *pDst) {
    unsigned int huffSize = HUFFCompress(pSrc, SrcSize, pDst);
    unsigned int lzSize = JLZCompress(pSrc, SrcSize, pDst);
    if (huffSize > lzSize) {
        return lzSize;
    }
    if (huffSize > SrcSize + sizeof(LZHeader)) {
        return RAWCompress(pSrc, SrcSize, pDst);
    }
    return HUFFCompress(pSrc, SrcSize, pDst);
}

unsigned int LZDecompress(uint8 *pSrc, uint8 *pDst) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pSrc);
    if ((header == nullptr) || (pDst == nullptr)) {
        return 0;
    }
    switch (header->ID) {
        case RAWCOMP_ID:
            return RAWDecompress(pSrc, pDst);
        case HUFFCOMP_ID:
            return HUFFDecompress(pSrc, pDst);
        case JLZCOMP_ID:
            return JLZDecompress(pSrc, pDst);
        case LCZOMP_ID:
            return OldLZDecompress(pSrc, pDst);
        default:
            return 0;
    }
}

#define COMPRESS_IN_PLACE_SIZE 32768       // Decl: 2475
#define COMPRESS_IN_PLACE_MAGIC 0x55441122 // Decl: 2476

// STRIPPED
bool IsCompressInPlaceData(const void *data) {
    return false;
}
