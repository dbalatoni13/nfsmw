#ifndef FRONTEND_MEMORYCARD_MEMORYCARD_H
#define FRONTEND_MEMORYCARD_MEMORYCARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace RealmcIface {
struct GCIconDataInfo;
struct GCBannerDataInfo;
}

class MemoryCard {
  public:
    enum SaveType {
        ST_PROFILE = 0,
        ST_THUMBNAIL = 1,
        ST_IMAGE = 2,
        ST_MAX = 3,
    };

    void Tick(int);

    static MemoryCard *GetInstance() {
        return s_pThis;
    }

    RealmcIface::GCIconDataInfo *GetSaveIcon() {
        return *reinterpret_cast<RealmcIface::GCIconDataInfo **>(reinterpret_cast<char *>(this) + 0x10);
    }

    RealmcIface::GCBannerDataInfo *GetSaveBanner() {
        return *reinterpret_cast<RealmcIface::GCBannerDataInfo **>(reinterpret_cast<char *>(this) + 0x14);
    }

    static MemoryCard *s_pThis;
    static int IsCardBusy();
};

void InitMemoryCard();

#endif
