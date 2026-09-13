#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEGameInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEGameInterface.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngRender.hpp"
#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageManager.hpp"
#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/FEng/FEPackage.h"

extern int g_discErrorOccured;

static FEColor gNormal(0xFFE6E6C8u);
static FEColor gTint(0xFFFFAF41u);
static FEColor gRapsheet(0xFFAAE646u);

cFEngGameInterface *cFEngGameInterface::pInstance = nullptr;
char *FEngPleaseRenderSinglePackage = nullptr;

static char *GetBaseName(char *dest, const char *filename) {
    long x = 0;
    long first = 0;
    long last;

    while (filename[x] != '\0') {
        int c = filename[x];
        x++;
        if (c == '\\' || c == '/') {
            first = x;
        }
    }

    last = x;
    if (x != 0) {
        if (filename[x] != '.') {
            while (--x != 0) {
                if (filename[x] == '.') {
                    last = x;
                    break;
                }
            }
        }
    }

    long y = 0;
    for (x = first; x < last; x++) {
        dest[y] = filename[x];
        y++;
    }
    dest[y] = '\0';
    return dest;
}

cFEngGameInterface::cFEngGameInterface() {
    RenderThisPackage = true;
    iGameMode = 0;
}

cFEngGameInterface::~cFEngGameInterface() {}

// UNSOLVED
bool cFEngGameInterface::LoadResources(FEPackage *pPackage, i32 Count, FEResourceRequest *pList) {
    uint32 length;
    for (int32 i = 0; i < Count; i++) {
        char filename[256];
        GetBaseName(filename, pList[i].pFilename);
        bToUpper(filename);
        switch (pList[i].Type) {
            case 1:
            case 2:
                pList[i].Handle = bStringHash(filename);
                pList[i].UserParam = 0;
                break;
            case 4: {
                void *mem = bMalloc(256, "Movie filename", 0, 0);
                bStrNCpy(static_cast<char *>(mem), filename, 256);
                pList[i].Handle = reinterpret_cast<u32>(mem);
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

bool cFEngGameInterface::UnloadResources(FEPackage *pPackage, i32 Count, FEResourceRequest *pList) {
    for (int i = 0; i < Count; i++) {
        if (pList[i].Type == 4) {
            bFree(reinterpret_cast<void *>(pList[i].Handle));
        }
    }
    return true;
}

static const u32 DISABLE_INPUTS = 0x5922615; // size: 0x4, Decl: 196
static const u32 ENABLE_INPUTS = 0x7e4d1288; // size: 0x4, Decl: 197

void cFEngGameInterface::NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    if (Message != DISABLE_INPUTS && Message != ENABLE_INPUTS) {
        FEPackageManager::Get()->NotificationMessage(Message, pObject, Param1, Param2);
    }
}

void cFEngGameInterface::NotifySoundMessage(u32 Message, FEObject *pObject, u32 ControlMask, u32 pPackagePtr) {
    FEPackageManager::Get()->NotifySoundMessage(Message, pObject, ControlMask, pPackagePtr);
}

void cFEngGameInterface::GenerateRenderContext(u16 uContext, FEObject *pObject) {
    cFEngRender::mInstance->GenerateRenderContext(uContext, pObject);
}

bool cFEngGameInterface::GetContextTransform(u16 uContext, FEMatrix4 &Matrix) {
    Matrix.Identify();
    if (uContext != 0) {
        RenderContext *ctxt = cFEngRender::mInstance->GetRenderContext(uContext);
        if (ctxt != nullptr) {
            Matrix.m11 = ctxt->matrix.v0.x;
            Matrix.m12 = ctxt->matrix.v0.y;
            Matrix.m13 = ctxt->matrix.v0.z;
            Matrix.m14 = ctxt->matrix.v0.w;

            Matrix.m21 = ctxt->matrix.v1.x;
            Matrix.m22 = ctxt->matrix.v1.y;
            Matrix.m23 = ctxt->matrix.v1.z;
            Matrix.m24 = ctxt->matrix.v1.w;

            Matrix.m31 = ctxt->matrix.v2.x;
            Matrix.m32 = ctxt->matrix.v2.y;
            Matrix.m33 = ctxt->matrix.v2.z;
            Matrix.m34 = ctxt->matrix.v2.w;

            Matrix.m41 = ctxt->matrix.v3.x;
            Matrix.m42 = ctxt->matrix.v3.y;
            Matrix.m43 = ctxt->matrix.v3.z;
            Matrix.m44 = ctxt->matrix.v3.w;
        }
    }
    return true;
}

void cFEngGameInterface::RenderObject(FEObject *pObject) {
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

void cFEngGameInterface::GetViewTransformation(FEMatrix4 *pView) {
    pView->Identify();
}

void cFEngGameInterface::BeginPackageRendering(FEPackage *pPackage) {
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

void cFEngGameInterface::EndPackageRendering(FEPackage *pPackage) {
    cFEngRender::mInstance->PackageFinished(pPackage);
}

void cFEngGameInterface::PackageWasLoaded(FEPackage *pPackage) {
    pPackage->InitializePackage();
    pPackage->SetExecute(true);
    if (!pPackage->IsLibrary()) {
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

bool cFEngGameInterface::PackageWillUnload(FEPackage *pPackage) {
    FEngMovieStopper movie_stop;
    pPackage->ForAllObjects(movie_stop);
    RenderObjectDisconnect disconnect;
    disconnect.pFEngRenderer = cFEngRender::mInstance;
    disconnect.PkgRenderInfo = HACK_FEPkgMgr_GetPackageRenderInfo(pPackage);
    pPackage->GetName();
    pPackage->ForAllObjects(disconnect);
    FEPackageManager::Get()->PackageWillBeUnloaded(pPackage);
    pPackage->GetName();
    return true;
}

void HackClearCache(FEPackage *pkg) {
    RenderObjectDisconnect disconnect;
    disconnect.pFEngRenderer = cFEngRender::mInstance;
    disconnect.PkgRenderInfo = HACK_FEPkgMgr_GetPackageRenderInfo(pkg);
    pkg->ForAllObjects(disconnect);
    pkg->GetName();
}

u8 *cFEngGameInterface::GetPackageData(const char *pPackageName, u8 **pBlockStart, bool &bDeleteBlock) {
    bDeleteBlock = false;
    u8 *data = static_cast<u8 *>(FEPackageManager::Get()->GetPackageData(pPackageName));
    return data;
}

u32 cFEngGameInterface::GetJoyPadMask(u8 feng_pad_index) {
    return cFEngJoyInput::mInstance->GetJoyPadMask(feng_pad_index);
}

void cFEngGameInterface::GetMouseInfo(FEMouseInfo &Info) {}

bool cFEngGameInterface::DoesPointTouchObject(float xPos, float yPos, FEObject *pButton) {
    return FEngTestForIntersection(xPos, yPos, pButton);
}

void cFEngGameInterface::OutputWarning(const char *pString, FEng_WarningLevel WarningLevel) {}
