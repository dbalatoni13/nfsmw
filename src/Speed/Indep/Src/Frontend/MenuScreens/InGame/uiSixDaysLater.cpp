#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"

extern FEString *FEngFindString(const char *pkg_name, int name_hash);
extern unsigned int FEngHashString(const char *, ...);
extern void FEngSetLanguageHash(FEString *, unsigned int);

struct SixDaysLater : MenuScreen {
    SixDaysLater(ScreenConstructorData *sd);
    ~SixDaysLater() override {}
    void NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) override;

    FEString *mpDataMainString; // offset 0x2C
    int mStringMode;            // offset 0x30
};

SixDaysLater::SixDaysLater(ScreenConstructorData *sd)
    : MenuScreen(sd) //
{
    mStringMode = sd->Arg;
    mpDataMainString = FEngFindString(GetPackageName(), 0xb769701e);
    FEngSetLanguageHash(mpDataMainString, FEngHashString("DDAY_TIMELAPSE_%d", mStringMode + 1));
    new EFadeScreenOff(0x14035fb);
}
