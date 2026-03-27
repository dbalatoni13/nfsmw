#include "WeatherMan.hpp"

#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

bTList<GenericRegion> RegionLists[NUM_REGION_TYPES];
int RegionCount[NUM_REGION_TYPES];
bVector3 cPos;
extern float BaseWeatherFogStart;
extern float BaseWeatherFog;
extern float BaseFogFalloffY;
extern float BaseFogFalloffX;
extern float BaseFogFalloff;
extern float DAT_80409c34;
extern float DAT_80409c38;
extern float DAT_80409c3c;
extern float DAT_80409c40;
extern float DAT_80409c44;
extern int FogControlOverRide;
extern int BaseWeatherFogColourR;
extern int BaseWeatherFogColourG;
extern int BaseWeatherFogColourB;
extern float oldDistFogStart_27399;
extern float oldDistFogPower_27398;
extern unsigned int oldDistFogColour_27397;

int LoaderWeatherMan(bChunk *bchunk) {
    if (bchunk->GetID() != 0x34250) {
        return 0;
    }

    unsigned char *data = reinterpret_cast<unsigned char *>(bchunk->GetAlignedData(0x10));
    bEndianSwap32(data + 8);
    bEndianSwap32(data + 0xC);
    if (*reinterpret_cast<int *>(data + 8) == 2) {
        int num_regions = *reinterpret_cast<int *>(data + 0xC);
        unsigned char *region_data = data + 0x10;
        for (int i = 0; i < num_regions; i++) {
            bEndianSwap32(region_data + 0x84);
            bEndianSwap32(region_data + 0x88);
            bEndianSwap32(region_data + 0x48);
            bEndianSwap32(region_data + 0x4C);
            bEndianSwap32(region_data + 0x50);
            bEndianSwap32(region_data + 0x54);
            bEndianSwap32(region_data + 0x58);
            bEndianSwap32(region_data + 0x5C);
            bEndianSwap32(region_data + 0x60);
            bEndianSwap32(region_data + 0x64);
            bEndianSwap32(region_data + 0x68);
            bEndianSwap32(region_data + 0x6C);
            bEndianSwap32(region_data + 0x70);
            bEndianSwap32(region_data + 0x74);
            bEndianSwap32(region_data + 0x78);
            bEndianSwap32(region_data + 0x8C);
            bEndianSwap32(region_data + 0x90);
            bEndianSwap32(region_data + 0x94);
            AddRegion(reinterpret_cast<GenericRegion *>(region_data));
            region_data += sizeof(GenericRegion);
        }
    }

    return 1;
}

int UnloaderWeatherMan(bChunk *bchunk) {
    if (bchunk->GetID() != 0x34250) {
        return 0;
    }

    unsigned char *data = reinterpret_cast<unsigned char *>(bchunk->GetAlignedData(0x10));
    int version = *reinterpret_cast<int *>(data + 8);
    if (version == 2) {
        GenericRegion *region = reinterpret_cast<GenericRegion *>(data + 0x10);
        int num_regions = *reinterpret_cast<int *>(data + 0xC);
        for (int i = 0; i < num_regions; i++) {
            RemoveRegion(region);
            region += 1;
        }
    }

    return 1;
}

void AddRegion(GenericRegion *region) {
    unsigned int region_type = static_cast<unsigned int>(region->Type);
    if (region_type == REGION_RAIN && region->Intensity == 0.0f) {
        region->Type = REGION_TUNNEL;
        region_type = REGION_TUNNEL;
    }

    if (region_type < NUM_REGION_TYPES) {
        RegionLists[region_type].AddTail(region);
        RegionCount[region_type] += 1;
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

int RegionQuery::CalculateRegionInfo(eView *view, RegionType regionKind, int InFE) {
    unsigned int colr_r = 0;
    unsigned int colr_g = 0;
    unsigned int colr_b = 0;
    bVector3 cPos(*view->GetCamera()->GetPosition());

    (void)InFE;

    if (FogControlOverRide) {
        unsigned int retcol;
        unsigned int fog_colour = BaseWeatherFogColourB << 16 | BaseWeatherFogColourG << 8 | BaseWeatherFogColourR;

        FogFalloff = BaseFogFalloff;
        FogFalloffX = BaseFogFalloffX;
        FogFalloffY = BaseFogFalloffY;
        DistFogStart = BaseWeatherFogStart;
        DistFogPower = BaseWeatherFog;
        retcol = fog_colour | 0x80000000;
        DistFogColour = retcol;
        if (oldDistFogColour_27397 == retcol) {
            if (oldDistFogPower_27398 == DistFogPower) {
                if (oldDistFogStart_27399 == DistFogStart) {
                    return 0;
                }
            }
        }
    } else {
        DistFogPower = DAT_80409c34;
        DistFogStart = DAT_80409c34;
        FogFalloffY = DAT_80409c34;
        FogFalloffX = DAT_80409c34;
        FogFalloff = DAT_80409c34;
        float smallest = DAT_80409c38;
        bTList<GenericRegion> *region_list = &RegionLists[static_cast<int>(regionKind)];

        for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
            bVector4 direction;
            direction.x = region->PositionX - cPos.x;
            direction.y = region->PositionY - cPos.y;
            float distanceSq = direction.x * direction.x + direction.y * direction.y;

            if (distanceSq < region->Radius * region->Radius) {
                float distance = DAT_80409c34;
                if (DAT_80409c3c < distanceSq) {
                    distance = bSqrt(distanceSq);
                }

                float blend;
                if (distance < region->FarFalloffStart) {
                    blend = DAT_80409c34;
                } else {
                    blend = (distance - region->FarFalloffStart) / (region->Radius - region->FarFalloffStart);
                }

                region->effect = DAT_80409c44 - blend;
                if (blend == DAT_80409c34) {
                    region->inFlags = 1;
                    if (region->Radius < smallest) {
                        smallest = region->Radius;
                    }
                } else {
                    region->inFlags = 2;
                }
            } else {
                region->inFlags = 4;
                region->effect = DAT_80409c34;
            }
        }

        float totaleffex = DAT_80409c34;
        for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
            region->effect = region->effect * region->modifier;
            totaleffex += region->effect;
        }

        for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
            if (totaleffex == DAT_80409c34) {
                region->effect = DAT_80409c34;
            } else {
                region->effect = region->effect / totaleffex;
            }

            if (region->effect != DAT_80409c34) {
                FogFalloff += region->FogFalloff * region->effect;
                FogFalloffX += region->FogFalloffX * region->effect;
                FogFalloffY += region->FogFalloffY * region->effect;
                DistFogStart += region->FogStart * region->effect;
                DistFogPower += region->Intensity * region->effect;

                unsigned int fog_colour = region->FogColour;
                unsigned int fog_colour_r = static_cast<unsigned char>(fog_colour);
                unsigned int fog_colour_g = static_cast<unsigned char>(fog_colour >> 8);
                unsigned int fog_colour_b = static_cast<unsigned char>(fog_colour >> 16);

                colr_r += static_cast<unsigned int>(fog_colour_r * region->effect);
                colr_g += static_cast<unsigned int>(fog_colour_g * region->effect);
                colr_b += static_cast<unsigned int>(fog_colour_b * region->effect);
            }
        }

        unsigned int retcol = colr_r | colr_g << 8 | colr_b << 16 | 0x80000000;
        DistFogColour = retcol;
        if (oldDistFogColour_27397 == retcol) {
            if (oldDistFogPower_27398 == DistFogPower) {
                if (oldDistFogStart_27399 == DistFogStart) {
                    return 0;
                }
            }
        }
    }

    oldDistFogColour_27397 = DistFogColour;
    oldDistFogPower_27398 = DistFogPower;
    oldDistFogStart_27399 = DistFogStart;
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
        return 0;
    }

    CameraAnchor *cameraAnchor = cameraMover->GetAnchor();
    if (!cameraAnchor) {
        return 0;
    }

    bVector3 MyCarPos(*cameraAnchor->GetGeometryPosition());
    float maxDist = 99999.0f;
    float angleCOS;
    GenericRegion *ClosestRegion = 0;

    for (GenericRegion *region = region_list->GetHead(); region != region_list->EndOfList(); region = region->GetNext()) {
        bVector3 Position(region->PositionX, region->PositionY, region->PositionZ);
        bVector3 Delta = Position - cPos;
        bVector3 Delta2(Delta);
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
