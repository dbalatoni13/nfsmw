#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Scenery.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

BOOL bBoundingBoxIsInside(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *point, float extra_width);
BOOL bBoundingBoxOverlapping(const bVector2 *bbox_min, const bVector2 *bbox_max, const bVector2 *bbox2_min, const bVector2 *bbox2_max);
bool bIsPointInPoly(const bVector2 *point, const bVector2 *points, int num_points);
void bEndianSwap16(void *value);
void bEndianSwap32(void *value);
void bPlatEndianSwap(bVector2 *value);
void NotifyGameZonesChanged();

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
    pBarriers = nullptr;
    NumBarriers = 0;
    zoneB[0] = 0;
    zoneB[1] = 0;
}

int TrackPathManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == 0x80034147) {
        bChunk *last_chunk = chunk->GetLastChunk();

        for (chunk = chunk->GetFirstChunk(); chunk != last_chunk; chunk = chunk->GetNext()) {
            if (chunk->GetID() == 0x3414A) {
                pZones = reinterpret_cast<TrackPathZone *>(chunk->GetData());
                TrackPathZone *zone = pZones;
                SizeofZones = chunk->GetSize();
                NumZones = 0;

                for (; zone < GetLastZone(); zone = zone->GetMemoryImageNext()) {
                    bEndianSwap32(&zone->Type);
                    bPlatEndianSwap(&zone->Position);
                    bPlatEndianSwap(&zone->Direction);
                    bEndianSwap32(&zone->Elevation);
                    bEndianSwap16(&zone->VisitInfo);
                    bEndianSwap16(&zone->NumPoints);
                    bEndianSwap16(&zone->MemoryImageSize);
                    bPlatEndianSwap(&zone->BBoxMin);
                    bPlatEndianSwap(&zone->BBoxMax);
                    for (int n = 0; n < zone->NumPoints; n++) {
                        bPlatEndianSwap(&zone->Points[n]);
                    }
                    for (int n = 0; n < 4; n++) {
                        bEndianSwap32(reinterpret_cast<void *>(n * sizeof(int) + reinterpret_cast<int>(zone) + 0x30));
                    }
                    NumZones += 1;
                }
            }
        }
        BuildZoneInfoTable();
        return true;
    }

    if (chunk->GetID() == 0x3414D) {
        pBarriers = reinterpret_cast<TrackPathBarrier *>(chunk->GetData());
        int i;
        unsigned int size = chunk->GetSize();
        NumBarriers = size / 0x18;
        for (i = 0; i < NumBarriers; i++) {
            pBarriers[i].EndianSwap();
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
        pBarriers = nullptr;
        NumBarriers = 0;
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
    for (int type = 0; type < NUM_TRACK_PATH_ZONES; type++) {
        ZoneInfo *zone_info = &ZoneInfoTable[type];
        zone_info->NumZones = 0;

        for (TrackPathZone *zone = pZones; zone < GetLastZone(); zone = zone->GetMemoryImageNext()) {
            if (zone->GetType() == static_cast<eTrackPathZoneType>(type)) {
                if (zone_info->NumZones == 0) {
                    zone_info->pFirstZone = zone;
                }
                zone_info->pLastZone = zone->GetMemoryImageNext();
                zone_info->NumZones += 1;
            }
        }
    }

    NotifyGameZonesChanged();
}

TrackPathZone *TrackPathManager::FindZone(const bVector2 *position, eTrackPathZoneType zone_type, TrackPathZone *prev_zone) {
    ZoneInfo *zone_info = &ZoneInfoTable[zone_type];
    bool cache_valid;

    if (!position) {
        cache_valid = false;
    } else if (bBoundingBoxIsInside(&zone_info->CachedBBoxMin, &zone_info->CachedBBoxMax, position, 0.0f)) {
        cache_valid = zone_info->NumCachedZones < 9;
    } else {
        const float cached_radius = 64.0f;
        TrackPathZone *first_zone;
        TrackPathZone *last_zone;

        last_zone = zone_info->pLastZone;
        first_zone = zone_info->pFirstZone;

        zone_info->CachedBBoxMin.x = position->x - cached_radius;
        zone_info->CachedBBoxMin.y = position->y - cached_radius;
        zone_info->CachedBBoxMax.x = position->x + cached_radius;
        zone_info->CachedBBoxMax.y = position->y + cached_radius;
        zone_info->NumCachedZones = 0;
        zone_info->NumFullRebuilds += 1;

        for (TrackPathZone *zone = first_zone; zone < last_zone; zone = zone->GetMemoryImageNext()) {
            if (bBoundingBoxOverlapping(&zone_info->CachedBBoxMin, &zone_info->CachedBBoxMax, &zone->BBoxMin, &zone->BBoxMax)) {
                if (zone_info->NumCachedZones < 8) {
                    zone->CachedIndex = static_cast<char>(zone_info->NumCachedZones);
                    zone_info->CachedZones[zone_info->NumCachedZones] = zone;
                }
                zone_info->NumCachedZones += 1;
            }
        }

        cache_valid = zone_info->NumCachedZones < 9;
        MostCachedZones = bMax(MostCachedZones, zone_info->NumCachedZones);
    }

    TrackPathZone *found_zone = 0;
    if (!cache_valid) {
        TrackPathZone *last_zone = zone_info->pLastZone;
        TrackPathZone *first_zone;

        first_zone = zone_info->pFirstZone;
        zone_info->NumCacheRebuilds += 1;
        if (prev_zone) {
            first_zone = prev_zone->GetMemoryImageNext();
        }

        TrackPathZone *zone = first_zone;
        while (zone < last_zone && position &&
               (!bBoundingBoxIsInside(&zone->BBoxMin, &zone->BBoxMax, position, 0.0f) || !zone->IsPointInside(position))) {
            zone = zone->GetMemoryImageNext();
        }
        found_zone = zone;
    } else {
        int first_zone_index = 0;
        zone_info->NumCacheHits += 1;
        if (prev_zone) {
            first_zone_index = prev_zone->CachedIndex + 1;
        }

        for (int index = first_zone_index; index < zone_info->NumCachedZones; index++) {
            TrackPathZone *zone = zone_info->CachedZones[index];
            if (bBoundingBoxIsInside(&zone->BBoxMin, &zone->BBoxMax, position, 0.0f) && zone->IsPointInside(position)) {
                found_zone = zone;
                break;
            }
        }
    }

    return found_zone;
}

void TrackPathManager::ResetZoneVisitInfos() {
    for (TrackPathZone *zone = pZones; zone < GetLastZone(); zone = zone->GetMemoryImageNext()) {
        zone->SetVisitInfo(0);
    }
}

bool TrackPathZone::IsPointInside(const bVector2 *point) {
    return bIsPointInPoly(point, Points, NumPoints);
}

float TrackPathZone::GetSegmentNextTo(bVector2 *point, bVector2 *segment_point_a, bVector2 *segment_point_b) {
    int Closest0 = -1;
    int Closest1 = -1;
    float d0 = 1.0e30f;
    float len;

    for (int n = 0; n < NumPoints; n++) {
        bVector2 *p0 = &Points[n % NumPoints];
        bVector2 *p1 = &Points[(n + 1) % NumPoints];
        bVector2 v = *p0 - *point;
        bVector2 r(p1->y - p0->y, p0->x - p1->x);

        bNormalize(&r, &r);
        len = bDot(&r, &v);
        bVector2 InPoint = r * (len * 0.999f) + *point;
        bVector2 InPoint2 = r * (len * 1.001f) + *point;
        len = bAbs(len);

        if (len < d0 && (IsPointInside(&InPoint) || IsPointInside(&InPoint2))) {
            Closest0 = n % NumPoints;
            Closest1 = (n + 1) % NumPoints;
            d0 = len;
        }
    }

    if (Closest0 == -1 || Closest1 == -1) {
        return -1.0f;
    }

    bCopy(segment_point_a, &Points[Closest0]);
    bCopy(segment_point_b, &Points[Closest1]);
    return d0;
}

void TrackPathInitRemoteCaffeineConnection() {}

int LoaderTrackPath(bChunk *chunk) {
    return TheTrackPathManager.Loader(chunk);
}

int UnloaderTrackPath(bChunk *chunk) {
    return TheTrackPathManager.Unloader(chunk);
}
