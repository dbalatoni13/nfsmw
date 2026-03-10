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
    EAGL4Anim::Skeleton *eagl_skeleton;
    int num_bones;

    Purge();
    eagl_skeleton = skeleton->GetEAGLSkeleton();
    num_bones = eagl_skeleton->GetNumBones();
    m_pSkeleton = skeleton;
    m_SQTsize = num_bones * 0xC;
    m_pSQTs = new ("EAGL4::SQT buffer CAnimPart", 0) float[num_bones * 0xC];
    for (int i = 0; i < m_SQTsize; i++) {
        m_pSQTs[i] = 0.0f;
    }
    if (m_pSQTs != nullptr) {
        m_numGlobalMatrices = num_bones;
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
