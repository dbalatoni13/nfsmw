#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

static char bMemoryDebugStringNoName[8]; // size: 0x8, address: 0x80416460
bSharedStringPool gSharedStringPool;     // size: 0x2840, address: 0x8045792C

unsigned int bStringHashUpper(const char *text) {
    unsigned int h = ~0;
    while (*text) {
        h = (h << 5) + h;
        h += bToUpper(*text++);
    }
    return h;
}

unsigned int bStringHash(const char *text) {
    unsigned int h = ~0;
    while (*text) {
        h = (h << 5) + h;
        h += *(unsigned char *)text++;
    }
    return h;
}

unsigned int bStringHash(const char *text, int prefix_hash) {
    unsigned int h = prefix_hash;
    while (*text) {
        h = (h << 5) + h;
        h += *(unsigned char *)text++;
    }
    return h;
}

int bStrLen(const char *s) {
    int n = 0;

    if (s) {
        while (s[n] != '\0') {
            n++;
        }
    }
    return n;
}

// UNSOLVED
char *bStrCpy(char *to, const char *from) {
    int n = 0;
    // wrong, it actually copies the first character no matter what
    while (from[n] != '\0') {
        to[n] = from[n];
        n++;
    }
    return to;
}

void bSharedStringPool::Init(int size) {
    int table_size = static_cast<unsigned int>(size) >> 3;
    int table_size_bytes = table_size << 3;

    this->Mutex.Create();
    bSharedString *string = static_cast<bSharedString *>(bMALLOC(table_size_bytes, "", 0, 0));
    this->StringTableSize = table_size;
    this->StringTableSizeBytes = table_size_bytes;
    this->StringTable = string;
    string->Count = 0;
    string->Size = this->StringTableSize;
    string->Prev = 0;
    string->String[0] = '\0';
    this->LargestFreeString = string;
}

void bInitSharedStringPool(int size) {
    gSharedStringPool.Init(size);
}

const char *bAllocateSharedString(const char *s) {
    return gSharedStringPool.Allocate(s);
}

void bFreeSharedString(const char *s) {
    gSharedStringPool.Free(s);
}
