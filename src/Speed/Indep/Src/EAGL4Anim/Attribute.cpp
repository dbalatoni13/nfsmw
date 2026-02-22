#include "Attribute.h"
#include "AttributeId.h"

namespace EAGL4Anim {

const Attribute *AttributeBlock::FindAttribute(AttributeId aid) const {
    short unsigned int id = aid.GetId();
    int m;
    int s = 0;
    int e = mNumAttributes - 1;
    short unsigned int r;

    while (s <= e) {
        m = (s + e) >> 1;
        r = mAttributes[m].mId.GetId();

        if (id > r) {
            s = m + 1;
        } else if (id < r) {
            e = m - 1;
        } else {
            return (Attribute *)(&mAttributes[m]);
        }
    }

    return nullptr;
}

bool AttributeBlock::GetAttribute(AttributeId aid, void *&result) const {
    const Attribute *attrib = FindAttribute(aid);
    if (attrib) {
        result = attrib->mData;
        return true;
    } else {
        return false;
    }
}

}; // namespace EAGL4Anim
