#ifndef FRONTEND_FEPACKAGEDATA_H
#define FRONTEND_FEPACKAGEDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

struct SlotPool;

// total size: 0x8
struct FEPackageRenderInfo {
    SlotPool *EpolySlotPool; // offset 0x0, size 0x4
    bool AllowOverflows;     // offset 0x4, size 0x1
};

// total size: 0x38
class FEPackageData : public bTNode<FEPackageData> {
  public:
    static int IsInScreenConstructor() { return mInScreenConstructor > 0; }

    bChunk *GetChunk() { return MyChunk; }

    struct FEPackage *GetPackage() { return pPackage; }

    bool IsCompressedChunk() { return DataChunk != nullptr; }

    void *GetDataChunk();

    unsigned int GetNameHash();

    bool IsActive() { return pScreen != nullptr; }

    FEPackageData(bChunk *chunk);
    virtual ~FEPackageData();
    void Activate(struct FEPackage *pkg, int arg);
    void Close();

    void SetPermanent(int flag) { IsPermanent = flag; }

    int GetPermanent() { return IsPermanent; }

    void SetArgument(int pArg) { mArg = pArg; }

    int GetArgument() { return mArg; }

    bool GetVisibility() { return IsVisible; }

    void SetVisibility(bool visible) { IsVisible = visible; }

    int GetLastKnownControlMask() { return LastKnownControlMask; }

    bool WasSetupForHotchunk() { return bWasSetupForHotchunk; }

    void SetupForHotchunk() { bWasSetupForHotchunk = true; }

    void ClearHotchunk() { bWasSetupForHotchunk = false; }

    struct MenuScreen *GetScreen() { return pScreen; }

    void NotificationMessage(unsigned long Message, struct FEObject *pObject, unsigned long Param1, unsigned long Param2);
    void NotifySoundMessage(unsigned long msg, struct FEObject *obj, unsigned long control_mask, unsigned long pkg_ptr);
    void UnActivate();

    // struct FEPackageRenderInfo *GetRenderInfo() {}

  private:
    // Static members
    static int mInScreenConstructor; // size: 0x4, address: 0x8041CB60

    int LastKnownControlMask;              // offset 0x8, size 0x4
    int bWasSetupForHotchunk;              // offset 0xC, size 0x4
    void *DataChunk;                       // offset 0x10, size 0x4
    bChunk *MyChunk;                       // offset 0x14, size 0x4
    struct MenuScreen *pScreen;            // offset 0x18, size 0x4
    struct FEPackage *pPackage;            // offset 0x1C, size 0x4
    int16 IsPermanent;                     // offset 0x20, size 0x2
    int16 IsVisible;                       // offset 0x22, size 0x2
    struct ScreenFactoryDatum *CreateData; // offset 0x24, size 0x4
    struct FEPackageRenderInfo RenderInfo; // offset 0x28, size 0x8
    int mArg; // offset 0x30, size 0x4
};

#endif
