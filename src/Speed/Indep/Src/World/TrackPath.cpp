#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/Gameplay/GManager.h"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Scenery.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

TrackPathZone *zoneB[2] = {};
TrackPathManager TheTrackPathManager;
bChunkLoader bChunkLoaderTrackPath(BCHUNK_TRACK_PATH_MANAGER, LoaderTrackPath, UnloaderTrackPath);
bChunkLoader bChunkLoaderTrackPathBarriers(BCHUNK_TTRACK_PATH_BARRIERS, LoaderTrackPath, UnloaderTrackPath);

// UNSOLVED
bool DoLinesIntersect(const bVector2 &line1_start, const bVector2 &line1_end, const bVector2 &line2_start, const bVector2 &line2_end) {
    float dy1 = line1_end.y - line1_start.y;
    float dx2 = line2_end.x - line2_start.x;
    float dx1 = line1_end.x - line1_start.x;
    float dy2 = line2_end.y - line2_start.y;
    float den = dx1 * dy2 - dy1 * dx2;

    if (den != 0.0f) {
        float dx3 = line1_start.x - line2_start.x;
        float dy3 = line1_start.y - line2_start.y;
        float r = (dy3 * dx2 - dx3 * dy2) / den;
        if (0.0f <= r && r <= 1.0f) {
            float s = (dy3 * dx1 - dx3 * dy1) / den;
            if (0.0f <= s && s <= 1.0f) {
                return true;
            }
        }
    }

    return false;
}

void TrackPathManager::Clear() {
    NumZones = 0;
    SizeofZones = 0;
    pZones = nullptr;
    bMemSet(ZoneInfoTable, 0, sizeof(ZoneInfoTable));
    MostCachedZones = 0;
    pBarriers = nullptr;
    NumBarriers = 0;
    zoneB[0] = nullptr;
    zoneB[1] = nullptr;
}

int TrackPathManager::Loader(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TRACK_PATH_MANAGER) {
        bChunk *last_chunk = chunk->GetLastChunk();

        for (chunk = chunk->GetFirstChunk(); chunk != last_chunk; chunk = chunk->GetNext()) {
            if (chunk->GetID() == BCHUNK_TTRACK_PATH_ZONES) {
                pZones = reinterpret_cast<TrackPathZone *>(chunk->GetData());
                SizeofZones = chunk->GetSize();
                NumZones = 0;

                for (TrackPathZone *zone = pZones; zone < GetLastZone(); zone = zone->GetMemoryImageNext()) {
                    bPlatEndianSwap(reinterpret_cast<int32 *>(&zone->Type));
                    bPlatEndianSwap(&zone->Position);
                    bPlatEndianSwap(&zone->Direction);
                    bPlatEndianSwap(&zone->Elevation);
                    bPlatEndianSwap(&zone->VisitInfo);
                    bPlatEndianSwap(&zone->NumPoints);
                    bPlatEndianSwap(&zone->MemoryImageSize);
                    bPlatEndianSwap(&zone->BBoxMin);
                    bPlatEndianSwap(&zone->BBoxMax);

                    int n;
                    for (n = 0; n < zone->NumPoints; n++) {
                        bPlatEndianSwap(&zone->Points[n]);
                    }
                    for (n = 0; n < 4; n++) {
                        bPlatEndianSwap(&zone->Data[n]);
                    }
                    NumZones++;
                }
            }
        }
        BuildZoneInfoTable();
        return true;
    }

    if (chunk->GetID() == BCHUNK_TTRACK_PATH_BARRIERS) {
        pBarriers = reinterpret_cast<TrackPathBarrier *>(chunk->GetData());
        NumBarriers = chunk->GetSize() / sizeof(*pBarriers);
        for (int i = 0; i < NumBarriers; i++) {
            pBarriers[i].EndianSwap();
        }
        return true;
    }

    return false;
}

int TrackPathManager::Unloader(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_TRACK_PATH_MANAGER) {
        Clear();
        NotifyGameZonesChanged();
        return true;
    }

    if (chunk->GetID() == BCHUNK_TTRACK_PATH_BARRIERS) {
        pBarriers = nullptr;
        NumBarriers = 0;
        return true;
    }

    return false;
}

void TrackPathManager::DisableAllBarriers() {
    for (int i = 0; i < NumBarriers; i++) {
        TrackPathBarrier *barrier = GetBarrier(i);
        barrier->Enabled = false;
    }
}

void TrackPathManager::EnableBarriers(const char *group_name) {
    unsigned int group_name_hash = bStringHash(group_name);
    for (int i = 0; i < NumBarriers; i++) {
        TrackPathBarrier *barrier = GetBarrier(i);
        if (barrier->HasGroup(group_name_hash)) {
            barrier->Enabled = true;

            SceneryGroup *scenery_group = FindSceneryGroup(group_name_hash);
            barrier->PlayerBarrier = scenery_group && scenery_group->DriveThroughBarrierFlag;
        }
    }
}

void TrackPathManager::BuildZoneInfoTable() {
    int type;
    for (type = 0; type < NUM_TRACK_PATH_ZONES; type++) {
        ZoneInfo *zone_info = &ZoneInfoTable[type];
        zone_info->NumZones = 0;

        TrackPathZone *zone;
        for (zone = pZones; zone < GetLastZone(); zone = zone->GetMemoryImageNext()) {
            if (zone->GetType() == type) {
                if (zone_info->NumZones == 0) {
                    zone_info->pFirstZone = zone;
                }
                zone_info->pLastZone = zone->GetMemoryImageNext();
                zone_info->NumZones++;
            }
        }
    }

    NotifyGameZonesChanged();
}

// UNSOLVED, regswap
TrackPathZone *TrackPathManager::FindZone(const bVector2 *position, eTrackPathZoneType zone_type, TrackPathZone *prev_zone) {
    ZoneInfo *zone_info = &ZoneInfoTable[zone_type];
    bool cache_valid;

    if (!position) {
        cache_valid = false;
    } else if (bBoundingBoxIsInside(&zone_info->CachedBBoxMin, &zone_info->CachedBBoxMax, position, 0.0f)) {
        cache_valid = zone_info->NumCachedZones < 9;
    } else {
        const float cached_radius = 64.0f;
        TrackPathZone *first_zone = zone_info->pFirstZone;
        TrackPathZone *last_zone = zone_info->pLastZone;

        zone_info->CachedBBoxMin.x = position->x - cached_radius;
        zone_info->CachedBBoxMin.y = position->y - cached_radius;
        zone_info->CachedBBoxMax.x = position->x + cached_radius;
        zone_info->CachedBBoxMax.y = position->y + cached_radius;
        zone_info->NumCachedZones = 0;
        zone_info->NumCacheRebuilds++;

        for (TrackPathZone *zone = zone_info->pFirstZone; zone < last_zone; zone = zone->GetMemoryImageNext()) {
            if (bBoundingBoxOverlapping(&zone_info->CachedBBoxMin, &zone_info->CachedBBoxMax, &zone->BBoxMin, &zone->BBoxMax)) {
                if (zone_info->NumCachedZones < 8) {
                    zone->CachedIndex = static_cast<char>(zone_info->NumCachedZones);
                    zone_info->CachedZones[zone_info->NumCachedZones] = zone;
                }
                zone_info->NumCachedZones++;
            }
        }

        cache_valid = zone_info->NumCachedZones < 9;
        MostCachedZones = bMax(MostCachedZones, zone_info->NumCachedZones);
    }

    TrackPathZone *found_zone = nullptr;
    if (!cache_valid) {
        TrackPathZone *first_zone;
        TrackPathZone *last_zone = zone_info->pLastZone;
        first_zone = zone_info->pFirstZone;

        zone_info->NumFullRebuilds++;
        if (prev_zone) {
            first_zone = prev_zone->GetMemoryImageNext();
        }

        for (TrackPathZone *zone = first_zone; zone < last_zone; zone = zone->GetMemoryImageNext()) {
            if (!position || (bBoundingBoxIsInside(&zone->BBoxMin, &zone->BBoxMax, position, 0.0f) && zone->IsPointInside(position))) {
                found_zone = zone;
                break;
            }
        }
    } else {
        int first_zone_index = 0;
        zone_info->NumCacheHits++;
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

void TrackPathInitRemoteCaffeineConnection() {}

int LoaderTrackPath(bChunk *chunk) {
    return TheTrackPathManager.Loader(chunk);
}

int UnloaderTrackPath(bChunk *chunk) {
    return TheTrackPathManager.Unloader(chunk);
}
float TrackPathZone::GetSegmentNextTo(bVector2 *point, bVector2 *s0, bVector2 *s1) {
    int Closest0 = -1;
    int Closest1 = -1;
    float d0 = 99999.0f;
    float len;

    for (int n = 0; n < NumPoints; n++) {
        bVector2 *p0 = &Points[n % NumPoints];
        bVector2 *p1 = &Points[(n + 1) % NumPoints];
        bVector2 r = *p0 - *point;
        bVector2 v(p1->y - p0->y, p0->x - p1->x);

        bNormalize(&v, &v);
        len = bDot(&v, &r);
        bVector2 InPoint = v * (len * -1.1f) + *point;
        bVector2 InPoint2 = v * (len * 1.1f) + *point;
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

    *s0 = Points[Closest0];
    *s1 = Points[Closest1];
    return d0;
}
