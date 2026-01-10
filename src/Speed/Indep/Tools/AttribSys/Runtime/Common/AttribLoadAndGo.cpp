#include "Speed/Indep/Tools/AttribSys/Runtime/AttribLoadAndGo.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

#include <algorithm>

namespace Attrib {

AssetID StringToAssetID(const char *assetName) {
    return StringToKey(assetName);
}

Key StringToTypeID(const char *typeName) {
    return StringToKey(typeName);
}

ExportManager::ExportManager(unsigned int reserve) {
    mExportPolicys = nullptr;
    mCount = 0;
    mReserve = reserve;
    mExportPolicys = reinterpret_cast<ExportPolicyPair *>(Alloc(reserve * sizeof(ExportPolicyPair), "Attrib::ExportPolicyPair"));
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
    return 0;
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
    DataBlock() : mData(nullptr), mSize(0), mGC(nullptr), mPolicyIndex(static_cast<unsigned int>(-1)) {}

    bool Inited() const {
        return mData != nullptr;
    }

    void Set(void *data, std::size_t bytes, IGarbageCollector *gc) {
        mSize = bytes;
        mData = data;
        mGC = gc;
    }

    void Clear(unsigned int id) {
        if (Inited()) {
            mGC->ReleaseData(id, mData, mSize);
        }
        mSize = 0;
        mData = nullptr;
        mGC = nullptr;
    }

    void *operator new(std::size_t, void *ptr) {
        return ptr;
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
    char *Data() {
        return reinterpret_cast<char *>(&this[1]);
    }

    ChunkBlock *Next() {
        // TODO
    }

    unsigned int mType; // offset 0x0, size 0x4
    unsigned int mSize; // offset 0x4, size 0x4
};

// total size: 0xC
struct DependencyNode : public ChunkBlock {
    unsigned int *GetAssetIDs() {
        return reinterpret_cast<unsigned int *>(&this[1]);
    }

    unsigned int mCount; // offset 0x8, size 0x4
};

// total size: 0xC
struct PtrRef {
    unsigned int mFixupOffset; // offset 0x0, size 0x4
    unsigned short mPtrType;   // offset 0x4, size 0x2
    unsigned short mIndex;     // offset 0x6, size 0x2
    union {
        unsigned int mExportID; // offset 0x0, size 0x4
        unsigned int mOffset;   // offset 0x0, size 0x4
    }; // offset 0x8, size 0x4
};

// total size: 0x8
struct PointerNode : public ChunkBlock {
    PtrRef *GetPointers() {
        return reinterpret_cast<PtrRef *>(&this[1]);
    }
};

// total size: 0xC
struct ExportNode : public ChunkBlock {
    ExportEntry *GetExports() {
        return reinterpret_cast<ExportEntry *>(&this[1]);
    }

    const char *GetNames(unsigned int numExports) {
        return reinterpret_cast<const char *>(&GetExports()[numExports]);
    }

    unsigned int mCount; // offset 0x8, size 0x4
};

Vault::Vault(ExportManager &mgr, unsigned int, void *data, std::size_t bytes, IGarbageCollector *gc)
    : mRefCount(1), mExportMgr(mgr), mDeinited(false), mDependencies(nullptr), mDepData(nullptr), mResolvedCount(0), mPointers(nullptr),
      mTransientData(nullptr), mStrings(nullptr), mExports(nullptr), mNumExports(0), mInited(false) {
    ChunkBlock *chunk = reinterpret_cast<ChunkBlock *>(data);
    ChunkBlock *endofdata = reinterpret_cast<ChunkBlock *>((uintptr_t)data + bytes);

    // TODO magic
    while (chunk < endofdata) {
        switch (chunk->mType) {
            case 0x4461744e:
                break;
            case 0x4465704e:
                mDependencies = reinterpret_cast<DependencyNode *>(chunk);
                break;
            case 0x5074724e:
                mPointers = reinterpret_cast<PointerNode *>(chunk);
                break;
            case 0x4578704e:
                mExports = reinterpret_cast<ExportNode *>(chunk);
                break;
            case 0x5374724e:
                mStrings = chunk;
                break;
        }
        chunk = reinterpret_cast<ChunkBlock *>((uintptr_t)chunk + chunk->mSize);
    }
    {
        const char *str = mStrings->Data();
        const char *end = &reinterpret_cast<char *>(mStrings)[mStrings->mSize];
        unsigned int stringcount = 0;
        while (str < end) {
            stringcount++;
            str = &str[strlen(str) + 1];
        }
        PrepareToAddStrings(stringcount);
        str = mStrings->Data();
        while (str < end) {
            RegisterString(str);
            str = &str[strlen(str) + 1];
        }
    }
    mTransientData = reinterpret_cast<unsigned char *>(data);
    mNumDependencies = mDependencies->mCount;
    mNumExports = mExports->mCount;
    mDepData = reinterpret_cast<DataBlock *>(Alloc((mNumDependencies + mNumExports) * sizeof(DataBlock), "Attrib::DataBlocks"));
    mExportData = &mDepData[mNumDependencies];

    for (unsigned int i = 0; i < mNumDependencies + mNumExports; i++) {
        new (&mDepData[i]) DataBlock();
    }
    mDepIDs = reinterpret_cast<unsigned int *>(Alloc((mNumDependencies + mNumExports) * sizeof(*mDepIDs), "Attrib::AssetIDs"));
    mExportIDs = &mDepIDs[mNumDependencies];
    for (unsigned int i = 0; i < mNumDependencies; i++) {
        mDepIDs[i] = mDependencies->GetAssetIDs()[i];
    }
    mDepData->Set(data, bytes, gc);
    mResolvedCount++;
}

Vault::~Vault() {
    if (mInited && !mDeinited) {
        Deinitialize();
    }
    ExportsCleared();
    Free(mDepData, (mNumDependencies + mNumExports) * sizeof(*mDepData), "Attrib::DataBlocks");
    Free(mDepIDs, (mNumDependencies + mNumExports) * sizeof(*mDepIDs), "Attrib::AssetIDs");
}

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

void Vault::Initialize() {
    char *fixuptarget = nullptr;
    unsigned int fixupsize;
    unsigned int depcount;
    bool endOfPtrs = false;

    HasUnresolvedDependency();
    const PtrRef *ptr = mPointers->GetPointers();
    do {
        char **targetptr = reinterpret_cast<char **>(&fixuptarget[ptr->mFixupOffset]);

        switch (ptr->mPtrType) {
            case 0:
            default:
                endOfPtrs = true;
                break;
            case 1:
                *targetptr = nullptr;
                break;
            case 2:
                fixuptarget = reinterpret_cast<char *>(mDepData[ptr->mIndex].mData);
                break;
            case 3:
                *targetptr = &reinterpret_cast<char *>(mDepData[ptr->mIndex].mData)[ptr->mOffset];
                break;
            case 4: {
                Vault *depVault = reinterpret_cast<Vault *>(mDepData[ptr->mIndex].mData);
                unsigned int exportIndex = depVault->FindExportID(ptr->mExportID);
                if (exportIndex != -1) {
                    *targetptr = reinterpret_cast<char *>(depVault->GetExportData(exportIndex));
                } else {
                    *targetptr = nullptr;
                }
                break;
            };
            case 5:
                *targetptr = const_cast<char *>(KeyToString(StringToKey(&mStrings->Data()[ptr->mOffset])));
                break;
        }
        ptr++;
    } while (!endOfPtrs);

    ExportEntry *exportTable = mExports->GetExports();
    const char *exportNames = mExports->GetNames(mNumExports);
    mExports = nullptr;
    for (unsigned int i = 0; i < mNumExports; i++) {
        mExportData[i].mPolicyIndex = mExportMgr.GetExportPolicyIndex(exportTable[i].mType);
        IExportPolicy *policy = mExportMgr.GetExportPolicyByIndex(mExportData[i].mPolicyIndex);
        if (policy) {
            mExportIDs[i] = exportTable[i].mID;
            policy->Initialize(*this, exportTable[i].mType, exportTable[i].mID, &exportNames[exportTable[i].mNameOffset],
                               &mTransientData[exportTable[i].mDataOffset], exportTable[i].mDataBytes);
        }
    }
    mDepData->Clear(mDepIDs[0]);
    mDependencies = nullptr;
    mPointers = nullptr;
    mInited = true;
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

void Vault::Deinitialize() {
    Clean();
    std::size_t i = mNumExports;
    while (i-- > 0) {
        IExportPolicy *policy = mExportMgr.GetExportPolicyByIndex(mExportData[i].mPolicyIndex);
        if (policy) {
            policy->Deinitialize(*this, mExportMgr.GetExportPolicyTypeByIndex(mExportData[i].mPolicyIndex), mExportIDs[i]);
        }
    }
    Database::Get().CollectGarbage();
    mDeinited = true;
    for (std::size_t d = 1; d < mNumDependencies; d++) {
        mDepData[d].Clear(mDepIDs[d]);
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
