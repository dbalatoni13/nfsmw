#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

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
