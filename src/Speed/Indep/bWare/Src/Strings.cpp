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

int bStrLen(const unsigned short *s) {
    int n = 0;

    if (s) {
        while (s[n] != 0) {
            n++;
        }
    }
    return n;
}

// UNSOLVED
char *bStrCpy(char *to, const char *from) {
    int n = 0;

    if ((*to = *from) != '\0') {
        do {
            n++;
        } while ((to[n] = from[n]) != '\0');
    }
    return to;
}

unsigned short *bStrCpy(unsigned short *to, const unsigned short *from) {
    int n = 0;

    to[n] = from[n];
    while (to[n] != 0) {
        n++;
        to[n] = from[n];
    }
    return to;
}

unsigned short *bStrCpy(unsigned short *to, const char *from) {
    int n = 0;

    if ((*to = *from) != '\0') {
        do {
            n++;
        } while ((to[n] = from[n]) != '\0');
    }
    return to;
}

char *bStrNCpy(char *to, const char *from, int m) {
    int n = 0;
    m = m - 1;
    if (m + 1 == 0) return to;
    if ((*to = *from) != '\0') {
        do {
            n = n + 1;
            if (m-- == 0) return to;
            if ((to[n] = from[n]) == '\0') break;
        } while (true);
    }
    return to;
}

unsigned short *bStrNCpy(unsigned short *to, const unsigned short *from, int m) {
    int n = 0;
    while ((n < m) && (from[n] != 0)) {
        to[n] = from[n];
        n++;
    }
    while (n < m) {
        to[n] = 0;
        n++;
    }
    return to;
}

unsigned short *bStrNCpy(unsigned short *to, const char *from, int m) {
    int n = 0;
    m = m - 1;
    if (m + 1 == 0) return to;
    if ((*to = *from) != '\0') {
        do {
            n = n + 1;
            if (m-- == 0) return to;
            if ((to[n] = from[n]) == '\0') break;
        } while (true);
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

void bSharedStringPool::Init(int size) {
    int table_size = static_cast<unsigned int>(size) >> 3;
    int table_size_bytes = table_size << 3;

    this->Mutex.Create();
    bSharedString *string = static_cast<bSharedString *>(bMalloc(table_size_bytes, "TODO", __LINE__, 0));
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
        char c1 = *s1;

        if (c1 == '\0') {
            break;
        }

        char c2 = *s2;
        if (c2 == '\0') {
            break;
        }

        s1 = s1 + 1;
        c1 = bToUpper(c1);
        s2 = s2 + 1;
        c2 = bToUpper(c2);
        if (c1 != c2) {
            break;
        }
    }

    if (n >= 0) {
        if (*s1 == '\0') {
            if (*s2 == '\0') {
                return bToUpper(s1[-1]) - bToUpper(s2[-1]);
            }

            return -1;
        }

        return 1;
    }

    return 0;
}

int bStrNCmp(const char *s1, const char *s2, int n) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        }
        return 0;
    }

    if (s2 != nullptr) {
        do {
            char c1;
            char c2;
            bool done = n == 0;
            n--;

            if (done) {
                break;
            }

            c1 = *s1;
            if (c1 == '\0') {
                break;
            }

            c2 = *s2;
            if (c2 == '\0') {
                break;
            }

            s2++;
            s1++;
            if (c1 != c2) {
                break;
            }
        } while (true);

        if (n < 0) {
            return 0;
        }
        if (*s1 == '\0') {
            if (*s2 == '\0') {
                return s1[-1] - s2[-1];
            }
            return -1;
        }
    }

    return 1;
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

    unsigned int c1;
    unsigned int c2;

    do {
        char ch = *s1;
        s1 = s1 + 1;
        c1 = static_cast<unsigned int>(ch);
        if ((c1 - 'a') < 26) {
            c1 = c1 & 0x5f;
        }

        ch = *s2;
        s2 = s2 + 1;
        c2 = static_cast<unsigned int>(ch);
        if ((c2 - 'a') < 26) {
            c2 = c2 & 0x5f;
        }
    } while ((c1 != 0) && (c2 != 0) && (c1 == c2));

    return c1 - c2;
}

char *bStrCat(char *to, const char *s1, const char *s2) {
    int to_n = 0;
    int s1_n = 0;
    int s2_n;

    while (s1[s1_n] != '\0') {
        to[to_n] = s1[s1_n];
        to_n++;
        s1_n++;
    }

    s2_n = 0;
    if (s2[s2_n] != '\0') {
        do {
            to[to_n] = s2[s2_n];
            s2_n++;
            to_n++;
        } while (s2[s2_n] != '\0');
    }

    to[to_n] = '\0';
    return to;
}

char *bToUpper(char *s) {
    if (*s != '\0') {
        do {
            *s = bToUpper(*s);
            s = s + 1;
        } while (*s != '\0');
    }

    return s;
}

int bStrToLong(const char *s) {
    if ((s[0] == '0') && (s[1] == 'x')) {
        int n;
        {
            char c;
            int value;

            s += 2;
            c = *s;
            n = 0;

            while (c != '\0') {
                value = c - '0';
                if (!bIsDigit(c)) {
                    c = bToUpper(c);
                    if (static_cast<unsigned int>(c - 'A') > 5U) {
                        return n;
                    }
                    value = c - '7';
                }
                c = *++s;
                n = n * 16 + value;
            }
        }
        return n;
    }

    bool negate = false;
    if (*s == '-') {
        negate = true;
    } else if (*s != '+') {
        goto parse_decimal;
    }
    s = s + 1;

parse_decimal:
    {
        int n = 0;
        char c = *s;

        if (c != '\0') {
            do {
                if (!bIsDigit(c)) {
                    break;
                }
                n = n * 10 + c - '0';
                c = *++s;
            } while (c != '\0');
        }

        if (negate) {
            return -n;
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
    char current = *s;

    while (bIsDigit(current)) {
        s++;
        value = value * 10.0f + static_cast<float>(current - '0');
        current = *s;
    }

    if (current == '.') {
        float fractional_part = 0.0f;
        float fraction = 1.0f;

        while (true) {
            current = *++s;
            if (!bIsDigit(current)) {
                break;
            }

            fraction = fraction * 0.1f;
            fractional_part = static_cast<float>(current - '0') * fraction + fractional_part;
        }

        if (fractional_part != 0.0f) {
            *reinterpret_cast<int *>(&fractional_part) += 1;
        }

        value = value + fractional_part;
    }

    if ((current == 'e') || (current == 'E')) {
        int exp = bStrToLong(s + 1);
        float multiplier = 1.0f;

        if (exp > 0) {
            do {
                multiplier = multiplier * 10.0f;
                exp--;
            } while (exp > 0);
        }

        if (exp < 0) {
            do {
                multiplier = multiplier * 0.1f;
                exp++;
            } while (exp < 0);
        }

        value = value * multiplier;
    }

    if (negate) {
        value = -value;
    }

    return value;
}

const char *bSharedStringPool::Allocate(const char *s) {
    if ((s == nullptr) || (this->StringTable == nullptr)) {
        return nullptr;
    }

    this->Mutex.Lock();

    int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);
    bSharedString *string = this->FastLookupTable[hash_index];
    if ((string != nullptr) && (bStrCmp(s, string->String) == 0)) {
        string->Count++;
        this->Mutex.Unlock();
        return string->String;
    }

    int search_table = 1;
    char count = this->FastLookupTableCount[hash_index];
    if (count == 0) {
        search_table = 0;
    } else if ((count == 1) && (string != nullptr)) {
        search_table = 0;
    }

    if (search_table != 0) {
        for (string = this->StringTable;
             string != reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(this->StringTable) + (this->StringTableSize << 3));
             string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + string->Size * 8)) {
            if ((string->Count != 0) && (bStrCmp(s, string->String) == 0)) {
                this->FastLookupTable[hash_index] = string;
                string->Count++;
                this->Mutex.Unlock();
                return string->String;
            }
        }
    }

    int size = (bStrLen(s) + 14U) >> 3;
    string = this->LargestFreeString;

    do {
        unsigned short string_count = string->Count;
        unsigned short string_size = string->Size;

        if ((string_count == 0) && (size <= static_cast<int>(string_size))) {
            if (string_size > static_cast<unsigned int>(size)) {
                bSharedString *next_string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + size * 8);
                next_string->Size = string->Size - static_cast<unsigned short>(size);
                next_string->Prev = static_cast<unsigned short>((reinterpret_cast<char *>(next_string) - reinterpret_cast<char *>(string)) >> 3);
                next_string->Count = string_count;
                next_string->String[0] = '\0';

                bSharedString *next_next_string =
                    reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(next_string) + next_string->Size * 8);
                if (next_next_string != reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(this->StringTable) + (this->StringTableSize << 3))) {
                    next_next_string->Prev = static_cast<unsigned short>((reinterpret_cast<char *>(next_next_string) - reinterpret_cast<char *>(next_string)) >> 3);
                }
            }

            string->Size = static_cast<unsigned short>(size);
            string->Count = 1;
            bStrCpy(string->String, s);
            this->FastLookupTable[hash_index] = string;
            this->FastLookupTableCount[hash_index] = this->FastLookupTableCount[hash_index] + 1;

            this->NumBytesAllocated += size * 8;
            if (this->NumBytesAllocated > this->MostBytesAllocated) {
                this->MostBytesAllocated = this->NumBytesAllocated;
            }

            this->NumStringsAllocated++;
            if (this->NumStringsAllocated > this->MostStringsAllocated) {
                this->MostStringsAllocated = this->NumStringsAllocated;
            }

            unsigned short allocated_size = string->Size;
            this->LargestFreeString = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + allocated_size * 8);
            if (this->LargestFreeString == reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(this->StringTable) + (this->StringTableSize << 3))) {
                this->LargestFreeString = this->StringTable;
            }

            this->Mutex.Unlock();
            return string->String;
        }

        string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + string_size * 8);
        if (string == reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(this->StringTable) + (this->StringTableSize << 3))) {
            string = this->StringTable;
        }
    } while (string != this->LargestFreeString);

    this->Mutex.Unlock();
    return nullptr;
}

void bSharedStringPool::Free(const char *s) {
    if (s == nullptr) {
        return;
    }

    intptr_t index = (reinterpret_cast<const char *>(s) - reinterpret_cast<const char *>(this->StringTable)) >> 3;
    short string_index;
    if ((index < 0) || (index >= this->StringTableSize)) {
        string_index = -1;
    } else {
        string_index = static_cast<short>(index);
    }

    if (string_index == -1) {
        return;
    }

    bSharedString *table_start = this->StringTable;
    bSharedString *string = table_start + string_index;

    this->Mutex.Lock();

    unsigned short count = string->Count - 1;
    string->Count = count;

    if (count != 0) {
        this->Mutex.Unlock();
        return;
    }

    int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);

    if (this->FastLookupTable[hash_index] == string) {
        this->FastLookupTable[hash_index] = 0;
    }

    this->FastLookupTableCount[hash_index] = this->FastLookupTableCount[hash_index] - 1;
    string->String[0] = 0;
    this->NumBytesAllocated -= string->Size * 8;
    this->NumStringsAllocated--;

    bSharedString *table_end = this->StringTable + this->StringTableSize;
    unsigned short size = string->Size;
    bSharedString *next_string = string + size;

    if ((next_string != table_end) && (next_string->Count == 0)) {
        string->Size = string->Size + next_string->Size;
        next_string = next_string + next_string->Size;
        if (next_string != table_end) {
            next_string->Prev = static_cast<unsigned short>(next_string - string);
        }
    }

    bSharedString *prev_string = string - string->Prev;
    if ((string != this->StringTable) && (prev_string->Count == 0)) {
        prev_string->Size = string->Size + prev_string->Size;
        next_string = string + string->Size;
        if (next_string != table_end) {
            next_string->Prev = static_cast<unsigned short>(next_string - prev_string);
        }
        string = prev_string;
    }

    if (string->Size > this->LargestFreeString->Size) {
        this->LargestFreeString = string;
    }

    this->Mutex.Unlock();
}

int bMatchNameWithWildcard(const char *wild, const char *string) {
    const char *cp = nullptr;
    const char *mp = nullptr;
 
    while ((*string != '\0') && (*wild != '*')) {
        char wild_char = *wild;
        int wild_int = static_cast<int>(wild_char);
        if ((bToUpper(wild_char) != bToUpper(*string)) && (wild_int != '?')) {
            return false;
        }

        string = string + 1;
        wild = wild + 1;
    }

    while (*string != '\0') {
        char wild_char = *wild;
        if (wild_char == '*') {
            wild = wild + 1;
            if (*wild == '\0') {
                return true;
            }

            mp = wild;
            cp = string + 1;
        } else {
            if ((bToUpper(wild_char) == bToUpper(*string)) || (wild_char == '?')) {
                wild = wild + 1;
                string = string + 1;
            } else {
                string = cp;
                wild = mp;
                cp = string + 1;
            }
        }
    }

    while (*wild == '*') {
        wild = wild + 1;
    }

    return *wild == '\0';
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
    char c = *s1;

    while (c != '\0') {
        if (bStrNICmp(s1, s2, len) == 0) {
            return const_cast<char *>(s1);
        }
        c = *++s1;
    }

    return nullptr;
}
