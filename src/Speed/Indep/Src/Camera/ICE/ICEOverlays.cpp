// ICE Overlay system

#include <cstddef>

class Event {
  public:
    void *operator new(std::size_t size);
    void operator delete(void *ptr, std::size_t size);
    virtual ~Event() {}
    virtual const char *GetEventName();
    Event(std::size_t size) : fEventSize(size) {}
    std::size_t fEventSize;
};

struct cFEng {
    static cFEng *Get();
    static cFEng *mInstance;
    void PushNoControlPackage(const char *name, int priority);
    void PopNoControlPackage(const char *name);
    bool IsPackagePushed(const char *name);
};

struct ELoadingScreenOff : public Event {
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
    int index = 0;
    for (int i = 0; i < NUM_OVERLAYS; i++) {
        if (overlay == gIceOverlays[i].id) {
            index = i;
            break;
        }
    }
    return index;
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
            cFEng::mInstance->PushNoControlPackage(GetOverlayName(overlay), 0x67);
        }
    }
}

void HideOverlay() {
    if ((gOverlay & 0x7f) != 0) {
        if (cFEng::mInstance->IsPackagePushed(GetOverlayName(gOverlay & 0x7f))) {
            cFEng::mInstance->PopNoControlPackage(GetOverlayName(gOverlay & 0x7f));
            gOverlay = 0;
        } else {
            gOverlay = gOverlay | 0x80;
        }
    }
}

} // namespace ICE