#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "types.h"

FEPackageManager *FEPackageManager::mInstance;
int FEPackageData::mInScreenConstructor;

void FEPackageManager::Init() {
    if (!mInstance) {
        mInstance = new ("", 0) FEPackageManager();
    }
}

FEPackageManager *FEPackageManager::Get() {
    return mInstance;
}

void FEPackageManager::BroadcastMessage(u32 msg) {
    FEPackageData *active[32];
    int count = 0;

    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetPackage() && count < 32) {
            if (msg != 0x9803F6E2 || f->GetPackage()->IsInputEnabled()) {
                active[count] = f;
                count++;
            }
        }
    }

    for (int i = 0; i < count; i++) {
        if (active[i]->GetPackage()) {
            active[i]->NotificationMessage(msg, nullptr, 0, 0);
        }
    }
}

u32 FEPackageManager::GetActiveScreensChecksum() {
    u32 checksum = 0;
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetPackage()) {
            if (bStrICmp(f->GetPackage()->GetName(), "EA_TRAX.fng") != 0) {
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
        if (f->GetPackage() && pkg_ptr == reinterpret_cast<u32>(f->GetPackage())) {
            f->NotifySoundMessage(Message, obj, controller_mask, pkg_ptr);
        }
    }
}

void FEPackageManager::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetPackage() && Param2 == reinterpret_cast<u32>(f->GetPackage())) {
            f->NotificationMessage(Message, pObject, Param1, Param2);
        }
    }
}

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
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetPackage();
}

void *FEPackageManager::GetPackageData(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetDataChunk();
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
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return false;
    }
    return data->GetVisibility();
}

MenuScreen *FEPackageManager::FindScreen(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetScreen();
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
    const char *baseName = GetBasePkgName(pkg_name);
    unsigned int hash = FEHashUpper(baseName);
    FEPackageData *found = nullptr;

    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        found = f;
        if (f->GetNameHash() == hash) {
            break;
        }
    }

    if (!found) {
        return nullptr;
    }

    found->Remove();
    ScreenList.AddHead(found);
    return found;
}

bool FEPackageManager::SetPackageDataArg(const char *pPackageName, const int pArg) {
    FEPackageData *data = mInstance->FindFEPackageData(pPackageName);
    if (data) {
        data->SetArgument(pArg);
    }
    return data != nullptr;
}

void FEPackageManager::PackageWasLoaded(FEPackage *pkg) {
    FEPackageData *data = FindFEPackageData(pkg->GetName());
    if (data) {
        data->Activate(pkg, data->GetArgument());
    }
}

void FEPackageManager::PackageWillBeUnloaded(FEPackage *pkg) {
    FEPackageData *data = FindFEPackageData(pkg->GetName());
    if (data) {
        data->UnActivate();
    }
}

void FEPackageManager::Loader(bChunk *chunk, bool hotchunk_flag) {
    FEPackageData *data = new ("", 0) FEPackageData(chunk);
    if (chunk->GetID() == 0x30210) {
        bEndianSwap32(reinterpret_cast<char *>(chunk) + 8);
        bEndianSwap32(reinterpret_cast<char *>(chunk) + 12);
        bEndianSwap16(reinterpret_cast<char *>(chunk) + 18);
        bEndianSwap32(reinterpret_cast<char *>(chunk) + 20);
        bEndianSwap32(reinterpret_cast<char *>(chunk) + 24);
    }
    FEPackageManager::Get()->ScreenList.AddTail(data);
}

void FEPackageManager::UnLoader(bChunk *chunk, bool hotchunk_flag) {
    cFEng::Get()->ServiceFengOnly();
    FEPackageData *data = FindFEPackageData(chunk);
    if (data) {
        data->ClearHotchunk();
        data->Close();
        FEPackageManager::Get();
        data->Remove();
        delete data;
    }
}

void FEPackageManager::ErrorTick() {
    BroadcastMessage(0xD0678849);
}

void FEPackageManager::Tick() {
    BroadcastMessage(0xC98356BA);
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
    if (chunk->GetID() == 0x30203 || chunk->GetID() == 0x30210) {
        FEPackageManager::Get()->Loader(chunk, IsCurrentlyHotChunking());
        return 1;
    }
    return 0;
}

int UnloaderFEngPackage(bChunk *chunk) {
    if (chunk->GetID() == 0x30203 || chunk->GetID() == 0x30210) {
        FEPackageManager::Get()->UnLoader(chunk, IsCurrentlyHotChunking());
        return 1;
    }
    return 0;
}
