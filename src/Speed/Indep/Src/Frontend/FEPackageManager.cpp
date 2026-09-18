#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"

bool IsCurrentlyHotChunking();

FEPackageManager *FEPackageManager::mInstance = 0;

void FEPackageManager::Init() {
    if (mInstance == 0) {
        mInstance = new ("FEPackageManager", 0) FEPackageManager;
    }
}

FEPackageManager *FEPackageManager::Get() {
    return mInstance;
}

inline FEPackageManager::~FEPackageManager() {
}

void FEPackageManager::BroadcastMessage(u32 msg) {
    FEPackageData *screens[32];
    int count = 0;
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        bool is_loaded = data->GetPackage() != 0;
        if (is_loaded && count <= 0x1F && (msg != FEMSG_MOUSE_CHANGED || data->GetPackage()->IsInputEnabled())) {
            screens[count] = data;
            count = count + 1;
        }
    }
    for (int i = 0; i < count; i++) {
        bool is_loaded = screens[i]->GetPackage() != 0;
        if (is_loaded) {
            screens[i]->NotificationMessage(msg, 0, 0, (u32)screens[i]->GetPackage());
        }
    }
}

u32 FEPackageManager::GetActiveScreensChecksum() {
    u32 checksum = 0;
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        bool is_loaded = data->GetPackage() != 0;
        if (is_loaded && bStrICmp(data->GetPackage()->GetName(), "EA_TRAX.fng") != 0) {
            checksum += bStringHash(data->GetPackage()->GetName());
        }
    }
    return checksum;
}

uint32 FEngGetActiveScreensChecksum() {
    return FEPackageManager::Get()->GetActiveScreensChecksum();
}

void FEPackageManager::NotifySoundMessage(u32 Message, FEObject *obj, u32 controller_mask, u32 pkg_ptr) {
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        bool is_loaded = data->GetPackage() != 0;
        if (is_loaded && pkg_ptr == (u32)data->GetPackage()) {
            data->NotifySoundMessage(Message, obj, controller_mask, pkg_ptr);
        }
    }
}

void FEPackageManager::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        bool is_loaded = data->GetPackage() != 0;
        if (is_loaded && Param2 == (u32)data->GetPackage()) {
            data->NotificationMessage(Message, pObject, Param1, Param2);
        }
    }
}

const char *FEPackageManager::GetBasePkgName(const char *pkg_name) {
    const char *base_pkg_name = pkg_name + bStrLen(pkg_name);

    while (base_pkg_name != pkg_name) {
        if (*base_pkg_name == '\\') {
            base_pkg_name++;
            break;
        }

        base_pkg_name--;
    }

    return base_pkg_name;
}

FEPackage *FEPackageManager::FindPackage(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (data != 0) {
        return data->GetPackage();
    }
    return 0;
}

void *FEPackageManager::GetPackageData(const char *pkg_name) {
    FEPackageData *screen = FindFEPackageData(pkg_name);
    if (screen != nullptr) {
        return screen->GetDataChunk();
    }
    return nullptr;
}

void FEPackageManager::CloseAllPackages(int close_permanent) {
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        if (data->GetPermanent() == 0 || close_permanent != 0) {
            data->Close();
        } else {
            if (data->GetPackage() != 0) {
                HackClearCache(data->GetPackage());
            }
        }
    }
}

bool FEPackageManager::GetVisibility(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (data != 0) {
        return data->GetVisibility();
    }
    return 0;
}

MenuScreen *FEPackageManager::FindScreen(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (data != 0) {
        return data->GetScreen();
    }
    return 0;
}

FEPackageData *FEPackageManager::FindFEPackageData(bChunk *chunk) {
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        if (data->GetChunk() == chunk) {
            return data;
        }
    }
    return 0;
}

FEPackageData *FEPackageManager::FindFEPackageData(const char *pkg_name) {
    uint32 hash = FEHashUpper(GetBasePkgName(pkg_name));
    FEPackageData *found = 0;
    for (FEPackageData *data = ScreenList.GetHead(); data != ScreenList.EndOfList(); data = data->GetNext()) {
        if (data->GetNameHash() == hash) {
            found = data;
            break;
        }
    }
    if (found != 0) {
        found->Remove();
        ScreenList.AddHead(found);
        return found;
    }
    return 0;
}

bool FEPackageManager::SetPackageDataArg(const char *pPackageName, const int pArg) {
    FEPackageData *data = mInstance->FindFEPackageData(pPackageName);
    if (data != 0) {
        data->SetArgument(pArg);
        return 1;
    }
    return 0;
}

void FEPackageManager::PackageWasLoaded(FEPackage *pkg) {
    FEPackageData *data = FindFEPackageData(pkg->GetName());
    if (data != 0) {
        data->Activate(pkg, data->GetArgument());
    }
}

void FEPackageManager::PackageWillBeUnloaded(FEPackage *pkg) {
    FEPackageData *data = FindFEPackageData(pkg->GetName());
    if (data != 0) {
        data->UnActivate();
    }
}

void FEPackageManager::Loader(bChunk *chunk, bool hotchunk_flag) {
    FEPackageData *data = new ("FEPackageData", 0) FEPackageData(chunk);
    if (chunk->GetID() == BCHUNK_FENG_COMPRESSED_PACKAGE) {
        bEndianSwap32((char *)chunk + 8);
        bEndianSwap32((char *)chunk + 0xC);
        bEndianSwap16((char *)chunk + 0x12);
        bEndianSwap32((char *)chunk + 0x14);
        bEndianSwap32((char *)chunk + 0x18);
    }
    FEPackageManager::Get()->ScreenList.AddTail(data);
}

void FEPackageManager::UnLoader(bChunk *chunk, bool hotchunk_flag) {
    cFEng::Get()->ServiceFengOnly();
    FEPackageData *data = FindFEPackageData(chunk);
    if (data != 0) {
        data->bWasSetupForHotchunk = 0;
        data->Close();
        FEPackageManager::Get()->ScreenList.Remove(data);
        delete data;
    }
}

void FEPackageManager::ErrorTick() {
    BroadcastMessage(FEMSG_ERROR_STATE);
}

void FEPackageManager::Tick() {
    BroadcastMessage(FEMSG_SCREEN_TICK);
}

FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg) {
    FEPackageData *data = (FEPackageData *)pkg->GetUserParam();
    if (data != 0) {
        return data->GetRenderInfo();
    }
    return 0;
}

MenuScreen *FEngFindScreen(const char *package_name) {
    return FEPackageManager::Get()->FindScreen(package_name);
}

int LoaderFEngPackage(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_FENG_PACKAGE || chunk->GetID() == BCHUNK_FENG_COMPRESSED_PACKAGE) {
        FEPackageManager::Get()->Loader(chunk, IsCurrentlyHotChunking());
        return 1;
    }
    return 0;
}

int UnloaderFEngPackage(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_FENG_PACKAGE || chunk->GetID() == BCHUNK_FENG_COMPRESSED_PACKAGE) {
        FEPackageManager::Get()->UnLoader(chunk, IsCurrentlyHotChunking());
        return 1;
    }
    return 0;
}
