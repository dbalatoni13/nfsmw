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
        AnimSkelSlotPool = bNewSlotPool(40, 32, "AnimSkelSlotPool", GetVirtualMemoryAllocParams());
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

        sym.name = nullptr;
        sym.data = nullptr;
        if (m_pDynLoader->GetNextSymbol("Skeleton", idx, sym)) {
            m_pName = sym.name;
            m_NameHash = bStringHash(sym.name);
            m_pSkeleton = reinterpret_cast< EAGL4Anim::Skeleton * >(sym.data);
            if (sym.data == nullptr) {
                m_loaded = 0;
            } else {
                m_loaded = 1;
            }
        } else {
            m_pSkeleton = nullptr;
        }
    }
}

CAnimSkeleton *GetSkeletonFromList(unsigned int namehash) {
    if (g_loadedSkeletonList.HeadNode.Next != &g_loadedSkeletonList.HeadNode) {
        CAnimSkeleton *skeleton = reinterpret_cast< CAnimSkeleton * >(g_loadedSkeletonList.HeadNode.Next);
        const char *name = skeleton->GetSkeletonName();

        while (true) {
            if (name != nullptr && bStringHash(name) == namehash) {
                return skeleton;
            }
            skeleton = skeleton->GetNext();
            if (skeleton == reinterpret_cast< CAnimSkeleton * >(&g_loadedSkeletonList)) {
                break;
            }
            name = skeleton->GetSkeletonName();
        }
    }

    return nullptr;
}

int LoaderEAGLSkeletons(bChunk *chunk) {
    bool result = chunk->ID == 0xE34009;

    if (result) {
        char *data = reinterpret_cast< char * >(reinterpret_cast< uintptr_t >(&chunk[2].Size + 1) & static_cast< uintptr_t >(0xFFFFFFF0));
        CAnimSkeleton *skeleton = new ("NFS CAnimSkeleton") CAnimSkeleton();

        skeleton->Initialize(data, chunk->Size - (reinterpret_cast< int >(data) - reinterpret_cast< int >(chunk + 1)));
        skeleton->SetAssociatedChunk(chunk);
        g_loadedSkeletonList.HeadNode.Prev->Next = skeleton;
        skeleton->Prev = g_loadedSkeletonList.HeadNode.Prev;
        g_loadedSkeletonList.HeadNode.Prev = skeleton;
        skeleton->Next = &g_loadedSkeletonList.HeadNode;
    }

    return result;
}

int UnloaderEAGLSkeletons(bChunk *chunk) {
    int result = 0;

    if (chunk->ID == 0xE34009) {
        if (g_loadedSkeletonList.HeadNode.Next != &g_loadedSkeletonList.HeadNode) {
            CAnimSkeleton *skeleton = reinterpret_cast< CAnimSkeleton * >(g_loadedSkeletonList.HeadNode.Next);

            while (true) {
                CAnimSkeleton *next_skeleton = reinterpret_cast< CAnimSkeleton * >(skeleton->Next);

                if (skeleton->GetAssociatedChunk() == chunk) {
                    skeleton->Prev->Next = next_skeleton;
                    next_skeleton->Prev = skeleton->Prev;
                    skeleton->Cleanup();
                    delete skeleton;
                }
                if (next_skeleton == reinterpret_cast< CAnimSkeleton * >(&g_loadedSkeletonList)) {
                    break;
                }
                skeleton = next_skeleton;
            }
        }
        result = 1;
    }

    return result;
}
