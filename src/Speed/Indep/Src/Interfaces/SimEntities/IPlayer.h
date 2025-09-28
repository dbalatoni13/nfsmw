#ifndef INTERFACES_SIMENTITIES_IPLAYER_H
#define INTERFACES_SIMENTITIES_IPLAYER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Libs/Support/Utility/UListable.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/ISteeringWheel.h"
#include "Speed/Indep/Src/Interfaces/Simables/ISimable.h"

// TODO move?
enum ePlayerList {
    PLAYER_MAX = 3,
    PLAYER_REMOTE = 2,
    PLAYER_LOCAL = 1,
    PLAYER_ALL = 0,
};

enum eControllerConfig {
    CC_CONFIG_1,
    CC_CONFIG_2,
    CC_CONFIG_3,
    CC_CONFIG_4,
    CC_CONFIG_5,
    NUM_CONTROLLER_CONFIGS,
    MIN_CONFIG = 0,
    MAX_CONFIG = 4,
};

struct PlayerSettings {
    bool GaugesOn;
    bool PositionOn;
    bool LapInfoOn;
    bool ScoreOn;
    bool Rumble;
    bool LeaderboardOn;
    bool TransmissionPromptOn;
    bool DriveWithAnalog;
    eControllerConfig Config;
    enum ePlayerSettingsCameras {
        PSC_BUMPER,
        PSC_HOOD,
        PSC_CLOSE,
        PSC_FAR,
        PSC_SUPER_FAR,
        PSC_DRIFT,
        PSC_PURSUIT,
        NUM_CAMERAS_IN_OPTIONS,
        PSC_DEFAULT = 2,
    } CurCam;
    uint8_t SplitTimeType;
    uint8_t Transmission;
    uint8_t Handling;
};

class IPlayer : public UTL::COM::IUnknown, public UTL::Collections::ListableSet<IPlayer, 8, ePlayerList, PLAYER_MAX> {
  public:
    IPlayer(UTL::COM::Object *owner) : UTL::COM::IUnknown(owner, nullptr) {}

    virtual ~IPlayer() {}

    static HINTERFACE _IHandle() {
        return (HINTERFACE)_IHandle;
    }

    virtual ISimable *GetSimable() const;
    virtual bool IsLocal() const;
    virtual const UMath::Vector3 &GetPosition() const;
    virtual bool SetPosition(const UMath::Vector3 &position);
    virtual struct PlayerSettings *GetSettings() const;
    virtual void SetSettings(int fe_index);
    virtual int GetSettingsIndex() const;
    virtual struct IHud *GetHud() const;
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
