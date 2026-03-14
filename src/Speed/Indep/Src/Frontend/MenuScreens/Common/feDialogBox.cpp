#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"

struct MenuScreen;
struct ScreenConstructorData;
struct feDialogScreen : MenuScreen { feDialogScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x258]; };

MenuScreen *DialogCreater(ScreenConstructorData *sd) {
    return new ("", 0) feDialogScreen(sd);
}

feDialogConfig::feDialogConfig() {
    bMemSet(BlurbString, 0, DIALOG_BLURB_MAX_LENGTH);
    Title = dialog_info;
    DialogHandle = 0;
    fCountdown = 0.0f;
    Button1TextHash = 0;
    Button1PressedMessage = 0;
    Button2TextHash = 0;
    Button2PressedMessage = 0;
    Button3TextHash = 0;
    Button3PressedMessage = 0;
    DialogCancelledMessage = 0;
    FirstButton = 0;
    ParentPackage = nullptr;
    DialogPackage = nullptr;
    NumButtons = 0;
    bIsDismissable = false;
    bDetectController = false;
    bBlurbIsUTF8 = false;
}