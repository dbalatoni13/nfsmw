#include "EReloadHud.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Sim/Simulation.h"

EReloadHud::EReloadHud() : Event(0x10) {
}

EReloadHud::~EReloadHud() {
    for (IPlayer::List::const_iterator iter = IPlayer::GetList(PLAYER_LOCAL).begin(); iter != IPlayer::GetList(PLAYER_LOCAL).end(); ++iter) {

        IPlayer *player = const_cast<IPlayer *>(*iter);

        if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {

            bool isDrag = false;
            bool isChallenge = false;

            if (GRaceStatus::Exists()) {
                isDrag = GRaceStatus::IsDragRace();
                isChallenge = GRaceStatus::IsChallengeRace();
            }

            if (player == IPlayer::First(PLAYER_LOCAL)) {

                if (isDrag) {
                    player->SetHud(PHT_DRAG_SPLIT1);
                } else {
                    player->SetHud(PHT_SPLIT1);
                }
            } else {

                if (isDrag) {
                    player->SetHud(PHT_DRAG_SPLIT2);
                } else {
                    player->SetHud(PHT_SPLIT2);
                }
            }

            if (!isChallenge) {
                g_pEAXSound->QueueNISButtonThrough(bStringHash("IntroNisDDEnd"), 0);
            }
        } else if (GRaceStatus::Exists()) {
            bool isChallenge = GRaceStatus::IsChallengeRace();

            if (GRaceStatus::Get().GetRaceType() == GRace::kRaceType_Drag) {
                player->SetHud(PHT_DRAG);
            } else {
                player->SetHud(PHT_STANDARD);
            }

            if (!isChallenge && GRaceStatus::Get().GetRaceType() != GRace::kRaceType_None) {
                g_pEAXSound->QueueNISButtonThrough(bStringHash("RESTART_FAKE"), -1);
            }
        } else {
            player->SetHud(PHT_STANDARD);
        }
    }
}

const char *EReloadHud::GetEventName() const {
    return "EReloadHud";
}

void EReloadHud_MakeEvent_Callback(const void *staticData) {
    new EReloadHud();
}

int EReloadHud_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 0) {
        new EReloadHud();
    }
    return 0;
}

void EReloadHud_ResolveEvent_Callback(void *event, const UGroup *group) {
}
