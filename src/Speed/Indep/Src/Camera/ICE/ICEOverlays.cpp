#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Camera/ICE/ICEMover.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Generated/Events/ELoadingScreenOff.hpp"

// total size: 0x8
struct IceOverlay {
    unsigned char nOverlay; // offset 0x0, size 0x1
    const char *pName;      // offset 0x4, size 0x4
};

IceOverlay gIceOverlays[5] = {
    {0, "<NONE>"},
    {1, "FLASHERS.fng"},
    {2, "BLACK_FADE_OUT.fng"},
    {3, "BUSTED_OVERLAY.fng"},
    {4, "BLACK_BACKGROUND.fng"},
};

static unsigned char gOverlay = 0;

int GetOverlayIndex(unsigned char overlay) {
    int index = 0;
    for (int i = 0; i <= 4; i++) {

        if (overlay == gIceOverlays[i].nOverlay) {

            index = i;

            break;
        }
    }

    return index;
}

const char *ICE::GetOverlayName(unsigned char overlay) {

    int index = GetOverlayIndex(overlay);
    return gIceOverlays[index].pName;
}

void ICE::ShowOverlay(unsigned char overlay) {
    if (overlay) {

        if ((gOverlay & 0x7f) != 0 && (gOverlay & 0x80) == 0) {
            return;
        }

        new ELoadingScreenOff;

        gOverlay = overlay;
        cFEng::Get()->PushNoControlPackage(ICE::GetOverlayName(overlay), FE_PACKAGE_PRIORITY_SECOND_CLOSEST);
    }
}

void ICE::HideOverlay() {
    if ((gOverlay & 0x7f) != 0) {
        if (cFEng::Get()->IsPackagePushed(ICE::GetOverlayName(gOverlay & 0x7f))) {

            cFEng::Get()->PopNoControlPackage(ICE::GetOverlayName(gOverlay & 0x7f));
            gOverlay = 0;

        } else {

            gOverlay |= 0x80;
        }
    }
}
