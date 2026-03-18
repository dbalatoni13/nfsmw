#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width);
BOOL bBoundingBoxOverlapping(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *bbox2_min, const bVector2 *bbox2_max);
bool bIsPointInPoly(const bVector2 *point, const bVector2 *points, int num_points);
void bEndianSwap16(void *value);
void bEndianSwap32(void *value);
void bPlatEndianSwap(bVector2 *value);
void NotifyGameZonesChanged();
void *FindSceneryGroup(unsigned int group_name_hash);

static inline TrackPathZone *NextTrackPathZone(TrackPathZone *zone) {
    return reinterpret_cast<TrackPathZone *>(reinterpret_cast<char *>(zone) + zone->MemoryImageSize);
}

static inline char *GetTrackPathBarrierData(TrackPathBarrier *barriers, int index) {
    return reinterpret_cast<char *>(barriers) + index * 0x18;
}

TrackPathZone *zoneB[2];
TrackPathManager TheTrackPathManager;
bChunkLoader bChunkLoaderTrackPath(0x80034147, LoaderTrackPath, UnloaderTrackPath);
bChunkLoader bChunkLoaderTrackPathBarriers(0x3414D, LoaderTrackPath, UnloaderTrackPath);

void TrackPathManager::Clear() {
    NumZones = 0;
    SizeofZones = 0;
    pZones = nullptr;
    bMemSet(ZoneInfoTable, 0, sizeof(ZoneInfoTable));
    MostCachedZones = 0;
    NumBarriers = 0;
    pBarriers = nullptr;
    zoneB[0] = 0;
    zoneB[1] = 0;
}

int TrackPathManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034147) {
        for (bChunk *child = chunk->GetFirstChunk(); child < chunk->GetLastChunk(); child = child->GetNext()) {
            if (child->GetID() == 0x3414A) {
                TrackPathZone *zone = reinterpret_cast<TrackPathZone *>(child->GetData());
                pZones = zone;
                SizeofZones = child->GetSize();
                NumZones = 0;

                for (; reinterpret_cast<char *>(zone) < reinterpret_cast<char *>(pZones) + SizeofZones; zone = NextTrackPathZone(zone)) {
                    bEndianSwap32(&zone->Type);
                    bPlatEndianSwap(&zone->Position);
                    bPlatEndianSwap(&zone->Direction);
                    bEndianSwap32(&zone->Elevation);
                    bEndianSwap16(&zone->VisitInfo);
                    bEndianSwap16(&zone->NumPoints);
                    bEndianSwap16(&zone->MemoryImageSize);
                    bPlatEndianSwap(&zone->BBoxMin);
                    bPlatEndianSwap(&zone->BBoxMax);
                    for (int i = 0; i < zone->NumPoints; i++) {
                        bPlatEndianSwap(&zone->Points[i]);
                    }
                    for (int i = 0; i < 4; i++) {
                        bEndianSwap32(&zone->Data[i]);
                    }
                    NumZones += 1;
                }
            }
        }
        BuildZoneInfoTable();
        return true;
    }

    if (chunk->GetID() == 0x3414D) {
        NumBarriers = chunk->GetSize() / 0x18;
        pBarriers = reinterpret_cast<TrackPathBarrier *>(chunk->GetData());
        for (int i = 0; i < NumBarriers; i++) {
            char *barrier = GetTrackPathBarrierData(pBarriers, i);
            bPlatEndianSwap(reinterpret_cast<bVector2 *>(barrier));
            bPlatEndianSwap(reinterpret_cast<bVector2 *>(barrier + 8));
            bEndianSwap32(barrier + 0x14);
        }
        return true;
    }

    return false;
}

int TrackPathManager::Unloader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034147) {
        Clear();
        NotifyGameZonesChanged();
        return true;
    }

    if (chunk->GetID() == 0x3414D) {
        NumBarriers = 0;
        pBarriers = nullptr;
        return true;
    }

    return false;
}

void TrackPathManager::DisableAllBarriers() {
    for (int i = 0; i < NumBarriers; i++) {
        GetTrackPathBarrierData(pBarriers, i)[0x10] = 0;
    }
}

void TrackPathManager::EnableBarriers(const char *group_name) {
    unsigned int group_name_hash = bStringHash(group_name);
    for (int i = 0; i < NumBarriers; i++) {
        char *barrier = GetTrackPathBarrierData(pBarriers, i);
        if (*reinterpret_cast<unsigned int *>(barrier + 0x14) == group_name_hash) {
            barrier[0x10] = 1;

            void *scenery_group = FindSceneryGroup(group_name_hash);
            barrier[0x12] = scenery_group && *reinterpret_cast<unsigned char *>(reinterpret_cast<char *>(scenery_group) + 0x11);
        }
    }
}

void TrackPathManager::BuildZoneInfoTable() {
    for (int zone_type = 0; zone_type < NUM_TRACK_PATH_ZONES; zone_type++) {
        ZoneInfo *zone_info = &ZoneInfoTable[zone_type];
        zone_info->NumZones = 0;

        for (TrackPathZone *zone = pZones; zone < reinterpret_cast<TrackPathZone *>(reinterpret_cast<char *>(pZones) + SizeofZones);
             zone = NextTrackPathZone(zone)) {
            if (zone->Type == static_cast<eTrackPathZoneType>(zone_type)) {
                if (zone_info->NumZones == 0) {
                    zone_info->pFirstZone = zone;
                }
                zone_info->pLastZone = NextTrackPathZone(zone);
                zone_info->NumZones += 1;
            }
        }
    }

    NotifyGameZonesChanged();
}

TrackPathZone *TrackPathManager::FindZone(const bVector2 *position, eTrackPathZoneType zone_type, TrackPathZone *prev_zone) {
    ZoneInfo *zone_info = &ZoneInfoTable[zone_type];
    bool use_cached_zones;

    if (!position) {
        use_cached_zones = false;
    } else {
        if (!bBoundingBoxIsInside(&zone_info->CachedBBoxMin, &zone_info->CachedBBoxMax, position, 0.0f)) {
            zone_info->CachedBBoxMin.x = position->x - 64.0f;
            zone_info->CachedBBoxMin.y = position->y - 64.0f;
            zone_info->CachedBBoxMax.x = position->x + 64.0f;
            zone_info->CachedBBoxMax.y = position->y + 64.0f;
            zone_info->NumCachedZones = 0;
            zone_info->NumFullRebuilds += 1;

            for (TrackPathZone *zone = zone_info->pFirstZone; zone < zone_info->pLastZone; zone = NextTrackPathZone(zone)) {
                if (bBoundingBoxOverlapping(&zone_info->CachedBBoxMin, &zone_info->CachedBBoxMax, &zone->BBoxMin, &zone->BBoxMax)) {
                    if (zone_info->NumCachedZones < 8) {
                        zone->CachedIndex = static_cast<char>(zone_info->NumCachedZones);
                        zone_info->CachedZones[zone_info->NumCachedZones] = zone;
                    }
                    zone_info->NumCachedZones += 1;
                }
            }
            MostCachedZones = bMax(MostCachedZones, zone_info->NumCachedZones);
        }
        use_cached_zones = zone_info->NumCachedZones < 9;
    }

    TrackPathZone *result = 0;
    if (use_cached_zones) {
        int zone_index = 0;
        zone_info->NumCacheHits += 1;
        if (prev_zone) {
            zone_index = prev_zone->CachedIndex + 1;
        }

        while (zone_index < zone_info->NumCachedZones) {
            TrackPathZone *zone = zone_info->CachedZones[zone_index];
            if (bBoundingBoxIsInside(&zone->BBoxMin, &zone->BBoxMax, position, 0.0f) && zone->IsPointInside(position)) {
                return zone;
            }
            zone_index += 1;
        }
    } else {
        TrackPathZone *last_zone = zone_info->pLastZone;
        TrackPathZone *current_zone = zone_info->pFirstZone;
        zone_info->NumCacheRebuilds += 1;
        if (prev_zone) {
            current_zone = NextTrackPathZone(prev_zone);
        }

        while (current_zone < last_zone && position &&
               (!bBoundingBoxIsInside(&current_zone->BBoxMin, &current_zone->BBoxMax, position, 0.0f) ||
                !current_zone->IsPointInside(position))) {
            current_zone = NextTrackPathZone(current_zone);
        }
        result = current_zone;
    }

    return result;
}

void TrackPathManager::ResetZoneVisitInfos() {
    for (TrackPathZone *zone = pZones; zone < reinterpret_cast<TrackPathZone *>(reinterpret_cast<char *>(pZones) + SizeofZones);
         zone = NextTrackPathZone(zone)) {
        zone->VisitInfo = 0;
    }
}

bool TrackPathZone::IsPointInside(const bVector2 *point) {
    return bIsPointInPoly(point, Points, NumPoints);
}

float TrackPathZone::GetSegmentNextTo(bVector2 *point, bVector2 *segment_point_a, bVector2 *segment_point_b) {
    float closest_distance = 1.0e30f;
    int closest_segment_a = -1;
    int closest_segment_b = -1;

    for (int i = 0; i < NumPoints; i++) {
        int next_point = (i + 1) % NumPoints;
        bVector2 edge;
        edge.x = Points[next_point].y - Points[i].y;
        edge.y = Points[i].x - Points[next_point].x;
        bNormalize(&edge, &edge);

        float distance = edge.x * (Points[i].x - point->x) + edge.y * (Points[i].y - point->y);
        bVector2 near_point;
        near_point.x = point->x + edge.x * (distance * 0.999f);
        near_point.y = point->y + edge.y * (distance * 0.999f);
        bVector2 far_point;
        far_point.x = point->x + edge.x * (distance * 1.001f);
        far_point.y = point->y + edge.y * (distance * 1.001f);

        if ((distance < 0.0f ? -distance : distance) < closest_distance && (IsPointInside(&near_point) || IsPointInside(&far_point))) {
            closest_segment_a = i;
            closest_segment_b = next_point;
            closest_distance = distance < 0.0f ? -distance : distance;
        }
    }

    if (closest_segment_a == -1 || closest_segment_b == -1) {
        return -1.0f;
    }

    *segment_point_a = Points[closest_segment_a];
    *segment_point_b = Points[closest_segment_b];
    return closest_distance;
}

void TrackPathInitRemoteCaffeineConnection() {}

int LoaderTrackPath(bChunk *chunk) {
    return TheTrackPathManager.Loader(chunk);
}

int UnloaderTrackPath(bChunk *chunk) {
    return TheTrackPathManager.Unloader(chunk);
}
