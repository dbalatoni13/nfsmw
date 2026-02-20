#ifndef EAGL4ANIM_EAGL4SUPPORTDLOPEN_H
#define EAGL4ANIM_EAGL4SUPPORTDLOPEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "eagl4supportconspool.h"
#include "eagl4supportsympool.h"

namespace EAGL4 {

// total size: 0x30
class DynamicLoader {
  public:
    // total size: 0x10
    struct Symbol {
        Symbol() {}

        const char *name;   // offset 0x0, size 0x4
        const char *type;   // offset 0x4, size 0x4
        void *data;         // offset 0x8, size 0x4
        bool isInternalRef; // offset 0xC, size 0x1
    };

    static void SetVersionCheck(bool check) {}

    // void *operator new(size_t size) {}

    // void *operator new(size_t size, const char *msg) {}

    // void operator delete(void *ptr, size_t size) {}

    // void *operator new[](size_t size) {}

    // void *operator new[](size_t size, const char *msg) {}

    // void operator delete[](void *ptr, size_t size) {}

    // void *operator new(size_t, void *ptr) {}

    // bool IsResolved() {}

    // void *ELFAddr(unsigned int offset) {}

    DynamicLoader(void *d, unsigned int len, DynamicUserCallback pSearchFunction);

    DynamicLoader(void *d, unsigned int len, void *r, bool delayResolve, DynamicUserCallback pSearchFunction);

    bool DoVersionCheck();

    ~DynamicLoader();

    void Release();

    void RunConstructors();

    void RunDestructors();

    void Resolve();

    void ReResolve(void *pNewAddr);

    void Initialize(DynamicUserCallback pSearchFunction);

    bool GetAddr(const char *type, const char *name, void *&result) const;

    int GetIndex(const char *name) const;

    int GetCount() const;

    Symbol GetSymbol(int i) const;

    void *GetElfData() const;

    bool GetNextSymbol(const char *type, int &iIndex, Symbol &result) const;

    bool GetNextAddr(const char *type, int &iIndex, void *&addr) const;

    static const char *RegisterVar(const char *name, void *addr);

    static void UnRegisterVar(const char *name);

    static void *GetRegisteredVar(const char *name, bool &valid);

    void AddPatchAddress(void *pPatchAddress);

    static SymbolPool gSymbolPool;                            // size: 0x14, address: 0x8045B130
    static ConstructorPool gConsPool;                         // size: 0x28, address: 0x8045B144
    static RuntimeAllocConstructorPool gRuntimeAllocConsPool; // size: 0x28, address: 0x8045B16C

    static const char ModelType[];           // size: 0x0, address: 0xFFFFFFFF
    static const char BBoxType[];            // size: 0x0, address: 0xFFFFFFFF
    static const char TARType[];             // size: 0x0, address: 0xFFFFFFFF
    static const char ShapeType[];           // size: 0x0, address: 0xFFFFFFFF
    static const char AnimBankType[];        // size: 0x0, address: 0x803D428C
    static const char BoneType[];            // size: 0x0, address: 0xFFFFFFFF
    static const char SkeletonType[];        // size: 0x0, address: 0x803D42A4
    static const char MorphType[];           // size: 0x0, address: 0xFFFFFFFF
    static const char VersionType[];         // size: 0x0, address: 0xFFFFFFFF
    static const char AnimClipSetType[];     // size: 0x0, address: 0xFFFFFFFF
    static const char AnimIdType[];          // size: 0x0, address: 0xFFFFFFFF
    static const char PosePaletteBankType[]; // size: 0x0, address: 0xFFFFFFFF

  private:
    void *handle;                                         // offset 0x0, size 0x4
    int nDestructors;                                     // offset 0x4, size 0x4
    DestructorEntry *destructors;                         // offset 0x8, size 0x4
    RuntimeAllocDestructorEntry *RuntimeAllocDestructors; // offset 0xC, size 0x4
    char *mpData;                                         // offset 0x10, size 0x4
    unsigned int mDataLen;                                // offset 0x14, size 0x4
    char *mpReloc;                                        // offset 0x18, size 0x4
    DynamicUserCallback mSearchCallback;                  // offset 0x1C, size 0x4
    int mNumPatchAddresses;                               // offset 0x20, size 0x4
    int mMaxPatchAddresses;                               // offset 0x24, size 0x4
    unsigned int **mpPatchAddresses32;                    // offset 0x28, size 0x4

    static const int kDefaultMaxPatchAddresses; // size: 0x4, address: 0xFFFFFFFF
    static bool VersionCheck;                   // size: 0x1, address: 0xFFFFFFFF

    bool mIsResolved; // offset 0x2C, size 0x1
};

}; // namespace EAGL4

#endif
