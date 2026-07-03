#ifndef FRONTEND_FECARVIEWER_H
#define FRONTEND_FECARVIEWER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/World/CarInfo.hpp"

typedef enum { SET_RIDE_INFO_REASON_VINYL = 0, SET_RIDE_INFO_REASON_LOAD_CAR = 1, SET_RIDE_INFO_REASON_CATCHALL = 2 } eSetRideInfoReasons;

typedef enum { eCARVIEWER_PLAYER1_CAR = 0, eCARVIEWER_PLAYER2_CAR = 1 } eCarViewerWhichCar;

typedef enum {
    CARVIEW_DRAMATIC_MODE_MAIN_MENU = 0,
    CARVIEW_DRAMATIC_MODE_UNDERGROUND_MAIN = 1,
    CARVIEW_DRAMATIC_MODE_UNDERGROUND_STATUS = 2,
    CARVIEW_DRAMATIC_MODE_ONLINE = 3,
    CARVIEW_DRAMATIC_MODE_OFF = 4,
    NUMBER_OF_CARVIEW_DRAMATIC_MODES = 4
} eCarViewerDramaticMode;

class CarViewer {
  private:
    static struct GarageMainScreen *FindWhichScreenToUpdate(eCarViewerWhichCar which_car);
    static RideInfo *FindWhichRideInfoToUpdate(/* parameters unknown */); // STRIPPED

  public:
    static RideInfo *GetRideInfo(eCarViewerWhichCar which_car);
    static void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car);
    static void HideAllCars();
    static void ShowAllCars();
    static void ShowCarScreen();
    static void UnshowCarScreen();
    static void CancelCarLoad(eCarViewerWhichCar which_car);

    static bool haveLoadedOnce;
};

extern RideInfo TopOrFullScreenRide;                     // size: 0x310, address: 0x804AB0D4
extern eSetRideInfoReasons TopOrFullScreenLoadingReason; // size: 0x4, address: 0x804AB5A8

#endif
