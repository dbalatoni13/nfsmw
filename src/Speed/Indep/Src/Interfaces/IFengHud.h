#ifndef INTERFACES_IFENGHUD_H
#define INTERFACES_IFENGHUD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"

class IHud : public UTL::COM::IUnknown, public UTL::Collections::Listable<IHud, 2> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IHud(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IHud() {}

    virtual void Update(class IPlayer *player, float dT);
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

enum GenericMessage_Priority {
    GenericMessage_Priority_None = 0,
    GenericMessage_Priority_5 = 1,
    GenericMessage_Priority_4 = 2,
    GenericMessage_Priority_3 = 3,
    GenericMessage_Priority_2 = 4,
    GenericMessage_Priority_1 = 5,
};

class IGenericMessage : public UTL::COM::IUnknown {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IGenericMessage(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

  protected:
    virtual ~IGenericMessage() {}

  public:
    virtual bool RequestGenericMessage(const char *string, bool singleFrame, unsigned int fengHash, unsigned int iconTextureHash,
                                       unsigned int iconFengHash, GenericMessage_Priority priority);
    virtual void RequestGenericMessageZoomOut(unsigned int fengHash);
    virtual bool IsGenericMessageShowing();
    virtual GenericMessage_Priority GetCurrentGenericMessagePriority();
};

#endif
