#include "AttribSys.h"
#include "Common/AttribPrivate.h"

namespace Attrib {

Attribute Collection::Get(const Instance &instance, Key attributeKey) const {
    const Collection *c;
    Node *node = GetNode(attributeKey, c);
    if (node) {
        return Attribute(instance, c, node);
    } else {
        return Attribute();
    }
}

void *Collection::GetData(Key attributeKey, std::size_t index) const {
    const Collection *c;
    Node *node = GetNode(attributeKey, c);
    if (node) {
        if (node->IsArray()) {
            return node->GetArray(c->mLayout)->GetData(index);
        } else {
            return node->GetPointer(c->mLayout);
        }
    } else {
        return nullptr;
    }
}

std::size_t Collection::Count() const {
    return mTable.Size() + mClass->mPrivates.mLayoutTable.Size();
}

Key Collection::FirstKey(bool &inLayout) const {
    Key k = ScanForValidKey(mTable, -1);
    if (k == 0) {
        k = ScanForValidKey(mClass->mPrivates.mLayoutTable, -1);
        inLayout = true;
    } else {
        inLayout = false;
    }
    return k;
}

}; // namespace Attrib
