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

    //  void SetFirstScreen(const char *pPackageName, int arg, unsigned int controlMask) {}

    //  void RequestBootFlow() {}

    //  eGarageType GetPreviousGarageType() {}

    //  ResourceFile *GetGarageBackground() {}

    //  void SetGarageBackground(ResourceFile *pBackground) {}

    //  void SetEATraxFirstButton(bool onOff) {}

    // static  bool IsPaused() {}

    // static  int GetNumPauseRequests() {}

    // static  const char *GetPauseReason(int idx) {}

    //  void ClearControllerError(int port) {}

    //  void SuppressControllerError(bool b) {}

    //  void AllowControllerError(bool b) {}

    //  bool IsAllowingControllerError() {}

    //  bool IsFirstBoot() {}

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

#endif
