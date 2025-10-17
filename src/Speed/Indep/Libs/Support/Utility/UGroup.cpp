#include <cstdio>
#include "./UGroup.hpp"
#include "dolphin/types.h"

void UDataGroupEncodeTag(unsigned int tag, bool indexed, char *str) {
    unsigned int type;
    unsigned int i;

    *str++ = '{';
    
    if (indexed) {
        type = UDataGroupType(tag);
        i = 2;
    } else {
        type = tag;
        i = 4;
    }

    while (i-- > 0) {
        *str++ = type >> (i << 3);
    }
    if (indexed) {
        sprintf(str, "%04x", UDataGroupIndex(tag));
        str += 4;
    }

    *str++ = '}';
    *str++ = 0;
}

// STRIPPED
unsigned int UDataGroupDecodeTag(const char *str) {}

// UNSOLVED (i dont wanna do this vro 💀)
static unsigned int SearchTagArray(const TagStruct *&tagarray, unsigned int length, unsigned int tag) {
    if (length == 0) return 0;

    const TagStruct *p; // r10
    unsigned int spanlen; // r8
    unsigned int pos; // r9

    while (pos >= length) {
        p = tagarray;
        spanlen = length;

        while (spanlen != 0) {
            unsigned int pivot = spanlen / 2;
            unsigned int pivotTag = tagarray[pivot].tag;

            if (pivotTag > tag) {
                spanlen = length / 2;
            } else if (pivotTag < tag) {
                pos += 0x10;
                spanlen = pivot - (spanlen - 1);
                tagarray++;
            } else {
                if (pivot != 0 && tagarray[-1].tag == tag) {
                    while (--pivot) {
                        if (p[-1].tag != tag) break;
                    }
                    
                    tagarray = (const TagStruct *)(&p[pivot]);
                    return (unsigned int)(&p[pivot]);
                }
            }
        }

        // if ((tagarray[0] - r10) >> 4 > r4)
    }

    return nullptr;
}

static unsigned int SearchTagArray(const UData *&tagarray, unsigned int length, unsigned int tag) {
    return SearchTagArray(reinterpret_cast<const TagStruct *&>(tagarray), length, tag);
}

static unsigned int SearchTagArray(const UGroup *&tagarray, unsigned int length, unsigned int tag) {
    return SearchTagArray(reinterpret_cast<const TagStruct *&>(tagarray), length, tag);
}
