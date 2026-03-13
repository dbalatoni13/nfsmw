#include "LZCompress.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstring>

void HUFF_decode(void *dest, const void *src);

static void ShortMove(unsigned char *pDest, unsigned char *pSrc, int Len) {
    int Run = 0;
    do {
        pDest[Run] = pSrc[Run];
        Run++;
    } while (Run < Len);
}

unsigned int LZGetMaxCompressedSize(unsigned int source_data_size) {
    return source_data_size * 2 + 0x12;
}

static int GetTableHash(unsigned char *pData) {
    return ((pData[0] << 6) ^ (pData[1] << 3) ^ pData[2]) & 0x1fff;
}

static int Compare(unsigned char *a, unsigned char *b, int max) {
    int i = 0;
    while (i < max && a[i] == b[i]) {
        i++;
    }
    return i;
}

// total size: 0x10
struct JLZHash {
    int Offset;
    unsigned char *pData;
    JLZHash *pPrev;
    JLZHash *pNext;
};

// total size: 0xC
struct JLZHashPool {
    JLZHash *pPool;
    JLZHash **pHead;
    int PoolSize;

    JLZHashPool(int WindowSize);
    ~JLZHashPool();
    void Update(unsigned char *pData, int Offset);
    JLZHash *FindList(unsigned char *pData);
};

JLZHashPool::JLZHashPool(int WindowSize) {
    PoolSize = WindowSize;
    pPool = new JLZHash[WindowSize];
    pHead = new JLZHash*[0x2000];
    bMemSet(pPool, 0, PoolSize << 4);
    bMemSet(pHead, 0, 0x8000);
}

JLZHashPool::~JLZHashPool() {
    if (pPool != nullptr) {
        delete[] pPool;
    }
    if (pHead != nullptr) {
        delete[] pHead;
    }
}

JLZHash *JLZHashPool::FindList(unsigned char *pData) {
    int hash = GetTableHash(pData);
    return pHead[hash];
}

void JLZHashPool::Update(unsigned char *pData, int Offset) {
    JLZHash *entry = &pPool[Offset - (Offset / PoolSize) * PoolSize];
    if (entry->pData != nullptr) {
        if (entry->pNext != nullptr) {
            entry->pNext->pPrev = entry->pPrev;
        }
        if (entry->pPrev != nullptr) {
            entry->pPrev->pNext = entry->pNext;
        }
        int hash = GetTableHash(entry->pData);
        if (pHead[hash] == entry) {
            pHead[hash] = entry->pNext;
        }
    }
    entry->Offset = Offset;
    entry->pData = pData;
    int hash = GetTableHash(pData);
    entry->pPrev = nullptr;
    entry->pNext = pHead[hash];
    pHead[hash] = entry;
    if (entry->pNext != nullptr) {
        entry->pNext->pPrev = entry;
    }
}

int JLZCompress(unsigned char *pSrc, int SrcSize, unsigned char *pDst) {
    unsigned int count = 0;
    JLZHashPool pool(0x810);
    unsigned int *header = reinterpret_cast<unsigned int *>(pDst);
    header[0] = 0x5a4c444a;
    int offset = 0;
    *reinterpret_cast<unsigned char *>(&header[1]) = 2;
    *(reinterpret_cast<unsigned char *>(header) + 5) = 0x10;
    unsigned int flags1 = 0xff00;
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(header) + 6) = 0;
    unsigned int flags2 = 0xff00;
    header[2] = SrcSize;
    unsigned char *flag1_ptr = reinterpret_cast<unsigned char *>(&header[4]);
    header[3] = 0;
    unsigned char *flag2_ptr = reinterpret_cast<unsigned char *>(header) + 0x11;
    unsigned char *out = reinterpret_cast<unsigned char *>(header) + 0x12;
    int progress = 0;
    int remaining = SrcSize;
    while (remaining > -1) {
        int max_match = remaining;
        if (remaining > 0x1001) {
            max_match = 0x1002;
        }
        JLZHash *list = pool.FindList(pSrc);
        flags1 = flags1 >> 1;
        count = count + 1;
        JLZHash *best = nullptr;
        int best_len = 2;
        unsigned char *next_src = pSrc + 1;
        if (list != nullptr) {
            do {
                int match = Compare(list->pData, pSrc, max_match);
                if (best_len < match && (match < 0x23 || offset - list->Offset < 0x10 || best_len < 0x23)) {
                    best = list;
                    best_len = match;
                }
                list = list->pNext;
            } while (list != nullptr && best_len < 0x1002);
        }
        if (best_len < 3) {
            pool.Update(pSrc, offset);
            flags1 = flags1 & 0x7f7f;
            remaining = remaining - 1;
            *out = *pSrc;
            out = out + 1;
            offset = offset + 1;
        } else {
            int dist = (offset - best->Offset) - 1;
            unsigned char byte0;
            if (dist < 0x10) {
                flags2 = flags2 >> 1;
                out[1] = static_cast<unsigned char>(best_len - 3);
                byte0 = static_cast<unsigned char>((best_len - 3 >> 8) << 4) | static_cast<unsigned char>(dist);
            } else {
                if (best_len > 0x22) {
                    best_len = 0x22;
                }
                dist = (offset - best->Offset) - 0x11;
                out[1] = static_cast<unsigned char>(dist);
                byte0 = static_cast<unsigned char>((dist >> 8) << 5) | static_cast<unsigned char>(best_len - 3);
                flags2 = (flags2 >> 1) & 0x7f7f;
            }
            *out = byte0;
            out = out + 2;
            remaining = remaining - best_len;
            do {
                next_src = pSrc + 1;
                pool.Update(pSrc, offset);
                offset = offset + 1;
                best_len = best_len - 1;
                pSrc = next_src;
            } while (best_len != 0);
        }
        unsigned char *out2 = out;
        if (flags1 < 0x100) {
            *flag1_ptr = static_cast<unsigned char>(flags1);
            flags1 = 0xff00;
            out2 = out + 1;
            flag1_ptr = out;
        }
        out = out2;
        if (flags2 < 0x100) {
            *flag2_ptr = static_cast<unsigned char>(flags2);
            flags2 = 0xff00;
            out = out2 + 1;
            flag2_ptr = out2;
        }
        pSrc = next_src;
        if ((count & 0x1fff) == 0) {
            int p = 10 - (remaining * 10) / SrcSize;
            if (p != progress) {
                progress = p;
            }
        }
    }
    for (; (flags1 & 0xff00) != 0; flags1 = flags1 >> 1) {
    }
    *flag1_ptr = static_cast<unsigned char>(flags1);
    for (; (flags2 & 0xff00) != 0; flags2 = flags2 >> 1) {
    }
    *flag2_ptr = static_cast<unsigned char>(flags2);
    header[3] = reinterpret_cast<int>(out) - reinterpret_cast<int>(header);
    return reinterpret_cast<int>(out) - reinterpret_cast<int>(header);
}

int JLZDecompress(unsigned char *pSrc, unsigned char *pDst) {
    int *header = reinterpret_cast<int *>(pSrc);
    int remaining = header[3];
    if (header[0] != 0x5a4c444a) {
        return 0;
    }
    if (*reinterpret_cast<unsigned char *>(&header[1]) != 2) {
        return 0;
    }
    remaining -= 0x12;
    unsigned char *src = pSrc + 0x12;
    unsigned char *end = pDst + header[2];
    unsigned int flags1 = *(pSrc + 0x10) | 0x100;
    unsigned int flags2 = *(pSrc + 0x11) | 0x100;
    while (remaining != 0) {
        if ((flags1 & 1) != 0) {
            int len;
            if ((flags2 & 1) != 0) {
                len = ((*src & 0xf0) << 4 | static_cast<unsigned int>(src[1])) + 3;
                ShortMove(pDst, pDst - ((*src & 0xf) + 1), len);
            } else {
                len = (*src & 0x1f) + 3;
                ShortMove(pDst, pDst - (((*src & 0xe0) << 3 | static_cast<unsigned int>(src[1])) + 0x11), len);
            }
            pDst = pDst + len;
            src = src + 2;
            remaining = remaining - 2;
            flags2 = static_cast<int>(flags2) >> 1;
        } else {
            if (pDst < end) {
                *pDst = *src;
                src = src + 1;
                pDst = pDst + 1;
            }
            remaining = remaining - 1;
        }
        flags1 = static_cast<int>(flags1) >> 1;
        if (flags1 == 1) {
            unsigned char b = *src;
            remaining = remaining - 1;
            src = src + 1;
            flags1 = b | 0x100;
        }
        if (flags2 == 1) {
            unsigned char b = *src;
            remaining = remaining - 1;
            src = src + 1;
            flags2 = b | 0x100;
        }
    }
    return header[2];
}

int OldLZDecompress(unsigned char *pSrc, unsigned char *pDst) {
    int *header = reinterpret_cast<int *>(pSrc);
    if (header == nullptr || pDst == nullptr) {
        return 0;
    }
    int packed_size = header[3];
    if (header[0] != 0x504d4f43) {
        return 0;
    }
    unsigned char *src_end = pSrc + packed_size;
    unsigned char *src = reinterpret_cast<unsigned char *>(&header[4]);
    unsigned char *threshold = src_end - 0x20;
    unsigned char *dst = pDst;
    unsigned int flags = 1;
    if (*reinterpret_cast<unsigned short *>(pSrc + 6) == 1) {
        packed_size -= 0x10;
        memcpy(pDst, src, packed_size);
        return packed_size;
    }
    while (src != src_end) {
        if (flags == 1) {
            unsigned char b0 = *src;
            unsigned char b1 = *(src + 1);
            src += 2;
            flags = b0 | 0x10000 | static_cast<unsigned int>(b1) << 8;
        }
        int count = 1;
        if (src <= threshold) {
            count = 0x10;
        }
        int i = count - 1;
        if (count != 0) {
            do {
                if ((flags & 1) != 0) {
                    unsigned char byte0 = *src;
                    unsigned char byte1 = *(src + 1);
                    src += 2;
                    unsigned char *ref = dst - (((byte0 & 0xf0) << 4) | static_cast<unsigned int>(byte1));
                    unsigned int extra = byte0 & 0xf;
                    *dst = *ref;
                    ref++; dst++;
                    *dst = *ref;
                    ref++; dst++;
                    *dst = *ref;
                    ref++; dst++;
                    if (extra != 0) {
                        do {
                            *dst = *ref;
                            ref++;
                            dst++;
                            extra--;
                        } while (extra != 0);
                    }
                } else {
                    *dst = *src;
                    src++;
                    dst++;
                }
                flags >>= 1;
                i--;
            } while (i != -1);
        }
    }
    return static_cast<int>(dst - pDst);
}

void LZByteSwapHeader(LZHeader *header) {
    bEndianSwap32(&header->ID);
    bEndianSwap16(&header->Flags);
    bEndianSwap32(&header->UncompressedSize);
    bEndianSwap32(&header->CompressedSize);
}

int LZValidHeader(LZHeader *header) {
    switch (header->ID) {
    case 0x504d4f43:
    case 0x46465548:
    case 0x57574152:
        return header->Version == 1;
    case 0x5a4c444a:
        return header->Version == 2;
    default:
        return false;
    }
}

int LZCompress(unsigned char *pSrc, unsigned int SrcSize, unsigned char *pDst) {
    int huffSize = HUFFCompress(pSrc, SrcSize, pDst);
    int jlzSize = JLZCompress(pSrc, SrcSize, pDst);
    if (huffSize > jlzSize) {
        return jlzSize;
    }
    if (huffSize > SrcSize + 0x10) {
        return RAWCompress(pSrc, SrcSize, pDst);
    }
    return HUFFCompress(pSrc, SrcSize, pDst);
}

int HUFFDecompress(unsigned char *pSrc, unsigned char *pDst);
int RAWDecompress(unsigned char *pSrc, unsigned char *pDst);

unsigned int LZDecompress(uint8 *pSrc, uint8 *pDst) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pSrc);
    switch (header->ID) {
    case 0x5a4c444a:
        return JLZDecompress(pSrc, pDst);
    case 0x504d4f43:
        return OldLZDecompress(pSrc, pDst);
    case 0x46465548:
        return HUFFDecompress(pSrc, pDst);
    case 0x57574152:
        return RAWDecompress(pSrc, pDst);
    default:
        return 0;
    }
}

int RAWCompress(unsigned char *pSrc, int SrcSize, unsigned char *pDst) {
    unsigned int *header = reinterpret_cast<unsigned int *>(pDst);
    header[0] = 0x57574152;
    *reinterpret_cast<unsigned char *>(&header[1]) = 1;
    *(reinterpret_cast<unsigned char *>(header) + 5) = 0x10;
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(header) + 6) = 0;
    header[2] = SrcSize;
    header[3] = SrcSize;
    memcpy(&header[4], pSrc, SrcSize);
    return header[3] + 0x10;
}

int RAWDecompress(unsigned char *pSrc, unsigned char *pDst) {
    int *header = reinterpret_cast<int *>(pSrc);
    if (header[0] != 0x57574152) goto done;
    if (*reinterpret_cast<char *>(&header[1]) == '\x01') goto decode;
done:
    return 0;
decode:
    memcpy(pDst, &header[4], header[2]);
    return header[2];
}

int HUFF_encode(void *dest, const void *src, int size);

int HUFFDecompress(unsigned char *pSrc, unsigned char *pDst) {
    int *header = reinterpret_cast<int *>(pSrc);
    if (header[0] != 0x46465548) goto done;
    if (*reinterpret_cast<char *>(&header[1]) == '\x01') goto decode;
done:
    return 0;
decode:
    HUFF_decode(pDst, &header[4]);
    return header[2];
}

int HUFFCompress(unsigned char *pSrc, int SrcSize, unsigned char *pDst) {
    unsigned int *header = reinterpret_cast<unsigned int *>(pDst);
    header[0] = 0x46465548;
    *reinterpret_cast<unsigned char *>(&header[1]) = 1;
    *(reinterpret_cast<unsigned char *>(header) + 5) = 0x10;
    *reinterpret_cast<unsigned short *>(reinterpret_cast<unsigned char *>(header) + 6) = 0;
    header[2] = SrcSize;
    int compressed = HUFF_encode(&header[4], pSrc, SrcSize);
    header[3] = compressed;
    return compressed + 0x10;
}

// total size: 0x31EC
struct HuffEncodeContext {
    char qleapcode[256];
    unsigned int count[768];
    unsigned int bitnum[17];
    unsigned int repbits[252];
    unsigned int repbase[252];
    unsigned int tree_left[520];
    unsigned int tree_right[520];
    unsigned int bitsarray[256];
    unsigned int patternarray[256];
    unsigned int masks[17];
    unsigned int packbits;
    unsigned int workpattern;
    unsigned char *buffer;
    unsigned char *bufptr;
    int flen;
    unsigned int csum;
    unsigned int mostbits;
    unsigned int codes;
    unsigned int chainused;
    unsigned int clue;
    unsigned int dclue;
    unsigned int clues;
    unsigned int dclues;
    int mindelta;
    int maxdelta;
    unsigned int plen;
    unsigned int ulen;
    unsigned int sortptr[256];
};

// total size: 0x8
struct HUFFMemStruct {
    char *ptr;
    int len;
};

static void HUFF_writebits(HuffEncodeContext *ctx, HUFFMemStruct *mem, unsigned int pattern, unsigned int bits) {
    if (bits < 0x11) {
        unsigned int total = ctx->packbits + bits;
        ctx->packbits = total;
        ctx->workpattern = ctx->workpattern + ((pattern & ctx->masks[bits]) << (0x18 - total));
        while (total > 7) {
            mem->ptr[mem->len] = static_cast<char>(*reinterpret_cast<unsigned short *>(&ctx->workpattern));
            mem->len = mem->len + 1;
            total = ctx->packbits - 8;
            ctx->workpattern = ctx->workpattern << 8;
            ctx->plen = ctx->plen + 1;
            ctx->packbits = total;
        }
    } else {
        HUFF_writebits(ctx, mem, pattern >> 0x10, bits - 0x10);
        HUFF_writebits(ctx, mem, pattern, 0x10);
    }
}

static void HUFF_treechase(HuffEncodeContext *ctx, unsigned int node, unsigned int depth) {
    if (node < 0x100) {
        ctx->bitsarray[node] = depth;
    } else {
        HUFF_treechase(ctx, ctx->tree_left[node], depth + 1);
        HUFF_treechase(ctx, ctx->tree_right[node], depth + 1);
    }
}

static void HUFF_maketree(HuffEncodeContext *ctx) {
    unsigned int freq[258];
    unsigned int index[259];

    freq[0] = 0;
    unsigned int j = 0;
    unsigned int n = 1;
    do {
        ctx->bitsarray[j] = 99;
        unsigned int c = ctx->count[j];
        unsigned int next = n;
        if (c != 0) {
            freq[n] = c;
            next = n + 1;
            index[n] = j;
        }
        j = j + 1;
        n = next;
    } while (j < 0x100);
    ctx->codes = n - 1;
    unsigned int treeNode = 0x100;
    if (n < 3) {
        HUFF_treechase(ctx, index[n - 1], 1);
    } else {
        unsigned int top;
        do {
            unsigned int prevTree = treeNode;
            top = n - 1;
            n = n - 2;
            unsigned int topFreq = freq[top];
            unsigned int topIdx = top;
            unsigned int secFreq = freq[n];
            unsigned int secIdx = n;
            if (topFreq < freq[n]) {
                topFreq = freq[n];
                topIdx = n;
                secFreq = freq[top];
                secIdx = top;
            }
            unsigned int smallest = secFreq;
            if (n != 0) {
                while (true) {
                    secFreq = smallest;
                    n = n - 1;
                    smallest = freq[n];
                    while (topFreq < smallest) {
                        n = n - 1;
                        smallest = freq[n];
                    }
                    if (n == 0) break;
                    topIdx = n;
                    smallest = secFreq;
                    topFreq = freq[n];
                    if (freq[n] <= secFreq) {
                        topIdx = secIdx;
                        smallest = freq[n];
                        topFreq = secFreq;
                        secIdx = n;
                    }
                }
            }
            unsigned int leftNode = index[topIdx];
            unsigned int rightNode = index[secIdx];
            freq[topIdx] = topFreq + secFreq;
            index[topIdx] = prevTree;
            ctx->tree_left[prevTree] = leftNode;
            unsigned int savedFreq = freq[top];
            ctx->tree_right[prevTree] = rightNode;
            freq[secIdx] = savedFreq;
            index[secIdx] = index[top];
            n = top;
            treeNode = prevTree + 1;
        } while (top > 2);
        HUFF_treechase(ctx, treeNode - 1, 0);
    }
}

static void HUFF_writenum(HuffEncodeContext *ctx, HUFFMemStruct *mem, unsigned int value) {
    int bits;
    int base;
    if (value < 0xfc) {
        bits = ctx->repbits[value];
        base = ctx->repbase[value];
    } else if (value < 0x1fc) { bits = 6; base = 0xfc; }
    else if (value < 0x3fc) { bits = 7; base = 0x1fc; }
    else if (value < 0x7fc) { bits = 8; base = 0x3fc; }
    else if (value < 0xffc) { bits = 9; base = 0x7fc; }
    else if (value < 0x1ffc) { bits = 10; base = 0xffc; }
    else if (value < 0x3ffc) { bits = 0xb; base = 0x1ffc; }
    else if (value < 0x7ffc) { bits = 0xc; base = 0x3ffc; }
    else if (value < 0xfffc) { bits = 0xd; base = 0x7ffc; }
    else if (value < 0x1fffc) { bits = 0xe; base = 0xfffc; }
    else if (value < 0x3fffc) { bits = 0xf; base = 0x1fffc; }
    else if (value < 0x80000) { bits = 0x10; base = 0x3fffc; }
    else if (value < 0x100000) { bits = 0x11; base = 0x80000; }
    else { bits = 0x12; base = 0x100000; }
    HUFF_writebits(ctx, mem, 1, bits + 1);
    HUFF_writebits(ctx, mem, value - base, bits + 2);
}

static void HUFF_writeexp(HuffEncodeContext *ctx, HUFFMemStruct *mem, unsigned int value) {
    int idx = ctx->clue * 4;
    HUFF_writebits(ctx, mem, ctx->patternarray[ctx->clue], ctx->bitsarray[ctx->clue]);
    HUFF_writenum(ctx, mem, 0);
    HUFF_writebits(ctx, mem, value, 9);
}

static void HUFF_writecode(HuffEncodeContext *ctx, HUFFMemStruct *mem, unsigned int code) {
    if (static_cast<int>(code) == static_cast<int>(ctx->clue)) {
        HUFF_writeexp(ctx, mem, code);
    } else {
        HUFF_writebits(ctx, mem, ctx->patternarray[code], ctx->bitsarray[code]);
    }
}

static void HUFF_init(HuffEncodeContext *ctx) {
    unsigned int i = 0;
    do {
        ctx->repbits[i] = 0;
        ctx->repbase[i] = 0;
        i = i + 1;
    } while (i < 4);
    for (; i < 0xc; i = i + 1) {
        ctx->repbits[i] = 1;
        ctx->repbase[i] = 4;
    }
    for (; i < 0x1c; i = i + 1) {
        ctx->repbits[i] = 2;
        ctx->repbase[i] = 0xc;
    }
    for (; i < 0x3c; i = i + 1) {
        ctx->repbits[i] = 3;
        ctx->repbase[i] = 0x1c;
    }
    for (; i < 0x7c; i = i + 1) {
        ctx->repbits[i] = 4;
        ctx->repbase[i] = 0x3c;
    }
    if (i < 0xfc) {
        do {
            ctx->repbits[i] = 5;
            ctx->repbase[i] = 0x7c;
            i = i + 1;
        } while (i < 0xfc);
    }
}

static int HUFF_minrep(HuffEncodeContext *ctx, unsigned int value, unsigned int level) {
    int result;
    if (level == 0) {
        result = 0;
        if (value != 0) {
            result = 0x14;
            if (value < 0xfc) {
                result = ctx->bitsarray[ctx->clue] + ctx->repbits[value] * 2 + 3;
            }
        }
    } else {
        result = HUFF_minrep(ctx, value, level - 1);
        if (ctx->count[ctx->clue + level] != 0) {
            int alt = HUFF_minrep(ctx, value - (value / level) * level, level - 1);
            alt = alt + ctx->bitsarray[ctx->clue + level] * static_cast<int>(value / level);
            if (alt < result) {
                result = alt;
            }
        }
    }
    return result;
}

static void HUFF_analysis(HuffEncodeContext *EC, unsigned int opt, unsigned int chainsaw);
static void HUFF_pack(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int opt);

static int HUFF_packfile(HuffEncodeContext *ctx, HUFFMemStruct *src, HUFFMemStruct *dst, int uncompressedSize) {
    ctx->masks[0] = 0;
    ctx->packbits = 0;
    ctx->workpattern = 0;
    unsigned int i = 1;
    do {
        int prev = i - 1;
        ctx->masks[i] = ctx->masks[prev] * 2 + 1;
        i = i + 1;
    } while (i < 0x11);
    HUFF_init(ctx);
    int bufStart = *reinterpret_cast<int *>(&src->ptr);
    ctx->buffer = reinterpret_cast<unsigned char *>(bufStart);
    int bufLen = src->len;
    ctx->flen = bufLen;
    ctx->ulen = bufLen;
    ctx->bufptr = reinterpret_cast<unsigned char *>(bufStart + bufLen);
    dst->len = 0;
    ctx->plen = 0;
    ctx->packbits = 0;
    ctx->workpattern = 0;
    HUFF_analysis(ctx, 0x39, 0xf);
    if (uncompressedSize < 0x1000000) {
        if (uncompressedSize == src->len) {
            HUFF_writebits(ctx, dst, 0x30fb, 0x10);
            HUFF_writebits(ctx, dst, src->len, 0x18);
        } else {
            HUFF_writebits(ctx, dst, 0x31fb, 0x10);
            HUFF_writebits(ctx, dst, uncompressedSize, 0x18);
            HUFF_writebits(ctx, dst, src->len, 0x18);
        }
    } else {
        if (uncompressedSize == src->len) {
            HUFF_writebits(ctx, dst, 0xb0fb, 0x10);
            HUFF_writebits(ctx, dst, uncompressedSize, 0x20);
        } else {
            HUFF_writebits(ctx, dst, 0xb1fb, 0x10);
            HUFF_writebits(ctx, dst, uncompressedSize, 0x20);
        }
        HUFF_writebits(ctx, dst, src->len, 0x20);
    }
    HUFF_pack(ctx, dst, 0x39);
    return dst->len;
}

int HUFF_encode(void *dest, const void *src, int size) {
    int result = 0;
    HuffEncodeContext *ctx = static_cast<HuffEncodeContext *>(bMalloc(0x31ec, 0));
    if (ctx != nullptr) {
        HUFFMemStruct srcMem;
        srcMem.ptr = const_cast<char *>(static_cast<const char *>(src));
        srcMem.len = size;
        HUFFMemStruct dstMem;
        dstMem.ptr = static_cast<char *>(dest);
        dstMem.len = size;
        result = HUFF_packfile(ctx, &srcMem, &dstMem, size);
        bFree(ctx);
    }
    return result;
}

static void HUFF_analysis(HuffEncodeContext *EC, unsigned int opt, unsigned int chainsaw) {
    unsigned int count2[256];
    unsigned int dcount[256];

    unsigned int i = 0;
    do {
        EC->count[i] = 0;
        i = i + 1;
    } while (i < 0x300);

    unsigned char *bptr1 = EC->buffer;
    unsigned char *bptr2 = EC->bufptr;
    unsigned int i1 = 0x100;
    int di = 0;
    unsigned int thres = ~opt;

    if (bptr1 < bptr2) {
        unsigned char *nextBptr;
        do {
            unsigned int i2 = *bptr1;
            unsigned int i3 = i1 - 0x100;
            nextBptr = bptr1 + 1;
            di = di + i2;
            if (i2 == i1) {
                unsigned int repn = 0;
                unsigned char *limit = bptr1 + 0x7531;
                if (bptr2 < limit) {
                    limit = bptr2;
                }
                do {
                    if (limit <= nextBptr) break;
                    i2 = *nextBptr;
                    repn = repn + 1;
                    nextBptr = nextBptr + 1;
                    di = di + i2;
                } while (i2 == i1);
                if (repn < 0xff) {
                    EC->count[repn + 0x200] = EC->count[repn + 0x200] + 1;
                } else {
                    EC->count[0x200] = EC->count[0x200] + 1;
                }
            }
            i1 = i2;
            unsigned int deltaIdx = ((i1 - i3) * 4 & 0x7fc | 0x400) >> 2;
            EC->count[i1] = EC->count[i1] + 1;
            EC->count[deltaIdx] = EC->count[deltaIdx] + 1;
            bptr2 = EC->bufptr;
            bptr1 = nextBptr;
        } while (nextBptr < bptr2);
    }
    EC->csum = di;
    if (EC->count[0x200] == 0) {
        EC->count[0x200] = 1;
    }

    unsigned int smallest = 0;
    EC->dclues = 0;
    unsigned int i2 = 0;
    EC->clues = 0;
    do {
        unsigned int cval = EC->count[i2];
        unsigned int repn = 0;
        unsigned int ncode = i2;
        if (cval < EC->count[smallest]) {
            smallest = i2;
        }
        while (cval == 0 && ncode < 0x100) {
            repn = repn + 1;
            cval = EC->count[ncode + 1];
            ncode = ncode + 1;
        }
        if (EC->dclues <= repn) {
            EC->dclue = i2;
            EC->dclues = repn;
            if (EC->clues <= repn) {
                EC->dclues = EC->clues;
                EC->dclue = EC->clue;
                EC->clue = i2;
                EC->clues = repn;
            }
        }
        i2 = ncode + 1;
    } while (i2 < 0x100);

    if ((opt & 0x20) != 0 && EC->clues == 0) {
        EC->clue = smallest;
        EC->clues = 1;
    }
    if ((thres & 2) != 0) {
        if (EC->clues > 1) {
            EC->clues = 1;
        }
        if ((thres & 1) != 0) {
            EC->clues = 0;
        }
    }
    if ((thres & 4) == 0) {
        if (EC->dclues > 10) {
            unsigned int tmp = EC->clue;
            unsigned int tmpClues = EC->clues;
            EC->clues = EC->dclues;
            EC->clue = EC->dclue;
            EC->dclue = tmp;
            EC->dclues = tmpClues;
        }
        unsigned int dcluesVal = EC->dclues;
        if (static_cast<unsigned int>(EC->clues << 2) < dcluesVal) {
            int adj = dcluesVal - (dcluesVal >> 2);
            EC->clues = dcluesVal >> 2;
            EC->dclues = adj;
            EC->clue = EC->dclue + adj;
        }
    } else {
        EC->dclues = 0;
    }

    int dcluesVal = EC->dclues;
    if (dcluesVal != 0) {
        unsigned int i2val = 1;
        int halfDclues = -(dcluesVal / 2);
        EC->mindelta = halfDclues;
        unsigned int threshold = EC->ulen / 0x19;
        EC->maxdelta = dcluesVal + halfDclues;
        if (EC->maxdelta != 0) {
            do {
                unsigned int dcountVal = EC->count[i2val + 0x100];
                if (threshold < dcountVal) {
                    EC->count[(EC->dclue + (i2val - 1) * 2)] = dcountVal;
                }
                i2val = i2val + 1;
            } while (static_cast<int>(i2val) <= EC->maxdelta);
        }
        i2val = 1;
        if (EC->mindelta != 0) {
            do {
                unsigned int dcountVal = EC->count[0x200 - i2val];
                if (threshold < dcountVal) {
                    EC->count[(EC->dclue + (i2val - 1) * 2 + 1)] = dcountVal;
                }
                i2val = i2val + 1;
            } while (static_cast<int>(i2val) <= static_cast<int>(-EC->mindelta));
        }

        i2val = 0;
        unsigned int lastUsed = 0;
        if (EC->dclues != 0) {
            do {
                if (EC->count[EC->dclue + i2val] != 0) {
                    lastUsed = i2val;
                }
                i2val = i2val + 1;
            } while (i2val < EC->dclues);
        }
        int trimCount = (EC->dclues - lastUsed) + -1;
        dcluesVal = EC->dclues - trimCount;
        EC->dclues = dcluesVal;
        if (static_cast<int>(EC->clue) == static_cast<int>(EC->dclue) + dcluesVal + trimCount) {
            EC->clue = EC->clue - trimCount;
            EC->clues = EC->clues + trimCount;
        }
        int newHalf = -(static_cast<int>(EC->dclues) / 2);
        EC->mindelta = newHalf;
        EC->maxdelta = EC->dclues + newHalf;
    }

    if (EC->clues != 0 && EC->clues != 0) {
        i2 = 0;
        do {
            int srcIdx = i2 + 0x200;
            int dstIdx = EC->clue + i2;
            i2 = i2 + 1;
            EC->count[dstIdx] = EC->count[srcIdx];
        } while (i2 < EC->clues);
    }
    HUFF_maketree(EC);

    if (EC->clues > 1 && EC->clues > 1) {
        i2 = 1;
        do {
            unsigned int ncode = i2 - 1;
            if (ncode > 8) {
                ncode = 8;
            }
            if (EC->count[EC->clue + i2] != 0) {
                ncode = HUFF_minrep(EC, i2, ncode);
                int idx = (EC->clue + i2);
                if (ncode <= EC->bitsarray[idx] ||
                    static_cast<int>(EC->count[idx]) * static_cast<int>(ncode - EC->bitsarray[idx]) < static_cast<int>(i2 >> 1)) {
                    EC->count[idx] = 0;
                }
            }
            i2 = i2 + 1;
        } while (i2 < EC->clues);
    }

    i2 = 0;
    unsigned int next;
    do {
        unsigned int cval = EC->count[i2];
        EC->count[i2] = 0;
        count2[i2] = cval;
        EC->count[i2 + 0x100] = 0;
        next = i2 + 1;
        EC->count[i2 + 0x200] = 0;
        dcount[i2] = 0;
        i2 = next;
    } while (next < 0x100);

    bptr1 = EC->bufptr;
    if (EC->buffer < bptr1) {
        bptr2 = EC->buffer;
        i1 = 0x100;
        unsigned char *nextBptr;
        do {
            i2 = *bptr2;
            nextBptr = bptr2 + 1;
            if (i2 == i1) {
                unsigned int repn = 0;
                unsigned char *limit = bptr2 + 0x7531;
                if (bptr1 < limit) {
                    limit = bptr1;
                }
                unsigned int cluesVal = EC->clues;
                int iVar19 = i2 * 4;
                do {
                    if (limit <= nextBptr) break;
                    i2 = *nextBptr;
                    repn = repn + 1;
                    nextBptr = nextBptr + 1;
                } while (i2 == i1);
                unsigned int directCost = repn * EC->bitsarray[iVar19 >> 2];
                unsigned int clueOneCost = 32000;
                unsigned int clueMultiCost = 32000;
                if (cluesVal != 0) {
                    int clueIdx = EC->clue;
                    if (count2[clueIdx] != 0 && (clueOneCost = 0x14, repn < 0xfc)) {
                        clueOneCost = EC->bitsarray[clueIdx] + EC->repbits[repn] * 2 + 3;
                    }
                    if (cluesVal > 1) {
                        clueMultiCost = 0;
                        unsigned int remaining = repn;
                        unsigned int level = cluesVal;
                        while (level = level - 1, level != 0) {
                            if (count2[clueIdx + level] != 0) {
                                unsigned int quotient = remaining / level;
                                remaining = remaining - quotient * level;
                                clueMultiCost = clueMultiCost + quotient * EC->bitsarray[clueIdx + level];
                            }
                        }
                        smallest = 0;
                        if (remaining != 0) {
                            clueMultiCost = 32000;
                        }
                    }
                }
                if (clueOneCost < directCost || clueMultiCost < directCost) {
                    unsigned int repnCopy = repn;
                    if (clueOneCost < clueMultiCost) {
                        int idx = EC->clue;
                        EC->count[idx] = EC->count[idx] + 1;
                    } else {
                        while (smallest = cluesVal - 1, smallest != 0) {
                            int idx = EC->clue + smallest;
                            cluesVal = smallest;
                            if (count2[idx] != 0) {
                                EC->count[idx] = EC->count[idx] + repnCopy / smallest;
                                repnCopy = repnCopy - (repnCopy / smallest) * smallest;
                            }
                        }
                    }
                } else {
                    EC->count[iVar19 >> 2] = EC->count[iVar19 >> 2] + repn;
                }
            }
            unsigned int ncode = i2;
            if (EC->dclues != 0) {
                unsigned int delta = i2 - i1;
                unsigned int dIdx = 0;
                unsigned int pattern = 0;
                if (static_cast<int>(delta) <= EC->maxdelta && EC->mindelta <= static_cast<int>(delta)) {
                    dIdx = (delta - 1) * 2 + EC->dclue;
                    if (i2 < i1) {
                        dIdx = (i1 - i2 - 1) * 2 + EC->dclue + 1;
                    }
                    if (thres < count2[dIdx]) {
                        pattern = static_cast<unsigned int>(count2[i2] < 4);
                        unsigned int dBits = EC->bitsarray[dIdx];
                        unsigned int iBits = EC->bitsarray[i2];
                        if (dBits < iBits) {
                            pattern = pattern + 1;
                        }
                        if (dBits == iBits && EC->count[i2] < EC->count[dIdx]) {
                            pattern = pattern + 1;
                        }
                    }
                }
                if (pattern != 0) {
                    ncode = dIdx;
                }
            }
            EC->count[ncode] = EC->count[ncode] + 1;
            bptr1 = EC->bufptr;
            bptr2 = nextBptr;
            i1 = i2;
        } while (nextBptr < bptr1);
    }
    if ((opt & 0x20) != 0) {
        int idx = EC->clue;
        EC->count[idx] = EC->count[idx] + 1;
    }
    HUFF_maketree(EC);
    EC->chainused = 0;

    while (chainsaw < 99) {
        unsigned int maxBits = 0;
        i2 = 0;
        unsigned int prevMax = 0;
        unsigned int maxIdx = 0;
        do {
            if (EC->count[i2] != 0) {
                unsigned int bits = EC->bitsarray[i2];
                if (maxBits <= bits) {
                    maxBits = bits;
                    prevMax = maxIdx;
                    maxIdx = i2;
                }
            }
            i2 = i2 + 1;
        } while (i2 < 0x100);
        if (maxBits <= chainsaw) break;

        unsigned int ncode = 0;
        unsigned int nbits = EC->bitsarray[0];
        if (EC->count[0] == 0) goto find_next_nonzero;
        while (chainsaw <= nbits) {
find_next_nonzero:
            do {
                ncode = ncode + 1;
                if (ncode > 0xff) goto find_best_below;
            } while (EC->count[ncode] == 0);
            nbits = EC->bitsarray[ncode];
        }
find_best_below:
        for (; ncode < 0x100; ncode = ncode + 1) {
            if (EC->count[ncode] != 0) {
                unsigned int curBits = EC->bitsarray[ncode];
                if (curBits < chainsaw && EC->bitsarray[i2] < curBits) {
                    i2 = ncode;
                }
            }
        }
        int newBits = EC->bitsarray[i2] + 1;
        EC->bitsarray[i2] = newBits;
        EC->bitsarray[maxIdx] = newBits;
        EC->bitsarray[prevMax] = EC->bitsarray[prevMax] - 1;
        EC->chainused = chainsaw;
    }

    if ((thres & 8) != 0) {
        unsigned int idx = 0;
        do {
            EC->bitsarray[idx] = 8;
            idx = idx + 1;
        } while (idx < 0x100);
    }

    i2 = 0;
    do {
        EC->bitnum[i2] = 0;
        i2 = i2 + 1;
    } while (i2 < 0x11);
    i2 = 0;
    do {
        unsigned int bits = EC->bitsarray[i2];
        if (bits < 0x11) {
            EC->bitnum[bits] = EC->bitnum[bits] + 1;
        }
        i2 = i2 + 1;
    } while (i2 < 0x100);

    int sortIdx = 0;
    unsigned int mostbits = 0;
    unsigned int bitLevel = 1;
    unsigned int nextLevel;
    do {
        nextLevel = bitLevel + 1;
        if (EC->bitnum[bitLevel] != 0) {
            i2 = 0;
            do {
                if (EC->bitsarray[i2] == bitLevel) {
                    EC->sortptr[sortIdx] = i2;
                    sortIdx = sortIdx + 1;
                }
                i2 = i2 + 1;
                mostbits = bitLevel;
            } while (i2 < 0x100);
        }
        bitLevel = nextLevel;
    } while (nextLevel < 0x11);

    i2 = 0;
    EC->mostbits = mostbits;
    int pattern = 0;
    unsigned int prevBits = 0;
    if (EC->codes != 0) {
        do {
            int symIdx = EC->sortptr[i2];
            i2 = i2 + 1;
            if (prevBits < EC->bitsarray[symIdx]) {
                do {
                    prevBits = prevBits + 1;
                    pattern = pattern << 1;
                } while (prevBits < EC->bitsarray[symIdx]);
            }
            EC->patternarray[symIdx] = pattern;
            pattern = pattern + 1;
        } while (i2 < EC->codes);
    }
}

static void HUFF_pack(HuffEncodeContext *EC, HUFFMemStruct *dest, unsigned int opt) {
    HUFF_writebits(EC, dest, EC->clue, 8);
    unsigned int i = 1;
    int curpc = 0;
    if (EC->mostbits != 0) {
        do {
            HUFF_writenum(EC, dest, EC->bitnum[i]);
            i = i + 1;
        } while (i <= EC->mostbits);
    }

    unsigned int ncode = 0;
    unsigned int i1 = 0xff;
    do {
        EC->qleapcode[ncode] = 0;
        ncode = ncode + 1;
    } while (ncode < 0x100);

    unsigned int sortI = 0;
    ncode = 0xff;
    if (EC->codes != 0) {
        do {
            unsigned int symIdx = EC->sortptr[sortI];
            sortI = sortI + 1;
            int gap = -1;
            do {
                ncode = (ncode + 1) & 0xff;
                if (EC->qleapcode[ncode] == '\0') {
                    gap = gap + 1;
                }
            } while (symIdx != ncode);
            EC->qleapcode[symIdx] = 1;
            HUFF_writenum(EC, dest, gap);
        } while (sortI < EC->codes);
    }

    if (EC->clues == 0) {
        EC->clue = 32000;
    }

    unsigned char *bptr2 = EC->bufptr;
    if (EC->buffer < bptr2) {
        i1 = 0x100;
        unsigned char *bptr1 = EC->buffer;
        unsigned char *nextBptr;
        do {
            ncode = *bptr1;
            nextBptr = bptr1 + 1;
            if (ncode == i1) {
                unsigned int repn = 0;
                unsigned char *limit = bptr1 + 0x7531;
                if (bptr2 < limit) {
                    limit = bptr2;
                }
                unsigned int cluesVal = EC->clues;
                int iVar5 = ncode * 4;
                do {
                    if (limit <= nextBptr) break;
                    ncode = *nextBptr;
                    repn = repn + 1;
                    nextBptr = nextBptr + 1;
                } while (ncode == i1);
                unsigned int directCost = repn * EC->bitsarray[iVar5 >> 2];
                unsigned int clueOneCost = 32000;
                unsigned int clueMultiCost = 32000;
                if (cluesVal != 0) {
                    int clueIdx = EC->clue * 4;
                    if (EC->count[clueIdx >> 2] != 0 && (clueOneCost = 0x14, repn < 0xfc)) {
                        clueOneCost = EC->bitsarray[clueIdx >> 2] + EC->repbits[repn] * 2 + 3;
                    }
                    if (cluesVal > 1) {
                        clueMultiCost = 0;
                        unsigned int remaining = repn;
                        unsigned int level = cluesVal;
                        while (level = level - 1, level != 0) {
                            int idx = (EC->clue + level);
                            if (EC->count[idx] != 0) {
                                unsigned int quotient = remaining / level;
                                remaining = remaining - quotient * level;
                                clueMultiCost = clueMultiCost + quotient * EC->bitsarray[idx];
                            }
                        }
                        if (remaining != 0) {
                            clueMultiCost = 32000;
                        }
                    }
                }
                if (clueOneCost < directCost || clueMultiCost < directCost) {
                    if (clueOneCost < clueMultiCost) {
                        int idx = EC->clue;
                        HUFF_writebits(EC, dest, EC->patternarray[idx], EC->bitsarray[idx]);
                        HUFF_writenum(EC, dest, repn);
                    } else if (cluesVal - 1 != 0) {
                        unsigned int level = cluesVal - 1;
                        unsigned int prevLevel;
                        do {
                            prevLevel = level - 1;
                            if (EC->count[EC->clue + level] != 0) {
                                unsigned int quotient = repn / level;
                                repn = repn - quotient * level;
                                for (; quotient != 0; quotient = quotient - 1) {
                                    HUFF_writecode(EC, dest, EC->clue + level);
                                }
                            }
                            level = prevLevel;
                        } while (prevLevel != 0);
                    }
                } else {
                    for (; repn != 0; repn = repn - 1) {
                        HUFF_writecode(EC, dest, i1);
                    }
                }
            }
            bool useDelta = false;
            if (EC->dclues != 0) {
                int delta = ncode - i1;
                if (delta <= EC->maxdelta && EC->mindelta <= delta) {
                    int dIdx = (delta - 1) * 2 + EC->dclue;
                    if (ncode < i1) {
                        dIdx = (i1 - ncode - 1) * 2 + EC->dclue + 1;
                    }
                    if (EC->bitsarray[dIdx] < EC->bitsarray[ncode]) {
                        useDelta = true;
                        HUFF_writebits(EC, dest, EC->patternarray[dIdx], EC->bitsarray[dIdx]);
                    }
                }
            }
            if (!useDelta) {
                HUFF_writecode(EC, dest, ncode);
            }
            int progress = reinterpret_cast<int>(nextBptr) - reinterpret_cast<int>(EC->buffer);
            if (EC->plen + curpc <= static_cast<unsigned int>(progress)) {
                curpc = progress - EC->plen;
            }
            bptr2 = EC->bufptr;
            i1 = ncode;
            bptr1 = nextBptr;
        } while (nextBptr < bptr2);
    }
    int clueIdx = EC->clue;
    HUFF_writebits(EC, dest, EC->patternarray[clueIdx], EC->bitsarray[clueIdx]);
    HUFF_writenum(EC, dest, 0);
    HUFF_writebits(EC, dest, 2, 2);
    HUFF_writebits(EC, dest, 0, 7);
}

static int ZERO;

#define HUFF_REFILL() \
    if (bitsleft < 0) { \
        bitsunshifted = (bitsunshifted << 8) | *qs; \
        bitsunshifted = (bitsunshifted << 8) | *(qs + 1); \
        bits = bitsunshifted << static_cast<unsigned int>(-bitsleft); \
        qs += 2; \
        bitsleft += 16; \
    }

#define HUFF_READNUM(dest) \
    if (static_cast<int>(bits) < 0) { \
        unsigned int t; \
        t = bits >> 29; \
        bitsleft -= 3; \
        bits <<= 3; \
        HUFF_REFILL() \
        dest = t - 4; \
    } else { \
        int n; \
        unsigned int v1; \
        if ((bits >> 16) == 0) { \
            n = 2; \
            do { \
                dest = -(static_cast<int>(bits) >> 31); \
                n++; \
                bits <<= 1; \
                bitsleft--; \
                HUFF_REFILL() \
            } while (dest == 0); \
        } else { \
            n = 2; \
            do { \
                v1 = bits; \
                n++; \
                bits = v1 << 1; \
            } while (static_cast<int>(bits) >= 0); \
            bitsleft = bitsleft + 1 - n; \
            bits = v1 << 2; \
            if (ZERO != 0) { \
                bitsleft -= ZERO; \
                dest = bits >> (32 - ZERO); \
                bits <<= ZERO; \
            } \
            HUFF_REFILL() \
        } \
        if (n < 17) { \
            if (n != 0) { \
                bitsleft -= n; \
                dest = bits >> (32 - n); \
                bits <<= n; \
            } \
            HUFF_REFILL() \
            dest = dest + (1 << n) - 4; \
        } else { \
            unsigned int _hi = bits >> (48 - n); \
            bits = bits << (n - 16); \
            bitsleft = bitsleft + 16 - n; \
            HUFF_REFILL() \
            unsigned int _mid = bits >> 16; \
            bitsleft -= 16; \
            bits <<= 16; \
            HUFF_REFILL() \
            dest = (_mid | (_hi << 16)) + (1 << n) - 4; \
        } \
    }

static int HUFF_decompress(unsigned char *packbuf, unsigned char *unpackbuf) {
    unsigned int type;
    unsigned char clue;
    int ulen = 0;
    unsigned int cmp;
    int bitnum = 0;
    int cluelen = 0;
    unsigned char *qs = packbuf;
    unsigned char *qd = unpackbuf;
    unsigned int bits = 0;
    unsigned int bitsunshifted = 0;
    int numbits;
    int bitsleft;
    unsigned int v = 0;

    if (qs != 0) {

    bitsleft = -16;
    if (ZERO != 0) {
        bitsleft = -16 - ZERO;
    }
    bits = 0;

    // Read type (16 bits)
    HUFF_REFILL()
    type = bits >> 16;
    bitsleft -= 16;
    bits <<= 16;
    HUFF_REFILL()

    // Read cmp/v based on type flags
    if (type & 0x8000) {
        if (type & 0x100) {
            bits <<= 16;
            bitsleft -= 16;
            HUFF_REFILL()
            bits <<= 16;
            bitsleft -= 16;
            HUFF_REFILL()
        }
        v = bits >> 16;
        type &= ~0x100u;
        bits <<= 16;
        bitsleft -= 16;
    } else {
        if (type & 0x100) {
            bits <<= 8;
            bitsleft -= 8;
            HUFF_REFILL()
            bits <<= 16;
            bitsleft -= 16;
            HUFF_REFILL()
        }
        v = bits >> 24;
        type &= ~0x100u;
        bits <<= 8;
        bitsleft -= 8;
    }
    HUFF_REFILL()

    // Read ulen (16 bits + upper from v)
    ulen = bits >> 16;
    bitsleft -= 16;
    bits <<= 16;
    HUFF_REFILL()
    ulen |= v << 16;

    // Read clue (8 bits)
    v = bits >> 24;
    bitsleft -= 8;
    bits <<= 8;
    HUFF_REFILL()
    clue = static_cast<unsigned char>(v);

    // Tree building
    {
        int mostbits;
        int i;
        int bitnumtbl[16];
        unsigned int deltatbl[16];
        unsigned int cmptbl[16];
        unsigned char codetbl[256];
        unsigned char quickcodetbl[256];
        unsigned char quicklentbl[256];

        int numchars = 0;
        int basecmp = 0;
        i = 1;
        do {
            mostbits = i;
            bitnumtbl[mostbits] = basecmp * 2 - numchars;

            HUFF_READNUM(bitnum)

            numchars += bitnum;
            deltatbl[mostbits] = bitnum;
            basecmp = basecmp * 2 + bitnum;
            cmp = 0;
            if (bitnum != 0) {
                cmp = (basecmp << (16 - mostbits)) & 0xffff;
            }
            cmptbl[mostbits] = cmp;
            i = mostbits + 1;
        } while (bitnum == 0 || cmp != 0);

        // Set sentinel
        cmptbl[mostbits] = 0xffffffff;

        // Code table building
        {
            signed char leap[256];
            unsigned char nextchar;

            // Zero leap array
            {
                int loopI = 16;
                int intval = 0;
                int *lptr = reinterpret_cast<int *>(leap);
                do {
                    lptr[0] = intval;
                    lptr[1] = intval;
                    lptr[2] = intval;
                    lptr[3] = intval;
                    lptr += 4;
                    loopI--;
                } while (loopI != 0);
            }

            // Read code values
            nextchar = 0xff;
            int charIdx = 0;
            if (numchars > 0) {
                do {
                    int leapdelta;
                    HUFF_READNUM(leapdelta)
                    int count = leapdelta - 3;
                    do {
                        unsigned int nc = nextchar + 1;
                        nextchar = nc & 0xff;
                        if (leap[nextchar] == 0) {
                            count--;
                        }
                    } while (count != 0);
                    leap[nextchar] = 1;
                    codetbl[charIdx] = nextchar;
                    charIdx++;
                } while (charIdx < numchars);
            }
        }

        // Fill quicklentbl with 0x40
        {
            int loopI = 16;
            int intval = 0x40404040;
            int *lptr = reinterpret_cast<int *>(quicklentbl);
            do {
                lptr[0] = intval;
                lptr[1] = intval;
                lptr[2] = intval;
                lptr[3] = intval;
                lptr += 4;
                loopI--;
            } while (loopI != 0);
        }

        // Fill quick decode tables
        {
            int bitsIdx = 1;
            int numbitentries;
            int nextcode;
            int nextlen;
            int idx;
            unsigned char *codeptr = codetbl;
            unsigned char *quickcodeptr = quickcodetbl;
            unsigned char *quicklenptr = quicklentbl;
            if (mostbits > 0) {
                numbitentries = deltatbl[1];
                do {
                    nextcode = 1 << (8 - bitsIdx);
                    nextlen = bitsIdx + 1;
                    idx = numbitentries - 1;
                    if (numbitentries != 0) {
                        do {
                            unsigned char code = *codeptr++;
                            unsigned int len = bitsIdx;
                            if (code == clue) {
                                cluelen = bitsIdx;
                                len = 0x60;
                            }
                            int j = 0;
                            idx--;
                            if (nextcode > 0) {
                                do {
                                    *quickcodeptr++ = code;
                                    *quicklenptr++ = static_cast<unsigned char>(len);
                                    j++;
                                } while (j < nextcode);
                            }
                        } while (idx != -1);
                    }
                } while (nextlen <= mostbits && (numbitentries = deltatbl[nextlen], bitsIdx = nextlen, nextlen < 9));
            }
        }

        // Main decode loop
        {
            unsigned char *quickcodeptr;
            unsigned char *quicklenptr;

        nextloop:
            unsigned int len = quicklentbl[bits >> 24];
            unsigned char *dst = qd;
            for (bitsleft = bitsleft - len; qd = dst, bitsleft > -1; bitsleft = bitsleft - len) {
                unsigned int idx = bits >> 24;
                bits <<= len;
                unsigned int idx2 = bits >> 24;
                *dst = quickcodetbl[idx];
                qd = dst + 1;
                len = quicklentbl[idx2];
                bitsleft -= len;
                if (bitsleft < 0) break;

                bits <<= len;
                idx = bits >> 24;
                *qd = quickcodetbl[idx2];
                qd = dst + 2;
                len = quicklentbl[idx];
                bitsleft -= len;
                if (bitsleft < 0) break;

                bits <<= len;
                idx2 = bits >> 24;
                *qd = quickcodetbl[idx];
                qd = dst + 3;
                len = quicklentbl[idx2];
                bitsleft -= len;
                if (bitsleft < 0) break;

                bits <<= len;
                *qd = quickcodetbl[idx2];
                dst = dst + 4;
                len = quicklentbl[bits >> 24];
            }

            // Refill after decode loop
            bitsleft += 16;
            if (bitsleft > -1) {
                *qd = quickcodetbl[bits >> 24];
                qd++;
                bitsunshifted = (bitsunshifted << 8) | *qs;
                bitsunshifted = (bitsunshifted << 8) | *(qs + 1);
                qs += 2;
                bits = bitsunshifted << static_cast<unsigned int>(16 - bitsleft);
                goto nextloop;
            }

            // Slow decode (len > 8 bits)
            {
                unsigned char code;
                unsigned int slowlen;
                if (len != 0x60) {
                    slowlen = 8;
                    do {
                        slowlen++;
                    } while (cmptbl[slowlen] <= (bits >> 16));
                } else {
                    slowlen = cluelen;
                }

                unsigned int codeIdx = bits >> (32 - slowlen);
                bits <<= slowlen;
                bitsleft = bitsleft + (len - 16) - slowlen;
                code = codetbl[codeIdx - bitnumtbl[slowlen]];

                if (code != clue) {
                    if (bitsleft < 0) {
                        HUFF_REFILL()
                    }
                    if (code != clue) {
                        *qd = code;
                        qd++;
                        bitsleft = bitsleft;
                        goto nextloop;
                    }
                }

                // Escape code handling
                if (bitsleft < 0) {
                    HUFF_REFILL()
                }

                // Read run length
                {
                    int runlen;
                    HUFF_READNUM(runlen)

                    if (runlen != 0) {
                        // RLE: copy previous byte
                        unsigned char *d = qd;
                        unsigned char *dest = qd + runlen;
                        unsigned char prev = *(qd - 1);
                        do {
                            *qd = prev;
                            qd++;
                        } while (qd < dest);
                        goto nextloop;
                    }

                    // End of data marker
                    bitsleft--;
                    int shiftedBits = bits << 1;
                    if (bitsleft < 0) {
                        bitsunshifted = (bitsunshifted << 8) | *qs;
                        bitsunshifted = (bitsunshifted << 8) | *(qs + 1);
                        qs += 2;
                        shiftedBits = bitsunshifted << static_cast<unsigned int>(-bitsleft);
                        bitsleft += 16;
                    }
                    if (static_cast<int>(bits) >= 0) {
                        // Read raw byte
                        unsigned int t;
                        code = static_cast<unsigned char>(shiftedBits >> 24);
                        bitsleft -= 8;
                        bits = shiftedBits << 8;
                        if (bitsleft < 0) {
                            HUFF_REFILL()
                        }
                        *qd = code;
                        qd++;
                        goto nextloop;
                    }
                }
            }
        }

        // Post-processing
        {
            int i;
            int nextchar;
            if (type == 0x32fb || type == 0xb2fb) {
                unsigned char prev = 0;
                unsigned char *end = unpackbuf + ulen;
                for (; unpackbuf < end; unpackbuf++) {
                    prev += *unpackbuf;
                    *unpackbuf = prev;
                }
            } else if (type == 0x34fb || type == 0xb4fb) {
                char delta = 0;
                unsigned char accum = 0;
                unsigned char *end = unpackbuf + ulen;
                for (; unpackbuf < end; unpackbuf++) {
                    delta += *unpackbuf;
                    accum += delta;
                    *unpackbuf = accum;
                }
            }
        }
    }
    }

    return ulen;
}

#undef HUFF_REFILL
#undef HUFF_READNUM

void HUFF_decode(void *dest, const void *src) {
    HUFF_decompress(static_cast<unsigned char *>(const_cast<void *>(src)),
                    static_cast<unsigned char *>(dest));
}

// VolumeTree types and functions (compiled as part of LZCompress TU)
void bEndianSwap16(void *ptr);
void bEndianSwap32(void *ptr);

struct vAABB {
    float PositionX;            // offset 0x0
    float PositionY;            // offset 0x4
    float PositionZ;            // offset 0x8
    short ParentIndex;          // offset 0xC
    short NumChildren;          // offset 0xE
    float ExtentX;              // offset 0x10
    float ExtentY;              // offset 0x14
    float ExtentZ;              // offset 0x18
    short ChildrenIndicies[10]; // offset 0x1C

    int Contains(float x, float y, float z);
};

struct vAABBTree {
    vAABB *NodeArray;   // offset 0x0
    short NumLeafNodes; // offset 0x4
    short NumParentNodes; // offset 0x6
    short TotalNodes;   // offset 0x8
    short Depth;        // offset 0xA
    int pad1;           // offset 0xC

    void SwapEndian();
    vAABB *QueryLeafHelper(vAABB *aabb, float x, float y, float z);
    vAABB *QueryLeaf(float x, float y, float z);
};

int vAABB::Contains(float x, float y, float z) {
    float delta_x = x - PositionX;
    float delta_y = y - PositionY;
    float delta_z = z - PositionZ;
    float ax = bAbs(delta_x);
    float ex = ExtentX;
    float ay = bAbs(delta_y);
    float ez = ExtentZ;
    float az = bAbs(delta_z);
    float ey = ExtentY;
    if (ax >= ex) return 0;
    if (ay >= ey) return 0;
    if (az >= ez) return 0;
    return 1;
}

void vAABBTree::SwapEndian() {
    bEndianSwap16(&NumLeafNodes);
    bEndianSwap16(&NumParentNodes);
    bEndianSwap16(&TotalNodes);
    bEndianSwap16(&Depth);
    for (int i = 0; i < TotalNodes; i++) {
        vAABB *node = &NodeArray[i];
        bEndianSwap32(&node->PositionX);
        bEndianSwap32(&node->PositionY);
        bEndianSwap32(&node->PositionZ);
        bEndianSwap16(&node->ParentIndex);
        bEndianSwap16(&node->NumChildren);
        bEndianSwap32(&node->ExtentX);
        bEndianSwap32(&node->ExtentY);
        bEndianSwap32(&node->ExtentZ);
        for (int j = 0; j < 10; j++) {
            bEndianSwap16(&node->ChildrenIndicies[j]);
        }
    }
}

vAABB *vAABBTree::QueryLeafHelper(vAABB *aabb, float x, float y, float z) {
    int num_children = aabb->NumChildren;
    vAABB *root = NodeArray;
    for (int i = 0; i < num_children; i++) {
        vAABB *child = &root[aabb->ChildrenIndicies[i]];
        if (child->Contains(x, y, z)) {
            if (child->NumChildren <= 0) return child;
            child = QueryLeafHelper(child, x, y, z);
            if (child != nullptr) return child;
        }
    }
    return nullptr;
}

vAABB *vAABBTree::QueryLeaf(float x, float y, float z) {
    vAABB *root = NodeArray;
    if (root == nullptr) return nullptr;
    if (!root->Contains(x, y, z)) return nullptr;
    if (root->NumChildren <= 0) return root;
    return QueryLeafHelper(root, x, y, z);
}