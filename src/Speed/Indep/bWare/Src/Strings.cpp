#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

bSharedStringPool gSharedStringPool; // size: 0x2840, address: 0x8045792C

unsigned int bStringHashUpper(const char *text) {
    unsigned int h = ~0;
    while (*text) {
        h = (h << 5) + h;
        h += bToUpper(*text++);
    }
    return h;
}

uint32 bStringHash(const char *text) {
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

char *bStrCpy(char *to, const char *from) {
#ifdef EA_PLATFORM_XENON
    int n = 0;
    unsigned int c = static_cast<unsigned char>(from[0]);
    to[0] = static_cast<char>(c);
    while (c != 0) {
        n++;
        c = static_cast<unsigned char>(from[n]);
        to[n] = static_cast<char>(c);
    }
#else
    int n = 0;
    to[0] = from[0];
    while (to[n] != '\0') {
        n++;
        to[n] = from[n];
    }
#endif
    return to;
}

char *bStrNCpy(char *to, const char *from, int m) {
#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON)
    if (m == 0) {
        return to;
    }

    int n = 0;
    do {
        to[n] = from[n];
        --m;
        if (to[n] == '\0') {
            return to;
        }
        ++n;
    } while (m != 0);
    return to;
#else
    int n = 0;
    if (m-- != 0) {
        to[0] = from[0];
        while (to[n] != '\0') {
            n++;
            if (m-- == 0) {
                goto copied;
            }
            to[n] = from[n];
        }
    }
copied:
    return to;
#endif
}

char *bSafeStrCpy(char *to, const char *from, int max_size) {
    int n = 0;

    if (from) {
        while ((n < max_size - 1) && (from[n] != '\0')) {
            to[n] = from[n];
            n++;
        }
    }
    to[n] = '\0';
    return to;
}

int bStrCmp(const char *s1, const char *s2) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        }
        return 0;
    }

    if (s2 == nullptr) {
        return 1;
    }

    char c1;
    char c2;

    do {
        c1 = *s1++;
        c2 = *s2++;
    } while ((c1 != '\0') && (c2 != '\0') && (c1 == c2));

    return c1 - c2;
}

int bStrNCmp(const char *s1, const char *s2, int n) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        } else {
            return 0;
        }
    } else if (s2 == nullptr) {
        return 1;
    }

#ifdef EA_PLATFORM_WIN32
    while ((n-- != 0) && (*s1 != '\0') && (*s2 != '\0') && (*s1++ == *s2++)) {
    }
    if (n >= 0) {
        if (*s1 != '\0') {
            return 1;
        }
        if (*s2 != '\0') {
            return -1;
        }
        return s1[-1] - s2[-1];
    }
    return 0;
#else
    while (n-- != 0) {
        if (*s1 == '\0') {
            break;
        }

        if (*s2 == '\0') {
            break;
        }

        if (*s1++ != *s2++) {
            break;
        }
    }

    if (n >= 0) {
        if (*s1 == '\0') {
            if (*s2 == '\0') {
                return s1[-1] - s2[-1];
            } else {
                return -1;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
#endif
}

int bStrICmp(const char *s1, const char *s2) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        }
        return 0;
    }

    if (s2 == nullptr) {
        return 1;
    }

    char c1;
    char c2;

    do {
        c1 = bToUpper(*s1++);
        c2 = bToUpper(*s2++);
    } while ((c1 != 0) && (c2 != 0) && (c1 == c2));

    return c1 - c2;
}

int bStrNICmp(const char *s1, const char *s2, int n) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        }
        return 0;
    }

#if defined(EA_PLATFORM_WIN32) || defined(EA_PLATFORM_XENON)
    if (s2 != nullptr) {
        while ((n-- != 0) && (*s1 != '\0') && (*s2 != '\0') && (bToUpper(*s1++) == bToUpper(*s2++))) {
        }

        if (n >= 0) {
            if (*s1 != '\0') {
                return 1;
            }
            if (*s2 != '\0') {
                return -1;
            }
            return bToUpper(s1[-1]) - bToUpper(s2[-1]);
        } else {
            return 0;
        }
    }
    return 1;
#else
    if (s2 == nullptr) {
        return 1;
    }

    for (;;) {
        if (n-- == 0) {
            break;
        }
        if (*s1 == '\0') {
            break;
        }
        if (*s2 == '\0') {
            break;
        }
        if (bToUpper(*s1++) != bToUpper(*s2++)) {
            break;
        }
    }

    if (n >= 0) {
        if (*s1 == '\0') {
            if (*s2 == '\0') {
                return bToUpper(s1[-1]) - bToUpper(s2[-1]);
            } else {
                return -1;
            }
        } else {
            return 1;
        }
    } else {
        return 0;
    }
#endif
}

char *bStrCat(char *to, const char *s1, const char *s2) {
    int n = 0;
    int nn = 0;

    while (s1[nn] != '\0') {
        to[n] = s1[nn];
        n++;
        nn++;
    }

    nn = 0;
    while (s2[nn] != '\0') {
        to[n] = s2[nn];
        n++;
        nn++;
    }

    to[n] = '\0';
    return to;
}

char *bStrChr(const char *s1, int c) {
#if defined(EA_PLATFORM_XENON) || (defined(EA_PLATFORM_PLAYSTATION2) && defined(EA_BUILD_A124))
    if (s1 != nullptr) {
        do {
            if (static_cast<int>(static_cast<signed char>(*s1)) == c) {
                goto found;
            }
        } while (*s1++ != '\0');
    }
    s1 = nullptr;
found:
    return const_cast<char *>(s1);
#else
    if (s1 == nullptr) {
        return nullptr;
    }

    // These platform paths stop before testing the terminator for a match.
    while (*s1 != '\0') {
        if (static_cast<int>(static_cast<signed char>(*s1)) == c) {
            return const_cast<char *>(s1);
        }
        s1++;
    }
    return nullptr;
#endif
}

char *bToUpper(char *s) {
    if (*s != '\0') {
        do {
            *s = bToUpper(*s);
            s++;
        } while (*s != '\0');
    }

    return s;
}

int bStrToLong(const char *s) {
    if ((s[0] == '0') && (s[1] == 'x')) {
        s += 2;

        int n = 0;
        while (*s != '\0') {
            char c = *s;
            int value = 0;
            if (bIsDigit(c)) {
                value = c - '0';
            } else {
                c = bToUpper(c);

                if (!('A' <= c && c <= 'F')) {
                    return n;
                }

                value = c - 'A' + 10;
            }

            n = n * 0x10 + value;
            s++;
        }
        return n;
    } else {
        bool negate = false;
        if (*s == '-') {
            negate = true;
            s++;
        } else if (*s == '+') {
            s++;
        }

        int n = 0;

        while (*s != '\0') {
            if (!bIsDigit(*s)) {
                break;
            }
            n = n * 10 + (*s - '0');
            s++;
        }

        if (negate) {
            n = -n;
        }
        return n;
    }
}

float bStrToFloat(const char *s) {
    bool negate = false;

    if (*s == '-') {
        negate = true;
        s++;
    } else if (*s == '+') {
        s++;
    }

    float value = 0.0f;

    while (bIsDigit(*s)) {
        value = value * 10.0f + static_cast<float>(*s++ - '0');
    }

    if (*s == '.') {
        float fractional_part = 0.0f;
        float fraction = 1.0f;
        s++;

        while (bIsDigit(*s)) {
            fraction *= 0.1f;
            fractional_part += static_cast<float>(*s - '0') * fraction;
            s++;
        }

        if (fractional_part != 0.0f) {
            *reinterpret_cast<int *>(&fractional_part) += 1;
        }

        value += fractional_part;
    }

    if ((*s == 'e') || (*s == 'E')) {
        int exp = bStrToLong(s + 1);
        float multiplier = 1.0f;

        while (exp > 0) {
            multiplier *= 10.0f;
            exp--;
        }

        while (exp < 0) {
            multiplier *= 0.1f;
            exp++;
        }

        value *= multiplier;
    }

    if (negate) {
        value = -value;
    }

    return value;
}

int bStrLen(const uint16 *s) {
    int n = 0;

    if (s) {
        while (s[n] != 0) {
            n++;
        }
    }
    return n;
}

uint16 *bStrCpy(uint16 *to, const uint16 *from) {
    int n = 0;

    to[0] = from[0];
    while (to[n] != 0) {
        n++;
        to[n] = from[n];
    }
    return to;
}

uint16 *bStrCpy(uint16 *to, const char *from) {
#ifdef EA_PLATFORM_WIN32
    to[0] = from[0];
    if (to[0] != 0) {
        uint16 *dest = to;
        do {
            from++;
            dest++;
            *dest = *from;
        } while (*dest != 0);
    }
    return to;
#else
    int n = 0;

    to[0] = from[0];
    while (to[n] != 0) {
        n++;
        to[n] = from[n];
    }
    return to;
#endif
}

uint16 *bStrNCpy(uint16 *to, const uint16 *from, int m) {
    int n = 0;
    while (m != 0) {
        uint16 c = from[n];
        m--;
        to[n] = c;
        if (c == 0) {
            break;
        }
        n++;
    }
    return to;
}

uint16 *bStrNCpy(uint16 *to, const char *from, int m) {
#ifdef EA_PLATFORM_WIN32
    int n = 0;
    while (m != 0) {
        uint16 c = from[n];
        m--;
        to[n] = c;
        if (c == 0) {
            break;
        }
        n++;
    }
    return to;
#else
    int n = 0;
    if (m-- != 0) {
        to[0] = from[0];
        while (to[n] != '\0') {
            n++;
            if (m-- == 0) {
                goto copied;
            }
            to[n] = from[n];
        }
    }
copied:
    return to;
#endif
}

int bStrCmp(uint16 *s1, uint16 *s2) {
    uint16 c1;
    uint16 c2;

    do {
        c1 = *s1++;
        c2 = *s2++;
    } while ((c1 != 0) && (c2 != 0) && (c1 == c2));

    // Retail zero-extends both code units before subtracting them.
    return static_cast<int>(c1) - static_cast<int>(c2);
}

int bStrNCmp(uint16 *s1, uint16 *s2, int n) {
#ifdef EA_PLATFORM_WIN32
    while ((n-- != 0) && (*s1 != 0) && (*s2 != 0) && (*s1++ == *s2++)) {
    }
    if (n >= 0) {
        if (*s1 != 0) {
            return 1;
        }
        if (*s2 != 0) {
            return -1;
        }
        return static_cast<int>(s1[-1]) - static_cast<int>(s2[-1]);
    }
    return 0;
#else
    while (n-- != 0) {
        if (*s1 == 0) {
            break;
        }
        if (*s2 == 0) {
            break;
        }
        if (*s1++ != *s2++) {
            break;
        }
    }

    // This deliberately mirrors the retail routine: an early mismatch is
    // reported as +/-1 unless it also terminates both strings, in which case
    // the last UTF-16 code units are subtracted as unsigned values.
    if (n >= 0) {
        if (*s1 == 0) {
            if (*s2 == 0) {
                return static_cast<int>(s1[-1]) - static_cast<int>(s2[-1]);
            }
            return -1;
        }
        return 1;
    }
    return 0;
#endif
}

char *bStrStr(const char *s1, const char *s2) {
    int len = bStrLen(s2);

    while (*s1 != '\0') {
        if ((*s1 == *s2) && (bStrNCmp(s1, s2, len) == 0)) {
            return const_cast<char *>(s1);
        }
        s1++;
    }

    return nullptr;
}

char *bStrIStr(const char *s1, const char *s2) {
    int len = bStrLen(s2);

    while (*s1 != '\0') {
        if (bStrNICmp(s1, s2, len) == 0) {
            return const_cast<char *>(s1);
        }
        s1++;
    }

    return nullptr;
}

uint16 *bStrCat(uint16 *to, uint16 *s1, uint16 *s2) {
    int n = 0;

    while (*s1 != 0) {
        to[n++] = *s1++;
    }

    while (*s2 != 0) {
        to[n++] = *s2++;
    }

    to[n] = 0;
    return to;
}

int bMatchNameWithWildcard(const char *wild, const char *string) {
    const char *cp = nullptr;
    const char *mp = nullptr;

    while ((*string != '\0') && (*wild != '*')) {
        if ((bToUpper(*wild) != bToUpper(*string)) && (*wild != '?')) {
            return false;
        }

        wild++;
        string++;
    }

    while (*string != '\0') {
        if (*wild == '*') {
            wild++;
            if (*wild == '\0') {
                return true;
            }

            mp = wild;
            cp = string + 1;
        } else {
            if ((bToUpper(*wild) == bToUpper(*string)) || (*wild == '?')) {
                wild++;
                string++;
            } else {
                string = cp;
                wild = mp;
                cp = string + 1;
            }
        }
    }

    while (*wild == '*') {
        wild++;
    }

    return *wild == '\0';
}

void bSharedStringPool::Init(int size) {
    int table_size = static_cast<unsigned int>(size) / sizeof(bSharedString);
    int table_size_bytes = table_size * sizeof(bSharedString);

    this->Mutex.Create();
    bSharedString *string = static_cast<bSharedString *>(bMalloc(table_size_bytes, "bSharedStringPool", 0, 0));
    this->StringTableSize = table_size;
    this->StringTableSizeBytes = table_size_bytes;
    this->StringTable = string;
    string->Count = 0;
    string->Size = this->StringTableSize;
    string->Prev = 0;
    string->String[0] = '\0';
    this->LargestFreeString = string;
}

void bSharedStringPool::Close() {
    if (StringTable) {
        bFree(StringTable);
        StringTable = nullptr;
        StringTableSizeBytes = 0;
        StringTableSize = 0;
        LargestFreeString = nullptr;
        Mutex.Destroy();
    }
}

#ifdef EA_PLATFORM_XENON
// Retail Xenon specializes these entries for the global pool. Keep the original
// instance expression elsewhere: a local alias changes historical MSVC codegen.
#define B_SHARED_STRING_POOL pool
#else
#define B_SHARED_STRING_POOL this
#endif

const char *bSharedStringPool::Allocate(const char *s) {
#ifdef EA_PLATFORM_XENON
    bSharedStringPool *pool = &gSharedStringPool;
#endif
    if (s == nullptr) {
        return nullptr;
    }
    if (B_SHARED_STRING_POOL->StringTable == nullptr) {
        return nullptr;
    }

    B_SHARED_STRING_POOL->Mutex.Lock();

    int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);
    bSharedString *string = B_SHARED_STRING_POOL->FastLookupTable[hash_index];
    if (string && (bStrCmp(s, string->String) == 0)) {
        string->Count++;
        B_SHARED_STRING_POOL->Mutex.Unlock();
        return string->String;
    }

#ifdef EA_PLATFORM_XENON
    bool search_table = true;
#else
    int search_table = true;
#endif
    if (B_SHARED_STRING_POOL->FastLookupTableCount[hash_index] == 0) {
        search_table = false;
    } else if ((B_SHARED_STRING_POOL->FastLookupTableCount[hash_index] == 1) && (string != nullptr)) {
        search_table = false;
    }

    if (search_table) {
        for (string = B_SHARED_STRING_POOL->GetStringTableStart(); string != B_SHARED_STRING_POOL->GetStringTableEnd(); string = string->GetNext()) {
            if ((string->Count != 0) && (bStrCmp(s, string->String) == 0)) {
                B_SHARED_STRING_POOL->FastLookupTable[hash_index] = string;
                string->Count++;
                B_SHARED_STRING_POOL->Mutex.Unlock();
                return string->String;
            }
        }
    }

    int size = (bStrLen(s) + 14) / sizeof(bSharedString);
    string = B_SHARED_STRING_POOL->LargestFreeString;

    do {
        if ((string->Count == 0) && (string->Size >= size)) {
            if (string->Size > size) {
                bSharedString *next_string = &string[size];
                next_string->Size = string->Size - size;
                next_string->Prev = next_string - string;
                next_string->Count = 0;
                next_string->String[0] = '\0';

                bSharedString *next_next_string = next_string->GetNext();
                if (next_next_string != B_SHARED_STRING_POOL->GetStringTableEnd()) {
                    next_next_string->Prev = next_next_string - next_string;
                }
            }

            string->Size = static_cast<unsigned short>(size);
            string->Count = 1;
            bStrCpy(string->String, s);
            B_SHARED_STRING_POOL->FastLookupTable[hash_index] = string;
            B_SHARED_STRING_POOL->FastLookupTableCount[hash_index]++;

            B_SHARED_STRING_POOL->NumBytesAllocated += size * sizeof(bSharedString);
            if (B_SHARED_STRING_POOL->NumBytesAllocated > B_SHARED_STRING_POOL->MostBytesAllocated) {
                B_SHARED_STRING_POOL->MostBytesAllocated = B_SHARED_STRING_POOL->NumBytesAllocated;
            }

            B_SHARED_STRING_POOL->NumStringsAllocated++;
            if (B_SHARED_STRING_POOL->NumStringsAllocated > B_SHARED_STRING_POOL->MostStringsAllocated) {
                B_SHARED_STRING_POOL->MostStringsAllocated = B_SHARED_STRING_POOL->NumStringsAllocated;
            }

            B_SHARED_STRING_POOL->LargestFreeString = string->GetNext();
            if (B_SHARED_STRING_POOL->LargestFreeString == B_SHARED_STRING_POOL->GetStringTableEnd()) {
                B_SHARED_STRING_POOL->LargestFreeString = B_SHARED_STRING_POOL->GetStringTableStart();
            }

            B_SHARED_STRING_POOL->Mutex.Unlock();
            return string->String;
        }

        string = string->GetNext();
        if (string == B_SHARED_STRING_POOL->GetStringTableEnd()) {
            string = B_SHARED_STRING_POOL->GetStringTableStart();
        }
    } while (string != B_SHARED_STRING_POOL->LargestFreeString);

    B_SHARED_STRING_POOL->Mutex.Unlock();
    return nullptr;
}

void bSharedStringPool::Free(const char *s) {
#ifdef EA_PLATFORM_XENON
    bSharedStringPool *pool = &gSharedStringPool;
#endif
    if (s == nullptr) {
        return;
    }

    int index = B_SHARED_STRING_POOL->GetIndex(s);
    if (index == -1) {
        return;
    }

    bSharedString *string = B_SHARED_STRING_POOL->GetSharedString(index);

    B_SHARED_STRING_POOL->Mutex.Lock();

    if (--string->Count != 0) {
        B_SHARED_STRING_POOL->Mutex.Unlock();
        return;
    }

    int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);

    if (B_SHARED_STRING_POOL->FastLookupTable[hash_index] == string) {
        B_SHARED_STRING_POOL->FastLookupTable[hash_index] = 0;
    }

    B_SHARED_STRING_POOL->FastLookupTableCount[hash_index]--;
    string->String[0] = 0;
    B_SHARED_STRING_POOL->NumBytesAllocated -= string->Size * sizeof(bSharedString);
    B_SHARED_STRING_POOL->NumStringsAllocated--;

    bSharedString *next_string = string->GetNext();

    if ((next_string != B_SHARED_STRING_POOL->GetStringTableEnd()) && (next_string->Count == 0)) {
        string->Size += next_string->Size;
        next_string = next_string->GetNext(); // TODO new variable to match dwarf
        if (next_string != B_SHARED_STRING_POOL->GetStringTableEnd()) {
            next_string->Prev = next_string - string;
        }
    }

    bSharedString *prev_string = string->GetPrev();
    if ((string != B_SHARED_STRING_POOL->GetStringTableStart()) && (prev_string->Count == 0)) {
        prev_string->Size += string->Size;
        bSharedString *next_string = string->GetNext();
        if (next_string != B_SHARED_STRING_POOL->GetStringTableEnd()) {
            next_string->Prev = next_string - prev_string;
        }
        string = prev_string;
    }

    if (string->Size > B_SHARED_STRING_POOL->LargestFreeString->Size) {
        B_SHARED_STRING_POOL->LargestFreeString = string;
    }

    B_SHARED_STRING_POOL->Mutex.Unlock();
}

#undef B_SHARED_STRING_POOL

// STRIPPED
void bSharedStringPool::Dump() {
    int total_size = 0;
    int allocated_size = 0;

    for (bSharedString *string = GetStringTableStart(); string != GetStringTableEnd(); string = string->GetNext()) {
        int size = string->Size * sizeof(bSharedString);
        total_size += size;
        if (string->Count != 0) {
            allocated_size += size;
        }
    }

    bReleasePrintf("Shared strings: %d bytes allocated of %d (%d strings)\n", allocated_size, total_size, NumStringsAllocated);
}

// STRIPPED
void bSharedStringPool::Validate() {
    int num_bytes = 0;
    int num_strings = 0;
    bSharedString *prev_string = GetStringTableStart();

    for (bSharedString *string = GetStringTableStart(); string != GetStringTableEnd(); string = string->GetNext()) {
        if (string != GetStringTableStart()) {
            bAssertMsg(string->GetPrev() == prev_string, "Shared string back-link is corrupt");
        }
        bAssertMsg(string->Size != 0, "Shared string has zero size");

        if (string->Count != 0) {
            num_bytes += string->Size * sizeof(bSharedString);
            num_strings++;
        }
        prev_string = string;
    }

    bAssertMsg(num_bytes == NumBytesAllocated, "Shared string byte count is corrupt");
    bAssertMsg(num_strings == NumStringsAllocated, "Shared string count is corrupt");
}

void bInitSharedStringPool(int size) {
    gSharedStringPool.Init(size);
}

void bCloseSharedStringPool() {
    gSharedStringPool.Close();
}

const char *bAllocateSharedString(const char *s) {
    return gSharedStringPool.Allocate(s);
}

void bFreeSharedString(const char *s) {
    gSharedStringPool.Free(s);
}

void bDumpSharedStrings() {
    gSharedStringPool.Dump();
}

short bGetSharedStringIndex(const char *s) {
    return gSharedStringPool.GetIndex(s);
}

const char *bGetSharedString(int index) {
    return gSharedStringPool.GetString(index);
}
