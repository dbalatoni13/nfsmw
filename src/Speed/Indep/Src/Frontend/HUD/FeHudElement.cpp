#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"

HudElement::HudElement(const char *pkg_name, HudFeaturesType mask)
    : pPackageName(pkg_name), //
      Mask(mask),             //
      CurrentHudFeatures(0),  //
      mCurrentlySetVisible(false) {}

FEString *HudElement::RegisterString(uint32 hash) {
    FEString *str = FEngFindString(pPackageName, hash);

    if (str != nullptr) {
        Objects.AddTail(str);
    }

    return str;
}

FEImage *HudElement::RegisterImage(uint32 hash) {
    FEImage *img = FEngFindImage(pPackageName, hash);

    if (img != nullptr) {
        Objects.AddTail(img);
    }

    return img;
}

FEMultiImage *HudElement::RegisterMultiImage(uint32 hash) {
    FEMultiImage *img = static_cast<FEMultiImage *>(FEngFindObject(pPackageName, hash));

    if (img != nullptr) {
        Objects.AddTail(img);
    }

    return img;
}

FEObject *HudElement::RegisterObject(uint32 hash) {
    FEObject *obj = FEngFindObject(pPackageName, hash);

    if (obj != nullptr) {
        Objects.AddTail(obj);
    }

    return obj;
}

FEGroup *HudElement::RegisterGroup(uint32 hash) {
    FEGroup *grp = static_cast<FEGroup *>(FEngFindGroup(pPackageName, hash));

    if (grp != nullptr) {

        FENode *pChild = reinterpret_cast<FENode *>(grp->GetFirstChild());
        while (pChild != nullptr) {
            FEObject *pChildObj = reinterpret_cast<FEObject *>(pChild);
            if (pChildObj->Type == FE_Group) {
                RegisterGroup(pChild->GetNameHash());
            } else {
                Objects.AddTail(pChildObj);
            }
            pChild = static_cast<FENode *>(pChild->GetNext());
        }
    }

    return grp;
}

void HudElement::Toggle(HudFeaturesType hud_features) {
    CurrentHudFeatures = hud_features;

    bool onoff = (hud_features & Mask) != 0;

    for (bPNode *n = static_cast<bPNode *>(Objects.GetHead()); n != Objects.EndOfList(); n = static_cast<bPNode *>(n->GetNext())) {
        FEObject *obj = static_cast<FEObject *>(n->GetObject());

        if (onoff) {
            FEngSetVisible(obj);
        } else {
            FEngSetInvisible(obj);
        }
    }
}
