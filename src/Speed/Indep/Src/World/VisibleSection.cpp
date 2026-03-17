#include "VisibleSection.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

VisibleSectionManager TheVisibleSectionManager; // size: 0x6830
int ScenerySectionLODOffset = 0;
DrivableScenerySection *pSectionD9 = 0;
DrivableScenerySection *pSectionC14 = 0;

void RefreshTrackStreamer();
BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width);
int MyIsPointInPoly(const bVector2 *point, const bVector2 *polygon, int num_points);
float bDistToLine(const bVector2 *point, const bVector2 *pointa, const bVector2 *pointb);

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

int VisibleSectionManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x34158) {
        VisibleSectionOverlay *overlay = reinterpret_cast<VisibleSectionOverlay *>(chunk->GetData());
        OverlayList.AddTail(overlay);
        overlay->EndianSwap();
        return 1;
    }

    if (chunk->GetID() != 0x80034150) {
        return 0;
    }

    bChunk *first_chunk = chunk->GetFirstChunk();
    bChunk *last_chunk = chunk->GetLastChunk();
    for (bChunk *current_chunk = first_chunk; current_chunk < last_chunk; current_chunk = current_chunk->GetNext()) {
        switch (current_chunk->GetID()) {
            case 0x34152: {
                VisibleSectionBoundary *boundary = reinterpret_cast<VisibleSectionBoundary *>(current_chunk->GetData());
                VisibleSectionBoundary *last_boundary = reinterpret_cast<VisibleSectionBoundary *>(current_chunk->GetLastChunk());
                while (boundary < last_boundary) {
                    boundary->EndianSwap();
                    if (IsScenerySectionDrivable(boundary->GetSectionNumber())) {
                        DrivableBoundaryList.AddTail(boundary);
                    } else {
                        NonDrivableBoundaryList.AddTail(boundary);
                    }
                    boundary = reinterpret_cast<VisibleSectionBoundary *>(reinterpret_cast<char *>(boundary) + boundary->GetMemoryImageSize());
                }
                break;
            }

            case 0x34153: {
                DrivableScenerySection *section = reinterpret_cast<DrivableScenerySection *>(current_chunk->GetData());
                DrivableScenerySection *last_section = reinterpret_cast<DrivableScenerySection *>(current_chunk->GetLastChunk());
                while (section < last_section) {
                    DrivableSectionList.AddTail(section);
                    section->EndianSwap();
                    section->pBoundary = FindBoundary(section->GetSectionNumber());
                    section = reinterpret_cast<DrivableScenerySection *>(reinterpret_cast<char *>(section) + section->GetMemoryImageSize());
                }
                pSectionD9 = FindDrivableSection(GetScenerySectionNumber('D', 9));
                pSectionC14 = FindDrivableSection(GetScenerySectionNumber('C', 14));
                break;
            }

            case 0x34151: {
                pInfo = reinterpret_cast<VisibleSectionManagerInfo *>(current_chunk->GetData());
                pInfo->EndianSwap();
                ScenerySectionLODOffset = pInfo->LODOffset;
                break;
            }

            case 0x34155: {
                LoadingSection *loading_sections = reinterpret_cast<LoadingSection *>(current_chunk->GetData());
                int num_loading_sections = current_chunk->Size / sizeof(LoadingSection);
                for (int n = 0; n < num_loading_sections; n++) {
                    LoadingSection *section = &loading_sections[n];
                    LoadingSectionList.AddTail(section);
                    section->EndianSwap();
                }
                break;
            }

            case 0x34154:
            default:
                break;
        }
    }

    InitVisibleZones();
    RefreshTrackStreamer();
    return 1;
}
