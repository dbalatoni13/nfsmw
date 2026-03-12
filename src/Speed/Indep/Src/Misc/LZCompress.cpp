#include "LZCompress.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstring>

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
    if (header[0] == 0x5a4c444a && *reinterpret_cast<char *>(&header[1]) == '\x02') {
        unsigned char *src = pSrc + 0x12;
        int remaining = header[3] - 0x12;
        unsigned char *end = pDst + header[2];
        unsigned int flags1 = *reinterpret_cast<unsigned char *>(&header[4]) | 0x100;
        unsigned int flags2 = *(pSrc + 0x11) | 0x100;
        while (remaining != 0) {
            if ((flags1 & 1) == 0) {
                if (pDst < end) {
                    *pDst = *src;
                    src = src + 1;
                    pDst = pDst + 1;
                }
                remaining = remaining - 1;
            } else {
                int len;
                if ((flags2 & 1) == 0) {
                    len = (*src & 0x1f) + 3;
                    ShortMove(pDst, pDst - (((*src & 0xe0) << 3 | static_cast<unsigned int>(src[1])) + 0x11), len);
                } else {
                    len = ((*src & 0xf0) << 4 | static_cast<unsigned int>(src[1])) + 3;
                    ShortMove(pDst, pDst - ((*src & 0xf) + 1), len);
                }
                pDst = pDst + len;
                src = src + 2;
                remaining = remaining - 2;
                flags2 = static_cast<int>(flags2) >> 1;
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
    } else {
        return 0;
    }
}

int OldLZDecompress(unsigned char *pSrc, unsigned char *pDst) {
    int *header = reinterpret_cast<int *>(pSrc);
    if (header == nullptr || pDst == nullptr || header[0] != 0x504d4f43) {
        return 0;
    }
    unsigned char *src_end = pSrc + header[3];
    unsigned char *src = reinterpret_cast<unsigned char *>(&header[4]);
    unsigned int flags = 1;
    unsigned char *dst = pDst;
    if (*reinterpret_cast<short *>(pSrc + 6) == 1) {
        int size = header[3] - 0x10;
        memcpy(pDst, src, size);
    } else {
        while (src != src_end) {
            if (flags == 1) {
                unsigned char b0 = *src;
                unsigned char b1 = src[1];
                src = src + 2;
                flags = b0 | 0x10000 | static_cast<unsigned int>(b1) << 8;
            }
            int count = 1;
            if (src <= src_end - 0x20) {
                count = 0x10;
            }
            int i = count - 1;
            if (count != 0) {
                do {
                    if ((flags & 1) == 0) {
                        *dst = *src;
                        src = src + 1;
                        dst = dst + 1;
                    } else {
                        unsigned char byte0 = *src;
                        unsigned char byte1 = src[1];
                        src = src + 2;
                        unsigned char *ref = dst - ((byte0 & 0xf0) << 4 | static_cast<unsigned int>(byte1));
                        unsigned int extra = byte0 & 0xf;
                        *dst = *ref;
                        dst[1] = ref[1];
                        ref = ref + 2;
                        dst[2] = *ref;
                        dst = dst + 3;
                        if (extra != 0) {
                            do {
                                ref = ref + 1;
                                *dst = *ref;
                                dst = dst + 1;
                                extra = extra - 1;
                            } while (extra != 0);
                        }
                    }
                    flags = flags >> 1;
                    i = i - 1;
                } while (i >= 0);
            }
        }
    }
    return header[2];
}

void LZByteSwapHeader(LZHeader *header) {
    bEndianSwap32(&header->ID);
    bEndianSwap16(&header->Flags);
    bEndianSwap32(&header->UncompressedSize);
    bEndianSwap32(&header->CompressedSize);
}

int LZValidHeader(LZHeader *header) {
    unsigned int id = header->ID;
    if (id != 0x504d4f43) {
        if (id < 0x504d4f44) {
            if (id != 0x46465548) {
                return false;
            }
        } else if (id != 0x57574152) {
            if (id != 0x5a4c444a) {
                return false;
            }
            return header->Version == 2;
        }
    }
    return header->Version == 1;
}

int LZCompress(unsigned char *pSrc, unsigned int SrcSize, unsigned char *pDst) {
    return JLZCompress(pSrc, SrcSize, pDst);
}

int32 LZDecompress(uint8 *pSrc, uint8 *pDst) {
    LZHeader *header = reinterpret_cast<LZHeader *>(pSrc);
    if (header->ID == 0x5a4c444a) {
        return JLZDecompress(pSrc, pDst);
    } else if (header->ID == 0x504d4f43) {
        return OldLZDecompress(pSrc, pDst);
    } else {
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

int HUFF_encode(void *dest, const void *src, int size);

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