#include "AttribAsset.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"

VaultMap gVaults;
FileMap gFiles;

bool AddDepFile(const char *filename, void *data, size_t bytes) {
    Attrib::AssetID assetID = Attrib::StringToAssetID(filename);
    FileMap::iterator result = gFiles.find(assetID);

    if (result == gFiles.end()) {
        result = gFiles.insert(FileMap::value_type(assetID, FileRecord(data, bytes))).first;
        return true;
    } else {
        return false;
    }
}

bool RemoveDepFile(const char *filename) {
    Attrib::AssetID assetID = Attrib::StringToAssetID(filename);
    FileMap::iterator iter = gFiles.find(assetID);

    if ((*iter).second.mRefCount != 0) {
        return false;
    } else {
        gFiles.erase(iter);
        return true;
    }
}

class FileGarbageCollector : public Attrib::IGarbageCollector {
    void ReleaseData(Attrib::AssetID id, void *data, size_t bytes) override {
        FileMap::iterator iter = gFiles.find(id);
        (*iter).second.mRefCount--;
    }
};

class VaultGarbageCollector : public Attrib::IGarbageCollector {
    void ReleaseData(Attrib::AssetID id, void *data, size_t bytes) override {
        reinterpret_cast<Attrib::Vault *>(data)->Release();
    }
};

static FileGarbageCollector gFileCollector;
static VaultGarbageCollector gVaultCollector;

Attrib::Vault *AddVault(const char *filename, void *data, unsigned int bytes) {
    Attrib::AssetID headerID = Attrib::StringToAssetID(filename);

    VaultMap::const_iterator result = gVaults.find(headerID);
    if (result != gVaults.end()) {
        return (*result).second;
    }

    Attrib::AssetID assetID = Attrib::StringToAssetID(filename);
    FileMap::iterator fileIter = gFiles.insert(FileMap::value_type(assetID, FileRecord(data, bytes))).first;
    (*fileIter).second.mRefCount++;

    Attrib::Vault *vault = new Attrib::Vault(Attrib::Database::GetExportPolicies(), headerID, data, bytes, &gFileCollector);
    unsigned int numdeps;
    const unsigned int *dependencies = vault->GetDependencyList(numdeps);

    for (unsigned int i = 0; i < numdeps; i++) {
        Attrib::AssetID depID = dependencies[i];
        if (vault->IsAssetDependency(i)) {
            VaultMap::iterator vIter = gVaults.find(depID);
            if (vIter != gVaults.end()) {
                vault->AddRef();
                vault->ResolveDependency(i, (*vIter).second, 0, &gVaultCollector);
            } else {
                FileMap::iterator depIter = gFiles.find(depID);
                if (depIter != gFiles.end()) {
                    (*depIter).second.mRefCount++;
                    vault->ResolveDependency(i, (*depIter).second.mData, (*depIter).second.mBytes, &gFileCollector);
                } else {
                    vault->ResolveDependency(i, 0, 0, 0);
                }
            }
        }
    }

    vault->Initialize();
    gVaults.insert(VaultMap::value_type(headerID, vault));
    return vault;
}

void RemoveVault(const char *filename) {
    Attrib::AssetID headerID = Attrib::StringToAssetID(filename);
    VaultMap::iterator existing = gVaults.find(headerID);
    if (existing != gVaults.end()) {
        gVaults.erase(existing);
    }
    RemoveDepFile(filename);
}
