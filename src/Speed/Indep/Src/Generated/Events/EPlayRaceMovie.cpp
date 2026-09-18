#include "EPlayRaceMovie.hpp"

#include <new>

#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameMovieScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/InGameTutorialScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOn.hpp"
// POSICION (ensayo z10): el objetivo parsea MNotifyMovieFinished.h aqui,
// entre MGamePlayMoment.h (EMomentStrm.cpp) y MMiscSound.h (EReportInfraction.cpp).
#include "Speed/Indep/Src/Generated/Messages/MNotifyMovieFinished.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

EPlayRaceMovie::EPlayRaceMovie(const char *pMovieName) : Event(0x10), fMovieName(EventManager::EmbedField(this, pMovieName)) {
    new EFadeScreenOn(false);

    if (!bStrICmp("drag_tutorial", fMovieName) || !bStrICmp("speedtrap_tutorial", fMovieName) || !bStrICmp("tollbooth_tutorial", fMovieName) || !bStrICmp("pursuit_tutorial", fMovieName) ||
        !bStrICmp("bounty_tutorial", fMovieName)) {
        InGameAnyTutorialScreen::LaunchMovie(fMovieName, NULL);
    } else {
        InGameAnyMovieScreen::LaunchMovie(fMovieName);
    }
}

EPlayRaceMovie::~EPlayRaceMovie() {
}

const char *EPlayRaceMovie::GetEventName() const {
    return "EPlayRaceMovie";
}

void EPlayRaceMovie_MakeEvent_Callback(const void *staticData) {
    new EPlayRaceMovie(((EPlayRaceMovie::StaticData *) staticData)->fMovieName);
}

int EPlayRaceMovie_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EPlayRaceMovie(lua_tostring(L, 1));
    }
    return 0;
}

void EPlayRaceMovie_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EPlayRaceMovie::StaticData *) event)->fMovieName) CARP::TagReference(group);
}
