#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

extern int UnlockAllThings;
extern char TheUnlockData[0x1c8];
extern char gMaxPartLevels[NUM_UNLOCKABLES];

bool GetIsCollectorsEdition();
eUnlockableEntity MapCarPartToUnlockable(int carslot, CarPart *part);
eUnlockableEntity MapPerfPkgToUnlockable(Physics::Upgrades::Type type);
const FECarPartInfo *LookupFEPartInfo(eUnlockableEntity unlockable, int level);

// ============================================================
// Free functions
// ============================================================

void DefaultUnlockData() {
    bMemSet(&TheUnlockData, 0, 0x1c8);
    bMemSet(&gMaxPartLevels, 0, NUM_UNLOCKABLES);
    gMaxPartLevels[4] = 3;   // PUT_TIRES
    gMaxPartLevels[5] = 4;   // PUT_BRAKES
    gMaxPartLevels[6] = 3;   // PUT_CHASSIS
    gMaxPartLevels[7] = 4;   // PUT_TRANSMISSION
    gMaxPartLevels[8] = 4;   // PUT_ENGINE
    gMaxPartLevels[9] = 3;   // PUT_INDUCTION
    gMaxPartLevels[10] = 3;  // PUT_NOS
    gMaxPartLevels[11] = 4;  // BODY_KIT
    gMaxPartLevels[12] = 5;  // SPOILERS
    gMaxPartLevels[13] = 6;  // RIM_BRANDS
    gMaxPartLevels[14] = 6;  // HOODS
    gMaxPartLevels[15] = 6;  // ROOF_SCOOPS
    gMaxPartLevels[17] = 4;  // CUSTOM_HUD
    gMaxPartLevels[18] = 4;  // WINDOW_TINT
    gMaxPartLevels[23] = 3;  // PAINTABLE_BODY
    gMaxPartLevels[40] = 6;  // VINYLS_GROUP_BODY
}

void UnlockUnlockableThing(eUnlockableEntity entity, unsigned int filter, int level, const char *part_name) {
    level = bMax(level, 0);
    if (filter & 1) {
        TheUnlockData[static_cast<int>(entity) * 8 + 5] = static_cast<char>(level);
        TheUnlockData[static_cast<int>(entity) * 8 + 4] = static_cast<char>(level);
    } else if (filter & 2) {
        TheUnlockData[static_cast<int>(entity) * 8 + 5] = static_cast<char>(level);
        TheUnlockData[static_cast<int>(entity) * 8 + 1] = static_cast<char>(level);
        TheUnlockData[static_cast<int>(entity) * 8 + 4] = static_cast<char>(level);
        TheUnlockData[static_cast<int>(entity) * 8 + 0] = static_cast<char>(level);
    }
}

// ============================================================
// QuickRaceUnlocker
// ============================================================

bool QuickRaceUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level, int player) {
    return false;
}

int QuickRaceUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, int player, bool backroom) {
    bool answer = level <= TheUnlockData[static_cast<int>(ent) * 8 + 4]
        | UnlockAllThings
        | FEDatabase->GetUserProfile(0)->GetCareer()->HasBeatenCareer()
        | FEDatabase->GetUserProfile(player)->CareerModeHasBeenCompletedAtLeastOnce;
    return answer;
}

int QuickRaceUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    return false;
}

int QuickRaceUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    bool answer = UnlockAllThings != 0;
    eUnlockableEntity unlockable = MapPerfPkgToUnlockable(pkg_type);
    int unlocked = QuickRaceUnlocker::IsUnlockableUnlocked(filter, unlockable, level, player, false);
    return answer | unlocked;
}

bool QuickRaceUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    bool answer = UnlockAllThings != 0;
    bool raceUnlocked = GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_QuickRace);
    unsigned int tutorialHash = Attrib::StringHash32("19.8.31");
    return static_cast<bool>(event_hash == static_cast<int>(tutorialHash) | answer | raceUnlocked);
}

// ============================================================
// OnlineUnlocker
// ============================================================

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
    bool answer;
    answer = UnlockAllThings;
    answer = answer | GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_QuickRace);
    answer = answer | GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_Online);
    return answer;
}

bool OnlineUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car) {
    return QuickRaceUnlocker::IsCarUnlocked(filter, car, 0);
}

bool OnlineUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    return QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0);
}

// ============================================================
// CareerUnlocker
// ============================================================

bool CareerUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom) {
    bool answer = level <= TheUnlockData[static_cast<int>(ent) * 8]
        | UnlockAllThings
        | FEDatabase->GetUserProfile(0)->GetCareer()->HasBeatenCareer()
        | FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce;

    if (!backroom) return answer;

    answer = level <= TheUnlockData[static_cast<int>(ent) * 8] + 1 | answer;

    if (TheUnlockData[static_cast<int>(ent) * 8] != gMaxPartLevels[static_cast<int>(ent)])
        return answer;

    return level <= 7 | answer;
}

bool CareerUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom) {
    bool answer = UnlockAllThings != 0;
    eUnlockableEntity unlockable = MapPerfPkgToUnlockable(pkg_type);
    bool unlocked = CareerUnlocker::IsUnlockableUnlocked(filter, unlockable, level, backroom);
    return static_cast<bool>(answer | unlocked);
}

bool CareerUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = UnlockAllThings != 0;
    bool raceUnlocked = GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_Career);
    return static_cast<bool>(answer | raceUnlocked);
}

// ============================================================
// UnlockSystem dispatchers
// ============================================================

bool UnlockSystem::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity thing, int level, int player, bool backroom) {
    if (UnlockAllThings) return true;
    bool answer = false;
    if (filter & 1) {
        answer = QuickRaceUnlocker::IsUnlockableUnlocked(filter, thing, level, player, backroom) != 0;
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
    if (UnlockAllThings) return true;
    bool answer = false;
    if (filter & 1) {
        answer = QuickRaceUnlocker::IsCarPartUnlocked(filter, carslot, part, player, backroom) != 0;
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
    if (UnlockAllThings) return true;
    bool answer = false;
    if (filter & 1) {
        answer = QuickRaceUnlocker::IsPerfPackageUnlocked(filter, pkg_type, level, player, backroom) != 0;
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
    if (UnlockAllThings) return true;
    bool answer = false;
    if (filter & 1) {
        answer = QuickRaceUnlocker::IsTrackUnlocked(filter, event_hash, player);
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
    if (UnlockAllThings) return true;
    bool answer = false;
    if (filter & 1) {
        answer = QuickRaceUnlocker::IsCarUnlocked(filter, handle, player);
    }
    if (filter & 2) {
        answer = static_cast<bool>(answer | CareerUnlocker::IsCarUnlocked(filter, handle));
    }
    if (filter & 4) {
        answer = static_cast<bool>(answer | OnlineUnlocker::IsCarUnlocked(filter, handle));
    }
    bool ceBonus = false;
    if (GetIsCollectorsEdition() && UnlockSystem::IsBonusCarCEOnly(handle)) {
        ceBonus = true;
    }
    return static_cast<bool>(answer | ceBonus);
}

bool UnlockSystem::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level) {
    bool answer = false;
    if (filter & 1) {
        answer = QuickRaceUnlocker::IsBackroomAvailable(filter, ent, level, 0);
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
    if (level == -2) {
        char newFlag;
        if (filter & 1) {
            newFlag = TheUnlockData[static_cast<int>(ent) * 8 + 5];
        } else if (filter & 2) {
            newFlag = TheUnlockData[static_cast<int>(ent) * 8 + 1];
        } else {
            newFlag = TheUnlockData[static_cast<int>(ent) * 8 + 5];
        }
        return newFlag != -1;
    }
    if (filter & 2) {
        return TheUnlockData[static_cast<int>(ent) * 8 + 1] == level;
    }
    return TheUnlockData[static_cast<int>(ent) * 8 + 5] == level;
}

void UnlockSystem::ClearNewUnlock(eUnlockableEntity ent, unsigned int filter) {
    if (filter & 1) {
        TheUnlockData[static_cast<int>(ent) * 8 + 5] = -1;
    }
    if (filter & 2) {
        TheUnlockData[static_cast<int>(ent) * 8 + 1] = -1;
    }
}

// ============================================================
// FEMarkerManager
// ============================================================

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
    case 0x17: marker = MARKER_BODY; break;
    case 0x3f: marker = MARKER_HOOD; break;
    case 0x2c: marker = MARKER_SPOILER; break;
    case 0x42: marker = MARKER_RIMS; break;
    case 0x3e: marker = MARKER_ROOF_SCOOP; break;
    case 0x84: marker = MARKER_CUSTOM_HUD; break;
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
    case 0x7b: marker = MARKER_DECAL; break;
    case 0x4d: marker = MARKER_VINYL; break;
    case 0x4c: marker = MARKER_PAINT; break;
    }
    UtilizeMarker(marker, 0);
}

void FEMarkerManager::UtilizeMarker(Physics::Upgrades::Type type) {
    ePossibleMarker marker = MARKER_NONE;
    switch (type) {
    case Physics::Upgrades::kType_Engine: marker = MARKER_TIRES; break;
    case Physics::Upgrades::kType_Transmission: marker = MARKER_BRAKES; break;
    case Physics::Upgrades::kType_Chassis: marker = MARKER_CHASSIS; break;
    case Physics::Upgrades::kType_Nitrous: marker = MARKER_TRANSMISSION; break;
    case Physics::Upgrades::kType_Tires: marker = MARKER_ENGINE; break;
    case Physics::Upgrades::kType_Brakes: marker = MARKER_INDUCTION; break;
    case Physics::Upgrades::kType_Induction: marker = MARKER_NOS; break;
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

char *FEMarkerManager::SaveToBuffer(char *buffer) {
    bMemCpy(buffer, this, 0x2F4);
    return buffer + 0x2F4;
}

char *FEMarkerManager::LoadFromBuffer(char *buffer) {
    bMemCpy(this, buffer, 0x2F4);
    return buffer + 0x2F4;
}