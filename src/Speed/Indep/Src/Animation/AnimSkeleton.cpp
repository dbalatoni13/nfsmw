#include "AnimSkeleton.hpp"
#include "AnimInternal.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

SlotPool *AnimSkelSlotPool = nullptr;
bool AnimSkelSlotPoolInitialized = false;
bTList<CAnimSkeleton> g_loadedSkeletonList;

CAnimSkeleton::CAnimSkeleton() {
    m_loaded = 0;
    m_pName = nullptr;
    m_NameHash = 0;
    mAssocChunk = nullptr;
    m_internalDynLoader = 0;
    m_pDynLoader = nullptr;
    m_pSkeleton = nullptr;
}

CAnimSkeleton::~CAnimSkeleton() {}

void InitAnimSkelSlotPool() {
    if (!AnimSkelSlotPoolInitialized) {
        AnimSkelSlotPool = bNewSlotPool(40, 8, "AnimSkelSlotPool", 0);
        AnimSkelSlotPoolInitialized = true;
    }
}

void CloseAnimSkelSlotPool() {
    bDeleteSlotPool(AnimSkelSlotPool);
    AnimSkelSlotPool = nullptr;
    AnimSkelSlotPoolInitialized = false;
}

#ifdef MILESTONE_OPT
static int NumAnimSkels = 0;
static int MaxNumAnimSkels = 0;
#endif

void *CAnimSkeleton::operator new(size_t size, const char *debug_name) {
    if (!AnimSkelSlotPoolInitialized) {
        InitAnimSkelSlotPool();
    }

#ifdef MILESTONE_OPT
    NumAnimSkels++;
    if (MaxNumAnimSkels < NumAnimSkels) {
        MaxNumAnimSkels = NumAnimSkels;
    }
#endif

    return bOMalloc(AnimSkelSlotPool);
}

void CAnimSkeleton::operator delete(void *ptr) {
#ifdef MILESTONE_OPT
    NumAnimSkels--;
#endif

    bFree(AnimSkelSlotPool, ptr);
    if (bCountFreeSlots(AnimSkelSlotPool) == bCountTotalSlots(AnimSkelSlotPool)) {
        CloseAnimSkelSlotPool();
    }
}

int CAnimSkeleton::Initialize(char *data, int size) {
    if (data) {
        m_pDynLoader = AnimBridgeNewDynamicLoader("EAGL4::DynamicLoader CAnimSkeleton", data, size);
        m_DynLoaderSize = size;
        m_internalDynLoader = 1;
        DynamicLoadResolve();
    }

    return 0;
}

void CAnimSkeleton::Cleanup() {
    m_loaded = 0;
    m_pName = nullptr;
    m_NameHash = 0;
    m_internalDynLoader = 0;
    if (m_pDynLoader) {
        AnimBridgeDeleteDynamicLoader(m_pDynLoader);
        m_pDynLoader = nullptr;
    }
    m_pSkeleton = nullptr;
}

void CAnimSkeleton::DynamicLoadResolve() {
    if (m_pDynLoader) {
        EAGL4::DynamicLoader::Symbol sym;
        int idx = 0;

        if (m_pDynLoader->GetNextSymbol("Skeleton", idx, sym)) {
            EAGL4Anim::Skeleton *skeleton = reinterpret_cast<EAGL4Anim::Skeleton *>(sym.data);
            m_pName = sym.name;
            m_NameHash = bStringHash(sym.name);
            m_pSkeleton = skeleton;
            if (skeleton) {
                m_loaded = 1;
            } else {
                m_loaded = 0;
            }
        } else {
            m_pSkeleton = nullptr;
        }
    }
}

CAnimSkeleton *GetSkeletonFromList(uint32 namehash) {
    CAnimSkeleton *skel_list = g_loadedSkeletonList.GetHead();
    while (skel_list != g_loadedSkeletonList.EndOfList()) {
        if (skel_list->GetSkeletonName() && bStringHash(skel_list->GetSkeletonName()) == namehash) {
            return skel_list;
        }
        skel_list = skel_list->GetNext();
    }

    return nullptr;
}

int LoaderEAGLSkeletons(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_EAGL_SKELETONS) {
        int size = chunk->GetAlignedSize(0x10); // unused
        CAnimSkeleton *skeleton = new ("NFS CAnimSkeleton") CAnimSkeleton();

        skeleton->Initialize(chunk->GetAlignedData(0x10), chunk->GetAlignedSize(0x10));
        skeleton->SetAssociatedChunk(chunk);
        g_loadedSkeletonList.AddTail(skeleton);

        return 1;
    }

    return 0;
}

int UnloaderEAGLSkeletons(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_EAGL_SKELETONS) {
        CAnimSkeleton *skel = g_loadedSkeletonList.GetHead();

        while (skel != g_loadedSkeletonList.EndOfList()) {
            CAnimSkeleton *next_skel = skel->GetNext();
            struct bChunk *assoc_chunk = skel->GetAssociatedChunk();

            if (assoc_chunk == chunk) {
                skel->Remove();
                skel->Cleanup();
                delete skel;
            }
            skel = next_skel;
        }
        return 1;
    }
    return 0;
}
