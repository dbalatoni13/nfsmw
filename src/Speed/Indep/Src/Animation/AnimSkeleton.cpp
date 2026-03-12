#include "AnimSkeleton.hpp"
#include "AnimInternal.hpp"
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

void *CAnimSkeleton::operator new(size_t size, const char *debug_name) {
    if (!AnimSkelSlotPoolInitialized) {
        InitAnimSkelSlotPool();
    }

    return bOMalloc(AnimSkelSlotPool);
}

void CAnimSkeleton::operator delete(void *ptr) {
    bFree(AnimSkelSlotPool, ptr);
    if (bCountFreeSlots(AnimSkelSlotPool) == bCountTotalSlots(AnimSkelSlotPool)) {
        CloseAnimSkelSlotPool();
    }
}

int CAnimSkeleton::Initialize(char *data, int size) {
    if (data != nullptr) {
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
    if (m_pDynLoader != nullptr) {
        AnimBridgeDeleteDynamicLoader(m_pDynLoader);
        m_pDynLoader = nullptr;
    }
    m_pSkeleton = nullptr;
}

void CAnimSkeleton::DynamicLoadResolve() {
    if (m_pDynLoader != nullptr) {
        EAGL4::DynamicLoader::Symbol sym;
        int idx = 0;

        if (m_pDynLoader->GetNextSymbol("Skeleton", idx, sym)) {
            EAGL4Anim::Skeleton *skeleton = reinterpret_cast<EAGL4Anim::Skeleton *>(sym.data);
            m_pName = sym.name;
            m_NameHash = bStringHash(sym.name);
            m_pSkeleton = skeleton;
            if (skeleton != nullptr) {
                m_loaded = 1;
            } else {
                m_loaded = 0;
            }
        } else {
            m_pSkeleton = nullptr;
        }
    }
}

CAnimSkeleton *GetSkeletonFromList(unsigned int namehash) {
    CAnimSkeleton *skeleton = g_loadedSkeletonList.GetHead();
    while (skeleton != g_loadedSkeletonList.EndOfList()) {
        const char *name = skeleton->GetSkeletonName();
        if (name != nullptr && bStringHash(name) == namehash) {
            return skeleton;
        }
        skeleton = skeleton->GetNext();
    }

    return nullptr;
}

int LoaderEAGLSkeletons(bChunk *chunk) {
    if (chunk->ID == 0xE34009) {
        char *data = chunk->GetAlignedData(16);
        int offset = data - chunk->GetData();
        CAnimSkeleton *skeleton = new ("NFS CAnimSkeleton") CAnimSkeleton();

        skeleton->Initialize(data, chunk->Size - offset);
        skeleton->SetAssociatedChunk(chunk);
        g_loadedSkeletonList.AddTail(skeleton);

        return 1;
    }

    return 0;
}

int UnloaderEAGLSkeletons(bChunk *chunk) {
    if (chunk->ID == 0xE34009) {
        CAnimSkeleton *skeleton = g_loadedSkeletonList.GetHead();

        while (skeleton != g_loadedSkeletonList.EndOfList()) {
            CAnimSkeleton *next_skeleton = skeleton->GetNext();

            if (skeleton->GetAssociatedChunk() == chunk) {
                skeleton->Remove();
                skeleton->Cleanup();
                delete skeleton;
            }
            skeleton = next_skeleton;
        }
        return 1;
    }
    return 0;
}
