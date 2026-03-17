#include "Clans.hpp"

#include "Skids.hpp"
#include "Speed/Indep/bWare/Inc/bVector.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void bInitializeBoundingBox(bVector3 *bbox_min, bVector3 *bbox_max, const bVector3 *point);

extern int WorldTime;

SlotPool *ClanSlotPool = 0;
bTList<Clan> ClanList;

void *Clan::operator new(size_t) {
    return bOMalloc(ClanSlotPool);
}

void Clan::operator delete(void *ptr) {
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
    if (!ClanSlotPool) {
        InitClans();
    }

    int hash = ((static_cast<int>(position->y * 65536.0f) >> 22) * 0x10000) +
               ((static_cast<int>(position->x * 65536.0f) >> 22) & 0xffff);
    for (Clan *clan = ClanList.GetHead(); clan != ClanList.EndOfList(); clan = clan->GetNext()) {
        if (clan->Hash == static_cast<unsigned int>(hash)) {
            ClanList.Remove(clan);
            clan->LastUpdateTime = WorldTime;
            ClanList.AddHead(clan);
            return clan;
        }
    }

    if (ClanSlotPool->IsFull() && !ClanList.IsEmpty()) {
        Clan *last_clan = ClanList.GetTail();
        ClanList.Remove(last_clan);
        delete last_clan;
    }

    Clan *clan = new Clan(position, static_cast<unsigned int>(hash));
    ClanList.AddHead(clan);
    return clan;
}

void RenderClans(eView *view) {
    Clan *next_clan = ClanList.GetHead();
    while (next_clan != ClanList.EndOfList()) {
        Clan *clan = next_clan;
        next_clan = clan->GetNext();

        if (view->GetPixelSize(&clan->BBoxMin, &clan->BBoxMax) > 10 &&
            view->GetVisibleState(&clan->BBoxMin, &clan->BBoxMax, 0) != 0) {
            if (WorldTime - clan->LastUpdateTime > 300) {
                clan->LastUpdateTime = WorldTime;
                ClanList.Remove(clan);
                ClanList.AddHead(clan);
            }
            RenderSkids(view, clan);
        }
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
