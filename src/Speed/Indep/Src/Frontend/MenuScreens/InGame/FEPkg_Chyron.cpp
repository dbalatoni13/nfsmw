#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_Chyron.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"

extern ICEManager TheICEManager;
extern Timer RealTimer;
extern MenuScreen *FEngFindScreen(const char *name);

extern const char lbl_803E59BC[];

void DismissChyron();

void SummonChyron(char *title, char *artist, char *album) {
    if (!TheICEManager.IsEditorOn()) {
        if (title != nullptr && artist != nullptr && album != nullptr) {
            Chyron::mTitle = title;
            Chyron::mArtist = artist;
            Chyron::mAlbum = album;
        }
        DismissChyron();
        if (!cFEng::Get()->IsPackagePushed(lbl_803E59BC)) {
            cFEng::Get()->PushNoControlPackage(lbl_803E59BC, FE_PACKAGE_PRIORITY_CLOSEST);
        }
        Chyron *chyron = static_cast< Chyron * >(FEngFindScreen(lbl_803E59BC));
        if (chyron != nullptr) {
            chyron->mDelayTimer = RealTimer;
        }
    }
}