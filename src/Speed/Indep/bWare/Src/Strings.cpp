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

char *bStrCpy(char *to, const char *from) {
    int n = 0;
    to[0] = from[0];
    while (to[n] != '\0') {
        n++;
        to[n] = from[n];
    }
    return to;
}

char *bStrNCpy(char *to, const char *from, int m) {
    int n = 0;
    if (m-- != 0) {
        to[0] = from[0];
        while (to[n] != '\0') {
            n++;
            if (m-- == 0) {
                return to;
            }
            to[n] = from[n];
        }
    }
    return to;
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

// UNSOLVED
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

// UNSOLVED
int bStrNICmp(const char *s1, const char *s2, int n) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        }
        return 0;
    }

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
        nn++;
        n++;
    }

    to[n] = '\0';
    return to;
}

// STRIPPED
char *bStrChr(const char *s1, int c) {}

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
    int n = 0;

    to[0] = from[0];
    while (to[n] != 0) {
        n++;
        to[n] = from[n];
    }
    return to;
}

uint16 *bStrNCpy(uint16 *to, const uint16 *from, int m) {
    int n = 0;
    if (m-- != 0) {
        to[0] = from[0];
        while (to[n] != '\0') {
            n++;
            if (m-- == 0) {
                return to;
            }
            to[n] = from[n];
        }
    }
    return to;
}

uint16 *bStrNCpy(uint16 *to, const char *from, int m) {
    int n = 0;
    if (m-- != 0) {
        to[0] = from[0];
        while (to[n] != '\0') {
            n++;
            if (m-- == 0) {
                return to;
            }
            to[n] = from[n];
        }
    }
    return to;
}

// STRIPPED
int bStrCmp(uint16 *s1, uint16 *s2) {}

// STRIPPED
int bStrNCmp(uint16 *s1, uint16 *s2, int n) {}

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

// STRIPPED
uint16 *bStrCat(uint16 *to, uint16 *s1, uint16 *s2) {}

int bMatchNameWithWildcard(const char *wild, const char *string) {
    const char *cp = nullptr;
    const char *mp = nullptr;

    while ((*string != '\0') && (*wild != '*')) {
        if ((bToUpper(*wild) != bToUpper(*string)) && (*wild != '?')) {
            return false;
        }

        string++;
        wild++;
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

// STRIPPED
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

const char *bSharedStringPool::Allocate(const char *s) {
    if (s == nullptr) {
        return nullptr;
    }
    if (this->StringTable == nullptr) {
        return nullptr;
    }

    this->Mutex.Lock();

    int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);
    bSharedString *string = this->FastLookupTable[hash_index];
    if (string && (bStrCmp(s, string->String) == 0)) {
        string->Count++;
        this->Mutex.Unlock();
        return string->String;
    }

    int search_table = true;
    if (this->FastLookupTableCount[hash_index] == 0) {
        search_table = false;
    } else if ((this->FastLookupTableCount[hash_index] == 1) && (string != nullptr)) {
        search_table = false;
    }

    if (search_table) {
        for (string = GetStringTableStart(); string != GetStringTableEnd(); string = string->GetNext()) {
            if ((string->Count != 0) && (bStrCmp(s, string->String) == 0)) {
                this->FastLookupTable[hash_index] = string;
                string->Count++;
                this->Mutex.Unlock();
                return string->String;
            }
        }
    }

    int size = (bStrLen(s) + 14) / sizeof(bSharedString);
    string = this->LargestFreeString;

    do {
        if ((string->Count == 0) && (string->Size >= size)) {
            if (string->Size > size) {
                bSharedString *next_string = &string[size];
                next_string->Size = string->Size - size;
                next_string->Prev = next_string - string;
                next_string->Count = 0;
                next_string->String[0] = '\0';

                bSharedString *next_next_string = next_string->GetNext();
                if (next_next_string != GetStringTableEnd()) {
                    next_next_string->Prev = next_next_string - next_string;
                }
            }

            string->Size = static_cast<unsigned short>(size);
            string->Count = 1;
            bStrCpy(string->String, s);
            this->FastLookupTable[hash_index] = string;
            this->FastLookupTableCount[hash_index]++;

            this->NumBytesAllocated += size * sizeof(bSharedString);
            if (this->NumBytesAllocated > this->MostBytesAllocated) {
                this->MostBytesAllocated = this->NumBytesAllocated;
            }

            this->NumStringsAllocated++;
            if (this->NumStringsAllocated > this->MostStringsAllocated) {
                this->MostStringsAllocated = this->NumStringsAllocated;
            }

            this->LargestFreeString = string->GetNext();
            if (this->LargestFreeString == GetStringTableEnd()) {
                this->LargestFreeString = GetStringTableStart();
            }

            this->Mutex.Unlock();
            return string->String;
        }

        string = string->GetNext();
        if (string == GetStringTableEnd()) {
            string = GetStringTableStart();
        }
    } while (string != this->LargestFreeString);

    this->Mutex.Unlock();
    return nullptr;
}

void bSharedStringPool::Free(const char *s) {
    if (s == nullptr) {
        return;
    }

    int index = GetIndex(s);
    if (index == -1) {
        return;
    }

    bSharedString *string = GetSharedString(index);

    this->Mutex.Lock();

    if (--string->Count != 0) {
        this->Mutex.Unlock();
        return;
    }

    int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);

    if (this->FastLookupTable[hash_index] == string) {
        this->FastLookupTable[hash_index] = 0;
    }

    this->FastLookupTableCount[hash_index]--;
    string->String[0] = 0;
    this->NumBytesAllocated -= string->Size * sizeof(bSharedString);
    this->NumStringsAllocated--;

    bSharedString *next_string = string->GetNext();

    if ((next_string != GetStringTableEnd()) && (next_string->Count == 0)) {
        string->Size += next_string->Size;
        next_string = next_string->GetNext(); // TODO new variable to match dwarf
        if (next_string != GetStringTableEnd()) {
            next_string->Prev = next_string - string;
        }
    }

    bSharedString *prev_string = string->GetPrev();
    if ((string != GetStringTableStart()) && (prev_string->Count == 0)) {
        prev_string->Size += string->Size;
        bSharedString *next_string = string->GetNext();
        if (next_string != GetStringTableEnd()) {
            next_string->Prev = next_string - prev_string;
        }
        string = prev_string;
    }

    if (string->Size > this->LargestFreeString->Size) {
        this->LargestFreeString = string;
    }

    this->Mutex.Unlock();
}

// STRIPPED
void bSharedStringPool::Dump() {}

// STRIPPED
void bSharedStringPool::Validate() {}

void bInitSharedStringPool(int size) {
    gSharedStringPool.Init(size);
}

// STRIPPED
void bCloseSharedStringPool() {}

const char *bAllocateSharedString(const char *s) {
    return gSharedStringPool.Allocate(s);
}

void bFreeSharedString(const char *s) {
    gSharedStringPool.Free(s);
}

// STRIPPED
void bDumpSharedStrings() {}

// STRIPPED
short bGetSharedStringIndex(const char *s) {}

// STRIPPED
const char *bGetSharedString(int index) {}
