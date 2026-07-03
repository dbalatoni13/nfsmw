#include "Speed/Indep/Src/World/WCollisionPack.h"

#include "Speed/Indep/Src/Sim/SimSurface.h"

WCollisionPack::WCollisionPack(bChunk *chunk)
    : mSectionNumber(0),      //
      mInstanceNum(0),        //
      mInstanceList(nullptr), //
      mObjectNum(0),          //
      mObjectList(nullptr),   //
      mCarpChunkHeader(reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(16))) {
    this->Init(chunk);
}

WCollisionPack::~WCollisionPack() {
    this->DeInit();
}

void WCollisionPack::Init(bChunk *chunk) {
    if (!this->mCarpChunkHeader->IsResolved()) {
        this->mCarpChunkHeader->PlatformEndianSwap();
    }

    this->mSectionNumber = this->mCarpChunkHeader->GetSectionNumber();
    {
        void *crpData = this->mCarpChunkHeader + 1;
        const int carpSourceCount = 1;
        const void *carpSource[1] = {crpData};
        int carpSize[carpSourceCount];
        unsigned int deltaRelocationOffset = 0;
        carpSize[0] = this->mCarpChunkHeader->GetCarpSize();

        if (this->mCarpChunkHeader->IsResolved()) {
            deltaRelocationOffset =
                reinterpret_cast<uintptr_t>(this->mCarpChunkHeader) - reinterpret_cast<uintptr_t>(mCarpChunkHeader->GetLastAddress());
        }

        const UGroup *carpGroup;
        const UGroup *cGroup;
        if (this->mCarpChunkHeader != this->mCarpChunkHeader->GetLastAddress()) {
            carpGroup = UGroup::Deserialize(carpSourceCount, reinterpret_cast<unsigned int *>(carpSize), carpSource, deltaRelocationOffset);
        } else {
            carpGroup = reinterpret_cast<const UGroup *>(carpSource[0]);
        }

        this->Resolve(carpGroup->GroupLocate(MAKE_UDATA_TYPE('Ar'), 'ti'), 0);
        this->mCarpChunkHeader->SetLastAddress(mCarpChunkHeader);
        this->mCarpChunkHeader->SetResolved();
    }
}

void WCollisionPack::DeInit() {
    this->mSectionNumber = 0;
    this->mInstanceList = nullptr;
    this->mObjectList = nullptr;
    this->mInstanceNum = 0;
    this->mObjectNum = 0;
}

void WCollisionArticle::Resolve() {
    if (!this->fResolvedFlag) {
        for (int ind = 0; ind < this->fNumSurfaces; ++ind) {
            char *dataStart = reinterpret_cast<char *>(reinterpret_cast<char *>(&this[1]) + this->fStripsSize + this->fEdgesSize);
            unsigned int hash = reinterpret_cast<unsigned int *>(dataStart)[ind];
            // TODO 64 bit, gotta reallocate and use uintptr_t
            reinterpret_cast<unsigned int *>(dataStart)[ind] = reinterpret_cast<unsigned int>(SimSurface::Lookup(hash));
        }
        this->fResolvedFlag = true;
    }
}

void WCollisionPack::Resolve(const UGroup *cGroup, unsigned int deltaAddress) {
    const UData *instanceUData = cGroup->DataLocate(MAKE_UDATA_TYPE('ci'), 0);
    const UData *objectUData = cGroup->DataLocate(MAKE_UDATA_TYPE('co'), 0);

    if (instanceUData != cGroup->DataEnd()) {
        this->mInstanceList = reinterpret_cast<const WCollisionInstance *>(instanceUData->GetDataConst());
        this->mInstanceNum = instanceUData->DataCount();
    } else {
        this->mInstanceList = nullptr;
        this->mInstanceNum = 0;
    }

    if (objectUData != cGroup->DataEnd()) {
        this->mObjectList = reinterpret_cast<const WCollisionObject *>(objectUData->GetDataConst());
        this->mObjectNum = objectUData->DataCount();
    } else {
        this->mObjectList = nullptr;
        this->mObjectNum = 0;
    }

    for (unsigned int i = 0; i < mInstanceNum; ++i) {
        const WCollisionInstance *cInst = &mInstanceList[i];
        if (deltaAddress == 0) {
            const UData *articleUData = cGroup->DataLocate('ca  ', cInst->fRenderInstanceInd);
            WCollisionArticle *cArt;
            if (articleUData != cGroup->DataEnd()) {
                cArt = reinterpret_cast<WCollisionArticle *>(const_cast<void *>(articleUData->GetDataConst()));
            } else {
                cArt = nullptr;
            }

            cInst->fCollisionArticle = cArt;
#ifndef EA_BUILD_A124
            if (cArt != nullptr) {
                cArt->Resolve();
            }
#endif
        } else if (cInst->fCollisionArticle) {
            cInst->fCollisionArticle =
                reinterpret_cast<const WCollisionArticle *>(reinterpret_cast<const char *>(cInst->fCollisionArticle) + deltaAddress);
        }
    }
}

const WCollisionInstance *WCollisionPack::Instance(unsigned short index) const {
    return &this->mInstanceList[index];
}

const WCollisionObject *WCollisionPack::Object(unsigned short index) const {
    return &this->mObjectList[index];
}
