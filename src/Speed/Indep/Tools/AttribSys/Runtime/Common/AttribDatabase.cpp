#include "../AttribHash.h"
#include "../AttribSys.h"
#include "AttribPrivate.h"
#include "dolphin/types.h"

namespace Attrib {

typedef unsigned int TypeID;
typedef unsigned int ExportID;

static bool gDatabaseSelfDestruct = false;

// total size: 0x4
class DatabaseExportPolicy : public IExportPolicy {
  public:
    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::DatabaseExportPolicy");
    }

    // Inline overrides
    override virtual void Initialize(Vault &v, const TypeID &type, const ExportID &id, const char *name, void *data, std::size_t bytes) {}

    override virtual bool IsReferenced(const Vault &v, const TypeID &type, const ExportID &id) {
        std::size_t index = v.FindExportID(id);
        DatabasePrivate *db = reinterpret_cast<DatabasePrivate *>(v.GetExportType(index));
        if (db) {
            return db->mClasses.Size() != 0;
        } else {
            return 0;
        }
    }

    override virtual void Clean(Vault &v, const TypeID &type, const ExportID &id) {}

    override virtual void Deinitialize(Vault &v, const TypeID &type, const ExportID &id) {
        Database::Get().CollectGarbage();
        v.Export(id, nullptr, 0);
        v.ExportsCleared();
        v.Release();
        gDatabaseSelfDestruct = true;
    }
};

// total size: 0x4
class ClassExportPolicy : public IExportPolicy {
  public:
    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::ClassExportPolicy");
    }

    // Inline overrides
    override virtual void Initialize(Vault &v, const TypeID &type, const ExportID &id, const char *name, void *data, std::size_t bytes) {}

    override virtual bool IsReferenced(const Vault &v, const TypeID &type, const ExportID &id) {
        std::size_t index = v.FindExportID(id);
        ClassPrivate *c = reinterpret_cast<ClassPrivate *>(v.GetExportData(index));
        if (c) {
            return c->GetRefs() > 1;
        } else {
            return false;
        }
    }

    override virtual void Clean(Vault &v, const TypeID &type, const ExportID &id) {}

    override virtual void Deinitialize(Vault &v, const TypeID &type, const ExportID &id) {
        std::size_t index = v.FindExportID(id);
        Class *c = reinterpret_cast<Class *>(v.GetExportData(index));
        if (c) {
            v.Export(id, nullptr, 0);
            c->Release();
        }
    }
};

// total size: 0x4
class CollectionExportPolicy : public IExportPolicy {
  public:
    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::CollectionExportPolicy");
    }

    // Inline overrides
    override virtual void Initialize(Vault &v, const TypeID &type, const ExportID &id, const char *name, void *data, std::size_t bytes) {
        const CollectionLoadData *loadData = reinterpret_cast<CollectionLoadData *>(data);

        Attrib::Class *container = Database::Get().GetClass(loadData->mClass);
        if (container && !container->GetCollection(loadData->mKey)) {
            Collection *c = new Collection(*loadData, &v);
            unsigned int exportIndex = v.FindExportID(id);
            if (name) {
                c->SetName(name);
            }
            v.Export(id, c, 0);
        }
    }

    override virtual bool IsReferenced(const Vault &v, const TypeID &type, const ExportID &id) {
        std::size_t index = v.FindExportID(id);
        Collection *c = reinterpret_cast<Collection *>(v.GetExportData(index));
        if (c) {
            return c->GetRefs() > 1;
        } else {
            return false;
        }
    }

    override virtual void Clean(Vault &v, const TypeID &type, const ExportID &id) {
        std::size_t index = v.FindExportID(id);
        Collection *c = reinterpret_cast<Collection *>(v.GetExportData(index));
        if (c) {
            c->Clean();
        }
    }

    override virtual void Deinitialize(Vault &v, const TypeID &type, const ExportID &id) {
        std::size_t index = v.FindExportID(id);
        Collection *c = reinterpret_cast<Collection *>(v.GetExportData(index));
        if (c) {
            v.Export(id, nullptr, 0);
            c->Release();
        }
    }
};

Database *Database::sThis = nullptr;
static unsigned int gDatabaseType;
static DatabaseExportPolicy *gDatabaseExportPolicy = nullptr;
static unsigned int gClassType;
static ClassExportPolicy *gClassExportPolicy = nullptr;
static unsigned int gCollectionType;
static CollectionExportPolicy *gCollectionExportPolicy = nullptr;
static ExportManager *gExportPolicies = nullptr;

ExportManager &Database::GetExportPolicies() {
    if (!gExportPolicies) {
        gExportPolicies = new ExportManager(3);

        gDatabaseExportPolicy = new DatabaseExportPolicy();
        gClassExportPolicy = new ClassExportPolicy();
        gCollectionExportPolicy = new CollectionExportPolicy();

        gExportPolicies->AddExportPolicy(gDatabaseType, gDatabaseExportPolicy);
        gExportPolicies->AddExportPolicy(gClassType, gClassExportPolicy);
        gExportPolicies->AddExportPolicy(gCollectionType, gCollectionExportPolicy);
        gExportPolicies->Seal();
    }
    return *gExportPolicies;
}

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
