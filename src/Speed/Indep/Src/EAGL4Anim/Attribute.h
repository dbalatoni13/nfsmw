#ifndef EAGL4ANIM_ATTRIBUTE_H
#define EAGL4ANIM_ATTRIBUTE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "AttributeId.h"
#include "Speed/Indep/Libs/Support/Utility/UTypes.h"

namespace EAGL4Anim {

// total size: 0x8
struct Attribute {
    AttributeId mId;          // offset 0x0, size 0x2
    short unsigned int mSize; // offset 0x2, size 0x2
    void *mData;              // offset 0x4, size 0x4
};

// total size: 0xC
class AttributeMetaData {
  public:
    enum AttributeType {
        AT_INVALID = -1,
        AT_FLOAT = 0,
        AT_INT = 1,
        AT_UNSIGNEDINT = 2,
        AT_SHORT = 3,
        AT_UNSIGNEDSHORT = 4,
        AT_CHAR = 5,
        AT_UNSIGNEDCHAR = 6,
        AT_COORD3 = 7,
        AT_COORD4 = 8,
        AT_MAX = 9,
        AT_FITIN32BITS = 7,
        AT_USER = 9,
    };

    AttributeMetaData() {}

    AttributeMetaData(const char *name, short unsigned int type, short unsigned int size, AttributeId id)
        : mName(name), //
          mType(type), //
          mSize(size), //
          mId(id) {}

    const char *GetName() const {
        return mName;
    }

    short unsigned int GetType() const {
        return mType;
    }

    short unsigned int GetSize() const {
        return mSize;
    }

    AttributeId GetId() const {
        return mId;
    }

    void SetName(const char *name) {
        mName = name;
    }

    void SetType(short unsigned int type) {
        mType = type;
    }

    void SetSize(short unsigned int size) {
        mSize = size;
    }

    void SetId(AttributeId id) {
        mId = id;
    }

    ~AttributeMetaData() {}

    AttributeMetaData(const AttributeMetaData &_ctor_arg)
        : mName(_ctor_arg.mName), //
          mPad(_ctor_arg.mPad),   //
          mType(_ctor_arg.mType), //
          mSize(_ctor_arg.mSize), //
          mId(_ctor_arg.mId) {}

  private:
    const char *mName;        // offset 0x0, size 0x4
    short unsigned int mPad;  // offset 0x4, size 0x2
    short unsigned int mType; // offset 0x6, size 0x2
    short unsigned int mSize; // offset 0x8, size 0x2
    AttributeId mId;          // offset 0xA, size 0x2
};

// total size: 0xC
class AttributeBlock {
  public:
    int GetNumAttributes() const {
        return mNumAttributes;
    }

    // bool GetAttribute(AttributeId id, float &result) const {}

    // bool GetAttribute(AttributeId id, unsigned short &result) const {}

    // bool GetAttribute(AttributeId id, short &result) const {}

    // bool GetAttribute(AttributeId id, int &result) const {}

    // bool GetAttribute(AttributeId id, unsigned int &result) const {}

    // bool GetAttribute(AttributeId id, char &result) const {}

    // bool GetAttribute(AttributeId id, unsigned char &result) const {}

    // bool GetAttribute(AttributeId id, char *&result) const {}

    // int GetAttributeByteSize(AttributeId id) const {}

    bool GetAttribute(AttributeId id, UMath::Vector3 &result) const;

    bool GetAttribute(AttributeId id, UMath::Vector4 &result) const;

    bool GetAttribute(AttributeId id, float *result) const;

    bool GetAttribute(AttributeId id, UMath::Vector3 *result) const;

    bool GetAttribute(AttributeId id, UMath::Vector4 *result) const;

    const Attribute *FindAttribute(AttributeId aid) const;

    bool GetAttribute(AttributeId aid, void *&result) const;

  private:
    int mNumAttributes;       // offset 0x0, size 0x4
    Attribute mAttributes[1]; // offset 0x4, size 0x8
};

}; // namespace EAGL4Anim

#endif
