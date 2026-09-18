#include "ECameraPhotoFinish.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/World/VisualTreatment.h"

ECameraPhotoFinish::ECameraPhotoFinish() : Event(0x10) {
}

ECameraPhotoFinish::~ECameraPhotoFinish() {
    if (IPlayer::Count(PLAYER_ALL) <= 1) {
        IPlayer *player = IPlayer::First(PLAYER_LOCAL);

        if (player) {
            if (GRaceStatus::Get().GetRacerCount() == 2) {
                IVisualTreatment *ivt = IVisualTreatment::Get();

                if (ivt) {
                    ivt->TriggerPulse(0.0f);
                }

                if (g_pEAXSound) {
                    g_pEAXSound->PlayCameraSnapShot();
                    SoundPause(false, eSNDPAUSE_PHOTOFINISH);
                    SetSoundControlState(false, SNDSTATE_PAUSE, "ECameraPhotoFinish");
                }
            }
        }
    }
}

const char *ECameraPhotoFinish::GetEventName() const {
    return "ECameraPhotoFinish";
}

void ECameraPhotoFinish_MakeEvent_Callback(const void *staticData) {
    new ECameraPhotoFinish();
}

int ECameraPhotoFinish_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new ECameraPhotoFinish();
    }
    return 0;
}

void ECameraPhotoFinish_ResolveEvent_Callback(void *event, const UGroup *group) {
}
