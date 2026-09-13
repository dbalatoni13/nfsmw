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

// UNSOLVED
bool DoesCategoryHaveNewUnlock(eUnlockableEntity entity) {
    bool answer = false;

    switch (entity) {
        case UNLOCKABLE_THING_CUSTOMIZE_PARTS:
            answer = TheUnlockData[UNLOCKABLE_THING_BODY_KIT].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_SPOILERS].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_RIM_BRANDS].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_ROOF_SCOOPS].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_HOODS].CareerIsNewPart != UNLOCK_IS_OLD;
            break;
        case UNLOCKABLE_THING_UNKNOWN:
            return false;
            break;
        case UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE:
            answer = TheUnlockData[UNLOCKABLE_THING_PUT_TIRES].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PUT_BRAKES].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PUT_CHASSIS].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PUT_TRANSMISSION].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PUT_ENGINE].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PUT_INDUCTION].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PUT_NOS].CareerIsNewPart != UNLOCK_IS_OLD;

            break;

        case UNLOCKABLE_THING_CUSTOMIZE_VISUAL:
            answer = TheUnlockData[UNLOCKABLE_THING_CUSTOM_HUD].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_DECAL_HOOD].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_DECAL_NUMBERS].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_PAINTABLE_BODY].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_THING_WINDOW_TINT].CareerIsNewPart != UNLOCK_IS_OLD ||
                     TheUnlockData[UNLOCKABLE_VINYLS_GROUP_BODY].CareerIsNewPart != UNLOCK_IS_OLD;

            break;
    }

    return answer;
}

// UNSOLVED
bool QuickRaceUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, int player, bool backroom) {
    bool answer;

    if (level <= TheUnlockData[ent].QuickRaceUnlockLevel) {
        answer = true;
    } else if (UnlockAllThings) {
        answer = true;
#ifndef EA_BUILD_A124
    } else if (FEDatabase->GetCareerSettings()->HasBeatenCareer()) {
        answer = true;
#endif
    } else if (FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce) {
        answer = true;
    }

    return answer;
}

// UNSOLVED
bool QuickRaceUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    bool answer = UnlockAllThings != 0;

    answer = answer || QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapCarPartToUnlockable(carslot, part), part->GetUpgradeLevel(), player, false);
    answer = answer || part->GetUpgradeLevel() == 0;

    return answer;
}

// UNSOLVED
bool QuickRaceUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    bool answer = UnlockAllThings != 0;

    answer |= QuickRaceUnlocker::IsUnlockableUnlocked(filter, MapPerfPkgToUnlockable(pkg_type), level, player, false);

    return answer;
}

// UNSOLVED
bool QuickRaceUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    bool answer = UnlockAllThings != 0;
    answer = answer | GRaceDatabase::Get().IsQuickRaceUnlocked(event_hash);
    if (event_hash == Attrib::StringHash32("19.8.31")) {
        return true;
    }
    return answer;
}

// UNSOLVED
bool QuickRaceUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car, int player) {
    bool answer = UnlockAllThings != 0;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(player);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);

    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    answer = static_cast<bool>(FEDatabase->GetCareerSettings()->GetCurrentBin() <= CarAttribs.UnlockedAt() | answer);

    if (fe_car->MatchesFilter(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_STOCK | FE_CAR_FILTER_LIST_CAREER | FE_CAR_FILTER_LIST_QUICK_RACE)) {
        int type = fe_car->GetType();
        if (type < 0x19) {
            if (type >= 0x17 || type <= 4 || type == 5 || type == 6 || type == 8) {
                answer = true;
            }
        } else if (type < 0x45) {
            if (type > 0x42 || type == 0x2F || type == 0x3E) {
                answer = true;
            }
        } else if (type == 0x4A) {
            answer = true;
        }
        return answer;
    }
    if (!fe_car->MatchesFilter(FE_CAR_FILTER_REGION_ALL | FE_CAR_FILTER_LIST_BONUS)) {
        return answer;
    }

    unsigned char currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    unsigned int handle = fe_car->Handle;
    if (handle == STRINGHASH_CE_ELISE) {
        return GetIsCollectorsEdition();
    }
    if (handle < 0x2D642B9) {
        if (handle != 0x9665) {
            if (handle > 0x9665) {
                if (handle == 0x136250) {
                    return currentBin < 0xC;
                }
                if (handle < 0x136251) {
                    if (handle == 0x13624E) {
                        return currentBin < 10;
                    }
                    if (handle < 0x13624F) {
                        if (handle == 0x9666) {
                            return currentBin < 9;
                        }
                    } else {
                        return currentBin < 0xB;
                    }
                } else if (handle == 0x136252) {
                    return currentBin < 0xE;
                } else if (handle < 0x136252) {
                    return currentBin < 0xD;
                } else if (handle == 0x136253) {
                    return currentBin < 0xF;
                }
                return false;
            }
            if (handle == 0x9661) {
                return currentBin < 4;
            }
            if (handle > 0x9661) {
                if (handle == 0x9663) {
                    return currentBin < 6;
                }
                if (handle < 0x9664) {
                    return currentBin < 5;
                }
                return currentBin < 7;
            }
            if (handle == 0x965F) {
                return currentBin < 2;
            }
            if (handle != 0x9660 || currentBin > 2) {
                return false;
            }
            return true;
        }
        return currentBin < 8;
    }
    if (handle == STRINGHASH_CE_GTRSTREET) {
        return GetIsCollectorsEdition();
    }
    if (handle > STRINGHASH_CE_GTRSTREET) {
        if (handle != STRINGHASH_CE_CORVETTE) {
            if (handle < 0x634D1BD3) {
                if (handle != STRINGHASH_CE_GT2) {
                    if (handle < 0x54655134) {
                        if (handle != STRINGHASH_CE_C6R) {
                            return false;
                        }
                    } else if (handle != STRINGHASH_CE_CAMARO) {
                        return false;
                    }
                }
            } else if (handle != STRINGHASH_CE_997S) {
                if (handle < 0xE1075863) {
                    if (handle != 0xCB6AAF2F) {
                        return false;
                    }
                    return (FEDatabase->GetCareerSettings()->SpecialFlags & 0x8000) != 0;
                }
                if (handle != STRINGHASH_CE_SL65) {
                    return false;
                }
            }
        }
        return GetIsCollectorsEdition();
    }
    if (handle == STRINGHASH_CE_SUPRA) {
        return GetIsCollectorsEdition();
    }
    if (handle < 0x3D8A6D2) {
        if (handle == 0x3A94520) {
#ifndef EA_BUILD_A124
            return FEDatabase->GetCareerSettings()->HasBeatenCareer();
#endif
        }
        if (handle != STRINGHASH_CE_SL500) {
            return false;
        }
        return GetIsCollectorsEdition();
    }
    if (handle == 0x2CF385B2) {
        return (FEDatabase->GetCareerSettings()->SpecialFlags & 1) != 0;
    }
    if (handle < 0x2CF385B3) {
        if (handle != 0x2CF370F0) {
            return false;
        }
#ifndef EA_BUILD_A124
        return FEDatabase->GetCareerSettings()->HasBeatenCareer();
#endif
    }
    if (handle != 0x34498EB2) {
        return false;
    }
    return (FEDatabase->GetCareerSettings()->SpecialFlags & 0x40000) != 0;
}

bool QuickRaceUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level, int player) {
    bool answer = false;
    return answer;
}

// UNSOLVED
bool OnlineUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom) {
    bool answer;
    answer = QuickRaceUnlocker::IsUnlockableUnlocked(filter, ent, level, 0, backroom) != false;
    return answer;
}

// UNSOLVED
bool OnlineUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer;
    answer = QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, 0, backroom) != false;
    return answer;
}

// UNSOLVED
bool OnlineUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer;
    answer = QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, 0, backroom) != false;
    return answer;
}

// UNSOLVED
bool OnlineUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = true;
    if (UnlockAllThings == 0) {
        answer = false;
    }
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

// UNSOLVED
bool CareerUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom) {
    bool answer = (level <= TheUnlockData[ent].CareerUnlockLevel) | UnlockAllThings
#ifndef EA_BUILD_A124
                  | FEDatabase->GetCareerSettings()->HasBeatenCareer()
#endif
                  | FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce;

    if (!backroom)
        return answer;

    answer = (level <= TheUnlockData[ent].QuickRaceUnlockLevel) | answer;

    if (TheUnlockData[ent].CareerUnlockLevel != gMaxPartLevels[ent])
        return answer;

    return (level <= 7) | answer;
}

// UNSOLVED
bool CareerUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer = UnlockAllThings != 0;
    eUnlockableEntity unlockable = MapCarPartToUnlockable(carslot, part);
    int unlocked = CareerUnlocker::IsUnlockableUnlocked(filter, unlockable, part->GetUpgradeLevel(), backroom);
    return (part->GetUpgradeLevel() == 0 || answer) || unlocked != 0;
}

// UNSOLVED
bool CareerUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer = UnlockAllThings != 0;
    eUnlockableEntity unlockable = MapPerfPkgToUnlockable(pkg_type);
    return static_cast<bool>(answer | CareerUnlocker::IsUnlockableUnlocked(filter, unlockable, level, backroom));
}

// UNSOLVED
bool CareerUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = true;
    if (UnlockAllThings == 0) {
        answer = false;
    }
    bool raceUnlocked = GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_Career);
    return static_cast<bool>(answer | raceUnlocked);
}

// UNSOLVED
bool CareerUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car) {
    bool answer = UnlockAllThings != 0;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    unsigned char unlockedAt = CarAttribs.UnlockedAt();
    unsigned char currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    return (currentBin >= unlockedAt) | answer;
}

// UNSOLVED
bool CareerUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    FEMarkerManager::ePossibleMarker marker = FEMarkerManager::MARKER_NONE;
    if (ent != UNLOCKABLE_THING_SPOILERS) {
        if (ent > UNLOCKABLE_THING_SPOILERS) {
            if (ent < UNLOCKABLE_THING_PAINTABLE_RIMS) {
                if (ent > UNLOCKABLE_THING_WINDOW_TINT) {
                    marker = FEMarkerManager::MARKER_PAINT;
                    goto marker_check;
                }
                if (ent == UNLOCKABLE_THING_HOODS) {
                    marker = FEMarkerManager::MARKER_HOOD;
                    goto marker_check;
                }
                if (ent > UNLOCKABLE_THING_RIM_BRANDS) {
                    if (ent == UNLOCKABLE_THING_ROOF_SCOOPS) {
                        marker = FEMarkerManager::MARKER_ROOF_SCOOP;
                    } else {
                        if (ent != UNLOCKABLE_THING_CUSTOM_HUD) {
                            return false;
                        }
                        marker = FEMarkerManager::MARKER_CUSTOM_HUD;
                    }
                    goto marker_check;
                }
            } else {
                if (ent > UNLOCKABLE_VINYLS_GROUP_CONTEST) {
                    if (ent > UNLOCKABLE_DECAL_SLOT_6 || ent < UNLOCKABLE_DECAL_WINDSHIELD) {
                        return false;
                    }
                    marker = FEMarkerManager::MARKER_DECAL;
                    goto marker_check;
                }
                if (ent > UNLOCKABLE_THING_RIM_BRAND_ROJA) {
                    marker = FEMarkerManager::MARKER_VINYL;
                    goto marker_check;
                }
                if (ent < UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN) {
                    return false;
                }
            }
            marker = FEMarkerManager::MARKER_RIMS;
        } else {
            if (ent != UNLOCKABLE_THING_PUT_CHASSIS) {
                if (ent > UNLOCKABLE_THING_PUT_CHASSIS) {
                    if (ent != UNLOCKABLE_THING_PUT_INDUCTION) {
                        if (ent < UNLOCKABLE_THING_PUT_INDUCTION) {
                            if (ent == UNLOCKABLE_THING_PUT_TRANSMISSION) {
                                marker = FEMarkerManager::MARKER_TRANSMISSION;
                            } else {
                                if (ent != UNLOCKABLE_THING_PUT_ENGINE) {
                                    return false;
                                }
                                marker = FEMarkerManager::MARKER_ENGINE;
                            }
                            goto marker_check;
                        }
                        if (ent != UNLOCKABLE_THING_PUT_NOS) {
                            if (ent != UNLOCKABLE_THING_BODY_KIT) {
                                return false;
                            }
                            marker = FEMarkerManager::MARKER_BODY;
                            goto marker_check;
                        }
                        marker = FEMarkerManager::MARKER_NOS;
                        goto marker_check;
                    }
                    marker = FEMarkerManager::MARKER_INDUCTION;
                    goto marker_check;
                }
                if (ent < UNLOCKABLE_THING_PUT_TRANSMISSION) {
                    if (ent != UNLOCKABLE_THING_CUSTOMIZE_VISUAL) {
                        if (ent > UNLOCKABLE_THING_CUSTOMIZE_VISUAL) {
                            if (ent == UNLOCKABLE_THING_PUT_TIRES) {
                                marker = FEMarkerManager::MARKER_TIRES;
                            } else {
                                if (ent != UNLOCKABLE_THING_PUT_BRAKES) {
                                    return false;
                                }
                                marker = FEMarkerManager::MARKER_BRAKES;
                            }
                            goto marker_check;
                        }
                        if (ent == UNLOCKABLE_THING_CUSTOMIZE_PARTS) {
                            answer = CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_BODY_KIT, level);
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_SPOILERS, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRANDS, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_HOODS, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_ROOF_SCOOPS, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_CUSTOM_HUD, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN, level));
                        } else {
                            if (ent != UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE) {
                                return false;
                            }
                            answer = CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TIRES, level);
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_BRAKES, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_CHASSIS, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TRANSMISSION, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_ENGINE, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_INDUCTION, level));
                            answer = static_cast<bool>(answer) |
                                     static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_NOS, level));
                        }
                    } else {
                        answer = CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PAINT_METALLIC, level);
                        answer = static_cast<bool>(answer) |
                                 static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_VINYLS_GROUP_FLAME, level));
                        answer = static_cast<bool>(answer) |
                                 static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_DECAL_WINDSHIELD, level));
                    }
                }
            } else {
                marker = FEMarkerManager::MARKER_CHASSIS;
                goto marker_check;
            }
        }
    } else {
        marker = FEMarkerManager::MARKER_SPOILER;
        goto marker_check;
    }

marker_check:
    return static_cast<bool>(answer) | static_cast<bool>(TheFEMarkerManager.IsMarkerAvailable(marker, 0));
}

// UNSOLVED
bool UnlockSystem::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity thing, int level, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer = QuickRaceUnlocker::IsUnlockableUnlocked(filter, thing, level, player, backroom) != 0;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsUnlockableUnlocked(filter, thing, level, backroom));
    }
    if (filter & UNLOCK_ONLINE) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsUnlockableUnlocked(filter, thing, level, backroom));
    }
    return answer;
}

// UNSOLVED
bool UnlockSystem::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer = QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, player, backroom) != 0;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom));
    }
    if (filter & UNLOCK_ONLINE) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsCarPartUnlocked(filter, carslot, part, backroom));
    }
    return answer;
}

// UNSOLVED
bool UnlockSystem::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer = QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, player, backroom) != 0;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom));
    }
    if (filter & UNLOCK_ONLINE) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, backroom));
    }
    return answer;
}

// UNSOLVED
bool UnlockSystem::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer = QuickRaceUnlocker::IsTrackUnlocked(filter, event_hash, player) != 0;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsTrackUnlocked(filter, event_hash));
    }
    if (filter & UNLOCK_ONLINE) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsTrackUnlocked(filter, event_hash));
    }
    return answer;
}

// UNSOLVED
bool UnlockSystem::IsCarUnlocked(eUnlockFilters filter, unsigned int handle, int player) {
    if (UnlockAllThings)
        return true;
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer = QuickRaceUnlocker::IsCarUnlocked(filter, handle, player);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsCarUnlocked(filter, handle));
    }
    if (filter & UNLOCK_ONLINE) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsCarUnlocked(filter, handle));
    }
    if (GetIsCollectorsEdition() && UnlockSystem::IsBonusCarCEOnly(handle)) {
        answer = true;
    }
    return answer;
}

// UNSOLVED
bool UnlockSystem::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    if (filter & UNLOCK_QUICK_RACE) {
        answer = QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0);
    }
    if (filter & UNLOCK_CAREER_MODE) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsBackroomAvailable(filter, ent, level));
    }
    if (filter & UNLOCK_ONLINE) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsBackroomAvailable(filter, ent, level));
    }
    return answer;
}

// UNSOLVED
bool UnlockSystem::IsUnlockableNew(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    if (level == UNLOCK_LEVEL_ANY) {
        if (filter & UNLOCK_QUICK_RACE) {
            if ((filter & UNLOCK_CAREER_MODE) == 0) {
                return TheUnlockData[ent].QuickRaceIsNewPart == level;
            }
            if (TheUnlockData[ent].QuickRaceIsNewPart != UNLOCK_IS_OLD) {
                return true;
            }
        }
        if (filter & UNLOCK_CAREER_MODE) {
            if (TheUnlockData[ent].CareerIsNewPart != UNLOCK_IS_OLD) {
                return true;
            }
        }
        return false;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        return TheUnlockData[ent].CareerIsNewPart == level;
    }
    return TheUnlockData[ent].QuickRaceIsNewPart == level;
}

// UNSOLVED
void UnlockSystem::ClearNewUnlock(eUnlockableEntity ent, uint32 filter) {
    if (filter & UNLOCK_QUICK_RACE) {
        TheUnlockData[ent].QuickRaceIsNewPart = UNLOCK_IS_OLD;
    }
    if (filter & UNLOCK_CAREER_MODE) {
        TheUnlockData[ent].CareerIsNewPart = UNLOCK_IS_OLD;
    }
}
// UNSOLVED
// 0x8017B688: d:/mw/speed/indep/src/frontend/careers/UnlockSystem.cpp (line 773)
eUnlockableEntity MapCarPartToUnlockable(int carslot, CarPart *part) {
    switch (carslot) {
        case CARSLOTID_BODY:
            return UNLOCKABLE_THING_BODY_KIT;
        case CARSLOTID_SPOILER:
            return UNLOCKABLE_THING_SPOILERS;
        case CARSLOTID_ROOF:
            return UNLOCKABLE_THING_ROOF_SCOOPS;
        case CARSLOTID_HOOD:
            return UNLOCKABLE_THING_HOODS;
        case CARSLOTID_FRONT_WHEEL:
            return UNLOCKABLE_THING_RIM_BRANDS;
        case CARSLOTID_REAR_WHEEL:
            return UNLOCKABLE_THING_RIM_BRANDS;
        case CARSLOTID_LICENSE_PLATE:
            return UNLOCKABLE_THING_LICENSE_PLATE;
        case CARSLOTID_BASE_PAINT:
            return UNLOCKABLE_THING_PAINTABLE_BODY;
        case CARSLOTID_VINYL_LAYER0:
            return UNLOCKABLE_VINYLS_GROUP_BODY;
        case CARSLOTID_PAINT_RIM:
            return UNLOCKABLE_THING_PAINTABLE_BODY;
        case CARSLOTID_DECAL_FRONT_WINDOW_TEX0:
            return UNLOCKABLE_DECAL_REAR_WINDOW;
        case CARSLOTID_DECAL_REAR_WINDOW_TEX0:
            return UNLOCKABLE_DECAL_REAR_WINDOW;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX0:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX1:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX2:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX3:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX4:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX5:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX6:
            return UNLOCKABLE_DECAL_NUMBERS;
        case CARSLOTID_DECAL_LEFT_DOOR_TEX7:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX0:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX1:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX2:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX3:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX4:
            return UNLOCKABLE_DECAL_LEFT_DOOR;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX5:
            return UNLOCKABLE_DECAL_NUMBERS;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX6:
            return UNLOCKABLE_DECAL_LEFT_QP;
        case CARSLOTID_DECAL_RIGHT_DOOR_TEX7:
            return UNLOCKABLE_DECAL_LEFT_QP;
        case CARSLOTID_WINDOW_TINT:
            return UNLOCKABLE_THING_WINDOW_TINT;
        case CARSLOTID_CUSTOM_HUD:
            return UNLOCKABLE_THING_CUSTOM_HUD;
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
            return GetIsCollectorsEdition() != false;
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

// UNSOLVED
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
        if (bStrICmp(marker_name, unlockType[i].MarkerName) == 0 &&
            ((unlockType[i].PartName == nullptr) || bStrICmp(partid, unlockType[i].PartName) == 0)) {
            return unlockType[i].Marker;
        }
    }

    return MARKER_NONE;
}

// UNSOLVED
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
                param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin(), 0);
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
