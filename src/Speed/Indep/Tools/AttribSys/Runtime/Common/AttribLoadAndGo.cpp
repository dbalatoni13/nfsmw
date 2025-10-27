#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include <algorithm>

namespace Attrib {

Key StringToAssetID(const char *assetName) {
    return StringToKey(assetName);
}

Key StringToTypeID(const char *typeName) {
    return StringToKey(typeName);
}

ExportManager::ExportManager(unsigned int reserve) {
    mExportPolicys = nullptr;
    mCount = 0;
    mReserve = reserve;
    mExportPolicys = reinterpret_cast<ExportPolicyPair *>(Alloc(reserve * sizeof(ExportPolicyPair), nullptr));
}

void ExportManager::AddExportPolicy(unsigned int type, IExportPolicy *policy) {
    mExportPolicys[mCount++] = ExportPolicyPair(type, policy);
}

void ExportManager::Seal() {
    std::sort(mExportPolicys, &mExportPolicys[mCount]);
}

IExportPolicy *ExportManager::GetExportPolicyByIndex(unsigned int index) const {
    if (index < mCount) {
        return mExportPolicys[index].mPolicy;
    }
    return nullptr;
}

unsigned int ExportManager::GetExportPolicyTypeByIndex(unsigned int index) const {
    if (index < mCount) {
        return mExportPolicys[index].mType;
    }
    return nullptr;
}

unsigned int ExportManager::GetExportPolicyIndex(unsigned int type) const {
    ExportPolicyPair *result = std::lower_bound(mExportPolicys, &mExportPolicys[mCount], ExportPolicyPair(type, nullptr));
    if (result < &mExportPolicys[mCount]) {
        return result - mExportPolicys;
    } else {
        return static_cast<unsigned int>(-1);
    }
}

// total size: 0x10
struct DataBlock {
    bool Inited() const {
        return mData != nullptr;
    }

    void Set(void *data, std::size_t bytes, IGarbageCollector *gc) {
        mSize = bytes;
        mData = data;
        mGC = gc;
    }

    void *mData;               // offset 0x0, size 0x4
    IGarbageCollector *mGC;    // offset 0x4, size 0x4
    unsigned int mPolicyIndex; // offset 0x8, size 0x4
    unsigned int mSize;        // offset 0xC, size 0x4
};

// total size: 0x14
struct ExportEntry {
    unsigned int mID;         // offset 0x0, size 0x4
    unsigned int mType;       // offset 0x4, size 0x4
    unsigned int mNameOffset; // offset 0x8, size 0x4
    unsigned int mDataBytes;  // offset 0xC, size 0x4
    unsigned int mDataOffset; // offset 0x10, size 0x4
};

// total size: 0x8
struct ChunkBlock {
    unsigned int mType; // offset 0x0, size 0x4
    unsigned int mSize; // offset 0x4, size 0x4
};

// total size: 0xC
struct ExportNode : public ChunkBlock {
    ExportEntry *GetExports() {
        return exports;
    }

    unsigned int mCount; // offset 0x8, size 0x4
    ExportEntry exports[];
};

const unsigned int *Vault::GetDependencyList(unsigned int &count) const {
    count = mNumDependencies - 1;
    return &mDepIDs[1];
}

bool Vault::IsAssetDependency(unsigned int index) const {
    if (mDepIDs[index + 1] != 0) {
        return true;
    }
    return false;
}

void Vault::ResolveDependency(unsigned int index, void *data, std::size_t bytes, IGarbageCollector *gc) {
    if (!mDepData[index + 1].Inited()) {
        mResolvedCount++;
    }
    mDepData[index + 1].Set(data, bytes, gc);
}

bool Vault::HasUnresolvedDependency() const {
    return mResolvedCount < mNumDependencies;
}

void Vault::Clean() {
    unsigned int i = mNumExports;
    while (i-- > 0) {
        IExportPolicy *policy = mExportMgr.GetExportPolicyByIndex(mExportData[i].mPolicyIndex);
        if (policy) {
            policy->Clean(*this, mExportMgr.GetExportPolicyTypeByIndex(mExportData[i].mPolicyIndex), mExportIDs[i]);
        }
    }
}

void Vault::Export(const unsigned int &id, void *data, std::size_t bytes) {
    unsigned int index = FindExportID(id);
    CountExports();
    mExportData[index].Set(data, bytes, nullptr);
}

unsigned int Vault::CountExports() const {
    return mNumExports;
}

unsigned int Vault::FindExportID(unsigned int id) const {
    unsigned int result = std::find(mExportIDs, &mExportIDs[mNumExports], id) - mExportIDs;
    if (result < mNumExports) {
        return result;
    }
    return static_cast<unsigned int>(-1);
}

const unsigned int Vault::GetExportType(unsigned int index) const {
    CountExports();
    if (mExports) {
        ExportEntry *exportTable = mExports->GetExports();
        return exportTable[index].mType;
    } else {
        return mExportMgr.GetExportPolicyTypeByIndex(mExportData[index].mPolicyIndex);
    }
}

void *Vault::GetExportData(unsigned int index) const {
    CountExports();
    return mExportData[index].mData;
}

bool Vault::ExportsCleared() const {
    for (unsigned int i = 0; i < mNumExports; i++) {
        if (mExportData[i].mData) {
            return false;
        }
    }
    return true;
}

}; // namespace Attrib
