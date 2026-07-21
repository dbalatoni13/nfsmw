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
class ResAllocList : public UTL::Std::vector<unsigned int, _type_ResAllocList> {
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
    stAssetDescription(const stAssetDescription &_ctor_arg) {}

    stAssetDescription &operator=(const stAssetDescription &_ctor_arg) {}

    stAssetDescription() {} // Decl: 234

    void Clear() {} // Decl: 239

    eSNDDATATYPE eDataType;     // offset 0x0, size 0x4, Decl: 252
    Attrib::StringKey FileName; // offset 0x8, size 0x10, Decl: 253
    eSNDDATAPATH DataPath;      // offset 0x18, size 0x4, Decl: 254
    bool bLoadToTop;            // offset 0x1C, size 0x1, Decl: 255
};

// total size: 0x28
// Decl: 261
struct stSndAssetQueue {
    stSndAssetQueue(const stSndAssetQueue &_ctor_arg) {}

    stSndAssetQueue() {} // Decl: 262

    bool operator==(const stSndAssetQueue &cmp) const {} // Decl: 268

    stAssetDescription Asset; // offset 0x0, size 0x20, Decl: 279
    SndBase *pThis;           // offset 0x20, size 0x4, Decl: 280
    EAX_CarState *pCar;       // offset 0x24, size 0x4, Decl: 281
};

class stSndDataLoadParams;

// total size: 0x24
// Decl: 286
struct stBankSlot {
    stBankSlot() {} // Decl: 287

    ~stBankSlot() {} // Decl: 292

    void Clear() {} // Decl: 298

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
    stSndDataLoadParams() {} // Decl: 330

    void Clear() {} // Decl: 335

    stSndDataLoadParams &operator=(stSndDataLoadParams &copy) {} // Decl: 361

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
