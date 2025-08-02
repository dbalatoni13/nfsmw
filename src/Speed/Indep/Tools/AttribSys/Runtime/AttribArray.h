#ifndef ATTRIBSYS_ATTRIB_ARRAY_H
#define ATTRIBSYS_ATTRIB_ARRAY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// Credit: Brawltendo
namespace Attrib {

class Array {
  private:
    unsigned short mAlloc;
    unsigned short mCount;
    unsigned short mSize;
    unsigned short mEncodedTypePad;

    unsigned char mData[];

  public:
    // Returns the base location of this array's data
    unsigned char *BasePointer() {
        return reinterpret_cast<unsigned char *>(&this[1]);
    }

    void *Data(unsigned int byteindex, unsigned char *base) {
        return &BasePointer()[byteindex + *base];
    }
    bool IsReferences();
    unsigned int GetPad();
    void *GetData(unsigned int index);
};

} // namespace Attrib

#endif
