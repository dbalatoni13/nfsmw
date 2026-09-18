#include "EEnterBin.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/Events/EAwardUpgrade.hpp"
#include "Speed/Indep/Src/Lua/source/lua.h"

EEnterBin::EEnterBin(int pBinIndex) : Event(0x10), fBinIndex(pBinIndex) {
    unsigned int currBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    unsigned int newBin = pBinIndex;

    for (unsigned int onBin = currBin - 1; onBin >= newBin; onBin--) {
        FEDatabase->GetCareerSettings()->SetCurrentBin(onBin);
        GManager::Get().EnableBinMilestones(onBin);
        GManager::Get().EnableBinSpeedTraps(onBin);

        if (!GManager::Get().GetInGameplay()) {
            GRaceBin *bin = GRaceDatabase::Get().GetBinNumber(onBin);

            if (bin) {
                if (bin->GetChildVault() && !bin->GetChildVault()->IsLoaded()) {
                    bin->GetChildVault()->LoadSyncTransient();
                }

                ClearAllNewStatus();

                for (unsigned int onUnlock = 0; onUnlock < bin->GetBaselineUnlockCount(); onUnlock++) {
                    unsigned int unlock = bin->GetBaselineUnlock(onUnlock);

                    if (unlock) {
                        new EAwardUpgrade(unlock);
                    }
                }
            }
        }
    }

    if (GRaceStatus::Exists()) {
        GRaceStatus::Get().RefreshBinWhileInGame();
    }
}

EEnterBin::~EEnterBin() {
}

const char *EEnterBin::GetEventName() const {
    return "EEnterBin";
}

void EEnterBin_MakeEvent_Callback(const void *staticData) {
    new EEnterBin(((EEnterBin::StaticData *) staticData)->fBinIndex);
}

int EEnterBin_MakeEvent_LuaBinding(lua_State *L) {
    if (lua_gettop(L) == 1) {
        new EEnterBin((int) lua_tonumber(L, 1));
    }
    return 0;
}

void EEnterBin_ResolveEvent_Callback(void *event, const UGroup *group) {
}
