//
//
#ifndef EVENTSYS_DATA_H
#define EVENTSYS_DATA_H

// En el original este tipo vive dentro de CARP: el unico simbolo que lo menciona
// es Q24CARP9QueryDesc. Declararlo en el ambito global hacia que las plantillas
// instanciadas con el (SearchPackedBinaryTree) salieran con 9QueryDesc a secas.
namespace CARP {

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

} // namespace CARP

#endif
