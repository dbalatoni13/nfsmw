#ifndef FRONTEND_FEPACKAGEDATA_H
#define FRONTEND_FEPACKAGEDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

// total size: 0x38
class FEPackageData : public bTNode<FEPackageData> {
  public:
    // static int IsInScreenConstructor() {}

    // bChunk *GetChunk() {}

    // struct FEPackage *GetPackage() {}

    // bool IsCompressedChunk() {}

    // bool IsActive() {}

    // void SetPermanent(int flag) {}

    // int GetPermanent() {}

    // void SetArgument(int pArg) {}

    // int GetArgument() {}

    // bool GetVisibility() {}

    // void SetVisibility(bool visible) {}

    // int GetLastKnownControlMask() {}

    // bool WasSetupForHotchunk() {}

    // void SetupForHotchunk() {}

    // void ClearHotchunk() {}

    // struct MenuScreen *GetScreen() {}

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
    // TODO
    // struct FEPackageRenderInfo RenderInfo; // offset 0x28, size 0x8
    int mArg; // offset 0x30, size 0x4
};

#endif
