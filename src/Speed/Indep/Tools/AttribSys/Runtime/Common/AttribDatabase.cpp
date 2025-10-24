#include "../AttribSys.h"
#include "AttribPrivate.h"

namespace Attrib {

Database::Database(DatabasePrivate &privates) : mPrivates(privates) {}

Database::~Database() {}

unsigned int Database::GetNumIndexedTypes() const {
    return mPrivates.mNumCompiledTypes;
}

const TypeDesc &Database::GetIndexedTypeDesc(unsigned short index) const {
    unsigned int actualIndex = 0;
    if (index < mPrivates.mNumCompiledTypes) {
        actualIndex = index;
    }
    const TypeDesc &result = *mPrivates.mCompiledTypes[actualIndex];
    return result;
}

const TypeDesc &Database::GetTypeDesc(unsigned int t) const {
    TypeTable::iterator iter = mPrivates.mTypes.find(TypeDesc(t));
    if (iter != mPrivates.mTypes.end()) {
        return *iter;
    }
    return *mPrivates.mCompiledTypes[0];
}

void Database::Delete(const Collection *c) {
    DatabasePrivate::QueueForDelete(c, mPrivates.mGarbageCollections);
}

}; // namespace Attrib
