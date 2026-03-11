#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEPackage.h"

void FEngSetVisibility(FEObject* obj, bool visible);

bool FEngMovieStopper::Callback(FEObject* obj) {
    if (obj == nullptr) {
        return true;
    }
    if (obj->Type == 3) {
        FEngSetInvisible(obj);
    }
    return true;
}

bool FEngHidePCObjects::Callback(FEObject* obj) {
    if (obj == nullptr) {
        return true;
    }
    if ((obj->Flags & 0x4000) != 0) {
        FEngSetInvisible(obj);
    }
    return true;
}

bool RenderObjectDisconnect::Callback(FEObject* pObj) {
    if (pObj != nullptr) {
        pObj->Cached = nullptr;
    }
    return true;
}

bool ObjectDirtySetter::Callback(FEObject* obj) {
    if (obj != nullptr) {
        obj->Flags |= 0x20;
    }
    return true;
}

bool ObjectVisibilitySetter::Callback(FEObject* obj) {
    FEngSetVisibility(obj, Visible);
    return true;
}
