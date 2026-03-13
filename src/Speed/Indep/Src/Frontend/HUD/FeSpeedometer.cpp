#include "Speed/Indep/Src/Frontend/HUD/FeSpeedometer.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
unsigned long FEHashUpper(const char *name);
int FEPrintf(FEString *text, const char *fmt, ...);
void FEngSetLanguageHash(FEString *text, unsigned int hash);
int bStrICmp(const char *s1, const char *s2);

bool UsingMetric();

extern const char lbl_803E4D20[];
extern const char lbl_803E4E24[];
extern const char lbl_803E4E38[];
extern const char lbl_803E4E48[];
extern const char lbl_803E4E58[];
extern const char lbl_803E4E68[];
extern const float lbl_803E4E7C;
extern const char lbl_803E4E80[];
extern const float lbl_803E4E84;
extern const float lbl_803E4E88;
extern const float lbl_803E4E8C;

Speedometer::Speedometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x8000000) //
    , ISpeedometer(pOutter) //
    , mSpeed(lbl_803E4E7C) //
{
    RegisterGroup(FEHashUpper(lbl_803E4E24));
    mpSpeedDigit1 = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4E38)));
    mpSpeedDigit2 = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4E48)));
    mpSpeedDigit3 = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4E58)));
    SpeedUnits = static_cast< FEString * >(FEngFindObject(pkg_name, FEHashUpper(lbl_803E4E68)));
}

void Speedometer::Update(IPlayer *player) {
    float speed = mSpeed;
    if (speed < lbl_803E4E7C) {
        speed = lbl_803E4E7C;
    }

    float convertedSpeed = speed * lbl_803E4E84;
    if (!UsingMetric()) {
        convertedSpeed = speed * lbl_803E4E88;
    }

    int displaySpeed = static_cast< int >(convertedSpeed);

    int digit1 = displaySpeed / 100;
    int digit2 = (displaySpeed / 10) % 10;
    int digit3 = displaySpeed % 10;

    FEPrintf(mpSpeedDigit1, lbl_803E4E80, digit1);
    FEPrintf(mpSpeedDigit2, lbl_803E4E80, digit2);
    FEPrintf(mpSpeedDigit3, lbl_803E4E80, digit3);

    if (UsingMetric()) {
        FEngSetLanguageHash(SpeedUnits, 0x84AFED0B);
    } else {
        FEngSetLanguageHash(SpeedUnits, 0x61E0FBED);
    }
}

void Speedometer::SetSpeed(float speed) {
    mSpeed = speed;
}
