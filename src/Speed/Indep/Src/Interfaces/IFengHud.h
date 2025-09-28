#ifndef INTERFACES_IFENGHUD_H
#define INTERFACES_IFENGHUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"

class IHud : public UTL::COM::IUnknown, public UTL::Collections::Listable<IHud, 2> {
  public:
    IHud(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IHud() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual void Update(IPlayer *player, float dT);
    virtual void Release();
    virtual void HideAll();
    virtual bool AreResourcesLoaded();
    virtual bool IsHudVisible();
    virtual void SetInPursuit(bool inPursuit);
    virtual void SetHasTurbo(bool hasTurbo);
    virtual void JoyEnable();
    virtual void JoyDisable();
    virtual void RefreshMiniMapItems();
};

#endif
