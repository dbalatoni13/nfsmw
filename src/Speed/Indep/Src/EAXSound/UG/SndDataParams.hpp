//
//
//
//
//
//
//
//
//
//
#ifndef SNDDATAPARAMS_H
#define SNDDATAPARAMS_H

#include "SndFileIOEnums.hpp"
#include "Speed/Indep/Libs/Support/Utility/UStandard.h"
#include "Speed/Indep/Src/EAXSound/States/STATE_Base.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// Decl: 53
enum eEVTSYS {
    EVTSYS_MAIN = 0,
    EVTSYS_FE = 1,
    EVTSYS_ENGINE = 2,
    EVTSYS_TURBO = 3,
    EVTSYS_ENVIRO = 4,
    EVTSYS_STITCH = 5,
    EVTSYS_SIRENS = 6,
    MAX_NFS_EVT_SYS = 7,
};

// Decl: 103
enum eNFSSndNOSClass {
    AEMS_NOS_00 = 0,
    AEMS_NOS_01 = 1,
    MAX_AEMS_NOS = 2,
};

// Decl: 190
enum eBANK_SLOT_TYPE {
    eBANK_SLOT_NONE = -1,
    eBANK_SLOT_AI_AEMS_ENGINE = 0,
    eBANK_SLOT_AI_GINA_ENGINE = 1,
    eBANK_SLOT_AI_GIND_ENGINE = 2,
    eBANK_SLOT_PATHFINDER = 3,
    eBANK_SLOT_MAX_NUM = 4,
};

DECLARE_CONTAINER_TYPE(ResAllocList);

// total size: 0x10
// Decl: 210
class ResAllocList : public UTL::Std::vector<uintptr_t, _type_ResAllocList> {
  public:
    ResAllocList() {} // Decl: 213

    ~ResAllocList() {}

    void *operator new(size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {}

    void operator delete(void *mem, const char *name) {} // Decl: 214

    void operator delete(void *mem, size_t size) {}

    void *operator new(size_t size, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {}
};

DECLARE_CONTAINER_TYPE(RefCountList);

// total size: 0x10
// Decl: 218
class RefCountList : public UTL::Std::vector<EAX_CarState *, _type_RefCountList> {
  public:
    RefCountList() {} // Decl: 221

    ~RefCountList() {}

    void *operator new(size_t size, void *ptr) {}

    void operator delete(void *mem, void *ptr) {}

    void *operator new(size_t size) {}

    void operator delete(void *mem, const char *name) {} // Decl: 222

    void operator delete(void *mem, size_t size) {}

    void *operator new(size_t size, const char *name) {}

    void operator delete(void *mem, size_t size, const char *name) {}
};

// total size: 0x20
// Decl: 233
struct stAssetDescription {
    // Decl: 234
    stAssetDescription() {
        Clear();
    }

    // TODO
    // Decl: 239
    void Clear() {
        this->eDataType = SDT_NONE;
        this->FileName = "";
        this->DataPath = SNDPATH_ROUTE;
    }

    eSNDDATATYPE eDataType;     // offset 0x0, size 0x4, Decl: 252
    Attrib::StringKey FileName; // offset 0x8, size 0x10, Decl: 253
    eSNDDATAPATH DataPath;      // offset 0x18, size 0x4, Decl: 254
    bool bLoadToTop;            // offset 0x1C, size 0x1, Decl: 255
};

// total size: 0x28
// Decl: 261
struct stSndAssetQueue {
    stSndAssetQueue() : pThis(nullptr), pCar(nullptr) {} // Decl: 262

    // Decl: 268
    bool operator==(const stSndAssetQueue &cmp) const {
        if (this->pThis != cmp.pThis) {
            return false;
        }

        return !(this->Asset.FileName != cmp.Asset.FileName);
    }

    stAssetDescription Asset; // offset 0x0, size 0x20, Decl: 279
    SndBase *pThis;           // offset 0x20, size 0x4, Decl: 280
    EAX_CarState *pCar;       // offset 0x24, size 0x4, Decl: 281
};

class stSndDataLoadParams;

// total size: 0x24
// Decl: 286
struct stBankSlot {
    // Decl: 287
    stBankSlot() {
        this->Clear();
    }

    // Decl: 292
    ~stBankSlot() {
        this->Clear();
    }

    // Decl: 298
    void Clear() {
        this->BANKmemLocation = 0;
        this->MAINmemLocation = nullptr;
        this->pLastAlloc = nullptr;
        this->Index = 0;
        this->pAssetParams = nullptr;
        this->BANKMemSize = 0;
        this->MAINmemSize = 0;
        this->Type = eBANK_SLOT_NONE;
        this->LoadFailed = 0;
    }

    eBANK_SLOT_TYPE Type;              // offset 0x0, size 0x4, Decl: 312
    int BANKmemLocation;               // offset 0x4, size 0x4, Decl: 313
    char *MAINmemLocation;             // offset 0x8, size 0x4, Decl: 314
    char *pLastAlloc;                  // offset 0xC, size 0x4, Decl: 315
    int MAINmemSize;                   // offset 0x10, size 0x4, Decl: 317
    int BANKMemSize;                   // offset 0x14, size 0x4, Decl: 318
    int LoadFailed;                    // offset 0x18, size 0x4, Decl: 319
    uint8 Index;                       // offset 0x1C, size 0x1, Decl: 321
    stSndDataLoadParams *pAssetParams; // offset 0x20, size 0x4, Decl: 322
};

// total size: 0x68
// Decl: 328
class stSndDataLoadParams {
  public:
    // Decl: 330
    stSndDataLoadParams() {
        this->Clear();
    }

    // Decl: 335
    void Clear() {
        this->AssetDescription.Clear();
        this->MemLocation = TMP_ALLOC_NONE;
        this->mBankSlot = nullptr;
        this->pmem = nullptr;
        this->plocmem = nullptr;
        this->nSize = 0;
        this->Handle = -1;
        this->bResolvedAsync = false;
        this->bResolvedSync = false;
        this->resallocs.clear();
        this->RefCount.clear();
        this->t_req = Timer(0);
        this->t_load = Timer(0);
    }

    // Decl: 361
    stSndDataLoadParams &operator=(stSndDataLoadParams &copy) {
        this->AssetDescription = copy.AssetDescription;
        this->MemLocation = copy.MemLocation;
        this->mBankSlot = copy.mBankSlot;
        if (this->mBankSlot != nullptr && this->mBankSlot->pAssetParams == &copy) {
            this->mBankSlot->pAssetParams = this;
        }

        this->pmem = copy.pmem;
        this->plocmem = copy.plocmem;
        this->nSize = copy.nSize;
        this->Handle = copy.Handle;
        this->bResolvedAsync = copy.bResolvedAsync;
        this->bResolvedSync = copy.bResolvedSync;

        this->resallocs.clear();
        this->resallocs.reserve(copy.resallocs.size());
        ResAllocList::const_iterator i;
        for (i = copy.resallocs.begin(); i != copy.resallocs.end(); ++i) {
            this->resallocs.push_back(*i);
        }
        copy.resallocs.clear();

        this->RefCount.clear();
        this->RefCount.reserve(copy.RefCount.size());
        RefCountList::iterator j;
        for (j = copy.RefCount.begin(); j != copy.RefCount.end(); ++j) {
            this->RefCount.push_back(*j);
        }
        copy.RefCount.clear();

        this->t_req = copy.t_req;
        this->t_load = copy.t_load;
        return *this;
    }

    stAssetDescription AssetDescription; // offset 0x0, size 0x20, Decl: 415
    eTEMPALLOCLOCATION MemLocation;      // offset 0x20, size 0x4, Decl: 416
    stBankSlot *mBankSlot;               // offset 0x24, size 0x4, Decl: 417
    void *pmem;                          // offset 0x28, size 0x4, Decl: 418
    void *plocmem;                       // offset 0x2C, size 0x4, Decl: 419
    int nSize;                           // offset 0x30, size 0x4, Decl: 420
    int Handle;                          // offset 0x34, size 0x4, Decl: 421
    bool bResolvedAsync;                 // offset 0x38, size 0x1, Decl: 422
    bool bResolvedSync;                  // offset 0x3C, size 0x1, Decl: 423
    ResAllocList resallocs;              // offset 0x40, size 0x10, Decl: 424
    RefCountList RefCount;               // offset 0x50, size 0x10, Decl: 425
    Timer t_req;                         // offset 0x60, size 0x4, Decl: 426
    Timer t_load;                        // offset 0x64, size 0x4, Decl: 427
};

#endif
