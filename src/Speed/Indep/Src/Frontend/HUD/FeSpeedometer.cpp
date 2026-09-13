#include "Speed/Indep/Src/Frontend/HUD/FeSpeedometer.hpp"

#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Tools/Inc/ConversionUtil.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

Speedometer::Speedometer(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0x8000000), //
      ISpeedometer(pOutter),           //
      mSpeed(0.0f)                     //
{
    RegisterGroup(FEHashUpper("SpeedometerGroup"));
    mpSpeedDigit1 = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("SPEED_DIGIT_1")));
    mpSpeedDigit2 = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("SPEED_DIGIT_2")));
    mpSpeedDigit3 = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("SPEED_DIGIT_3")));
    SpeedUnits = static_cast<FEString *>(FEngFindObject(pkg_name, FEHashUpper("3rdPersonSpeedUnits")));
}

void Speedometer::Update(IPlayer *player) {
    float speed = bAbs(mSpeed);

    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
        speed = MPS2KPH(speed);
        FEPrintf(SpeedUnits, GetTranslatedString(0x8569a25f));
    } else {
        speed = MPS2MPH(speed);
        FEPrintf(SpeedUnits, GetTranslatedString(0x8569ab44));
    }

    int digit3 = static_cast<int>(speed * 0.01f);
    int digit2 = static_cast<int>((speed - static_cast<float>(digit3) * 100.0f) * 0.1f);
    int digit1 = static_cast<int>(speed - static_cast<float>(digit3) * 100.0f - static_cast<float>(digit2) * 10.0f);

    FEPrintf(mpSpeedDigit3, "%d", digit3);
    FEPrintf(mpSpeedDigit2, "%d", digit2);
    FEPrintf(mpSpeedDigit1, "%d", digit1);

    if (digit3 > 0) {
        FEngSetVisible(mpSpeedDigit3);
        FEngSetVisible(mpSpeedDigit2);
        FEngSetVisible(mpSpeedDigit1);
    } else if (digit2 > 0) {
        FEngSetInvisible(mpSpeedDigit3);
        FEngSetVisible(mpSpeedDigit2);
        FEngSetVisible(mpSpeedDigit1);
    } else {
        FEngSetInvisible(mpSpeedDigit3);
        FEngSetInvisible(mpSpeedDigit2);
        FEngSetVisible(mpSpeedDigit1);
    }
}
