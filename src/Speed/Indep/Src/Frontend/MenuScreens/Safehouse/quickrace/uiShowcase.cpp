#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiShowcase.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"

extern void FEngSetButtonTexture(FEImage *img, unsigned int hash);

const char *Showcase::FromPackage;
unsigned int Showcase::FromArgs;
unsigned int Showcase::FromIndex;
unsigned int Showcase::BlackListNumber;
int Showcase::FromFilter;
void *Showcase::FromColor[3];

Showcase::Showcase(ScreenConstructorData *sd)
    : MenuScreen(sd) //
      ,
      RivalStreamer(sd->PackageFilename, false) {
    if (eIsWidescreen()) {
        cFEng::Get()->QueuePackageMessage(bStringHash("WidescreenFix"), GetPackageName(), 0);
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
            FEngSetLanguageHash(GetPackageName(), 0x7ac3d0c9, FEngHashString("BL_RIVAL_%d", BlackListNumber));
            pTagImg = FEngFindImage(GetPackageName(), 0xf5a2a087);
            RivalStreamer.Init(BlackListNumber, nullptr, pTagImg, nullptr);
        } else {
            const char *pkg = GetPackageName();
            unsigned int manuLogoHash = car->GetManuLogoHash();
            FEImage *manuLogo = FEngFindImage(pkg, 0x3e01ad1d);
            FEngSetTextureHash(manuLogo, manuLogoHash);
            unsigned int logoHash = car->GetLogoHash();
            FEImage *carBadge = FEngFindImage(pkg, 0xb05dd708);
            FEngSetTextureHash(carBadge, logoHash);
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
            InputDevice *dev = IOModule::GetIOModule().GetDevice(i);
            if (dev) {
                UTL::COM::IUnknown *pUnk = dev->GetInterfaces();
                if (pUnk) {
                    ISteeringWheel *wheelDevice = nullptr;
                    if (pUnk->QueryInterface(&wheelDevice) && wheelDevice && wheelDevice->IsConnected()) {
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

void Showcase::NotificationMessage(unsigned long msg, FEObject *pObj, unsigned long param1, unsigned long param2) {
    if (msg == 0x406415e3 || msg == 0x911ab364) {
        cFEng::Get()->QueuePackageSwitch(FromPackage, FromArgs, 0, false);
        if (BlackListNumber != 0) {
            car->Handle = 0xFFFFFFFF;
            GarageMainScreen::GetInstance()->DisableCarRendering();
        }
        FromArgs = 0;
        BlackListNumber = 0;
    }
}
