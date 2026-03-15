#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEResourceRequest.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

bool FEngTestForIntersection(float xPos, float yPos, FEObject* obj);
char* GetBaseName(char* dest, const char* path);
void bToUpper(char* str);
FEPackageRenderInfo* HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage* pkg);

extern int g_discErrorOccured;
extern char* FEngPleaseRenderSinglePackage;

static FEColor gNormal(0xFFE6E6C8u);
static FEColor gTint(0xFFFFAF41u);
static FEColor gRapsheet(0xFFAAE646u);

cFEngGameInterface* cFEngGameInterface::pInstance;

bool FEGameInterface::UnloadUnreferencedLibrary(FEPackage*) {
    return false;
}

void FEGameInterface::RenderObjectList(FEObjectListEntry *pList, unsigned long Count) {
    while (Count) {
        Count--;
        RenderObject(pList[Count].pObject);
    }
}

bool FEGameInterface::SetCellData(FECodeListBox*, unsigned long, unsigned long) {
    return false;
}

cFEngGameInterface::cFEngGameInterface() {
    RenderThisPackage = true;
    iGameMode = 0;
}

cFEngGameInterface::~cFEngGameInterface() {
}

bool cFEngGameInterface::LoadResources(FEPackage* pPackage, long Count, FEResourceRequest* pList) {
    for (int i = 0; i < Count; i++) {
        char filename[256];
        GetBaseName(filename, pList[i].pFilename);
        bToUpper(filename);
        unsigned int length = pList[i].Type;
        switch (length) {
        case 1:
        case 2:
            pList[i].Handle = bStringHash(filename);
            pList[i].UserParam = 0;
            break;
        case 4: {
            void* mem = bMalloc(256, "TODO", __LINE__, 0);
            bStrNCpy(static_cast<char*>(mem), filename, 256);
            pList[i].Handle = reinterpret_cast<unsigned long>(mem);
            pList[i].UserParam = 0;
            break;
        }
        case 3:
        default:
            pList[i].Handle = bStringHash(filename);
            pList[i].UserParam = 0;
            break;
        }
    }
    return true;
}

bool cFEngGameInterface::UnloadResources(FEPackage* pPackage, long Count, FEResourceRequest* pList) {
    for (int i = 0; i < Count; i++) {
        if (pList[i].Type == 4) {
            bFree(reinterpret_cast<void*>(pList[i].Handle));
        }
    }
    return true;
}

void cFEngGameInterface::NotificationMessage(unsigned long Message, FEObject* pObject, unsigned long Param1, unsigned long Param2) {
    if (Message != 0x5922615 && Message != 0x7e4d1288) {
        FEPackageManager::Get()->NotificationMessage(Message, pObject, Param1, Param2);
    }
}

void cFEngGameInterface::NotifySoundMessage(unsigned long Message, FEObject* pObject, unsigned long ControlMask, unsigned long pPackagePtr) {
    FEPackageManager::Get()->NotifySoundMessage(Message, pObject, ControlMask, pPackagePtr);
}

void cFEngGameInterface::GenerateRenderContext(unsigned short uContext, FEObject* pObject) {
    cFEngRender::mInstance->GenerateRenderContext(uContext, pObject);
}

bool cFEngGameInterface::GetContextTransform(unsigned short uContext, FEMatrix4& Matrix) {
    Matrix.Identify();
    if (uContext != 0) {
        RenderContext* ctxt = cFEngRender::mInstance->GetRenderContext(uContext);
        if (ctxt != nullptr) {
            Matrix = *reinterpret_cast<FEMatrix4*>(ctxt);
        }
    }
    return true;
}

void cFEngGameInterface::RenderObject(FEObject* pObject) {
    bool visible = false;
    if (!(pObject->Flags & 1) && RenderThisPackage) {
        visible = true;
    }
    if (pObject->Flags & 0x10) {
        if (iGameMode == 0) {
            pObject->GetObjData()->Col = gNormal;
        } else if (iGameMode == 1) {
            pObject->GetObjData()->Col = gTint;
        } else if (iGameMode == 2) {
            pObject->GetObjData()->Col = gRapsheet;
        }
    }
    if (visible) {
        cFEngRender::mInstance->AddToRenderList(pObject);
    }
}

void cFEngGameInterface::GetViewTransformation(FEMatrix4* pView) {
    pView->Identify();
}

void cFEngGameInterface::BeginPackageRendering(FEPackage* pPackage) {
    RenderThisPackage = true;
    if (g_discErrorOccured != 0 && pPackage->GetNameHash() != 0x942C98B5u) {
        RenderThisPackage = false;
    }
    if (FEngPleaseRenderSinglePackage != nullptr) {
        if (FEHashUpper(FEngPleaseRenderSinglePackage) != pPackage->GetNameHash()) {
            RenderThisPackage = false;
        }
    }
    if (!FEPackageManager::Get()->GetVisibility(pPackage->GetName())) {
        RenderThisPackage = false;
    }
    cFEngRender::mInstance->PrepForPackage(pPackage);
}

void cFEngGameInterface::EndPackageRendering(FEPackage* pPackage) {
    cFEngRender::mInstance->PackageFinished(pPackage);
}

void cFEngGameInterface::PackageWasLoaded(FEPackage* pPackage) {
    pPackage->InitializePackage();
    pPackage->bExecuting = true;
    if (!pPackage->bIsLibrary) {
        pPackage->Update(cFEng::Get()->mFEng, 0);
    }
    FEPackageManager::Get()->PackageWasLoaded(pPackage);
    {
        FEngMovieStarter movie_starter(pPackage);
        pPackage->ForAllObjects(movie_starter);
        FEngHidePCObjects pcHideObjects;
        pPackage->ForAllObjects(pcHideObjects);
        FEngTransferFlagsToChildren transfer_to_children(4);
        pPackage->ForAllObjects(transfer_to_children);
    }
    if (GRaceStatus::Exists()) {
        iGameMode = 1;
    } else {
        if (FEDatabase != nullptr && FEDatabase->IsRapSheetMode()) {
            iGameMode = 2;
        } else {
            iGameMode = 0;
        }
    }
}

bool cFEngGameInterface::PackageWillUnload(FEPackage* pPackage) {
    FEngMovieStopper movie_stop;
    pPackage->ForAllObjects(movie_stop);
    RenderObjectDisconnect disconnect;
    disconnect.pFEngRenderer = cFEngRender::mInstance;
    disconnect.PkgRenderInfo = HACK_FEPkgMgr_GetPackageRenderInfo(pPackage);
    pPackage->ForAllObjects(disconnect);
    FEPackageManager::Get()->PackageWillBeUnloaded(pPackage);
    return true;
}

void HackClearCache(FEPackage* pkg) {
    RenderObjectDisconnect disconnect;
    disconnect.pFEngRenderer = cFEngRender::mInstance;
    disconnect.PkgRenderInfo = HACK_FEPkgMgr_GetPackageRenderInfo(pkg);
    pkg->ForAllObjects(disconnect);
}

unsigned char* cFEngGameInterface::GetPackageData(const char* pPackageName, unsigned char** pBlockStart, bool& bDeleteBlock) {
    bDeleteBlock = false;
    return static_cast<unsigned char*>(FEPackageManager::Get()->GetPackageData(pPackageName));
}

unsigned long cFEngGameInterface::GetJoyPadMask(unsigned char feng_pad_index) {
    return cFEngJoyInput::mInstance->GetJoyPadMask(feng_pad_index);
}

void cFEngGameInterface::GetMouseInfo(FEMouseInfo&) {}

void cFEngGameInterface::OutputWarning(const char*, FEng_WarningLevel) {}

bool cFEngGameInterface::DoesPointTouchObject(float xPos, float yPos, FEObject* pButton) {
    return FEngTestForIntersection(xPos, yPos, pButton);
}
