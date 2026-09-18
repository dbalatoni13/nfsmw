#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/FEPkg_GarageMain.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"

const char *Showcase::FromPackage = "";
uint32 Showcase::FromArgs = 0;
uint32 Showcase::FromIndex = 0;
uint32 Showcase::BlackListNumber = 0;
int32 Showcase::FromFilter = (int)0xFFFFFFFF;
#ifndef EA_BUILD_A124
void *Showcase::FromColor[3];
#endif

Showcase::Showcase(ScreenConstructorData *sd) : MenuScreen(sd), RivalStreamer(sd->PackageFilename, false) {
    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("CURRENT_GEN_WIDESCREEN"), GetPackageName(), 0);
    }

    car = reinterpret_cast<FECarRecord *>(sd->Arg);

    if (car) {
        if (BlackListNumber != 0) {
            const char *titleStr = GetLocalizedString(0x3a64de21);
            char buf[32];
            FEngSNPrintf(buf, 0x20, titleStr, BlackListNumber);
            FEPrintf(GetPackageName(), 0x242657ce, "%s", buf);
            cFEng::Get()->QueuePackageMessage(0x89d0649c, GetPackageName(), 0);
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

    bool isDeviceWheel = false;
    FEImage *controlGroupImg = FEngFindImage(GetPackageName(), 0x66be0542);
    FEngSetButtonTexture(controlGroupImg, 0xfbb0b78e);
    FEImage *img1 = FEngFindImage(GetPackageName(), 0x5bc);
    FEngSetButtonTexture(img1, 0x5bc);
    FEImage *img2 = FEngFindImage(GetPackageName(), 0x682);
    FEngSetButtonTexture(img2, 0x682);

    for (int i = 0; i < 4; i++) {
        if (i < IOModule::GetIOModule().GetNumDevices()) {
            InputDevice *device = IOModule::GetIOModule().GetDevice(i);
            if (device) {
                UTL::COM::IUnknown *pUnk = device->GetSecondaryDevice();
                if (pUnk) {
                    SteeringWheelDevice *wheelDevice = nullptr;
                    if (pUnk->QueryInterface(&wheelDevice) && wheelDevice->IsConnected()) {
                        isDeviceWheel = true;
                    }
                }
            }
        }
    }

    if (isDeviceWheel) {
        FEObject *obj = FEngFindObject(GetPackageName(), 0xd86aacf8);
        FEngSetInvisible(obj);
    }
}

Showcase::~Showcase() {}

void Showcase::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    if (msg == 0x406415e3 || msg == 0x911ab364) {
        cFEng::Get()->QueuePackageSwitch(FromPackage, FromArgs, 0, false);
        if (BlackListNumber != 0) {
            car->Handle = INVALID_CAR_HANDLE;
            GarageMainScreen::GetInstance()->DisableCarRendering();
        }
        FromArgs = 0;
        BlackListNumber = 0;
    }
}
