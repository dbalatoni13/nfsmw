#include "EReportMilestoneAtStake.hpp"

#include <new>

#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Gameplay/GMilestone.h"
#include "Speed/Indep/Src/Interfaces/IFengHud.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Lua/source/lua.h"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"

EReportMilestoneAtStake::EReportMilestoneAtStake(GMilestone *pMilestonePtr) : Event(0x10), fMilestonePtr(pMilestonePtr) {
    IHud *hud = IPlayer::First(PLAYER_LOCAL)->GetHud();

    if (hud) {
        IGenericMessage *igenericmessage;

        if (hud->QueryInterface(&igenericmessage)) {
            char milestoneStr[64];

            bSNPrintf(milestoneStr, 64, "%s\n%s", GetTranslatedString(0xfb197367),
                      GetTranslatedString(FEDatabase->GetMilestoneHeaderHash(fMilestonePtr->GetLocalizationTag())));
            igenericmessage->RequestGenericMessage(milestoneStr, false, FEHASH_ZOOMIN, 0, 0, GenericMessage_Priority_2);
        }
    }

    PostRacePursuitScreen::GetPursuitData().AddMilestone(fMilestonePtr);
}

EReportMilestoneAtStake::~EReportMilestoneAtStake() {
}

const char *EReportMilestoneAtStake::GetEventName() const {
    return "EReportMilestoneAtStake";
}

void EReportMilestoneAtStake_MakeEvent_Callback(const void *staticData) {
    new EReportMilestoneAtStake(((EReportMilestoneAtStake::StaticData *) staticData)->fMilestonePtr);
}

int EReportMilestoneAtStake_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EReportMilestoneAtStake((GMilestone *) lua_tostring(L, 1));
    }
    return 0;
}

void EReportMilestoneAtStake_ResolveEvent_Callback(void *event, const UGroup *group) {
    new (&((EReportMilestoneAtStake::StaticData *) event)->fMilestonePtr) CARP::TagReference(group);
}
