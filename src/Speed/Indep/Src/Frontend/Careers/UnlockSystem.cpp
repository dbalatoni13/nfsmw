#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"

// Definiciones, no declaraciones: el objetivo las tiene en
// .bss:0x80473AC4 (0x1C8 B) y .bss:0x80473C8C (0x39 B), las dos scope:global y
// las DOS ULTIMAS del rango .bss de zFe2 (que acaba en 0x80473CC8).  Sin
// inicializador -> grupo 3 del .bss de GCC 2.9 (definicion tentativa diferida a
// finish_file), que es justo el que se emite al final.
UnlockDatum TheUnlockData[57];
char gMaxPartLevels[NUM_UNLOCKABLES];
extern bool gVerboseTesterOutput;

void DefaultUnlockData() {
    bMemSet(&TheUnlockData, 0, sizeof(TheUnlockData));
    bMemSet(&gMaxPartLevels, 0, NUM_UNLOCKABLES);
    gMaxPartLevels[4] = 3;  // PUT_TIRES
    gMaxPartLevels[5] = 4;  // PUT_BRAKES
    gMaxPartLevels[6] = 3;  // PUT_CHASSIS
    gMaxPartLevels[7] = 4;  // PUT_TRANSMISSION
    gMaxPartLevels[8] = 4;  // PUT_ENGINE
    gMaxPartLevels[9] = 3;  // PUT_INDUCTION
    gMaxPartLevels[10] = 3; // PUT_NOS
    gMaxPartLevels[11] = 4; // BODY_KIT
    gMaxPartLevels[12] = 5; // SPOILERS
    gMaxPartLevels[13] = 6; // RIM_BRANDS
    gMaxPartLevels[14] = 6; // HOODS
    gMaxPartLevels[15] = 6; // ROOF_SCOOPS
    gMaxPartLevels[17] = 4; // CUSTOM_HUD
    gMaxPartLevels[18] = 4; // WINDOW_TINT
    gMaxPartLevels[23] = 3; // PAINTABLE_BODY
    gMaxPartLevels[40] = 6; // VINYLS_GROUP_BODY
}

void UnlockUnlockableThing(eUnlockableEntity entity, uint32 filter, int level, const char *part_name) {
    if (level < 0) {
        level = 0;
    }
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
    char count;
    if (filter & UNLOCK_QUICK_RACE) {
        if (++TheUnlockData[entity].QuickRaceTimesSeen < 4) {
            return;
        }
        TheUnlockData[entity].QuickRaceTimesSeen = 0;
        TheUnlockData[entity].QuickRaceIsNewPart = UNLOCK_IS_OLD;
        return;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        if (++TheUnlockData[entity].CareerTimesSeen < 4) {
            return;
        }
        TheUnlockData[entity].CareerTimesSeen = 0;
        TheUnlockData[entity].CareerIsNewPart = UNLOCK_IS_OLD;
        return;
    }
}

bool DoesCategoryHaveNewUnlock(eUnlockableEntity ent) {
    bool answer = false;

    switch (ent) {
        case UNLOCKABLE_THING_CUSTOMIZE_PARTS:
            if (TheUnlockData[UNLOCKABLE_THING_BODY_KIT].CareerIsNewPart != UNLOCK_IS_OLD) {
                answer = true;
            }
            answer |= (TheUnlockData[UNLOCKABLE_THING_SPOILERS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_RIM_BRANDS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_HOODS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_ROOF_SCOOPS].CareerIsNewPart != UNLOCK_IS_OLD);
            return answer;
        case UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE:
            if (TheUnlockData[UNLOCKABLE_THING_PUT_TIRES].CareerIsNewPart != UNLOCK_IS_OLD) {
                answer = true;
            }
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_BRAKES].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_CHASSIS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_TRANSMISSION].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_ENGINE].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_INDUCTION].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PUT_NOS].CareerIsNewPart != UNLOCK_IS_OLD);
            return answer;
        case UNLOCKABLE_THING_CUSTOMIZE_VISUAL:
            if (TheUnlockData[UNLOCKABLE_DECAL_HOOD].CareerIsNewPart != UNLOCK_IS_OLD) {
                answer = true;
            }
            answer |= (TheUnlockData[UNLOCKABLE_THING_CUSTOM_HUD].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_DECAL_NUMBERS].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_PAINTABLE_BODY].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_THING_WINDOW_TINT].CareerIsNewPart != UNLOCK_IS_OLD);
            answer |= (TheUnlockData[UNLOCKABLE_VINYLS_GROUP_BODY].CareerIsNewPart != UNLOCK_IS_OLD);
            return answer;
    }
    return false;
}

static inline bool AllThingsUnlocked() {
    return UnlockAllThings != 0;
}

int QuickRaceUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, int player, bool backroom) {
    bool answer = AllThingsUnlocked();
    answer = answer | FEDatabase->GetUserProfile(0)->GetCareer()->HasBeatenCareer();
    answer |= (level <= TheUnlockData[ent].QuickRaceUnlockLevel);
    return (answer != 0) | (FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce != 0);
}

int QuickRaceUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    bool answer = AllThingsUnlocked();
    answer |= (part->GetUpgradeLevel() == 0);
    return (answer != 0) |
           (QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapCarPartToUnlockable(carslot, part), part->GetUpgradeLevel(), backroom, false) != 0);
}

int QuickRaceUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    bool answer = AllThingsUnlocked();
    answer = (answer != 0) | (QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapPerfPkgToUnlockable(pkg_type), level, backroom, false) != 0);
    return answer;
}

bool QuickRaceUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    bool answer = AllThingsUnlocked();
    answer = answer | GRaceDatabase::Get().IsQuickRaceUnlocked(event_hash);
    int tutorialHash = Attrib::StringHash32("19.8.31");
    if (event_hash == tutorialHash) {
        return true;
    }
    return answer;
}

bool QuickRaceUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car, int player) {
    bool answer = AllThingsUnlocked();
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(player);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    if (CarAttribs.UnlockedAt() >= FEDatabase->GetCareerSettings()->GetCurrentBin()) {
        answer = true;
    } else {
        answer = answer != 0;
    }
    if (fe_car->MatchesFilter(0xF0007)) {
        switch (fe_car->GetType()) {
            case 5:
            case 6:
            case 8:
            case 0x17:
            case 0x18:
            case 0x2F:
            case 0x3E:
            case 0x43:
            case 0x44:
            case 0x4A:
                answer = true;
                break;
            default:
                break;
        }
    } else if (fe_car->MatchesFilter(0xF0008)) {
        switch (fe_car->Handle) {
            case 0x965F:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 2) {
                    break;
                }
                return true;
            case 0x9660:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 3) {
                    break;
                }
                return true;
            case 0x9661:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 4) {
                    break;
                }
                return true;
            case 0x9662:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 5) {
                    break;
                }
                return true;
            case 0x9663:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 6) {
                    break;
                }
                return true;
            case 0x9664:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 7) {
                    break;
                }
                return true;
            case 0x9665:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 8) {
                    break;
                }
                return true;
            case 0x9666:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 9) {
                    break;
                }
                return true;
            case 0x13624E:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 10) {
                    break;
                }
                return true;
            case 0x13624F:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 11) {
                    break;
                }
                return true;
            case 0x136250:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 12) {
                    break;
                }
                return true;
            case 0x136251:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 13) {
                    break;
                }
                return true;
            case 0x136252:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 14) {
                    break;
                }
                return true;
            case 0x136253:
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 15) {
                    break;
                }
                return true;
            case 0x3A94520:
            case 0x2CF370F0:
                if (!FEDatabase->GetCareerSettings()->HasBeatenCareer()) {
                    break;
                }
                return true;
            case 0xCB6AAF2F:
                if ((FEDatabase->GetCareerSettings()->SpecialFlags & 0x8000) == 0) {
                    break;
                }
                return true;
            case 0x2CF385B2:
                if (((FEDatabase->GetCareerSettings()->SpecialFlags >> 16) & 1) == 0) {
                    break;
                }
                return true;
            case 0x34498EB2:
                if ((FEDatabase->GetCareerSettings()->SpecialFlags & 0x40000) == 0) {
                    break;
                }
                return true;
            case 0x2D642B8:
            case 0x363A1FEA:
            case 0x54653C71:
            case 0x54655133:
            case 0x582F21D9:
            case 0x634D1BD2:
            case 0xE1075862:
            case 0xE115EAD0:
            case 0x3D8A6D1:
            case 0x3D3401A:
                if (!GetIsCollectorsEdition()) {
                    break;
                }
                return true;
            default:
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
    bool answer;
    answer = QuickRaceUnlocker::IsUnlockableUnlocked(filter, ent, level, backroom, false);
    return answer;
}

bool OnlineUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer;
    answer = QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom, false);
    return answer;
}

bool OnlineUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer;
    answer = QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom, false);
    return answer;
}

bool OnlineUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = AllThingsUnlocked();
    answer = answer | GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_QuickRace);
    answer = answer | GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_Online);
    return answer;
}

bool OnlineUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car) {
    return QuickRaceUnlocker::IsCarUnlocked(filter, car, 0);
}

bool OnlineUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0);
    return answer;
}

bool CareerUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom) {
    bool answer = AllThingsUnlocked();
    answer = answer | FEDatabase->GetUserProfile(0)->GetCareer()->HasBeatenCareer();
    answer |= (level <= TheUnlockData[ent].CareerUnlockLevel);
    answer = answer | FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce;

    if (!backroom)
        return answer;

    answer |= (level <= TheUnlockData[ent].CareerUnlockLevel + 1);

    if (TheUnlockData[ent].CareerUnlockLevel != gMaxPartLevels[ent])
        return answer;

    answer |= (level <= 7);
    return answer;
}

bool CareerUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer = AllThingsUnlocked();
    answer |= (part->GetUpgradeLevel() == 0);
    return (answer != 0) |
           (CareerUnlocker::IsUnlockableUnlocked(filter, MapCarPartToUnlockable(carslot, part), part->GetUpgradeLevel(), backroom) != 0);
}

bool CareerUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer = AllThingsUnlocked();
    answer = static_cast<bool>(answer | CareerUnlocker::IsUnlockableUnlocked(filter, MapPerfPkgToUnlockable(pkg_type), level, backroom));
    return answer;
}

bool CareerUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = AllThingsUnlocked();
    answer = answer | GRaceDatabase::Get().IsCareerRaceUnlocked(event_hash);
    return answer;
}

bool CareerUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car) {
    bool answer = AllThingsUnlocked();
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    answer = answer | (CarAttribs.UnlockedAt() >= FEDatabase->GetCareerSettings()->GetCurrentBin());
    return answer;
}

static inline bool BackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    return CareerUnlocker::IsBackroomAvailable(filter, ent, level) != 0;
}

bool CareerUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    FEMarkerManager::ePossibleMarker marker = FEMarkerManager::MARKER_NONE;
    switch (ent) {
        case UNLOCKABLE_THING_CUSTOMIZE_PARTS:
            answer = BackroomAvailable(filter, UNLOCKABLE_THING_BODY_KIT, level);
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_SPOILERS, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRANDS, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_HOODS, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_ROOF_SCOOPS, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_CUSTOM_HUD, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN, level));
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE:
            answer = BackroomAvailable(filter, UNLOCKABLE_THING_PUT_TIRES, level);
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_BRAKES, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_CHASSIS, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TRANSMISSION, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_ENGINE, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_INDUCTION, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_NOS, level));
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_VISUAL:
            answer = BackroomAvailable(filter, UNLOCKABLE_THING_PAINT_METALLIC, level);
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_VINYLS_GROUP_FLAME, level));
            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_DECAL_WINDSHIELD, level));
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
    return static_cast<bool>(answer) | static_cast<bool>(TheFEMarkerManager.IsMarkerAvailable(marker, 0));
}

bool UnlockSystem::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity thing, int level, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & 1) {
        answer = static_cast<bool>(answer | QuickRaceUnlocker::IsUnlockableUnlocked(filter, thing, level, player, backroom));
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsUnlockableUnlocked(filter, thing, level, backroom));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsUnlockableUnlocked(filter, thing, level, backroom));
    }
    return answer;
}

bool UnlockSystem::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & 1) {
        answer = static_cast<bool>(answer | QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, player, backroom));
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom));
    }
    return answer;
}

bool UnlockSystem::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & 1) {
        answer = static_cast<bool>(answer | QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, player, backroom));
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom));
    }
    return answer;
}

bool UnlockSystem::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & 1) {
        answer = static_cast<bool>(answer | QuickRaceUnlocker::IsTrackUnlocked(filter, event_hash, player));
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsTrackUnlocked(filter, event_hash));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsTrackUnlocked(filter, event_hash));
    }
    return answer;
}

bool UnlockSystem::IsCarUnlocked(eUnlockFilters filter, unsigned int handle, int player) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & 1) {
        answer = static_cast<bool>(answer | QuickRaceUnlocker::IsCarUnlocked(filter, handle, player));
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsCarUnlocked(filter, handle));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsCarUnlocked(filter, handle));
    }
    return static_cast<bool>(answer | (GetIsCollectorsEdition() && UnlockSystem::IsBonusCarCEOnly(handle)));
}

bool UnlockSystem::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    if (filter & 1) {
        answer = static_cast<bool>(answer | QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0));
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsBackroomAvailable(filter, ent, level));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsBackroomAvailable(filter, ent, level));
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
    unsigned int key = feclass->GetFirstCollection();
    Attrib::Gen::frontend carparts(Attrib::StringToKey("carparts"), 0, nullptr);
    while (key) {
        Attrib::Gen::frontend part(key, 0, nullptr);
        if (part.GetParent() == Attrib::StringToKey("carparts")) {
            if (static_cast<int>(unlockable) == static_cast<int>(part.feCarPartName())) {
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
        if (info) {
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
        if (info) {
            price = info->Cost;
        }
    }
    return static_cast<int>(price);
}

bool UnlockSystem::IsEventAvailable(unsigned int event_hash) {
    if (event_hash == Attrib::StringHash32("99.1.1")) {
        return false;
    }
    if (event_hash == Attrib::StringHash32("21.1.1") || event_hash == Attrib::StringHash32("21.2.1") ||
        event_hash == Attrib::StringHash32("21.2.2") || event_hash == Attrib::StringHash32("19.9.70")) {
        if (GetIsCollectorsEdition()) {
            return true;
        }
        return false;
    }
    if (event_hash == Attrib::StringHash32("19.8.31")) {
        if (gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_BURGER_KING) && !FEDatabase->GetCareerSettings()->HasBeenAwardedBKReward()) {
            return true;
        }
        return false;
    }
    return true;
}

bool UnlockSystem::IsBonusCarCEOnly(unsigned int name_hash) {
    switch (name_hash) {
        case 0x02d642b8:
        case 0x03d3401a:
        case 0x03d8a6d1:
        case 0x363a1fea:
        case 0x54653c71:
        case 0x54655133:
        case 0x582f21d9:
        case 0x634d1bd2:
        case 0xe1075862:
        case 0xe115ead0:
            return true;
        default:
            return false;
    }
}

bool UnlockSystem::IsUnlockableAvailable(unsigned int part_name_hash) {
    if (part_name_hash <= 0x13D0CA) {
        unsigned int collectors_edition_start = 0x13D0C8;
        if (part_name_hash < collectors_edition_start) {
            return true;
        }
        return GetIsCollectorsEdition() != 0;
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

void FEMarkerManager::UtilizeMarker(unsigned int slot_id) {
    ePossibleMarker marker = MARKER_NONE;
    switch (slot_id) {
        case 0x17:
            marker = MARKER_BODY;
            break;
        case 0x3f:
            marker = MARKER_HOOD;
            break;
        case 0x2c:
            marker = MARKER_SPOILER;
            break;
        case 0x42:
            marker = MARKER_RIMS;
            break;
        case 0x3e:
            marker = MARKER_ROOF_SCOOP;
            break;
        case 0x84:
            marker = MARKER_CUSTOM_HUD;
            break;
        case 0x53:
        case 0x5b:
        case 99:
        case 100:
        case 0x65:
        case 0x66:
        case 0x67:
        case 0x68:
        case 0x6b:
        case 0x6c:
        case 0x6d:
        case 0x6e:
        case 0x6f:
        case 0x70:
        case 0x73:
        case 0x7b:
            marker = MARKER_DECAL;
            break;
        case 0x4d:
            marker = MARKER_VINYL;
            break;
        case 0x4c:
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
    int total = 0;
    for (int i = static_cast<int>(MARKER_CUSTOMIZE_FIRST); i < static_cast<int>(MARKER_CUSTOMIZE_LAST); i++) {
        total += GetNumMarkers(static_cast<ePossibleMarker>(i), 0);
    }
    return total;
}

int FEMarkerManager::GetNumMarkers(ePossibleMarker marker, int param) {
    int count = 0;
    for (int i = 0; i < 63; i++) {
        if (OwnedMarkers[i].Marker == marker) {
            if (OwnedMarkers[i].Param == param || (param == 0 && marker == MARKER_VINYL)) {
                if (OwnedMarkers[i].State == MARKER_STATE_OWNED) {
                    count++;
                }
            }
        }
    }
    return count;
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
    for (int i = 0; i <= 20; i++) {
        if (bStrICmp(marker_name, unlockType[i].MarkerName) == 0) {
            if (unlockType[i].PartName == 0) {
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
            if (marker == MARKER_PINK_SLIP) {
                goto award_pink_slip;
            }
            if (marker != MARKER_CASH) {
                goto add_inventory;
            }
            param = static_cast<int>(inst.CashReward());
            FEDatabase->GetCareerSettings()->CurrentCash = FEDatabase->GetCareerSettings()->CurrentCash + param;
            goto award_done;

        award_pink_slip:
            param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin());
            FEDatabase->GetPlayerCarStable(0)->AwardRivalCar(param);
            goto award_done;

        add_inventory:
            AddMarkerToInventory(marker, 0);

        award_done:;
        } else {
            if (marker != MARKER_PINK_SLIP) {
                if (marker == MARKER_CASH) {
                    param = static_cast<int>(inst.CashReward());
                }
            } else {
                param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin());
            }
            AddMarkerForLaterSelection(marker, param);
        }
    }
}

// 67h627h5908672h83906h79ghg67896h79835g42h78693g24589h75342g583724h90f87095h34f8709f54j3f0357849j50789f4jh789f5jh342

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
        {"decals", static_cast<eUnlockableEntity>(50)},
        {"hood", UNLOCKABLE_THING_HOODS},
        {"hud", UNLOCKABLE_THING_CUSTOM_HUD},
        {"numbers", static_cast<eUnlockableEntity>(43)},
        {"paint", UNLOCKABLE_THING_PAINTABLE_BODY},
        {"rims", UNLOCKABLE_THING_RIM_BRANDS},
        {"roofscoop", UNLOCKABLE_THING_ROOF_SCOOPS},
        {"spoiler", UNLOCKABLE_THING_SPOILERS},
        {"tint", UNLOCKABLE_THING_WINDOW_TINT},
        {"vinyls", static_cast<eUnlockableEntity>(40)},
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
        if (entity == static_cast<eUnlockableEntity>(0x32)) {
            switch (upgradeLevel) {
            case 1:
                entity = static_cast<eUnlockableEntity>(0x2c);
                break;
            case 2:
                entity = static_cast<eUnlockableEntity>(0x2e);
                break;
            case 3:
                entity = static_cast<eUnlockableEntity>(0x30);
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
