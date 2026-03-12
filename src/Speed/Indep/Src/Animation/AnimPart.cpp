#include "AnimPart.hpp"
#include "AnimInternal.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *AnimPartSlotPool;

CAnimPart::CAnimPart()
    : m_pSkeleton(nullptr),      //
      m_pSQTs(nullptr),          //
      m_SQTsize(0),              //
      m_pGlobalMatrices(nullptr), //
      m_numGlobalMatrices(0) {}

CAnimPart::~CAnimPart() {
    Purge();
}

void CAnimPart::operator delete(void *ptr) {
    bFree(AnimPartSlotPool, ptr);
}

int CAnimPart::Init(CAnimSkeleton *skeleton) {
    Purge();
    m_pSkeleton = skeleton;
    int num_bones = skeleton->GetEAGLSkeleton()->GetNumBones();
    int sqt_size = num_bones * 0xC;
    m_SQTsize = sqt_size;
    m_pSQTs = new ("EAGL4::SQT buffer CAnimPart", 0) float[sqt_size];
    for (int i = 0; i < sqt_size; i++) {
        m_pSQTs[i] = 0.0f;
    }
    if (m_pSQTs != nullptr) {
        m_numGlobalMatrices = skeleton->GetEAGLSkeleton()->GetNumBones();
        m_pGlobalMatrices = AnimBridgeNewTransform("EAGL4::Transform GlobalPoseBuffer CAnimPart", m_numGlobalMatrices);
        if (m_pGlobalMatrices != nullptr) {
            skeleton->GetEAGLSkeleton()->GetStillPose(m_pSQTs, nullptr);
            return true;
        }
    }
    Purge();
    return false;
}

void CAnimPart::Purge() {
    m_pSkeleton = nullptr;
    if (m_pSQTs != nullptr) {
        delete[] m_pSQTs;
        m_pSQTs = nullptr;
    }
    m_SQTsize = 0;
    if (m_pGlobalMatrices != nullptr) {
        AnimBridgeDeleteTransform(m_pGlobalMatrices);
        m_pGlobalMatrices = nullptr;
    }
    m_numGlobalMatrices = 0;
}
