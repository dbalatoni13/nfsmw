#include "Speed/Indep/Src/Frontend/FERenderObject.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"

extern SlotPool *mpobFERenderObjectSlotPool;
extern SlotPool *FERenderEPolySlotPool;
extern SlotPool *FERenderEPolySlotPoolOverflow;
extern void PSMTX44Identity(float *mtx);

void FERenderObject::Initialize() {
    mpobFERenderObjectSlotPool = bNewSlotPool(0x64, 0x180, "FERenderObjectSlotPool", 0);
    FERenderEPolySlotPool = bNewSlotPool(0xA4, 0x348, "FERenderEPolySlotPool", 0);
}

FERenderObject::FERenderObject(FEObject *obj, TextureInfo *tex) {
    mpobOwner = obj;
    mpobTexture = tex;
    mobPolyList.InitList();
    mulNumTimesRendered = 0;
    mulFlags = 0;
    mPolyCount = 0;
    PSMTX44Identity(reinterpret_cast<float *>(&mstTransform));
}

FERenderObject::~FERenderObject() {
    while (mobPolyList.GetHead() != mobPolyList.EndOfList()) {
        FERenderEPoly *poly = mobPolyList.GetHead();
        poly->Remove();
        delete poly;
    }
}

void *FERenderEPoly::operator new(unsigned int) {
    if (FERenderEPolySlotPool->NumAllocatedSlots != FERenderEPolySlotPool->TotalNumSlots) {
        return FERenderEPolySlotPool->FastMalloc();
    }
    if (!FERenderEPolySlotPoolOverflow) {
        FERenderEPolySlotPoolOverflow = bNewSlotPool(0xA4, 0x200, "FERenderEPolySlotPoolOverflow", 0);
        FERenderEPolySlotPoolOverflow->ClearFlag(SLOTPOOL_FLAG_WARN_IF_OVERFLOW);
    }
    return FERenderEPolySlotPoolOverflow->Malloc();
}

void FERenderEPoly::operator delete(void *p) {
    if (FERenderEPolySlotPool->GetSlotNumber(p) >= 0) {
        FERenderEPolySlotPool->Free(p);
    } else {
        FERenderEPolySlotPoolOverflow->Free(p);
        if (FERenderEPolySlotPoolOverflow->NumAllocatedSlots == 0) {
            bDeleteSlotPool(FERenderEPolySlotPoolOverflow);
            FERenderEPolySlotPoolOverflow = nullptr;
        }
    }
}

bVector4 V4Mult(const bVector4 &v, float d) {
    return bVector4(v.x * d, v.y * d, v.z * d, v.w * d);
}

void FERenderObject::SetTransform(bMatrix4 *pMatrix) {
    bMemCpy(&mstTransform, pMatrix, sizeof(bMatrix4));
}