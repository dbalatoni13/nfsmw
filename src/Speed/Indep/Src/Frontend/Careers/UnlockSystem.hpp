#ifndef FRONTEND_CAREERS_UNLOCKSYSTEM_H
#define FRONTEND_CAREERS_UNLOCKSYSTEM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"
#include "Speed/Indep/Src/Physics/PhysicsUpgrades.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/frontend.h"

struct CarPart;

struct UnlockSystem {
    static bool IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity thing, int level, int player, bool backroom);
    static bool IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom);
    static bool IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom);
    static bool IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player);
    static bool IsCarUnlocked(eUnlockFilters filter, unsigned int handle, int player);
    static bool IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level);
    static bool IsUnlockableNew(eUnlockFilters filter, eUnlockableEntity ent, int level);
    static void ClearNewUnlock(eUnlockableEntity ent, unsigned int filter);
    static int GetPerfPackageCost(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player);
    static int GetCarPartCost(eUnlockFilters filter, int carslot, CarPart *part, int player);
    static bool IsEventAvailable(unsigned int event_hash);
    static bool IsBonusCarCEOnly(unsigned int name_hash);
    static bool IsUnlockableAvailable(unsigned int part_name_hash);
};

struct CareerUnlocker {
    static bool IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom);
    static bool IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom);
    static bool IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom);
    static bool IsTrackUnlocked(eUnlockFilters filter, int event_hash);
    static bool IsCarUnlocked(eUnlockFilters filter, unsigned int car);
    static bool IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level);
};

struct QuickRaceUnlocker {
    static int IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, int player, bool backroom);
    static int IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, int player, bool backroom);
    static int IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, int player, bool backroom);
    static bool IsTrackUnlocked(eUnlockFilters filter, int event_hash, int player);
    static bool IsCarUnlocked(eUnlockFilters filter, unsigned int car, int player);
    static bool IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level, int player);
};

struct OnlineUnlocker {
    static bool IsUnlockableUnlocked(eUnlockFilters filter, eUnlockableEntity ent, int level, bool backroom);
    static bool IsCarPartUnlocked(eUnlockFilters filter, int carslot, CarPart *part, bool backroom);
    static bool IsPerfPackageUnlocked(eUnlockFilters filter, Physics::Upgrades::Type pkg_type, int level, bool backroom);
    static bool IsTrackUnlocked(eUnlockFilters filter, int event_hash);
    static bool IsCarUnlocked(eUnlockFilters filter, unsigned int car);
    static bool IsBackroomAvailable(eUnlockFilters filter, eUnlockableEntity ent, int level);
};

struct FEMarkerManager {
    enum ePossibleMarker {
        MARKER_NONE = 0,
        MARKER_BRAKES = 1,
        MARKER_ENGINE = 2,
        MARKER_NOS = 3,
        MARKER_INDUCTION = 4,
        MARKER_CHASSIS = 5,
        MARKER_TIRES = 6,
        MARKER_TRANSMISSION = 7,
        MARKER_BODY = 8,
        MARKER_HOOD = 9,
        MARKER_SPOILER = 10,
        MARKER_RIMS = 11,
        MARKER_ROOF_SCOOP = 12,
        MARKER_CUSTOM_HUD = 13,
        MARKER_VINYL = 14,
        MARKER_DECAL = 15,
        MARKER_PAINT = 16,
        MARKER_CUSTOMIZE_FIRST = 1,
        MARKER_CUSTOMIZE_LAST = 16,
        MARKER_GET_OUT_OF_JAIL = 17,
        MARKER_PINK_SLIP = 18,
        MARKER_CASH = 19,
        MARKER_ADD_IMPOUND_BOX = 20,
        MARKER_IMPOUND_RELEASE = 21,
        MARKER_FIRST = 1,
        MARKER_LAST = 21,
    };

    enum eMarkerStates {
        MARKER_STATE_NOT_OWNED = 0,
        MARKER_STATE_OWNED = 1,
        MARKER_STATE_USED = 2,
    };

    struct OwnedMarker {
        ePossibleMarker Marker;  // offset 0x0
        int Param;               // offset 0x4
        eMarkerStates State;     // offset 0x8
    };

    FEMarkerManager();
    void Default();
    void GetMarkerForLaterSelection(int index, ePossibleMarker &marker, int &param);
    void AddMarkerForLaterSelection(ePossibleMarker marker, int param);
    void ClearMarkersForLaterSelection();
    void AddMarkerToInventory(ePossibleMarker marker, int param);
    void UtilizeMarker(ePossibleMarker marker, int param);
    void UtilizeMarker(unsigned int slot_id);
    void UtilizeMarker(Physics::Upgrades::Type type);
    bool IsMarkerAvailable(ePossibleMarker marker, int param);
    int GetNumCustomizeMarkers();
    int GetNumMarkers(ePossibleMarker marker, int param);
    ePossibleMarker ConvertBigBangMarkerAward(const char *marker_name, const char *partid);
    void AwardMarker(Attrib::Gen::gameplay &inst, bool immediate_reward);
    char *SaveToBuffer(char *buffer);
    char *LoadFromBuffer(char *buffer);
    int GetSaveBufferSize() { return 0x2F4; }

    inline int GetNumTempMarkers() { return iNumTempMarkers; }
    inline bool HasMarker(ePossibleMarker marker, int param) {
        return GetNumMarkers(marker, param) > 0;
    }

    OwnedMarker OwnedMarkers[63];           // offset 0x0
    OwnedMarker TempSelectionMarkers[6];     // offset 0x2F4
    int iNumTempMarkers;                     // offset 0x33C
};

extern FEMarkerManager TheFEMarkerManager;

#endif
