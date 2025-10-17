#ifndef SUPPORT_UTILITY_UEALIBS_H
#define SUPPORT_UTILITY_UEALIBS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct TagStruct {
    unsigned int tag; // offset 0x0, size 0x4
    unsigned int data[3]; // offset 0x4, size 0xC
};

class UData {
public:
    // total size: 0x10
    unsigned int fTag; // offset 0x0, size 0x4
    unsigned int fIndexed : 1; // offset 0x4, size 0x4
    unsigned int fEmbedded : 1; // offset 0x4, size 0x4
    unsigned int fAllocated : 1; // offset 0x4, size 0x4
    unsigned int fSection : 5; // offset 0x4, size 0x4
    unsigned int fSize : 24; // offset 0x4, size 0x4
    unsigned int fCount; // offset 0x8, size 0x4
    union {
        void * fPointer; // offset 0x0, size 0x4
        unsigned int fOffset; // offset 0x0, size 0x4
    }; // offset 0xC, size 0x4
};

struct UGroup {
    // total size: 0x10
    struct SerializationControls {
        // total size: 0xC
        unsigned int fSuppressDuplicates; // offset 0x0, size 0x4
        unsigned int fAlign[2]; // offset 0x4, size 0x8
    };
    class Processor {
    private:
        // total size: 0xC
        bool mSkipData; // offset 0x0, size 0x1
        bool mSkipGroups; // offset 0x4, size 0x1
    };
    unsigned int fTag; // offset 0x0, size 0x4
    unsigned int fIndexed : 1; // offset 0x4, size 0x4
    unsigned int fEmbedded : 1; // offset 0x4, size 0x4
    unsigned int fAllocated : 1; // offset 0x4, size 0x4
    unsigned int fDataSorted : 1; // offset 0x4, size 0x4
    unsigned int fGroupSorted : 1; // offset 0x4, size 0x4
    unsigned int fGroupCount : 27; // offset 0x4, size 0x4
    unsigned int fDataCount; // offset 0x8, size 0x4
    union {
        void * fPointer; // offset 0x0, size 0x4
        unsigned int fOffset; // offset 0x0, size 0x4
    }; // offset 0xC, size 0x4
};

inline unsigned int UDataGroupType(unsigned int tag) {
    return tag >> 16;
}

inline unsigned int UDataGroupIndex(unsigned int tag) {
    return tag & 0xFFFF;
}

#endif
