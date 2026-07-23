//
//
#ifndef EVENTSYS_DATA_H
#define EVENTSYS_DATA_H

struct QueryDesc {
    unsigned int mQueryName;  // offset 0x0, size 0x4
    unsigned int mInputName;  // offset 0x4, size 0x4
    unsigned int mIndex;      // offset 0x8, size 0x4
    unsigned int mCount;      // offset 0xC, size 0x4
    unsigned int mDataOffset; // offset 0x10, size 0x4

    // QueryDesc() {}

    // QueryDesc(const struct QueryDesc & src) {}

    // QueryDesc(unsigned int qName, unsigned int iName, unsigned int index, unsigned int count, unsigned int offset) {}

    // bool operator<(const struct QueryDesc & rhs) const {}

    // bool operator==(const struct QueryDesc & rhs) const {}

    // bool operator!=(const struct QueryDesc & rhs) const {}

    bool operator<(unsigned int rhs) const {
        return this->mInputName < rhs;
    }

    bool operator==(unsigned int rhs) const {
        return this->mInputName == rhs;
    }

    bool operator!=(unsigned int rhs) const {
        return this->mInputName != rhs;
    }
};

#endif
