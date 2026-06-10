#ifndef FEPACKAGEDATA_H
#define FEPACKAGEDATA_H

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"

struct ScreenFactoryDatum;

// File: speed/indep/src/frontend/FEPackageData.hpp
// total size: 0x38
// Decl: speed/indep/src/frontend/FEPackageData.hpp:12
class FEPackageData : public bTNode<FEPackageData> {
  public:
    FEPackageData(bChunk *chunk);
    virtual ~FEPackageData();

    uint32 GetNameHash();

    void *GetDataChunk();

    bChunk *GetChunk() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:20
        return MyChunk;
    }
    FEPackage *GetPackage() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:21
        return pPackage;
    }
    bool IsCompressedChunk() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:22
        return DataChunk != nullptr;
    }

    void Activate(FEPackage *pkg, int arg);

    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2);

    void NotifySoundMessage(u32 msg, FEObject *obj, u32 control_mask, u32 pkg_ptr);

    void UnActivate();

    bool IsActive() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:28
        return pScreen != nullptr;
    }

    void Close();

    void SetPermanent(int flag) { // Decl: speed/indep/src/frontend/FEPackageData.hpp:31
        IsPermanent = flag;
    }

    int GetPermanent() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:32
        return IsPermanent;
    }

    void SetArgument(int pArg) { // Decl: speed/indep/src/frontend/FEPackageData.hpp:34
        mArg = pArg;
    }
    int GetArgument() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:35
        return mArg;
    }

    bool GetVisibility() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:37
        return IsVisible;
    }
    void SetVisibility(bool visible) { // Decl: speed/indep/src/frontend/FEPackageData.hpp:38
        IsVisible = visible;
    }

    int GetLastKnownControlMask() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:43
        return LastKnownControlMask;
    }
    int LastKnownControlMask;    // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:44
    int bWasSetupForHotchunk;    // offset 0xC, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:45
    bool WasSetupForHotchunk() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:46
        return bWasSetupForHotchunk;
    }
    void SetupForHotchunk() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:47
        bWasSetupForHotchunk = true;
    }
    void ClearHotchunk() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:48
        bWasSetupForHotchunk = false;
    }

    MenuScreen *GetScreen() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:50
        return pScreen;
    }

    FEPackageRenderInfo *GetRenderInfo() {
        return &RenderInfo;
    }

    static int IsInScreenConstructor() { // Decl: speed/indep/src/frontend/FEPackageData.hpp:52
        return mInScreenConstructor > 0;
    }

  private:
    void *DataChunk;                // offset 0x10, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:55
    bChunk *MyChunk;                // offset 0x14, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:56
    MenuScreen *pScreen;            // offset 0x18, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:57
    FEPackage *pPackage;            // offset 0x1C, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:58
    int16 IsPermanent;              // offset 0x20, size 0x2, Decl: speed/indep/src/frontend/FEPackageData.hpp:59
    int16 IsVisible;                // offset 0x22, size 0x2, Decl: speed/indep/src/frontend/FEPackageData.hpp:60
    ScreenFactoryDatum *CreateData; // offset 0x24, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:61
    FEPackageRenderInfo RenderInfo; // offset 0x28, size 0x8
    int mArg;                       // offset 0x30, size 0x4, Decl: speed/indep/src/frontend/FEPackageData.hpp:62

    static int mInScreenConstructor; // size: 0x4, address: 0x8041CB60, Decl: speed/indep/src/frontend/FEPackageData.cpp:730
};

#define gMainMenuName "FeMainMenu.fng" // :70

uint8 FEngGetLastButton(const char *pkg_name);

void SetLoadingScreenPackageName(const char *name);

const char *GetLoadingScreenPackageName();

void FEngSetLastButton(const char *pkg_name, uint8 button_hash);
void FEngSetCreateCallback(const char *abstract_pkg_name, MenuScreenCreateFunction function);

#endif
