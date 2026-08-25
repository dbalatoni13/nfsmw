#include <stdlib.h>

enum LOCALESTATE {
    LOCALE_LANGUAGEID = 0,
    LOCALE_LANGUAGECOUNT = 1,
    LOCALE_STRINGCOUNT = 2,
    LOCALE_HASINDEX = 3,
};

struct LOCALE_HEADER {
    unsigned int chunkid;
    unsigned int chunksize;
    unsigned int flags;
    unsigned short languagecount;
    unsigned short languageid;
};

struct LOCALE_INDEX {
    unsigned int chunkid;
    unsigned int chunksize;
    unsigned int stringcount;
    unsigned int pad;
};

struct LOCALE_LANGUAGE {
    unsigned int chunkid;
    unsigned int chunksize;
    unsigned int languageid;
    unsigned int stringcount;
};

static inline unsigned int little_get(const void *src, int n) {
    if (n == 4) {
        return (static_cast<const unsigned char *>(src)[3] << 24) |
            (static_cast<const unsigned char *>(src)[2] << 16) |
            (static_cast<const unsigned char *>(src)[1] << 8) |
            static_cast<const unsigned char *>(src)[0];
    } else if (n == 2) {
        return (static_cast<const unsigned char *>(src)[1] << 8) |
            static_cast<const unsigned char *>(src)[0];
    } else if (n == 1) {
        return static_cast<const unsigned char *>(src)[0];
    }
    return 0;
}

static inline void little_put(void *dst, unsigned int data, int n) {
    if (n == 2) {
        static_cast<unsigned char *>(dst)[0] = static_cast<unsigned char>(data);
        static_cast<unsigned char *>(dst)[1] = static_cast<unsigned char>(data >> 8);
    } else if (n == 1) {
        static_cast<unsigned char *>(dst)[0] = static_cast<unsigned char>(data);
    }
}

int LOCALE_getstate(const void *localefile, LOCALESTATE state) {
    int value;
    int languageID;
    unsigned int offset;
    const LOCALE_LANGUAGE *lang;
    const LOCALE_HEADER *header;
    const char *lf;

    lf = static_cast<const char *>(localefile);
    header = reinterpret_cast<const LOCALE_HEADER *>(localefile);
    switch (state) {
    case LOCALE_LANGUAGEID:
        value = little_get(&header->languageid, 2);
        break;
    case LOCALE_LANGUAGECOUNT:
        value = little_get(&header->languagecount, 2);
        break;
    case LOCALE_STRINGCOUNT:
        languageID = little_get(&header->languageid, 2);
        offset = little_get(lf + 0x10 + languageID * 4, 4);
        lang = reinterpret_cast<const LOCALE_LANGUAGE *>(lf + offset);
        value = little_get(&lang->stringcount, 4);
        break;
    case LOCALE_HASINDEX:
        value = little_get(&header->flags, 4) & 1;
        break;
    default:
        value = 0;
        break;
    }
    return value;
}

void LOCALE_setstate(void *localefile, LOCALESTATE state, int value) {
    LOCALE_HEADER *header;

    header = static_cast<LOCALE_HEADER *>(localefile);
    if (state == LOCALE_LANGUAGEID) {
        little_put(&header->languageid, value, 2);
    }
}

static int compare(const void *arg1, const void *arg2) {
    const unsigned short *s1;
    const unsigned short *s2;

    s1 = static_cast<const unsigned short *>(arg1);
    s2 = static_cast<const unsigned short *>(arg2);
    return *s1 - little_get(s2, 2);
}

static int getstringidbyindex(unsigned short key, const LOCALE_INDEX *index) {
    int stringid;
    unsigned short *result;
    const unsigned char *base;

    base = reinterpret_cast<const unsigned char *>(index) + 0x10;
    result = static_cast<unsigned short *>(bsearch(
        &key, base, little_get(&index->stringcount, 4), 4, compare));
    if (result != 0) {
        result++;
        stringid = little_get(result, 2);
    } else {
        stringid = -1;
    }
    return stringid;
}

const char *LOCALE_getstrA(const void *localefile, int stringid) {
    const char *p;
    const LOCALE_HEADER *header;
    const LOCALE_LANGUAGE *bank;
    unsigned int offset;
    const char *lf;

    lf = static_cast<const char *>(localefile);
    header = reinterpret_cast<const LOCALE_HEADER *>(localefile);
    if (LOCALE_getstate(localefile, LOCALE_HASINDEX) != 0) {
        const LOCALE_INDEX *index;

        index = reinterpret_cast<const LOCALE_INDEX *>(
            lf + little_get(&header->chunksize, 4));
        stringid = getstringidbyindex(static_cast<unsigned short>(stringid), index);
    }
    bank = reinterpret_cast<const LOCALE_LANGUAGE *>(
        lf + little_get(lf + 0x10 + (little_get(&header->languageid, 2) << 2), 4));
    if (stringid < 0) {
        p = 0;
    } else {
        if (stringid < static_cast<int>(little_get(&bank->stringcount, 4))) {
            p = reinterpret_cast<const char *>(bank) +
                little_get(reinterpret_cast<const char *>(bank) + 0x10 + stringid * 4, 4);
        } else {
            p = 0;
        }
    }
    return p;
}

void *LOCALE_create(void *localefile, int unicode) {
    const LOCALE_HEADER *header;
    int languagecount;
    int lang;
    int str;

    if (unicode != 0) {
        header = reinterpret_cast<const LOCALE_HEADER *>(localefile);
        languagecount = little_get(&header->languagecount, 2);
        for (lang = 0; lang < languagecount; lang++) {
            unsigned int offset;
            const LOCALE_LANGUAGE *bank;
            int stringcount;

            offset = little_get(reinterpret_cast<const char *>(header) + 0x10 + lang * 4, 4);
            bank = reinterpret_cast<const LOCALE_LANGUAGE *>(
                reinterpret_cast<const char *>(header) + offset);
            stringcount = little_get(&bank->stringcount, 4);
            for (str = 0; str < stringcount; str++) {
                unsigned int offset;
                unsigned short *p;

                offset = little_get(reinterpret_cast<const char *>(bank) + 0x10 + str * 4, 4);
                p = reinterpret_cast<unsigned short *>(
                    const_cast<char *>(reinterpret_cast<const char *>(bank)) + offset);
                while (*p != 0) {
                    *p = static_cast<unsigned short>(little_get(p, 2));
                    p++;
                }
            }
        }
    }
    return localefile;
}
