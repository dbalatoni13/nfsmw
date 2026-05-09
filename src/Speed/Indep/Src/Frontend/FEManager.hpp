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
    static void InitInput();

    static void Init();

    static void Destroy();

    static FEManager *Get();

    void SetFirstScreen(const char *pPackageName, int arg, unsigned int controlMask) {
        mFirstScreen = pPackageName;
        mFirstScreenArg = arg;
        mFirstScreenMask = controlMask;
    }

    void RequestBootFlow() {
        mFirstBoot = true;
    }

    void SetGarageType(eGarageType pGarageType);

    eGarageType GetGarageType();

    eGarageType GetPreviousGarageType() {
        return mPreviousGarageType;
    }

    const char *GetGarageNameFromType();

    const char *GetGaragePrefixFromType(eGarageType pGarageType);

    ResourceFile *GetGarageBackground() {
        return mGarageBackground;
    }

    void SetGarageBackground(ResourceFile *pBackground) {
        mGarageBackground = pBackground;
    }

    void StartFE();

    void StopFE();

    void Render();

    void Update();

    void UpdateJoyInput();

#ifdef EA_BUILD_A124
    void SetEATraxDelay();

    int GetEATraxDelay();
#endif

    void SetEATraxFirstButton(bool onOff) {
        mEATraxFirstButton = onOff;
    }

    void SetEATraxSecondButton();

    static bool IsOkayToRequestPauseSimulation(int playerIndex, bool useControllerErrors, bool okIfAutoSaveActive);

    static void RequestPauseSimulation(const char *reason);

    static bool ShouldPauseSimulation(bool useControllerErrors);

    static void RequestUnPauseSimulation(const char *reason);

    static inline bool IsPaused() {
        return mInstance->mPauseRequest > 0;
    }

    static int GetNumPauseRequests() {
        return mPauseRequest;
    }

#ifdef EA_BUILD_A124
    // static  const char *GetPauseReason(int idx) {}
#endif

    void WantControllerError(int port);

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

    bool WaitingForControllerError();

    void SuppressControllerError(bool b) {
        bSuppressControllerError = b;
    }

    void AllowControllerError(bool b) {
        bAllowControllerError = b;
    }

    bool IsAllowingControllerError() {
        return bAllowControllerError;
    }

    void ExitOnlineGameplayBasedOnConnection();

    bool IsFirstBoot() {
        return mFirstBoot;
    }

  private:
    FEManager();

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

int GetPortsPlayer(int port);

#endif
