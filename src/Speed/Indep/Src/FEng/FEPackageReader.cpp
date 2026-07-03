
#include "FEPackageReader.h"
#include "FEChunk.h"
#include "FEPackage.h"
#include "FEObject.h"
#include "FEScript.h"
#include "FEString.h"
#include "FEImage.h"
#include "FEMultiImage.h"
#include "FEMovie.h"
#include "FEListBox.h"
#include "FECodeListBox.h"
#include "FEGroup.h"
#include "FESimpleImage.h"
#include "FEAnimImage.h"
#include "FEColoredImage.h"
#include "FETypes.h"
#include "FEKeyTrack.h"
#include "FEngStandard.h"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEKeyInterpolators.h"
#include "Speed/Indep/Src/FEng/FEPackageChunks.h"
#include "FEMsgTargetList.h"
#include "FEWideString.h"
#include "Speed/Indep/Src/FEng/FEngine.h"
#include "types.h"

// File: speed/indep/src/feng/FEPackageReader.cpp
// total size: 0x8
// Decl: speed/indep/src/feng/FEPackageReader.cpp:36
struct FETypeSize {
    u32 ID;   // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.cpp:37
    u32 Size; // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FEPackageReader.cpp:37
};

FEPackageReader::FEPackageReader() {
    Reset();
}

FEPackageReader::~FEPackageReader() {}

void FEPackageReader::Reset() {
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
    ButtonCount = 0;
}

FEPackage *FEPackageReader::Load(const void *pDataPtr, FEGameInterface *pInt, FEngine *pEng, bool bLoadObjNames, bool bLoadScrNames, bool bLibrary) {
    FEPackage *pResult = nullptr;
    Reset();
    bIsLibrary = bLibrary;
    bLoadObjectNames = bLoadObjNames;
    bLoadScriptNames = bLoadScrNames;
    pChunk = reinterpret_cast<FEChunk *>(const_cast<void *>(pDataPtr));
    pInterface = pInt;
    pEngine = pEng;
    if (!ReadHeaderChunk() || !ReadTypeSizes() || !ReadReferencedPackagesChunk() || !ReadLibraryRefsChunk() || !ReadResourceChunk() ||
        !ReadObjectChunk() || !ReadPackageResponseChunk() || !ReadMessageTargetListChunk()) {
        goto Error;
    }
    pPack->bIsLibrary = bIsLibrary;
    if (pPack->Startup(pInterface)) {
        pResult = pPack;
        pPack = nullptr;
    }
Error:
    if (pPack) {
        delete pPack;
    }
    pPack = nullptr;
    return pResult;
}

FEChunk *FEPackageReader::FindChild(FEChunk *pChunk, u32 ID) {
    FEChunk *pLast = pChunk->GetLastChunk();
    FEChunk *pCur = pChunk->GetFirstChunk();
    while (pCur->GetID() != ID && pCur != pLast) {
        pCur = pCur->GetNext();
    }
    if (pCur->GetID() == ID) {
        return pCur;
    }
    return nullptr;
}

u32 FEPackageReader::GetTypeSize(u32 TypeID) {
    for (u32 i = 0; i < TypeSizeCount; i++) {
        if (FEngGetu32(TypeSizeList[i].ID) == TypeID) {
            return FEngGetu32(TypeSizeList[i].Size);
        }
    }
    return 0;
}

bool FEPackageReader::ReadTypeSizes() {
    FEChunk *pSizesChunk = FindChild(pChunk, Chunk_FETypeSizes);
    if (!pSizesChunk) {
        return true;
    }
    TypeSizeCount = pSizesChunk->GetSize() / sizeof(FETypeSize);
    TypeSizeList = reinterpret_cast<FETypeSize *>(pSizesChunk->GetData());
    return true;
}

bool FEPackageReader::ReadHeaderChunk() {
    if (pChunk->GetID() != Chunk_FEPackage) {
        return false;
    }
    FEChunk *pHeadChunk = pChunk->GetFirstChunk();
    if (pHeadChunk->GetID() != Chunk_FEPackageHeader) {
        return false;
    }
    u32 *pData = reinterpret_cast<u32 *>(pHeadChunk->GetData());
    if (FEngGetu32(pData[0]) < FENG_PACKAGEWRITER_VERSION) {
        return false;
    }
    pPack = FNEW FEPackage();
    pPack->pCurrentButton = nullptr;
    ResourceCount = FEngGetu32(pData[2]);
    ObjectCount = FEngGetu32(pData[3]);
    u32 NameLen = FEngGetu32(pData[4]);
    char *pShortName = reinterpret_cast<char *>(&pData[6]);
    char *pFileName = pShortName + NameLen;
    pPack->SetName(pShortName);
    pPack->SetFilename(pFileName);
    return true;
}

// TODO: regswap
bool FEPackageReader::ReadReferencedPackagesChunk() {
    FEChunk *pRefChunk = FindChild(pChunk, Chunk_FELibraryList);
    if (!pRefChunk) {
        return true;
    }

    u32 *pData = reinterpret_cast<u32 *>(pRefChunk->GetData());
    char *pStrings = reinterpret_cast<char *>(pData);

    u32 NumRefs = FEngGetu32(pData[0]);
    FEList &LibList = pPack->GetLibraryList();
    pData++;
    for (u32 i = 0; i < NumRefs; i++) {
        FENode *pNode = new FENode();
        u32 Offset = FEngGetu32(pData[i]);
        pNode->SetName(pStrings + Offset);
        LibList.AddTail(pNode);
    }
    FENode *pNode = static_cast<FENode *>(LibList.GetHead());
    while (pNode) {
        FEPackage *pLibPack = pEngine->FindLibraryPackage(pNode->GetNameHash());
        if (!pLibPack) {
            bool bDeleteBlock;
            u8 *pBlockStart;
            u8 *pData = pInterface->GetPackageData(pNode->GetName(), &pBlockStart, bDeleteBlock);
            if (!pData) {
                return false;
            }
            pLibPack = pEngine->LoadPackage(pData, true);
            if (bDeleteBlock && pBlockStart) {
                delete[] pBlockStart;
            }
            if (!pLibPack) {
                return false;
            }
            pInterface->PackageWasLoaded(pLibPack);
            pLibPack->SetPriority(1);
            pEngine->AddToLibraryList(pLibPack);
        } else {
            pLibPack->SetPriority(pLibPack->GetPriority() + 1);
        }
        pNode = pNode->GetNext();
    }
    return true;
}

bool FEPackageReader::ReadLibraryRefsChunk() {
    FEChunk *pRefChunk = FindChild(pChunk, Chunk_FELibRefList);
    if (!pRefChunk) {
        return pPack->GetLibraryList().GetNumElements() == 0;
    }
    u32 Count = pRefChunk->GetSize() / sizeof(FELibraryRef);
    if (pRefChunk->GetSize() != Count * sizeof(FELibraryRef)) {
        return false;
    }
    u32 *pData = reinterpret_cast<u32 *>(pRefChunk->GetData());
    pPack->SetNumLibraryRefs(Count);

    FELibraryRef *pRef = pPack->GetLibraryReference(0);
    for (u32 i = 0; i < Count; i++) {
        pRef[i].ObjGUID = FEngGetu32(pData[0]);
        pRef[i].PackNameHash = FEngGetu32(pData[1]);
        pRef[i].LibGUID = FEngGetu32(pData[2]);
        pData += 3;
    }
    return true;
}

bool FEPackageReader::ReadResourceChunk() {
    FEChunk *pResChunk = FindChild(pChunk, Chunk_FEResList);
    if (!pResChunk) {
        return false;
    }
    FEChunk *pNameChunk = pResChunk->GetFirstChunk();
    if (pNameChunk->GetID() != Chunk_FEResNames) {
        return false;
    }
    FEChunk *pResReqChunk = pNameChunk->GetNext();
    if (pResReqChunk->GetID() != Chunk_FEResRequests) {
        return false;
    }
    u32 *pData = reinterpret_cast<u32 *>(pResReqChunk->GetData());
    u32 NumRequests = FEngGetu32(pData[0]);
    pData++;
    pPack->NumRequests = NumRequests;
    if (NumRequests != 0) {
        pPack->pRequests = FNEW FEResourceRequest[NumRequests];
        pPack->pResourceNames = FNEW char[pNameChunk->GetSize()];
        for (u32 Index = 0; Index < NumRequests; Index++) {
            pPack->pRequests[Index].ID = FEngGetu32(pData[0]);
            pPack->pRequests[Index].pFilename = reinterpret_cast<const char *>(FEngGetu32(pData[1]));
            pPack->pRequests[Index].Type = FEngGetu32(pData[2]);
            pPack->pRequests[Index].Flags = FEngGetu32(pData[3]);
            pPack->pRequests[Index].Handle = FEngGetu32(pData[4]);
            pPack->pRequests[Index].UserParam = FEngGetu32(pData[5]);
            pData += 6;
        }
        FEngMemCpy(pPack->pResourceNames, pNameChunk->GetData(), pNameChunk->GetSize());
        for (u32 i = 0; i < NumRequests; i++) {
            pPack->pRequests[i].pFilename = pPack->pRequests[i].pFilename + reinterpret_cast<u32>(pPack->pResourceNames);
        }
    }
    return true;
}

bool FEPackageReader::ReadPackageResponseChunk() {
    FEChunk *pRespList = FindChild(pChunk, Chunk_FEPackageResponseList);
    if (pRespList) {
        ReadMessageResponseTags(reinterpret_cast<FETag *>(pRespList->GetData()), pRespList->GetSize(), true);
    }
    return true;
}

bool FEPackageReader::ReadObjectChunk() {
    FEChunk *pObjList = FindChild(pChunk, Chunk_FEObjectList);
    if (!pObjList) {
        return true;
    }

    FEChunk *pLast = pObjList->GetLastChunk();
    FEChunk *pObjChunk = pObjList->GetFirstChunk();

    if (!pObjChunk || !pLast) {
        return true;
    }

    while (true) {
        if (pObjChunk->GetID() != Chunk_FEObject) {
            if (pObjChunk >= pLast) {
                return true;
            }
            if (pObjChunk->GetID() == Chunk_FEButtonCount) {
                u32 count = FEngGetu32(*reinterpret_cast<u32 *>(pObjChunk->GetData()));
                ButtonCount = count;
                pPack->ButtonMap.SetCount(count);
            }
            pObjChunk = pObjChunk->GetNext();
            continue;
        }
        if (pObjChunk >= pLast) {
            return true;
        }
        do {

            FEChunk *pSubChunk = pObjChunk->GetFirstChunk();
            FEChunk *pLastSub = pObjChunk->GetLastChunk();
            pObj = nullptr;
            pParent = nullptr;

            while (pSubChunk < pLastSub) {
                u32 subID = pSubChunk->GetID();
                switch (subID) {
                    case Chunk_FEObjectData:
                        if (!ReadObjectTags(reinterpret_cast<FETag *>(pSubChunk->GetData()), pSubChunk->GetSize())) {
                            return false;
                        }
                        break;
                    case Chunk_FEScript:
                        ReadScriptTags(reinterpret_cast<FETag *>(pSubChunk->GetData()), pSubChunk->GetSize());
                        break;
                    case Chunk_FEMessageResponseList:
                        ReadMessageResponseTags(reinterpret_cast<FETag *>(pSubChunk->GetData()), pSubChunk->GetSize(), false);
                        break;
                }
                pSubChunk = pSubChunk->GetNext();
            }

            if (pObj) {
                if (pObj->Type == FE_List) {
                    static_cast<FEListBox *>(pObj)->RecalculateCummulative();
                } else if (pObj->Type == FE_CodeList) {
                    static_cast<FECodeListBox *>(pObj)->FillAllCells();
                }

                FEScript *pScript = pObj->GetFirstScript();
                while (pScript) {
                    if (pScript->pChainTo) {
                        pScript->pChainTo = pObj->FindScript(reinterpret_cast<u32>(pScript->pChainTo)); // TODO: how is this not a bug?
                    }
                    pScript = pScript->GetNext();
                }

                FEScript *pDefaultScript = pObj->FindScript(FEHash_Init);
                pObj->SetCurrentScript(pDefaultScript);
                pDefaultScript->CurTime = 0;

                if (!bIsLibrary) {
                    for (u8 i = 0; i < pDefaultScript->TrackCount; i++) {
                        FEKeyInterp(pDefaultScript, i, 0, pObj);
                    }
                }

                if (pParent) {
                    pParent->AddObject(pObj);
                } else {
                    pPack->AddObject(pObj);
                }
            }

            pObjChunk = pObjChunk->GetNext();
        } while (pObjChunk < pLast);
        return true;
    }
}

FEObject *FEPackageReader::CreateObject(u32 ObjectType) {
    FEObject *pObject;
    switch (ObjectType) {
        case FE_String:
            pObject = FNEW FEString();
            break;
        case FE_List:
            pObject = FNEW FEListBox();
            break;
        case FE_CodeList:
            pObject = FNEW FECodeListBox();
            static_cast<FECodeListBox *>(pObject)->SetGameInterface(pInterface);
            break;
        case FE_Group:
            pObject = FNEW FEGroup();
            break;
        case FE_Image:
            pObject = FNEW FEImage();
            break;
        case FE_Movie:
            pObject = FNEW FEMovie();
            break;
        case FE_ColoredImage:
            pObject = FNEW FEColoredImage();
            break;
        case FE_AnimImage:
            pObject = FNEW FEAnimImage();
            break;
        case FE_SimpleImage:
            pObject = FNEW FESimpleImage();
            break;
        case FE_MultiImage:
            pObject = FNEW FEMultiImage();
            break;
        case FE_None:
            return nullptr;
        default:
            pObject = FNEW FEObject();
            break;
    }
    pObject->Type = static_cast<FEObjType>(ObjectType);
    u32 Size = GetTypeSize(ObjectType);
    pObject->SetDataSize(Size);
    return pObject;
}

bool FEPackageReader::ReadObjectTags(FETag *pTag, u32 Length) {
    FETag *pEnd = reinterpret_cast<FETag *>(reinterpret_cast<char *>(pTag) + Length);
    bIsReference = false;

    while (pTag < pEnd) {
        switch (pTag->GetID()) {
            case Tag_FEObjType:
                pObj = CreateObject(pTag->Getu32(0));
                break;
            case Tag_FEObjName:
                if (bLoadObjectNames) {
                    pObj->SetName(reinterpret_cast<const char *>(pTag->Data()));
                }
                break;
            case Tag_FEObjNameHash:
                pObj->SetNameHash(pTag->Getu32(0));
                break;
            case Tag_FEObjProperties:
                pObj->GUID = pTag->Getu32(0);
                pObj->SetNameHash(pTag->Getu32(1));
                pObj->Flags = pTag->Getu32(2);
                pObj->ResourceIndex = static_cast<u16>(pTag->Getu32(3));

                if (pObj->Flags & FF_UsesLibraryObject) {
                    if (!FindReferencedObject(pObj->GUID, &pRefObj, &pRefPack)) {
                        if (pObj) {
                            delete pObj;
                        }
                        pObj = nullptr;
                        return false;
                    }
                    bIsReference = true;
                    pObj->Flags |= pRefObj->Flags;

                    if (pObj->ResourceIndex == 0xFFFF) {
                        pObj->Handle = pRefObj->Handle;
                        pObj->UserParam = pRefObj->UserParam;
                    }

                    FEObject *pCopy;
                    if (pRefObj->Type != FE_Group) {
                        pCopy = pRefObj->Clone(true);
                    } else {
                        pCopy = new FEGroup(static_cast<const FEGroup &>(*pRefObj), false, true);
                    }

                    pCopy->GUID = pObj->GUID;
                    pCopy->NameHash = pObj->NameHash;
                    pCopy->Flags = pObj->Flags;
                    pCopy->ResourceIndex = pObj->ResourceIndex;
                    pCopy->Handle = pObj->Handle;
                    pCopy->UserParam = pObj->UserParam;

                    if (pObj) {
                        delete pObj;
                    }
                    pObj = pCopy;

                    FEScript *pScript = pObj->GetFirstScript();
                    while (pScript) {
                        if (pScript->pChainTo) {
                            pScript->pChainTo = reinterpret_cast<FEScript *>(pScript->pChainTo->ID);
                        }
                        pScript = pScript->GetNext();
                    }
                }

                if (pObj->Flags & FF_IsButton) {
                    pPack->ButtonMap.SetButton(CurButton, pObj);
                    CurButton++;
                }
                break;
            case Tag_FEObjParentGUID:
                if (!pLastParent || pLastParent->GUID != pTag->Getu32(0)) {
                    pLastParent = static_cast<FEGroup *>(pPack->FindObjectByGUID(pTag->Getu32(0)));
                }
                pParent = pLastParent;
                break;
            case Tag_FEObjStaticAnimData: {
                u32 Size = pTag->GetSize();
                for (u32 i = 0; i < Size / sizeof(u32); i++) {
                    reinterpret_cast<u32 *>(pObj->pData)[i] = pTag->Getu32(i);
                }
                break;
            }
            default:
                if (pObj) {
                    switch (pObj->Type) {
                        case FE_String:
                            ProcessStringTag(pTag);
                            break;
                        case FE_List:
                            ProcessListBoxTag(pTag);
                            break;
                        case FE_CodeList:
                            ProcessCodeListBoxTag(pTag);
                            break;
                        case FE_Image:
                        case FE_AnimImage:
                        case FE_ColoredImage:
                            ProcessImageTag(pTag);
                            break;
                        case FE_MultiImage:
                            ProcessImageTag(pTag);
                            ProcessMultiImageTag(pTag);
                            break;
                    }
                }
                break;
        }
        pTag = pTag->Next();
    }
    return true;
}

void FEPackageReader::ProcessStringTag(FETag *pTag) {
    FEString *pString = static_cast<FEString *>(pObj);
    switch (pTag->GetID()) {
        case Tag_FEObjStringBufLength:
            pString->string.SetLength(pTag->Getu32(0));
            break;
        case Tag_FEObjStringText:
            pString->string = reinterpret_cast<i16 *>(pTag->Data());
            {
                i16 *ptr = pString->string.mpsString;
                while (*ptr) {
                    *ptr = FEngGeti16(*ptr);
                    ptr++;
                }
            }
            break;
        case Tag_FEObjStringJustification:
            pString->Format = pTag->Getu32(0);
            break;
        case Tag_FEObjStringLeading:
            pString->Leading = pTag->Getu32(0);
            break;
        case Tag_FEObjStringMaxWidth:
            pString->MaxWidth = pTag->Getu32(0);
            break;
        case Tag_FEObjStringLabelName:
            if (bLoadObjectNames) {
                pString->SetLabel(reinterpret_cast<const char *>(pTag->Data()));
            }
            break;
        case Tag_FEObjStringLabelHash:
            pString->SetLabelHash(pTag->Getu32(0));
            break;
    }
}

void FEPackageReader::ProcessImageTag(FETag *pTag) {
    FEImage *pImage = static_cast<FEImage *>(pObj);
    if (pTag->GetID() != Tag_FEObjImageFlags) {
        return;
    }
    pImage->ImageFlags = pTag->Getu32(0);
}

void FEPackageReader::ProcessMultiImageTag(FETag *pTag) {
    FEMultiImage *pImage = static_cast<FEMultiImage *>(pObj);
    switch (pTag->GetID()) {
        case Tag_FEObjMultiImageTex1:
            pImage->hTexture[0] = pTag->Getu32(0);
            break;
        case Tag_FEObjMultiImageTex2:
            pImage->hTexture[1] = pTag->Getu32(0);
            break;
        case Tag_FEObjMultiImageTex3:
            pImage->hTexture[2] = pTag->Getu32(0);
            break;
        case Tag_FEObjMultiImageFlags1:
            pImage->TextureFlags[0] = pTag->Getu32(0);
            break;
        case Tag_FEObjMultiImageFlags2:
            pImage->TextureFlags[1] = pTag->Getu32(0);
            break;
        case Tag_FEObjMultiImageFlags3:
            pImage->TextureFlags[2] = pTag->Getu32(0);
            break;
    }
}

void FEPackageReader::ProcessListBoxTag(FETag *pTag) {
#ifdef EA_BUILD_A124
    (void)pTag;
    return;
#else
    FEListBox *pList = static_cast<FEListBox *>(pObj);
    switch (pTag->GetID()) {
        case Tag_FEObjListDimesions:
            pList->SetNumColumns(pTag->Getu32(0));
            pList->SetNumRows(pTag->Getu32(1));
            CurListCell = 0xFFFFFFFF;
            CurListRow = 0xFFFFFFFF;
            CurListCol = 0xFFFFFFFF;
            pList->SetCurrentColumn(0);
            pList->SetCurrentRow(0);
            break;
        case Tag_FEObjListAutoWrap:
            pList->SetAutoWrap(pTag->Getu32(0) != 0);
            break;
        case Tag_FEObjListViewDimensions: {
            FEPoint Point;
            Point.h = pTag->Getf32(0);
            Point.v = pTag->Getf32(1);
            pList->SetViewDimensions(Point);
            break;
        }
        case Tag_FEObjListSelectSpeed: {
            FEPoint Point;
            Point.h = pTag->Getf32(0);
            Point.v = pTag->Getf32(1);
            pList->SetSelectionSpeed(Point);
            break;
        }
        case Tag_FEObjListColumnData:
            CurListCol++;
            {
                FEListEntryData *pRowColData = pList->GetPColumnData(CurListCol);
                pRowColData->fValue = pTag->Getf32(0);
                pRowColData->ulJustification = pTag->Getu32(1);
            }
            break;
        case Tag_FEObjListRowData:
            CurListRow++;
            {
                FEListEntryData *pRowColData = pList->GetPRowData(CurListRow);
                pRowColData->fValue = pTag->Getf32(0);
                pRowColData->ulJustification = pTag->Getu32(1);
            }
            break;
        case Tag_FEObjListCellColor: {
            CurListCell++;
            if (CurListCell != 0) {
                pList->IncrementCellByColumn();
            }
            pList->SetCellColor(pTag->Getu32(0));
            break;
        }
        case Tag_FEObjListCellScale: {
            FEPoint Point;
            Point.h = pTag->Getf32(0);
            Point.v = pTag->Getf32(1);
            pList->SetCellScale(Point);
            break;
        }
        case Tag_FEObjListCellResource:
            pList->SetCellResource(0, 0, pTag->Getu32(0));
            break;
        case Tag_FEObjListCellType:
            pList->SetCellType(pTag->Getu32(0));
            break;
        case Tag_FEObjListCellStringText:
            pList->SetCellString(reinterpret_cast<const i16 *>(pTag->Data()));
            break;
        case Tag_FEObjListCellImageRect: {
            FERect Rect;
            Rect.left = pTag->Getf32(0);
            Rect.top = pTag->Getf32(1);
            Rect.right = pTag->Getf32(2);
            Rect.bottom = pTag->Getf32(3);
            pList->SetCellUV(Rect);
            break;
        }
        default:
            break;
    }
#endif
}

void FEPackageReader::ProcessCodeListBoxTag(FETag *pTag) {
#ifdef EA_BUILD_A124
    (void)pTag;
    return;
#else
    FECodeListBox *pList = static_cast<FECodeListBox *>(pObj);
    switch (pTag->GetID()) {
        case Tag_FEObjCodeListDimensions:
            pList->Initialize(pTag->Getu32(0), pTag->Getu32(1));
            break;
        case Tag_FEObjCodeListViewDimensions: {
            FEPoint Point;
            Point.h = pTag->Getf32(0);
            Point.v = pTag->Getf32(1);
            pList->SetViewDimensions(Point);
            break;
        }
        case Tag_FEObjCodeListStringInfo:
            pList->AllocateStrings(pTag->Getu32(0), pTag->Getu32(1));
            break;
        case Tag_FEObjCodeListAutoWrap:
            pList->SetFlags(FECODELISTBOX_PUBLICFLAGS_MASK, false);
            pList->SetFlags(pTag->Getu32(0) & FECODELISTBOX_PUBLICFLAGS_MASK, true);
            break;
        case Tag_FEObjCodeListJustifyFlags:
            pList->SetCellJustification(0, 0, pTag->Getu32(0), pList->GetNumVisColumns(), pList->GetNumVisRows());
            break;
        case Tag_FEObjListCellColor:
            pList->SetCellColor(0, 0, pTag->Getu32(0), pList->GetNumVisColumns(), pList->GetNumVisRows());
            break;
        case Tag_FEObjListCellScale: {
            FEPoint Point;
            Point.h = pTag->Getf32(0);
            Point.v = pTag->Getf32(1);
            pList->SetCellScale(0, 0, Point, pList->GetNumVisColumns(), pList->GetNumVisRows());
            break;
        }
        default:
            break;
    }
#endif
}

bool FEPackageReader::ReadScriptTags(FETag *pTag, u32 Length) {
    FETag *pEnd = reinterpret_cast<FETag *>(reinterpret_cast<unsigned char *>(pTag) + Length);
    u32 CurTrack = static_cast<u32>(-1);
    FEScript *pScript = nullptr;
    FEKeyTrack *pTrack = nullptr;
    int RunningTrackOffset = 0;

    while (pTag < pEnd) {
        switch (pTag->GetID()) {
            case Tag_FEScriptName:
                pScript = new FEScript();
                pScript->CurTime = 0;
                if (bLoadScriptNames) {
                    pScript->SetName(reinterpret_cast<const char *>(pTag->Data()));
                }
                CurTrack = static_cast<u32>(-1);
                RunningTrackOffset = 0;
                break;
            case Tag_FEScriptHeader:
                if (!pScript) {
                    pScript = new FEScript();
                    pScript->CurTime = 0;
                    CurTrack = static_cast<u32>(-1);
                    RunningTrackOffset = 0;
                }
                pScript->ID = pTag->Getu32(0);
                pScript->Length = pTag->Geti32(1);
                pScript->Flags = pTag->Getu32(2);
                pScript->SetTrackCount(pTag->Geti32(3));
                break;
            case Tag_FEScriptChainTo:
                pScript->pChainTo = reinterpret_cast<FEScript *>(pTag->Getu32(0));
                break;
            case Tag_FEScriptID:
                pTrack = nullptr;
                pScript = pObj->FindScript(pTag->Getu32(0));
                break;
            case Tag_FEScriptLength:
                pScript->Length = pTag->Getu32(0);
                break;
            case Tag_FEScriptFlags:
                if (pScript) {
                    pScript->Flags = pTag->Getu32(0);
                }
                break;
            case Tag_FEScriptFieldInfo:
                CurTrack++;
                pTrack = &pScript->pTracks[CurTrack];
                pTrack->ParamType = pTag->Data()[0];
                pTrack->ParamSize = pTag->Data()[1];
                pTrack->InterpType = pTag->Data()[2];
                pTrack->InterpAction = pTag->Data()[3];
                pTrack->Length = pTag->Getu32(1);
                pTrack->LongOffset = RunningTrackOffset;
                RunningTrackOffset += pTrack->ParamSize >> 2;
                break;
            case Tag_FEScriptTrackOffset:
                pTrack->LongOffset = pTag->Getu16(0) >> 8;
                break;
            case Tag_FEScriptTrackIndex: {
                if (pScript) {
                    u16 Index = pTag->Getu16(0);
                    pTrack = pScript->FindTrack(static_cast<FEKeyTrack_Indices>(Index));
                    if (!pTrack) {
                        FEKeyTrack *pNewArray = new FEKeyTrack[pScript->TrackCount + 1];
                        FETypeNode *pTypeNode = pEngine->GetTypeLib().FindType(pObj->Type);
                        FEFieldNode *pField = pTypeNode->GetField(static_cast<int>(Index));
                        u32 SrcIndex = 0;
                        FEKeyTrack *pSrcTrack = pScript->pTracks;
                        for (u32 DestIndex = 0; DestIndex <= pScript->TrackCount; DestIndex++) {
                            if (pSrcTrack && SrcIndex < pScript->TrackCount) {
                                if (pField) {
                                    int srcLongOffset = pSrcTrack[SrcIndex].LongOffset;
                                    int fieldOffset = static_cast<int>(pField->GetOffset());
                                    if (fieldOffset < 0) {
                                        fieldOffset += 3;
                                    }
                                    if (srcLongOffset >= (fieldOffset >> 2)) {
                                        goto insert_track;
                                    }
                                }
                                pNewArray[DestIndex] = pSrcTrack[SrcIndex];
                                SrcIndex++;
                                continue;
                            }
                        insert_track:
                            pNewArray[DestIndex].ParamType = static_cast<u8>(pField->GetType());
                            pNewArray[DestIndex].InterpType = 1;
                            pNewArray[DestIndex].InterpAction = 0;
                            pNewArray[DestIndex].ParamSize = static_cast<u8>(pField->GetSize());
                            pTrack = &pNewArray[DestIndex];
                            pTrack->Length = pScript->Length;
                            pTrack->LongOffset = static_cast<int>(pField->GetOffset() >> 2);
                            pField = nullptr;
                        }
                        delete[] pScript->pTracks;
                        pScript->pTracks = pNewArray;
                        pScript->TrackCount++;
                    }
                }
                break;
            }
            case Tag_FEScriptTrackInterpType:
                if (pScript) {
                    pTrack->InterpType = static_cast<u8>(pTag->Getu16(0) >> 8);
                }
                break;
            case Tag_FEScriptTrackInterpAction:
                if (pScript) {
                    pTrack->InterpAction = static_cast<u8>(pTag->Getu16(0) >> 8);
                }
                break;
            case Tag_FEScriptTrackBaseKey: {
                u32 KeyLongs = pTrack->ParamSize >> 2;
                pTrack->BaseKey.tTime = pTag->Geti32(0);

                for (u32 i = 0; i < KeyLongs; i++) {
                    reinterpret_cast<u32 *>(&pTrack->BaseKey.Val)[i] = pTag->Getu32(i + 1);
                    i++;
                }

                break;
            }
            case Tag_FEScriptFieldKeyData: {
                u32 CurKey = 0;
                u32 KeySize = pTrack->ParamSize + 4;
                u32 NumKeys = pTag->GetSize() / KeySize;
                u8 *pKeyData = pTag->Data();
                FEKeyNode *pKey;
                u32 *pSrc;
                u32 Count = (KeySize / 4) - 1;
                u32 Index;

                if (pTrack->IsReference()) {
                    pTrack->DeltaKeys.ReferenceList(nullptr);
                }

                do {
                    if (CurKey != 0) {
                        pKey = new FEKeyNode();
                    } else {
                        pKey = &pTrack->BaseKey;
                    }
                    pSrc = reinterpret_cast<u32 *>(pKeyData);
                    pKey->tTime = FEngGeti32(*pSrc);
                    Index = 0;
                    if (Count != 0) {
                        do {
                            reinterpret_cast<u32 *>(&pKey->Val)[Index] = FEngGetu32(pSrc[Index + 1]);
                            Index++;
                        } while (Index < Count);
                    }
                    if (CurKey != 0) {
                        pTrack->DeltaKeys.AddTail(pKey);
                    }
                    CurKey++;
                    pKeyData += KeySize;
                } while (CurKey < NumKeys);

                break;
            }
            case Tag_FEScriptEvents: {
                u32 NumEvents = pTag->GetSize() / sizeof(FEEvent);
                pScript->Events.SetCount(static_cast<long>(NumEvents));
                FEEvent *pEvent = &pScript->Events[0];
                u32 *pData = reinterpret_cast<u32 *>(pTag->Data());
                do {
                    NumEvents--;
                    pEvent->EventID = FEngGetu32(pData[0]);
                    pEvent->Target = FEngGetu32(pData[1]);
                    pEvent->tTime = FEngGetu32(pData[2]);
                    pData += 3;
                    pEvent++;
                } while (NumEvents != 0);

                break;
            }
        }
        pTag = pTag->Next();
    }

    if (!bIsReference) {
        pObj->Scripts.AddTail(pScript);
    }
    if (pScript->ID == FEHash_Init) {
        pObj->pCurrentScript = pScript;
    }
    return true;
}

bool FEPackageReader::FindReferencedObject(u32 ObjGUID, FEObject **ppRefObj, FEPackage **ppRefPack) {
    *ppRefObj = nullptr;
    *ppRefPack = nullptr;
    FELibraryRef *pRef = pPack->FindLibraryReference(ObjGUID);
    if (!pRef) {
        return false;
    }
    *ppRefPack = pEngine->FindLibraryPackage(pRef->PackNameHash);
    if (!*ppRefPack) {
        return false;
    }
    *ppRefObj = (*ppRefPack)->FindObjectByGUID(pRef->LibGUID);
    return *ppRefObj != nullptr;
}

bool FEPackageReader::ReadMessageResponseTags(FETag *pTag, u32 Length, bool bPackage) {
    FETag *pEnd = reinterpret_cast<FETag *>(reinterpret_cast<char *>(pTag) + Length);
    FEMessageResponse *pMsgResp = nullptr;
    FEResponse *pResp = nullptr;
    int CurResponse = -1;
    while (pTag < pEnd) {
        switch (pTag->GetID()) {
            case Tag_FEMsgRespMsgID: {
                u32 MsgID = pTag->Getu32(0);
                pMsgResp = nullptr;
                if (!bPackage && bIsReference) {
                    pMsgResp = pObj->FindResponse(MsgID);
                }
                if (!pMsgResp) {
                    pMsgResp = new FEMessageResponse();
                    pMsgResp->SetMsgID(MsgID);
                    if (bPackage) {
                        pPack->AddResponse(pMsgResp);
                    } else {
                        pObj->Responses.AddTail(pMsgResp);
                    }
                } else {
                    pMsgResp->PurgeResponses();
                }
                CurResponse = -1;
                break;
            }
            case Tag_FEMsgRespCount:
                pMsgResp->SetCount(pTag->Getu32(0));
                break;
            case Tag_FEResponseID:
                CurResponse++;
                pResp = pMsgResp->GetResponse(CurResponse);
                pResp->SetID(pTag->Getu32(0));
                break;
            case Tag_FEResponseUINT:
                pResp->ResponseParam = pTag->Getu32(0);
                break;
            case Tag_FEResponseString:
                pResp->SetParam(reinterpret_cast<const char *>(pTag->Data()));
                break;
            case Tag_FEResponseTarget:
                pResp->ResponseTarget = pTag->Getu32(0);
                break;
        }
        pTag = pTag->Next();
    }
    return true;
}

bool FEPackageReader::ReadMessageTargetListChunk() {
    FEChunk *pTargetsChunk = FindChild(pChunk, Chunk_FEMessageTargetList);
    if (pTargetsChunk) {
        FETag *pTag = reinterpret_cast<FETag *>(pTargetsChunk->GetData());
        FETag *pLast = reinterpret_cast<FETag *>(reinterpret_cast<char *>(pTag) + pTargetsChunk->GetSize());
        u32 CurMsgTarg = 0;
        while (pTag < pLast) {
            switch (pTag->GetID()) {
                case Tag_FEMsgTargetCount: {
                    u32 NumMsgs = pTag->Getu32(0);
                    pPack->NumMsgTargets = NumMsgs;
                    pPack->pMsgTargets = FNEW FEMsgTargetList[NumMsgs];
                    break;
                }
                case Tag_FEMsgTarget: {
                    u32 NumTargets = (pTag->GetSize() / 4) - 1;
                    pPack->pMsgTargets[CurMsgTarg].SetMsgID(pTag->Getu32(0));
                    pPack->pMsgTargets[CurMsgTarg].Allocate(NumTargets);
                    u32 *pData = reinterpret_cast<u32 *>(pTag->Data());
                    for (u32 i = 0; i < NumTargets; i++) {
                        FEObject *pTarg = pPack->FindObjectByGUID(FEngGetu32(pData[i]));
                        pPack->pMsgTargets[CurMsgTarg].AppendTarget(pTarg);
                    }
                    CurMsgTarg++;
                    break;
                }
            }
            pTag = pTag->Next();
        }
    }
    return true;
}
