#ifndef FRONTEND_FEMANAGER_H
#define FRONTEND_FEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

// TODO move?
enum eGarageType {
    GARAGETYPE_NONE = 0,
    GARAGETYPE_MAIN_FE = 1,
    GARAGETYPE_CAREER_SAFEHOUSE = 2,
    GARAGETYPE_CUSTOMIZATION_SHOP = 3,
    GARAGETYPE_CUSTOMIZATION_SHOP_BACKROOM = 4,
    GARAGETYPE_CAR_LOT = 5,
};

// total size: 0x4C
class FEManager {
  public:
    FEManager();

    static void Init();

    static void InitInput();

    static void Destroy();

    static FEManager *Get();

    eGarageType GetGarageType();

    void SetGarageType(eGarageType pGarageType);

    const char *GetGarageNameFromType();

    const char *GetGaragePrefixFromType(eGarageType pGarageType);

    static bool IsOkayToRequestPauseSimulation(int playerIndex, bool useControllerErrors, bool okIfAutoSaveActive);

    static bool ShouldPauseSimulation(bool useControllerErrors);

    static void RequestPauseSimulation(const char *reason);

    static void RequestUnPauseSimulation(const char *reason);

    void WantControllerError(int port);

    bool WaitingForControllerError();

    void StartFE();

    void StopFE();

    void Render();

    void UpdateJoyInput();

    void Update();

    void SetEATraxSecondButton();

    void ExitOnlineGameplayBasedOnConnection();

    void SetFirstScreen(const char *pPackageName, int arg, unsigned int controlMask) {
        mFirstScreen = pPackageName;
        mFirstScreenArg = arg;
        mFirstScreenMask = controlMask;
    }

    void RequestBootFlow() {
        mFirstBoot = true;
    }

    eGarageType GetPreviousGarageType() {
        return mPreviousGarageType;
    }

    ResourceFile *GetGarageBackground() {
        return mGarageBackground;
    }

    void SetGarageBackground(ResourceFile *pBackground) {
        mGarageBackground = pBackground;
    }

    void SetEATraxFirstButton(bool onOff) {
        mEATraxFirstButton = onOff;
    }

    static inline bool IsPaused() {
        return mInstance->mPauseRequest > 0;
    }

    static int GetNumPauseRequests() {
        return mPauseRequest;
    }

    // static  const char *GetPauseReason(int idx) {}

    void ClearControllerError(int port) {
        if (port == -1) {
            return;
        }
        if (port == 4) {
            for (int i = 0; i < 8; i++) {
                bWantControllerError[i] = 0;
            }
        } else {
            bWantControllerError[port] = 0;
        }
    }

    void SuppressControllerError(bool b) {
        bSuppressControllerError = b;
    }

    void AllowControllerError(bool b) {
        bAllowControllerError = b;
    }

    bool IsAllowingControllerError() {
        return bAllowControllerError;
    }

    bool IsFirstBoot() {
        return mFirstBoot;
    }

    //  ~FEManager() {}

  private:
    static FEManager *mInstance;        // size: 0x4
    static int mPauseRequest;           // size: 0x4
    static const char *mPauseReason[8]; // size: 0x20

    bool bSuppressControllerError;   // offset 0x0, size 0x1
    bool bAllowControllerError;      // offset 0x4, size 0x1
    bool bWantControllerError[8];    // offset 0x8, size 0x8
    const char *mFirstScreen;        // offset 0x28, size 0x4
    int mFirstScreenArg;             // offset 0x2C, size 0x4
    unsigned int mFirstScreenMask;   // offset 0x30, size 0x4
    eGarageType mGarageType;         // offset 0x34, size 0x4
    eGarageType mPreviousGarageType; // offset 0x38, size 0x4
    ResourceFile *mGarageBackground; // offset 0x3C, size 0x4
    bool mFirstBoot;                 // offset 0x40, size 0x1
    int mEATraxDelay;                // offset 0x44, size 0x4
    bool mEATraxFirstButton;         // offset 0x48, size 0x1
};

struct RideInfo;

enum eSetRideInfoReasons {
    SET_RIDE_INFO_REASON_VINYL = 0,
    SET_RIDE_INFO_REASON_LOAD_CAR = 1,
    SET_RIDE_INFO_REASON_CATCHALL = 2,
};

enum eCarViewerWhichCar {
    eCARVIEWER_PLAYER1_CAR = 0,
    eCARVIEWER_PLAYER2_CAR = 1,
};

struct GarageMainScreen;

struct CarViewer {
    static GarageMainScreen *FindWhichScreenToUpdate(eCarViewerWhichCar which_car);
    static void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car);
    static void CancelCarLoad(eCarViewerWhichCar which_car);
    static RideInfo *GetRideInfo(eCarViewerWhichCar which_car);
    static void HideAllCars();
    static void ShowAllCars();
    static void ShowCarScreen();
    static void UnshowCarScreen();
    static bool haveLoadedOnce;
};

#endif
