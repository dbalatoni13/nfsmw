#include "AnimPart.hpp"
#include "AnimInternal.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *AnimPartSlotPool;

CAnimPart::CAnimPart()
    : m_pSkeleton(nullptr),       //
      m_pSQTs(nullptr),           //
      m_SQTsize(0),               //
      m_pGlobalMatrices(nullptr), //
      m_numGlobalMatrices(0) {}

CAnimPart::~CAnimPart() {
    Purge();
}

#ifdef MILESTONE_OPT
static int NumAnimParts = 0;
static int MaxNumAnimParts = 0;
#endif

void CAnimPart::operator delete(void *ptr) {
#ifdef MILESTONE_OPT
    NumAnimParts--;
#endif
    bFree(AnimPartSlotPool, ptr);
}

int CAnimPart::Init(CAnimSkeleton *skeleton) {
    Purge();
    m_pSkeleton = skeleton;
    int alloc_size = skeleton->GetEAGLSkeleton()->GetPoseBufferLength();
    m_SQTsize = alloc_size;
    m_pSQTs = new ("EAGL4::SQT buffer CAnimPart", 0) float[alloc_size];
    for (int i = 0; i < alloc_size; i++) {
        m_pSQTs[i] = 0.0f;
    }
    if (m_pSQTs) {
        m_numGlobalMatrices = skeleton->GetEAGLSkeleton()->GetNumBones();
        m_pGlobalMatrices = AnimBridgeNewTransform("EAGL4::Transform GlobalPoseBuffer CAnimPart", m_numGlobalMatrices);
        if (m_pGlobalMatrices) {
            skeleton->GetEAGLSkeleton()->GetStillPose(m_pSQTs, nullptr);
            return true;
        }
    }
    Purge();
    return false;
}

void CAnimPart::Purge() {
    m_pSkeleton = nullptr;
    if (m_pSQTs) {
        delete[] m_pSQTs;
        m_pSQTs = nullptr;
    }
    m_SQTsize = 0;
    if (m_pGlobalMatrices) {
        AnimBridgeDeleteTransform(m_pGlobalMatrices);
        m_pGlobalMatrices = nullptr;
    }
    m_numGlobalMatrices = 0;
}
