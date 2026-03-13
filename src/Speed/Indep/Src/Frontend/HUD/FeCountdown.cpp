#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

Countdown::Countdown(UTL::COM::Object *pOutter, const char *pkg_name, int player_number)
    : HudElement(pkg_name, 0) //
    , ICountdown(pOutter)
{
}

void Countdown::Update(IPlayer *player) {
}

void Countdown::BeginCountdown() {
    mCountdown = RACE_COUNTDOWN_NUMBER_4;
    SetSoundControlState(true, SNDSTATE_NIS_321, "BeginCountdown");
}

bool Countdown::IsActive() {
    return mCountdown > 0;
}

float Countdown::GetSecondsBeforeRaceStart() {
    if (mCountdown == RACE_COUNTDOWN_NUMBER_4) {
        return 3.0f;
    }
    if (mCountdown > 0) {
        return static_cast< float >(mCountdown) - static_cast< float >((WorldTimer - mSecondTimer).GetPackedTime()) * 0.00025f;
    }
    return 0.0f;
}
