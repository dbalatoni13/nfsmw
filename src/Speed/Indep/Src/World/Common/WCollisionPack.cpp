#include "Speed/Indep/Src/World/WCollisionPack.h"

#include "Speed/Indep/Src/Sim/SimSurface.h"

WCollisionPack::WCollisionPack(bChunk *chunk)
    : mSectionNumber(0),      //
      mInstanceNum(0),        //
      mInstanceList(nullptr), //
      mObjectNum(0),          //
      mObjectList(nullptr),   //
      mCarpChunkHeader(reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(16))) {
    Init(chunk);
}

WCollisionPack::~WCollisionPack() {
    DeInit();
}

void WCollisionPack::Init(bChunk *chunk) {
    if (!mCarpChunkHeader->IsResolved()) {
        mCarpChunkHeader->PlatformEndianSwap();
    }

    mSectionNumber = mCarpChunkHeader->GetSectionNumber();
    {
        void *crpData = mCarpChunkHeader + 1;
        const int carpSourceCount = 1;
        const void *carpSource[1] = {crpData};
        int carpSize[carpSourceCount];
        unsigned int deltaRelocationOffset = 0;
        carpSize[0] = mCarpChunkHeader->GetCarpSize();

        if (mCarpChunkHeader->IsResolved()) {
            deltaRelocationOffset = reinterpret_cast<uintptr_t>(mCarpChunkHeader) - reinterpret_cast<uintptr_t>(mCarpChunkHeader->GetLastAddress());
        }

        const UGroup *carpGroup;
        const UGroup *cGroup;
        if (mCarpChunkHeader != mCarpChunkHeader->GetLastAddress()) {
            carpGroup = UGroup::Deserialize(carpSourceCount, reinterpret_cast<unsigned int *>(carpSize), carpSource, deltaRelocationOffset);
        } else {
            carpGroup = reinterpret_cast<const UGroup *>(carpSource[0]);
        }

        Resolve(carpGroup->GroupLocate(0x41720000, 0x7469), 0);
        mCarpChunkHeader->SetLastAddress(mCarpChunkHeader);
        mCarpChunkHeader->SetResolved();
    }
}

void WCollisionPack::DeInit() {
    mSectionNumber = 0;
    mInstanceList = nullptr;
    mObjectList = nullptr;
    mInstanceNum = 0;
    mObjectNum = 0;
}

void WCollisionArticle::Resolve() {
    if (!fResolvedFlag) {
        for (int ind = 0; ind < fNumSurfaces; ++ind) {
            char *dataStart = reinterpret_cast<char *>(reinterpret_cast<char *>(&this[1]) + fStripsSize + fEdgesSize);
            unsigned int hash = reinterpret_cast<unsigned int *>(dataStart)[ind];
            // TODO 64 bit, gotta reallocate and use uintptr_t
            reinterpret_cast<unsigned int *>(dataStart)[ind] = reinterpret_cast<unsigned int>(SimSurface::Lookup(hash));
        }
        fResolvedFlag = true;
    }
}

void WCollisionPack::Resolve(const UGroup *cGroup, unsigned int deltaAddress) {
    const UData *instanceUData = cGroup->DataLocate(0x63690000, 0);
    const UData *objectUData = cGroup->DataLocate(0x636F0000, 0);

    if (instanceUData != cGroup->DataEnd()) {
        mInstanceList = reinterpret_cast<const WCollisionInstance *>(instanceUData->GetDataConst());
        mInstanceNum = instanceUData->DataCount();
    } else {
        mInstanceList = nullptr;
        mInstanceNum = 0;
    }

    if (objectUData != cGroup->DataEnd()) {
        mObjectList = reinterpret_cast<const WCollisionObject *>(objectUData->GetDataConst());
        mObjectNum = objectUData->DataCount();
    } else {
        mObjectList = nullptr;
        mObjectNum = 0;
    }

    for (unsigned int i = 0; i < mInstanceNum; ++i) {
        const WCollisionInstance *cInst = &mInstanceList[i];
        if (deltaAddress == 0) {
            const UData *articleUData = cGroup->DataLocate(0x63612020, cInst->fRenderInstanceInd);
            WCollisionArticle *cArt;
            if (articleUData != cGroup->DataEnd()) {
                cArt = reinterpret_cast<WCollisionArticle *>(const_cast<void *>(articleUData->GetDataConst()));
            } else {
                cArt = nullptr;
            }

            cInst->fCollisionArticle = cArt;
            if (cArt) {
                cArt->Resolve();
            }
        } else if (cInst->fCollisionArticle) {
            cInst->fCollisionArticle =
                reinterpret_cast<const WCollisionArticle *>(reinterpret_cast<const char *>(cInst->fCollisionArticle) + deltaAddress);
        }
    }
}

const WCollisionInstance *WCollisionPack::Instance(unsigned short index) const {
    return &mInstanceList[index];
}

const WCollisionObject *WCollisionPack::Object(unsigned short index) const {
    return &mObjectList[index];
}
