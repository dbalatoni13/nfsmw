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
    char *dest = to;

    while ((*dest++ = *from++) != '\0') {}
    return to;
}

unsigned short *bStrCpy(unsigned short *to, const unsigned short *from) {
    unsigned short *dest = to;

    while ((*dest++ = *from++) != 0) {}
    return to;
}

unsigned short *bStrCpy(unsigned short *to, const char *from) {
    unsigned short *dest = to;

    while ((*dest++ = static_cast<unsigned char>(*from++)) != 0) {}
    return to;
}

char *bStrNCpy(char *to, const char *from, int m) {
    int n = 0;
    while ((n < m) && (from[n] != '\0')) {
        to[n] = from[n];
        n++;
    }
    while (n < m) {
        to[n] = '\0';
        n++;
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
    while ((n < m) && (from[n] != '\0')) {
        to[n] = static_cast<unsigned char>(from[n]);
        n++;
    }
    while (n < m) {
        to[n] = 0;
        n++;
    }
    return to;
}

char *bSafeStrCpy(char *to, const char *from, int max_size) {
    int n = 0;

    if (max_size > 0) {
        while ((n < max_size - 1) && (from[n] != '\0')) {
            to[n] = from[n];
            n++;
        }
        to[n] = '\0';
    }

    return to;
}

int bStrCmp(const char *s1, const char *s2) {
    if (s1 == nullptr) {
        if (s2 != nullptr) {
            return -1;
        }
        return 0;
    }

    if (s2 != nullptr) {
        char c1;
        char c2;

        do {
            c1 = *s1++;
            c2 = *s2++;
        } while ((c1 != '\0') && (c2 != '\0') && (c1 == c2));

        return c1 - c2;
    }

    return 1;
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

    if (s2 != nullptr) {
        while (n != 0) {
            char c1 = *s1;
            char c2 = *s2;
            n--;

            if ((c1 == '\0') || (c2 == '\0')) {
                break;
            }

            s1++;
            c1 = bToUpper(c1);
            s2++;
            c2 = bToUpper(c2);
            if (c1 != c2) {
                break;
            }
        }

        if (n < 0) {
            return 0;
        }

        if (*s1 == '\0') {
            if (*s2 == '\0') {
                return bToUpper(s1[-1]) - bToUpper(s2[-1]);
            }

            return -1;
        }
    }

    return 1;
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
    if (s1 != nullptr) {
        if (s2 == nullptr) {
            return 1;
        }

        unsigned int c1;
        unsigned int c2;

        do {
            char ch = *s1;
            s1++;
            c1 = static_cast<unsigned int>(ch);
            if ((c1 - 'a') < 26) {
                c1 &= 0x5f;
            }

            ch = *s2;
            s2++;
            c2 = static_cast<unsigned int>(ch);
            if ((c2 - 'a') < 26) {
                c2 &= 0x5f;
            }
        } while ((c1 != 0) && (c2 != 0) && (c1 == c2));

        return c1 - c2;
    }

    if (s2 != nullptr) {
        return -1;
    }

    return 0;
}

char *bStrCat(char *to, const char *s1, const char *s2) {
    int n = 0;
    int nn = 0;

    while (s1[n] != '\0') {
        to[n] = s1[n];
        n++;
    }

    while (s2[nn] != '\0') {
        to[n + nn] = s2[nn];
        nn++;
    }

    to[n + nn] = '\0';
    return to;
}

char *bToUpper(char *s) {
    int n = 0;

    while (s[n] != '\0') {
        s[n] = bToUpper(s[n]);
        n++;
    }

    return s;
}

int bStrToLong(const char *s) {
    if ((s[0] == '0') && (s[1] == 'x')) {
        const unsigned char *p = reinterpret_cast<const unsigned char *>(s + 2);
        unsigned char c = *p;
        int value = 0;

        do {
            unsigned int digit;

            if (c == '\0') {
                return value;
            }

            digit = c - '0';
            if (digit > 9) {
                if ((c - 'a') < 26) {
                    c &= 0x5f;
                }
                if ((c - 'A') > 5) {
                    return value;
                }
                digit = c - '7';
            }

            p = p + 1;
            c = *p;
            value = value * 16 + digit;
        } while (true);
    }

    bool negative = false;
    char c = *s;

    if (c == '-') {
        negative = true;
        s = s + 1;
    } else if (c == '+') {
        s = s + 1;
    }

    int value = 0;

    if ((*s != '\0') && ((*s - '0') < 10U)) {
        int scaled = 0;
        char next;
        c = *s;

        do {
            s = s + 1;
            next = *s;
            value = scaled + c - '0';
            if (next == '\0') {
                break;
            }
            scaled = value * 10;
            c = next;
        } while ((next - '0') < 10U);
    }

    if (negative) {
        return -value;
    }

    return value;
}

float bStrToFloat(const char *s) {
    bool negative = false;

    if (*s == '-') {
        negative = true;
        s++;
    } else if (*s == '+') {
        s++;
    }

    float value = 0.0f;
    char current = *s;
    char test = *s;

    while ((int)test - '0' < 10U) {
        int digit = static_cast<int>(current);
        s++;
        current = *s;
        value = value * 10.0f + static_cast<float>(digit - '0');
        test = *s;
    }

    if (current == '.') {
        float fraction = 0.0f;
        float scale = 1.0f;

        s++;
        current = *s;
        test = *s;
        while ((int)test - '0' < 10U) {
            int digit = static_cast<int>(current);
            s++;
            current = *s;
            fraction = static_cast<float>(digit - '0') * scale * 0.1f + fraction;
            scale = scale * 0.1f;
            test = *s;
        }

        if (fraction != 0.0f) {
            fraction = static_cast<float>(static_cast<int>(fraction) + 1);
        }

        value = value + fraction;
    }

    if ((current == 'e') || (current == 'E')) {
        int exponent = bStrToLong(s + 1);
        float scale = 1.0f;

        if (exponent > 0) {
            do {
                scale = scale * 10.0f;
                exponent--;
            } while (exponent > 0);
        }

        if (exponent < 0) {
            do {
                scale = scale * 0.1f;
                exponent++;
            } while (exponent < 0);
        }

        value = value * scale;
    }

    if (negative) {
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

    bool search_table = true;
    char count = this->FastLookupTableCount[hash_index];
    if ((count == 0) || ((count == 1) && (string != nullptr))) {
        search_table = false;
    }

    bSharedString *table_start = this->StringTable;
    bSharedString *table_end = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(table_start) + this->StringTableSizeBytes);

    if (search_table) {
        for (string = table_start; string != table_end; string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + string->Size * 8)) {
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
        unsigned int string_size = string->Size;

        if ((string->Count == 0) && (size <= static_cast<int>(string_size))) {
            if (size < static_cast<int>(string_size)) {
                bSharedString *next_string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + size * 8);
                next_string->Size = string->Size - static_cast<unsigned short>(size);
                next_string->Prev = static_cast<unsigned short>((reinterpret_cast<char *>(next_string) - reinterpret_cast<char *>(string)) >> 3);
                next_string->Count = 0;
                next_string->String[0] = '\0';

                bSharedString *next_next_string =
                    reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(next_string) + next_string->Size * 8);
                if (next_next_string != table_end) {
                    next_next_string->Prev = static_cast<unsigned short>((reinterpret_cast<char *>(next_next_string) - reinterpret_cast<char *>(next_string)) >> 3);
                }
            }

            string->Size = static_cast<unsigned short>(size);
            string->Count = 1;
            bStrCpy(string->String, s);
            this->FastLookupTable[hash_index] = string;
            this->FastLookupTableCount[hash_index] = this->FastLookupTableCount[hash_index] + 1;

            this->NumBytesAllocated += size * 8;
            if (this->MostBytesAllocated < this->NumBytesAllocated) {
                this->MostBytesAllocated = this->NumBytesAllocated;
            }

            this->NumStringsAllocated++;
            if (this->MostStringsAllocated < this->NumStringsAllocated) {
                this->MostStringsAllocated = this->NumStringsAllocated;
            }

            unsigned short allocated_size = string->Size;
            this->LargestFreeString = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + allocated_size * 8);
            if (this->LargestFreeString == table_end) {
                this->LargestFreeString = table_start;
            }

            this->Mutex.Unlock();
            return string->String;
        }

        string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + string_size * 8);
        if (string == table_end) {
            string = table_start;
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
    short string_index = -1;

    if ((index >= 0) && (index < this->StringTableSize)) {
        string_index = static_cast<short>(index);
    }

    if (string_index == -1) {
        return;
    }

    bSharedString *table_start = this->StringTable;
    bSharedString *table_end = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(table_start) + this->StringTableSizeBytes);
    bSharedString *string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(this->StringTable) + string_index * 8);

    this->Mutex.Lock();

    unsigned short count = string->Count;
    string->Count = count - 1;

    if (static_cast<unsigned short>(count - 1) == 0) {
        int hash_index = static_cast<int>(bStringHash(s) & 0x7ff);

        if (this->FastLookupTable[hash_index] == string) {
            this->FastLookupTable[hash_index] = nullptr;
        }

        this->FastLookupTableCount[hash_index] = this->FastLookupTableCount[hash_index] - 1;
        string->String[0] = '\0';
        this->NumBytesAllocated -= string->Size * 8;
        this->NumStringsAllocated--;

        unsigned int size = string->Size;
        bSharedString *next_string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + size * 8);

        if ((next_string != table_end) && (next_string->Count == 0)) {
            string->Size = string->Size + next_string->Size;
            next_string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(next_string) + next_string->Size * 8);
            if (next_string != table_end) {
                next_string->Prev = static_cast<unsigned short>((reinterpret_cast<char *>(next_string) - reinterpret_cast<char *>(string)) >> 3);
            }
        }

        bSharedString *prev_string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) - string->Prev * 8);
        if ((string != table_start) && (prev_string->Count == 0)) {
            prev_string->Size = prev_string->Size + string->Size;
            next_string = reinterpret_cast<bSharedString *>(reinterpret_cast<char *>(string) + string->Size * 8);
            string = prev_string;
            if (next_string != table_end) {
                next_string->Prev = static_cast<unsigned short>((reinterpret_cast<char *>(next_string) - reinterpret_cast<char *>(prev_string)) >> 3);
            }
        }

        if (this->LargestFreeString->Size < string->Size) {
            this->LargestFreeString = string;
        }
    }

    this->Mutex.Unlock();
}

int bMatchNameWithWildcard(const char *wild, const char *string) {
    const char *saved_string = nullptr;
    const char *saved_wild = nullptr;
    char string_char = *string;

    while (string_char != '\0') {
        char wild_char = *wild;
        if (wild_char == '*') {
            if (string_char == '\0') {
                break;
            }

            const char *next_wild = wild + 1;
            const char *next_string = string;
            if (*next_wild == '\0') {
                return true;
            }

            saved_string = next_string + 1;
            saved_wild = next_wild;
            wild = next_wild;
            string = next_string;
        } else {
            int wild_int = static_cast<int>(wild_char);
            if (static_cast<unsigned int>(wild_int - 'a') < 0x1aU) {
                wild_char &= 0x5f;
            }

            unsigned int upper_string = static_cast<unsigned int>(*string);
            if (upper_string - 'a' < 0x1aU) {
                upper_string &= 0x5f;
            }

            if ((static_cast<int>(wild_char) != static_cast<int>(upper_string)) && (wild_int != '?')) {
                wild = saved_wild;
                string = saved_string;
                if (wild == nullptr) {
                    return false;
                }
                saved_string = string + 1;
            } else {
                wild = wild + 1;
                string = string + 1;
            }
        }

        string_char = *string;
    }

    while (*wild == '*') {
        wild = wild + 1;
    }

    return *wild == '\0';
}

char *bStrStr(const char *s1, const char *s2) {
    int len = bStrLen(s2);
    char c = *s1;

    while (true) {
        if (c == '\0') {
            return nullptr;
        }
        if ((c == *s2) && (bStrNCmp(s1, s2, len) == 0)) {
            break;
        }
        s1++;
        c = *s1;
    }

    return const_cast<char *>(s1);
}

char *bStrIStr(const char *s1, const char *s2) {
    int len = bStrLen(s2);
    char c = *s1;

    while (true) {
        if (c == '\0') {
            return nullptr;
        }
        if (bStrNICmp(s1, s2, len) == 0) {
            break;
        }
        s1++;
        c = *s1;
    }

    return const_cast<char *>(s1);
}
