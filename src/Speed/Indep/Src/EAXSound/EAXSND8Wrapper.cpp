#include <types.h>

struct SlotPool;

extern SlotPool *pCsisSlotPools[];
extern int nCsisSlotPoolSizes[];

EAXSND8Wrapper::EAXSND8Wrapper() {
    m_pSoundHeap = nullptr;
    m_pStreamBuff = nullptr;
    pCsisSlotPools[0] = nullptr;
    nCsisSlotPoolSizes[0] = 0x88;
}

void EAXSND8Wrapper::Update() {
    STUPID();
}

void EAXSND8Wrapper::STUPID() {}
