#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Frontend/FEReflected.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

extern UnlockDatum TheUnlockData[57];
extern char gMaxPartLevels[NUM_UNLOCKABLES];
extern bool gVerboseTesterOutput;

void DefaultUnlockData() {
    bMemSet(&TheUnlockData, 0, sizeof(TheUnlockData));
    bMemSet(&gMaxPartLevels, 0, NUM_UNLOCKABLES);
    gMaxPartLevels[UNLOCKABLE_THING_PUT_TIRES] = 3;
    gMaxPartLevels[UNLOCKABLE_THING_PUT_BRAKES] = 4;
    gMaxPartLevels[UNLOCKABLE_THING_PUT_CHASSIS] = 3;
    gMaxPartLevels[UNLOCKABLE_THING_PUT_TRANSMISSION] = 4;
    gMaxPartLevels[UNLOCKABLE_THING_PUT_ENGINE] = 4;
    gMaxPartLevels[UNLOCKABLE_THING_PUT_INDUCTION] = 3;
    gMaxPartLevels[UNLOCKABLE_THING_PUT_NOS] = 3;
    gMaxPartLevels[UNLOCKABLE_THING_BODY_KIT] = 4;
    gMaxPartLevels[UNLOCKABLE_THING_SPOILERS] = 5;
    gMaxPartLevels[UNLOCKABLE_THING_RIM_BRANDS] = 6;
    gMaxPartLevels[UNLOCKABLE_THING_HOODS] = 6;
    gMaxPartLevels[UNLOCKABLE_THING_ROOF_SCOOPS] = 6;
    gMaxPartLevels[UNLOCKABLE_THING_CUSTOM_HUD] = 4;
    gMaxPartLevels[UNLOCKABLE_THING_WINDOW_TINT] = 4;
    gMaxPartLevels[UNLOCKABLE_THING_PAINTABLE_BODY] = 3;
    gMaxPartLevels[UNLOCKABLE_VINYLS_GROUP_BODY] = 6;
}

void UnlockUnlockableThing(eUnlockableEntity entity, uint32 filter, int level, const char *part_name) {
    level = bMax(0, level);
    if (filter & UNLOCK_QUICK_RACE) {
        TheUnlockData[entity].QuickRaceUnlockLevel = level;
        TheUnlockData[entity].QuickRaceIsNewPart = level;
        return;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        TheUnlockData[entity].CareerUnlockLevel = level;
        TheUnlockData[entity].CareerIsNewPart = level;
        TheUnlockData[entity].QuickRaceUnlockLevel = level;
        TheUnlockData[entity].QuickRaceIsNewPart = level;
    }
}

void MarkUnlockableThingSeen(eUnlockableEntity entity, uint32 filter) {
    if (filter & UNLOCK_QUICK_RACE) {
        if (++TheUnlockData[entity].QuickRaceTimesSeen <= 3) {
            return;
        }
        TheUnlockData[entity].QuickRaceTimesSeen = 0;
        TheUnlockData[entity].QuickRaceIsNewPart = UNLOCK_IS_OLD;
        return;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        if (++TheUnlockData[entity].CareerTimesSeen <= 3) {
            return;
        }
        TheUnlockData[entity].CareerTimesSeen = 0;
        TheUnlockData[entity].CareerIsNewPart = UNLOCK_IS_OLD;
        return;
    }
}

bool DoesCategoryHaveNewUnlock(eUnlockableEntity entity) {
    bool answer = false;

    switch (entity) {
        case UNLOCKABLE_THING_CUSTOMIZE_PARTS:
            answer |= (TheUnlockData[UNLOCKABLE_THING_BODY_KIT].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_SPOILERS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_RIM_BRANDS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_HOODS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_ROOF_SCOOPS].CareerIsNewPart != UNLOCK_IS_OLD);
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE:
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_TIRES].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_BRAKES].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_CHASSIS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_TRANSMISSION].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_ENGINE].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_INDUCTION].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_NOS].CareerIsNewPart != UNLOCK_IS_OLD);
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_VISUAL:
            answer |= (TheUnlockData[UNLOCKABLE_DECAL_HOOD].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_CUSTOM_HUD].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_DECAL_NUMBERS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PAINTABLE_BODY].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_WINDOW_TINT].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_VINYLS_GROUP_BODY].CareerIsNewPart != UNLOCK_IS_OLD);
            break;
        default:
            return false;
    }

    return answer;
}

bool QuickRaceUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, int player, bool backroom) {
    bool answer = false;
    answer |= UnlockAllThings;
#ifndef EA_BUILD_A124
    answer |= FEDatabase->GetCareerSettings()->HasBeatenCareer();
#endif
    answer |= (level <= TheUnlockData[ent].QuickRaceUnlockLevel);
    answer |= FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce;
    return answer;
}

bool QuickRaceUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    bool answer = false;
    answer |= UnlockAllThings;
    answer |= (part->GetUpgradeLevel() == 0);
#ifdef FIX_BUGS // BUG: backroom is passed as the player argument (also below); harmless, both are unused
    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapCarPartToUnlockable(carslot, part), part->GetUpgradeLevel(), 0, backroom);
#else
    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapCarPartToUnlockable(carslot, part), part->GetUpgradeLevel(), backroom, 0);
#endif
    return answer;
}

bool QuickRaceUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    bool answer = false;
    answer |= UnlockAllThings;

#ifdef FIX_BUGS
    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapPerfPkgToUnlockable(pkg_type), level, 0, backroom);
#else
    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapPerfPkgToUnlockable(pkg_type), level, backroom, 0);
#endif

    return answer;
}

bool QuickRaceUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    bool answer = false;
    answer |= UnlockAllThings;
    answer = answer | GRaceDatabase::Get().IsQuickRaceUnlocked(event_hash);
    if (event_hash == Attrib::StringHash32("19.8.31")) {
        return true;
    }
    return answer;
}

bool QuickRaceUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car, int player) {
    bool answer = false;
    answer |= UnlockAllThings;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(player);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);

    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    answer |= (CarAttribs.UnlockedAt() >= FEDatabase->GetCareerSettings()->GetCurrentBin());

    if (fe_car->MatchesFilter(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_STOCK | FE_CAR_FILTER_LIST_CAREER | FE_CAR_FILTER_LIST_QUICK_RACE)) {
        switch (fe_car->GetType()) {
            case CARTYPE_RX8:
            case CARTYPE_IMPREZAWRX:
            case CARTYPE_MUSTANGGT:
            case CARTYPE_SL500:
            case CARTYPE_997S:
            case CARTYPE_IS300:
            case CARTYPE_GTI:
            case CARTYPE_GALLARDO:
            case CARTYPE_COBALTSS:
            case CARTYPE_PUNTO:
                answer = true;
                break;
        }
    } else if (fe_car->MatchesFilter(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_BONUS)) {
        switch (fe_car->Handle) {
            case STRINGHASH_BL2:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 1) {
                    return true;
                }
                break;
            case STRINGHASH_BL3:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 2) {
                    return true;
                }
                break;
            case STRINGHASH_BL4:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 3) {
                    return true;
                }
                break;
            case STRINGHASH_BL5:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 4) {
                    return true;
                }
                break;
            case STRINGHASH_BL6:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 5) {
                    return true;
                }
                break;
            case STRINGHASH_BL7:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 6) {
                    return true;
                }
                break;
            case STRINGHASH_BL8:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 7) {
                    return true;
                }
                break;
            case STRINGHASH_BL9:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 8) {
                    return true;
                }
                break;
            case STRINGHASH_BL10:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 9) {
                    return true;
                }
                break;
            case STRINGHASH_BL11:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 10) {
                    return true;
                }
                break;
            case STRINGHASH_BL12:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 11) {
                    return true;
                }
                break;
            case STRINGHASH_BL13:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 12) {
                    return true;
                }
                break;
            case STRINGHASH_BL14:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 13) {
                    return true;
                }
                break;
            case STRINGHASH_BL15:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 14) {
                    return true;
                }
                break;
            case STRINGHASH_E3_DEMO_BMW:
            case STRINGHASH_BONUS_C6R:
#ifndef EA_BUILD_A124
                if (FEDatabase->GetCareerSettings()->HasBeatenCareer()) {
                    return true;
                }
                break;
#else
                break;
#endif
            case STRINGHASH_BONUS_SL65:
                if (FEDatabase->GetCareerSettings()->HasBeatenSpecialChallengeEvent()) {
                    return true;
                }
                break;
            case STRINGHASH_BONUS_GT2:
                if (FEDatabase->GetCareerSettings()->HasBeatenChallengeSeries()) {
                    return true;
                }
                break;
            case STRINGHASH_CASTROLGT:
                if (FEDatabase->GetCareerSettings()->HasBeenAwardedCastrolGT()) {
                    return true;
                }
                break;
            case STRINGHASH_CE_ELISE:
            case STRINGHASH_CE_SL500:
            case STRINGHASH_CE_SUPRA:
            case STRINGHASH_CE_GTRSTREET:
            case STRINGHASH_CE_C6R:
            case STRINGHASH_CE_GT2:
            case STRINGHASH_CE_CAMARO:
            case STRINGHASH_CE_CORVETTE:
            case STRINGHASH_CE_997S:
            case STRINGHASH_CE_SL65:
                if (GetIsCollectorsEdition()) {
                    return true;
                }
                break;
        }
        return false;
    }

    return answer;
}

bool QuickRaceUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level, int player) {
    bool answer = false;
    return answer;
}

bool OnlineUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom) {
    bool answer = false;
#ifdef FIX_BUGS
    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, ent, level, 0, backroom);
#else
    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, ent, level, backroom, 0);
#endif
    return answer;
}

bool OnlineUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer = false;
#ifdef FIX_BUGS
    answer |= QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, 0, backroom);
#else
    answer |= QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom, 0);
#endif
    return answer;
}

bool OnlineUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer = false;
#ifdef FIX_BUGS
    answer |= QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, 0, backroom);
#else
    answer |= QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom, 0);
#endif
    return answer;
}

bool OnlineUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = false;
    answer |= UnlockAllThings;
    answer = answer | GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_QuickRace);
    answer = answer | GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_Online);
    return answer;
}

bool OnlineUnlocker::IsCarUnlocked(eUnlockFilters filter, uint32 car) {
    return QuickRaceUnlocker::IsCarUnlocked(filter, car, 0);
}

bool OnlineUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0);
    return answer;
}

bool CareerUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom) {
    bool answer = false;
    answer |= UnlockAllThings;
#ifndef EA_BUILD_A124
    answer |= FEDatabase->GetCareerSettings()->HasBeatenCareer();
#endif
    answer |= (level <= TheUnlockData[ent].CareerUnlockLevel);
    answer |= FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce;
    if (backroom) {
        answer |= (level <= TheUnlockData[ent].CareerUnlockLevel + 1);
        if (TheUnlockData[ent].CareerUnlockLevel == gMaxPartLevels[ent]) {
            answer |= (level <= 7);
        }
    }
    return answer;
}

bool CareerUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer = false;
    answer |= UnlockAllThings;
    answer |= (part->GetUpgradeLevel() == 0);
    answer |= CareerUnlocker::IsUnlockableUnlocked(filter, MapCarPartToUnlockable(carslot, part), part->GetUpgradeLevel(), backroom);
    return answer;
}

bool CareerUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer = false;
    answer |= UnlockAllThings;
    answer |= CareerUnlocker::IsUnlockableUnlocked(filter, MapPerfPkgToUnlockable(pkg_type), level, backroom);
    return answer;
}

bool CareerUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = false;
    answer |= UnlockAllThings;
    answer |= GRaceDatabase::Get().IsCareerRaceUnlocked(event_hash);
    return answer;
}

bool CareerUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car) {
    bool answer = false;
    answer |= UnlockAllThings;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    answer |= (CarAttribs.UnlockedAt() >= FEDatabase->GetCareerSettings()->GetCurrentBin());
    return answer;
}

bool CareerUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    FEMarkerManager::ePossibleMarker marker = FEMarkerManager::MARKER_NONE;
    switch (ent) {
        case UNLOCKABLE_THING_CUSTOMIZE_PARTS:
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_BODY_KIT, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_SPOILERS, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRANDS, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_HOODS, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_ROOF_SCOOPS, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_CUSTOM_HUD, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN, level);
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE:
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TIRES, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_BRAKES, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_CHASSIS, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TRANSMISSION, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_ENGINE, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_INDUCTION, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_NOS, level);
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_VISUAL:
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PAINT_METALLIC, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_VINYLS_GROUP_FLAME, level);
            answer |= CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_DECAL_WINDSHIELD, level);
            break;
        case UNLOCKABLE_THING_PUT_TIRES:
            marker = FEMarkerManager::MARKER_TIRES;
            break;
        case UNLOCKABLE_THING_PUT_BRAKES:
            marker = FEMarkerManager::MARKER_BRAKES;
            break;
        case UNLOCKABLE_THING_PUT_CHASSIS:
            marker = FEMarkerManager::MARKER_CHASSIS;
            break;
        case UNLOCKABLE_THING_PUT_TRANSMISSION:
            marker = FEMarkerManager::MARKER_TRANSMISSION;
            break;
        case UNLOCKABLE_THING_PUT_ENGINE:
            marker = FEMarkerManager::MARKER_ENGINE;
            break;
        case UNLOCKABLE_THING_PUT_INDUCTION:
            marker = FEMarkerManager::MARKER_INDUCTION;
            break;
        case UNLOCKABLE_THING_PUT_NOS:
            marker = FEMarkerManager::MARKER_NOS;
            break;
        case UNLOCKABLE_THING_BODY_KIT:
            marker = FEMarkerManager::MARKER_BODY;
            break;
        case UNLOCKABLE_THING_SPOILERS:
            marker = FEMarkerManager::MARKER_SPOILER;
            break;
        case UNLOCKABLE_THING_HOODS:
            marker = FEMarkerManager::MARKER_HOOD;
            break;
        case UNLOCKABLE_THING_ROOF_SCOOPS:
            marker = FEMarkerManager::MARKER_ROOF_SCOOP;
            break;
        case UNLOCKABLE_THING_CUSTOM_HUD:
            marker = FEMarkerManager::MARKER_CUSTOM_HUD;
            break;
        case UNLOCKABLE_THING_PAINT_METALLIC:
        case UNLOCKABLE_THING_PAINT_PEARL:
        case UNLOCKABLE_THING_PAINT_GLOSS:
        case UNLOCKABLE_THING_PAINT_STOCK:
        case UNLOCKABLE_THING_PAINTABLE_BODY:
            marker = FEMarkerManager::MARKER_PAINT;
            break;
        case UNLOCKABLE_THING_RIM_BRANDS:
        case UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN:
        case UNLOCKABLE_THING_RIM_BRAND_ADR:
        case UNLOCKABLE_THING_RIM_BRAND_BBS:
        case UNLOCKABLE_THING_RIM_BRAND_ENKEI:
        case UNLOCKABLE_THING_RIM_BRAND_KONIG:
        case UNLOCKABLE_THING_RIM_BRAND_LOWENHART:
        case UNLOCKABLE_THING_RIM_BRAND_RACING_HART:
        case UNLOCKABLE_THING_RIM_BRAND_OZ:
        case UNLOCKABLE_THING_RIM_BRAND_VOLK:
        case UNLOCKABLE_THING_RIM_BRAND_ROJA:
            marker = FEMarkerManager::MARKER_RIMS;
            break;
        case UNLOCKABLE_VINYLS_GROUP_FLAME:
        case UNLOCKABLE_VINYLS_GROUP_TRIBAL:
        case UNLOCKABLE_VINYLS_GROUP_STRIPE:
        case UNLOCKABLE_VINYLS_GROUP_RACING_FLAG:
        case UNLOCKABLE_VINYLS_GROUP_NATIONAL_FLAG:
        case UNLOCKABLE_VINYLS_GROUP_BODY:
        case UNLOCKABLE_VINYLS_GROUP_UNIQUE:
        case UNLOCKABLE_VINYLS_GROUP_CONTEST:
            marker = FEMarkerManager::MARKER_VINYL;
            break;
        case UNLOCKABLE_DECAL_WINDSHIELD:
        case UNLOCKABLE_DECAL_REAR_WINDOW:
        case UNLOCKABLE_DECAL_LEFT_DOOR:
        case UNLOCKABLE_DECAL_RIGHT_DOOR:
        case UNLOCKABLE_DECAL_LEFT_QP:
        case UNLOCKABLE_DECAL_RIGHT_QP:
        case UNLOCKABLE_DECAL_HOOD:
        case UNLOCKABLE_DECAL_SLOT_1:
        case UNLOCKABLE_DECAL_SLOT_2:
        case UNLOCKABLE_DECAL_SLOT_3:
        case UNLOCKABLE_DECAL_SLOT_4:
        case UNLOCKABLE_DECAL_SLOT_5:
        case UNLOCKABLE_DECAL_SLOT_6:
            marker = FEMarkerManager::MARKER_DECAL;
            break;
        default:
            return false;
    }
    answer |= TheFEMarkerManager.IsMarkerAvailable(marker, 0);
    return answer;
}

bool UnlockSystem::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity thing, int level, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, thing, level, player, backroom);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer |= CareerUnlocker::IsUnlockableUnlocked(filter, thing, level, backroom);
    }
    if (filter & UNLOCK_ONLINE) {
        answer |= OnlineUnlocker::IsUnlockableUnlocked(filter, thing, level, backroom);
    }
    return answer;
}

bool UnlockSystem::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer |= QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, player, backroom);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer |= CareerUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom);
    }
    if (filter & UNLOCK_ONLINE) {
        answer |= OnlineUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom);
    }
    return answer;
}

bool UnlockSystem::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer |= QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, player, backroom);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer |= CareerUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom);
    }
    if (filter & UNLOCK_ONLINE) {
        answer |= OnlineUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom);
    }
    return answer;
}

bool UnlockSystem::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer |= QuickRaceUnlocker::IsTrackUnlocked(filter, event_hash, player);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer |= CareerUnlocker::IsTrackUnlocked(filter, event_hash);
    }
    if (filter & UNLOCK_ONLINE) {
        answer |= OnlineUnlocker::IsTrackUnlocked(filter, event_hash);
    }
    return answer;
}

bool UnlockSystem::IsCarUnlocked(eUnlockFilters filter, unsigned int handle, int player) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer |= QuickRaceUnlocker::IsCarUnlocked(filter, handle, player);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer |= CareerUnlocker::IsCarUnlocked(filter, handle);
    }
    if (filter & UNLOCK_ONLINE) {
        answer |= OnlineUnlocker::IsCarUnlocked(filter, handle);
    }
    answer |= (GetIsCollectorsEdition() && UnlockSystem::IsBonusCarCEOnly(handle));
    return answer;
}

bool UnlockSystem::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer |= QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer |= CareerUnlocker::IsBackroomAvailable(filter, ent, level);
    }
    if (filter & UNLOCK_ONLINE) {
        answer |= OnlineUnlocker::IsBackroomAvailable(filter, ent, level);
    }
    return answer;
}

bool UnlockSystem::IsUnlockableNew(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    if (level == UNLOCK_LEVEL_ANY) {
        if (filter & UNLOCK_QUICK_RACE) {
            return TheUnlockData[ent].QuickRaceIsNewPart != UNLOCK_IS_OLD;
        }
        if (filter & UNLOCK_CAREER_MODE) {
            return TheUnlockData[ent].CareerIsNewPart != UNLOCK_IS_OLD;
        }
    }
    if (filter & UNLOCK_CAREER_MODE) {
        return TheUnlockData[ent].CareerIsNewPart == level;
    }
    return TheUnlockData[ent].QuickRaceIsNewPart == level;
}

void UnlockSystem::ClearNewUnlock(eUnlockableEntity ent, uint32 filter) {
    if (filter & UNLOCK_QUICK_RACE) {
        TheUnlockData[ent].QuickRaceIsNewPart = UNLOCK_IS_OLD;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        TheUnlockData[ent].CareerIsNewPart = UNLOCK_IS_OLD;
    }
}
// 0x8017B688: d:/mw/speed/indep/src/frontend/careers/UnlockSystem.cpp (line 773)
eUnlockableEntity MapCarPartToUnlockable(int carslot, CarPart *part) {
    switch (carslot) {
        case CARSLOTID_BODY:
            return UNLOCKABLE_THING_BODY_KIT;
        case CARSLOTID_SPOILER:
            return UNLOCKABLE_THING_SPOILERS;
        case CARSLOTID_HOOD:
            return UNLOCKABLE_THING_HOODS;
        case CARSLOTID_ROOF:
            return UNLOCKABLE_THING_ROOF_SCOOPS;
        case CARSLOTID_LICENSE_PLATE:
            return UNLOCKABLE_THING_LICENSE_PLATE;
        case CARSLOTID_CUSTOM_HUD:
            return UNLOCKABLE_THING_CUSTOM_HUD;
        case CARSLOTID_WINDOW_TINT:
            return UNLOCKABLE_THING_WINDOW_TINT;
        case CARSLOTID_FRONT_WHEEL:
            return UNLOCKABLE_THING_RIM_BRANDS;
        case CARSLOTID_REAR_WHEEL:
            return UNLOCKABLE_THING_RIM_BRANDS;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX6:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX7:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX6:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX7:
            return UNLOCKABLE_DECAL_NUMBERS;
        case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
        case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
            return UNLOCKABLE_DECAL_WINDSHIELD;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_QUARTER_TEX0:
        case CARSLOTID_DECAL_RIGHT_QUARTER_TEX0:
            return UNLOCKABLE_DECAL_LEFT_QP;
        case CARSLOTID_BASE_PAINT:
        case CARSLOTID_PAINT_RIM:
            return UNLOCKABLE_THING_PAINTABLE_BODY;
        case CARSLOTID_VINYL_LAYER0:
        case CARSLOTID_VINYL_COLOUR0_0:
        case CARSLOTID_VINYL_COLOUR0_1:
        case CARSLOTID_VINYL_COLOUR0_2:
        case CARSLOTID_VINYL_COLOUR0_3:
            return UNLOCKABLE_VINYLS_GROUP_BODY;
        default:
            return UNLOCKABLE_THING_UNKNOWN;
    }
}

// 0x8017B7D4: d:/mw/speed/indep/src/frontend/careers/UnlockSystem.cpp (line 842)
eUnlockableEntity MapPerfPkgToUnlockable(Physics::Upgrades::Type pkg_type) {
    switch (pkg_type) {
        case Physics::Upgrades::PUT_TIRES:
            return UNLOCKABLE_THING_PUT_TIRES;
        case Physics::Upgrades::PUT_BRAKES:
            return UNLOCKABLE_THING_PUT_BRAKES;
        case Physics::Upgrades::PUT_CHASSIS:
            return UNLOCKABLE_THING_PUT_CHASSIS;
        case Physics::Upgrades::PUT_TRANSMISSION:
            return UNLOCKABLE_THING_PUT_TRANSMISSION;
        case Physics::Upgrades::PUT_ENGINE:
            return UNLOCKABLE_THING_PUT_ENGINE;
        case Physics::Upgrades::PUT_INDUCTION:
            return UNLOCKABLE_THING_PUT_INDUCTION;
        case Physics::Upgrades::PUT_NOS:
            return UNLOCKABLE_THING_PUT_NOS;
    }
    return UNLOCKABLE_THING_UNKNOWN;
}

const FECarPartInfo *LookupFEPartInfo(eUnlockableEntity unlockable, int upgrade_level) {
    const Attrib::Class *feclass = Attrib::Database::Get().GetClass(0x85885722);
    Attrib::Key key = feclass->GetFirstCollection();
    Attrib::Gen::frontend carparts(Attrib::StringToKey("carparts"), 0, nullptr);
    while (key) {
        Attrib::Gen::frontend part(key, 0, nullptr);
        if (part.GetParent() == Attrib::StringToKey("carparts")) {
            if (unlockable == part.feCarPartName()) {
                for (unsigned int i = 0; i < part.Num_feCarPartInfo(); i++) {
                    if (static_cast<int>(part.feCarPartInfo(i).Level) == upgrade_level) {
                        return &part.feCarPartInfo(i);
                    }
                }
            }
        }
        key = feclass->GetNextCollection(key);
    }
    return nullptr;
}

int UnlockSystem::GetPerfPackageCost(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player) {
    eUnlockableEntity unlockable = MapPerfPkgToUnlockable(pkg_type);
    float price = 0.0f;
    if (unlockable != UNLOCKABLE_THING_UNKNOWN) {
        const FECarPartInfo *info = LookupFEPartInfo(unlockable, level);
        if (info != nullptr) {
            price = info->Cost;
        }
    }
    return static_cast<int>(price);
}

int UnlockSystem::GetCarPartCost(eUnlockFilters filter, int carslot, CarPart *part, int player) {
    eUnlockableEntity unlockable = MapCarPartToUnlockable(carslot, part);
    float price = 0.0f;
    if (unlockable != UNLOCKABLE_THING_UNKNOWN) {
        const FECarPartInfo *info = LookupFEPartInfo(unlockable, part->GetUpgradeLevel());
        if (info != nullptr) {
            price = info->Cost;
        }
    }
    return static_cast<int>(price);
}

int UnlockSystem::GetCarPartRep(eUnlockFilters filter, int carslot, CarPart *part, int player) {
    eUnlockableEntity unlockable = MapCarPartToUnlockable(carslot, part);
    float price = 0.0f;
    if (unlockable != UNLOCKABLE_THING_UNKNOWN) {
        const FECarPartInfo *info = LookupFEPartInfo(unlockable, part->GetUpgradeLevel());
        if (info != nullptr) {
            price = info->Rep;
        }
    }
    return static_cast<int>(price);
}

bool UnlockSystem::IsEventAvailable(uint32 event_hash) {
    if (event_hash == Attrib::StringHash32("99.1.1")) {
        return false;
    }
    if (event_hash == Attrib::StringHash32("21.1.1") || event_hash == Attrib::StringHash32("21.2.1") ||
        event_hash == Attrib::StringHash32("21.2.2") || event_hash == Attrib::StringHash32("19.9.70")) {
        if (GetIsCollectorsEdition()) {
            return true;
        }
        return false;
    } else {
        if (event_hash == Attrib::StringHash32("19.8.31")) {
            if (gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_BURGER_KING) && !FEDatabase->GetCareerSettings()->HasBeenAwardedBKReward()) {
                return true;
            }
            return false;
        }
    }
    return true;
}

bool UnlockSystem::IsBonusCarAvailable(uint32 name_hash) {
    if (IsBonusCarCEOnly(name_hash)) {
        if (GetIsCollectorsEdition()) {
            return true;
        }
        return false;
    }
    return true;
}

bool UnlockSystem::IsBonusCarCEOnly(uint32 name_hash) {
    switch (name_hash) {
        case 0x02d642b8:
        case 0x03d3401a:
        case 0x03d8a6d1:
        case STRINGHASH_CE_GTRSTREET:
        case STRINGHASH_CE_C6R:
        case STRINGHASH_CE_GT2:
        case STRINGHASH_CE_CAMARO:
        case STRINGHASH_CE_CORVETTE:
        case STRINGHASH_CE_997S:
        case STRINGHASH_CE_SL65:
            return true;
        default:
            return false;
    }
}

bool UnlockSystem::IsUnlockableAvailable(uint32 part_name_hash) {

    switch (part_name_hash) {
        case STRINGHASH_CE01:
        case STRINGHASH_CE02:
        case STRINGHASH_CE03:
            if (GetIsCollectorsEdition()) {
                return true;
            }
            return false;
    }
    return true;
}

FEMarkerManager TheFEMarkerManager;

FEMarkerManager::FEMarkerManager() {
    Default();
}

void FEMarkerManager::Default() {
    for (int i = 0; i < 63; i++) {
        OwnedMarkers[i].Marker = MARKER_NONE;
        OwnedMarkers[i].Param = 0;
        OwnedMarkers[i].State = MARKER_STATE_NOT_OWNED;
    }
    ClearMarkersForLaterSelection();
}

void FEMarkerManager::CheatToGetMarkers() {
    for (int i = MARKER_FIRST; i <= MARKER_LAST; i++) {
        if (i != MARKER_CASH && i != MARKER_PINK_SLIP) {
            AddMarkerToInventory(static_cast<ePossibleMarker>(i), 0);
        }
    }
}

void FEMarkerManager::GetMarkerForLaterSelection(int index, ePossibleMarker &marker, int &param) {
    marker = TempSelectionMarkers[index].Marker;
    param = TempSelectionMarkers[index].Param;
}

void FEMarkerManager::AddMarkerForLaterSelection(ePossibleMarker marker, int param) {
    for (int i = 0; i < 6; i++) {
        if (TempSelectionMarkers[i].Marker == MARKER_NONE) {
            TempSelectionMarkers[i].Marker = marker;
            TempSelectionMarkers[i].Param = param;
            iNumTempMarkers++;
            return;
        }
    }
}

void FEMarkerManager::ClearMarkersForLaterSelection() {
    for (int i = 0; i < 6; i++) {
        TempSelectionMarkers[i].Marker = MARKER_NONE;
        TempSelectionMarkers[i].Param = 0;
        TempSelectionMarkers[i].State = MARKER_STATE_NOT_OWNED;
    }
    iNumTempMarkers = 0;
}

void FEMarkerManager::AddMarkerToInventory(ePossibleMarker marker, int param) {
    for (int i = 0; i < 63; i++) {
        if (OwnedMarkers[i].Marker == MARKER_NONE) {
            OwnedMarkers[i].Marker = marker;
            OwnedMarkers[i].Param = param;
            OwnedMarkers[i].State = MARKER_STATE_OWNED;
            return;
        }
    }
}

void FEMarkerManager::UtilizeMarker(ePossibleMarker marker, int param) {
    for (int i = 0; i < 63; i++) {
        if (OwnedMarkers[i].Marker == marker && OwnedMarkers[i].Param == param && OwnedMarkers[i].State == MARKER_STATE_OWNED) {
            OwnedMarkers[i].Marker = MARKER_NONE;
            OwnedMarkers[i].State = MARKER_STATE_NOT_OWNED;
            return;
        }
    }
}

void FEMarkerManager::UtilizeMarker(uint32 slot_id) {
    ePossibleMarker marker = MARKER_NONE;
    switch (slot_id) {
        case CARSLOTID_BODY:
            marker = MARKER_BODY;
            break;
        case CARSLOTID_HOOD:
            marker = MARKER_HOOD;
            break;
        case CARSLOTID_SPOILER:
            marker = MARKER_SPOILER;
            break;
        case CARSLOTID_FRONT_WHEEL:
            marker = MARKER_RIMS;
            break;
        case CARSLOTID_ROOF:
            marker = MARKER_ROOF_SCOOP;
            break;
        case CARSLOTID_CUSTOM_HUD:
            marker = MARKER_CUSTOM_HUD;
            break;
        case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
        case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
        case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
        case CARSLOTID_DECAL_LEFT_QUARTER_TEX0:
        case CARSLOTID_DECAL_RIGHT_QUARTER_TEX0:
            marker = MARKER_DECAL;
            break;
        case CARSLOTID_VINYL_LAYER0:
            marker = MARKER_VINYL;
            break;
        case CARSLOTID_BASE_PAINT:
            marker = MARKER_PAINT;
            break;
    }
    UtilizeMarker(marker, 0);
}

void FEMarkerManager::UtilizeMarker(Physics::Upgrades::Type type) {
    ePossibleMarker marker = MARKER_NONE;
    switch (type) {
        case Physics::Upgrades::PUT_TIRES:
            marker = MARKER_TIRES;
            break;
        case Physics::Upgrades::PUT_BRAKES:
            marker = MARKER_BRAKES;
            break;
        case Physics::Upgrades::PUT_CHASSIS:
            marker = MARKER_CHASSIS;
            break;
        case Physics::Upgrades::PUT_TRANSMISSION:
            marker = MARKER_TRANSMISSION;
            break;
        case Physics::Upgrades::PUT_ENGINE:
            marker = MARKER_ENGINE;
            break;
        case Physics::Upgrades::PUT_INDUCTION:
            marker = MARKER_INDUCTION;
            break;
        case Physics::Upgrades::PUT_NOS:
            marker = MARKER_NOS;
            break;
    }
    UtilizeMarker(marker, 0);
}

bool FEMarkerManager::IsMarkerAvailable(ePossibleMarker marker, int param) {
    for (int i = 0; i < 63; i++) {
        if (OwnedMarkers[i].Marker == marker) {
            if (OwnedMarkers[i].Param == param || (param == 0 && marker == MARKER_VINYL)) {
                if (OwnedMarkers[i].State == MARKER_STATE_OWNED) {
                    return true;
                }
            }
        }
    }
    return false;
}

int FEMarkerManager::GetNumCustomizeMarkers() {
    int num = 0;
    for (int i = static_cast<int>(MARKER_CUSTOMIZE_FIRST); i < static_cast<int>(MARKER_CUSTOMIZE_LAST); i++) {
        num += GetNumMarkers(static_cast<ePossibleMarker>(i), 0);
    }
    return num;
}

int FEMarkerManager::GetNumMarkers(ePossibleMarker marker, int param) {
    int num = 0;
    for (int i = 0; i < 63; i++) {
        if (OwnedMarkers[i].Marker == marker) {
            if (OwnedMarkers[i].Param == param || (param == 0 && marker == MARKER_VINYL)) {
                if (OwnedMarkers[i].State == MARKER_STATE_OWNED) {
                    num++;
                }
            }
        }
    }
    return num;
}

FEMarkerManager::ePossibleMarker FEMarkerManager::ConvertBigBangMarkerAward(const char *marker_name, const char *partid) {
    static struct {
        const char *MarkerName;
        const char *PartName;
        FEMarkerManager::ePossibleMarker Marker;
    } unlockType[21] = {
        {"backroom", "brakes", FEMarkerManager::MARKER_BRAKES},
        {"backroom", "chassis", FEMarkerManager::MARKER_CHASSIS},
        {"backroom", "engine", FEMarkerManager::MARKER_ENGINE},
        {"backroom", "induction", FEMarkerManager::MARKER_INDUCTION},
        {"backroom", "nos", FEMarkerManager::MARKER_NOS},
        {"backroom", "tires", FEMarkerManager::MARKER_TIRES},
        {"backroom", "transmission", FEMarkerManager::MARKER_TRANSMISSION},
        {"backroom", "bodykit", FEMarkerManager::MARKER_BODY},
        {"backroom", "decals", FEMarkerManager::MARKER_DECAL},
        {"backroom", "hood", FEMarkerManager::MARKER_HOOD},
        {"backroom", "hud", FEMarkerManager::MARKER_CUSTOM_HUD},
        {"backroom", "paint", FEMarkerManager::MARKER_PAINT},
        {"backroom", "rims", FEMarkerManager::MARKER_RIMS},
        {"backroom", "roofscoop", FEMarkerManager::MARKER_ROOF_SCOOP},
        {"backroom", "spoiler", FEMarkerManager::MARKER_SPOILER},
        {"backroom", "vinyls", FEMarkerManager::MARKER_VINYL},
        {"add_impound_box", nullptr, FEMarkerManager::MARKER_ADD_IMPOUND_BOX},
        {"cash_bonus", nullptr, FEMarkerManager::MARKER_CASH},
        {"out_of_jail_free", nullptr, FEMarkerManager::MARKER_GET_OUT_OF_JAIL},
        {"pink_slip", nullptr, FEMarkerManager::MARKER_PINK_SLIP},
        {"release_car_from_impound", nullptr, FEMarkerManager::MARKER_IMPOUND_RELEASE},
    };

    for (int i = 0; i < 21; i++) {
        if (bStrICmp(marker_name, unlockType[i].MarkerName) == 0) {
            if (unlockType[i].PartName == nullptr) {
                return unlockType[i].Marker;
            }
            if (bStrICmp(partid, unlockType[i].PartName) == 0) {
                return unlockType[i].Marker;
            }
        }
    }

    return MARKER_NONE;
}

void FEMarkerManager::AwardMarker(Attrib::Gen::gameplay &inst, bool immediate_reward) {
    ePossibleMarker marker = ConvertBigBangMarkerAward(inst.RewardMarkerType(), inst.UpgradePartID());
    if (marker != MARKER_NONE) {
        int param = 0;
        if (immediate_reward) {
            switch (marker) {
                case MARKER_CASH:
                    param = static_cast<int>(inst.CashReward());
                    FEDatabase->GetCareerSettings()->AwardCash(param);
                    break;
                case MARKER_PINK_SLIP:
                    param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin());
                    {
                        FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
                        stable->AwardRivalCar(param);
                    }
                    break;
                default:
                    AddMarkerToInventory(marker, 0);
                    break;
            }
        } else {
            switch (marker) {
                case MARKER_CASH:
                    param = static_cast<int>(inst.CashReward());
                    break;
                case MARKER_PINK_SLIP:
                    param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin());
                    break;
            }
            AddMarkerForLaterSelection(marker, param);
        }
    }
}

char *FEMarkerManager::SaveToBuffer(char *buffer) {
    int nSize = GetSaveBufferSize();
    bMemCpy(buffer, this, nSize);
    return buffer + nSize;
}

char *FEMarkerManager::LoadFromBuffer(char *buffer) {
    int nSize = GetSaveBufferSize();
    bMemCpy(this, buffer, nSize);
    return buffer + nSize;
}

eUnlockableEntity ConvertBigBangUpgradeAward(const char *partname) {
    static struct {
        const char *mPartName;
        eUnlockableEntity mUnlockable;
    } unlockType[18] = {
        {"brakes", UNLOCKABLE_THING_PUT_BRAKES},
        {"chassis", UNLOCKABLE_THING_PUT_CHASSIS},
        {"engine", UNLOCKABLE_THING_PUT_ENGINE},
        {"induction", UNLOCKABLE_THING_PUT_INDUCTION},
        {"nos", UNLOCKABLE_THING_PUT_NOS},
        {"tires", UNLOCKABLE_THING_PUT_TIRES},
        {"transmission", UNLOCKABLE_THING_PUT_TRANSMISSION},
        {"bodykit", UNLOCKABLE_THING_BODY_KIT},
        {"decals", UNLOCKABLE_DECAL_HOOD},
        {"hood", UNLOCKABLE_THING_HOODS},
        {"hud", UNLOCKABLE_THING_CUSTOM_HUD},
        {"numbers", UNLOCKABLE_DECAL_NUMBERS},
        {"paint", UNLOCKABLE_THING_PAINTABLE_BODY},
        {"rims", UNLOCKABLE_THING_RIM_BRANDS},
        {"roofscoop", UNLOCKABLE_THING_ROOF_SCOOPS},
        {"spoiler", UNLOCKABLE_THING_SPOILERS},
        {"tint", UNLOCKABLE_THING_WINDOW_TINT},
        {"vinyls", UNLOCKABLE_VINYLS_GROUP_BODY},
    };

    for (unsigned int onPart = 0; onPart < 18; onPart++) {
        if (bStrCmp(partname, unlockType[onPart].mPartName) == 0) {
            return unlockType[onPart].mUnlockable;
        }
    }
    return UNLOCKABLE_THING_UNKNOWN;
}

void AwardUnlockUpgrade(Attrib::Gen::gameplay &inst) {
    const char *upgradePartName = inst.UpgradePartName();
    const char *upgradePartID = inst.UpgradePartID();
    int upgradeLevel = inst.UpgradeLevel();
    eUnlockableEntity entity = ConvertBigBangUpgradeAward(upgradePartID);
    if (entity != UNLOCKABLE_THING_UNKNOWN) {
        if (entity == UNLOCKABLE_DECAL_HOOD) {
            switch (upgradeLevel) {
                case 1:
                    entity = UNLOCKABLE_DECAL_WINDSHIELD;
                    break;
                case 2:
                    entity = UNLOCKABLE_DECAL_LEFT_DOOR;
                    break;
                case 3:
                    entity = UNLOCKABLE_DECAL_LEFT_QP;
                    break;
            }
        }
        UnlockUnlockableThing(entity, 2, upgradeLevel, "");
    }
}

void ClearAllNewStatus() {
    for (int i = 0; i < NUM_UNLOCKABLES; i++) {
        UnlockSystem::ClearNewUnlock(static_cast<eUnlockableEntity>(i), UNLOCK_CAREER_MODE);
        UnlockSystem::ClearNewUnlock(static_cast<eUnlockableEntity>(i), UNLOCK_QUICK_RACE);
    }
}
