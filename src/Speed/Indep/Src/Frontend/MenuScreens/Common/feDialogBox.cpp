struct MenuScreen;
struct ScreenConstructorData;
struct feDialogScreen : MenuScreen { feDialogScreen(ScreenConstructorData *); void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override {} char _pad[0x258]; };

MenuScreen *DialogCreater(ScreenConstructorData *sd) {
    return new ("", 0) feDialogScreen(sd);
}