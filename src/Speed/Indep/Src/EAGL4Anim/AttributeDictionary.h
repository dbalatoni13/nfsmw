#ifndef EAGL4ANIM_ATTRIBUTEDICTIONARY_H
#define EAGL4ANIM_ATTRIBUTEDICTIONARY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Attribute.h"

namespace EAGL4Anim {

// total size: 0x10
struct AttributeDictionary {
    AttributeDictionary() {}

    int GetNumUserAttributeMetaData() const {
        return mNumUserAttributes;
    }

    const AttributeMetaData *GetUserAttributeMetaData(int idx) const {
        return &mUserAttributes[idx];
    }

    const AttributeMetaData *GetAttributeMetaData(const char *name) const;

    const AttributeMetaData *GetAttributeMetaData(AttributeId id) const;

    bool GetAttributeId(const char *name, AttributeId &id) const;

    static const AttributeMetaData *GetReservedAttributeMetaData(AttributeId id);

    static const AttributeMetaData *GetReservedAttributeMetaData(const char *name);

    static AttributeMetaData mReservedAttributeMetaData[2]; // size: 0x18, address: 0x8045B118

    int mNumUserAttributes;               // offset 0x0, size 0x4
    AttributeMetaData mUserAttributes[1]; // offset 0x4, size 0xC
};

}; // namespace EAGL4Anim

#endif
