#include "Speed/Indep/Src/World/WCollisionPack.h"

#include "Speed/Indep/Src/Sim/SimSurface.h"

static const unsigned int kWCollisionArticleGroupTag = 0x41727469;
static const unsigned int kWCollisionInstanceTag = 0x63690000;
static const unsigned int kWCollisionObjectTag = 0x636F0000;
static const unsigned int kDefaultCollisionArticleTag = 0x63612020;

WCollisionPack::WCollisionPack(bChunk *chunk)
    : mSectionNumber(0),            //
      mInstanceNum(0),              //
      mInstanceList(nullptr),       //
      mObjectNum(0),                //
      mObjectList(nullptr),         //
      mCarpChunkHeader(reinterpret_cast<bChunkCarpHeader *>(chunk->GetAlignedData(0x10))) {
    Init(chunk);
}

WCollisionPack::~WCollisionPack() {
    DeInit();
}

void WCollisionPack::Init(bChunk *chunk) {
    const void *carpSource;
    int carpSize;
    unsigned int deltaRelocationOffset;
    const UGroup *carpGroup;

    if (mCarpChunkHeader->GetFlags() & 1) {
    } else {
        mCarpChunkHeader->PlatformEndianSwap();
    }

    carpSource = mCarpChunkHeader + 1;
    mSectionNumber = mCarpChunkHeader->GetSectionNumber();
    deltaRelocationOffset = 0;
    carpSize = mCarpChunkHeader->GetCarpSize();

    if (mCarpChunkHeader->GetFlags() & 1) {
        deltaRelocationOffset =
            reinterpret_cast<unsigned int>(mCarpChunkHeader) - reinterpret_cast<unsigned int>(mCarpChunkHeader->GetLastAddress());
    }

    if (mCarpChunkHeader != mCarpChunkHeader->GetLastAddress()) {
        carpGroup = UGroup::Deserialize(1, reinterpret_cast<unsigned int *>(&carpSize), &carpSource, deltaRelocationOffset);
    } else {
        carpGroup = reinterpret_cast<const UGroup *>(carpSource);
    }

    Resolve(carpGroup->GroupLocateTag(kWCollisionArticleGroupTag), 0);
    mCarpChunkHeader->SetLastAddress(mCarpChunkHeader);
    mCarpChunkHeader->SetResolved();

    (void)chunk;
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
        for (int i = 0; i < fNumSurfaces; ++i) {
            unsigned int *surfaceData = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(this) + (fStripsSize + 0x10) + fEdgesSize);
            UCrc32 crc(surfaceData[i]);
            surfaceData[i] = reinterpret_cast<unsigned int>(SimSurface::Lookup(crc));
        }
        fResolvedFlag = true;
    }
}

void WCollisionPack::Resolve(const UGroup *cGroup, unsigned int deltaAddress) {
    unsigned int i = 0;
    const UData *instanceUData = cGroup->DataLocateTag(kWCollisionInstanceTag);
    const UData *objectUData = cGroup->DataLocateTag(kWCollisionObjectTag);

    if (instanceUData != reinterpret_cast<const UData *>(
                             reinterpret_cast<unsigned int>(cGroup->GetArray()) + cGroup->fGroupCount * sizeof(UGroup) +
                             cGroup->fDataCount * sizeof(UData))) {
        mInstanceList = reinterpret_cast<const WCollisionInstance *>(instanceUData->GetDataConst());
        mInstanceNum = instanceUData->DataCount();
    } else {
        mInstanceList = nullptr;
        mInstanceNum = i;
    }

    if (objectUData != reinterpret_cast<const UData *>(
                            reinterpret_cast<unsigned int>(cGroup->GetArray()) + cGroup->fGroupCount * sizeof(UGroup) +
                            cGroup->fDataCount * sizeof(UData))) {
        mObjectList = reinterpret_cast<const WCollisionObject *>(objectUData->GetDataConst());
        mObjectNum = objectUData->DataCount();
    } else {
        mObjectList = nullptr;
        mObjectNum = 0;
    }

    i = 0;
    for (; i < mInstanceNum; ++i) {
        WCollisionInstance *cInst = const_cast<WCollisionInstance *>(&mInstanceList[i]);
        if (deltaAddress == 0) {
            unsigned int articleTag = kDefaultCollisionArticleTag;
            int renderInstanceInd = cInst->fRenderInstanceInd;
            if (renderInstanceInd != -1) {
                articleTag = 0x63610000 | renderInstanceInd;
            }

            const UData *articleUData = cGroup->DataLocateTag(articleTag);
            WCollisionArticle *cArt;
            if (articleUData != reinterpret_cast<const UData *>(
                                    reinterpret_cast<unsigned int>(cGroup->GetArray()) + cGroup->fGroupCount * sizeof(UGroup) +
                                    cGroup->fDataCount * sizeof(UData))) {
                cArt = reinterpret_cast<WCollisionArticle *>(const_cast<void *>(articleUData->GetDataConst()));
            } else {
                cArt = nullptr;
            }

            cInst->fCollisionArticle = cArt;
            if (cArt) {
                cArt->Resolve();
            }
        } else if (cInst->fCollisionArticle != nullptr) {
            cInst->fCollisionArticle =
                reinterpret_cast<const WCollisionArticle *>(reinterpret_cast<unsigned int>(cInst->fCollisionArticle) + deltaAddress);
        }
    }
}

const WCollisionInstance *WCollisionPack::Instance(unsigned short index) const {
    return &mInstanceList[index];
}

const WCollisionObject *WCollisionPack::Object(unsigned short index) const {
    return &mObjectList[index];
}
