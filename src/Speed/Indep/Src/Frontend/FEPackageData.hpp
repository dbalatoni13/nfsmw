#ifndef FRONTEND_FEPACKAGEDATA_H
#define FRONTEND_FEPACKAGEDATA_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"

struct ScreenFactoryDatum {
    char *FEngPackageFilename;
    MenuScreenCreateFunction ConstructorFunction;
};

struct ScreenButtonDatum {
    uint32 ScreenHash;
    uint8 LastButton;
    uint32 GameMode;
};

// total size: 0x38
class FEPackageData : public bTNode<FEPackageData> {
  public:
    FEPackageData(bChunk *chunk);
    virtual ~FEPackageData();
    uint32 GetNameHash();
    void *GetDataChunk();
    bChunk *GetChunk() {
        return MyChunk;
    }
    FEPackage *GetPackage() {
        return pPackage;
    }
    bool IsCompressedChunk() {
        return DataChunk != nullptr;
    }
    void Activate(FEPackage *pkg, int arg);
    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2);
    void NotifySoundMessage(u32 msg, FEObject *obj, u32 control_mask, u32 pkg_ptr);
    void UnActivate();
    bool IsActive() {
        return pScreen != nullptr;
    }

    void Close();
    void SetPermanent(int flag) {
        IsPermanent = flag;
    }
    int GetPermanent() {
        return IsPermanent;
    }
    void SetArgument(int pArg) {
        mArg = pArg;
    }
    int GetArgument() {
        return mArg;
    }
    bool GetVisibility() {
        return IsVisible;
    }
    void SetVisibility(bool visible) {
        IsVisible = visible;
    }
    int GetLastKnownControlMask() {
        return LastKnownControlMask;
    }

    bool WasSetupForHotchunk() {
        return bWasSetupForHotchunk;
    }
    void SetupForHotchunk() {
        bWasSetupForHotchunk = true;
    }
    void ClearHotchunk() {
        bWasSetupForHotchunk = false;
    }
    MenuScreen *GetScreen() {
        return pScreen;
    }
    FEPackageRenderInfo *GetRenderInfo() {
        return &RenderInfo;
    }
    static int IsInScreenConstructor() {
        return mInScreenConstructor > 0;
    }

    int LastKnownControlMask; // offset 0x8, size 0x4
    int bWasSetupForHotchunk; // offset 0xC, size 0x4
  private:
    void *DataChunk;                 // offset 0x10, size 0x4
    bChunk *MyChunk;                 // offset 0x14, size 0x4
    MenuScreen *pScreen;             // offset 0x18, size 0x4
    FEPackage *pPackage;             // offset 0x1C, size 0x4
    int16 IsPermanent;               // offset 0x20, size 0x2
    int16 IsVisible;                 // offset 0x22, size 0x2
    ScreenFactoryDatum *CreateData;  // offset 0x24, size 0x4
    FEPackageRenderInfo RenderInfo;  // offset 0x28, size 0x8
    int mArg;                        // offset 0x30, size 0x4
    static int mInScreenConstructor; // size: 0x4, address: 0x8041CB60
};

uint8 FEngGetLastButton(const char *pkg_name);

void SetLoadingScreenPackageName(const char *name);

const char *GetLoadingScreenPackageName();

void FEngSetLastButton(const char *pkg_name, uint8 button_hash);
void FEngSetCreateCallback(const char *abstract_pkg_name, MenuScreenCreateFunction function);

#endif
