#include "../AttribHash.h"
#include "../AttribSys.h"
#include "AttribPrivate.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/Libs/Support/Utility/UVectorMath.h"
#include <types.h>

namespace Attrib {

typedef unsigned int TypeID;
typedef unsigned int ExportID;

static bool gDatabaseSelfDestruct = false;

// c36attx e2: movidas desde AttribPrivate.h -- es donde las tiene el
// original (attribdatabase.cpp:36 y :62 en debug_lines). De esto depende
// que el std::find(list) de QueueForDelete se PIDA desde el tercer .cpp y
// no desde la cabecera del primero: es el orden del bloque de plantillas.

// total size: 0x10
class ClassTable : public VecHashMap<unsigned int, Class, Class::TablePolicy, false, 16> {
  public:
    ClassTable(std::size_t capacity) : VecHashMap<unsigned int, Class, Class::TablePolicy, false, 16>(capacity) {}

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::ClassTable");
    }
};

// total size: 0x10
class TypeDescPtrVec : public std::vector<const TypeDesc *> {
    USE_ATTRIB_ALLOC("Attrib::TypeDescPtrVec");
};

// total size: 0x10
class TypeTable : public std::set<TypeDesc> {
    USE_ATTRIB_ALLOC("Attrib::TypeTable");
};

// total size: 0x8
class CollectionList : public std::list<const Collection *> {
    USE_ATTRIB_ALLOC("Attrib::CollectionList");
};

// total size: 0x8
class ClassList : public std::list<const Class *> {
    USE_ATTRIB_ALLOC("Attrib::ClassList");
};

class DatabaseLoadData {
  public:
    const unsigned int *GetTypeSizes() const {
        return (const unsigned int *)(&this[1]);
    }

    uint32_t mNumClasses;      // offset 0x0, size 0x4
    uint32_t mDefaultDataSize; // offset 0x4, size 0x4
    uint32_t mNumTypes;        // offset 0x8, size 0x4
    const char *mTypenames;    // offset 0xC, size 0x4
};

// total size: 0x4C
class DatabasePrivate : public Database {
  public:
    USE_ATTRIB_ALLOC("Attrib::DatabasePrivate");

    DatabasePrivate(const DatabaseLoadData &loadData) : Database(*this), mClasses(loadData.mNumClasses) {
        mClasses.Reserve(loadData.mNumClasses);
        mNumCompiledTypes = loadData.mNumTypes + 1;
        mCompiledTypes.reserve(mNumCompiledTypes);
        DefaultDataArea(loadData.mDefaultDataSize);
        mCompiledTypes.push_back(&*mTypes.insert(TypeDesc()).first);

        const unsigned int *sizes = loadData.GetTypeSizes();
        const char *name = loadData.mTypenames;

        for (unsigned int i = 0; i < loadData.mNumTypes; i++) {
            TypeTable::iterator iter = mTypes.insert(TypeDesc(name, sizes[i], mCompiledTypes.size())).first;
            mCompiledTypes.push_back(&*iter);
            name += strlen(name) + 1;
        }
    }

    ~DatabasePrivate() {
        mClasses.Size();
        mTypes.clear();
        mCompiledTypes.clear();
    }

    // c36attx e3: los cuatro estaticos van DETRAS del ctor/dtor: el
    // std::find(list) que pide QueueForDelete se instancia en el objetivo
    // DESPUES de reserve/_M_insert/insert_unique (ctor) y _M_erase/clear
    // (dtor), o sea que en el original se parsean despues.
    static void QueueForDelete(const Collection *obj, std::list<const Collection *> &bag) {
        obj->IsReferenced();
        if (std::find(bag.begin(), bag.end(), obj) == bag.end()) {
            bag.push_back(obj);
        }
    }

    static void QueueForDelete(const Class *obj, std::list<const Class *> &bag) {
        obj->IsReferenced();
        if (std::find(bag.begin(), bag.end(), obj) == bag.end()) {
            bag.push_back(obj);
        }
    }

    static void CollectGarbageBag(std::list<const Collection *> &bag) {
        std::list<const Collection *>::iterator iter = bag.begin();

        while (iter != bag.end()) {
            const Collection *obj = *iter;
            if (!obj->IsReferenced()) {
                obj->Delete();
            }
            bag.pop_front();
            iter = bag.begin();
        }
    }

    static void CollectGarbageBag(std::list<const Class *> &bag) {
        std::list<const Class *>::iterator iter = bag.begin();

        while (iter != bag.end()) {
            const Class *obj = *iter;
            if (!obj->IsReferenced()) {
                obj->Delete();
            }
            bag.pop_front();
            iter = bag.begin();
        }
    }

    ClassTable mClasses;                // offset 0x8, size 0x10
    unsigned int mNumCompiledTypes;     // offset 0x18, size 0x4
    TypeDescPtrVec mCompiledTypes;      // offset 0x1C, size 0x10
    TypeTable mTypes;                   // offset 0x2C, size 0x10
    CollectionList mGarbageCollections; // offset 0x3C, size 0x8
    ClassList mGarbageClasses;          // offset 0x44, size 0x8
};

// total size: 0x4
class DatabaseExportPolicy : public IExportPolicy {
  public:
    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::DatabaseExportPolicy");
    }

    // Inline overrides
    void Initialize(Vault &v, const TypeID &type, const ExportID &id, const char *name, void *data, std::size_t bytes) override {
        const DatabaseLoadData *loadData = reinterpret_cast<const DatabaseLoadData *>(data);
        Database::sThis = new DatabasePrivate(*loadData);
        v.AddRef();
        v.Export(id, Database::sThis, 0);
    }

    bool IsReferenced(const Vault &v, const TypeID &type, const ExportID &id) override {
        std::size_t index = v.FindExportID(id);
        DatabasePrivate *db = reinterpret_cast<DatabasePrivate *>(v.GetExportData(index));
        if (db) {
            return db->mClasses.Size() != 0;
        } else {
            return 0;
        }
    }

    void Clean(Vault &v, const TypeID &type, const ExportID &id) override {}

    void Deinitialize(Vault &v, const TypeID &type, const ExportID &id) override {
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
    void Initialize(Vault &v, const TypeID &type, const ExportID &id, const char *name, void *data, std::size_t bytes) override {
        const ClassLoadData *loadData = reinterpret_cast<ClassLoadData *>(data);
        if (!Database::Get().GetClass(loadData->mClass)) {
            ClassPrivate *c = new ClassPrivate(*loadData, &v);
            v.Export(id, c, 0);
        }
    }

    bool IsReferenced(const Vault &v, const TypeID &type, const ExportID &id) override {
        std::size_t index = v.FindExportID(id);
        ClassPrivate *c = reinterpret_cast<ClassPrivate *>(v.GetExportData(index));
        if (c) {
            return c->GetRefs() > 1;
        } else {
            return false;
        }
    }

    void Clean(Vault &v, const TypeID &type, const ExportID &id) override {}

    void Deinitialize(Vault &v, const TypeID &type, const ExportID &id) override {
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
    void Initialize(Vault &v, const TypeID &type, const ExportID &id, const char *name, void *data, std::size_t bytes) override {
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

    bool IsReferenced(const Vault &v, const TypeID &type, const ExportID &id) override {
        std::size_t index = v.FindExportID(id);
        Collection *c = reinterpret_cast<Collection *>(v.GetExportData(index));
        if (c) {
            return c->GetRefs() > 1;
        } else {
            return false;
        }
    }

    void Clean(Vault &v, const TypeID &type, const ExportID &id) override {
        std::size_t index = v.FindExportID(id);
        Collection *c = reinterpret_cast<Collection *>(v.GetExportData(index));
        if (c) {
            c->Clean();
        }
    }

    void Deinitialize(Vault &v, const TypeID &type, const ExportID &id) override {
        std::size_t index = v.FindExportID(id);
        Collection *c = reinterpret_cast<Collection *>(v.GetExportData(index));
        if (c) {
            v.Export(id, nullptr, 0);
            c->Release();
        }
    }
};

Database *Database::sThis = nullptr;

static unsigned int gDatabaseType = StringToTypeID("Attrib::DatabaseLoadData");
static DatabaseExportPolicy *gDatabaseExportPolicy = nullptr;

static unsigned int gClassType = StringToTypeID("Attrib::ClassLoadData");
static ClassExportPolicy *gClassExportPolicy = nullptr;

static unsigned int gCollectionType = StringToTypeID("Attrib::CollectionLoadData");
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
    while (mPrivates.mGarbageCollections.size() > 0 || mPrivates.mGarbageClasses.size() > 0) {
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

bool Database::AddClass(Class *c) {
    return mPrivates.mClasses.Add(c->GetKey(), c);
}

void Database::RemoveClass(const Class *c) {
    mPrivates.mClasses.Remove(c->GetKey());
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
