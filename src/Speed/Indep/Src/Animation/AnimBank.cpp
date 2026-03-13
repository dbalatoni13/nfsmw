#include "AnimBank.hpp"
#include "AnimInternal.hpp"
#include "Speed/Indep/Src/EAGL4Anim/FnDefaultAnimBank.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4AnimBank.h"
#include "Speed/Indep/Src/EAGL4Anim/eagl4supportdlopen.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Libs/Support/Utility/UMath.h"

SlotPool *AnimBankSlotPool = nullptr;
bool AnimBankSlotPoolInitialized = false;

CAnimBank::CAnimBank() {
    m_internalDynLoader = 0;
    m_pDynLoader = nullptr;
    m_DynLoaderSize = 0;
    m_pFnAnimBanks = nullptr;
    m_pAnimBank = nullptr;
    m_maxBanks = 0;
    m_pBankIDs = nullptr;
    m_pName = nullptr;
    m_numAnimEnumList = 0;
    m_pAnimEnumList = nullptr;
    m_purge = false;
}

CAnimBank::~CAnimBank() {}

EAGL4Anim::AnimBank *GetNextAnimBank(const EAGL4::DynamicLoader *loader, int &index) {
    void *addr;
    bool symFound = loader->GetNextAddr(EAGL4::DynamicLoader::AnimBankType, index, addr);
    if (symFound) {
        return reinterpret_cast<EAGL4Anim::AnimBank *>(addr);
    } else {
        return nullptr;
    }
}

EAGL4Anim::AnimBank *GetFirstAnimBank(const EAGL4::DynamicLoader *loader) {
    int index = 0;
    EAGL4Anim::AnimBank *firstBank = GetNextAnimBank(loader, index);
    return firstBank;
}

int CAnimBank::Initialize(char *data, int size) {
    m_pDynLoader = AnimBridgeNewDynamicLoader("EAGL4::DynamicLoder CAnimBank", data, size);
    m_DynLoaderSize = size;
    m_internalDynLoader = 1;
    m_pFnAnimBanks = new ("EAGL4::FnDefaultAnimBank CAnimBank", 0) EAGL4Anim::FnDefaultAnimBank();
    m_pAnimBank = GetFirstAnimBank(m_pDynLoader);
    m_pFnAnimBanks->Init(m_pAnimBank);

    int num_anims = m_pFnAnimBanks->GetNumAnims() + m_pAnimBank->GetNumAnims();

    return 0;
}

void CAnimBank::Cleanup() {
    m_internalDynLoader = 0;
    m_DynLoaderSize = 0;
    m_maxBanks = 0;
    m_numAnimEnumList = 0;
    m_pAnimBank = nullptr;
    m_pName = nullptr;
    m_pAnimEnumList = nullptr;
    m_purge = false;

    delete m_pFnAnimBanks;
    // TODO why isn't this if auto-generated? hmm
    if (m_pAnimBank) {
        delete m_pAnimBank;
    }

    if (m_pDynLoader) {
        AnimBridgeDeleteDynamicLoader(m_pDynLoader);
    }
}

void InitAnimBankSlotPool() {
    if (!AnimBankSlotPoolInitialized) {
        AnimBankSlotPool = bNewSlotPool(60, 81, "Anim_CNFSAnimBank_SlotPool", GetVirtualMemoryAllocParams());
        AnimBankSlotPoolInitialized = true;
    }
}

void CloseAnimBankSlotPool() {
    bDeleteSlotPool(AnimBankSlotPool);
    AnimBankSlotPool = nullptr;
    AnimBankSlotPoolInitialized = false;
}

bTList<CNFSAnimBank> g_loadedAnimBankList;

void *CNFSAnimBank::operator new(size_t size, const char *debug_name) {
    if (!AnimBankSlotPoolInitialized) {
        InitAnimBankSlotPool();
    }

    return bOMalloc(AnimBankSlotPool);
}

void CNFSAnimBank::operator delete(void *ptr) {
    bFree(AnimBankSlotPool, ptr);
    if (bCountFreeSlots(AnimBankSlotPool) == bCountTotalSlots(AnimBankSlotPool)) {
        CloseAnimBankSlotPool();
    }
}

CNFSAnimBank::CNFSAnimBank(struct bChunk *chunk) {
    Chunk = chunk;
}

CNFSAnimBank::~CNFSAnimBank() {}

void DumpAnimBanks() {
    CAnimBank *bank;
    int num_anims;
    const char *animation_item_name;
    unsigned int anim_hash;

    for (bank = g_loadedAnimBankList.GetHead(); bank != g_loadedAnimBankList.EndOfList(); bank = bank->GetNext()) {
        EAGL4Anim::AnimBank *eagl_anim_bank = bank->GetAnimBank();
        if (!eagl_anim_bank) {
            continue;
        }
        num_anims = eagl_anim_bank->GetNumAnims();

        for (int loop = 0; loop < num_anims; loop++) {
            animation_item_name = eagl_anim_bank->GetAnimName(loop);
            anim_hash = bStringHash(animation_item_name);
        }
    }
}

int GetAnimFromBankByNamehash(unsigned int namehash, EAGL4Anim::AnimBank **animBank, int *item_index) {
    CAnimBank *bank;
    int num_anims;
    const char *animation_item_name;
    unsigned int anim_hash;

    for (bank = g_loadedAnimBankList.GetHead(); bank != g_loadedAnimBankList.EndOfList(); bank = bank->GetNext()) {
        EAGL4Anim::AnimBank *eagl_anim_bank = bank->GetAnimBank();
        if (!eagl_anim_bank) {
            continue;
        }
        num_anims = eagl_anim_bank->GetNumAnims();

        for (int loop = 0; loop < num_anims; loop++) {
            animation_item_name = eagl_anim_bank->GetAnimName(loop);
            anim_hash = bStringHash(animation_item_name);
            if (namehash == anim_hash) {
                *animBank = eagl_anim_bank;
                *item_index = loop;
                return true;
            }
        }
    }
    return false;
}

int LoaderEAGLAnimations(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_EAGL_ANIMATIONS) {
        if (chunk->GetAlignedSize(16) > 0) {
            CNFSAnimBank *anim_bank = new ("NFS CNFSAnimBank") CNFSAnimBank(chunk);

            anim_bank->Initialize(chunk->GetAlignedData(16), chunk->GetAlignedSize(16));
            g_loadedAnimBankList.AddTail(anim_bank);
        }
        return 1;
    } else {
        return 0;
    }
}

int UnloaderEAGLAnimations(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_EAGL_ANIMATIONS) {
        for (CAnimBank *anim_bank = g_loadedAnimBankList.GetHead(); anim_bank != g_loadedAnimBankList.EndOfList(); anim_bank = anim_bank->GetNext()) {
            if (static_cast<CNFSAnimBank *>(anim_bank)->GetChunk() == chunk) {
                anim_bank->Cleanup();
                g_loadedAnimBankList.Remove(anim_bank);
                delete anim_bank;
                break;
            }
        }
        return 1;
    } else {
        return 0;
    }
}
