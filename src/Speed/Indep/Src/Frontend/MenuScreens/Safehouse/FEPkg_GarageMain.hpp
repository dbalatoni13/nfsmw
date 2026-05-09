#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_FEPKG_GARAGEMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_FEPKG_GARAGEMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/FECarLoader.hpp"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/Camera/Movers/SelectCar.hpp"

typedef enum { SET_RIDE_INFO_REASON_VINYL = 0, SET_RIDE_INFO_REASON_LOAD_CAR = 1, SET_RIDE_INFO_REASON_CATCHALL = 2 } eSetRideInfoReasons;

class FEGeometryModels {
  public:
    FEGeometryModels() {
        mModelCastsShadowMapFlags = 0;
        mModelCurrGenOnly = 0;
        mModelNextGenOnly = 0;
        mNumModels = 0;
        mModels = nullptr;
    }
    void Init(char *filterPrefix);
    void UnInit();
    void Render(eView *view, bMatrix4 *local, uint32 render_flags);

  private:
    uint32 mModelCastsShadowMapFlags; // offset 0x0, size 0x4
    uint32 mModelCurrGenOnly;         // offset 0x4, size 0x4
    uint32 mModelNextGenOnly;         // offset 0x8, size 0x4
    int mNumModels;                   // offset 0xC, size 0x4
    eModel *mModels;                  // offset 0x10, size 0x4
};

class GarageMainScreen : public MenuScreen {
  public:
    GarageMainScreen(ScreenConstructorData *sd, int eview_id, RideInfo *start_ride, int player);
    ~GarageMainScreen() override;
    void NotificationMessage(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) override;
    void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason);
    void CancelCarLoad();
    void RequestCameraPush() {
        CameraPushRequested = true;
    }
    void CancelCameraPush() {
        CameraPushRequested = false;
    }
    void UpdateCurrentCameraView(bool bForce);
    void EnableCarRendering();
    void DisableCarRendering();
    bool IsCarRendering();
    void SetCustomizationCategory(int category) {
        mCustomizationCategory = category;
    }
    static GarageMainScreen *GetInstance();
    void HandleRender(uint32 render_flags);
    void RefreshBackground();
    static void BackgroundLoaded(int param);
    bool IsVisable() {
        return !HideEntireScreen;
    }

  private:
    void HandleTick(u32 msg);
    void UpdateRenderingCarParameters(FrontEndRenderingCar *fe_car);
    void HandleShowPackage(uint32 msg);
    void HandleHidePackage(uint32 msg);
    void HandleJoyEvents();
    float GetCarRotationX();
    float GetCarRotationY();
    float GetCarRotationZ();
    float GetGeometryXPos();
    float GetGeometryYPos();
    float GetGeometryZPos();
    float GetGeometryZAngle();

  public:
    GarageCarLoader *TheGarageCarLoader; // offset 0x2C, size 0x4
    eSetRideInfoReasons LoadingReason;   // offset 0x30, size 0x4

  private:
    FrontEndRenderingCar *RenderingCar; // offset 0x34, size 0x4
    FEGeometryModels mGeometryModels;   // offset 0x38, size 0x14
    SelectCarCameraMover *pCameraMover; // offset 0x4C, size 0x4
    int Player;                         // offset 0x50, size 0x4
    int CarState;                       // offset 0x54, size 0x4
    bool CameraPushRequested;           // offset 0x58, size 0x1
    Attrib::Key DesiredCamKey;          // offset 0x5C, size 0x4
    ActionQueue *mActionQ[2];           // offset 0x60, size 0x8
    FEString *pCarName;                 // offset 0x68, size 0x4
    FEString *pPlayerName;              // offset 0x6C, size 0x4
    int HideEntireScreen;               // offset 0x70, size 0x4
    int ViewID;                         // offset 0x74, size 0x4
    Attrib::Key mScreenKeyCamIsSetTo;   // offset 0x78, size 0x4
    bool bUserRotate;                   // offset 0x7C, size 0x1
    float mOrbitV;                      // offset 0x80, size 0x4
    float mOrbitH;                      // offset 0x84, size 0x4
    float mZoom;                        // offset 0x88, size 0x4
    int mCustomizationCategory;         // offset 0x8C, size 0x4
};
#endif
