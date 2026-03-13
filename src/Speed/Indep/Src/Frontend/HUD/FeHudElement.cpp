#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"

#include "Speed/Indep/Src/FEng/FEGroup.h"
#include "Speed/Indep/Src/FEng/FEMultiImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/feimage.h"

FEString *FEngFindString(const char *pkg_name, int name_hash);
FEImage *FEngFindImage(const char *pkg_name, int name_hash);
FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
FEObject *FEngFindGroup(const char *pkg_name, unsigned int obj_hash);
void FEngSetVisible(FEObject *obj);
void FEngSetInvisible(FEObject *obj);

HudElement::HudElement(const char *pkg_name, unsigned long long mask)
    : pPackageName(pkg_name) //
    , Mask(mask) //
    , CurrentHudFeatures(0) //
    , mCurrentlySetVisible(false) {}

HudElement::~HudElement() {
    while (true) {
        bPNode *node = static_cast< bPNode * >(Objects.GetHead());

        if (node == Objects.EndOfList()) {
            break;
        }

        bNode *next_node = node->Next;
        bNode *prev_node = node->Prev;

        prev_node->Next = next_node;
        next_node->Prev = prev_node;

        if (node != nullptr) {
            bPNode::Free(node);
        }
    }
}

void HudElement::Update(IPlayer *player) {}

FEString *HudElement::RegisterString(unsigned int hash) {
    FEString *string = FEngFindString(pPackageName, hash);

    if (string != nullptr) {
        Objects.AddTail(string);
    }

    return string;
}

FEImage *HudElement::RegisterImage(unsigned int hash) {
    FEImage *image = FEngFindImage(pPackageName, hash);

    if (image != nullptr) {
        Objects.AddTail(image);
    }

    return image;
}

FEMultiImage *HudElement::RegisterMultiImage(unsigned int hash) {
    FEMultiImage *image = static_cast< FEMultiImage * >(FEngFindObject(pPackageName, hash));

    if (image != nullptr) {
        Objects.AddTail(image);
    }

    return image;
}

FEObject *HudElement::RegisterObject(unsigned int hash) {
    FEObject *object = FEngFindObject(pPackageName, hash);

    if (object != nullptr) {
        Objects.AddTail(object);
    }

    return object;
}

FEGroup *HudElement::RegisterGroup(unsigned int hash) {
    FEGroup *group = static_cast< FEGroup * >(FEngFindGroup(pPackageName, hash));

    if (group != nullptr) {
        for (FEObject *object = group->GetFirstChild(); object != nullptr; object = object->GetNext()) {
            if (object->Type == FE_Group) {
                RegisterGroup(object->NameHash);
            } else {
                Objects.AddTail(object);
            }
        }
    }

    return group;
}

void HudElement::Toggle(unsigned long long hud_features) {
    CurrentHudFeatures = hud_features;

    for (bPNode *node = static_cast< bPNode * >(Objects.GetHead()); node != Objects.EndOfList();
         node = static_cast< bPNode * >(node->GetNext())) {
        FEObject *object = static_cast< FEObject * >(node->GetpObject());

        if ((hud_features & Mask) == 0) {
            FEngSetInvisible(object);
        } else {
            FEngSetVisible(object);
        }
    }
}
