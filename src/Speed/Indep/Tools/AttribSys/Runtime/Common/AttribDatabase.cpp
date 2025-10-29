#include "../AttribHash.h"
#include "../AttribSys.h"
#include "AttribPrivate.h"

namespace Attrib {

Database::Database(DatabasePrivate &privates) : mPrivates(privates) {}

Database::~Database() {}

Class *Database::GetClass(Key k) const {
    return mPrivates.mClasses.Find(k);
}

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

void Database::Delete(const Class *c) {
    DatabasePrivate::QueueForDelete(c, mPrivates.mGarbageClasses);
}

static bool gDatabaseSelfDestruct = false;

void Database::CollectGarbage() {
    while (mPrivates.mGarbageCollections.size() != 0 || mPrivates.mGarbageClasses.size() != 0) {
        DatabasePrivate::CollectGarbageBag(mPrivates.mGarbageCollections);
        DatabasePrivate::CollectGarbageBag(mPrivates.mGarbageClasses);
    }
    if (gDatabaseSelfDestruct) {
        Get().DumpContents(0);
        delete sThis;
        sThis = nullptr;
        gDatabaseSelfDestruct = false;
    }
}

void Database::DumpContents(unsigned int classFilter) const {}

void PrepareToAddStrings(unsigned int numstrings) {}

Key RegisterString(const char *str) {
    Key k = StringToKey(str);
    return k;
}

const char *KeyToString(Key k) {
    return nullptr;
}

Key StringToKey(const char *str) {
    return StringHash32(str);
}

}; // namespace Attrib
