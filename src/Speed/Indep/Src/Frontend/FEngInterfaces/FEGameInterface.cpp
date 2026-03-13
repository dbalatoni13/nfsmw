#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/FEng/FEMath.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

struct RenderContext;
class FEngine;

class cFEngRender {
  public:
    static cFEngRender *mInstance;
    RenderContext *GetRenderContext(unsigned short renderContext);
    void GenerateRenderContext(unsigned short groupContext, FEObject *pObject);
    void PrepForPackage(FEPackage *pPackage);
    void PackageFinished(FEPackage *pPackage);
    void AddToRenderList(FEObject *pObject);
};

extern FEColor gNormal;
extern FEColor gTint;
extern FEColor gRapsheet;
extern int g_discErrorOccured;
extern int FEngPleaseRenderSinglePackage;

void GetBaseName(char *dst, const char *src);
void bToUpper(char *s);
FEPackageRenderInfo *HACK_FEPkgMgr_GetPackageRenderInfo(FEPackage *pkg);
bool FEngTestForIntersection(float x, float y, FEObject *obj);

cFEngGameInterface *cFEngGameInterface::pInstance = nullptr;

cFEngGameInterface::cFEngGameInterface() {
    RenderThisPackage = false;
    iGameMode = 0;
}

cFEngGameInterface::~cFEngGameInterface() {
}

bool cFEngGameInterface::LoadResources(FEPackage *pPackage, int Count, FEResourceRequest *pList) {
    for (int i = 0; i < Count; i++) {
        FEResourceRequest *req = &pList[i];
        char filename[256];
        GetBaseName(filename, req->pFilename);
        bToUpper(filename);
        unsigned long type = req->Type;
        if (type == 1 || type == 2) {
            req->Handle = bStringHash(filename);
            req->UserParam = 0;
        } else if (type == 4) {
            void *mem = bMalloc(256, 0);
            bStrNCpy(static_cast<char *>(mem), filename, 256);
            req->Handle = reinterpret_cast<unsigned long>(mem);
            req->UserParam = 0;
        } else {
            req->Handle = bStringHash(filename);
            req->UserParam = 0;
        }
    }
    return true;
}

bool cFEngGameInterface::UnloadResources(FEPackage *pPackage, int Count, FEResourceRequest *pList) {
    for (int i = 0; i < Count; i++) {
        if (pList[i].Type == 4) {
            bFree(reinterpret_cast<void *>(pList[i].Handle));
        }
    }
    return true;
}

void cFEngGameInterface::NotificationMessage(
    unsigned long Message,
    FEObject *pObject,
    unsigned long Param1,
    unsigned long Param2
) {
    if (Message != 0x5922615 && Message != 0x7E4D1288) {
        FEPackageManager::Get()->NotificationMessage(Message, pObject, Param1, Param2);
    }
}

void cFEngGameInterface::NotifySoundMessage(
    unsigned long Message,
    FEObject *pObject,
    unsigned long ControlMask,
    unsigned long pPackagePtr
) {
    FEPackageManager::Get()->NotifySoundMessage(Message, pObject, ControlMask, pPackagePtr);
}

void cFEngGameInterface::GenerateRenderContext(unsigned short uContext, FEObject *pObject) {
    cFEngRender::mInstance->GenerateRenderContext(uContext, pObject);
}

bool cFEngGameInterface::GetContextTransform(unsigned short uContext, FEMatrix4 &Matrix) {
    Matrix.Identify();
    if (uContext != 0) {
        RenderContext *ctxt = cFEngRender::mInstance->GetRenderContext(uContext);
        if (ctxt) {
            Matrix = *reinterpret_cast<FEMatrix4 *>(ctxt);
        }
    }
    return true;
}

void cFEngGameInterface::RenderObject(FEObject *pObject) {
    bool visible = false;
    if (!(pObject->Flags & 1)) {
        if (RenderThisPackage) {
            visible = true;
        }
    }

    if (pObject->Flags & 0x10) {
        FEObjData *data = pObject->GetObjData();
        if (iGameMode == 0) {
            data->Color = gNormal;
        } else if (iGameMode == 1) {
            data->Color = gTint;
        } else if (iGameMode == 2) {
            data->Color = gRapsheet;
        }
    }

    if (visible) {
        cFEngRender::mInstance->AddToRenderList(pObject);
    }
}

void cFEngGameInterface::GetViewTransformation(FEMatrix4 *pView) {
    pView->Identify();
}

void cFEngGameInterface::BeginPackageRendering(FEPackage *pPackage) {
    RenderThisPackage = true;
    if (g_discErrorOccured && pPackage->GetNameHash() != 0x942C98B5) {
        RenderThisPackage = false;
    }
    if (FEngPleaseRenderSinglePackage) {
        if (FEHashUpper(pPackage->GetName()) != pPackage->GetNameHash()) {
            RenderThisPackage = false;
        }
    }
    if (!FEPackageManager::Get()->GetVisibility(pPackage->GetName())) {
        RenderThisPackage = false;
    }
    cFEngRender::mInstance->PrepForPackage(pPackage);
}

void cFEngGameInterface::EndPackageRendering(FEPackage *pPackage) {
    cFEngRender::mInstance->PackageFinished(pPackage);
}

void cFEngGameInterface::PackageWasLoaded(FEPackage *pPackage) {
    pPackage->InitializePackage();
    pPackage->bExecuting = true;
    if (!pPackage->bIsLibrary) {
        pPackage->Update(cFEng::Get()->mFEng, 0);
    }
    FEPackageManager::Get()->PackageWasLoaded(pPackage);

    {
        FEngMovieStarter movie_starter(pPackage);
        pPackage->ForAllObjects(movie_starter);
    }
    {
        FEngHidePCObjects pcHideObjects;
        pPackage->ForAllObjects(pcHideObjects);
    }
    {
        FEngTransferFlagsToChildren transfer_to_children(4);
        pPackage->ForAllObjects(transfer_to_children);
    }

    if (GRaceStatus::Exists()) {
        iGameMode = 1;
    } else {
        if (FEDatabase && (FEDatabase->GetFEGameMode() & eFE_GAME_MODE_RAP_SHEET)) {
            iGameMode = 2;
        } else {
            iGameMode = 0;
        }
    }
}

bool cFEngGameInterface::PackageWillUnload(FEPackage *pPackage) {
    {
        FEngMovieStopper movie_stop;
        pPackage->ForAllObjects(movie_stop);
    }
    {
        RenderObjectDisconnect disconnect(
            HACK_FEPkgMgr_GetPackageRenderInfo(pPackage),
            cFEngRender::mInstance
        );
        pPackage->ForAllObjects(disconnect);
    }
    FEPackageManager::Get()->PackageWillBeUnloaded(pPackage);
    return true;
}

unsigned char *cFEngGameInterface::GetPackageData(
    const char *pPackageName,
    unsigned char **pBlockStart,
    bool &bDeleteBlock
) {
    bDeleteBlock = false;
    return static_cast<unsigned char *>(FEPackageManager::Get()->GetPackageData(pPackageName));
}

unsigned long cFEngGameInterface::GetJoyPadMask(unsigned char feng_pad_index) {
    return cFEngJoyInput::mInstance->GetJoyPadMask(feng_pad_index);
}

void cFEngGameInterface::GetMouseInfo(FEMouseInfo &Info) {
}

bool cFEngGameInterface::DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) {
    return FEngTestForIntersection(xPos, yPos, pButton);
}

void cFEngGameInterface::OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) {
}
