#include "Speed/Indep/Src/Frontend/Careers/UnlockSystem.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceDatabase.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/EasterEggs.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/gameplay.h"
#include "Speed/Indep/Src/Misc/MD5.hpp"

extern int UnlockAllThings;
extern char TheUnlockData[0x1c8];
extern char gMaxPartLevels[NUM_UNLOCKABLES];
extern EasterEggs gEasterEggs;
extern bool gVerboseTesterOutput;
extern int SkipFE;
extern int SkipFESplitScreen;

void GetLocalizedString(char *buf, unsigned int maxlen, unsigned int hash);
unsigned int FEngHashString(const char *fmt, ...);

// GRaceDatabase inline methods (can't add bodies to header - DWARF crash)
inline GRaceSaveInfo *GRaceDatabase::GetScoreInfo() {
    return mRaceScoreInfo;
}

inline unsigned int GRaceDatabase::GetScoreInfoCount() {
    return mRaceCountStatic;
}

// total size: 0x10
#ifndef GRACESAVEINFO_DEFINED
#define GRACESAVEINFO_DEFINED
struct GRaceSaveInfo {
    unsigned int mRaceHash;                              // offset 0x0, size 0x4
    unsigned int mFlags;                                 // offset 0x4, size 0x4
    float mHighScores;                                   // offset 0x8, size 0x4
    unsigned short mTopSpeed;                            // offset 0xC, size 0x2
    unsigned short mAverageSpeed;                        // offset 0xE, size 0x2
};
#endif

#include "Speed/Indep/Src/World/CarPart.hpp"

bool GetIsCollectorsEdition();
eUnlockableEntity MapCarPartToUnlockable(int carslot, CarPart *part);
eUnlockableEntity MapPerfPkgToUnlockable(Physics::Upgrades::Type type);
const FECarPartInfo *LookupFEPartInfo(eUnlockableEntity unlockable, int level);
eUnlockableEntity ConvertBigBangUpgradeAward(const char *partname);
char *SaveSomeData(void *save_to, void *save_from, int bytes, void *maxptr);
char *LoadSomeData(void *load_to, void *load_from, int bytes, void *maxptr);

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
    int unlock_index;
    if (level < 0) {
        level = 0;
    }
    if (filter & 1) {
        char *secondary_data = TheUnlockData;
        char *primary_data = TheUnlockData;
        secondary_data += 4;
        primary_data += 5;
        unlock_index = static_cast<int>(entity) * 8;
        primary_data[unlock_index] = level;
        secondary_data[unlock_index] = level;
        return;
    }
    if ((filter & 2) == 0) {
        return;
    }
    char *primary_data = TheUnlockData;
    char *secondary_data = TheUnlockData;
    char *tertiary_data = TheUnlockData;
    primary_data += 5;
    secondary_data += 1;
    tertiary_data += 4;
    unlock_index = static_cast<int>(entity) * 8;
    primary_data[unlock_index] = level;
    secondary_data[unlock_index] = level;
    tertiary_data[unlock_index] = level;
    TheUnlockData[unlock_index] = level;
}

// ============================================================
// QuickRaceUnlocker
// ============================================================

bool QuickRaceUnlocker::IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level, int player) {
    bool answer = false;
    return answer;
}

int QuickRaceUnlocker::IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, int player, bool backroom) {
    return level <= TheUnlockData[static_cast<int>(ent) * 8 + 4]
        || UnlockAllThings != 0
        || FEDatabase->GetUserProfile(0)->GetCareer()->HasBeatenCareer()
        || FEDatabase->GetUserProfile(0)->CareerModeHasBeenCompletedAtLeastOnce;
}

int QuickRaceUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom) {
    bool answer = UnlockAllThings != 0;
    unsigned char group_and_level = part->GroupNumber_UpgradeLevel;
    eUnlockableEntity unlockable = MapCarPartToUnlockable(carslot, part);
    int unlocked = QuickRaceUnlocker::IsUnlockableUnlocked(filter, unlockable, part->GroupNumber_UpgradeLevel >> 5, player, false);
    return (group_and_level >> 5 == 0 || answer) || unlocked != 0;
}

int QuickRaceUnlocker::IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom) {
    bool answer = UnlockAllThings != 0;
    eUnlockableEntity unlockable = MapPerfPkgToUnlockable(pkg_type);
    int unlocked = QuickRaceUnlocker::IsUnlockableUnlocked(filter, unlockable, level, player, false);
    return answer | unlocked;
}

bool QuickRaceUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player) {
    bool answer = UnlockAllThings != 0;
    int raceUnlocked = GRaceDatabase::Get().CheckRaceScoreFlags(event_hash, GRaceDatabase::kUnlocked_QuickRace);
    answer = answer || raceUnlocked != 0;
    int tutorialHash = Attrib::StringHash32("19.8.31");
    if (event_hash == tutorialHash) {
        return true;
    }
    return answer;
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
    bool answer = true;
    if (UnlockAllThings == 0) {
        answer = false;
    }
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
    return static_cast<bool>(answer | CareerUnlocker::IsUnlockableUnlocked(filter, unlockable, level, backroom));
}

bool CareerUnlocker::IsTrackUnlocked(eUnlockFilters filter, int event_hash) {
    bool answer = true;
    if (UnlockAllThings == 0) {
        answer = false;
    }
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
        answer = QuickRaceUnlocker::IsTrackUnlocked(filter, event_hash, player) != 0;
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
    if (GetIsCollectorsEdition() && UnlockSystem::IsBonusCarCEOnly(handle)) {
        answer = true;
    }
    return answer;
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
        if (filter & 1) {
            char *new_flag = TheUnlockData;
            new_flag += 5;
            int unlock_index = static_cast<int>(ent) * 8;
            return new_flag[unlock_index] != -1;
        }
        if (filter & 2) {
            char *new_flag = TheUnlockData;
            new_flag += 1;
            int unlock_index = static_cast<int>(ent) * 8;
            return new_flag[unlock_index] != -1;
        }
        char *new_flag = TheUnlockData;
        new_flag += 5;
        int unlock_index = static_cast<int>(ent) * 8;
        return new_flag[unlock_index] != -1;
    }
    if (filter & 2) {
        char *new_flag = TheUnlockData;
        new_flag += 1;
        int unlock_index = static_cast<int>(ent) * 8;
        return new_flag[unlock_index] == level;
    }
    char *new_flag = TheUnlockData;
    new_flag += 5;
    int unlock_index = static_cast<int>(ent) * 8;
    return new_flag[unlock_index] == level;
}

void UnlockSystem::ClearNewUnlock(eUnlockableEntity ent, unsigned int filter) {
    if (filter & 1) {
        char *unlock_data = TheUnlockData;
        unlock_data += 5;
        int unlock_index = static_cast<int>(ent) * 8;
        unlock_data[unlock_index] = -1;
    }
    if ((filter & 2) == 0) {
        return;
    }
    char *unlock_data = TheUnlockData;
    unlock_data += 1;
    int unlock_index = static_cast<int>(ent) * 8;
    unlock_data[unlock_index] = -1;
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
    int nSize = GetSaveBufferSize();
    bMemCpy(buffer, this, nSize);
    return buffer + nSize;
}

char *FEMarkerManager::LoadFromBuffer(char *buffer) {
    int nSize = GetSaveBufferSize();
    bMemCpy(this, buffer, nSize);
    return buffer + nSize;
}

// ============================================================
// CareerSettings Save/Load unlock data
// ============================================================

char *CareerSettings::SaveUnlockData(void *save_to, void *maxptr) {
    char *buf = static_cast<char *>(save_to);
    for (unsigned int i = 0; i < 0x39; i++) {
        buf = SaveSomeData(buf, &TheUnlockData[i * 8], 8, maxptr);
    }
    return buf;
}

char *CareerSettings::LoadUnlockData(void *load_from, void *maxptr) {
    char *buf = static_cast<char *>(load_from);
    for (unsigned int i = 0; i < 0x39; i++) {
        buf = LoadSomeData(&TheUnlockData[i * 8], buf, 8, maxptr);
    }
    return buf;
}

// ============================================================
// More free functions
// ============================================================

void MarkUnlockableThingSeen(eUnlockableEntity entity, unsigned int filter) {
    char count;
    if (filter & 1) {
        int unlock_index = static_cast<int>(entity) * 8;
        count = TheUnlockData[unlock_index + 6];
        TheUnlockData[unlock_index + 6] = count + 1;
        if (static_cast<char>(count + 1) < 4) {
            return;
        }
        TheUnlockData[unlock_index + 6] = 0;
        TheUnlockData[unlock_index + 5] = -1;
        return;
    }
    if ((filter & 2) == 0) {
        return;
    }
    int unlock_index = static_cast<int>(entity) * 8;
    count = TheUnlockData[unlock_index + 2];
    TheUnlockData[unlock_index + 2] = count + 1;
    if (static_cast<char>(count + 1) < 4) {
        return;
    }
    TheUnlockData[unlock_index + 1] = -1;
    TheUnlockData[unlock_index + 2] = 0;
}

eUnlockableEntity MapPerfPkgToUnlockable(Physics::Upgrades::Type pkg_type) {
    switch (pkg_type) {
    case Physics::Upgrades::kType_Tires: return UNLOCKABLE_THING_PUT_TIRES;
    case Physics::Upgrades::kType_Brakes: return UNLOCKABLE_THING_PUT_BRAKES;
    case Physics::Upgrades::kType_Chassis: return UNLOCKABLE_THING_PUT_CHASSIS;
    case Physics::Upgrades::kType_Transmission: return UNLOCKABLE_THING_PUT_TRANSMISSION;
    case Physics::Upgrades::kType_Engine: return UNLOCKABLE_THING_PUT_ENGINE;
    case Physics::Upgrades::kType_Induction: return UNLOCKABLE_THING_PUT_INDUCTION;
    case Physics::Upgrades::kType_Nitrous: return UNLOCKABLE_THING_PUT_NOS;
    }
    return UNLOCKABLE_THING_UNKNOWN;
}

eUnlockableEntity MapCarPartToUnlockable(int carslot, CarPart *part) {
    switch (carslot) {
    case 0x17: return UNLOCKABLE_THING_BODY_KIT;
    case 0x2c: return UNLOCKABLE_THING_SPOILERS;
    case 0x3e: return UNLOCKABLE_THING_ROOF_SCOOPS;
    case 0x3f: return UNLOCKABLE_THING_HOODS;
    case 0x42: return UNLOCKABLE_THING_RIM_BRANDS;
    case 0x43: return UNLOCKABLE_THING_RIM_BRANDS;
    case 0x45: return UNLOCKABLE_THING_LICENSE_PLATE;
    case 0x4c: return UNLOCKABLE_THING_PAINTABLE_BODY;
    case 0x4d: return UNLOCKABLE_VINYLS_GROUP_BODY;
    case 0x4e: return UNLOCKABLE_THING_PAINTABLE_BODY;
    case 0x53: return UNLOCKABLE_DECAL_REAR_WINDOW;
    case 0x5b: return UNLOCKABLE_DECAL_REAR_WINDOW;
    case 99: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 100: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x65: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x66: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x67: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x68: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x69: return UNLOCKABLE_DECAL_NUMBERS;
    case 0x6b: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x6c: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x6d: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x6e: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x6f: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x70: return UNLOCKABLE_DECAL_LEFT_DOOR;
    case 0x71: return UNLOCKABLE_DECAL_NUMBERS;
    case 0x73: return UNLOCKABLE_DECAL_LEFT_QP;
    case 0x7b: return UNLOCKABLE_DECAL_LEFT_QP;
    case 0x83: return UNLOCKABLE_THING_WINDOW_TINT;
    case 0x84: return UNLOCKABLE_THING_CUSTOM_HUD;
    default: return UNLOCKABLE_THING_UNKNOWN;
    }
}

const FECarPartInfo *LookupFEPartInfo(eUnlockableEntity unlockable, int upgrade_level) {
    const Attrib::Class *feclass = Attrib::Database::Get().GetClass(0x85885722);
    unsigned int key = feclass->GetFirstCollection();
    Attrib::Gen::frontend carparts(Attrib::StringToKey("carparts"), 0, nullptr);
    do {
        if (!key) {
            return nullptr;
        }
        Attrib::Gen::frontend part(key, 0, nullptr);
        if (part.GetParent() == Attrib::StringToKey("carparts")) {
            if (static_cast<int>(part.feCarPartName()) == static_cast<int>(unlockable)) {
                for (unsigned int i = 0; i < part.Num_feCarPartInfo(); i++) {
                    if (static_cast<int>(part.feCarPartInfo(i).Level) == upgrade_level) {
                        return &part.feCarPartInfo(i);
                    }
                }
            }
        }
        key = feclass->GetNextCollection(key);
    } while (true);
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
    if (event_hash == Attrib::StringHash32("21.1.1")
        || event_hash == Attrib::StringHash32("21.2.1")
        || event_hash == Attrib::StringHash32("21.2.2")
        || event_hash == Attrib::StringHash32("19.9.70")) {
        if (GetIsCollectorsEdition()) {
            return true;
        }
    } else {
        if (event_hash != Attrib::StringHash32("19.8.31")) {
            return true;
        }
        if (gEasterEggs.IsEasterEggUnlocked(EASTER_EGG_BURGER_KING)
            && !FEDatabase->GetCareerSettings()->HasBeenAwardedBKReward()) {
            return true;
        }
    }
    return false;
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

// ============================================================
// CareerUnlocker remaining
// ============================================================

bool CareerUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer = UnlockAllThings != 0;
    unsigned char group_and_level = part->GroupNumber_UpgradeLevel;
    eUnlockableEntity unlockable = MapCarPartToUnlockable(carslot, part);
    int unlocked = CareerUnlocker::IsUnlockableUnlocked(filter, unlockable, part->GroupNumber_UpgradeLevel >> 5, backroom);
    return (group_and_level >> 5 == 0 || answer) || unlocked != 0;
}

bool CareerUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car) {
    bool answer = UnlockAllThings != 0;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    unsigned char unlockedAt = CarAttribs.UnlockedAt();
    unsigned char currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    return currentBin >= unlockedAt | answer;
}

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
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_SPOILERS, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRANDS, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_HOODS, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_ROOF_SCOOPS, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_CUSTOM_HUD, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_RIM_BRAND_5_ZIGEN, level));
                        } else {
                            if (ent != UNLOCKABLE_THING_CUSTOMIZE_PERFORMANCE) {
                                return false;
                            }
                            answer = CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TIRES, level);
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_BRAKES, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_CHASSIS, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_TRANSMISSION, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_ENGINE, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_INDUCTION, level));
                            answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PUT_NOS, level));
                        }
                    } else {
                        answer = CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_THING_PAINT_METALLIC, level);
                        answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_VINYLS_GROUP_FLAME, level));
                        answer = static_cast<bool>(answer) | static_cast<bool>(CareerUnlocker::IsBackroomAvailable(filter, UNLOCKABLE_DECAL_WINDSHIELD, level));
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

// ============================================================
// QuickRaceUnlocker::IsCarUnlocked
// ============================================================

bool QuickRaceUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car, int player) {
    bool answer = UnlockAllThings != 0;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(player);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    {
        Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
        bool unlockedCheck = FEDatabase->GetCareerSettings()->GetCurrentBin() <= CarAttribs.UnlockedAt();
        answer = static_cast<bool>(unlockedCheck | answer);
    }
    if (fe_car->MatchesFilter(0xF0007)) {
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
    if (!fe_car->MatchesFilter(0xF0008)) {
        return answer;
    }

    unsigned char currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    unsigned int handle = fe_car->Handle;
    if (handle == 0x2D642B8) {
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
    if (handle == 0x363A1FEA) {
        return GetIsCollectorsEdition();
    }
    if (handle > 0x363A1FEA) {
        if (handle != 0x634D1BD2) {
            if (handle < 0x634D1BD3) {
                if (handle != 0x54655133) {
                    if (handle < 0x54655134) {
                        if (handle != 0x54653C71) {
                            return false;
                        }
                    } else if (handle != 0x582F21D9) {
                        return false;
                    }
                }
            } else if (handle != 0xE1075862) {
                if (handle < 0xE1075863) {
                    if (handle != 0xCB6AAF2F) {
                        return false;
                    }
                    return (FEDatabase->GetCareerSettings()->SpecialFlags & 0x8000) != 0;
                }
                if (handle != 0xE115EAD0) {
                    return false;
                }
            }
        }
        return GetIsCollectorsEdition();
    }
    if (handle == 0x3D8A6D1) {
        return GetIsCollectorsEdition();
    }
    if (handle < 0x3D8A6D2) {
        if (handle == 0x3A94520) {
            return FEDatabase->GetCareerSettings()->HasBeatenCareer();
        }
        if (handle != 0x3D3401A) {
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
        return FEDatabase->GetCareerSettings()->HasBeatenCareer();
    }
    if (handle != 0x34498EB2) {
        return false;
    }
    return (FEDatabase->GetCareerSettings()->SpecialFlags & 0x40000) != 0;
}

void ClearAllNewStatus() {
    for (int i = 0; i < 0x39; i++) {
        UnlockSystem::ClearNewUnlock(static_cast<eUnlockableEntity>(i), 2);
        UnlockSystem::ClearNewUnlock(static_cast<eUnlockableEntity>(i), 1);
    }
}

bool DoesCategoryHaveNewUnlock(eUnlockableEntity ent) {
    int category = static_cast<int>(ent);
    bool answer = false;
    bool bVar1;
    bool bVar2;
    bool bVar3;
    char cVar4;
    if (category == 2) {
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_BRAKES) * 8 + 1] == -1) {
            bVar1 = true;
            if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_TIRES) * 8 + 1] == -1) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_CHASSIS) * 8 + 1] == -1) {
            bVar2 = true;
            if (!bVar1) {
                bVar2 = false;
            }
        } else {
            bVar2 = true;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_TRANSMISSION) * 8 + 1] == -1) {
            bVar1 = true;
            if (!bVar2) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_ENGINE) * 8 + 1] == -1) {
            bVar2 = true;
            if (!bVar1) {
                bVar2 = false;
            }
        } else {
            bVar2 = true;
        }
        cVar4 = TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_NOS) * 8 + 1];
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PUT_INDUCTION) * 8 + 1] == -1) {
            bVar1 = true;
            if (!bVar2) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
    } else if (category < 3) {
        if (category != 1) {
            return answer;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_SPOILERS) * 8 + 1] == -1) {
            bVar1 = true;
            if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_BODY_KIT) * 8 + 1] == -1) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_RIM_BRANDS) * 8 + 1] == -1) {
            bVar2 = true;
            if (!bVar1) {
                bVar2 = false;
            }
        } else {
            bVar2 = true;
        }
        cVar4 = TheUnlockData[static_cast<int>(UNLOCKABLE_THING_ROOF_SCOOPS) * 8 + 1];
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_HOODS) * 8 + 1] == -1) {
            bVar1 = true;
            if (!bVar2) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
    } else {
        if (category != 3) {
            return answer;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_CUSTOM_HUD) * 8 + 1] == -1) {
            bVar1 = true;
            if (TheUnlockData[static_cast<int>(static_cast<eUnlockableEntity>(50)) * 8 + 1] == -1) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
        if (TheUnlockData[static_cast<int>(static_cast<eUnlockableEntity>(43)) * 8 + 1] == -1) {
            bVar2 = true;
            if (!bVar1) {
                bVar2 = false;
            }
        } else {
            bVar2 = true;
        }
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_PAINTABLE_BODY) * 8 + 1] == -1) {
            bVar3 = true;
            if (!bVar2) {
                bVar3 = false;
            }
        } else {
            bVar3 = true;
        }
        cVar4 = TheUnlockData[static_cast<int>(UNLOCKABLE_VINYLS_GROUP_BODY) * 8 + 1];
        if (TheUnlockData[static_cast<int>(UNLOCKABLE_THING_WINDOW_TINT) * 8 + 1] == -1) {
            bVar1 = true;
            if (!bVar3) {
                bVar1 = false;
            }
        } else {
            bVar1 = true;
        }
    }
    if (cVar4 == -1) {
        if (!bVar1) {
            return answer;
        }
        return true;
    }
    return true;
}

eUnlockableEntity ConvertBigBangUpgradeAward(const char *partname) {
    static struct {
        const char *mPartName;
        eUnlockableEntity mUnlockable;
    } unlockType[18] = {
        { "brakes", UNLOCKABLE_THING_PUT_BRAKES },
        { "chassis", UNLOCKABLE_THING_PUT_CHASSIS },
        { "engine", UNLOCKABLE_THING_PUT_ENGINE },
        { "induction", UNLOCKABLE_THING_PUT_INDUCTION },
        { "nos", UNLOCKABLE_THING_PUT_NOS },
        { "tires", UNLOCKABLE_THING_PUT_TIRES },
        { "transmission", UNLOCKABLE_THING_PUT_TRANSMISSION },
        { "bodykit", UNLOCKABLE_THING_BODY_KIT },
        { "decals", static_cast< eUnlockableEntity >(50) },
        { "hood", UNLOCKABLE_THING_HOODS },
        { "hud", UNLOCKABLE_THING_CUSTOM_HUD },
        { "numbers", static_cast< eUnlockableEntity >(43) },
        { "paint", UNLOCKABLE_THING_PAINTABLE_BODY },
        { "rims", UNLOCKABLE_THING_RIM_BRANDS },
        { "roofscoop", UNLOCKABLE_THING_ROOF_SCOOPS },
        { "spoiler", UNLOCKABLE_THING_SPOILERS },
        { "tint", UNLOCKABLE_THING_WINDOW_TINT },
        { "vinyls", static_cast< eUnlockableEntity >(40) },
    };

    for (unsigned int onPart = 0; onPart < 18; onPart++) {
        if (bStrCmp(partname, unlockType[onPart].mPartName) == 0) {
            return unlockType[onPart].mUnlockable;
        }
    }
    return UNLOCKABLE_THING_UNKNOWN;
}

void AwardUnlockUpgrade(Attrib::Gen::gameplay &inst) {
    const char *upgradePartName = inst.UpgradePartName(0);
    const char *upgradePartID = inst.UpgradePartID(0);
    int upgradeLevel = inst.UpgradeLevel(0);
    eUnlockableEntity entity = ConvertBigBangUpgradeAward(upgradePartID);
    if (entity != UNLOCKABLE_THING_UNKNOWN) {
        if (entity == static_cast<eUnlockableEntity>(0x32)) {
            if (upgradeLevel == 2) {
                entity = static_cast<eUnlockableEntity>(0x2e);
            } else if (upgradeLevel == 1) {
                entity = static_cast<eUnlockableEntity>(0x2c);
            } else if (upgradeLevel == 3) {
                entity = static_cast<eUnlockableEntity>(0x30);
            }
        }
        UnlockUnlockableThing(entity, 2, upgradeLevel, "");
    }
}

struct MarkerUnlockTypeEntry {
    const char *mMarkerName;
    const char *mPartName;
    FEMarkerManager::ePossibleMarker mMarker;
};

static MarkerUnlockTypeEntry markerUnlockType[21] = {
    { "backroom", "brakes", FEMarkerManager::MARKER_BRAKES },
    { "backroom", "chassis", FEMarkerManager::MARKER_CHASSIS },
    { "backroom", "engine", FEMarkerManager::MARKER_ENGINE },
    { "backroom", "induction", FEMarkerManager::MARKER_INDUCTION },
    { "backroom", "nos", FEMarkerManager::MARKER_NOS },
    { "backroom", "tires", FEMarkerManager::MARKER_TIRES },
    { "backroom", "transmission", FEMarkerManager::MARKER_TRANSMISSION },
    { "backroom", "bodykit", FEMarkerManager::MARKER_BODY },
    { "backroom", "decals", FEMarkerManager::MARKER_DECAL },
    { "backroom", "hood", FEMarkerManager::MARKER_HOOD },
    { "backroom", "hud", FEMarkerManager::MARKER_CUSTOM_HUD },
    { "backroom", "paint", FEMarkerManager::MARKER_PAINT },
    { "backroom", "rims", FEMarkerManager::MARKER_RIMS },
    { "backroom", "roofscoop", FEMarkerManager::MARKER_ROOF_SCOOP },
    { "backroom", "spoiler", FEMarkerManager::MARKER_SPOILER },
    { "backroom", "vinyls", FEMarkerManager::MARKER_VINYL },
    { "add_impound_box", nullptr, FEMarkerManager::MARKER_ADD_IMPOUND_BOX },
    { "cash_bonus", nullptr, FEMarkerManager::MARKER_CASH },
    { "out_of_jail_free", nullptr, FEMarkerManager::MARKER_GET_OUT_OF_JAIL },
    { "pink_slip", nullptr, FEMarkerManager::MARKER_PINK_SLIP },
    { "release_car_from_impound", nullptr, FEMarkerManager::MARKER_IMPOUND_RELEASE },
};

FEMarkerManager::ePossibleMarker FEMarkerManager::ConvertBigBangMarkerAward(const char *marker_name, const char *partid) {
    const char *const *partName = reinterpret_cast<const char *const *>(&markerUnlockType[0].mPartName);
    const ePossibleMarker *marker = reinterpret_cast<const ePossibleMarker *>(&markerUnlockType[0].mMarker);

    for (int onMarker = 0; onMarker < 21; onMarker++) {
        if (bStrICmp(marker_name, markerUnlockType[onMarker].mMarkerName) == 0
            && (!partName[onMarker * 3] || bStrICmp(partid, partName[onMarker * 3]) == 0)) {
            return marker[onMarker * 3];
        }
    }
    return MARKER_NONE;
}

void FEMarkerManager::AwardMarker(Attrib::Gen::gameplay &inst, bool immediate_reward) {
    ePossibleMarker marker = ConvertBigBangMarkerAward(inst.RewardMarkerType(0), inst.UpgradePartID(0));
    if (marker != MARKER_NONE) {
        int param = 0;
        if (immediate_reward) {
            if (marker == MARKER_PINK_SLIP) {
                goto award_pink_slip;
            }
            if (marker != MARKER_CASH) {
                goto add_inventory;
            }
            param = static_cast<int>(inst.CashReward(0));
            FEDatabase->GetCareerSettings()->CurrentCash = FEDatabase->GetCareerSettings()->CurrentCash + param;
            goto award_done;

        award_pink_slip:
            param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin());
            FEDatabase->GetPlayerCarStable(0)->AwardRivalCar(param);
            goto award_done;

        add_inventory:
            AddMarkerToInventory(marker, 0);

        award_done:
            ;
        } else {
            if (marker != MARKER_PINK_SLIP) {
                if (marker == MARKER_CASH) {
                    param = static_cast<int>(inst.CashReward(0));
                }
            } else {
                param = FEngHashString("BL%d", FEDatabase->GetCareerSettings()->GetCurrentBin(), 0);
            }
            AddMarkerForLaterSelection(marker, param);
        }
    }
}

inline bool CareerSettings::HasBeenAwardedDemoMarker() {
    return SpecialFlags & 0x20000;
}

inline void CareerSettings::SetAwardedDemoMarker() {
    SpecialFlags |= 0x20000;
}

void CareerSettings::TryAwardDemoMarker() {
    if (!HasBeenAwardedDemoMarker() && gEasterEggs.IsEasterEggUnlocked(static_cast<EasterEggsSpecial>(5))) {
        TheFEMarkerManager.AddMarkerToInventory(FEMarkerManager::ePossibleMarker(2), 0);
        SetAwardedDemoMarker();
    }
}

char *CareerSettings::SaveToBuffer(void *buffer, void *maxbuf) {
    char *buf = SaveGameplayData(buffer, maxbuf);
    buf = SaveSomeData(buf, &CurrentCar, 4, maxbuf);
    buf = SaveSomeData(buf, &CurrentBin, 1, maxbuf);
    buf = SaveSomeData(buf, &CurrentCash, 4, maxbuf);
    buf = SaveSomeData(buf, &AdaptiveDifficulty, 2, maxbuf);
    buf = SaveSomeData(buf, &SpecialFlags, 4, maxbuf);
    buf = SaveSomeData(buf, SMSMessages, 600, maxbuf);
    buf = SaveSomeData(buf, &SMSSortOrder, 2, maxbuf);
    buf = SaveSomeData(buf, CaseFileName, 16, maxbuf);
    buf = SaveRaceData(buf, maxbuf);
    buf = SaveUnlockData(buf, maxbuf);
    TheFEMarkerManager.SaveToBuffer(buf);
    return buf;
}

char *CareerSettings::LoadFromBuffer(void *buffer, void *maxbuf) {
    char *buf = LoadGameplayData(buffer, maxbuf);
    buf = LoadSomeData(&CurrentCar, buf, 4, maxbuf);
    buf = LoadSomeData(&CurrentBin, buf, 1, maxbuf);
    buf = LoadSomeData(&CurrentCash, buf, 4, maxbuf);
    buf = LoadSomeData(&AdaptiveDifficulty, buf, 2, maxbuf);
    buf = LoadSomeData(&SpecialFlags, buf, 4, maxbuf);
    buf = LoadSomeData(SMSMessages, buf, 600, maxbuf);
    buf = LoadSomeData(&SMSSortOrder, buf, 2, maxbuf);
    buf = LoadSomeData(CaseFileName, buf, 16, maxbuf);
    buf = LoadRaceData(buf, maxbuf);
    buf = LoadUnlockData(buf, maxbuf);
    TheFEMarkerManager.LoadFromBuffer(buf);
    return buf;
}

char *CareerSettings::SaveGameplayData(void *save_to, void *maxptr) {
    char *buf = static_cast<char *>(save_to);
    if (!GManager::Exists()) {
        bMemSet(buf, 0, 0x4000);
    } else {
        GManager::Get().SaveGameplayData(reinterpret_cast<unsigned char *>(buf), 0x4000);
    }
    return buf + 0x4000;
}

char *CareerSettings::LoadGameplayData(void *load_from_here, void *maxptr) {
    char *buf = static_cast<char *>(load_from_here);
    if (GManager::Exists()) {
        GManager::Get().LoadGameplayData(reinterpret_cast<unsigned char *>(buf), 0x4000);
    }
    return buf + 0x4000;
}

char *CareerSettings::SaveRaceData(void *save_to, void *maxptr) {
    char *buf = static_cast<char *>(save_to);
    if (GRaceDatabase::Exists()) {
        unsigned int nEntries = GRaceDatabase::Get().GetScoreInfoCount();
        nEntries = bMin(static_cast<int>(nEntries), 300);
        buf = SaveSomeData(buf, &nEntries, 4, maxptr);
        GRaceSaveInfo *current = GRaceDatabase::Get().GetScoreInfo();
        for (unsigned int index = 0; index < nEntries; index++) {
            if (gVerboseTesterOutput && current->mRaceHash != 0 && (current->mFlags & 2)) {
                GRaceDatabase::Get().GetRaceFromHash(current->mRaceHash);
            }
            buf = SaveSomeData(buf, current, 0x10, maxptr);
            current++;
        }
    }
    return static_cast<char *>(save_to) + 0x12C8;
}

char *CareerSettings::LoadRaceData(void *load_from_here, void *maxptr) {
    char *buf = static_cast<char *>(load_from_here);
    if (GRaceDatabase::Exists()) {
        unsigned int nEntries = 0;
        buf = LoadSomeData(&nEntries, buf, 4, maxptr);
        nEntries = bMin(static_cast<int>(nEntries), 300);
        GRaceSaveInfo saveInfoEntries[300];
        GRaceSaveInfo *current = saveInfoEntries;
        for (unsigned int index = 0; index < nEntries; index++) {
            buf = LoadSomeData(current, buf, 0x10, maxptr);
            if (gVerboseTesterOutput && current->mRaceHash != 0 && (current->mFlags & 2)) {
                GRaceDatabase::Get().GetRaceFromHash(current->mRaceHash);
            }
            current++;
        }
        GRaceDatabase::Get().LoadBestScores(saveInfoEntries, nEntries);
    }
    return static_cast<char *>(load_from_here) + 0x12C8;
}

// ==================== cFrontendDatabase implementations ====================

void cFrontendDatabase::DefaultRaceSettings() {
    unsigned int default_car = GetDefaultCar();
    for (unsigned int i = 0; i < 11; i++) {
        RaceSettings &settings = TheQuickRaceSettings[i];
        settings.Default();
        settings.SetSelectedCar(default_car, 0);
        settings.SetSelectedCar(default_car, 1);
    }
    TheQuickRaceSettings[0].NumLaps = 1;
    TheQuickRaceSettings[2].NumLaps = 1;
    TheQuickRaceSettings[5].NumLaps = 1;
    TheQuickRaceSettings[4].NumOpponents = 0;
    TheQuickRaceSettings[3].NumLaps = TheQuickRaceSettings[3].NumOpponents;
    TheQuickRaceSettings[4].NumLaps = 1;
}


void cFrontendDatabase::RestoreFromBackupDB() {
    if (m_pDBBackup) {
        LoadUserProfileFromBuffer(m_pDBBackup, GetUserProfileSaveSize(false), 0);
        DeallocBackupDB();
    }
}

void cFrontendDatabase::CreateMultiplayerProfile(int player) {
    if (!CurrentUserProfiles[player]) {
        CurrentUserProfiles[player] = new(__FILE__, __LINE__) UserProfile;
        CurrentUserProfiles[player]->Default(player, true);
    }
}

void cFrontendDatabase::DeleteMultiplayerProfile(int player) {
    if (player == 1 && CurrentUserProfiles[1]) {
        RaceSettings *settings = GetQuickRaceSettings(static_cast<GRace::Type>(11));
        FEPlayerCarDB *stable = GetPlayerCarStable(1);
        FECarRecord *record = stable->GetCarRecordByHandle(settings->GetSelectedCar(1));
        FECustomizationRecord *customization = stable->GetCustomizationRecordByHandle(record->Customization);
        bStrCpy(SplitScreenCarType, record->GetDebugName());
        if (!customization) {
            SplitScreenCustomization = nullptr;
        } else {
            SplitScreenCustomization = static_cast<FECustomizationRecord *>(bMalloc(sizeof(FECustomizationRecord), 0x47));
            bMemCpy(SplitScreenCustomization, customization, sizeof(FECustomizationRecord));
        }
        if (CurrentUserProfiles[1]) {
            delete CurrentUserProfiles[1];
        }
        CurrentUserProfiles[1] = nullptr;
    }
}

void cFrontendDatabase::DefaultProfile() {
    CurrentUserProfiles[0]->Default(0, true);
    bAutoSaveOverwriteConfirmed = false;
    DefaultRaceSettings();
    unsigned int default_car = GetDefaultCar();
    GetCareerSettings()->SetCurrentCar(default_car);
    bIsOptionsDirty = false;
    GetPlayerCarStable(0)->Default();
    MemoryCard::GetInstance()->SetCardRemovedWithAutoSaveEnabled(false);
    DefaultUnlockData();
    TheFEMarkerManager.Default();
    if (GRaceDatabase::Exists()) {
        GRaceDatabase::Get().ClearRaceScores();
    }
    if (GManager::Exists()) {
        GManager::Get().ResetAllGameplayData();
    }
}

bool cFrontendDatabase::LoadUserProfileFromBuffer(void *buffer, int bufsize, int player) {
    if (player == 0) {
        return CurrentUserProfiles[0]->LoadFromBuffer(buffer, bufsize, true, 0);
    } else {
        bool result = CurrentUserProfiles[player]->LoadFromBuffer(buffer, bufsize, false, player);
        bMemCpy(&CurrentUserProfiles[0]->GetOptions()->ThePlayerSettings[1],
                &CurrentUserProfiles[1]->GetOptions()->ThePlayerSettings[0],
                sizeof(PlayerSettings));
        return result;
    }
}

void cFrontendDatabase::RefreshCurrentRide() {
    RideInfo ride;
    FEPlayerCarDB *stable = GetPlayerCarStable(0);
    if (IsCareerMode() || IsSafehouseMode() || IsCareerManagerMode()) {
        BuildCurrentRideForPlayer(0, &ride);
    } else {
        RaceSettings *settings = GetQuickRaceSettings(static_cast<GRace::Type>(11));
        unsigned int handle = settings->GetSelectedCar(0);
        stable->BuildRideForPlayer(handle, 0, &ride);
    }
    CarViewer::SetRideInfo(&ride, static_cast<eSetRideInfoReasons>(2), static_cast<eCarViewerWhichCar>(0));
}

// ==================== UserProfile implementations ====================

void UserProfile::SetProfileName(const char *pName, bool isP1) {
    bool named = false;
    if (pName && bStrLen(pName) > 0) {
        named = true;
    }
    bMemSet(m_aProfileName, 0, 0x20);
    if (named) {
        bStrNCpy(m_aProfileName, pName, 0x20);
        m_bNamed = true;
    } else {
        char defaultName[32];
        if (isP1) {
            GetLocalizedString(defaultName, 0x20, 0x7b070984);
        } else {
            GetLocalizedString(defaultName, 0x20, 0x7b070985);
        }
        bStrNCpy(m_aProfileName, defaultName, 0x20);
        m_bNamed = false;
    }
}

void UserProfile::WriteProfileHash(void *bufferToHash, void *bufferToWrite, int bytes, void *maxptr) {
    MD5 md5;
    md5.Reset();
    md5.Update(bufferToHash, bytes);
    md5.GetRaw();
    SaveSomeData(bufferToWrite, md5.GetRaw(), 0x10, maxptr);
}

bool UserProfile::VerifyProfileHash(void *bufferToHash, void *bufferHash, int bytes) {
    MD5 md5;
    md5.Reset();
    md5.Update(bufferToHash, bytes);
    md5.GetRaw();
    return bMemCmp(md5.GetRaw(), bufferHash, 0x10) == 0;
}

void UserProfile::SaveToBuffer(void *buffer, int size) {
    char *buf = static_cast<char *>(buffer);
    char *maxbuf = buf + size;
    bMemSet(buf, 0, size);
    char aVersion[16];
    bMemSet(aVersion, 0, 0x10);
    buf = SaveSomeData(buf, aVersion, 0x10, maxbuf);
    buf = TheCareerSettings.SaveToBuffer(buf, maxbuf);
    buf = SaveSomeData(buf, &FEDatabase->iDefaultStableHash, 4, maxbuf);
    buf = SaveSomeData(buf, m_aProfileName, 0x20, maxbuf);
    buf = SaveSomeData(buf, Playlist, 0xF0, maxbuf);
    buf = SaveSomeData(buf, &TheOptionsSettings, 0xC0, maxbuf);
    int stableSize = PlayersCarStable.GetSaveBufferSize();
    buf = PlayersCarStable.SaveToBuffer(buf, stableSize);
    buf = SaveSomeData(buf, &CareerModeHasBeenCompletedAtLeastOnce, 4, maxbuf);
    buf = SaveSomeData(buf, &HighScores, 0xBD8, maxbuf);
    for (int i = 0; i < 11; i++) {
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i));
        unsigned int h = settings->GetSelectedCar(0);
        buf = SaveSomeData(buf, &h, 4, maxbuf);
    }
    WriteProfileHash(static_cast<char *>(buffer) + 0x10, buf, size - 0x20, maxbuf);
}

bool UserProfile::LoadFromBuffer(void *buffer, int size, bool commit_changes, int player_id) {
    char *buf = static_cast<char *>(buffer);
    char *maxbuf = buf + size;
    char aVersion[16];
    buf = LoadSomeData(aVersion, buf, 0x10, maxbuf);
    if (!player_id) {
        buf = TheCareerSettings.LoadFromBuffer(buf, maxbuf);
        TheCareerSettings.TryAwardDemoMarker();
    } else {
        int careerSize = TheCareerSettings.GetSaveBufferSize(false);
        buf = buf + careerSize;
    }
    unsigned int version;
    buf = LoadSomeData(&version, buf, 4, maxbuf);
    if (version != FEDatabase->iDefaultStableHash) {
        return false;
    }
    buf = LoadSomeData(m_aProfileName, buf, 0x20, maxbuf);
    if (!player_id) {
        buf = LoadSomeData(Playlist, buf, 0xF0, maxbuf);
    } else {
        buf = buf + 0xF0;
    }
    buf = LoadSomeData(&TheOptionsSettings, buf, 0xC0, maxbuf);
    int stableSize = PlayersCarStable.GetSaveBufferSize();
    buf = PlayersCarStable.LoadFromBuffer(buf, stableSize);
    PlayersCarStable.AwardBonusCars();
    buf = LoadSomeData(&CareerModeHasBeenCompletedAtLeastOnce, buf, 4, maxbuf);
    buf = LoadSomeData(&HighScores, buf, 0xBD8, maxbuf);
    for (int i = 0; i < 11; i++) {
        unsigned int h;
        buf = LoadSomeData(&h, buf, 4, maxbuf);
        RaceSettings *settings = FEDatabase->GetQuickRaceSettings(static_cast<GRace::Type>(i));
        settings->SelectedCar[player_id] = h;
    }
    if (!VerifyProfileHash(static_cast<char *>(buffer) + 0x10, buf, size - 0x20)) {
        return false;
    }
    m_bNamed = true;
    return true;
}
