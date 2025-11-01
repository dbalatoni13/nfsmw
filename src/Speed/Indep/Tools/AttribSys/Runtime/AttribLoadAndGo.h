#ifndef ATTRIBSYS_ATTRIB_LOAD_AND_GO_H
#define ATTRIBSYS_ATTRIB_LOAD_AND_GO_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/AttribAsset.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

namespace Attrib {

Key StringToAssetID(const char *assetName);
Key StringToTypeID(const char *typeName);

// total size: 0xC
class ExportManager {
  public:
    // total size: 0x8
    class ExportPolicyPair {
      public:
        ExportPolicyPair(unsigned int t, IExportPolicy *p) {
            mPolicy = p;
            mType = t;
        }

        ExportPolicyPair(const ExportPolicyPair &src) {
            mType = src.mType;
            mPolicy = src.mPolicy;
        }

        const ExportPolicyPair &operator=(const ExportPolicyPair &rhs) {
            mType = rhs.mType;
            mPolicy = rhs.mPolicy;
        }

        bool operator<(const ExportPolicyPair &rhs) const {
            return mType < rhs.mType;
        }

        unsigned int mType;     // offset 0x0, size 0x4
        IExportPolicy *mPolicy; // offset 0x4, size 0x4
    };

    ExportManager(unsigned int reserve);
    void AddExportPolicy(unsigned int type, IExportPolicy *policy);
    void Seal();
    IExportPolicy *GetExportPolicyByIndex(unsigned int index) const;
    unsigned int GetExportPolicyTypeByIndex(unsigned int index) const;
    unsigned int GetExportPolicyIndex(unsigned int type) const;

    void *operator new(std::size_t bytes) {
        return Alloc(bytes, "Attrib::ExportManager");
    }

    ExportPolicyPair *mExportPolicys; // offset 0x0, size 0x4
    unsigned int mReserve;            // offset 0x4, size 0x4
    std::size_t mCount;               // offset 0x8, size 0x4
};

struct ChunkBlock;
struct DataBlock;
struct DependencyNode;
struct ExportNode;
struct PointerNode;

// total size: 0x40
class Vault {
  public:
    Vault(ExportManager &mgr, unsigned int, void *data, std::size_t bytes, IGarbageCollector *gc);
    ~Vault();
    const unsigned int *GetDependencyList(unsigned int &count) const;
    bool IsAssetDependency(unsigned int index) const;
    void ResolveDependency(unsigned int index, void *data, std::size_t bytes, IGarbageCollector *gc);
    bool HasUnresolvedDependency() const;
    void Initialize();
    void Clean();
    void Deinitialize();
    void Export(const unsigned int &id, void *data, std::size_t bytes);
    unsigned int CountExports() const;
    unsigned int FindExportID(unsigned int id) const;
    const unsigned int GetExportType(unsigned int index) const;
    void *GetExportData(unsigned int index) const;
    bool ExportsCleared() const;

    void operator delete(void *ptr, std::size_t bytes) {
        Free(ptr, bytes, "Attrib::Vault");
    }

    bool Release() const {
        mRefCount--;
        if (mRefCount == 0) {
            delete this;
        }
        return mRefCount == 0;
    }

  private:
    mutable unsigned int mRefCount; // offset 0x0, size 0x4
    ExportManager &mExportMgr;      // offset 0x4, size 0x4
    DependencyNode *mDependencies;  // offset 0x8, size 0x4
    DataBlock *mDepData;            // offset 0xC, size 0x4
    unsigned int *mDepIDs;          // offset 0x10, size 0x4
    unsigned int mNumDependencies;  // offset 0x14, size 0x4
    unsigned int mResolvedCount;    // offset 0x18, size 0x4
    PointerNode *mPointers;         // offset 0x1C, size 0x4
    unsigned char *mTransientData;  // offset 0x20, size 0x4
    ChunkBlock *mStrings;           // offset 0x24, size 0x4
    ExportNode *mExports;           // offset 0x28, size 0x4
    DataBlock *mExportData;         // offset 0x2C, size 0x4
    unsigned int *mExportIDs;       // offset 0x30, size 0x4
    unsigned int mNumExports;       // offset 0x34, size 0x4
    bool mInited;                   // offset 0x38, size 0x1
    bool mDeinited;                 // offset 0x3C, size 0x1
};

}; // namespace Attrib

#endif
