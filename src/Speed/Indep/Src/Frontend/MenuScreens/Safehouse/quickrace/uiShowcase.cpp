#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"

const char *Showcase::FromPackage;
uint32 Showcase::FromArgs;
uint32 Showcase::FromIndex;
uint32 Showcase::BlackListNumber;
int32 Showcase::FromFilter;
#ifndef EA_BUILD_A124
void *Showcase::FromColor[3];
#endif

// UNSOLVED
Showcase::Showcase(ScreenConstructorData *sd) : MenuScreen(sd), RivalStreamer(sd->PackageFilename, false) {
    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), nullptr);
    }

    car = reinterpret_cast<FECarRecord *>(sd->Arg);

    if (car != nullptr) {
        if (BlackListNumber != 0) {
            char buf[32];
            const u32 FEObj_TITLE_GROUP = 0x242657ce;
            const u32 FEObj_SHOWCASE = 0;
            FEngSNPrintf(buf, sizeof(buf), GetLocalizedString(0x3a64de21), BlackListNumber);
            FEPrintf(GetPackageName(), FEObj_TITLE_GROUP, "%s", buf);
            cFEng::Get()->QueuePackageMessage(0x89d0649c, GetPackageName(), nullptr);
            FEPrintf(GetPackageName(), 0xb695a891, "%d", BlackListNumber);
            FEngSetLanguageHash(GetPackageName(), 0x7ac3d0c9, FEngHashString("BLACKLIST_RIVAL_%02d_AKA", BlackListNumber));
            pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
            RivalStreamer.Init(BlackListNumber, nullptr, pTagImg, nullptr);
        } else {
            FEngSetTextureHash(GetPackageName(), 0x3e01ad1d, car->GetManuLogoHash());
            FEngSetTextureHash(GetPackageName(), 0xb05dd708, car->GetLogoHash());
            RivalStreamer.Init(1, nullptr, nullptr, nullptr);
        }
    }

    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x66be0542), 0xfbb0b78e);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x5bc), 0x5bc);
    FEngSetButtonTexture(FEngFindImage(GetPackageName(), 0x682), 0x682);

    bool isDeviceWheel = false;
    for (int i = 0; i < 4; i++) {
        InputDevice *device;
        if (i < IOModule::GetIOModule().GetNumDevices()) {
            device = IOModule::GetIOModule().GetDevice(i);
            if (device != nullptr) {
                UTL::COM::IUnknown *pUnk = device->GetSecondaryDevice();
                SteeringWheelDevice *wheelDevice = nullptr;
                if (pUnk != nullptr && pUnk->QueryInterface(&wheelDevice)) {
                    if ((wheelDevice != nullptr) && wheelDevice->IsConnected()) {
                        isDeviceWheel = true;
                    }
                }
            }
        }
    }

    if (isDeviceWheel) {
        const u32 FEObj_CONROLGROUP = 0xd86aacf8;
        FEngSetInvisible(GetPackageName(), FEObj_CONROLGROUP);
    }
}

Showcase::~Showcase() {}

void Showcase::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == __PAD_ACCEPT__ || msg == __PAD_BACK__) {
        cFEng::Get()->QueuePackageSwitch(FromPackage, FromArgs, 0, false);
        if (BlackListNumber != 0) {
            car->Handle = INVALID_CAR_HANDLE;
            GarageMainScreen::GetInstance()->DisableCarRendering();
        }
        FromArgs = 0;
        BlackListNumber = 0;
    }
}
