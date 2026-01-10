#ifndef INTERFACES_SIMENTITIES_IPLAYER_H
#define INTERFACES_SIMENTITIES_IPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// TODO move?
enum ePlayerList {
    PLAYER_ALL = 0,
    PLAYER_LOCAL = 1,
    PLAYER_REMOTE = 2,
    PLAYER_MAX = 3,
};

class IPlayer : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX> {
  public:
    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    IPlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, _IHandle()) {}

    virtual ~IPlayer() {}

    virtual ISimable *GetSimable() const;

#ifndef EA_BUILD_A124
    virtual bool IsLocal() const;
#endif

    virtual const UMath::Vector3 &GetPosition() const;
    virtual bool SetPosition(const UMath::Vector3 &position);
    virtual PlayerSettings *GetSettings() const;
    virtual void SetSettings(int fe_index);
    virtual int GetSettingsIndex() const;
    virtual IHud *GetHud() const;
    virtual void SetHud() const; // TODO fix params
    virtual void SetRenderPort(int renderport);
    virtual int GetRenderPort() const;
    virtual void SetControllerPort(int port);
    virtual int GetControllerPort() const;
    virtual IFeedback *GetFFB();
    virtual ISteeringWheel *GetSteeringDevice();
    virtual bool InGameBreaker() const;
    virtual bool CanRechargeNOS() const;
    virtual void ResetGameBreaker(bool full);
    virtual void ChargeGameBreaker(float amount);
    virtual bool ToggleGameBreaker();
};

#endif
