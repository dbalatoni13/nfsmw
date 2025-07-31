#ifndef BWARE_STRINGS_H
#define BWARE_STRINGS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "bDebug.hpp"

struct bSharedString {
    // total size: 0x8
    unsigned short Size;  // offset 0x0, size 0x2
    unsigned short Prev;  // offset 0x2, size 0x2
    unsigned short Count; // offset 0x4, size 0x2
    char String[2];       // offset 0x6, size 0x2
};

struct bSharedStringPool {
    // total size: 0x2840
    int StringTableSize;                  // offset 0x0, size 0x4
    int StringTableSizeBytes;             // offset 0x4, size 0x4
    bSharedString *StringTable;           // offset 0x8, size 0x4
    bMutex Mutex;                         // offset 0xC, size 0x20
    int NumBytesAllocated;                // offset 0x2C, size 0x4
    int MostBytesAllocated;               // offset 0x30, size 0x4
    int NumStringsAllocated;              // offset 0x34, size 0x4
    int MostStringsAllocated;             // offset 0x38, size 0x4
    bSharedString *FastLookupTable[2048]; // offset 0x3C, size 0x2000
    char FastLookupTableCount[2048];      // offset 0x203C, size 0x800
    bSharedString *LargestFreeString;     // offset 0x283C, size 0x4

    void Init(int size);
    const char *Allocate(const char *s);
    void Free(const char *s);
};

char *bStrNCpy(char *to, const char *from, int m);
char *bSafeStrCpy(char *to, const char *from, int max_size);
int bStrCmp(const char *s1, const char *s2);
int bStrNCmp(const char *s1, const char *s2, int n);
int bStrICmp(const char *s1, const char *s2);
const char *bAllocateSharedString(const char *s);
void bFreeSharedString(const char *s);
unsigned int bStringHash(const char *text);
unsigned int bStringHash(const char *text, int prefix_hash);
int bStrToLong(const char *s);
float bStrToFloat(const char *s);
int bStrLen(const char *s);
char *bStrCpy(char *to, const char *from);

inline char bToUpper(char c) {
    if (c >= 'a' && c <= 'z') {
        c &= 0x5f;
    }
    return c;
}

#endif
