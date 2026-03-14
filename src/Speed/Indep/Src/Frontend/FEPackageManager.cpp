#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern unsigned long FEHashUpper(const char *str);
extern unsigned int bStringHash(const char *str);
extern cFEng *cFEng_mInstance;
extern void HackClearCache(FEPackage *pkg);
extern bool IsCurrentlyHotChunking();

FEPackageManager *FEPackageManager::mInstance;
int FEPackageData::mInScreenConstructor;

FEPackageManager *FEPackageManager::Get() {
    return mInstance;
}

void FEPackageManager::ErrorTick() {
    BroadcastMessage(0xD0678849);
}

void FEPackageManager::Tick() {
    BroadcastMessage(0xC98356BA);
}

unsigned int FEngGetActiveScreensChecksum() {
    return FEPackageManager::Get()->GetActiveScreensChecksum();
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

MenuScreen *FEPackageManager::FindScreen(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return nullptr;
    }
    return data->GetScreen();
}

MenuScreen *FEngFindScreen(const char *package_name) {
    return FEPackageManager::Get()->FindScreen(package_name);
}

FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg) {
    unsigned long userParam = pkg->GetUserParam();
    if (userParam == 0) {
        return nullptr;
    }
    return reinterpret_cast<FEPackageRenderInfo *>(userParam + 0x28);
}

FEPackageData *FEPackageManager::FindFEPackageData(bChunk *chunk) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetChunk() == chunk) {
            return f;
        }
    }
    return nullptr;
}

void FEPackageManager::PackageWillBeUnloaded(FEPackage *pkg) {
    FEPackageData *data = FindFEPackageData(pkg->GetName());
    if (data) {
        data->UnActivate();
    }
}

void FEPackageManager::Init() {
    if (!mInstance) {
        mInstance = new ("", 0) FEPackageManager();
    }
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

unsigned long FEPackageManager::GetActiveScreensChecksum() {
    unsigned long checksum = 0;
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (f->GetPackage()) {
            if (bStrICmp(f->GetPackage()->GetName(), "EA_TRAX.fng") != 0) {
                checksum += bStringHash(f->GetPackage()->GetName());
            }
        }
    }
    return checksum;
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

void FEPackageManager::CloseAllPackages(int close_permanent) {
    for (FEPackageData *f = ScreenList.GetHead(); f != ScreenList.EndOfList(); f = f->GetNext()) {
        if (!f->GetPermanent() || close_permanent) {
            f->Close();
        } else if (f->GetPackage()) {
            HackClearCache(f->GetPackage());
        }
    }
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
    cFEng_mInstance->ServiceFengOnly();
    FEPackageData *data = FindFEPackageData(chunk);
    if (data) {
        data->ClearHotchunk();
        data->Close();
        FEPackageManager::Get();
        data->Remove();
        delete data;
    }
}

FEPackageManager::~FEPackageManager() {
}

bool FEPackageManager::GetVisibility(const char *pkg_name) {
    FEPackageData *data = FindFEPackageData(pkg_name);
    if (!data) {
        return false;
    }
    return data->GetVisibility();
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