#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEPackage.h"
#include "Speed/Indep/Src/Frontend/MoviePlayer/MoviePlayer.hpp"
#include "Speed/Indep/Src/Frontend/cFEngRender.hpp"


bool FEngMovieStopper::Callback(FEObject* obj) {
    if (obj->Type == 7) {
        if (gMoviePlayer != nullptr) {
            gMoviePlayer->Stop();
        }
        MoviePlayer_ShutDown();
        return false;
    }
    return true;
}

bool FEngHidePCObjects::Callback(FEObject* obj) {
    if (obj->Flags & 0x8) {
        FEngSetInvisible(obj);
        if (obj->Flags & 0x10000000) {
            obj->Flags &= ~0x10000000;
        }
        obj->Flags |= 0x00400000;
    }
    return true;
}

bool RenderObjectDisconnect::Callback(FEObject* pObj) {
    pFEngRenderer->RemoveCachedRender(pObj, PkgRenderInfo);
    return true;
}

bool ObjectDirtySetter::Callback(FEObject* obj) {
    obj->Flags |= 0x00400000;
    cFEngRender::mInstance->RemoveCachedRender(obj, pRenderInfo);
    return true;
}

bool ObjectVisibilitySetter::Callback(FEObject* obj) {
    if (Visible) {
        FEngSetVisible(obj);
    } else {
        FEngSetInvisible(obj);
    }
    return true;
}
