#include "WorldAnimInstanceDirectory.hpp"

#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int AnimCfg_DisableWorldAnimations;

WorldAnimInstanceDirectory TheWorldAnimInstanceDirectory; // size: 0x50, address: 0x80457718

static int NumWorldAnimInstanceEntries = 0; // size: 0x4, address: 0x80415898
static int MaxWorldAnimInstances = 0;       // size: 0x4, address: 0x80415894

// STRIPPED
int GetMaxSlotPoolSizes_ForInstances() {}

void *WorldAnimInstanceEntry::operator new(size_t size, const char *debug_name) {
    NumWorldAnimInstanceEntries++;
    if (NumWorldAnimInstanceEntries > MaxWorldAnimInstances) {
        MaxWorldAnimInstances = NumWorldAnimInstanceEntries;
    }
    return bOMalloc(TheWorldAnimInstanceDirectory.GetWorldAnimInstanceEntrySlotPool());
}

void WorldAnimInstanceEntry::operator delete(void *ptr) {
    NumWorldAnimInstanceEntries--;
    bFree(TheWorldAnimInstanceDirectory.GetWorldAnimInstanceEntrySlotPool(), ptr);
}

// STRIPPED
void DumpWorldAnimMemUsage() {}

WorldAnimInstanceDirectory::WorldAnimInstanceDirectory() {
    mInitialized = false;
    mDeInitializing = false;
}

WorldAnimInstanceEntry::WorldAnimInstanceEntry() {
    mAnimInstance = nullptr;
    mInstanceEntryInfo = nullptr;
}

WorldAnimEntityTreeInfo *WorldAnimInstanceDirectory::GetAnimTreeInfo(uint32 anim_tree_name_hash) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return nullptr;
    }
    if (!mInitialized) {
        Init();
    }
    for (WorldAnimEntityTreeInfo *tree = mLoadedWorldAnimTrees.GetHead(); tree != mLoadedWorldAnimTrees.EndOfList(); tree = tree->GetNext()) {
        if (tree->tree_name_hash == anim_tree_name_hash) {
            return tree;
        }
    }
    return nullptr;
}

// STRIPPED
bool GetAnimPosition(int instance_id, uint32 entity_hash, bVector3 *pos_out, bool *is_between_loops) {}

void WorldAnimInstanceDirectory::AddLoadedAnimTreeInfo(WorldAnimEntityTreeInfo *anim_tree_info) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    if (!mInitialized) {
        Init();
    }
    mLoadedWorldAnimTrees.AddTail(anim_tree_info);
}

// STRIPPED
WorldAnimEntityInfo *WorldAnimInstanceDirectory::GetAnimEntityInfo(uint32 anim_entity_name_hash) {}

void WorldAnimInstanceDirectory::AddLoadedAnimEntityInfo(WorldAnimEntityInfo *anim_entity_info) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    if (!mInitialized) {
        Init();
    }
    mLoadedWorldAnimEntityInfos.AddTail(anim_entity_info);
}

void WorldAnimInstanceDirectory::RemoveAndDeleteAllAnimTreeInfos() {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    while (!mLoadedWorldAnimTrees.IsEmpty()) {
        WorldAnimEntityTreeInfo *tree = mLoadedWorldAnimTrees.GetTail();
        tree->Remove();
        delete tree;
    }
}

void WorldAnimInstanceDirectory::RemoveAndDeleteAnimTreeInfo(uint32 tree_name_hash) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }

    for (WorldAnimEntityTreeInfo *tree = mLoadedWorldAnimTrees.GetHead(); tree != mLoadedWorldAnimTrees.EndOfList(); tree = tree->GetNext()) {
        if (tree->tree_name_hash == tree_name_hash) {
            tree->Remove();
            delete tree;
            break;
        }
    }
}

void WorldAnimInstanceDirectory::RemoveEntityInfo(WorldAnimEntityInfo *entity_info) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    for (bPNode *node = mLoadedWorldAnimEntityInfos.GetHead(); node != mLoadedWorldAnimEntityInfos.EndOfList(); node = node->GetNext()) {
        WorldAnimEntityInfo *waei = reinterpret_cast<WorldAnimEntityInfo *>(node->GetpObject());
        if (waei == entity_info) {
            mLoadedWorldAnimEntityInfos.Remove(node);
            return;
        }
    }
}

void WorldAnimInstanceDirectory::AddLoadedAnimInstance(WorldAnimInstance *instance) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    if (!mInitialized) {
        Init();
    }
    mLoadedWorldAnimInstance.AddTail(instance);
}

void WorldAnimInstanceDirectory::RemoveAnimInstance(WorldAnimInstance *instance) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    for (bPNode *node = mLoadedWorldAnimInstance.GetHead(); node != mLoadedWorldAnimInstance.EndOfList(); node = node->GetNext()) {
        WorldAnimInstance *wai = reinterpret_cast<WorldAnimInstance *>(node->GetObject());
        if (wai == instance) {
            mLoadedWorldAnimInstance.Remove(node);
            return;
        }
    }
}

int WorldAnimInstanceDirectory::GetNumInstanceEntries() {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return -1;
    }
    return GetNumLoadedInstances();
}

int WorldAnimInstanceDirectory::GetNumLoadedInstances() {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return -1;
    }
    return mLoadedWorldAnimInstance.CountElements();
}

bPList<WorldAnimInstance> &WorldAnimInstanceDirectory::GetInstanceList() {
    return mLoadedWorldAnimInstance;
}

void WorldAnimInstanceDirectory::RemoveAndDeleteAllInstanceEntries() {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    while (!mResidentWorldAnimInstanceEntries.IsEmpty()) {
        WorldAnimInstanceEntry *new_entry = mResidentWorldAnimInstanceEntries.GetTail();
        new_entry->Remove();
        delete new_entry;
    }
}

void WorldAnimInstanceDirectory::AddInstanceEntryFromInfo(WorldAnimInstanceEntryInfo *entry_info) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    if (!mInitialized) {
        Init();
    }
    mResidentWorldAnimInstanceEntryInfos.AddTail(entry_info);
    WorldAnimInstanceEntry *new_entry = new ("WorldAnimInstanceEntry") WorldAnimInstanceEntry();
    new_entry->mInstanceEntryInfo = entry_info;
    new_entry->mAnimInstance = nullptr;
    mResidentWorldAnimInstanceEntries.AddTail(new_entry);
}

void WorldAnimInstanceDirectory::RemoveInstanceEntryAndInfo(WorldAnimInstanceEntryInfo *entry_info) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    for (WorldAnimInstanceEntry *entry = mResidentWorldAnimInstanceEntries.GetHead(); entry != mResidentWorldAnimInstanceEntries.EndOfList();
         entry = entry->GetNext()) {
        if (entry->mInstanceEntryInfo == entry_info) {
            entry->Remove();
            delete entry;
            break;
        }
    }
    for (bPNode *node = mResidentWorldAnimInstanceEntryInfos.GetHead(); node != mResidentWorldAnimInstanceEntryInfos.EndOfList();
         node = node->GetNext()) {
        WorldAnimInstanceEntryInfo *info = reinterpret_cast<WorldAnimInstanceEntryInfo *>(node->GetpObject());
        if (info == entry_info) {
            mResidentWorldAnimInstanceEntryInfos.Remove(node);
            break;
        }
    }
}

void WorldAnimInstanceDirectory::Init() {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
        return;
    }
    if (mInitialized || mDeInitializing) {
        return;
    }
    InitAnimControlScenarios(mAnimControlScenarios);
    mInitialized = true;

#ifdef EA_BUILD_A124
    WorldAnimEntitySlotPool = bNewSlotPool(0x24, 0xD5, "WorldAnimEntity_SlotPool", 0);
    WorldAnimEntityTreeSlotPool = bNewSlotPool(0x2C, 0x42, "WorldAnimEntityTree_SlotPool", 0);
    WorldAnimEntityTreeInfoSlotPool = bNewSlotPool(0x58, 0x27, "WorldAnimEntityTreeInfo_SlotPool", 0);
    WorldAnimCtrlSlotPool = bNewSlotPool(0x74, 0x95, "WorldAnimCtrl_SlotPool", 0);
    WorldAnimInstanceEntrySlotPool = bNewSlotPool(0x10, 0x42, "WorldAnimInstanceEntry_SlotPool", 0);
#else
    WorldAnimEntitySlotPool = bNewSlotPool(0x24, 0xD5, "WorldAnimEntity_SlotPool", GetVirtualMemoryAllocParams());
    WorldAnimEntityTreeSlotPool = bNewSlotPool(0x2C, 0x42, "WorldAnimEntityTree_SlotPool", GetVirtualMemoryAllocParams());
    WorldAnimEntityTreeInfoSlotPool = bNewSlotPool(0x58, 0x27, "WorldAnimEntityTreeInfo_SlotPool", GetVirtualMemoryAllocParams());
    WorldAnimCtrlSlotPool = bNewSlotPool(0x74, 0x95, "WorldAnimCtrl_SlotPool", GetVirtualMemoryAllocParams());
    WorldAnimInstanceEntrySlotPool = bNewSlotPool(0x10, 0x42, "WorldAnimInstanceEntry_SlotPool", GetVirtualMemoryAllocParams());
#endif
}

WorldAnimInstanceDirectory::~WorldAnimInstanceDirectory() {
    DeInit(true, false);
}

void WorldAnimInstanceDirectory::DeInit(bool full_unload, bool quickrace_drag_restart) {
    if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations || !full_unload) {
        return;
    }
    mDeInitializing = true;
    CleanControlScenarios(mAnimControlScenarios);
    if (!quickrace_drag_restart) {
        RemoveAndDeleteAllAnimTreeInfos();
        RemoveAndDeleteAllInstanceEntries();
    }
    if (WorldAnimCtrlSlotPool != nullptr) {
        bDeleteSlotPool(WorldAnimCtrlSlotPool);
        WorldAnimCtrlSlotPool = nullptr;
    }
    if (WorldAnimEntitySlotPool != nullptr) {
        bDeleteSlotPool(WorldAnimEntitySlotPool);
        WorldAnimEntitySlotPool = nullptr;
    }
    if (WorldAnimEntityTreeInfoSlotPool != nullptr) {
        bDeleteSlotPool(WorldAnimEntityTreeInfoSlotPool);
        WorldAnimEntityTreeInfoSlotPool = nullptr;
    }
    if (WorldAnimEntityTreeSlotPool != nullptr) {
        bDeleteSlotPool(WorldAnimEntityTreeSlotPool);
        WorldAnimEntityTreeSlotPool = nullptr;
    }
    if (WorldAnimInstanceEntrySlotPool != nullptr) {
        bDeleteSlotPool(WorldAnimInstanceEntrySlotPool);
        WorldAnimInstanceEntrySlotPool = nullptr;
    }
    mInitialized = false;
    mDeInitializing = false;
}

int LoaderWorldAnimInstanceEntry(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_WORLD_ANIM_CTL) {
        return 0;
    }

    int num_objects = chunk->GetAlignedSize(0x10) / sizeof(WorldAnimInstanceEntryInfo);
    for (int n = 0; n < num_objects; n++) {
        if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
            return 1;
        }

        WorldAnimInstanceEntryInfo *waiei = &reinterpret_cast<WorldAnimInstanceEntryInfo *>(chunk->GetAlignedData(16))[n];
        bPlatEndianSwap(&waiei->mAnimTreeNameHash);
        bPlatEndianSwap(&waiei->mUniqueInstanceID);
        bPlatEndianSwap(&waiei->mSectionNumber);
        bPlatEndianSwap(&waiei->mEvalTime);
        TheWorldAnimInstanceDirectory.AddInstanceEntryFromInfo(waiei);
    }

    return 1;
}

int UnloaderWorldAnimInstanceEntry(bChunk *chunk) {
    if (chunk->GetID() != BCHUNK_WORLD_ANIM_CTL) {
        return 0;
    }

    int num_objects = chunk->GetAlignedSize(16) / sizeof(WorldAnimInstanceEntryInfo);
    for (int n = 0; n < num_objects; n++) {
        if (AnimCfg_DisableWorldAnimations || DisableWorldAnimations) {
            return 1;
        }
        WorldAnimInstanceEntryInfo *waiei = &reinterpret_cast<WorldAnimInstanceEntryInfo *>(chunk->GetAlignedData(16))[n];
        TheWorldAnimInstanceDirectory.RemoveInstanceEntryAndInfo(waiei);
    }

    return 1;
}

// STRIPPED
IControlScenario *WorldAnimInstanceDirectory::GetControlScenario(eControlScenarioType cst) {}

// STRIPPED
CWorldAnimEntityTree *WorldAnimInstanceDirectory::GetAnimationAssociatedWithTriggerZone(uint32 trigger_zone_hash) {}
