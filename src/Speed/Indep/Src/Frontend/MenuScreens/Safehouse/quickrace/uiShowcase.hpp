#ifndef UISHOWCASE_H
#define UISHOWCASE_H

#include <types.h>
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/career/uiRepSheetRivalStreamer.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"

// total size: 0x70
class Showcase : public MenuScreen {
  public:
    Showcase(ScreenConstructorData *sd);
    ~Showcase() override;

    void NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) override;

    static const char *FromPackage;
    static uint32 FromArgs;
    static uint32 FromIndex;
    static uint32 BlackListNumber;
    static int32 FromFilter;
#ifndef EA_BUILD_A124
    static void *FromColor[3];
#endif

  private:
    FECarRecord *car;                      // offset 0x2C, size 0x4
    FEImage *pTagImg;                      // offset 0x30, size 0x4
    uiRepSheetRivalStreamer RivalStreamer; // offset 0x34, size 0x3C
};

#endif
