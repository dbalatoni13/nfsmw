#include "Speed/Indep/Src/Frontend/FEObjectCallbacks.hpp"

#include "Speed/Indep/Src/FEng/FEPackage.h"

bool FEngMovieStopper::Callback(FEObject* obj) {
    if (obj == nullptr) {
        return true;
    }
    unsigned int type = obj->Type;
    if (type == 3) {
        FEngSetInvisible(obj);
        return true;
    }
    return true;
}

bool FEngHidePCObjects::Callback(FEObject* obj) {
    if (obj == nullptr) {
        return true;
    }
    unsigned int flags = obj->Flags;
    if ((flags & 0x4000) != 0) {
        FEngSetInvisible(obj);
    }
    return true;
}

bool RenderObjectDisconnect::Callback(FEObject* pObj) {
    if (pObj != nullptr) {
        pObj->pUserData = nullptr;
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
