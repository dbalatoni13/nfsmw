#ifndef IPLAYER_H
#define IPLAYER_H

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

enum ePlayerHudType {
    PHT_NONE = 0,
    PHT_STANDARD = 1,
    PHT_DRAG = 2,
    PHT_SPLIT1 = 3,
    PHT_SPLIT2 = 4,
    PHT_DRAG_SPLIT1 = 5,
    PHT_DRAG_SPLIT2 = 6,
};

enum ePlayerList {
    PLAYER_ALL = 0,
    PLAYER_LOCAL = 1,
    PLAYER_REMOTE = 2,
    PLAYER_MAX = 3,
};

class IPlayer : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX> {
  public:
    DECL_INTERFACE(IPlayer);

    virtual ISimable *GetSimable() const = 0;

#ifndef EA_BUILD_A124
    virtual bool IsLocal() const = 0;
#endif

    virtual const UMath::Vector3 &GetPosition() const = 0;
    virtual bool SetPosition(const UMath::Vector3 &position) = 0;
    virtual PlayerSettings *GetSettings() const = 0;
    virtual void SetSettings(int fe_index) = 0;
    virtual int GetSettingsIndex() const = 0;
    virtual IHud *GetHud() const = 0;
    virtual void SetHud(ePlayerHudType hudType) = 0;
    virtual void SetRenderPort(int renderport) = 0;
    virtual int GetRenderPort() const = 0;
    virtual void SetControllerPort(int port) = 0;
    virtual int GetControllerPort() const = 0;
    virtual IFeedback *GetFFB() = 0;
    virtual ISteeringWheel *GetSteeringDevice() = 0;
    virtual bool InGameBreaker() const = 0;
    virtual bool CanRechargeNOS() const = 0;
    virtual void ResetGameBreaker(bool full) = 0;
    virtual void ChargeGameBreaker(float amount) = 0;
    virtual bool ToggleGameBreaker() = 0;
};

#endif
