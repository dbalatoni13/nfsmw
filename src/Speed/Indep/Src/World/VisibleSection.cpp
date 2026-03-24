#include "VisibleSection.hpp"

#include "Scenery.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void RefreshTrackStreamer();
BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width);
float bDistToLine(const bVector2 *point, const bVector2 *pointa, const bVector2 *pointb);

struct SectionRemapper {
    short SectionNumber;
    short SectionNumber2P;
};

extern SectionRemapper SectionRemapperTable_Gamecube[129];
extern SectionRemapper SectionRemapperTable[134];
extern VisibleSectionManager TheVisibleSectionManager;

static bool initialized_VisibleSection = false;
static int map_table_VisibleSection[2800];
static int counter_VisibleSection = 0;
static char text_VisibleSection[4][16];

int Get2PlayerSectionNumber(int section_number, const char *build_platform) {
    if (bStrICmp(build_platform, "PC") != 0) {
        char section_letter = GetScenerySectionLetter(section_number);
        if (section_letter == 'Y') {
            return static_cast<short>(section_number % 100 + 0x8FC);
        }

        if (section_letter == 'X') {
            return static_cast<short>(section_number % 100 + 0x834);
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

    if (map_table_VisibleSection[section_number_2p] != 0) {
        return map_table_VisibleSection[section_number_2p];
    }
    return section_number_2p;
}

int GetBoundarySectionNumber(int section_number, const char *platform_name) {
    int boundary_section_number = Get1PlayerSectionNumber(section_number, platform_name);
    int subsection_number = boundary_section_number % 100;
    int is_boundary_section = 0;

    if (subsection_number >= ScenerySectionLODOffset) {
        is_boundary_section = subsection_number < ScenerySectionLODOffset * 2;
    }

    if (is_boundary_section) {
        boundary_section_number -= ScenerySectionLODOffset;
    }

    return boundary_section_number;
}

int LoaderVisibleSections(bChunk *chunk) {
    return TheVisibleSectionManager.Loader(chunk);
}

int UnloaderVisibleSections(bChunk *chunk) {
    return TheVisibleSectionManager.Unloader(chunk);
}

char *GetScenerySectionName(char *name, int section_number) {
    if (section_number < 1) {
        name[0] = '-';
        name[1] = '-';
        name[2] = '\0';
    } else {
        bSPrintf(name, "%c%d", GetScenerySectionLetter(section_number), section_number % 100);
    }

    return name;
}

char *GetScenerySectionName(int section_number) {
    unsigned int index = static_cast<unsigned int>(counter_VisibleSection) & 3;
    counter_VisibleSection += 1;
    return GetScenerySectionName(text_VisibleSection[index], section_number);
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

static inline bool PointInBBox(const bVector2 *point, const bVector2 *bbox_min, const bVector2 *bbox_max) {
    return point->x >= bbox_min->x && point->x <= bbox_max->x && point->y >= bbox_min->y && point->y <= bbox_max->y;
}

bool VisibleSectionBoundary::IsPointInside(const bVector2 *point) {
    if (!bBoundingBoxIsInside(&BBoxMin, &BBoxMax, point, 0.0f)) {
        return false;
    }

    return MyIsPointInPoly(point, Points, NumPoints);
}

float VisibleSectionBoundary::GetDistanceOutside(const bVector2 *point, float max_distance) {
    if (!bBoundingBoxIsInside(&BBoxMin, &BBoxMax, point, max_distance)) {
        return max_distance;
    }

    if (IsPointInside(point)) {
        return 0.0f;
    }

    float closest_distance = max_distance;
    {
        int point_number = 0;
        while (point_number < NumPoints) {
            int next = point_number + 1;
            bVector2 *point1 = GetPoint(point_number);
            bVector2 *point2 = GetPoint(next - (next / NumPoints) * NumPoints);
            float distance = bDistToLine(point, point1, point2);
            if (distance < closest_distance) {
                closest_distance = distance;
            }
            point_number = next;
        }
    }

    return closest_distance;
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
    {
        int n = 0;
        if (n >= NumVisibleSections) {
            return;
        }

        do {
            if (VisibleSections[n] == section_number) {
                {
                    int i = n;

                    if (i < NumVisibleSections - 1) {
                        do {
                            VisibleSections[i] = VisibleSections[i + 1];
                            i++;
                        } while (i < NumVisibleSections - 1);
                    }
                }

                NumVisibleSections--;
                VisibleSections[NumVisibleSections] = 0;
                return;
            }

            n++;
        } while (n < NumVisibleSections);
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

LoadingSection *VisibleSectionManager::FindLoadingSection(int section_number) {
    for (LoadingSection *loading_section = LoadingSectionList.GetHead(); loading_section != LoadingSectionList.EndOfList();
         loading_section = loading_section->GetNext()) {
        short target_section_number = static_cast<short>(section_number);
        short *drivable_sections = loading_section->DrivableSections;
        int num_drivable_sections = loading_section->NumDrivableSections;

        for (int i = 0; i < num_drivable_sections; i++) {
            if (drivable_sections[i] == target_section_number) {
                return loading_section;
            }
        }
    }

    return 0;
}

int VisibleSectionManager::GetSectionsToLoad(LoadingSection *loading_section, short *section_numbers, int max_sections) {
    if (!loading_section) {
        return 0;
    }

    int num_sections = 0;
    for (int n = 0; n < loading_section->NumDrivableSections; n++) {
        DrivableScenerySection *drivable_section = FindDrivableSection(loading_section->DrivableSections[n]);
        if (!drivable_section) {
            continue;
        }

        for (int i = 0; i < drivable_section->GetNumVisibleSections(); i++) {
            int section_number = drivable_section->GetVisibleSection(i);
            if (!HasSection(section_numbers, num_sections, static_cast<short>(section_number)) && num_sections < max_sections) {
                section_numbers[num_sections] = section_number;
                num_sections += 1;
            }
        }
    }

    for (int i = 0; i < loading_section->NumExtraSections; i++) {
        int section_number = loading_section->ExtraSections[i];
        if (!HasSection(section_numbers, num_sections, static_cast<short>(section_number)) && num_sections < max_sections) {
            section_numbers[num_sections] = section_number;
            num_sections += 1;
        }

        if (IsScenerySectionDrivable(section_number)) {
            section_number = GetLODScenerySectionNumber(section_number);
            if (!HasSection(section_numbers, num_sections, static_cast<short>(section_number)) && num_sections < max_sections) {
                section_numbers[num_sections] = section_number;
                num_sections += 1;
            }
        }
    }

    return num_sections;
}

VisibleGroupInfo VisibleGroupInfoTable[5] = {
    {"BARRIER_", 1},
    {"BARRIERS_", 1},
    {"PLAYER_BARRIERS_", 1},
    {"SCENERY_GROUP_", 1},
    {"FREE_ROAM", 0},
};
SectionRemapper SectionRemapperTable_Gamecube[129] = {
    {GetScenerySectionNumber('D', 7), GetScenerySectionNumber('K', 1)},
    {GetScenerySectionNumber('D', 47), GetScenerySectionNumber('K', 41)},
    {GetScenerySectionNumber('D', 21), GetScenerySectionNumber('K', 2)},
    {GetScenerySectionNumber('D', 61), GetScenerySectionNumber('K', 42)},
    {GetScenerySectionNumber('D', 29), GetScenerySectionNumber('K', 3)},
    {GetScenerySectionNumber('D', 69), GetScenerySectionNumber('K', 43)},
    {GetScenerySectionNumber('D', 35), GetScenerySectionNumber('K', 4)},
    {GetScenerySectionNumber('D', 75), GetScenerySectionNumber('K', 44)},
    {GetScenerySectionNumber('D', 38), GetScenerySectionNumber('K', 5)},
    {GetScenerySectionNumber('D', 78), GetScenerySectionNumber('K', 45)},
    {GetScenerySectionNumber('F', 4), GetScenerySectionNumber('K', 6)},
    {GetScenerySectionNumber('F', 44), GetScenerySectionNumber('K', 46)},
    {GetScenerySectionNumber('F', 11), GetScenerySectionNumber('K', 7)},
    {GetScenerySectionNumber('F', 51), GetScenerySectionNumber('K', 47)},
    {GetScenerySectionNumber('F', 13), GetScenerySectionNumber('K', 8)},
    {GetScenerySectionNumber('F', 53), GetScenerySectionNumber('K', 48)},
    {GetScenerySectionNumber('G', 2), GetScenerySectionNumber('K', 9)},
    {GetScenerySectionNumber('G', 42), GetScenerySectionNumber('K', 49)},
    {GetScenerySectionNumber('G', 3), GetScenerySectionNumber('K', 10)},
    {GetScenerySectionNumber('G', 43), GetScenerySectionNumber('K', 50)},
    {GetScenerySectionNumber('G', 4), GetScenerySectionNumber('K', 11)},
    {GetScenerySectionNumber('G', 44), GetScenerySectionNumber('K', 51)},
    {GetScenerySectionNumber('G', 8), GetScenerySectionNumber('K', 12)},
    {GetScenerySectionNumber('G', 48), GetScenerySectionNumber('K', 52)},
    {GetScenerySectionNumber('G', 9), GetScenerySectionNumber('K', 13)},
    {GetScenerySectionNumber('G', 49), GetScenerySectionNumber('K', 53)},
    {GetScenerySectionNumber('G', 10), GetScenerySectionNumber('K', 14)},
    {GetScenerySectionNumber('G', 50), GetScenerySectionNumber('K', 54)},
    {GetScenerySectionNumber('G', 12), GetScenerySectionNumber('K', 15)},
    {GetScenerySectionNumber('G', 52), GetScenerySectionNumber('K', 55)},
    {GetScenerySectionNumber('G', 14), GetScenerySectionNumber('K', 16)},
    {GetScenerySectionNumber('G', 54), GetScenerySectionNumber('K', 56)},
    {GetScenerySectionNumber('G', 15), GetScenerySectionNumber('K', 17)},
    {GetScenerySectionNumber('G', 55), GetScenerySectionNumber('K', 57)},
    {GetScenerySectionNumber('G', 20), GetScenerySectionNumber('K', 18)},
    {GetScenerySectionNumber('G', 60), GetScenerySectionNumber('K', 58)},
    {GetScenerySectionNumber('G', 22), GetScenerySectionNumber('K', 19)},
    {GetScenerySectionNumber('G', 62), GetScenerySectionNumber('K', 59)},
    {GetScenerySectionNumber('G', 29), GetScenerySectionNumber('K', 20)},
    {GetScenerySectionNumber('G', 69), GetScenerySectionNumber('K', 60)},
    {GetScenerySectionNumber('G', 30), GetScenerySectionNumber('K', 21)},
    {GetScenerySectionNumber('G', 70), GetScenerySectionNumber('K', 61)},
    {GetScenerySectionNumber('H', 2), GetScenerySectionNumber('K', 22)},
    {GetScenerySectionNumber('H', 42), GetScenerySectionNumber('K', 62)},
    {GetScenerySectionNumber('H', 3), GetScenerySectionNumber('K', 23)},
    {GetScenerySectionNumber('H', 43), GetScenerySectionNumber('K', 63)},
    {GetScenerySectionNumber('H', 4), GetScenerySectionNumber('K', 24)},
    {GetScenerySectionNumber('H', 44), GetScenerySectionNumber('K', 64)},
    {GetScenerySectionNumber('H', 10), GetScenerySectionNumber('K', 25)},
    {GetScenerySectionNumber('H', 50), GetScenerySectionNumber('K', 65)},
    {GetScenerySectionNumber('H', 11), GetScenerySectionNumber('K', 26)},
    {GetScenerySectionNumber('H', 51), GetScenerySectionNumber('K', 66)},
    {GetScenerySectionNumber('H', 13), GetScenerySectionNumber('K', 27)},
    {GetScenerySectionNumber('H', 53), GetScenerySectionNumber('K', 67)},
    {GetScenerySectionNumber('H', 15), GetScenerySectionNumber('K', 28)},
    {GetScenerySectionNumber('H', 55), GetScenerySectionNumber('K', 68)},
    {GetScenerySectionNumber('H', 18), GetScenerySectionNumber('K', 29)},
    {GetScenerySectionNumber('H', 58), GetScenerySectionNumber('K', 69)},
    {GetScenerySectionNumber('I', 7), GetScenerySectionNumber('K', 30)},
    {GetScenerySectionNumber('I', 47), GetScenerySectionNumber('K', 70)},
    {GetScenerySectionNumber('I', 13), GetScenerySectionNumber('K', 31)},
    {GetScenerySectionNumber('I', 53), GetScenerySectionNumber('K', 71)},
    {GetScenerySectionNumber('I', 19), GetScenerySectionNumber('K', 32)},
    {GetScenerySectionNumber('I', 59), GetScenerySectionNumber('K', 72)},
    {GetScenerySectionNumber('I', 20), GetScenerySectionNumber('K', 33)},
    {GetScenerySectionNumber('I', 60), GetScenerySectionNumber('K', 73)},
    {GetScenerySectionNumber('I', 22), GetScenerySectionNumber('K', 34)},
    {GetScenerySectionNumber('I', 62), GetScenerySectionNumber('K', 74)},
    {GetScenerySectionNumber('I', 31), GetScenerySectionNumber('K', 35)},
    {GetScenerySectionNumber('I', 71), GetScenerySectionNumber('K', 75)},
    {GetScenerySectionNumber('I', 32), GetScenerySectionNumber('K', 36)},
    {GetScenerySectionNumber('I', 72), GetScenerySectionNumber('K', 76)},
    {GetScenerySectionNumber('J', 9), GetScenerySectionNumber('K', 37)},
    {GetScenerySectionNumber('J', 49), GetScenerySectionNumber('K', 77)},
    {GetScenerySectionNumber('J', 10), GetScenerySectionNumber('K', 38)},
    {GetScenerySectionNumber('J', 50), GetScenerySectionNumber('K', 78)},
    {GetScenerySectionNumber('O', 9), GetScenerySectionNumber('K', 39)},
    {GetScenerySectionNumber('O', 49), GetScenerySectionNumber('K', 79)},
    {GetScenerySectionNumber('P', 4), GetScenerySectionNumber('N', 1)},
    {GetScenerySectionNumber('P', 44), GetScenerySectionNumber('N', 41)},
    {GetScenerySectionNumber('P', 5), GetScenerySectionNumber('N', 2)},
    {GetScenerySectionNumber('P', 45), GetScenerySectionNumber('N', 42)},
    {GetScenerySectionNumber('P', 7), GetScenerySectionNumber('N', 3)},
    {GetScenerySectionNumber('P', 47), GetScenerySectionNumber('N', 43)},
    {GetScenerySectionNumber('P', 10), GetScenerySectionNumber('N', 4)},
    {GetScenerySectionNumber('P', 50), GetScenerySectionNumber('N', 44)},
    {GetScenerySectionNumber('P', 17), GetScenerySectionNumber('N', 5)},
    {GetScenerySectionNumber('P', 57), GetScenerySectionNumber('N', 45)},
    {GetScenerySectionNumber('P', 22), GetScenerySectionNumber('N', 6)},
    {GetScenerySectionNumber('P', 62), GetScenerySectionNumber('N', 46)},
    {GetScenerySectionNumber('P', 33), GetScenerySectionNumber('N', 7)},
    {GetScenerySectionNumber('P', 73), GetScenerySectionNumber('N', 47)},
    {GetScenerySectionNumber('Q', 3), GetScenerySectionNumber('N', 8)},
    {GetScenerySectionNumber('Q', 43), GetScenerySectionNumber('N', 48)},
    {GetScenerySectionNumber('Q', 5), GetScenerySectionNumber('N', 9)},
    {GetScenerySectionNumber('Q', 45), GetScenerySectionNumber('N', 49)},
    {GetScenerySectionNumber('Q', 10), GetScenerySectionNumber('N', 10)},
    {GetScenerySectionNumber('Q', 50), GetScenerySectionNumber('N', 50)},
    {GetScenerySectionNumber('Q', 12), GetScenerySectionNumber('N', 11)},
    {GetScenerySectionNumber('Q', 52), GetScenerySectionNumber('N', 51)},
    {GetScenerySectionNumber('Q', 16), GetScenerySectionNumber('N', 12)},
    {GetScenerySectionNumber('Q', 56), GetScenerySectionNumber('N', 52)},
    {GetScenerySectionNumber('Q', 23), GetScenerySectionNumber('N', 13)},
    {GetScenerySectionNumber('Q', 63), GetScenerySectionNumber('N', 53)},
    {GetScenerySectionNumber('R', 1), GetScenerySectionNumber('N', 14)},
    {GetScenerySectionNumber('R', 41), GetScenerySectionNumber('N', 54)},
    {GetScenerySectionNumber('R', 3), GetScenerySectionNumber('N', 15)},
    {GetScenerySectionNumber('R', 43), GetScenerySectionNumber('N', 55)},
    {GetScenerySectionNumber('R', 4), GetScenerySectionNumber('N', 16)},
    {GetScenerySectionNumber('R', 44), GetScenerySectionNumber('N', 56)},
    {GetScenerySectionNumber('R', 7), GetScenerySectionNumber('N', 17)},
    {GetScenerySectionNumber('R', 47), GetScenerySectionNumber('N', 57)},
    {GetScenerySectionNumber('R', 16), GetScenerySectionNumber('N', 18)},
    {GetScenerySectionNumber('R', 56), GetScenerySectionNumber('N', 58)},
    {GetScenerySectionNumber('R', 18), GetScenerySectionNumber('N', 19)},
    {GetScenerySectionNumber('R', 58), GetScenerySectionNumber('N', 59)},
    {GetScenerySectionNumber('R', 26), GetScenerySectionNumber('N', 20)},
    {GetScenerySectionNumber('R', 66), GetScenerySectionNumber('N', 60)},
    {GetScenerySectionNumber('R', 33), GetScenerySectionNumber('N', 21)},
    {GetScenerySectionNumber('R', 73), GetScenerySectionNumber('N', 61)},
    {GetScenerySectionNumber('V', 1), GetScenerySectionNumber('V', 99)},
    {GetScenerySectionNumber('V', 4), GetScenerySectionNumber('V', 98)},
    {GetScenerySectionNumber('V', 6), GetScenerySectionNumber('V', 97)},
    {GetScenerySectionNumber('V', 14), GetScenerySectionNumber('V', 96)},
    {GetScenerySectionNumber('V', 19), GetScenerySectionNumber('V', 95)},
    {GetScenerySectionNumber('V', 22), GetScenerySectionNumber('V', 94)},
    {GetScenerySectionNumber('V', 59), GetScenerySectionNumber('V', 93)},
    {GetScenerySectionNumber('V', 71), GetScenerySectionNumber('V', 92)},
    {GetScenerySectionNumber('V', 70), GetScenerySectionNumber('V', 91)},
};
SectionRemapper SectionRemapperTable[134] = {
    {GetScenerySectionNumber('Q', 3), GetScenerySectionNumber('Q', 39)},
    {GetScenerySectionNumber('Q', 43), GetScenerySectionNumber('Q', 79)},
    {GetScenerySectionNumber('G', 8), GetScenerySectionNumber('G', 39)},
    {GetScenerySectionNumber('G', 48), GetScenerySectionNumber('G', 79)},
    {GetScenerySectionNumber('F', 11), GetScenerySectionNumber('F', 39)},
    {GetScenerySectionNumber('F', 51), GetScenerySectionNumber('F', 79)},
    {GetScenerySectionNumber('H', 10), GetScenerySectionNumber('H', 39)},
    {GetScenerySectionNumber('H', 50), GetScenerySectionNumber('H', 79)},
    {GetScenerySectionNumber('Q', 12), GetScenerySectionNumber('Q', 38)},
    {GetScenerySectionNumber('Q', 52), GetScenerySectionNumber('Q', 78)},
    {GetScenerySectionNumber('G', 12), GetScenerySectionNumber('G', 38)},
    {GetScenerySectionNumber('G', 52), GetScenerySectionNumber('G', 78)},
    {GetScenerySectionNumber('H', 4), GetScenerySectionNumber('H', 38)},
    {GetScenerySectionNumber('H', 44), GetScenerySectionNumber('H', 78)},
    {GetScenerySectionNumber('G', 10), GetScenerySectionNumber('G', 37)},
    {GetScenerySectionNumber('G', 50), GetScenerySectionNumber('G', 77)},
    {GetScenerySectionNumber('G', 20), GetScenerySectionNumber('G', 36)},
    {GetScenerySectionNumber('G', 60), GetScenerySectionNumber('G', 76)},
    {GetScenerySectionNumber('P', 17), GetScenerySectionNumber('Q', 37)},
    {GetScenerySectionNumber('P', 57), GetScenerySectionNumber('Q', 77)},
    {GetScenerySectionNumber('P', 4), GetScenerySectionNumber('Q', 33)},
    {GetScenerySectionNumber('P', 44), GetScenerySectionNumber('Q', 73)},
    {GetScenerySectionNumber('P', 5), GetScenerySectionNumber('Q', 34)},
    {GetScenerySectionNumber('P', 45), GetScenerySectionNumber('Q', 74)},
    {GetScenerySectionNumber('P', 7), GetScenerySectionNumber('Q', 35)},
    {GetScenerySectionNumber('P', 47), GetScenerySectionNumber('Q', 75)},
    {GetScenerySectionNumber('P', 10), GetScenerySectionNumber('Q', 36)},
    {GetScenerySectionNumber('P', 50), GetScenerySectionNumber('Q', 76)},
    {GetScenerySectionNumber('D', 35), GetScenerySectionNumber('D', 39)},
    {GetScenerySectionNumber('D', 75), GetScenerySectionNumber('D', 79)},
    {GetScenerySectionNumber('D', 16), GetScenerySectionNumber('C', 38)},
    {GetScenerySectionNumber('D', 56), GetScenerySectionNumber('C', 78)},
    {GetScenerySectionNumber('D', 21), GetScenerySectionNumber('C', 37)},
    {GetScenerySectionNumber('D', 61), GetScenerySectionNumber('C', 77)},
    {GetScenerySectionNumber('T', 6), GetScenerySectionNumber('T', 34)},
    {GetScenerySectionNumber('T', 46), GetScenerySectionNumber('T', 74)},
    {GetScenerySectionNumber('T', 8), GetScenerySectionNumber('T', 35)},
    {GetScenerySectionNumber('T', 48), GetScenerySectionNumber('T', 75)},
    {GetScenerySectionNumber('T', 9), GetScenerySectionNumber('T', 36)},
    {GetScenerySectionNumber('T', 49), GetScenerySectionNumber('T', 76)},
    {GetScenerySectionNumber('T', 13), GetScenerySectionNumber('T', 37)},
    {GetScenerySectionNumber('T', 53), GetScenerySectionNumber('T', 77)},
    {GetScenerySectionNumber('T', 17), GetScenerySectionNumber('T', 38)},
    {GetScenerySectionNumber('T', 57), GetScenerySectionNumber('T', 78)},
    {GetScenerySectionNumber('T', 22), GetScenerySectionNumber('T', 39)},
    {GetScenerySectionNumber('T', 62), GetScenerySectionNumber('T', 79)},
    {GetScenerySectionNumber('R', 16), GetScenerySectionNumber('S', 36)},
    {GetScenerySectionNumber('R', 56), GetScenerySectionNumber('S', 76)},
    {GetScenerySectionNumber('R', 26), GetScenerySectionNumber('S', 38)},
    {GetScenerySectionNumber('R', 66), GetScenerySectionNumber('S', 78)},
    {GetScenerySectionNumber('R', 17), GetScenerySectionNumber('S', 37)},
    {GetScenerySectionNumber('R', 57), GetScenerySectionNumber('S', 77)},
    {GetScenerySectionNumber('R', 18), GetScenerySectionNumber('S', 39)},
    {GetScenerySectionNumber('R', 58), GetScenerySectionNumber('S', 79)},
    {GetScenerySectionNumber('R', 3), GetScenerySectionNumber('S', 35)},
    {GetScenerySectionNumber('R', 43), GetScenerySectionNumber('S', 75)},
    {GetScenerySectionNumber('R', 7), GetScenerySectionNumber('S', 34)},
    {GetScenerySectionNumber('R', 47), GetScenerySectionNumber('S', 74)},
    {GetScenerySectionNumber('R', 33), GetScenerySectionNumber('S', 33)},
    {GetScenerySectionNumber('R', 73), GetScenerySectionNumber('S', 73)},
    {GetScenerySectionNumber('R', 1), GetScenerySectionNumber('S', 28)},
    {GetScenerySectionNumber('R', 41), GetScenerySectionNumber('S', 68)},
    {GetScenerySectionNumber('R', 2), GetScenerySectionNumber('S', 27)},
    {GetScenerySectionNumber('R', 42), GetScenerySectionNumber('S', 67)},
    {GetScenerySectionNumber('R', 14), GetScenerySectionNumber('S', 26)},
    {GetScenerySectionNumber('R', 54), GetScenerySectionNumber('S', 66)},
    {GetScenerySectionNumber('R', 20), GetScenerySectionNumber('S', 25)},
    {GetScenerySectionNumber('R', 60), GetScenerySectionNumber('S', 65)},
    {GetScenerySectionNumber('R', 21), GetScenerySectionNumber('S', 24)},
    {GetScenerySectionNumber('R', 61), GetScenerySectionNumber('S', 64)},
    {GetScenerySectionNumber('R', 27), GetScenerySectionNumber('S', 23)},
    {GetScenerySectionNumber('R', 67), GetScenerySectionNumber('S', 63)},
    {GetScenerySectionNumber('R', 28), GetScenerySectionNumber('L', 36)},
    {GetScenerySectionNumber('R', 68), GetScenerySectionNumber('L', 76)},
    {GetScenerySectionNumber('O', 35), GetScenerySectionNumber('S', 32)},
    {GetScenerySectionNumber('O', 75), GetScenerySectionNumber('S', 72)},
    {GetScenerySectionNumber('O', 9), GetScenerySectionNumber('S', 31)},
    {GetScenerySectionNumber('O', 49), GetScenerySectionNumber('S', 71)},
    {GetScenerySectionNumber('O', 26), GetScenerySectionNumber('S', 30)},
    {GetScenerySectionNumber('O', 66), GetScenerySectionNumber('S', 70)},
    {GetScenerySectionNumber('O', 32), GetScenerySectionNumber('S', 29)},
    {GetScenerySectionNumber('O', 72), GetScenerySectionNumber('S', 69)},
    {GetScenerySectionNumber('O', 35), GetScenerySectionNumber('L', 37)},
    {GetScenerySectionNumber('O', 75), GetScenerySectionNumber('L', 77)},
    {GetScenerySectionNumber('I', 2), GetScenerySectionNumber('I', 39)},
    {GetScenerySectionNumber('I', 42), GetScenerySectionNumber('I', 79)},
    {GetScenerySectionNumber('I', 3), GetScenerySectionNumber('I', 38)},
    {GetScenerySectionNumber('I', 43), GetScenerySectionNumber('I', 78)},
    {GetScenerySectionNumber('I', 1), GetScenerySectionNumber('I', 37)},
    {GetScenerySectionNumber('I', 41), GetScenerySectionNumber('I', 77)},
    {GetScenerySectionNumber('I', 20), GetScenerySectionNumber('I', 36)},
    {GetScenerySectionNumber('I', 60), GetScenerySectionNumber('I', 76)},
    {GetScenerySectionNumber('I', 22), GetScenerySectionNumber('I', 35)},
    {GetScenerySectionNumber('I', 62), GetScenerySectionNumber('I', 75)},
    {GetScenerySectionNumber('I', 31), GetScenerySectionNumber('L', 38)},
    {GetScenerySectionNumber('I', 71), GetScenerySectionNumber('L', 78)},
    {GetScenerySectionNumber('I', 32), GetScenerySectionNumber('L', 39)},
    {GetScenerySectionNumber('I', 72), GetScenerySectionNumber('L', 79)},
    {GetScenerySectionNumber('J', 9), GetScenerySectionNumber('J', 39)},
    {GetScenerySectionNumber('J', 49), GetScenerySectionNumber('J', 79)},
    {GetScenerySectionNumber('J', 10), GetScenerySectionNumber('J', 38)},
    {GetScenerySectionNumber('J', 50), GetScenerySectionNumber('J', 78)},
    {GetScenerySectionNumber('F', 6), GetScenerySectionNumber('F', 38)},
    {GetScenerySectionNumber('F', 46), GetScenerySectionNumber('F', 78)},
    {GetScenerySectionNumber('F', 13), GetScenerySectionNumber('F', 37)},
    {GetScenerySectionNumber('F', 53), GetScenerySectionNumber('F', 77)},
    {GetScenerySectionNumber('H', 2), GetScenerySectionNumber('H', 37)},
    {GetScenerySectionNumber('H', 42), GetScenerySectionNumber('H', 77)},
    {GetScenerySectionNumber('H', 3), GetScenerySectionNumber('H', 36)},
    {GetScenerySectionNumber('H', 43), GetScenerySectionNumber('H', 76)},
    {GetScenerySectionNumber('H', 11), GetScenerySectionNumber('H', 35)},
    {GetScenerySectionNumber('H', 51), GetScenerySectionNumber('H', 75)},
    {GetScenerySectionNumber('H', 15), GetScenerySectionNumber('H', 34)},
    {GetScenerySectionNumber('H', 55), GetScenerySectionNumber('H', 74)},
    {GetScenerySectionNumber('G', 17), GetScenerySectionNumber('G', 35)},
    {GetScenerySectionNumber('G', 57), GetScenerySectionNumber('G', 75)},
    {GetScenerySectionNumber('V', 14), GetScenerySectionNumber('V', 99)},
    {GetScenerySectionNumber('V', 4), GetScenerySectionNumber('V', 98)},
    {GetScenerySectionNumber('V', 41), GetScenerySectionNumber('V', 97)},
    {GetScenerySectionNumber('V', 31), GetScenerySectionNumber('V', 96)},
    {GetScenerySectionNumber('V', 55), GetScenerySectionNumber('V', 95)},
    {GetScenerySectionNumber('E', 2), GetScenerySectionNumber('E', 39)},
    {GetScenerySectionNumber('E', 42), GetScenerySectionNumber('E', 79)},
    {GetScenerySectionNumber('E', 3), GetScenerySectionNumber('E', 38)},
    {GetScenerySectionNumber('E', 43), GetScenerySectionNumber('E', 78)},
    {GetScenerySectionNumber('E', 5), GetScenerySectionNumber('E', 37)},
    {GetScenerySectionNumber('E', 45), GetScenerySectionNumber('E', 77)},
    {GetScenerySectionNumber('E', 8), GetScenerySectionNumber('E', 36)},
    {GetScenerySectionNumber('E', 48), GetScenerySectionNumber('E', 76)},
    {GetScenerySectionNumber('E', 1), GetScenerySectionNumber('E', 35)},
    {GetScenerySectionNumber('E', 41), GetScenerySectionNumber('E', 75)},
    {GetScenerySectionNumber('M', 12), GetScenerySectionNumber('M', 39)},
    {GetScenerySectionNumber('M', 52), GetScenerySectionNumber('M', 79)},
    {GetScenerySectionNumber('C', 9), GetScenerySectionNumber('C', 39)},
};

VisibleSectionManager TheVisibleSectionManager; // size: 0x6830
int ScenerySectionLODOffset = 0;
DrivableScenerySection *pSectionD9 = 0;
DrivableScenerySection *pSectionC14 = 0;
char *bGetPlatformName();
int bStrNICmp(const char *s1, const char *s2, int n);

VisibleSectionManager::VisibleSectionManager() {
    pBoundaryChunks = 0;
    pInfo = 0;
    pActiveOverlay = 0;
    pUndoOverlay = 0;

    bMemSet(UserInfoTable, 0, sizeof(UserInfoTable));
    NumAllocatedUserInfo = 0;

    bNode *head = &UnallocatedUserInfoList.HeadNode;
    for (int i = 0; i < 512; i++) {
        VisibleSectionUserInfo *user_info = &UserInfoStorageTable[i];
        bNode *tail = head->Prev;

        tail->Next = reinterpret_cast<bNode *>(user_info);
        head->Prev = reinterpret_cast<bNode *>(user_info);
        reinterpret_cast<bNode *>(user_info)->Prev = tail;
        reinterpret_cast<bNode *>(user_info)->Next = head;
    }

    VisibleBitTables = 0;
    bMemSet(EnabledGroups, 0, sizeof(EnabledGroups));
}

VisibleSectionUserInfo *VisibleSectionManager::AllocateUserInfo(int section_number) {
    VisibleSectionUserInfo *user_info = UserInfoTable[section_number];
    if (!user_info) {
        user_info = reinterpret_cast<VisibleSectionUserInfo *>(UnallocatedUserInfoList.HeadNode.Next);
        NumAllocatedUserInfo += 1;

        bNode *next = reinterpret_cast<bNode *>(user_info)->Next;
        bNode *prev = reinterpret_cast<bNode *>(user_info)->Prev;
        prev->Next = next;
        next->Prev = prev;

        bMemSet(user_info, 0, sizeof(VisibleSectionUserInfo));
        UserInfoTable[section_number] = user_info;
    }

    user_info->ReferenceCount += 1;
    return user_info;
}

void VisibleSectionManager::UnallocateUserInfo(int section_number) {
    VisibleSectionUserInfo *user_info = UserInfoTable[section_number];
    if (!user_info) {
        return;
    }

    int ref_count = user_info->ReferenceCount - 1;
    user_info->ReferenceCount = ref_count;
    if (ref_count != 0) {
        return;
    }

    bNode *tail = UnallocatedUserInfoList.HeadNode.Prev;
    NumAllocatedUserInfo -= 1;
    tail->Next = reinterpret_cast<bNode *>(user_info);
    UnallocatedUserInfoList.HeadNode.Prev = reinterpret_cast<bNode *>(user_info);
    reinterpret_cast<bNode *>(user_info)->Prev = tail;
    reinterpret_cast<bNode *>(user_info)->Next = &UnallocatedUserInfoList.HeadNode;
    UserInfoTable[section_number] = 0;
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

int VisibleSectionManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034150) {
        bChunk *first_chunk = chunk->GetFirstChunk();
        bChunk *current_chunk = first_chunk;
        bChunk *last_chunk = chunk->GetLastChunk();

        while (current_chunk < last_chunk) {
            unsigned int current_chunk_id = current_chunk->GetID();
            if (current_chunk_id == 0x34152) {
                VisibleSectionBoundary *boundary = reinterpret_cast<VisibleSectionBoundary *>(current_chunk->GetData());
                VisibleSectionBoundary *last_boundary = reinterpret_cast<VisibleSectionBoundary *>(current_chunk->GetLastChunk());

                if (boundary < last_boundary) {
                    do {
                        boundary->EndianSwap();
                        if (IsScenerySectionDrivable(boundary->GetSectionNumber())) {
                            DrivableBoundaryList.AddTail(boundary);
                        } else {
                            NonDrivableBoundaryList.AddTail(boundary);
                        }

                        boundary = reinterpret_cast<VisibleSectionBoundary *>(
                            reinterpret_cast<char *>(boundary) + boundary->GetMemoryImageSize());
                    } while (boundary < last_boundary);
                }
            } else if (current_chunk_id == 0x34153) {
                DrivableScenerySection *section = reinterpret_cast<DrivableScenerySection *>(current_chunk->GetData());
                DrivableScenerySection *last_section = reinterpret_cast<DrivableScenerySection *>(current_chunk->GetLastChunk());

                if (section < last_section) {
                    do {
                        DrivableSectionList.AddTail(section);
                        section->EndianSwap();
                        section->pBoundary = FindBoundary(section->GetSectionNumber());
                        int section_size = 0xA4 - (0x48 - section->MaxVisibleSections) * sizeof(short);
                        section = reinterpret_cast<DrivableScenerySection *>(
                            reinterpret_cast<char *>(section) + section_size);
                    } while (section < last_section);
                }

                pSectionD9 = FindDrivableSection(0x199);
                pSectionC14 = FindDrivableSection(0x13A);
            } else if (current_chunk_id == 0x34151) {
                pInfo = reinterpret_cast<VisibleSectionManagerInfo *>(current_chunk->GetData());
                pInfo->EndianSwap();
                ScenerySectionLODOffset = pInfo->LODOffset;
            } else if (current_chunk_id == 0x34154) {
            } else if (current_chunk_id == 0x34155) {
                LoadingSection *loading_sections = reinterpret_cast<LoadingSection *>(current_chunk->GetData());
                int num_loading_sections = current_chunk->Size / sizeof(LoadingSection);
                int n = 0;
                while (n < num_loading_sections) {
                    LoadingSection *section = &loading_sections[n];
                    LoadingSectionList.AddTail(section);
                    section->EndianSwap();
                    n += 1;
                }
            }

            current_chunk = current_chunk->GetNext();
        }

        InitVisibleZones();
        RefreshTrackStreamer();
        return 1;
    }

    if (chunk->GetID() == 0x34158) {
        VisibleSectionOverlay *overlay = reinterpret_cast<VisibleSectionOverlay *>(chunk->GetData());
        OverlayList.AddTail(overlay);
        overlay->EndianSwap();
        return 1;
    }

    return 0;
}
int VisibleSectionManager::Unloader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034150) {
        pInfo = 0;
        DrivableBoundaryList.InitList();
        NonDrivableBoundaryList.InitList();
        LoadingSectionList.InitList();
        DrivableSectionList.InitList();
        return 1;
    }

    if (chunk->GetID() == 0x34158) {
        reinterpret_cast<VisibleSectionOverlay *>(chunk->GetData())->Remove();
        return 1;
    }

    return 0;
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
        if (section->pBoundary->IsPointInside(point)) {
            DrivableSectionList.Remove(section);
            DrivableSectionList.AddHead(section);
            return section;
        }
    }

    float distance;
    VisibleSectionBoundary *boundary = FindClosestBoundary(point, &distance);
    if (distance < 0.1f) {
        return FindDrivableSection(boundary->SectionNumber);
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

void VisibleSectionManager::EnableGroup(unsigned int group_name) {
    for (int i = 0; i < 0x100; i++) {
        if (EnabledGroups[i] == 0) {
            EnabledGroups[i] = group_name;
            return;
        }
    }
}
