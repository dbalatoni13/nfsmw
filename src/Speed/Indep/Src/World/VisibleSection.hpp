#ifndef WORLD_VISIBLE_SECTION_H
#define WORLD_VISIBLE_SECTION_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int ScenerySectionLODOffset;

static inline char GetScenerySectionLetter(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static inline bool IsRegularScenerySection(int section_number) {
    char section_letter = GetScenerySectionLetter(section_number);
    return section_letter >= 'A' && section_letter < 'U';
}

static inline int GetScenerySubsectionNumber(int section_number) {
    return section_number % 100;
}

static inline short GetScenerySectionNumber(char section_letter, int subsection_number) {
    return static_cast<short>((section_letter - 'A' + 1) * 100 + subsection_number);
}

static inline bool IsTextureSection(int section_number) {
    char section_letter = GetScenerySectionLetter(section_number);
    return section_letter == 'Y' || section_letter == 'W';
}

static inline bool IsLibrarySection(int section_number) {
    char section_letter = GetScenerySectionLetter(section_number);
    return section_letter == 'X' || section_letter == 'U';
}

static inline bool IsScenerySectionDrivable(int section_number) {
    if (!IsRegularScenerySection(section_number)) {
        return false;
    }

    int subsection_number = GetScenerySubsectionNumber(section_number);
    return subsection_number > 0 && subsection_number < ScenerySectionLODOffset;
}

static inline int GetLODScenerySectionNumber(int drivable_section_number) {
    return drivable_section_number + ScenerySectionLODOffset;
}

static inline int bGetTablePos(short *table, int num_elements, short element) {
    for (int n = 0; n < num_elements; n++) {
        if (table[n] == element) {
            return n;
        }
    }

    return -1;
}

static inline bool bIsInTable(short *table, int num_elements, short element) {
    return bGetTablePos(table, num_elements, element) >= 0;
}

static inline bool HasSection(short *section_table, int num_sections, short section_number) {
    return bIsInTable(section_table, num_sections, section_number);
}

struct VisibleSectionBoundary : public bTNode<VisibleSectionBoundary> {
    // total size: 0xA4
    short SectionNumber;   // offset 0x8, size 0x2
    char NumPoints;        // offset 0xA, size 0x1
    char PanoramaBoundary; // offset 0xB, size 0x1
    bVector2 BBoxMin;      // offset 0xC, size 0x8
    bVector2 BBoxMax;      // offset 0x14, size 0x8
    bVector2 Centre;       // offset 0x1C, size 0x8
    bVector2 Points[16];   // offset 0x24, size 0x80

    void EndianSwap();
    bool IsPointInside(const bVector2 *point);
    float GetDistanceOutside(const bVector2 *point, float max_distance);
    int GetSectionNumber();
    int GetMemoryImageSize();

    int GetNumPoints() {
        return NumPoints;
    }

    bVector2 *GetPoint(int n) {
        return &Points[n];
    }
};

struct VisibleSectionCoordinate {
    // total size: 0x10
    bVector2 Position;                 // offset 0x0, size 0x8
    VisibleSectionBoundary *pBoundary; // offset 0x8, size 0x4
    float TestDistance;                // offset 0xC, size 0x4
};

struct DrivableScenerySection : public bTNode<DrivableScenerySection> {
    // total size: 0xA4
    VisibleSectionBoundary *pBoundary; // offset 0x8, size 0x4
    short SectionNumber;               // offset 0xC, size 0x2
    char MostVisibleSections;          // offset 0xE, size 0x1
    char MaxVisibleSections;           // offset 0xF, size 0x1
    short NumVisibleSections;          // offset 0x10, size 0x2
    short VisibleSections[72];         // offset 0x12, size 0x90
    short Padding;                     // offset 0xA2, size 0x2

    void EndianSwap();
    int GetSectionNumber() {
        return SectionNumber;
    }
    int GetMemoryImageSize();
    void AddVisibleSection(int section_number);
    int IsSectionVisible(int section_number);
    void RemoveVisibleSection(int section_number);
    void SortVisibleSections();

    int GetNumVisibleSections() {
        return this->NumVisibleSections;
    }

    int GetVisibleSection(int i) {
        return this->VisibleSections[i];
    }
};

struct DrivableSectionsInRegion {
    // total size: 0x324
    int NumSections;     // offset 0x0, size 0x4
    short Sections[400]; // offset 0x4, size 0x320

    void EndianSwap();
};

struct VisibleTextureSection : public bTNode<VisibleTextureSection> {
    // total size: 0x150
    short SectionNumber;            // offset 0x8, size 0x2
    short NumVisibleFromSections;   // offset 0xA, size 0x2
    short VisibleFromSections[128]; // offset 0xC, size 0x100
    short NumSuperSections;         // offset 0x10C, size 0x2
    short SuperSections[32];        // offset 0x10E, size 0x40
};

struct LoadingSection : public bTNode<LoadingSection> {
    // total size: 0x4C
    char Name[15];              // offset 0x8, size 0xF
    char DefaultFlag;           // offset 0x17, size 0x1
    short NumDrivableSections;  // offset 0x18, size 0x2
    short DrivableSections[16]; // offset 0x1A, size 0x20
    short NumExtraSections;     // offset 0x3A, size 0x2
    short ExtraSections[8];     // offset 0x3C, size 0x10

    void EndianSwap();
};

struct SuperScenerySection : public bTNode<SuperScenerySection> {
    // total size: 0x30
    int SectionNumber;  // offset 0x8, size 0x4
    short NumSections;  // offset 0xC, size 0x2
    short Sections[16]; // offset 0xE, size 0x20
};

struct VisibleSectionBitTable {
    // total size: 0x15E
    unsigned char Bits[350]; // offset 0x0, size 0x15E
};

struct UsedInSectionInfo : public bTNode<UsedInSectionInfo> {
    // total size: 0x638
    char Name[64];                         // offset 0x8, size 0x40
    bool IsTexture;                        // offset 0x48, size 0x1
    bool AlwaysVisible;                    // offset 0x4C, size 0x1
    int NumUsedInSections;                 // offset 0x50, size 0x4
    short UsedInSections[128];             // offset 0x54, size 0x100
    int NumVisibleFromSections;            // offset 0x154, size 0x4
    short VisibleFromSections[256];        // offset 0x158, size 0x200
    int NumVisibleFromTextureSections;     // offset 0x358, size 0x4
    short VisibleFromTextureSections[100]; // offset 0x35C, size 0xC8
    int NumPlacedInSections;               // offset 0x424, size 0x4
    short PlacedInSections[256];           // offset 0x428, size 0x200
    float PlacementScore;                  // offset 0x628, size 0x4
    float MemoryScore;                     // offset 0x62C, size 0x4
    float BandwidthScore;                  // offset 0x630, size 0x4
    const char *PlacementReason;           // offset 0x634, size 0x4
};

struct OverlayEntry {
    // total size: 0x6
    char AddRemove;              // offset 0x0, size 0x1
    char Pad;                    // offset 0x1, size 0x1
    short DrivableSectionNumber; // offset 0x2, size 0x2
    short SectionNumber;         // offset 0x4, size 0x2
};

struct VisibleSectionOverlay : public bTNode<VisibleSectionOverlay> {
    // total size: 0x6034
    char Name[40];                 // offset 0x8, size 0x28
    int NumEntries;                // offset 0x30, size 0x4
    OverlayEntry EntryTable[4096]; // offset 0x34, size 0x6000

    void EndianSwap();
};

struct VisibleSectionManagerInfo {
    // total size: 0x328
    int LODOffset;                                        // offset 0x0, size 0x4
    DrivableSectionsInRegion TheDrivableSectionsInRegion; // offset 0x4, size 0x324

    void EndianSwap();
};

inline void VisibleSectionBoundary::EndianSwap() {
    bPlatEndianSwap(&SectionNumber);
    bPlatEndianSwap(reinterpret_cast<signed char *>(&NumPoints));
    bPlatEndianSwap(reinterpret_cast<signed char *>(&PanoramaBoundary));
    bPlatEndianSwap(&BBoxMin);
    bPlatEndianSwap(&BBoxMax);
    for (int i = 0; i < NumPoints; i++) {
        bPlatEndianSwap(&Points[i]);
    }
}

inline int VisibleSectionBoundary::GetSectionNumber() {
    return SectionNumber;
}

inline int VisibleSectionBoundary::GetMemoryImageSize() {
    return 0xA4 - (16 - NumPoints) * sizeof(bVector2);
}

inline void DrivableSectionsInRegion::EndianSwap() {
    bPlatEndianSwap(&NumSections);
    for (int i = 0; i < NumSections; i++) {
        bPlatEndianSwap(&Sections[i]);
    }
}

inline void DrivableScenerySection::EndianSwap() {
    bPlatEndianSwap(&SectionNumber);
    bPlatEndianSwap(reinterpret_cast<signed char *>(&MostVisibleSections));
    bPlatEndianSwap(reinterpret_cast<signed char *>(&MaxVisibleSections));
    bPlatEndianSwap(&NumVisibleSections);
    for (int i = 0; i < NumVisibleSections; i++) {
        bPlatEndianSwap(&VisibleSections[i]);
    }
}

inline int DrivableScenerySection::GetMemoryImageSize() {
    return 0x12 + NumVisibleSections * sizeof(short) + sizeof(Padding);
}

inline void LoadingSection::EndianSwap() {
    bPlatEndianSwap(&NumDrivableSections);
    for (int i = 0; i < NumDrivableSections; i++) {
        bPlatEndianSwap(&DrivableSections[i]);
    }
    bPlatEndianSwap(&NumExtraSections);
    for (int i = 0; i < NumExtraSections; i++) {
        bPlatEndianSwap(&ExtraSections[i]);
    }
}

inline void VisibleSectionOverlay::EndianSwap() {
    bPlatEndianSwap(&NumEntries);
    for (int n = 0; n < NumEntries; n++) {
        OverlayEntry *entry = &EntryTable[n];
        bPlatEndianSwap(&entry->DrivableSectionNumber);
        bPlatEndianSwap(&entry->SectionNumber);
    }
}

inline void VisibleSectionManagerInfo::EndianSwap() {
    bPlatEndianSwap(&LODOffset);
    TheDrivableSectionsInRegion.EndianSwap();
}

struct OverrideSectionObject : public bTNode<OverrideSectionObject> {
    // total size: 0x4C
    short SectionNumber; // offset 0x8, size 0x2
    short Touched;       // offset 0xA, size 0x2
    char ObjectName[64]; // offset 0xC, size 0x40
};

struct VisibleSectionUserInfo {
    // total size: 0x1C
    int ReferenceCount;                                 // offset 0x0, size 0x4
    struct ScenerySectionHeader *pScenerySectionHeader; // offset 0x4, size 0x4
    struct eLightFlarePackHeader *pLightFlarePack;      // offset 0x8, size 0x4
    struct eLightPack *pLightPack;                      // offset 0xC, size 0x4
    struct SmokeableSpawnerPack *pSmokeableSpawnerPack; // offset 0x10, size 0x4
    struct ParkedCarPack *pParkedCarPack;               // offset 0x14, size 0x4
    struct EventTriggerPack *pEventTriggerPack;         // offset 0x18, size 0x4
};

struct UnallocatedVisibleSectionUserInfo {};

struct VisibleGroupInfo {
    // total size: 0x8
    char *SelectionSetName; // offset 0x0, size 0x4
    bool UsedForTopology;   // offset 0x4, size 0x1
};

class VisibleSectionManager {
  public:
    static VisibleGroupInfo *GetGroupInfo(const char *selection_set_name);

    VisibleTextureSection *FindVisibleTextureSection(int section_number);

    LoadingSection *FindLoadingSection(int drivable_section_number);

    LoadingSection *FindLoadingSection(const char *name);

    int GetSectionsToLoad(LoadingSection *loading_section, short *section_numbers, int max_sections);

    OverrideSectionObject *FindOverrideSectionObject(const char *name, OverrideSectionObject *prev_object, bool partial_compare);

    VisibleSectionManager();

    ~VisibleSectionManager();

    VisibleSectionUserInfo *AllocateUserInfo(int section_number);

    void UnallocateUserInfo(int section_number);

    void ActivateOverlay(const char *name);

    void ActivateOverlay(VisibleSectionOverlay *overlay, VisibleSectionOverlay *undo_overlay);

    void UnactivateOverlay();

    int Loader(bChunk *chunk);

    int Unloader(bChunk *chunk);

    VisibleSectionBoundary *FindBoundary(int section_number);

    VisibleSectionBoundary *FindClosestBoundary(const bVector2 *point, float *distance_outside);

    VisibleSectionBoundary *FindBoundary(const bVector2 *point);

    int FindCloseBoundaries(VisibleSectionBoundary **boundaries, int max_boundaries, const bVector2 *point, float distance_outside);

    DrivableScenerySection *FindDrivableSection(const bVector2 *point);

    DrivableScenerySection *FindDrivableSection(int section_number);

    unsigned int GetVisibleSectionChecksum(int section_number);

    bool IsGroupEnabled(unsigned int group_name_hash);

    void EnableGroup(unsigned int group_name_hash);

    void DisableGroup(unsigned int group_name_hash);

    void DisableAllGroups() {
        bMemSet(EnabledGroups, 0, 0x400);
    }

    VisibleSectionUserInfo *GetUserInfo(int section_number) {
        return this->UserInfoTable[section_number];
    }

  private:
    bTList<VisibleSectionBoundary> DrivableBoundaryList;               // offset 0x0, size 0x8
    bTList<VisibleSectionBoundary> NonDrivableBoundaryList;            // offset 0x8, size 0x8
    bTList<DrivableScenerySection> DrivableSectionList;                // offset 0x10, size 0x8
    bTList<VisibleTextureSection> VisibleTextureSectionList;           // offset 0x18, size 0x8
    bTList<LoadingSection> LoadingSectionList;                         // offset 0x20, size 0x8
    bTList<SuperScenerySection> SuperScenerySectionList;               // offset 0x28, size 0x8
    bTList<OverrideSectionObject> OverrideSectionObjectList;           // offset 0x30, size 0x8
    bTList<UsedInSectionInfo> GeometryUsedInSectionInfoList;           // offset 0x38, size 0x8
    bTList<UsedInSectionInfo> TextureUsedInSectionInfoList;            // offset 0x40, size 0x8
    bTList<VisibleSectionOverlay> OverlayList;                         // offset 0x48, size 0x8
    bChunk *pBoundaryChunks;                                           // offset 0x50, size 0x4
    VisibleSectionManagerInfo *pInfo;                                  // offset 0x54, size 0x4
    VisibleSectionOverlay *pActiveOverlay;                             // offset 0x58, size 0x4
    VisibleSectionOverlay *pUndoOverlay;                               // offset 0x5C, size 0x4
    VisibleSectionUserInfo *UserInfoTable[2800];                       // offset 0x60, size 0x2BC0
    int NumAllocatedUserInfo;                                          // offset 0x2C20, size 0x4
    VisibleSectionUserInfo UserInfoStorageTable[512];                  // offset 0x2C24, size 0x3800
    bTList<UnallocatedVisibleSectionUserInfo> UnallocatedUserInfoList; // offset 0x6424, size 0x8
    VisibleSectionBitTable *VisibleBitTables;                          // offset 0x642C, size 0x4
    unsigned int EnabledGroups[256];                                   // offset 0x6430, size 0x40
};

extern VisibleSectionManager TheVisibleSectionManager; // size: 0x6830

#endif
