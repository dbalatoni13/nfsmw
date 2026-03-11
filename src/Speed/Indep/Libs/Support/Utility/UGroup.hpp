#ifndef SUPPORT_UTILITY_UEALIBS_H
#define SUPPORT_UTILITY_UEALIBS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

inline unsigned int UDataGroupTag(unsigned int type, unsigned int index) {
    return (type << 16) | index;
}

inline unsigned int UDataGroupType(unsigned int tag) {
    return tag >> 16;
}

inline unsigned int UDataGroupIndex(unsigned int tag) {
    return tag & 0xFFFF;
}

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

    unsigned int DataCount() const {
        return fCount;
    }

    const void *GetDataConst() const {
        if (fEmbedded) {
            return reinterpret_cast<const char *>(this) + fOffset;
        }
        return fPointer;
    }
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

    static const UGroup *Deserialize(unsigned int numParts, const unsigned int *dataLengths, const void **serializedData, unsigned int deltaAddress);
    unsigned int GroupCountType(unsigned int type) const;
    const UGroup *GroupLocateFirst(unsigned int type, unsigned int baseIndex, unsigned int maxIndex) const;
    const UGroup *GroupLocateTag(unsigned int typeIndexTag) const;
    const UData *DataLocateTag(unsigned int typeIndexTag) const;
    const void *GetArray() const;

    const UGroup *GroupBegin() const {
        return static_cast< const UGroup * >(GetArray());
    }

    const UGroup *GroupEnd() const {
        return GroupBegin() + fGroupCount;
    }

    const UData *DataBegin() const {
        return reinterpret_cast< const UData * >(GroupEnd());
    }

    const UData *DataEnd() const {
        return DataBegin() + fDataCount;
    }

    const UGroup *GroupLocate(unsigned int type, unsigned int index) const {
        return GroupLocateTag(UDataGroupTag(type, index));
    }

    const UData *DataLocate(unsigned int type, unsigned int index) const {
        return DataLocateTag(UDataGroupTag(type, index));
    }
};

#endif
