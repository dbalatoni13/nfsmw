#include "./UGroup.hpp"
#include "./UCrc.h"
#include <cstdio>


#include <types.h>

const UCrc32 UCrc32::kNull;

static inline void InitResolverData(UGroupResolverData &self, unsigned int numParts, const unsigned int *partOffsets, const void *const *partBases, unsigned int deltaAddress) {
    self.fNumParts = numParts;
    self.fPartOffsets = partOffsets;
    self.fPartBases = partBases;
    self.fDeltaAddress = deltaAddress;
}

inline void *UGroupResolverData::ConvertOffsetToPointer(const void *basePtr, unsigned int offset, void *pointer) const {
    if (fDeltaAddress == 0) {
        unsigned int baseOffset, dataOffset, p;

        baseOffset = reinterpret_cast<unsigned int>(basePtr) - reinterpret_cast<unsigned int>(fPartBases[0]);
        dataOffset = baseOffset + offset;

        p = 0;
        while (p + 1 < fNumParts && dataOffset >= fPartOffsets[p]) {
            dataOffset -= fPartOffsets[p];
            p++;
        }
        return reinterpret_cast<void *>(reinterpret_cast<unsigned int>(fPartBases[p]) + dataOffset);
    }

    if (pointer != 0) {
        return reinterpret_cast<void *>(reinterpret_cast<unsigned int>(pointer) + fDeltaAddress);
    }
    return 0;
}

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

static unsigned int SearchTagArray(const TagStruct *&tagarray, unsigned int length, unsigned int tag) {
    if (length == 0)
        return 0;

    const TagStruct *base = tagarray;
    const TagStruct *p = tagarray;
    unsigned int span = length;

    while (span != 0) {
        unsigned int half = span >> 1;
        unsigned int off = half << 4;
        const TagStruct *pivot = (const TagStruct *)((const char *)off + (unsigned int)p);
        unsigned int pt = *(const unsigned int *)((const char *)off + (unsigned int)p);
        if (pt > tag) {
            span = half;
        } else if (pt < tag) {
            p = (const TagStruct *)((const char *)p + (off + 0x10));
            unsigned int tmp = span - 1;
            span = tmp - half;
        } else {
            if (half != 0) {
                if (pivot[-1].tag == tag) {
                    do {
                        half = half - 1;
                        if (half == 0)
                            break;
                    } while ((p + half - 1)->tag == tag);
                }
            }
            tagarray = (const TagStruct *)((const char *)p + (half << 4));
            return tagarray->tag;
        }
    }

    unsigned int index = (unsigned int)(p - base);
    if (index < length) {
        if (p->tag < tag)
            index++;
        tagarray = base + index;
        if (index < length)
            return tagarray->tag;
        return 0;
    }
    tagarray = p;
    return 0;
}

static unsigned int SearchTagArray(const UData *&tagarray, unsigned int length, unsigned int tag) {
    return SearchTagArray(reinterpret_cast<const TagStruct *&>(tagarray), length, tag);
}

static unsigned int SearchTagArray(const UGroup *&tagarray, unsigned int length, unsigned int tag) {
    return SearchTagArray(reinterpret_cast<const TagStruct *&>(tagarray), length, tag);
}


// Decl: UEALibs GC
void UData::ResolveOffsets(const UGroupResolverData &resolverData) const {
    UData *mutableThis = const_cast<UData *>(this);
    mutableThis->fPointer = resolverData.ConvertOffsetToPointer(this, fOffset, fPointer);
    mutableThis->fEmbedded = 0;
}

const UGroup *UGroup::Deserialize(const void *serializedData, bool resolveOffsets, unsigned int deltaAddress) {
    const void *parts[1] = {serializedData};
    if (resolveOffsets) {
        const unsigned int lengths[1] = {0x40000000};
        return Deserialize(1, lengths, parts, deltaAddress);
    }
    return static_cast<const UGroup *>(serializedData);
}

const UGroup *UGroup::Deserialize(unsigned int numParts, const unsigned int *dataLengths, const void **serializedData, unsigned int deltaAddress) {
    UGroupResolverData resolver;
    const UGroup *group = static_cast<const UGroup *>(serializedData[0]);
    InitResolverData(resolver, numParts, dataLengths, serializedData, deltaAddress);
    group->ResolveOffsets(resolver);
    return group;
}

unsigned int UGroup::GroupCountType(unsigned int type) const {
    unsigned int count = 0;
    const UGroup *g = GroupBegin();
    while (g != GroupEnd()) {
        unsigned int tag;
        if (g->fIndexed) {
            tag = (g->fTag & 0xFFFF0000U) | 0x2020U;
        } else {
            tag = g->fTag;
        }
        if (tag == type)
            count++;
        g++;
    }
    return count;
}

const UGroup *UGroup::GroupLocateFirst(unsigned int type, unsigned int baseIndex, unsigned int maxIndex) const {
    const UGroup *result = GroupBegin();
    if (result != 0) {
        if (baseIndex == -1) {
            unsigned int resultKey = SearchTagArray(result, fGroupCount, type);
            if (resultKey != type)
                result = GroupEnd();
        } else {
            unsigned int resultKey = SearchTagArray(result, fGroupCount, UDataGroupTag(type, baseIndex));
            if (UDataGroupTag(resultKey, 0x2020U) != type || maxIndex < UDataGroupIndex(resultKey))
                result = GroupEnd();
        }
    }
    return result;
}

const UGroup *UGroup::GroupLocateTag(unsigned int typeIndexTag) const {
    const UGroup *result = GroupBegin();
    if (result != 0) {
        if (fGroupSorted) {
            unsigned int tag = SearchTagArray(result, fGroupCount, typeIndexTag);
            if (tag != typeIndexTag)
                result = GroupEnd();
        } else {
            const UGroup *end = GroupEnd();
            do {
                if (result >= end)
                    break;
                if (result->fTag == typeIndexTag)
                    break;
                result++;
            } while (true);
        }
    }
    return result;
}

unsigned int UGroup::DataCountType(unsigned int type) const {
    unsigned int count = 0;
    const UData *d = DataBegin();
    while (d != DataEnd()) {
        unsigned int tag;
        if (d->fIndexed) {
            tag = (d->fTag & 0xFFFF0000U) | 0x2020U;
        } else {
            tag = d->fTag;
        }
        if (tag == type)
            count++;
        d++;
    }
    return count;
}

const UData *UGroup::DataLocateFirst(unsigned int type, unsigned int baseIndex, unsigned int maxIndex) const {
    const UData *result = DataBegin();
    if (result != 0) {
        if (baseIndex == -1) {
            unsigned int resultKey = SearchTagArray(result, fDataCount, type);
            if (resultKey != type)
                result = DataEnd();
        } else {
            unsigned int resultKey = SearchTagArray(result, fDataCount, (type & 0xFFFF0000U) | baseIndex);
            if (((resultKey & 0xFFFF0000U) | 0x2020U) != type || maxIndex < (resultKey & 0xFFFFU))
                result = DataEnd();
        }
    }
    return result;
}

const UData *UGroup::DataLocateTag(unsigned int typeIndexTag) const {
    const UData *result = DataBegin();
    if (result != 0) {
        if (fDataSorted) {
            unsigned int tag = SearchTagArray(result, fDataCount, typeIndexTag);
            if (tag != typeIndexTag)
                result = DataEnd();
        } else {
            const UData *end = DataEnd();
            do {
                if (result >= end)
                    break;
                if (result->fTag == typeIndexTag)
                    break;
                result++;
            } while (true);
        }
    }
    return result;
}

void UGroup::ProcessBreadthFirst(UGroup::Processor &processor) const {
    bool keepProcessing;
    unsigned int index;

    keepProcessing = processor.StartGroup(this);

    index = 0;
    while (true) {
        if (!keepProcessing)
            break;
        unsigned int skipData = processor.mSkipData;
        processor.mSkipData = 0;
        if (skipData)
            break;
        if (index >= fDataCount)
            break;
        keepProcessing = processor.ProcessData(this, DataBegin() + index);
        index++;
    }

    if (keepProcessing) {
        index = 0;
        while (true) {
            unsigned int skipGroups = processor.mSkipGroups;
            processor.mSkipGroups = 0;
            if (skipGroups)
                break;
            if (index >= fGroupCount)
                break;
            const UGroup *g = GroupBegin() + index;
            g->ProcessBreadthFirst(processor);
            index++;
        }
    }
    processor.EndGroup(this);
}

void UGroup::ResolveOffsets(const UGroupResolverData &resolverData) const {
    UGroup *mutableThis = const_cast<UGroup *>(this);
    mutableThis->fPointer = resolverData.ConvertOffsetToPointer(this, fOffset * 16, fPointer);
    mutableThis->fEmbedded = 0;

    {
        const UGroup *g = GroupBegin();
        while (g != GroupEnd()) {
            g->ResolveOffsets(resolverData);
            g++;
        }
    }

    {
        const UData *d = DataBegin();
        while (d != DataEnd()) {
            d->ResolveOffsets(resolverData);
            d++;
        }
    }
}

const void *UGroup::GetArray() const {
    const void *result;
    if (fEmbedded) {
        result = this + fOffset;
    } else {
        result = fPointer;
    }
    return result;
}
