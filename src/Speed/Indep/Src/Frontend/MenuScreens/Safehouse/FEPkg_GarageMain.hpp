#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_FEPKG_GARAGEMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_FEPKG_GARAGEMAIN_H
#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include <types.h>

struct bMatrix4;
struct eView;
struct eModel;
struct FrontEndRenderingCar;
struct ActionQueue;
struct FEString;
struct SelectCarCameraMover;
struct GarageCarLoader;
namespace Attrib {
namespace Gen {
struct frontend;
}
} // namespace Attrib
struct FEGeometryModels {
    FEGeometryModels() {}
    void Init(char *filterPrefix);
    void UnInit();
    void Render(eView *view, bMatrix4 *local, unsigned int render_flags);
    unsigned int mModelCastsShadowMapFlags; // offset 0x0, size 0x4
    unsigned int mModelCurrGenOnly;         // offset 0x4, size 0x4
    unsigned int mModelNextGenOnly;         // offset 0x8, size 0x4
    int mNumModels;                         // offset 0xC, size 0x4
    eModel *mModels;                        // offset 0x10, size 0x4
};
struct GarageMainScreen : public MenuScreen {
    GarageMainScreen(ScreenConstructorData *sd, int eview_id, RideInfo *start_ride, int player);
    ~GarageMainScreen() override;
    void NotificationMessage(unsigned long Message, FEObject *pObject, unsigned long Param1, unsigned long Param2) override;
    void RequestCameraPush() {}
    void CancelCameraPush() {}
    void SetCustomizationCategory(int category) {
        mCustomizationCategory = category;
    }
    bool IsVisable() {}
    static GarageMainScreen *GetInstance();
    void EnableCarRendering();
    void DisableCarRendering();
    bool IsCarRendering();
    void HandleTick(unsigned long msg);
    void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason);
    void CancelCarLoad();
    void UpdateCurrentCameraView(bool bForce);
    void RefreshBackground();
    static void BackgroundLoaded(int param);
    float GetCarRotationX();
    float GetCarRotationY();
    float GetCarRotationZ();
    float GetGeometryZAngle();
    float GetGeometryXPos();
    float GetGeometryYPos();
    float GetGeometryZPos();
    void UpdateRenderingCarParameters(FrontEndRenderingCar *fe_car);
    void HandleRender(unsigned int render_flags);
    void HandleShowPackage(unsigned int msg);
    void HandleHidePackage(unsigned int msg);
    void HandleJoyEvents();
    static GarageMainScreen *sInstance;
    GarageCarLoader *TheGarageCarLoader; // offset 0x2C, size 0x4
    eSetRideInfoReasons LoadingReason;   // offset 0x30, size 0x4
    FrontEndRenderingCar *RenderingCar;  // offset 0x34, size 0x4
    FEGeometryModels mGeometryModels;    // offset 0x38, size 0x14
    SelectCarCameraMover *pCameraMover;  // offset 0x4C, size 0x4
    int Player;                          // offset 0x50, size 0x4
    int CarState;                        // offset 0x54, size 0x4
    bool CameraPushRequested;            // offset 0x58, size 0x1
    unsigned int DesiredCamKey;          // offset 0x5C, size 0x4
    ActionQueue *mActionQ[2];            // offset 0x60, size 0x8
    FEString *pCarName;                  // offset 0x68, size 0x4
    FEString *pPlayerName;               // offset 0x6C, size 0x4
    int HideEntireScreen;                // offset 0x70, size 0x4
    int ViewID;                          // offset 0x74, size 0x4
    unsigned int mScreenKeyCamIsSetTo;   // offset 0x78, size 0x4
    bool bUserRotate;                    // offset 0x7C, size 0x1
    float mOrbitV;                       // offset 0x80, size 0x4
    float mOrbitH;                       // offset 0x84, size 0x4
    float mZoom;                         // offset 0x88, size 0x4
    int mCustomizationCategory;          // offset 0x8C, size 0x4
};
#endif
