#include "WeatherMan.hpp"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

bTList<GenericRegion> RegionLists[NUM_REGION_TYPES];
int RegionCount[NUM_REGION_TYPES] = {};
int WeatherUseCoolFogValues = 0;
bVector3 cPos;
float BaseFogFalloff = 0.62f;
float BaseFogFalloffX = 0.31f;
float BaseFogFalloffY = 1.3f;
float BaseWeatherFog = 0.74f;
float BaseWeatherFogStart = 212.0f;
int BaseWeatherFogColourR = 0x4B;
int BaseWeatherFogColourG = 0x69;
int BaseWeatherFogColourB = 0x6E;

// TODO move
extern int FogControlOverRide;

int LoaderWeatherMan(bChunk *bchunk) {
    if (bchunk->GetID() != BCHUNK_GENERIC_REGIONS) {
        return 0;
    }

    WeatherRegionPack *region_pack = reinterpret_cast<WeatherRegionPack *>(bchunk->GetAlignedData(16));
    bPlatEndianSwap(&region_pack->Version);
    bPlatEndianSwap(&region_pack->NumRegions);
    if (region_pack->Version == 2) {
        GenericRegion *region = reinterpret_cast<GenericRegion *>(&region_pack[1]);
        int num_regions = region_pack->NumRegions;
        for (int i = 0; i < num_regions; i++) {
            bPlatEndianSwap(&region->NameHash);
            bPlatEndianSwap(&region->GroupNameHash);
            bPlatEndianSwap(&region->Type);
            bPlatEndianSwap(&region->FogColour);
            bPlatEndianSwap(&region->FogStart);
            bPlatEndianSwap(&region->Parameter1);
            bPlatEndianSwap(&region->PositionX);
            bPlatEndianSwap(&region->PositionY);
            bPlatEndianSwap(&region->PositionZ);
            bPlatEndianSwap(&region->Radius);
            bPlatEndianSwap(&region->Intensity);
            bPlatEndianSwap(&region->FarFalloffStart);
            bPlatEndianSwap(&region->FogFalloff);
            bPlatEndianSwap(&region->FogFalloffX);
            bPlatEndianSwap(&region->FogFalloffY);
            bPlatEndianSwap(&region->Shape);
            bPlatEndianSwap(&region->Blend);
            bPlatEndianSwap(&region->modifier);

            AddRegion(region);
            region++;
        }
    }

    return 1;
}

int UnloaderWeatherMan(bChunk *bchunk) {
    if (bchunk->GetID() != BCHUNK_GENERIC_REGIONS) {
        return 0;
    }

    WeatherRegionPack *region_pack = reinterpret_cast<WeatherRegionPack *>(bchunk->GetAlignedData(16));
    if (region_pack->Version == 2) {
        GenericRegion *region = reinterpret_cast<GenericRegion *>(&region_pack[1]);
        int num_regions = region_pack->NumRegions;
        for (int i = 0; i < num_regions; i++) {
            RemoveRegion(region);
            region++;
        }
    }

    return 1;
}

void AddRegion(GenericRegion *region) {
    RegionType type = region->GetType();
    if (type == REGION_RAIN && region->Intensity == 0.0f) {
        region->SetType(REGION_TUNNEL);
        type = REGION_TUNNEL;
    }

    // TODO why is that cast needed?
    if ((unsigned int)type < NUM_REGION_TYPES) {
        RegionLists[type].AddTail(region);
        RegionCount[type]++;
    }
}

void RemoveRegion(GenericRegion *region) {
    region->Remove();
}

int DepthRegion(GenericRegion *before, GenericRegion *after) {
    bVector3 Position(before->PositionX, before->PositionY, before->PositionZ);
    bVector3 Delta = Position - cPos;

    float distB = bLength(Delta);
    Position = bVector3(after->PositionX, after->PositionY, after->PositionZ);
    Delta = Position - cPos;

    float distA = bLength(Delta);
    return distB <= distA;
}

// UNSOLVED, just a scheduling issue I think
int RegionQuery::CalculateRegionInfo(eView *view, RegionType regionKind, int InFE) {
    static unsigned int oldDistFogColour = 0x000003E7;
    static float oldDistFogPower = 999.0f;
    static float oldDistFogStart = 999.0f;

    unsigned int colr_r = 0;
    unsigned int colr_g = 0;
    unsigned int colr_b = 0;
    bVector3 cPos(*view->GetCamera()->GetPosition());

    if (FogControlOverRide) {
        DistFogStart = BaseWeatherFogStart;
        FogFalloff = BaseFogFalloff;
        FogFalloffX = BaseFogFalloffX;
        FogFalloffY = BaseFogFalloffY;

        unsigned int fog_colour = ((BaseWeatherFogColourB << 16) | (BaseWeatherFogColourG << 8) | BaseWeatherFogColourR) & 0xFFFFFF;
        unsigned int retcol = fog_colour | 0x80000000;
        DistFogPower = BaseWeatherFog;
        DistFogColour = retcol;

        if (oldDistFogColour == retcol && oldDistFogPower == DistFogPower && oldDistFogStart == DistFogStart) {
            return 0;
        }

        oldDistFogColour = DistFogColour;
        oldDistFogPower = DistFogPower;
        oldDistFogStart = DistFogStart;
        return 1;
    }

    DistFogStart = 0.0f;
    DistFogPower = 0.0f;
    FogFalloff = 0.0f;
    FogFalloffX = 0.0f;
    FogFalloffY = 0.0f;
    float smallest = 9999999.0f;
    bTList<GenericRegion> *region_list = &RegionLists[regionKind];

    for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
        bVector4 direction;
        direction.x = region->PositionX - cPos.x;
        direction.y = region->PositionY - cPos.y;
        float distanceSq = direction.x * direction.x + direction.y * direction.y;

        if (distanceSq < region->Radius * region->Radius) {
            float distance = bSqrt(distanceSq);

            float ratio;
            if (distance >= region->FarFalloffStart) {
                ratio = (distance - region->FarFalloffStart) / (region->Radius - region->FarFalloffStart);
            } else {
                ratio = 0.0f;
            }

            region->effect = 1.0f - ratio;
            if (ratio == 0.0f) {
                region->inFlags = 1;
                if (region->Radius < smallest) {
                    smallest = region->Radius;
                }
            } else {
                region->inFlags = 2;
            }
        } else {
            region->inFlags = 4;
            region->effect = 0.0f;
        }
    }

    float totaleffex = 0.0f;
    for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
        region->effect = region->effect * region->modifier;
        totaleffex += region->effect;
    }

    for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
        if (totaleffex == 0.0f) {
            region->effect = 0.0f;
        } else {
            region->effect = region->effect / totaleffex;
        }

        if (region->effect != 0.0f) {
            FogFalloff += region->FogFalloff * region->effect;
            FogFalloffX += region->FogFalloffX * region->effect;
            FogFalloffY += region->FogFalloffY * region->effect;
            DistFogStart += region->FogStart * region->effect;
            DistFogPower += region->Intensity * region->effect;

            unsigned int tcolr_r = region->FogColour & 0xFF;
            unsigned int tcolr_g = (region->FogColour & 0xFF00) >> 8;
            unsigned int tcolr_b = (region->FogColour & 0xFF0000) >> 16;

            colr_r += static_cast<unsigned int>(tcolr_r * region->effect);
            colr_g += static_cast<unsigned int>(tcolr_g * region->effect);
            colr_b += static_cast<unsigned int>(tcolr_b * region->effect);
        }
    }

    unsigned int retcol = colr_r | (colr_g << 8) | (colr_b << 16) | 0x80000000;
    DistFogColour = retcol;
    if (oldDistFogColour == retcol && oldDistFogPower == DistFogPower && oldDistFogStart == DistFogStart) {
        return 0;
    }

    oldDistFogColour = DistFogColour;
    oldDistFogPower = DistFogPower;
    oldDistFogStart = DistFogStart;
    return 1;
}

GenericRegion *GetClosestRegionInView(eView *view, bVector3 *endVector, float *angleCos) {
    cPos = *view->GetCamera()->GetPosition();
    bVector3 cDir(*view->GetCamera()->GetDirection());
    bTList<GenericRegion> *region_list = &RegionLists[REGION_BLOOM];
    region_list->Sort(DepthRegion);
    bVector3 posScreen;

    CameraMover *cameraMover = view->GetCameraMover();
    if (!cameraMover) {
        return nullptr;
    }

    CameraAnchor *cameraAnchor = cameraMover->GetAnchor();
    if (!cameraAnchor) {
        return nullptr;
    }

    bVector3 MyCarPos(*cameraAnchor->GetGeometryPosition());
    float maxDist = 99999.0f;
    float angleCOS = 0.0f;
    GenericRegion *ClosestRegion = nullptr;

    for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
        bVector3 Position(region->PositionX, region->PositionY, region->PositionZ);
        bVector3 Delta = Position - cPos;
        bVector3 Delta2;
        bNormalize(&Delta2, &Delta);

        angleCOS = bDot(Delta2, cDir);
        if (0.0f < angleCOS) {
            float dist = bLength(Delta);
            if (dist < maxDist) {
                *angleCos = angleCOS;
                ClosestRegion = region;
                maxDist = dist;
            }
        }
    }

    if (ClosestRegion) {
        endVector->x = ClosestRegion->PositionX;
        endVector->y = ClosestRegion->PositionY;
        endVector->z = ClosestRegion->PositionZ;
        return ClosestRegion;
    }
    return 0;
}
