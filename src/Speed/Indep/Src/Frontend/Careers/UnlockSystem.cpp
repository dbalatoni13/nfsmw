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

// GRaceDatabase inline methods (can't add bodies to header - DWARF crash)
inline GRaceSaveInfo *GRaceDatabase::GetScoreInfo() {
    return mRaceScoreInfo;
}

inline unsigned int GRaceDatabase::GetScoreInfoCount() {
    return mRaceCountStatic;
}

// total size: 0x10
struct GRaceSaveInfo {
    unsigned int mRaceHash;                              // offset 0x0, size 0x4
    unsigned int mFlags;                                 // offset 0x4, size 0x4
    float mHighScores;                                   // offset 0x8, size 0x4
    unsigned short mTopSpeed;                            // offset 0xC, size 0x2
    unsigned short mAverageSpeed;                        // offset 0xE, size 0x2
};

struct CarPart {
    unsigned short PartNameHashBot;
    unsigned short PartNameHashTop;
    char PartID;
    unsigned char GroupNumber_UpgradeLevel;
    char BaseModelNameHashSelector;
    unsigned char CarTypeNameHashIndex;
    unsigned short NameOffset;
    unsigned short AttributeTableOffset;
    unsigned short ModelNameHashTableOffset;

    char GetUpgradeLevel() { return GroupNumber_UpgradeLevel >> 5; }
};

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
    if (filter & 1) {
        char count = TheUnlockData[static_cast<int>(entity) * 8 + 7]++;
        if (count + 1 >= 4) {
            TheUnlockData[static_cast<int>(entity) * 8 + 7] = 0;
            TheUnlockData[static_cast<int>(entity) * 8 + 6] = -1;
        }
        return;
    }
    if (filter & 2) {
        char count = TheUnlockData[static_cast<int>(entity) * 8 + 3]++;
        if (count + 1 >= 4) {
            TheUnlockData[static_cast<int>(entity) * 8 + 2] = -1;
            TheUnlockData[static_cast<int>(entity) * 8 + 3] = 0;
        }
    }
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
    default: return UNLOCKABLE_THING_UNKNOWN;
    }
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
    if (part_name_hash > 0x13d0c7 && part_name_hash < 0x13d0cb) {
        return GetIsCollectorsEdition();
    }
    return true;
}

// ============================================================
// CareerUnlocker remaining
// ============================================================

bool CareerUnlocker::IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom) {
    bool answer = UnlockAllThings != 0;
    eUnlockableEntity unlockable = MapCarPartToUnlockable(carslot, part);
    int unlocked = CareerUnlocker::IsUnlockableUnlocked(filter, unlockable, part->GetUpgradeLevel(), backroom);
    return part->GetUpgradeLevel() == 0 | answer | unlocked;
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
    bool answer = UnlockAllThings != 0;
    FEMarkerManager::ePossibleMarker marker = FEMarkerManager::MARKER_NONE;
    switch (ent) {
    case UNLOCKABLE_THING_PUT_TIRES:        marker = FEMarkerManager::MARKER_TIRES; break;
    case UNLOCKABLE_THING_PUT_BRAKES:       marker = FEMarkerManager::MARKER_BRAKES; break;
    case UNLOCKABLE_THING_PUT_CHASSIS:      marker = FEMarkerManager::MARKER_CHASSIS; break;
    case UNLOCKABLE_THING_PUT_TRANSMISSION: marker = FEMarkerManager::MARKER_TRANSMISSION; break;
    case UNLOCKABLE_THING_PUT_ENGINE:       marker = FEMarkerManager::MARKER_ENGINE; break;
    case UNLOCKABLE_THING_PUT_INDUCTION:    marker = FEMarkerManager::MARKER_INDUCTION; break;
    case UNLOCKABLE_THING_PUT_NOS:          marker = FEMarkerManager::MARKER_NOS; break;
    case UNLOCKABLE_THING_BODY_KIT:         marker = FEMarkerManager::MARKER_BODY; break;
    case UNLOCKABLE_THING_SPOILERS:         marker = FEMarkerManager::MARKER_SPOILER; break;
    case UNLOCKABLE_THING_RIM_BRANDS:       marker = FEMarkerManager::MARKER_RIMS; break;
    case UNLOCKABLE_THING_HOODS:            marker = FEMarkerManager::MARKER_HOOD; break;
    case UNLOCKABLE_THING_ROOF_SCOOPS:      marker = FEMarkerManager::MARKER_ROOF_SCOOP; break;
    case UNLOCKABLE_THING_CUSTOM_HUD:       marker = FEMarkerManager::MARKER_CUSTOM_HUD; break;
    case UNLOCKABLE_THING_PAINTABLE_BODY:   marker = FEMarkerManager::MARKER_PAINT; break;
    case UNLOCKABLE_VINYLS_GROUP_BODY:      marker = FEMarkerManager::MARKER_VINYL; break;
    default: break;
    }
    if (marker == FEMarkerManager::MARKER_NONE) {
        return answer;
    }
    if (!CareerUnlocker::IsUnlockableUnlocked(filter, ent, level, false)) {
        if (TheFEMarkerManager.IsMarkerAvailable(marker, 0)) {
            return true;
        }
    }
    return answer;
}

// ============================================================
// QuickRaceUnlocker::IsCarUnlocked
// ============================================================

bool QuickRaceUnlocker::IsCarUnlocked(eUnlockFilters filter, unsigned int car, int player) {
    bool answer = UnlockAllThings != 0;
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *fe_car = stable->GetCarRecordByHandle(car);
    Attrib::Gen::frontend CarAttribs(fe_car->FEKey, 0, nullptr);
    unsigned char unlockedAt = CarAttribs.UnlockedAt();
    bool hasBeaten = FEDatabase->GetUserProfile(0)->GetCareer()->HasBeatenCareer();
    bool completedOnce = FEDatabase->GetUserProfile(player)->CareerModeHasBeenCompletedAtLeastOnce;
    unsigned char currentBin = FEDatabase->GetCareerSettings()->GetCurrentBin();
    return currentBin >= unlockedAt | answer | hasBeaten | completedOnce;
}

void ClearAllNewStatus() {
    for (int i = 0; i < 0x39; i++) {
        UnlockSystem::ClearNewUnlock(static_cast<eUnlockableEntity>(i), 2);
        UnlockSystem::ClearNewUnlock(static_cast<eUnlockableEntity>(i), 1);
    }
}

bool DoesCategoryHaveNewUnlock(eUnlockableEntity ent) {
    bool hasNew = false;
    for (int i = 0; i <= gMaxPartLevels[static_cast<int>(ent)]; i++) {
        if (UnlockSystem::IsUnlockableNew(static_cast<eUnlockFilters>(7), ent, i)) {
            hasNew = true;
        }
    }
    return hasNew;
}

struct UnlockTypeEntry {
    const char *mPartName;
    eUnlockableEntity mUnlockable;
};

static UnlockTypeEntry unlockType[18] = {
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

eUnlockableEntity ConvertBigBangUpgradeAward(const char *partname) {
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
    TheQuickRaceSettings[5].NumLaps = 1;
    TheQuickRaceSettings[2].NumLaps = 1;
    TheQuickRaceSettings[0].NumLaps = 1;
    TheQuickRaceSettings[3].NumLaps = TheQuickRaceSettings[3].NumOpponents;
    TheQuickRaceSettings[4].NumOpponents = 0;
    TheQuickRaceSettings[4].NumLaps = 1;
}

void cFrontendDatabase::NotifyDeleteCar(unsigned int handle) {
    unsigned int default_car = GetDefaultCar();
    for (unsigned int i = 0; i < 11; i++) {
        RaceSettings &settings = TheQuickRaceSettings[i];
        if (settings.GetSelectedCar(0) == handle) {
            settings.SetSelectedCar(default_car, 0);
        }
        if (settings.GetSelectedCar(1) == handle) {
            settings.SetSelectedCar(default_car, 0);
        }
    }
}

void cFrontendDatabase::RestoreFromBackupDB() {
    char *backup = m_pDBBackup;
    if (backup) {
        int size = GetUserProfileSaveSize(false);
        LoadUserProfileFromBuffer(backup, size, 0);
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