#include "TrackStreamer.hpp"

#include "VisibleSection.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/bWare/Inc/bDebug.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bFunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern BOOL bMemoryTracing;
extern int ScenerySectionLODOffset;
extern int SeeulatorToolActive;
extern int ScenerySectionToBlink;
extern bool PostLoadFixupDisabled;
int Get2PlayerSectionNumber(int section_number);
void GetScenerySectionName(char *name, int section_number);
const char *GetScenerySectionName(int section_number);
void PostLoadFixup();

static unsigned int prev_need_loading_bar_26275 = 0;
static const float kMaxDistance_TrackStreamer = 3.4028235e+38f;
static const float kVelocityEpsilon_TrackStreamer = 0.0f;
static const float kFuturePositionScale_TrackStreamer = 0.5f;
static const float kPredictionScaleA_TrackStreamer = 1.0f;
static const float kPredictionScaleB_TrackStreamer = 1.0f;
static const float kLoadingBarDistanceThreshold_TrackStreamer = 15.0f;
static const float kLoadingBarSpeedThreshold_TrackStreamer = 100.0f;
static const float kSwitchZoneFarLoadThreshold_TrackStreamer = 0.1f;

static char GetScenerySectionLetter_TrackStreamer(int section_number) {
    return static_cast<char>(section_number / 100 + 'A' - 1);
}

static bool IsRegularScenerySection_TrackStreamer(int section_number) {
    char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
    return section_letter >= 'A' && section_letter < 'U';
}

static bool IsLoadingBarSection_TrackStreamer(int section_number) {
    if (!IsRegularScenerySection_TrackStreamer(section_number)) {
        return false;
    }

    int subsection_number = section_number % 100;
    return (subsection_number > 0 && subsection_number < ScenerySectionLODOffset) ||
           (ScenerySectionLODOffset <= subsection_number && subsection_number < ScenerySectionLODOffset * 2);
}

struct bBitTableLayout_TrackStreamer {
    int NumBits;
    uint8 *Bits;
};

static void SetBit_TrackStreamer(bBitTable *bit_table, int bit) {
    bBitTableLayout_TrackStreamer *layout = reinterpret_cast<bBitTableLayout_TrackStreamer *>(bit_table);
    layout->Bits[bit >> 3] |= static_cast<uint8>(1 << (bit & 7));
}

static void ClearTable_TrackStreamer(bBitTable *bit_table) {
    bBitTableLayout_TrackStreamer *layout = reinterpret_cast<bBitTableLayout_TrackStreamer *>(bit_table);
    bMemSet(layout->Bits, 0, layout->NumBits >> 3);
}

struct bMemoryTraceAllocatePacket {
    uintptr_t PoolID;
    uintptr_t MemoryAddress;
    int Size;
    int AllocationNumber;
    char Name[48];
};

TSMemoryNode *TSMemoryPool::GetNewNode(int address, int size, bool allocated, const char *debug_name) {
    TSMemoryNode *node = 0;

    if (!UnusedNodeList.IsEmpty()) {
        node = UnusedNodeList.RemoveHead();
    } else {
        for (int i = 0; i < 192; i++) {
            TSMemoryNode *candidate = &MemoryNodes[i];
            if (!candidate->Allocated && candidate->Size == 0 && candidate->DebugName[0] == 0) {
                node = candidate;
                break;
            }
        }
    }

    if (!node) {
        return 0;
    }

    node->Address = address;
    node->Size = size;
    node->Allocated = allocated;
    bMemSet(node->DebugName, 0, sizeof(node->DebugName));
    if (debug_name) {
        bStrNCpy(node->DebugName, debug_name, sizeof(node->DebugName) - 1);
    }
    NodeList.AddTail(node);
    return node;
}

void TSMemoryPool::RemoveNode(TSMemoryNode *node) {
    if (!node) {
        return;
    }

    NodeList.Remove(node);
    node->Address = 0;
    node->Size = 0;
    node->Allocated = false;
    bMemSet(node->DebugName, 0, sizeof(node->DebugName));
    UnusedNodeList.AddTail(node);
}

void *TSMemoryPool::Malloc(int size, const char *debug_name, bool best_fit, bool allocate_from_top, int address) {
    TSMemoryNode *found_node = 0;
    TSMemoryNode *new_node;
    int new_bottom_size;
    int new_top_size;

    size = (size + 0x7f) & ~0x7f;

    if (address != 0) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (!node->Allocated && node->Size >= size && address >= node->Address && address + size <= node->Address + node->Size) {
                found_node = node;
                break;
            }
        }
    } else if (best_fit) {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (!node->Allocated && node->Size >= size &&
                (!found_node || node->Size - size < found_node->Size - size)) {
                found_node = node;
            }
        }
    } else if (allocate_from_top) {
        for (TSMemoryNode *node = NodeList.GetTail(); node != NodeList.EndOfList(); node = node->GetPrev()) {
            if (!node->Allocated && node->Size >= size) {
                found_node = node;
                break;
            }
        }
    } else {
        for (TSMemoryNode *node = NodeList.GetHead(); node != NodeList.EndOfList(); node = node->GetNext()) {
            if (!node->Allocated && node->Size >= size) {
                found_node = node;
                break;
            }
        }
    }

    if (!found_node) {
        return 0;
    }

    if (address == 0) {
        if (allocate_from_top) {
            address = found_node->Address;
        } else {
            address = found_node->Address + found_node->Size - size;
        }
    }

    AmountFree -= size;
    if (found_node->Size == LargestFree) {
        NeedToRecalcLargestFree = true;
    }

    new_node = GetNewNode(address, size, true, debug_name);
    if (!new_node) {
        return 0;
    }
    new_node->AddAfter(found_node);

    new_bottom_size = address - found_node->Address;
    new_top_size = found_node->Address + found_node->Size - (address + size);
    found_node->Size = new_bottom_size;
    if (new_bottom_size == 0) {
        RemoveNode(found_node);
    }

    if (new_top_size != 0) {
        TSMemoryNode *top_node = GetNewNode(address + size, new_top_size, false, 0);
        if (top_node) {
            top_node->AddAfter(new_node);
        }
    }

    if (TracingEnabled && bMemoryTracing) {
        bMemoryTraceAllocatePacket packet;
        bMemSet(&packet, 0, sizeof(packet));
        packet.PoolID = reinterpret_cast<uintptr_t>(this);
        packet.MemoryAddress = static_cast<uintptr_t>(address);
        packet.Size = size;
        packet.AllocationNumber = AllocationNumber;
        if (debug_name) {
            bStrNCpy(packet.Name, debug_name, sizeof(packet.Name) - 1);
        }
        bFunkCallASync("CODEINE", 0x1c, &packet, bStrLen(packet.Name) + 0x15);
    }

    Updated = true;
    AllocationNumber += 1;
    return reinterpret_cast<void *>(address);
}

void *TrackStreamer::AllocateMemory(TrackStreamingSection *section, int allocation_params) {
    void *memory = bMalloc(section->Size, allocation_params | 0x2007);
    if (!memory) {
        bBreak();
    }
    return memory;
}

int TrackStreamer::AllocateSectionMemory(int *ptotal_needing_allocation) {
    int total_out_of_memory = 0;
    int total_needing_allocation = 0;
    int total_sections_allocated = 0;

    if (LoadingPhase == ALLOCATING_REGULAR_SECTIONS && pDiscBundleSections < pLastDiscBundleSection) {
        for (DiscBundleSection *disc_bundle = pDiscBundleSections; disc_bundle < pLastDiscBundleSection;
             disc_bundle = reinterpret_cast<DiscBundleSection *>(
                 reinterpret_cast<char *>(disc_bundle) + disc_bundle->NumMembers * sizeof(DiscBundleSectionMember) + 0x14)) {
            int member_index = 0;
            if (disc_bundle->NumMembers > 0) {
                do {
                    TrackStreamingSection *section = disc_bundle->Members[member_index].pSection;
                    if (!section->CurrentlyVisible || section->Status != TrackStreamingSection::UNLOADED) {
                        break;
                    }
                    member_index += 1;
                } while (member_index < disc_bundle->NumMembers);
            }

            if (member_index == disc_bundle->NumMembers) {
                int largest_free_block = pMemoryPool->GetLargestFreeBlock();
                if (disc_bundle->FileSize <= largest_free_block) {
                    void *bundle_memory = pMemoryPool->Malloc(disc_bundle->FileSize, disc_bundle->SectionName, true, false, 0);
                    pMemoryPool->Free(bundle_memory);

                    for (int i = 0; i < disc_bundle->NumMembers; i++) {
                        DiscBundleSectionMember *member = &disc_bundle->Members[i];
                        TrackStreamingSection *section = member->pSection;
                        char *section_memory = static_cast<char *>(bundle_memory) + member->FileOffset * 0x80;

                        total_sections_allocated += 1;
                        section->pDiscBundle = disc_bundle;
                        section->Status = TrackStreamingSection::ALLOCATED;
                        section->pMemory = section_memory;
                        pMemoryPool->Malloc(section->Size, disc_bundle->SectionName, false, false, reinterpret_cast<int>(section_memory));
                    }

                    total_needing_allocation += disc_bundle->FileSize;
                }
            }
        }
    }

    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (section->Status != TrackStreamingSection::UNLOADED) {
            continue;
        }

        bool should_allocate = true;
        int section_number = section->SectionNumber;
        if (section_number != 0x9c4 && section_number != 0x960 && section_number != 0x8fc && section_number != 0x834 &&
            section_number != 0xa28) {
            if (LoadingPhase == ALLOCATING_TEXTURE_SECTIONS) {
                should_allocate = false;
                char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
                if (section_letter == 'Y' || section_letter == 'W') {
                    should_allocate = true;
                }
            }

            if (LoadingPhase == ALLOCATING_GEOMETRY_SECTIONS) {
                should_allocate = false;
                char section_letter = GetScenerySectionLetter_TrackStreamer(section_number);
                if (section_letter == 'X' || section_letter == 'U') {
                    should_allocate = true;
                }
            }
        }

        if (!should_allocate) {
            continue;
        }

        total_needing_allocation += section->Size;
        if (bLargestMalloc(7) < section->Size) {
            total_out_of_memory += section->Size;
            NumSectionsOutOfMemory += 1;
        } else {
            total_sections_allocated += 1;
            section->pMemory = AllocateMemory(section, 0x80);
            section->Status = TrackStreamingSection::ALLOCATED;
            if (total_sections_allocated > 99999) {
                CurrentZoneAllocatedButIncomplete = true;
                return total_out_of_memory;
            }
        }
    }

    CurrentZoneAllocatedButIncomplete = false;
    *ptotal_needing_allocation = total_needing_allocation;
    return total_out_of_memory;
}

void TrackStreamer::RemoveCurrentStreamingSections() {
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        CurrentStreamingSections[i]->CurrentlyVisible = 0;
    }

    NumCurrentStreamingSections = 0;
    ClearTable_TrackStreamer(&CurrentVisibleSectionTable);
}

void TrackStreamer::AddCurrentStreamingSections(short *section_numbers, int num_sections, int position_number) {
    if (num_sections <= 0) {
        return;
    }

    StreamingPositionEntry *streaming_position = &StreamingPositionEntries[position_number];
    for (int i = 0; i < num_sections; i++) {
        short &section_number = section_numbers[i];
        SetBit_TrackStreamer(&CurrentVisibleSectionTable, section_number);
        if (SplitScreen) {
            section_number = static_cast<short>(Get2PlayerSectionNumber(section_number));
        }

        TrackStreamingSection *section = FindSection(section_number);
        if (!section) {
            continue;
        }

        section->LastNeededTimestamp = RealTimeFrames;
        if (!section->CurrentlyVisible) {
            CurrentStreamingSections[NumCurrentStreamingSections] = section;
            NumCurrentStreamingSections += 1;
        }

        unsigned char position_bit = static_cast<unsigned char>(1 << (position_number & 0x1f));
        if ((section->CurrentlyVisible & position_bit) == 0) {
            section->CurrentlyVisible |= position_bit;
            if (section->Status < TrackStreamingSection::LOADED) {
                streaming_position->NumSectionsToLoad += 1;
                streaming_position->AmountToLoad += section->Size;
            }
        }
    }
}

void TrackStreamer::DetermineStreamingSections() {
    short section_numbers[384];
    int num_sections = 3;

    RemoveCurrentStreamingSections();

    section_numbers[0] = 0x9c4;
    section_numbers[1] = 0x960;
    section_numbers[2] = 0xa28;
    if (SeeulatorToolActive && ScenerySectionToBlink != 0) {
        section_numbers[3] = static_cast<short>(ScenerySectionToBlink);
        num_sections = 4;
    }

    for (int i = 0; i < 4; i++) {
        short keep_section = KeepSectionTable[i];
        if (keep_section != 0) {
            section_numbers[num_sections] = keep_section;
            num_sections += 1;
        }
    }

    AddCurrentStreamingSections(section_numbers, num_sections, 0);
    AddCurrentStreamingSections(section_numbers, num_sections, 1);

    for (int i = 0; i < 2; i++) {
        StreamingPositionEntry *streaming_position = &StreamingPositionEntries[i];
        if (streaming_position->CurrentZone > 0) {
            LoadingSection *loading_section = TheVisibleSectionManager.FindLoadingSection(streaming_position->CurrentZone);
            if (!loading_section) {
                DrivableScenerySection *drivable_section = TheVisibleSectionManager.FindDrivableSection(streaming_position->CurrentZone);
                num_sections = 0;
                if (drivable_section) {
                    for (int n = 0; n < drivable_section->NumVisibleSections; n++) {
                        section_numbers[num_sections] = drivable_section->VisibleSections[n];
                        num_sections += 1;
                    }
                }
            } else {
                num_sections = TheVisibleSectionManager.GetSectionsToLoad(loading_section, section_numbers, 0x180);
            }

            AddCurrentStreamingSections(section_numbers, num_sections, i);
        }
    }
}

void TrackStreamer::PlotLoadingMarker(StreamingPositionEntry *streaming_position) {}

void TrackStreamer::SwitchZones(short *current_zones) {
    StartLoadingTime = GetDebugRealTime();
    CurrentZoneNeedsRefreshing = false;

    for (int i = 0; i < 2; i++) {
        short current_zone = current_zones[i];
        StreamingPositionEntry *streaming_position = &StreamingPositionEntries[i];
        if (streaming_position->CurrentZone != current_zone) {
            PlotLoadingMarker(streaming_position);

            VisibleSectionBoundary *old_boundary = TheVisibleSectionManager.FindBoundary(streaming_position->CurrentZone);
            VisibleSectionBoundary *new_boundary = TheVisibleSectionManager.FindBoundary(current_zone);
            float distance_to_boundary = kMaxDistance_TrackStreamer;
            if (old_boundary && new_boundary) {
                for (int n = 0; n < old_boundary->NumPoints; n++) {
                    float boundary_distance =
                        new_boundary->GetDistanceOutside(&old_boundary->Points[n], kMaxDistance_TrackStreamer);
                    if (boundary_distance < distance_to_boundary) {
                        distance_to_boundary = boundary_distance;
                    }
                }
            }

            if (kSwitchZoneFarLoadThreshold_TrackStreamer < distance_to_boundary) {
                CurrentZoneFarLoad = true;
            }

            streaming_position->CurrentZone = current_zone;
            streaming_position->BeginLoadingPosition = streaming_position->Position;
            streaming_position->BeginLoadingTime = GetDebugRealTime();
            streaming_position->NumSectionsToLoad = 0;
            streaming_position->NumSectionsLoaded = 0;
            streaming_position->AmountToLoad = 0;
            streaming_position->AmountLoaded = 0;
        }

        if (i == 0) {
            GetScenerySectionName(CurrentZoneName, current_zone);
        } else if (current_zone > 0) {
            bSPrintf(CurrentZoneName, "%s - %s", CurrentZoneName, GetScenerySectionName(current_zone));
        }
    }

    int num_sections_unactivated = 0;
    DetermineStreamingSections();
    PostLoadFixupDisabled = true;
    for (int i = 0; i < NumTrackStreamingSections; i++) {
        TrackStreamingSection *section = &pTrackStreamingSections[i];
        if (section->Status == TrackStreamingSection::ACTIVATED && !section->CurrentlyVisible) {
            char section_letter = GetScenerySectionLetter_TrackStreamer(section->SectionNumber);
            if (section_letter != 'Y' && section_letter != 'W' && section_letter != 'X' && section_letter != 'U') {
                num_sections_unactivated += 1;
                UnactivateSection(section);
            }
        }
    }
    PostLoadFixupDisabled = false;

    if (num_sections_unactivated > 0) {
        PostLoadFixup();
        SkipNextHandleLoad = true;
    }

    FreeSectionMemory();
    SetLoadingPhase(ALLOCATING_TEXTURE_SECTIONS);
    NumJettisonedSections = 0;
    CurrentZoneOutOfMemory = false;
    CurrentZoneAllocatedButIncomplete = false;
    MemorySafetyMargin = 0;
    AmountJettisoned = 0;
    bMemSet(JettisonedSections, 0, sizeof(JettisonedSections));
    AssignLoadingPriority();
    CalculateLoadingBacklog();
}

int TrackStreamer::GetSectionToActivate(int loaded_frames) {
    if (NumSectionsActivated < NumCurrentStreamingSections) {
        for (int i = 0; i < NumCurrentStreamingSections; i++) {
            TrackStreamingSection *section = CurrentStreamingSections[i];
            if (section->Status == TrackStreamingSection::LOADED && NeedsGameStateActivation(section) &&
                loaded_frames <= RealTimeFrames - section->LoadedTime) {
                return section->SectionNumber;
            }
        }
    }

    return 0;
}

int TrackStreamer::GetLoadingPriority(TrackStreamingSection *section, StreamingPositionEntry *streaming_position, bool use_direction) {
    if (!section->pBoundary) {
        return 0;
    }

    float speed = bLength(&streaming_position->Velocity);
    if (use_direction) {
        speed = 100.0f;
    }

    if (speed < 1.0f) {
        return 0;
    }

    bVector2 lookahead(streaming_position->Position.x + streaming_position->Velocity.x,
                       streaming_position->Position.y + streaming_position->Velocity.y);
    float distance = section->pBoundary->GetDistanceOutside(&lookahead, 999.0f);

    bVector2 heading;
    if (use_direction) {
        bNormalize(&heading, &streaming_position->Direction);
    } else {
        bNormalize(&heading, &streaming_position->Velocity);
    }

    bVector2 to_section(section->Centre.x - lookahead.x, section->Centre.y - lookahead.y);
    bVector2 section_direction;
    bNormalize(&section_direction, &to_section);

    float speed_scale = speed * 0.016666668f;
    if (speed_scale > 1.0f) {
        speed_scale = 1.0f;
    }

    unsigned short angle = bASin(bDot(&section_direction, &heading));
    float degrees = static_cast<unsigned short>(0x4000 - angle) * 0.005493164f;
    if (degrees < 20.0f) {
        degrees = 20.0f;
    }
    if (degrees > 90.0f) {
        degrees = 90.0f;
    }

    int loading_priority =
        static_cast<int>(distance * (1.0f - (90.0f - degrees) * 0.014285714f * speed_scale * 0.66999996f) * 0.013333334f);
    if (loading_priority < 0) {
        loading_priority = 0;
    }
    if (loading_priority > 2) {
        loading_priority = 2;
    }

    return loading_priority;
}

void TrackStreamer::AssignLoadingPriority() {
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        int loading_priority = 99;
        for (unsigned int position_number = 0; position_number < 2; position_number++) {
            if (((section->CurrentlyVisible >> (position_number & 0x1f)) & 1U) != 0) {
                int position_priority = GetLoadingPriority(section, &StreamingPositionEntries[position_number], false);
                if (position_priority < loading_priority) {
                    loading_priority = position_priority;
                }
            }
        }

        section->LoadingPriority = loading_priority * 100000 + section->SectionPriority;
    }
}

void TrackStreamer::CalculateLoadingBacklog() {
    float loading_backlog = 0.0f;
    for (int i = 0; i < NumCurrentStreamingSections; i++) {
        TrackStreamingSection *section = CurrentStreamingSections[i];
        if (section->CurrentlyVisible && section->Status != TrackStreamingSection::LOADED &&
            section->Status != TrackStreamingSection::ACTIVATED) {
            int rounded_size = section->Size;
            if (rounded_size < 0) {
                rounded_size += 0x3ff;
            }

            float section_backlog = static_cast<float>(rounded_size >> 10) * 0.0004f + 0.2f;
            if (section->BaseLoadingPriority == 1) {
                section_backlog *= 0.4f;
            }
            if (section->BaseLoadingPriority == 2) {
                section_backlog *= 0.2f;
            }
            loading_backlog += section_backlog;
        }
    }

    LoadingBacklog = loading_backlog;
}

bool TrackStreamer::AreAllSectionsActivated() {
    bool all_sections_activated = false;
    if (LoadingPhase == LOADING_IDLE) {
        all_sections_activated = NumCurrentStreamingSections <= NumSectionsActivated + NumSectionsOutOfMemory;
    }
    return all_sections_activated;
}

bool TrackStreamer::IsLoadingInProgress() {
    bool loading_in_progress = !AreAllSectionsActivated();

    if (!loading_in_progress && !AreAllSectionsActivated()) {
        while (!AreAllSectionsActivated()) {
            ServiceResourceLoading();
            ServiceNonGameState();
        }
    }

    return loading_in_progress;
}

void TrackStreamer::CheckLoadingBar() {
    float minimum_distance = kMaxDistance_TrackStreamer;

    for (int i = 0; i < 2; i++) {
        StreamingPositionEntry *entry = &StreamingPositionEntries[i];
        if (!IsLoadingInProgress()) {
            break;
        }

        bool far_load = false;
        if (CurrentZoneFarLoad && IsLoadingInProgress()) {
            far_load = true;
        }

        if (far_load || !entry->PositionSet || !entry->FollowingCar) {
            break;
        }

        float velocity_squared = entry->Velocity.x * entry->Velocity.x + entry->Velocity.y * entry->Velocity.y;
        float velocity_magnitude = 0.0f;
        if (velocity_squared > kVelocityEpsilon_TrackStreamer) {
            velocity_magnitude = bSqrt(velocity_squared);
        }

        if (velocity_magnitude > kLoadingBarSpeedThreshold_TrackStreamer) {
            break;
        }

        for (int n = 0; n < NumCurrentStreamingSections; n++) {
            TrackStreamingSection *section = CurrentStreamingSections[n];
            if (!section->pBoundary || section->Status == TrackStreamingSection::ACTIVATED ||
                !IsLoadingBarSection_TrackStreamer(section->SectionNumber)) {
                continue;
            }

            bVector2 future_position;
            future_position.x = entry->Position.x + entry->Velocity.x * kFuturePositionScale_TrackStreamer;
            future_position.y = entry->Position.y + entry->Velocity.y * kFuturePositionScale_TrackStreamer;

            float current_distance = section->pBoundary->GetDistanceOutside(&entry->Position, kMaxDistance_TrackStreamer);
            float future_distance = section->pBoundary->GetDistanceOutside(&future_position, kMaxDistance_TrackStreamer);
            float predicted_distance =
                current_distance - (current_distance - future_distance) * kPredictionScaleA_TrackStreamer * kPredictionScaleB_TrackStreamer;
            if (predicted_distance < minimum_distance) {
                minimum_distance = predicted_distance;
            }
        }
    }

    prev_need_loading_bar_26275 = minimum_distance < kLoadingBarDistanceThreshold_TrackStreamer;
}
