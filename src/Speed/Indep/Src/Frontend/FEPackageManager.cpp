#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Misc/LZCompress.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "types.h"

FEPackageManager *FEPackageManager::mInstance;
int FEPackageData::mInScreenConstructor;

void FEPackageManager::Init() {
    if (mInstance == nullptr) {
        mInstance = new ("FEPackageManager", 0) FEPackageManager();
    }
}

FEPackageManager *FEPackageManager::Get() {
    return mInstance;
}

void FEPackageManager::BroadcastMessage(u32 msg) {
    FEPackageData *pkgs[32];
    int npkgs = 0;

    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if ((f->IsActive()) && npkgs < 32) {
            if (msg != FEMSG_MOUSE_CHANGED || f->GetPackage()->IsInputEnabled()) {
                pkgs[npkgs] = f;
                npkgs++;
            }
        }
    }

    for (int i = 0; i < npkgs; i++) {
        if (pkgs[i]->IsActive()) {
            pkgs[i]->NotificationMessage(msg, nullptr, 0, reinterpret_cast<u32>(pkgs[i]->GetPackage()));
        }
    }
}

u32 FEPackageManager::GetActiveScreensChecksum() {
    u32 checksum = 0;
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->IsActive()) {
            if (!bStrEqual(f->GetPackage()->GetName(), "EA_TRAX.fng")) {
                checksum += bStringHash(f->GetPackage()->GetName());
            }
        }
    }
    return checksum;
}

uint32 FEngGetActiveScreensChecksum() {
    return FEPackageManager::Get()->GetActiveScreensChecksum();
}

void FEPackageManager::NotifySoundMessage(u32 Message, FEObject *obj, u32 controller_mask, u32 pkg_ptr) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->IsActive() && pkg_ptr == reinterpret_cast<u32>(f->GetPackage())) {
            f->NotifySoundMessage(Message, obj, controller_mask, pkg_ptr);
        }
    }
}

void FEPackageManager::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->IsActive() && Param2 == reinterpret_cast<u32>(f->GetPackage())) {
            f->NotificationMessage(Message, pObject, Param1, Param2);
        }
    }
}

// UNSOLVED
const char *FEPackageManager::GetBasePkgName(const char *pkg_name) {
    int len = bStrLen(pkg_name);
    const char *ptr = pkg_name + len;
    if (ptr != pkg_name) {
        char c = pkg_name[len];
        while (c != '\\') {
            ptr--;
            if (ptr == pkg_name) {
                return ptr;
            }
            c = *ptr;
        }
        ptr++;
    }
    return ptr;
}

FEPackage *FEPackageManager::FindPackage(const char *pkg_name) {
    FEPackageData *d = FindFEPackageData(pkg_name);
    if (d != nullptr) {
        return d->GetPackage();
    }
    return nullptr;
}

void *FEPackageManager::GetPackageData(const char *pkg_name) {
    FEPackageData *screen = FindFEPackageData(pkg_name);
    if (screen != nullptr) {
        return screen->GetDataChunk();
    }
    return nullptr;
}

void FEPackageManager::CloseAllPackages(int close_permanent) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (!f->GetPermanent() || close_permanent) {
            f->Close();
        } else if (f->GetPackage()) {
            HackClearCache(f->GetPackage());
        }
    }
}

bool FEPackageManager::GetVisibility(const char *pkg_name) {
    FEPackageData *pkg_data = FindFEPackageData(pkg_name);
    if (pkg_data != nullptr) {
        return pkg_data->GetVisibility();
    }
    return false;
}

MenuScreen *FEPackageManager::FindScreen(const char *pkg_name) {
    FEPackageData *pkg_data = FindFEPackageData(pkg_name);
    if (pkg_data != nullptr) {
        return pkg_data->GetScreen();
    }
    return nullptr;
}

FEPackageData *FEPackageManager::FindFEPackageData(bChunk *chunk) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetChunk() == chunk) {
            return f;
        }
    }
    return nullptr;
}

FEPackageData *FEPackageManager::FindFEPackageData(const char *pkg_name) {
    const char *basename = GetBasePkgName(pkg_name);
    FEPackageData *found = nullptr;
    uint32 test_hash = FEHashUpper(basename);

    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetNameHash() == test_hash) {
            found = f;
            break;
        }
    }

    if (found != nullptr) {
        ScreenList.Remove(found);
        ScreenList.AddHead(found);
        return found;
    }

    return nullptr;
}

bool FEPackageManager::SetPackageDataArg(const char *pPackageName, const int pArg) {
    FEPackageData *packageData = mInstance->FindFEPackageData(pPackageName);
    if (packageData != nullptr) {
        packageData->SetArgument(pArg);
        return true;
    }
    return false;
}

void FEPackageManager::PackageWasLoaded(FEPackage *pkg) {
    FEPackageData *screen = FindFEPackageData(pkg->GetName());
    if (screen != nullptr) {
        screen->Activate(pkg, screen->GetArgument());
    }
}

void FEPackageManager::PackageWillBeUnloaded(FEPackage *pkg) {
    pkg->GetName();
    FEPackageData *screen = FindFEPackageData(pkg->GetName());
    if (screen != nullptr) {
        screen->UnActivate();
    }
}

void FEPackageManager::Loader(bChunk *chunk, bool hotchunk_flag) {
    {
        FEPackageData *pkg = new ("FEPackageData", 0) FEPackageData(chunk);
        if (chunk->GetID() == BCHUNK_FENG_COMPRESSED_PACKAGE) {
            uint32 *data = reinterpret_cast<uint32 *>(chunk->GetData());

            bPlatEndianSwap(&data[0]);
            LZHeader *header = reinterpret_cast<LZHeader *>(&data[1]);
            bPlatEndianSwap(&header->ID);
            bPlatEndianSwap(&header->Version);
            bPlatEndianSwap(&header->HeaderSize);
            bPlatEndianSwap(&header->Flags);
            bPlatEndianSwap(&header->UncompressedSize);
            bPlatEndianSwap(&header->CompressedSize);
        }

        FEPackageManager::Get()->Add(pkg);
    }
}

void FEPackageManager::UnLoader(bChunk *chunk, bool hotchunk_flag) {
    cFEng::Get()->ServiceFengOnly();
    FEPackageData *pkg = FindFEPackageData(chunk);
    if (pkg != nullptr) {
        pkg->ClearHotchunk();
        pkg->Close();
        FEPackageManager::Get();
        Remove(pkg);
        delete pkg;
    }
}

void FEPackageManager::ErrorTick() {
    BroadcastMessage(FEMSG_ERROR_STATE);
}

void FEPackageManager::Tick() {
    BroadcastMessage(FEMSG_SCREEN_TICK);
}

FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg) {
    FEPackageData *pkg_data = reinterpret_cast<FEPackageData *>(pkg->GetUserParam());
    if (pkg_data != nullptr) {
        return pkg_data->GetRenderInfo();
    }
    return nullptr;
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
