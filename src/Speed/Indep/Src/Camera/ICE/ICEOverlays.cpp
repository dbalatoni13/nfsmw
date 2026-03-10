// ICE Overlay system

struct cFEng {
    static cFEng *Get();
    void PushNoControlPackage(const char *name, int priority);
    void PopNoControlPackage(const char *name);
    bool IsPackagePushed(const char *name);
};

struct ELoadingScreenOff {
    ELoadingScreenOff();
};

struct OverlayEntry {
    unsigned char id;
    char pad[3];
    const char *name;
};

static const int NUM_OVERLAYS = 5;
extern OverlayEntry gIceOverlays[NUM_OVERLAYS];
static unsigned char gOverlay;

static int GetOverlayIndex(unsigned char overlay) {
    for (int i = 0; i < NUM_OVERLAYS; i++) {
        if (gIceOverlays[i].id == overlay) {
            return i;
        }
    }
    return 0;
}

namespace ICE {

char *GetOverlayName(unsigned char overlay) {
    int index = GetOverlayIndex(overlay);
    return const_cast<char *>(gIceOverlays[index].name);
}

void ShowOverlay(unsigned char overlay) {
    if (overlay != 0) {
        if ((gOverlay & 0x7f) == 0 || (gOverlay & 0x80) != 0) {
            new ELoadingScreenOff();
            gOverlay = overlay;
            cFEng::Get()->PushNoControlPackage(GetOverlayName(overlay), 0x67);
        }
    }
}

void HideOverlay() {
    if ((gOverlay & 0x7f) != 0) {
        if (!cFEng::Get()->IsPackagePushed(GetOverlayName(gOverlay & 0x7f))) {
            gOverlay = gOverlay | 0x80;
        } else {
            cFEng::Get()->PopNoControlPackage(GetOverlayName(gOverlay & 0x7f));
            gOverlay = 0;
        }
    }
}

} // namespace ICE