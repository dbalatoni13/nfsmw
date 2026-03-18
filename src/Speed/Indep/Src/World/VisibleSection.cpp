#include "VisibleSection.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

VisibleSectionManager TheVisibleSectionManager; // size: 0x6830
int ScenerySectionLODOffset = 0;
DrivableScenerySection *pSectionD9 = 0;
DrivableScenerySection *pSectionC14 = 0;

int LoaderVisibleSections(bChunk *chunk) {
    return TheVisibleSectionManager.Loader(chunk);
}

int UnloaderVisibleSections(bChunk *chunk) {
    return TheVisibleSectionManager.Unloader(chunk);
}

void RefreshTrackStreamer();
BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width);
int Get2PlayerSectionNumber(int section_number, const char *build_platform);
int Get2PlayerSectionNumber(int section_number);
int Get1PlayerSectionNumber(int section_number_2p, const char *build_platform);
int GetBoundarySectionNumber(int section_number, const char *platform_name);
static bool MyIsPointInPoly(const bVector2 *point, const bVector2 *polygon, int num_points);
float bDistToLine(const bVector2 *point, const bVector2 *pointa, const bVector2 *pointb);

struct SectionRemapper {
    short SectionNumber;
    short SectionNumber2P;
};

extern VisibleGroupInfo VisibleGroupInfoTable[5];
extern SectionRemapper SectionRemapperTable[134];
extern SectionRemapper SectionRemapperTable_Gamecube[129];
char *bGetPlatformName();
int bStrNICmp(const char *s1, const char *s2, int n);

static bool initialized_VisibleSection = false;
static int map_table_VisibleSection[2800];
static int counter_VisibleSection = 0;
static char text_VisibleSection[4][16];

static char GetScenerySectionLetter(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static int GetScenerySubsectionNumber(int section_number) {
    return section_number % 100;
}

static short GetScenerySectionNumber(char section_letter, int subsection_number) {
    return static_cast<short>((section_letter - 'A' + 1) * 100 + subsection_number);
}

static bool IsRegularScenerySection(int section_number) {
    char section_letter = GetScenerySectionLetter(section_number);
    return section_letter >= 'A' && section_letter < 'U';
}

static bool IsScenerySectionDrivable(int section_number) {
    int subsection_number = GetScenerySubsectionNumber(section_number);
    return IsRegularScenerySection(section_number) && subsection_number > 0 && subsection_number < ScenerySectionLODOffset;
}

void GetScenerySectionName(char *name, int section_number) {
    if (section_number < 1) {
        bStrCpy(name, "--");
    } else {
        bSPrintf(name, "%c%d", GetScenerySectionLetter(section_number), GetScenerySubsectionNumber(section_number));
    }
}

char *GetScenerySectionName(int section_number) {
    unsigned int index = static_cast<unsigned int>(counter_VisibleSection) & 3;
    counter_VisibleSection += 1;
    GetScenerySectionName(text_VisibleSection[index], section_number);
    return text_VisibleSection[index];
}

static bool PointInBBox(const bVector2 *point, const bVector2 *bbox_min, const bVector2 *bbox_max) {
    return point->x >= bbox_min->x && point->x <= bbox_max->x && point->y >= bbox_min->y && point->y <= bbox_max->y;
}

void VisibleSectionBoundary::EndianSwap() {
    bPlatEndianSwap(&SectionNumber);
    bPlatEndianSwap(reinterpret_cast<signed char *>(&NumPoints));
    bPlatEndianSwap(reinterpret_cast<signed char *>(&PanoramaBoundary));
    bPlatEndianSwap(&BBoxMin);
    bPlatEndianSwap(&BBoxMax);
    bPlatEndianSwap(&Centre);
    for (int i = 0; i < NumPoints; i++) {
        bPlatEndianSwap(&Points[i]);
    }
}

bool VisibleSectionBoundary::IsPointInside(const bVector2 *point) {
    if (!bBoundingBoxIsInside(&BBoxMin, &BBoxMax, point, 0.0f)) {
        return false;
    }

    return MyIsPointInPoly(point, Points, NumPoints) != 0;
}

float VisibleSectionBoundary::GetDistanceOutside(const bVector2 *point, float max_distance) {
    if (bBoundingBoxIsInside(&BBoxMin, &BBoxMax, point, 0.0f)) {
        if (IsPointInside(point)) {
            max_distance = 0.0f;
        } else {
            for (int i = 0; i < NumPoints; i++) {
                int next = i + 1;
                float distance = bDistToLine(point, &Points[i], &Points[next - (next / NumPoints) * NumPoints]);
                if (distance < max_distance) {
                    max_distance = distance;
                }
            }
        }
    }

    return max_distance;
}

int VisibleSectionBoundary::GetSectionNumber() {
    return SectionNumber;
}

int VisibleSectionBoundary::GetMemoryImageSize() {
    return 0x24 + NumPoints * sizeof(bVector2);
}

void DrivableSectionsInRegion::EndianSwap() {
    bPlatEndianSwap(&NumSections);
    for (int i = 0; i < NumSections; i++) {
        bPlatEndianSwap(&Sections[i]);
    }
}

void DrivableScenerySection::EndianSwap() {
    bPlatEndianSwap(&SectionNumber);
    bPlatEndianSwap(reinterpret_cast<signed char *>(&MostVisibleSections));
    bPlatEndianSwap(reinterpret_cast<signed char *>(&MaxVisibleSections));
    bPlatEndianSwap(&NumVisibleSections);
    for (int i = 0; i < NumVisibleSections; i++) {
        bPlatEndianSwap(&VisibleSections[i]);
    }
}

int DrivableScenerySection::GetSectionNumber() {
    return SectionNumber;
}

int DrivableScenerySection::GetMemoryImageSize() {
    return 0x12 + NumVisibleSections * sizeof(short) + sizeof(Padding);
}

void DrivableScenerySection::AddVisibleSection(int section_number) {
    if (NumVisibleSections < MaxVisibleSections && !IsSectionVisible(section_number)) {
        short num_visible_sections = NumVisibleSections;
        NumVisibleSections = num_visible_sections + 1;
        VisibleSections[num_visible_sections] = static_cast<short>(section_number);
        if (MostVisibleSections < NumVisibleSections) {
            MostVisibleSections = NumVisibleSections;
        }
    }
}

int DrivableScenerySection::IsSectionVisible(int section_number) {
    for (int i = 0; i < NumVisibleSections; i++) {
        if (VisibleSections[i] == section_number) {
            return 1;
        }
    }
    return 0;
}

void DrivableScenerySection::RemoveVisibleSection(int section_number) {
    if (NumVisibleSections < 1) {
        return;
    }

    short *visible_sections = VisibleSections;
    for (int i = 0; i < NumVisibleSections; i++) {
        if (visible_sections[i] == section_number) {
            if (i < NumVisibleSections - 1) {
                do {
                    int next = i + 1;
                    visible_sections[i] = visible_sections[next];
                    i = next;
                } while (i < NumVisibleSections - 1);
            }

            short num_visible_sections = NumVisibleSections - 1;
            NumVisibleSections = num_visible_sections;
            visible_sections[num_visible_sections] = 0;
            return;
        }
    }
}

void DrivableScenerySection::SortVisibleSections() {
    bool swap;
    do {
        swap = false;
        if (NumVisibleSections - 1 > 0) {
            for (int i = 0; i < NumVisibleSections - 1; i++) {
                short a = VisibleSections[i];
                short b = VisibleSections[i + 1];
                if (b < a) {
                    VisibleSections[i + 1] = a;
                    swap = true;
                    VisibleSections[i] = b;
                }
            }
        }
    } while (swap);
}

void LoadingSection::EndianSwap() {
    bPlatEndianSwap(&NumDrivableSections);
    for (int i = 0; i < NumDrivableSections; i++) {
        bPlatEndianSwap(&DrivableSections[i]);
    }
    bPlatEndianSwap(&NumExtraSections);
    for (int i = 0; i < NumExtraSections; i++) {
        bPlatEndianSwap(&ExtraSections[i]);
    }
}

void VisibleSectionOverlay::EndianSwap() {
    bPlatEndianSwap(&NumEntries);
    for (int n = 0; n < NumEntries; n++) {
        OverlayEntry *entry = &EntryTable[n];
        bPlatEndianSwap(&entry->DrivableSectionNumber);
        bPlatEndianSwap(&entry->SectionNumber);
    }
}

void VisibleSectionManagerInfo::EndianSwap() {
    bPlatEndianSwap(&LODOffset);
    TheDrivableSectionsInRegion.EndianSwap();
}

VisibleSectionUserInfo *VisibleSectionManager::AllocateUserInfo(int section_number) {
    VisibleSectionUserInfo *user_info = UserInfoTable[section_number];
    if (user_info) {
        user_info->ReferenceCount += 1;
        return user_info;
    }

    for (int i = 0; i < 512; i++) {
        user_info = &UserInfoStorageTable[i];
        if (user_info->ReferenceCount == 0) {
            user_info->ReferenceCount = 1;
            user_info->pScenerySectionHeader = 0;
            user_info->pLightFlarePack = 0;
            user_info->pLightPack = 0;
            user_info->pSmokeableSpawnerPack = 0;
            user_info->pParkedCarPack = 0;
            user_info->pEventTriggerPack = 0;
            UserInfoTable[section_number] = user_info;
            NumAllocatedUserInfo += 1;
            return user_info;
        }
    }

    return 0;
}

void VisibleSectionManager::UnallocateUserInfo(int section_number) {
    VisibleSectionUserInfo *user_info = UserInfoTable[section_number];
    if (!user_info) {
        return;
    }

    if (user_info->ReferenceCount > 1) {
        user_info->ReferenceCount -= 1;
        return;
    }

    user_info->ReferenceCount = 0;
    user_info->pScenerySectionHeader = 0;
    user_info->pLightFlarePack = 0;
    user_info->pLightPack = 0;
    user_info->pSmokeableSpawnerPack = 0;
    user_info->pParkedCarPack = 0;
    user_info->pEventTriggerPack = 0;
    UserInfoTable[section_number] = 0;
    NumAllocatedUserInfo -= 1;
}

VisibleSectionBoundary *VisibleSectionManager::FindBoundary(int section_number) {
    for (VisibleSectionBoundary *boundary = DrivableBoundaryList.GetHead(); boundary != DrivableBoundaryList.EndOfList();
         boundary = boundary->GetNext()) {
        if (boundary->SectionNumber == section_number) {
            return boundary;
        }
    }

    for (VisibleSectionBoundary *boundary = NonDrivableBoundaryList.GetHead(); boundary != NonDrivableBoundaryList.EndOfList();
         boundary = boundary->GetNext()) {
        if (boundary->SectionNumber == section_number) {
            return boundary;
        }
    }

    return 0;
}

VisibleSectionBoundary *VisibleSectionManager::FindClosestBoundary(const bVector2 *point, float *distance) {
    float closest_distance = 9999999.0f;
    VisibleSectionBoundary *closest_boundary = 0;

    for (VisibleSectionBoundary *boundary = DrivableBoundaryList.GetHead(); boundary != DrivableBoundaryList.EndOfList();
         boundary = boundary->GetNext()) {
        if (boundary->IsPointInside(point)) {
            closest_distance = 0.0f;
            DrivableBoundaryList.Remove(boundary);
            DrivableBoundaryList.AddHead(boundary);
            closest_boundary = boundary;
            break;
        }

        float boundary_distance = boundary->GetDistanceOutside(point, closest_distance);
        if (!closest_boundary || boundary_distance < closest_distance ||
            (boundary_distance == closest_distance && boundary->SectionNumber < closest_boundary->SectionNumber)) {
            closest_distance = boundary_distance;
            closest_boundary = boundary;
        }
    }

    if (distance) {
        *distance = closest_distance;
    }

    return closest_boundary;
}

VisibleSectionBoundary *VisibleSectionManager::FindBoundary(const bVector2 *point) {
    for (VisibleSectionBoundary *boundary = DrivableBoundaryList.GetHead(); boundary != DrivableBoundaryList.EndOfList();
         boundary = boundary->GetNext()) {
        if (boundary->IsPointInside(point)) {
            DrivableBoundaryList.Remove(boundary);
            DrivableBoundaryList.AddHead(boundary);
            return boundary;
        }
    }

    float distance_to_boundary;
    VisibleSectionBoundary *boundary = FindClosestBoundary(point, &distance_to_boundary);
    if (distance_to_boundary >= 0.1f) {
        return 0;
    }

    return boundary;
}

DrivableScenerySection *VisibleSectionManager::FindDrivableSection(const bVector2 *point) {
    for (DrivableScenerySection *section = DrivableSectionList.GetHead(); section != DrivableSectionList.EndOfList(); section = section->GetNext()) {
        if (section->pBoundary && PointInBBox(point, &section->pBoundary->BBoxMin, &section->pBoundary->BBoxMax)) {
            return section;
        }
    }

    return 0;
}

DrivableScenerySection *VisibleSectionManager::FindDrivableSection(int section_number) {
    for (DrivableScenerySection *section = DrivableSectionList.GetHead(); section != DrivableSectionList.EndOfList(); section = section->GetNext()) {
        if (section->SectionNumber == section_number) {
            return section;
        }
    }

    return 0;
}

LoadingSection *VisibleSectionManager::FindLoadingSection(int section_number) {
    for (LoadingSection *loading_section = LoadingSectionList.GetHead(); loading_section != LoadingSectionList.EndOfList();
         loading_section = loading_section->GetNext()) {
        for (int i = 0; i < loading_section->NumDrivableSections; i++) {
            if (loading_section->DrivableSections[i] == section_number) {
                return loading_section;
            }
        }
    }

    return 0;
}

int VisibleSectionManager::GetSectionsToLoad(LoadingSection *loading_section, short *section_numbers, int max_sections) {
    int num_sections = 0;
    if (!loading_section) {
        return num_sections;
    }

    for (int i = 0; i < loading_section->NumDrivableSections; i++) {
        DrivableScenerySection *drivable_section = FindDrivableSection(loading_section->DrivableSections[i]);
        if (!drivable_section) {
            continue;
        }

        for (int n = 0; n < drivable_section->NumVisibleSections; n++) {
            short section_number = drivable_section->VisibleSections[n];
            int existing_index = -1;
            for (int j = 0; j < num_sections; j++) {
                if (section_numbers[j] == section_number) {
                    existing_index = j;
                    break;
                }
            }

            if (existing_index < 0 && num_sections < max_sections) {
                section_numbers[num_sections] = section_number;
                num_sections += 1;
            }
        }
    }

    for (int i = 0; i < loading_section->NumExtraSections; i++) {
        short section_number = loading_section->ExtraSections[i];
        int existing_index = -1;
        for (int j = 0; j < num_sections; j++) {
            if (section_numbers[j] == section_number) {
                existing_index = j;
                break;
            }
        }

        if (existing_index < 0 && num_sections < max_sections) {
            section_numbers[num_sections] = section_number;
            num_sections += 1;
        }

        if (IsScenerySectionDrivable(section_number)) {
            short lod_section_number = static_cast<short>(section_number + ScenerySectionLODOffset);
            existing_index = -1;
            for (int j = 0; j < num_sections; j++) {
                if (section_numbers[j] == lod_section_number) {
                    existing_index = j;
                    break;
                }
            }

            if (existing_index < 0 && num_sections < max_sections) {
                section_numbers[num_sections] = lod_section_number;
                num_sections += 1;
            }
        }
    }

    return num_sections;
}

VisibleGroupInfo *VisibleSectionManager::GetGroupInfo(const char *selection_set_name) {
    VisibleGroupInfo *group_info = VisibleGroupInfoTable;
    for (int i = 0; i < 5; i++) {
        int name_length = bStrLen(group_info->SelectionSetName);
        if (bStrNICmp(selection_set_name, group_info->SelectionSetName, name_length) == 0) {
            return group_info;
        }
        group_info += 1;
    }
    return 0;
}

void VisibleSectionManager::ActivateOverlay(const char *name) {
    VisibleSectionOverlay *overlay = OverlayList.GetHead();
    while (overlay != OverlayList.EndOfList()) {
        if (bStrICmp(overlay->Name, name) == 0) {
            break;
        }
        overlay = overlay->GetNext();
    }

    if (overlay == OverlayList.EndOfList() || overlay == pActiveOverlay) {
        return;
    }

    if (pActiveOverlay) {
        UnactivateOverlay();
    }

    DisablePrecullerCounter += 1;
    pActiveOverlay = overlay;
    VisibleSectionOverlay *undo_overlay = new VisibleSectionOverlay;
    undo_overlay->NumEntries = 0;
    bMemSet(undo_overlay->Name, 0, sizeof(undo_overlay->Name));
    bSafeStrCpy(undo_overlay->Name, "Undo", sizeof(undo_overlay->Name));
    pUndoOverlay = undo_overlay;
    ActivateOverlay(overlay, undo_overlay);
    RefreshTrackStreamer();
}

void VisibleSectionManager::ActivateOverlay(VisibleSectionOverlay *overlay, VisibleSectionOverlay *undo_overlay) {
    for (int i = 0; i < overlay->NumEntries; i++) {
        OverlayEntry *entry = &overlay->EntryTable[i];
        DrivableScenerySection *section = FindDrivableSection(entry->DrivableSectionNumber);
        if (!section) {
            continue;
        }

        bool changed = false;
        if (entry->AddRemove == 0) {
            if (section->IsSectionVisible(entry->SectionNumber)) {
                changed = true;
                section->RemoveVisibleSection(entry->SectionNumber);
            }
        } else if (!section->IsSectionVisible(entry->SectionNumber)) {
            changed = true;
            section->AddVisibleSection(entry->SectionNumber);
        }

        if (changed) {
            section->SortVisibleSections();
            if (undo_overlay) {
                OverlayEntry *undo_entry = &undo_overlay->EntryTable[undo_overlay->NumEntries];
                undo_overlay->NumEntries += 1;
                *undo_entry = *entry;
                undo_entry->AddRemove = entry->AddRemove == 0;
            }
        }
    }
}

void VisibleSectionManager::UnactivateOverlay() {
    if (pActiveOverlay) {
        DisablePrecullerCounter -= 1;
        ActivateOverlay(pUndoOverlay, 0);
        if (pUndoOverlay) {
            delete pUndoOverlay;
        }
        pActiveOverlay = 0;
    }
}

void VisibleSectionManager::EnableGroup(unsigned int group_name) {
    for (int i = 0; i < 0x100; i++) {
        if (EnabledGroups[i] == 0) {
            EnabledGroups[i] = group_name;
            return;
        }
    }
}

int VisibleSectionManager::Unloader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034150) {
        pInfo = 0;
        NonDrivableBoundaryList.InitList();
        LoadingSectionList.InitList();
        DrivableSectionList.InitList();
        DrivableBoundaryList.InitList();
        return 1;
    }

    if (chunk->GetID() != 0x34158) {
        return 0;
    }

    reinterpret_cast<VisibleSectionOverlay *>(chunk->GetData())->Remove();
    return 1;
}

int Get2PlayerSectionNumber(int section_number, const char *build_platform) {
    if (bStrICmp(build_platform, "PC") != 0) {
        char section_letter = GetScenerySectionLetter(section_number);
        if (section_letter == 'Y') {
            return GetScenerySubsectionNumber(section_number) + 0x8FC;
        }

        if (section_letter == 'X') {
            return GetScenerySubsectionNumber(section_number) + 0x834;
        }

        SectionRemapper *remap_table = SectionRemapperTable;
        int table_size = 134;
        if (bStrICmp(build_platform, "GAMECUBE") == 0) {
            table_size = 129;
            remap_table = SectionRemapperTable_Gamecube;
        }

        for (int n = 0; n < table_size; n++) {
            if (remap_table[n].SectionNumber == section_number) {
                return remap_table[n].SectionNumber2P;
            }
        }
    }

    return section_number;
}

int Get2PlayerSectionNumber(int section_number) {
    return Get2PlayerSectionNumber(section_number, bGetPlatformName());
}

int Get1PlayerSectionNumber(int section_number_2p, const char *build_platform) {
    if (!initialized_VisibleSection) {
        initialized_VisibleSection = true;
        bMemSet(map_table_VisibleSection, 0, sizeof(map_table_VisibleSection));
        for (int sec_1p = 0; sec_1p < 2800; sec_1p++) {
            int sec_2p = Get2PlayerSectionNumber(sec_1p, build_platform);
            if (sec_2p != sec_1p) {
                map_table_VisibleSection[sec_2p] = sec_1p;
            }
        }
    }

    int sec_1p = map_table_VisibleSection[section_number_2p];
    if (sec_1p == 0) {
        sec_1p = section_number_2p;
    }

    return sec_1p;
}

int GetBoundarySectionNumber(int section_number, const char *platform_name) {
    int boundary_section_number = Get1PlayerSectionNumber(section_number, platform_name);
    int subsection_number = GetScenerySubsectionNumber(boundary_section_number);

    if (ScenerySectionLODOffset <= subsection_number && subsection_number < ScenerySectionLODOffset * 2) {
        boundary_section_number -= ScenerySectionLODOffset;
    }

    return boundary_section_number;
}

static bool MyIsPointInPoly(const bVector2 *point, const bVector2 *points, int num_points) {
    float x = point->x;
    float y = point->y;
    bool inside = false;
    int j = num_points - 1;

    for (int i = 0; i < num_points; i++) {
        float point_y = points[i].y;
        if (((point_y <= y && y < points[j].y) || (points[j].y <= y && y < point_y)) &&
            x < ((points[j].x - points[i].x) * (y - point_y)) / (points[j].y - point_y) + points[i].x) {
            inside = !inside;
        }

        j = i;
    }

    return inside;
}

int VisibleSectionManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034150) {
        bChunk *current_chunk = chunk->GetFirstChunk();
        bChunk *last_chunk = chunk->GetLastChunk();

        while (current_chunk < last_chunk) {
            switch (current_chunk->GetID()) {
                case 0x34152: {
                    VisibleSectionBoundary *boundary = reinterpret_cast<VisibleSectionBoundary *>(current_chunk->GetData());
                    VisibleSectionBoundary *last_boundary = reinterpret_cast<VisibleSectionBoundary *>(current_chunk->GetLastChunk());

                    if (boundary < last_boundary) {
                        do {
                            bPlatEndianSwap(&boundary->SectionNumber);
                            bPlatEndianSwap(&boundary->BBoxMin);
                            bPlatEndianSwap(&boundary->BBoxMax);
                            bPlatEndianSwap(&boundary->Centre);
                            for (int i = 0; i < boundary->NumPoints; i++) {
                                bPlatEndianSwap(&boundary->Points[i]);
                            }

                            if (IsScenerySectionDrivable(boundary->SectionNumber)) {
                                DrivableBoundaryList.AddTail(boundary);
                            } else {
                                NonDrivableBoundaryList.AddTail(boundary);
                            }

                            boundary = reinterpret_cast<VisibleSectionBoundary *>(
                                reinterpret_cast<char *>(boundary) + 0x24 + boundary->NumPoints * sizeof(bVector2));
                        } while (boundary < last_boundary);
                    }
                    break;
                }

                case 0x34153: {
                    DrivableScenerySection *section = reinterpret_cast<DrivableScenerySection *>(current_chunk->GetData());
                    DrivableScenerySection *last_section = reinterpret_cast<DrivableScenerySection *>(current_chunk->GetLastChunk());

                    if (section < last_section) {
                        do {
                            DrivableSectionList.AddTail(section);
                            bPlatEndianSwap(&section->SectionNumber);
                            bPlatEndianSwap(&section->NumVisibleSections);
                            for (int i = 0; i < section->NumVisibleSections; i++) {
                                bPlatEndianSwap(&section->VisibleSections[i]);
                            }
                            section->pBoundary = FindBoundary(section->SectionNumber);
                            section = reinterpret_cast<DrivableScenerySection *>(
                                reinterpret_cast<char *>(section) + 0x14 + section->NumVisibleSections * sizeof(short));
                        } while (section < last_section);
                    }

                    pSectionD9 = FindDrivableSection(0x199);
                    pSectionC14 = FindDrivableSection(0x13A);
                    break;
                }

                case 0x34151: {
                    pInfo = reinterpret_cast<VisibleSectionManagerInfo *>(current_chunk->GetData());
                    bPlatEndianSwap(&pInfo->LODOffset);
                    bPlatEndianSwap(&pInfo->TheDrivableSectionsInRegion.NumSections);
                    for (int i = 0; i < pInfo->TheDrivableSectionsInRegion.NumSections; i++) {
                        bPlatEndianSwap(&pInfo->TheDrivableSectionsInRegion.Sections[i]);
                    }
                    ScenerySectionLODOffset = pInfo->LODOffset;
                    break;
                }

                case 0x34155: {
                    int num_loading_sections = current_chunk->Size / sizeof(LoadingSection);
                    if (num_loading_sections != 0) {
                        for (int n = 0; n < num_loading_sections; n++) {
                            LoadingSection *section =
                                reinterpret_cast<LoadingSection *>(reinterpret_cast<char *>(current_chunk) + n * sizeof(LoadingSection) + 8);
                            LoadingSectionList.AddTail(section);
                            bPlatEndianSwap(&section->NumDrivableSections);
                            for (int i = 0; i < section->NumDrivableSections; i++) {
                                bPlatEndianSwap(&section->DrivableSections[i]);
                            }
                            bPlatEndianSwap(&section->NumExtraSections);
                            for (int i = 0; i < section->NumExtraSections; i++) {
                                bPlatEndianSwap(&section->ExtraSections[i]);
                            }
                        }
                    }
                    break;
                }

                case 0x34154:
                default:
                    break;
            }

            current_chunk = current_chunk->GetNext();
        }

        InitVisibleZones();
        RefreshTrackStreamer();
    } else {
        if (chunk->GetID() != 0x34158) {
            return 0;
        }

        VisibleSectionOverlay *overlay = reinterpret_cast<VisibleSectionOverlay *>(chunk->GetData());
        OverlayList.AddTail(overlay);
        bPlatEndianSwap(&overlay->NumEntries);
        for (int n = 0; n < overlay->NumEntries; n++) {
            OverlayEntry *entry = &overlay->EntryTable[n];
            bPlatEndianSwap(&entry->DrivableSectionNumber);
            bPlatEndianSwap(&entry->SectionNumber);
        }
    }

    return 1;
}
