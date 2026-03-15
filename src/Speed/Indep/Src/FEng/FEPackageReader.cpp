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
#include "FEGroup.h"
#include "FESimpleImage.h"
#include "FEAnimImage.h"
#include "FEColoredImage.h"
#include "FETypes.h"
#include "FEKeyTrack.h"
#include "FEngStandard.h"
#include "fengine.h"
#include "fengine_full.h"
#include "FEMsgTargetList.h"
#include "FEWideString.h"

void FEKeyInterp(FEScript* pScript, unsigned char TrackNum, long tTime, FEObject* pOutObj);

inline unsigned long BSwap32(unsigned long v) {
    return (v >> 24) | (v << 24) | ((v & 0xFF00) << 8) | ((v >> 8) & 0xFF00);
}

inline unsigned short BSwap16(unsigned short v) {
    return static_cast<unsigned short>((v >> 8) | (v << 8));
}

FEPackageReader::FEPackageReader() {
    Reset();
}

FEPackageReader::~FEPackageReader() {
}

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
    for (unsigned long i = 0; i < TypeSizeCount; i++) {
        if (BSwap32(TypeSizeList[i].ID) == TypeID) {
            return BSwap32(TypeSizeList[i].Size);
        }
    }
    return 0;
}

bool FEPackageReader::ReadTypeSizes() {
    FEChunk* pChild = FindChild(pChunk, 0x53707954);
    if (pChild) {
        unsigned long Size = pChild->GetSize();
        TypeSizeList = reinterpret_cast<FETypeSize*>(pChild->GetData());
        TypeSizeCount = BSwap32(Size) >> 3;
    }
    return true;
}

bool FEPackageReader::ReadHeaderChunk() {
    if (BSwap32(pChunk->GetID()) != 0xE76E4546) {
        return false;
    }
    FEChunk* pHeadChunk = pChunk->GetFirstChunk();
    if (BSwap32(pHeadChunk->GetID()) != 0x64486B50) {
        return false;
    }
    unsigned long* pData = reinterpret_cast<unsigned long*>(pHeadChunk->GetData());
    if (BSwap32(pData[0]) <= 0x1FFFF) {
        return false;
    }
    FEPackage* pNewPack = FENG_NEW FEPackage();
    pPack = pNewPack;
    pNewPack->pCurrentButton = nullptr;
    char* pShortName = reinterpret_cast<char*>(pChunk) + 0x28;
    ResourceCount = BSwap32(pData[2]);
    ObjectCount = BSwap32(pData[3]);
    unsigned long NameLen = BSwap32(pData[4]);
    pPack->SetName(pShortName);
    char* pFileName = pShortName + NameLen;
    pPack->SetFilename(pFileName);
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

FEObject* FEPackageReader::CreateObject(unsigned long ObjectType) {
    FEObject* pObject;
    switch (ObjectType) {
    case FE_None:
        return nullptr;
    case FE_Image:
        pObject = FENG_NEW FEImage();
        break;
    case FE_String:
        pObject = FENG_NEW FEString();
        break;
    case FE_List:
        pObject = FENG_NEW FEListBox();
        break;
    case FE_Group:
        pObject = FENG_NEW FEGroup();
        break;
    case FE_CodeList:
        pObject = FENG_NEW FECodeListBox();
        static_cast<FECodeListBox*>(pObject)->mpobRenderer = pInterface;
        break;
    case FE_Movie:
        pObject = FENG_NEW FEMovie();
        break;
    case FE_ColoredImage:
        pObject = FENG_NEW FEColoredImage();
        break;
    case FE_AnimImage:
        pObject = FENG_NEW FEAnimImage();
        break;
    case FE_SimpleImage:
        pObject = FENG_NEW FESimpleImage();
        break;
    case FE_MultiImage:
        pObject = FENG_NEW FEMultiImage();
        break;
    default:
        pObject = FENG_NEW FEObject();
        break;
    }
    pObject->Type = static_cast<FEObjType>(ObjectType);
    unsigned long Size = GetTypeSize(ObjectType);
    pObject->SetDataSize(Size);
    return pObject;
}

void FEPackageReader::ProcessImageTag(FETag* pTag) {
    FEImage* pImage = static_cast<FEImage*>(pObj);
    if (BSwap16(pTag->GetID()) != 0x6649) {
        return;
    }
    pImage->ImageFlags = BSwap32(pTag->Getu32(0));
}

bool FEPackageReader::FindReferencedObject(unsigned long ObjGUID, FEObject** ppRefObj, FEPackage** ppRefPack) {
    *ppRefObj = nullptr;
    *ppRefPack = nullptr;
    FELibraryRef* pRef = pPack->FindLibraryReference(ObjGUID);
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

bool FEPackageReader::ReadReferencedPackagesChunk() {
    FEChunk* pRefChunk = FindChild(pChunk, 0x4C62694C);
    if (pRefChunk) {
        char* pStrings = pRefChunk->GetData();
        unsigned long* pData = reinterpret_cast<unsigned long*>(pStrings);
        unsigned long NumRefs = BSwap32(pData[0]);
        FEList& LibList = pPack->LibrariesUsed;
        unsigned long i = 0;
        if (NumRefs != 0) {
            do {
                FENode* pNode = new FENode();
                unsigned long Offset = BSwap32(pData[1 + i]);
                i++;
                pNode->SetName(pStrings + Offset);
                LibList.AddNode(LibList.GetTail(), pNode);
            } while (i < NumRefs);
        }
        FENode* pLibNode = static_cast<FENode*>(LibList.GetHead());
        while (pLibNode) {
            FEPackage* pLibPack = pEngine->FindLibraryPackage(pLibNode->GetNameHash());
            if (!pLibPack) {
                bool bDeleteBlock;
                unsigned char* pBlockStart;
                unsigned char* pPackData = pInterface->GetPackageData(pLibNode->GetName(), &pBlockStart, bDeleteBlock);
                if (!pPackData) {
                    return false;
                }
                pLibPack = pEngine->LoadPackage(pPackData, true);
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
            pLibNode = pLibNode->GetNext();
        }
    }
    return true;
}

bool FEPackageReader::ReadLibraryRefsChunk() {
    FEChunk* pChild = FindChild(pChunk, 0x5262694C);
    if (!pChild) {
        return pPack->LibrariesUsed.GetNumElements() == 0;
    }
    unsigned long ChunkSize = BSwap32(pChild->GetSize());
    unsigned long Count = ChunkSize / sizeof(FELibraryRef);
    if (ChunkSize != Count * sizeof(FELibraryRef)) {
        return false;
    }
    unsigned long* pData = reinterpret_cast<unsigned long*>(pChild->GetData());
    pPack->SetNumLibraryRefs(Count);
    unsigned long i = 0;
    FELibraryRef* pRef = pPack->pLibRefs;
    for (; i < Count; i++) {
        pRef[i].ObjGUID = BSwap32(pData[0]);
        pRef[i].PackNameHash = BSwap32(pData[1]);
        pRef[i].LibGUID = BSwap32(pData[2]);
        pData += 3;
    }
    return true;
}

bool FEPackageReader::ReadResourceChunk() {
    FEChunk* pChild = FindChild(pChunk, 0xcc736552);
    if (!pChild) {
        return false;
    }
    FEChunk* pNameChunk = pChild->GetFirstChunk();
    if (BSwap32(pNameChunk->GetID()) != 0x6d4e7352) {
        return false;
    }
    FEChunk* pResReqChunk = pNameChunk->GetNext();
    if (BSwap32(pResReqChunk->GetID()) != 0x71527352) {
        return false;
    }
    unsigned long* pData = reinterpret_cast<unsigned long*>(pResReqChunk->GetData()) + 1;
    unsigned long NumRequests = BSwap32(*reinterpret_cast<unsigned long*>(pResReqChunk->GetData()));
    pPack->NumRequests = NumRequests;
    if (NumRequests != 0) {
        pPack->pRequests = static_cast<FEResourceRequest*>(FEngMalloc(NumRequests * sizeof(FEResourceRequest), nullptr, 0));
        pPack->pResourceNames = static_cast<char*>(FEngMalloc(BSwap32(pNameChunk->GetSize()), nullptr, 0));
        char* nameData = pNameChunk->GetData();
        for (unsigned long Index = 0; Index < NumRequests; Index++) {
            pPack->pRequests[Index].ID = BSwap32(pData[0]);
            pPack->pRequests[Index].pFilename = reinterpret_cast<const char*>(BSwap32(pData[1]));
            pPack->pRequests[Index].Type = BSwap32(pData[2]);
            pPack->pRequests[Index].Flags = BSwap32(pData[3]);
            pPack->pRequests[Index].Handle = BSwap32(pData[4]);
            pPack->pRequests[Index].UserParam = BSwap32(pData[5]);
            pData += 6;
        }
        FEngMemCpy(pPack->pResourceNames, nameData, BSwap32(pNameChunk->GetSize()));
        for (unsigned long i = 0; i < NumRequests; i++) {
            pPack->pRequests[i].pFilename = pPack->pRequests[i].pFilename + reinterpret_cast<unsigned long>(pPack->pResourceNames);
        }
    }
    return true;
}

void FEPackageReader::ProcessMultiImageTag(FETag* pTag) {
    FEMultiImage* pImage = static_cast<FEMultiImage*>(pObj);
    unsigned short tagID = BSwap16(pTag->GetID());
    switch (tagID) {
        case 0x314d:
            pImage->hTexture[0] = BSwap32(pTag->Getu32(0));
            break;
        case 0x324d:
            pImage->hTexture[1] = BSwap32(pTag->Getu32(0));
            break;
        case 0x334d:
            pImage->hTexture[2] = BSwap32(pTag->Getu32(0));
            break;
        case 0x614d:
            pImage->TextureFlags[0] = BSwap32(pTag->Getu32(0));
            break;
        case 0x624d:
            pImage->TextureFlags[1] = BSwap32(pTag->Getu32(0));
            break;
        case 0x634d:
            pImage->TextureFlags[2] = BSwap32(pTag->Getu32(0));
            break;
    }
}

void FEPackageReader::ProcessStringTag(FETag* pTag) {
    FEString* pString = static_cast<FEString*>(pObj);
    unsigned short tagID = BSwap16(pTag->GetID());
    switch (tagID) {
        case 0x6253:
            pString->string.SetLength(BSwap32(pTag->Getu32(0)));
            break;
        case 0x7453:
            pString->string = reinterpret_cast<short*>(pTag->Data());
            {
                short* ptr = pString->string.mpsString;
                while (*ptr) {
                    short s = *ptr;
                    *ptr = static_cast<short>(((s >> 8) & 0xFF) | (s << 8));
                    ptr++;
                }
            }
            break;
        case 0x6a53:
            pString->Format = BSwap32(pTag->Getu32(0));
            break;
        case 0x6c53:
            pString->Leading = BSwap32(pTag->Getu32(0));
            break;
        case 0x7753:
            pString->MaxWidth = BSwap32(pTag->Getu32(0));
            break;
        case 0x4c53:
            if (bLoadObjectNames) {
                pString->SetLabel(reinterpret_cast<const char*>(pTag->Data()));
            }
            break;
        case 0x4853:
            pString->SetLabelHash(BSwap32(pTag->Getu32(0)));
            break;
    }
}

void FEPackageReader::ProcessCodeListBoxTag(FETag* pTag) {
    FECodeListBox* pList = static_cast<FECodeListBox*>(pObj);
    unsigned short tagID = BSwap16(pTag->GetID());
    switch (tagID) {
        case 0x444c:
            pList->Initialize(BSwap32(pTag->Getu32(0)), BSwap32(pTag->Getu32(1)));
            break;
        case 0x764c: {
            FEPoint pt;
            *reinterpret_cast<unsigned long*>(&pt.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&pt.v) = BSwap32(pTag->Getu32(1));
            pList->mstViewDimensions.h = pt.h;
            pList->mstViewDimensions.v = pt.v;
            break;
        }
        case 0x4953:
            pList->AllocateStrings(BSwap32(pTag->Getu32(0)), BSwap32(pTag->Getu32(1)));
            break;
        case 0x744c:
            pList->mulFlags &= 1;
            pList->mulFlags |= BSwap32(pTag->Getu32(0)) & 0xFFFFFFFE;
            break;
        case 0x6a4c:
            pList->SetCellJustification(0, 0, BSwap32(pTag->Getu32(0)), pList->mulNumVisibleColumns, pList->mulNumVisibleRows);
            break;
        case 0x6343:
            pList->SetCellColor(0, 0, BSwap32(pTag->Getu32(0)), pList->mulNumVisibleColumns, pList->mulNumVisibleRows);
            break;
        case 0x7343: {
            FEPoint scale;
            *reinterpret_cast<unsigned long*>(&scale.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&scale.v) = BSwap32(pTag->Getu32(1));
            pList->SetCellScale(0, 0, scale, pList->mulNumVisibleColumns, pList->mulNumVisibleRows);
            break;
        }
    }
}

bool FEPackageReader::ReadMessageResponseTags(FETag* pTag, unsigned long Length, bool bPackage) {
    FETag* pEnd = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + Length);
    FEMessageResponse* pMsgResp = nullptr;
    FEResponse* pResp = nullptr;
    int CurResponse = -1;
    while (pTag < pEnd) {
        unsigned short tagID = BSwap16(pTag->GetID());
        switch (tagID) {
            case 0x694d: {
                unsigned long MsgID = BSwap32(pTag->Getu32(0));
                pMsgResp = nullptr;
                if (!bPackage && bIsReference) {
                    pMsgResp = pObj->FindResponse(MsgID);
                }
                if (!pMsgResp) {
                    pMsgResp = new FEMessageResponse();
                    pMsgResp->SetMsgID(MsgID);
                    if (bPackage) {
                        pPack->Responses.AddNode(pPack->Responses.GetTail(), pMsgResp);
                    } else {
                        pObj->Responses.AddNode(pObj->Responses.GetTail(), pMsgResp);
                    }
                } else {
                    pMsgResp->PurgeResponses();
                }
                CurResponse = -1;
                break;
            }
            case 0x434d:
                pMsgResp->SetCount(BSwap32(pTag->Getu32(0)));
                break;
            case 0x6952:
                CurResponse++;
                pResp = pMsgResp->GetResponse(CurResponse);
                pResp->SetID(BSwap32(pTag->Getu32(0)));
                break;
            case 0x7552:
                pResp->ResponseTarget = BSwap32(pTag->Getu32(0));
                break;
            case 0x7352:
                pResp->SetParam(reinterpret_cast<const char*>(pTag->Data()));
                break;
            case 0x7452:
                pResp->ResponseParam = BSwap32(pTag->Getu32(0));
                break;
        }
        pTag = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + 4 + BSwap16(pTag->GetSize()));
    }
    return true;
}

bool FEPackageReader::ReadMessageTargetListChunk() {
    FEChunk* pChild = FindChild(pChunk, 0x67726154);
    if (pChild) {
        FETag* pTag = reinterpret_cast<FETag*>(pChild->GetData());
        FETag* pEnd = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + BSwap32(pChild->GetSize()));
        int idx = 0;
        while (pTag < pEnd) {
            unsigned short tagID = BSwap16(pTag->GetID());
            switch (tagID) {
            case 0x6354: {
                unsigned long NumTargets = BSwap32(pTag->Getu32(0));
                pPack->NumMsgTargets = NumTargets;
                unsigned long* pMem = static_cast<unsigned long*>(FEngMalloc(NumTargets * 0x10 + 0x10, nullptr, 0));
                FEMsgTargetList* pEntries = reinterpret_cast<FEMsgTargetList*>(pMem + 4);
                *pMem = NumTargets;
                if (NumTargets != 0) {
                    FEMsgTargetList* pCur = pEntries;
                    do {
                        pCur->MsgID = 0;
                        pCur->Alloc = 0;
                        pCur->Count = 0;
                        pCur->pTargets = nullptr;
                        pCur++;
                        NumTargets--;
                    } while (NumTargets != 0);
                }
                pPack->pMsgTargets = pEntries;
                break;
            }
            case 0x744d: {
                FEMsgTargetList* pCurTarget = &pPack->pMsgTargets[idx];
                pCurTarget->MsgID = BSwap32(pTag->Getu32(0));
                unsigned long NumObjs = (BSwap16(pTag->GetSize()) >> 2) - 1;
                pCurTarget->Allocate(NumObjs);
                idx++;
                unsigned long i = 0;
                if (NumObjs != 0) {
                    do {
                        FEObject* pTarget = pPack->FindObjectByGUID(BSwap32(pTag->Getu32(1 + i)));
                        pCurTarget->AppendTarget(pTarget);
                        i++;
                    } while (i < NumObjs);
                }
                break;
            }
            }
            pTag = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + BSwap16(pTag->GetSize()) + 4);
        }
    }
    return true;
}

void FEPackageReader::ProcessListBoxTag(FETag* pTag) {
    FEListBox* pList = static_cast<FEListBox*>(pObj);
    FEListEntryData* pRowColData;
    unsigned long val;
    int idx;
    unsigned short tagID = BSwap16(pTag->GetID());
    switch (tagID) {
        case 0x644c:
            pList->SetNumColumns(BSwap32(pTag->Getu32(0)));
            pList->SetNumRows(BSwap32(pTag->Getu32(1)));
            CurListCell = 0xFFFFFFFF;
            CurListRow = 0xFFFFFFFF;
            CurListCol = 0xFFFFFFFF;
            {
                unsigned long col = 0;
                if (pList->mulNumColumns == 0) {
                    col = 0;
                } else if (col >= pList->mulNumColumns) {
                    col = pList->mulNumColumns - 1;
                }
                pList->mulCurrentColumn = col;
            }
            {
                unsigned long row = 0;
                if (pList->mulNumRows == 0) {
                    row = 0;
                } else if (row >= pList->mulNumRows) {
                    row = pList->mulNumRows - 1;
                }
                pList->mulCurrentRow = row;
            }
            return;
        case 0x774c:
            pList->SetAutoWrap(BSwap32(pTag->Getu32(0)) != 0);
            return;
        case 0x764c: {
            FEPoint pt;
            *reinterpret_cast<unsigned long*>(&pt.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&pt.v) = BSwap32(pTag->Getu32(1));
            pList->mstViewDimensions = pt;
            return;
        }
        case 0x734c: {
            FEPoint pt;
            *reinterpret_cast<unsigned long*>(&pt.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&pt.v) = BSwap32(pTag->Getu32(1));
            pList->mstSelectionSpeed = pt;
            return;
        }
        case 0x634c:
            CurListCol++;
            val = pTag->Getu32(0);
            pRowColData = pList->GetPColumnData(CurListCol);
            break;
        case 0x724c:
            CurListRow++;
            val = pTag->Getu32(0);
            pRowColData = pList->GetPRowData(CurListRow);
            break;
        case 0x6343: {
            CurListCell++;
            if (CurListCell != 0) {
                pList->IncrementCellByColumn();
            }
            FEColor color(BSwap32(pTag->Getu32(0)));
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            pCell->ulColor = static_cast<unsigned long>(color);
            return;
        }
        case 0x7343: {
            FEPoint pt;
            *reinterpret_cast<unsigned long*>(&pt.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&pt.v) = BSwap32(pTag->Getu32(1));
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            pCell->stScale = pt;
            return;
        }
        case 0x7243: {
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            pCell->stResource.ResourceIndex = BSwap32(pTag->Getu32(0));
            pCell->stResource.Handle = 0;
            pCell->stResource.UserParam = 0;
            return;
        }
        case 0x5443:
            pList->SetCellType(BSwap32(pTag->Getu32(0)));
            return;
        case 0x7443:
            pList->SetCellString(reinterpret_cast<const short*>(pTag->Data()));
            return;
        case 0x6943: {
            unsigned long c0 = BSwap32(pTag->Getu32(0));
            unsigned long c1 = BSwap32(pTag->Getu32(1));
            unsigned long c2 = BSwap32(pTag->Getu32(2));
            unsigned long c3 = BSwap32(pTag->Getu32(3));
            FERect rect;
            *reinterpret_cast<unsigned long*>(&rect.left) = c0;
            *reinterpret_cast<unsigned long*>(&rect.top) = c1;
            *reinterpret_cast<unsigned long*>(&rect.right) = c2;
            *reinterpret_cast<unsigned long*>(&rect.bottom) = c3;
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            pCell->SetUV() = rect;
            return;
        }
        default:
            return;
    }
    // Shared code for Lc (0x634c) and Lr (0x724c)
    *reinterpret_cast<unsigned long*>(&pRowColData->fValue) = BSwap32(val);
    pRowColData->ulJustification = BSwap32(pTag->Getu32(1));
}

bool FEPackageReader::ReadObjectChunk() {
    FEChunk* pObjList = FindChild(pChunk, 0xcc6a624f);
    if (!pObjList) {
        return true;
    }

    FEChunk* pLast = pObjList->GetLastChunk();
    FEChunk* pObjChunk = pObjList->GetFirstChunk();

    if (!pObjChunk || !pLast) {
        return true;
    }

    while (true) {
        unsigned long chunkID = BSwap32(pObjChunk->GetID());
        if (chunkID != 0xea624f46) {
            if (pObjChunk >= pLast) {
                return true;
            }
            if (chunkID == 0x6e747542) {
                unsigned long count = BSwap32(*reinterpret_cast<unsigned long*>(pObjChunk->GetData()));
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

            pObj = nullptr;
            pParent = nullptr;

            FEChunk* pLastSub = pObjChunk->GetLastChunk();
            FEChunk* pSubChunk = pObjChunk->GetFirstChunk();

            while (pSubChunk < pLastSub) {
                unsigned long subID = BSwap32(pSubChunk->GetID());
                switch (subID) {
                    case 0x446a624f:
                        if (!ReadObjectTags(reinterpret_cast<FETag*>(pSubChunk->GetData()), BSwap32(pSubChunk->GetSize()))) {
                            return false;
                        }
                        break;
                    case 0x5267734d:
                        ReadMessageResponseTags(reinterpret_cast<FETag*>(pSubChunk->GetData()), BSwap32(pSubChunk->GetSize()), false);
                        break;
                    case 0x70726353:
                        ReadScriptTags(reinterpret_cast<FETag*>(pSubChunk->GetData()), BSwap32(pSubChunk->GetSize()));
                        break;
                }
                pSubChunk = pSubChunk->GetNext();
            }

            if (pObj) {
                if (pObj->Type == FE_List) {
                    static_cast<FEListBox*>(pObj)->RecalculateCummulative();
                } else if (pObj->Type == FE_CodeList) {
                    static_cast<FECodeListBox*>(pObj)->FillAllCells();
                }

                FEScript* pScript = pObj->GetFirstScript();
                while (pScript) {
                    if (pScript->pChainTo) {
                        pScript->pChainTo = pObj->FindScript(reinterpret_cast<unsigned long>(pScript->pChainTo));
                    }
                    pScript = pScript->GetNext();
                }

                FEScript* pDefaultScript = pObj->FindScript(0x1744b3);
                pObj->SetCurrentScript(pDefaultScript);
                pDefaultScript->CurTime = 0;

                if (!bIsLibrary) {
                    unsigned char i = 0;
                    if (pDefaultScript->TrackCount != 0) {
                        do {
                            FEKeyInterp(pDefaultScript, i, 0, pObj);
                            i++;
                        } while (i < pDefaultScript->TrackCount);
                    }
                }

                if (pParent) {
                    static_cast<FEGroup*>(pParent)->AddObject(pObj);
                } else {
                    pPack->AddObject(pObj);
                }
            }

            pObjChunk = pObjChunk->GetNext();
        } while (pObjChunk < pLast);
        return true;
    }
}

bool FEPackageReader::ReadObjectTags(FETag* pTag, unsigned long Length) {
    FETag* pEnd = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + Length);
    bIsReference = false;

    while (pTag < pEnd) {
        unsigned short tagID = BSwap16(pTag->GetID());
        switch (tagID) {
            case 0x744f:
                pObj = CreateObject(BSwap32(pTag->Getu32(0)));
                break;
            case 0x6e4f:
                if (bLoadObjectNames) {
                    pObj->SetName(reinterpret_cast<const char*>(pTag->Data()));
                }
                break;
            case 0x684f:
                pObj->NameHash = BSwap32(pTag->Getu32(0));
                break;
            case 0x504f: {
                pObj->GUID = BSwap32(pTag->Getu32(0));
                pObj->NameHash = BSwap32(pTag->Getu32(1));
                pObj->Flags = BSwap32(pTag->Getu32(2));
                pObj->ResourceIndex = static_cast<unsigned short>(BSwap32(pTag->Getu32(3)));

                if (pObj->Flags & 0x100000) {
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

                    FEObject* pClone;
                    if (pRefObj->Type != FE_Group) {
                        pClone = pRefObj->Clone(true);
                    } else {
                        pClone = new FEGroup(static_cast<const FEGroup&>(*pRefObj), false, true);
                    }

                    pClone->GUID = pObj->GUID;
                    pClone->NameHash = pObj->NameHash;
                    pClone->Flags = pObj->Flags;
                    pClone->ResourceIndex = pObj->ResourceIndex;
                    pClone->Handle = pObj->Handle;
                    pClone->UserParam = pObj->UserParam;

                    if (pObj) {
                        delete pObj;
                    }
                    pObj = pClone;

                    FEScript* pScript = pObj->GetFirstScript();
                    while (pScript) {
                        if (pScript->pChainTo) {
                            pScript->pChainTo = reinterpret_cast<FEScript*>(pScript->pChainTo->ID);
                        }
                        pScript = pScript->GetNext();
                    }
                }

                if (pObj->Flags & 0x10000000) {
                    pPack->ButtonMap.pList[CurButton] = pObj;
                    CurButton++;
                }
                break;
            }
            case 0x4150: {
                unsigned long parentGUID = BSwap32(pTag->Getu32(0));
                if (!pLastParent || pLastParent->GUID != parentGUID) {
                    pLastParent = static_cast<FEGroup*>(pPack->FindObjectByGUID(parentGUID));
                }
                pParent = pLastParent;
                break;
            }
            case 0x4153: {
                unsigned long Size = BSwap16(pTag->GetSize());
                unsigned long count = Size >> 2;
                if (count != 0) {
                    unsigned long i = 0;
                    do {
                        reinterpret_cast<unsigned long*>(pObj->pData)[i] = BSwap32(pTag->Getu32(i));
                        i++;
                    } while (i < count);
                }
                break;
            }
            default:
                if (pObj) {
                    switch (pObj->Type) {
                        case FE_Image:
                        case FE_ColoredImage:
                        case FE_AnimImage:
                            ProcessImageTag(pTag);
                            break;
                        case FE_String:
                            ProcessStringTag(pTag);
                            break;
                        case FE_List:
                            ProcessListBoxTag(pTag);
                            break;
                        case FE_CodeList:
                            ProcessCodeListBoxTag(pTag);
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

bool FEPackageReader::ReadScriptTags(FETag* pTag, unsigned long Length) {
    FETag* pEnd = reinterpret_cast<FETag*>(reinterpret_cast<unsigned char*>(pTag) + Length);
    unsigned long CurTrack = static_cast<unsigned long>(-1);
    FEScript* pScript = nullptr;
    FEKeyTrack* pTrack = nullptr;
    int RunningTrackOffset = 0;

    while (pTag < pEnd) {
        unsigned short tagID = BSwap16(pTag->GetID());
        switch (tagID) {
            case 0x6e53: {
                pScript = new FEScript();
                pScript->Init();
                pScript->CurTime = 0;
                if (bLoadScriptNames) {
                    pScript->SetName(reinterpret_cast<const char*>(pTag->Data()));
                }
                CurTrack = static_cast<unsigned long>(-1);
                RunningTrackOffset = 0;
                break;
            }
            case 0x6853: {
                if (!pScript) {
                    CurTrack = static_cast<unsigned long>(-1);
                    pScript = new FEScript();
                    RunningTrackOffset = 0;
                    pScript->Init();
                    pScript->CurTime = 0;
                }
                pScript->ID = BSwap32(pTag->Getu32(0));
                pScript->Length = static_cast<int>(BSwap32(pTag->Getu32(1)));
                pScript->Flags = BSwap32(pTag->Getu32(2));
                pScript->SetTrackCount(static_cast<long>(BSwap32(pTag->Getu32(3))));
                break;
            }
            case 0x6353: {
                pScript->pChainTo = reinterpret_cast<FEScript*>(BSwap32(pTag->Getu32(0)));
                break;
            }
            case 0x4953: {
                pTrack = nullptr;
                pScript = pObj->FindScript(BSwap32(pTag->Getu32(0)));
                break;
            }
            case 0x6c53: {
                pScript->Length = static_cast<int>(BSwap32(pTag->Getu32(0)));
                break;
            }
            case 0x6653: {
                if (pScript) {
                    pScript->Flags = BSwap32(pTag->Getu32(0));
                }
                break;
            }
            case 0x4946: {
                CurTrack++;
                pTrack = &pScript->pTracks[CurTrack];
                pTrack->ParamType = pTag->Data()[0];
                unsigned char paramSize = pTag->Data()[1];
                pTrack->ParamSize = paramSize;
                pTrack->InterpType = pTag->Data()[2];
                pTrack->InterpAction = pTag->Data()[3];
                pTrack->Length = static_cast<int>(BSwap32(pTag->Getu32(1)));
                pTrack->LongOffset = RunningTrackOffset;
                RunningTrackOffset += paramSize >> 2;
                break;
            }
            case 0x6f54: {
                pTrack->LongOffset = pTag->Getu16(0) >> 8;
                break;
            }
            case 0x6954: {
                if (pScript) {
                    unsigned short Index = BSwap16(pTag->Getu16(0));
                    pTrack = pScript->FindTrack(static_cast<FEKeyTrack_Indices>(Index));
                    if (!pTrack) {
                        unsigned long trackCount = pScript->TrackCount;
                        FEKeyTrack* pNewArray = new FEKeyTrack[trackCount + 1];
                        FETypeNode* pTypeNode = pEngine->GetTypeLib().FindType(pObj->NameHash);
                        FEFieldNode* pField = pTypeNode->GetField(static_cast<int>(Index));
                        unsigned long SrcIndex = 0;
                        FEKeyTrack* pSrcTrack = pScript->pTracks;
                        {
                            unsigned long DestIndex = 0;
                            do {
                                if (!pSrcTrack || pScript->TrackCount <= SrcIndex) {
                                insert_track:
                                    pNewArray[DestIndex].ParamType = static_cast<unsigned char>(pField->GetType());
                                    pTrack = &pNewArray[DestIndex];
                                    pTrack->InterpType = 1;
                                    pTrack->ParamSize = static_cast<unsigned char>(pField->GetSize());
                                    pTrack->InterpAction = 0;
                                    pTrack->Length = pScript->Length;
                                    pTrack->LongOffset = static_cast<int>(pField->GetOffset() >> 2);
                                    pField = nullptr;
                                } else {
                                    if (pField) {
                                        int fieldOffset = static_cast<int>(pField->GetOffset());
                                        if (fieldOffset < 0) {
                                            fieldOffset += 3;
                                        }
                                        if ((fieldOffset >> 2) <= pSrcTrack[SrcIndex].LongOffset) {
                                            goto insert_track;
                                        }
                                    }
                                    pNewArray[DestIndex] = pSrcTrack[SrcIndex];
                                    SrcIndex++;
                                }
                                DestIndex++;
                            } while (DestIndex <= pScript->TrackCount);
                        }
                        delete[] pScript->pTracks;
                        pScript->pTracks = pNewArray;
                        pScript->TrackCount++;
                    }
                }
                break;
            }
            case 0x7454: {
                if (pScript) {
                    pTrack->InterpType = pTag->Data()[0];
                }
                break;
            }
            case 0x6154: {
                if (pScript) {
                    pTrack->InterpAction = pTag->Data()[0];
                }
                break;
            }
            case 0x6254: {
                {
                    unsigned long KeyLongs = pTrack->ParamSize >> 2;
                    pTrack->BaseKey.tTime = static_cast<int>(BSwap32(pTag->Getu32(0)));
                    {
                        unsigned long i = 0;
                        if (KeyLongs != 0) {
                            do {
                                reinterpret_cast<unsigned long*>(&pTrack->BaseKey.Val)[i] = BSwap32(pTag->Getu32(i + 1));
                                i++;
                            } while (i < KeyLongs);
                        }
                    }
                }
                break;
            }
            case 0x644b: {
                {
                    unsigned long CurKey = 0;
                    unsigned long KeySize = pTrack->ParamSize + 4;
                    unsigned long NumKeys = BSwap16(pTag->GetSize()) / KeySize;
                    unsigned char* pKeyData = pTag->Data();
                    FEKeyNode* pKey;
                    unsigned long* pSrc;
                    unsigned long Count;
                    unsigned long Index;

                    if (pTrack->IsReference()) {
                        pTrack->DeltaKeys.ReferenceList(nullptr);
                    }

                    do {
                        if (CurKey == 0) {
                            pKey = &pTrack->BaseKey;
                        } else {
                            pKey = new FEKeyNode();
                        }
                        pSrc = reinterpret_cast<unsigned long*>(pKeyData);
                        Index = 0;
                        pKey->tTime = static_cast<int>(BSwap32(*pSrc));
                        Count = (KeySize >> 2) - 1;
                        if (Count != 0) {
                            do {
                                reinterpret_cast<unsigned long*>(&pKey->Val)[Index] = BSwap32(pSrc[Index + 1]);
                                Index++;
                            } while (Index < Count);
                        }
                        if (CurKey != 0) {
                            pTrack->DeltaKeys.AddTail(pKey);
                        }
                        CurKey++;
                        pKeyData += KeySize;
                    } while (CurKey < NumKeys);
                }
                break;
            }
            case 0x5645: {
                {
                    unsigned long NumEvents = BSwap16(pTag->GetSize()) / sizeof(FEEvent);
                    pScript->Events.SetCount(static_cast<long>(NumEvents));
                    FEEvent* pEvent = &pScript->Events[0];
                    unsigned long* pData = reinterpret_cast<unsigned long*>(pTag->Data());
                    do {
                        NumEvents--;
                        pEvent->EventID = BSwap32(*pData);
                        pEvent->Target = BSwap32(pData[1]);
                        pEvent->tTime = BSwap32(pData[2]);
                        pData += 3;
                        pEvent++;
                    } while (NumEvents != 0);
                }
                break;
            }
        }
        pTag = reinterpret_cast<FETag*>(reinterpret_cast<unsigned char*>(pTag) + BSwap16(pTag->GetSize()) + 4);
    }

    if (!bIsReference) {
        pObj->Scripts.AddTail(pScript);
    }
    if (pScript->ID == 0x1744b3) {
        pObj->pCurrentScript = pScript;
    }
    return true;
}
