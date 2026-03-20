#include "Clans.hpp"

#include "Skids.hpp"
#include "Speed/Indep/bWare/Inc/bVector.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void bInitializeBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *point);

extern int WorldTime;

SlotPool *ClanSlotPool = 0;
bTList<Clan> ClanList;

inline void *Clan::operator new(size_t) {
    return bOMalloc(ClanSlotPool);
}

inline void Clan::operator delete(void *ptr) {
    bFree(ClanSlotPool, ptr);
}

void InitClans() {
    if (!ClanSlotPool) {
        ClanSlotPool = bNewSlotPool(0x48, 0x28, "ClanSlotPool", 0);
    }
}

void FlushClans() {
    while (!ClanList.IsEmpty()) {
        Clan *clan = ClanList.GetHead();
        ClanList.Remove(clan);
        delete clan;
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
    unsigned int hash = static_cast<unsigned int>(cx + cy);
    Clan *clan = ClanList.GetHead();

    if (clan != ClanList.EndOfList() && clan->GetHash() != hash) {
        do {
            clan = clan->GetNext();
        } while (clan != ClanList.EndOfList() && clan->GetHash() != hash);
    }

    if (clan != ClanList.EndOfList()) {
        clan->SetLastUpdateTime(WorldTime);
        ClanList.Remove(clan);
        ClanList.AddHead(clan);
        return clan;
    }

    if (bIsSlotPoolFull(ClanSlotPool)) {
        clan = ClanList.GetTail();
        ClanList.Remove(clan);
        delete clan;
    }

    clan = new Clan(position, hash);
    ClanList.AddHead(clan);
    return clan;
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

Clan::Clan(bVector3 *position, unsigned int hash)
    : Hash(hash)
{
    Position = *position;
    bInitializeBoundingBox(&BBoxMin, &BBoxMax, position);
}

Clan::~Clan() {
    while (SkidSetList.GetHead() != SkidSetList.EndOfList()) {
        bPNode *node = SkidSetList.GetHead();
        DeleteThisSkid(reinterpret_cast<SkidSet *>(node->GetpObject()));
    }

    while (SkidSetList.GetHead() != SkidSetList.EndOfList()) {
        SkidSetList.RemoveHead();
    }
}
