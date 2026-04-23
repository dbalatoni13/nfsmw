#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Generated/Messages/MNotifyMessageDone.h"

extern FEString *FEngFindString(const char *pkg_name, int name_hash);
extern unsigned int FEngHashString(const char *, ...);
extern void FEngSetLanguageHash(FEString *, unsigned int);
extern bool FEngIsScriptSet(FEObject *obj, unsigned int script_hash);
extern bool FEngIsScriptRunning(FEObject *object, unsigned int script_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool start_at_beginning);

struct SixDaysLater : MenuScreen {
    SixDaysLater(ScreenConstructorData *sd);
    ~SixDaysLater() override;
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

SixDaysLater::~SixDaysLater() {}

void SixDaysLater::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    if (msg == 0xC98356BA) {
        if (FEngIsScriptSet(mpDataMainString, 0x5079c8f8)) {
            if (!FEngIsScriptSet(GetPackageName(), 0x53d9eb7e, 0x5079c8f8)) {
                FEngSetScript(GetPackageName(), 0x53d9eb7e, 0x5079c8f8, true);
            }
        }
        if (FEngIsScriptSet(mpDataMainString, 0x5a8e4ebe)) {
            if (!FEngIsScriptRunning(mpDataMainString, 0x5a8e4ebe)) {
                cFEng::Get()->QueuePackagePop(0);
                UCrc32 target(0x20d60dbf);
                MNotifyMessageDone done;
                done.Post(target);
            }
        }
    }
}
