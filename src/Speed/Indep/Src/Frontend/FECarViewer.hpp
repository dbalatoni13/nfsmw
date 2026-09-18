#ifndef FRONTEND_FECARVIEWER_H
#define FRONTEND_FECARVIEWER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// La unidad zFeOverlay se compila entera en la seccion .over: lo hace el paso
// de post-proceso del build (tools/rename_section.py, opcion section_rename de
// configure.py), no un atributo por funcion.

class GarageMainScreen;
class RideInfo;

enum eSetRideInfoReasons {
    SET_RIDE_INFO_REASON_VINYL = 0,
    SET_RIDE_INFO_REASON_LOAD_CAR = 1,
    SET_RIDE_INFO_REASON_CATCHALL = 2,
};

enum eCarViewerWhichCar {
    eCARVIEWER_PLAYER1_CAR = 0,
    eCARVIEWER_PLAYER2_CAR = 1,
};

enum eCarViewerDramaticMode {
    CARVIEW_DRAMATIC_MODE_MAIN_MENU = 0,
    CARVIEW_DRAMATIC_MODE_UNDERGROUND_MAIN = 1,
    CARVIEW_DRAMATIC_MODE_UNDERGROUND_STATUS = 2,
    CARVIEW_DRAMATIC_MODE_ONLINE = 3,
    CARVIEW_DRAMATIC_MODE_OFF = 4,
    NUMBER_OF_CARVIEW_DRAMATIC_MODES = 4,
};

// Definidas en FECarViewer.cpp. FEPkg_GarageMain.cpp tambien lee la primera.
extern RideInfo TopOrFullScreenRide;                     // .bss 0x804AB0D4, 0x310
extern eSetRideInfoReasons TopOrFullScreenLoadingReason; // .bss 0x804AB5A8 (common)

// Solo miembros estaticos.
class CarViewer {
  public:
    static RideInfo *FindWhichRideInfoToUpdate(/* parameters unknown */); // STRIPPED

    static GarageMainScreen *FindWhichScreenToUpdate(eCarViewerWhichCar which_car);
    static void SetRideInfo(RideInfo *ride, eSetRideInfoReasons reason, eCarViewerWhichCar which_car);
    static void CancelCarLoad(eCarViewerWhichCar which_car);
    static RideInfo *GetRideInfo(eCarViewerWhichCar which_car);
    static void HideAllCars();
    static void ShowAllCars();
    static void ShowCarScreen();

    // El DWARF la declara, pero la release no emite ni un byte para ella. No
    // definirla en el .cpp: GCC la emitiria y sobrarian bytes.
    static void UnshowCarScreen();

    static bool haveLoadedOnce; // .data 0x80439110
};

#endif
