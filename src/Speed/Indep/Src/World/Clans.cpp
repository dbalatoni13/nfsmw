#include "Clans.hpp"

#include "Skids.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

SlotPool *ClanSlotPool = nullptr;
bTList<Clan> ClanList;

void InitClans() {
    if (!ClanSlotPool) {
        ClanSlotPool = bNewSlotPool(0x48, 40, "ClanSlotPool", 0);
    }
}

void FlushClans() {
    while (!ClanList.IsEmpty()) {
        delete ClanList.RemoveHead();
    }
}

void CloseClans() {
    if (ClanSlotPool) {
        FlushClans();
        bDeleteSlotPool(ClanSlotPool);
        ClanSlotPool = 0;
    }
}

Clan *GetClan(bVector3 *position) {
    int cx = (static_cast<int>(position->x * 65536.0f) >> 22) & 0xffff;
    int cy = (static_cast<int>(position->y * 65536.0f) >> 22) * 0x10000;
    uint32 hash = static_cast<unsigned int>(cy + cx);
    Clan *clan = ClanList.GetHead();

    for (; clan != ClanList.EndOfList(); clan = clan->GetNext()) {
        if (clan->GetHash() == hash) {
            break;
        }
    }

    if (clan != ClanList.EndOfList()) {
        clan->SetLastUpdateTime(WorldTime);
        ClanList.Remove(clan);
        ClanList.AddHead(clan);
        return clan;
    }

    if (bIsSlotPoolFull(ClanSlotPool)) {
        Clan *clan = ClanList.RemoveTail();
        delete clan;
    }

    return ClanList.AddHead(new Clan(position, hash));
}

void RenderClans(eView *view) {
    ProfileNode profile_node("TODO", 0);
    Clan *clan = ClanList.GetHead();
    while (clan != ClanList.EndOfList()) {
        Clan *next_clan = clan->GetNext();
        int pixel_size = view->GetPixelSize(clan->GetBBoxMin(), clan->GetBBoxMax());

        if (pixel_size > 10) {
            eVisibleState visibility = view->GetVisibleState(clan->GetBBoxMin(), clan->GetBBoxMax(), 0);
            if (visibility != 0) {
                if (WorldTime - clan->GetLastUpdateTime() > 300) {
                    clan->SetLastUpdateTime(WorldTime);
                    ClanList.Remove(clan);
                    ClanList.AddHead(clan);
                }
                RenderSkids(view, clan);
            }
        }
        clan = next_clan;
    }
}

Clan::Clan(bVector3 *position, uint32 hash) {
    Position = *position;
    Hash = hash;
    bInitializeBoundingBox(&BBoxMin, &BBoxMax, position);
}

Clan::~Clan() {
    while (!SkidSetList.IsEmpty()) {
        bPNode *p = SkidSetList.GetHead();
        SkidSet *skid_set = static_cast<SkidSet *>(p->GetObject());
        DeleteThisSkid(skid_set);
    }
}
