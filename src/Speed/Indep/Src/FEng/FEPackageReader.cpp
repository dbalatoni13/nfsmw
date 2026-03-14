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

FEObject* FEPackageReader::CreateObject(unsigned long ObjectType) {
    FEObject* pObject;
    if (ObjectType == FE_CodeList) {
        pObject = static_cast<FEObject*>(FEngMalloc(sizeof(FECodeListBox), 0, 0));
        new (static_cast<FECodeListBox*>(pObject)) FECodeListBox();
        static_cast<FECodeListBox*>(pObject)->mpobRenderer = pInterface;
    } else if (ObjectType < FE_CodeList) {
        if (ObjectType == FE_String) {
            pObject = static_cast<FEObject*>(FEngMalloc(0x78, 0, 0));
            new (pObject) FEObject();
            pObject->pData = nullptr;
            static_cast<FEString*>(pObject)->SetLabelHash(0xFFFFFFFF);
            new (&static_cast<FEString*>(pObject)->string) FEWideString();
            static_cast<FEString*>(pObject)->MaxWidth = 0;
            static_cast<FEString*>(pObject)->Leading = 0;
            static_cast<FEString*>(pObject)->Format = 0;
            pObject->Type = static_cast<FEObjType>(ObjectType);
        } else if (ObjectType < FE_String) {
            if (ObjectType == FE_None) {
                return nullptr;
            }
            if (ObjectType == FE_Image) {
                pObject = static_cast<FEObject*>(FEngMalloc(0x60, 0, 0));
                new (pObject) FEObject();
                pObject->pData = nullptr;
                pObject->Type = static_cast<FEObjType>(ObjectType);
            } else {
                goto make_default;
            }
        } else {
            if (ObjectType == FE_List) {
                pObject = static_cast<FEObject*>(FEngMalloc(sizeof(FEListBox), 0, 0));
                new (static_cast<FEListBox*>(pObject)) FEListBox();
            } else if (ObjectType == FE_Group) {
                pObject = static_cast<FEObject*>(FEngMalloc(sizeof(FEGroup), 0, 0));
                new (pObject) FEObject();
                pObject->pData = nullptr;
                pObject->Type = static_cast<FEObjType>(ObjectType);
            } else {
                goto make_default;
            }
        }
    } else {
        if (ObjectType == FE_AnimImage) {
            pObject = static_cast<FEObject*>(FEngMalloc(0x60, 0, 0));
            new (pObject) FEObject();
            pObject->pData = nullptr;
            pObject->Type = static_cast<FEObjType>(ObjectType);
        } else if (ObjectType > FE_AnimImage) {
            if (ObjectType == FE_SimpleImage) {
                pObject = static_cast<FEObject*>(FEngMalloc(0x5C, 0, 0));
                new (pObject) FEObject();
                pObject->Type = static_cast<FEObjType>(ObjectType);
            } else if (ObjectType == FE_MultiImage) {
                pObject = static_cast<FEObject*>(FEngMalloc(0x78, 0, 0));
                new (pObject) FEObject();
                pObject->pData = nullptr;
                pObject->Type = static_cast<FEObjType>(ObjectType);
            } else {
                goto make_default;
            }
        } else {
            if (ObjectType == FE_Movie) {
                pObject = static_cast<FEObject*>(FEngMalloc(0x60, 0, 0));
                new (pObject) FEObject();
                pObject->pData = nullptr;
                pObject->Type = static_cast<FEObjType>(ObjectType);
            } else if (ObjectType == FE_ColoredImage) {
                pObject = static_cast<FEObject*>(FEngMalloc(0x60, 0, 0));
                new (pObject) FEObject();
                pObject->pData = nullptr;
                pObject->Type = static_cast<FEObjType>(ObjectType);
            } else {
                goto make_default;
            }
        }
    }
    goto set_data;
make_default:
    pObject = static_cast<FEObject*>(FEngMalloc(sizeof(FEObject), 0, 0));
    new (pObject) FEObject();
set_data:
    pObject->Type = static_cast<FEObjType>(ObjectType);
    pObject->SetDataSize(GetTypeSize(ObjectType));
    return pObject;
}

void FEPackageReader::ProcessImageTag(FETag* pTag) {
    if (BSwap16(pTag->GetID()) != 0x6649) {
        return;
    }
    static_cast<FEImage*>(pObj)->ImageFlags = BSwap32(pTag->Getu32(0));
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
    if (Count != 0) {
        do {
            unsigned long idx = i * 3;
            i++;
            pRef[i - 1].ObjGUID = BSwap32(pData[idx]);
            pRef[i - 1].PackNameHash = BSwap32(pData[idx + 1]);
            pRef[i - 1].LibGUID = BSwap32(pData[idx + 2]);
        } while (i < Count);
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
    FEChunk* pResReqChunk = reinterpret_cast<FEChunk*>(reinterpret_cast<char*>(pNameChunk) + 8 + BSwap32(pNameChunk->GetSize()));
    if (BSwap32(pResReqChunk->GetID()) != 0x71527352) {
        return false;
    }
    unsigned long* pData = reinterpret_cast<unsigned long*>(pResReqChunk->GetData() + 8);
    unsigned long NumRequests = BSwap32(*(reinterpret_cast<unsigned long*>(pResReqChunk->GetData()) + 2));
    pPack->NumRequests = NumRequests;
    if (NumRequests != 0) {
        pPack->pRequests = static_cast<FEResourceRequest*>(FEngMalloc(NumRequests * sizeof(FEResourceRequest), nullptr, 0));
        pPack->pResourceNames = static_cast<char*>(FEngMalloc(BSwap32(pNameChunk->GetSize()), nullptr, 0));
        unsigned long i = 0;
        if (NumRequests != 0) {
            do {
                unsigned long offset = i * 6;
                i++;
                FEResourceRequest* pReq = &pPack->pRequests[i - 1];
                pReq->ID = BSwap32(pData[offset]);
                pReq->pFilename = reinterpret_cast<const char*>(BSwap32(pData[offset + 1]));
                pReq->Type = BSwap32(pData[offset + 2]);
                pReq->Flags = BSwap32(pData[offset + 3]);
                pReq->Handle = BSwap32(pData[offset + 4]);
                pReq->UserParam = BSwap32(pData[offset + 5]);
            } while (i < NumRequests);
        }
        FEngMemCpy(pPack->pResourceNames, reinterpret_cast<char*>(pNameChunk) + 8, BSwap32(pNameChunk->GetSize()));
        i = 0;
        if (NumRequests != 0) {
            do {
                FEResourceRequest* pReq = &pPack->pRequests[i];
                i++;
                pReq->pFilename = pReq->pFilename + reinterpret_cast<unsigned long>(pPack->pResourceNames);
            } while (i < NumRequests);
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
                    *ptr = BSwap16(*ptr);
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
        case 0x4953:
            pList->AllocateStrings(BSwap32(pTag->Getu32(0)), BSwap32(pTag->Getu32(1)));
            break;
        case 0x6343:
            pList->SetCellColor(0, 0, BSwap32(pTag->Getu32(0)), pList->mulNumVisibleColumns, pList->mulNumVisibleRows);
            break;
        case 0x6a4c:
            pList->SetCellJustification(0, 0, BSwap32(pTag->Getu32(0)), pList->mulNumVisibleColumns, pList->mulNumVisibleRows);
            break;
        case 0x7343: {
            FEPoint scale;
            unsigned long sh = BSwap32(pTag->Getu32(0));
            unsigned long sv = BSwap32(pTag->Getu32(1));
            scale.h = *reinterpret_cast<float*>(&sh);
            scale.v = *reinterpret_cast<float*>(&sv);
            pList->SetCellScale(0, 0, scale, pList->mulNumVisibleColumns, pList->mulNumVisibleRows);
            break;
        }
        case 0x744c:
            pList->mulFlags = (pList->mulFlags & 1) | (BSwap32(pTag->Getu32(0)) & 0xFFFFFFFE);
            break;
        case 0x764c: {
            unsigned long vh = BSwap32(pTag->Getu32(0));
            unsigned long vv = BSwap32(pTag->Getu32(1));
            pList->mstViewDimensions.h = *reinterpret_cast<float*>(&vh);
            pList->mstViewDimensions.v = *reinterpret_cast<float*>(&vv);
            break;
        }
    }
}

bool FEPackageReader::ReadMessageResponseTags(FETag* pTag, unsigned long Length, bool bPackage) {
    FETag* pEnd = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + Length);
    FEMessageResponse* pMsgResp = nullptr;
    int CurResponse = -1;
    FEResponse* pResp = nullptr;
    while (pTag < pEnd) {
        unsigned short tagID = BSwap16(pTag->GetID());
        switch (tagID) {
            case 0x694d: {
                unsigned long MsgID = BSwap32(pTag->Getu32(0));
                pMsgResp = nullptr;
                if (!bPackage && pObj) {
                    pMsgResp = pObj->FindResponse(MsgID);
                }
                if (!pMsgResp) {
                    pMsgResp = new FEMessageResponse();
                    pMsgResp->SetMsgID(MsgID);
                    if (!bPackage) {
                        pObj->Responses.AddNode(pObj->Responses.GetTail(), pMsgResp);
                    } else {
                        pPack->Responses.AddNode(pPack->Responses.GetTail(), pMsgResp);
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
            case 0x7352:
                pResp->SetParam(reinterpret_cast<const char*>(pTag->Data()));
                break;
            case 0x7452:
                pResp->ResponseTarget = BSwap32(pTag->Getu32(0));
                break;
            case 0x7552:
                pResp->ResponseParam = BSwap32(pTag->Getu32(0));
                break;
        }
        pTag = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + BSwap16(pTag->GetSize()) + 4);
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
            if (tagID == 0x6354) {
                unsigned long NumTargets = BSwap32(pTag->Getu32(0));
                pPack->NumMsgTargets = NumTargets;
                unsigned long* pMem = static_cast<unsigned long*>(FEngMalloc(NumTargets * 0x10 + 0x10, nullptr, 0));
                FEMsgTargetList* pEntries = reinterpret_cast<FEMsgTargetList*>(pMem + 4);
                *pMem = NumTargets;
                unsigned long i = NumTargets;
                if (i != 0) {
                    FEMsgTargetList* pCur = pEntries;
                    do {
                        i--;
                        pCur->MsgID = 0;
                        pCur->Alloc = 0;
                        pCur->Count = 0;
                        pCur->pTargets = nullptr;
                        pCur++;
                    } while (i != 0);
                }
                pPack->pMsgTargets = pEntries;
            } else if (tagID == 0x744d) {
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
            }
            pTag = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + BSwap16(pTag->GetSize()) + 4);
        }
    }
    return true;
}

void FEPackageReader::ProcessListBoxTag(FETag* pTag) {
    FEListBox* pList = static_cast<FEListBox*>(pObj);
    FEListEntryData* pRowColData;
    unsigned short tagID = BSwap16(pTag->GetID());
    switch (tagID) {
        case 0x5443:
            pList->SetCellType(BSwap32(pTag->Getu32(0)));
            return;
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
        case 0x634c:
            CurListCol++;
            pRowColData = pList->GetPColumnData(CurListCol);
            break;
        case 0x644c:
            pList->SetNumColumns(BSwap32(pTag->Getu32(0)));
            pList->SetNumRows(BSwap32(pTag->Getu32(1)));
            CurListCol = 0xFFFFFFFF;
            CurListCell = 0xFFFFFFFF;
            CurListRow = 0xFFFFFFFF;
            if (pList->mulNumColumns == 0) {
                pList->mulCurrentColumn = 0;
            } else {
                pList->mulCurrentColumn = 0;
                if (pList->mulNumColumns == 0) {
                    pList->mulCurrentColumn = 0xFFFFFFFF;
                }
            }
            if (pList->mulNumRows == 0) {
                pList->mulCurrentRow = 0;
            } else {
                pList->mulCurrentRow = 0;
                if (pList->mulNumRows == 0) {
                    pList->mulCurrentRow = 0xFFFFFFFF;
                }
            }
            return;
        case 0x6943: {
            unsigned long c0 = BSwap32(pTag->Getu32(0));
            unsigned long c1 = BSwap32(pTag->Getu32(1));
            unsigned long c2 = BSwap32(pTag->Getu32(2));
            unsigned long c3 = BSwap32(pTag->Getu32(3));
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            reinterpret_cast<unsigned long*>(&pCell->u)[0] = c0;
            reinterpret_cast<unsigned long*>(&pCell->u)[1] = c1;
            reinterpret_cast<unsigned long*>(&pCell->u)[2] = c2;
            reinterpret_cast<unsigned long*>(&pCell->u)[3] = c3;
            return;
        }
        case 0x7243: {
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            pCell->stResource.ResourceIndex = BSwap32(pTag->Getu32(0));
            pCell->stResource.UserParam = 0;
            pCell->stResource.Handle = 0;
            return;
        }
        case 0x724c:
            CurListRow++;
            pRowColData = pList->GetPRowData(CurListRow);
            break;
        case 0x7343: {
            FEListBoxCell* pCell = pList->GetPCellData(pList->mulCurrentColumn, pList->mulCurrentRow);
            *reinterpret_cast<unsigned long*>(&pCell->stScale.h) = pTag->Getu32(0);
            *reinterpret_cast<unsigned long*>(&pCell->stScale.v) = pTag->Getu32(1);
            return;
        }
        case 0x734c:
            *reinterpret_cast<unsigned long*>(&pList->mstSelectionSpeed.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&pList->mstSelectionSpeed.v) = BSwap32(pTag->Getu32(1));
            return;
        case 0x764c:
            *reinterpret_cast<unsigned long*>(&pList->mstViewDimensions.h) = BSwap32(pTag->Getu32(0));
            *reinterpret_cast<unsigned long*>(&pList->mstViewDimensions.v) = BSwap32(pTag->Getu32(1));
            return;
        case 0x7443:
            pList->SetCellString(reinterpret_cast<const short*>(pTag->Data()));
            return;
        case 0x774c:
            pList->SetAutoWrap(pTag->Getu32(0) != 0);
            return;
        default:
            return;
    }
    // Shared code for Lc (0x634c) and Lr (0x724c)
    *reinterpret_cast<unsigned long*>(&pRowColData->fValue) = pTag->Getu32(0);
    pRowColData->ulJustification = pTag->Getu32(1);
}

bool FEPackageReader::ReadObjectChunk() {
    FEChunk* pObjList = FindChild(pChunk, 0xcc6a624f);
    if (!pObjList) {
        return true;
    }

    FEChunk* pLast = pObjList->GetLastChunk();
    FEChunk* pObjChunk = pObjList->GetFirstChunk();

    if (!pObjChunk || pLast == reinterpret_cast<FEChunk*>(-8)) {
        return true;
    }

    while (true) {
        unsigned long chunkID = BSwap32(pObjChunk->GetID());
        if (chunkID == 0xea624f46) {
            do {
                if (pLast <= pObjChunk) {
                    return true;
                }

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
            } while (true);
        }
        pObjChunk = pObjChunk->GetNext();
        if (pLast <= pObjChunk) {
            return true;
        }
    }
}

bool FEPackageReader::ReadObjectTags(FETag* pTag, unsigned long Length) {
    FETag* pEnd = reinterpret_cast<FETag*>(reinterpret_cast<char*>(pTag) + Length);
    bIsReference = false;

    while (pTag < pEnd) {
        unsigned short tagID = BSwap16(pTag->GetID());
        switch (tagID) {
            case 0x504f: {
                pObj->GUID = BSwap32(pTag->Getu32(0));
                pObj->NameHash = BSwap32(pTag->Getu32(1));
                pObj->Flags = BSwap32(pTag->Getu32(2));
                pObj->ResourceIndex = BSwap16(pTag->Getu16(6));

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

                    if (static_cast<short>(pObj->ResourceIndex) == -1) {
                        pObj->Handle = pRefObj->Handle;
                        pObj->UserParam = pRefObj->UserParam;
                    }

                    FEObject* pClone;
                    if (pRefObj->Type == FE_Group) {
                        pClone = new FEGroup(static_cast<const FEGroup&>(*pRefObj), false, true);
                    } else {
                        pClone = pRefObj->Clone(true);
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
            case 0x6e4f:
                if (bLoadObjectNames) {
                    pObj->SetName(reinterpret_cast<const char*>(pTag->Data()));
                }
                break;
            case 0x684f:
                pObj->NameHash = pTag->Getu32(0);
                break;
            case 0x744f:
                pObj = CreateObject(BSwap32(pTag->Getu32(0)));
                break;
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
