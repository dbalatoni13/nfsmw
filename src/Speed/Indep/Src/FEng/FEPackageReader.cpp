#include <new>

#include "FEPackageReader.h"
#include "FEChunk.h"
#include "FEPackage.h"
#include "FEObject.h"
#include "FEScript.h"
#include "FEString.h"
#include "feimage.h"
#include "FEMultiImage.h"
#include "FEMovie.h"
#include "FEListBox.h"
#include "FECodeListBox.h"
#include "FETypes.h"
#include "FEKeyTrack.h"
#include "FEngStandard.h"
#include "fengine.h"

struct FEColoredImage : public FEImage {
};

inline unsigned long BSwap32(unsigned long v) {
    return (v >> 24) | (v << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00);
}

FEPackageReader::FEPackageReader() {
    Reset();
}

FEPackageReader::~FEPackageReader() {
}

void FEPackageReader::Reset() {
    ButtonCount = 0;
    pChunk = nullptr;
    pPack = nullptr;
    pObj = nullptr;
    pParent = nullptr;
    pLastParent = nullptr;
    TypeSizeCount = 0;
    TypeSizeList = nullptr;
    ObjectCount = 0;
    ResourceCount = 0;
    CurButton = 0;
}

FEChunk* FEPackageReader::FindChild(FEChunk* pCh, unsigned long ID) {
    FEChunk* pChild = pCh->GetFirstChunk();
    while (BSwap32(pChild->GetID()) != ID && pChild != pCh->GetLastChunk()) {
        pChild = pChild->GetNext();
    }
    if (BSwap32(pChild->GetID()) == ID) {
        return pChild;
    }
    return nullptr;
}

unsigned long FEPackageReader::GetTypeSize(unsigned long TypeID) {
    unsigned long i = 0;
    if (TypeSizeCount != 0) {
        do {
            if (BSwap32(TypeSizeList[i].ID) == TypeID) {
                return BSwap32(TypeSizeList[i].Size);
            }
            i++;
        } while (i < TypeSizeCount);
    }
    return 0;
}

bool FEPackageReader::ReadTypeSizes() {
    FEChunk* pChild = FindChild(pChunk, 0x53707954);
    if (pChild) {
        TypeSizeList = reinterpret_cast<FETypeSize*>(pChild->GetData());
        TypeSizeCount = BSwap32(pChild->GetSize()) >> 3;
    }
    return true;
}

bool FEPackageReader::ReadHeaderChunk() {
    unsigned long* pData = reinterpret_cast<unsigned long*>(pChunk);
    if (BSwap32(pData[0]) != 0xE76E4546 || BSwap32(pData[2]) != 0x64486B50) {
        return false;
    }
    if (BSwap32(pData[4]) <= 0x1FFFF) {
        return false;
    }
    FEPackage* pNewPack = static_cast<FEPackage*>(FEngMalloc(sizeof(FEPackage), 0, 0));
    new (pNewPack) FEPackage();
    pPack = pNewPack;
    pNewPack->pCurrentButton = nullptr;
    ResourceCount = BSwap32(pData[6]);
    ObjectCount = BSwap32(pData[7]);
    unsigned long nameLen = BSwap32(pData[8]);
    pPack->SetName(reinterpret_cast<const char*>(pData + 10));
    pPack->SetFilename(reinterpret_cast<const char*>(pData + 10) + nameLen);
    return true;
}

bool FEPackageReader::ReadPackageResponseChunk() {
    FEChunk* pChild = FindChild(pChunk, 0x52676B50);
    if (pChild) {
        ReadMessageResponseTags(reinterpret_cast<FETag*>(pChild->GetData()), BSwap32(pChild->GetSize()), true);
    }
    return true;
}

FEPackage* FEPackageReader::Load(const void* pDataPtr, FEGameInterface* pInt, FEngine* pEng,
                                  bool bLoadObjNames, bool bLoadScrNames, bool bLibrary) {
    FEPackage* pResult = nullptr;
    Reset();
    bIsLibrary = bLibrary;
    bLoadObjectNames = bLoadObjNames;
    bLoadScriptNames = bLoadScrNames;
    pChunk = reinterpret_cast<FEChunk*>(const_cast<void*>(pDataPtr));
    pInterface = pInt;
    pEngine = pEng;
    if (ReadHeaderChunk() &&
        ReadTypeSizes() &&
        ReadReferencedPackagesChunk() &&
        ReadLibraryRefsChunk() &&
        ReadResourceChunk() &&
        ReadObjectChunk() &&
        ReadPackageResponseChunk() &&
        ReadMessageTargetListChunk()) {
        pPack->bIsLibrary = bIsLibrary;
        if (pPack->Startup(pInterface)) {
            pResult = pPack;
            pPack = nullptr;
        }
    }
    if (pPack) {
        delete pPack;
        pPack = nullptr;
    }
    return pResult;
}
