#include "WeatherMan.hpp"

#include "Speed/Indep/Src/Camera/Camera.hpp"
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
extern unsigned char BaseWeatherFogColourR;
extern unsigned char BaseWeatherFogColourG;
extern unsigned char BaseWeatherFogColourB;
extern float oldDistFogStart_27399;
extern float oldDistFogPower_27398;
extern unsigned int oldDistFogColour_27397;

int RegionQuery::CalculateRegionInfo(eView *view, RegionType regionKind, int InFE) {
    unsigned int red = 0;
    int green = 0;
    int blue = 0;
    char *camera = reinterpret_cast<char *>(view->pCamera);
    float camera_x = *reinterpret_cast<float *>(camera + 0x40);
    float camera_y = *reinterpret_cast<float *>(camera + 0x44);

    (void)InFE;

    if (!FogControlOverRide) {
        FogFalloff = DAT_80409c34;
        FogFalloffX = DAT_80409c34;
        FogFalloffY = DAT_80409c34;
        DistFogStart = DAT_80409c34;
        DistFogPower = DAT_80409c34;

        float smallest_radius = DAT_80409c38;
        bTList<GenericRegion> &region_list = RegionLists[static_cast<int>(regionKind)];

        for (GenericRegion *region = region_list.GetHead(); region != region_list.EndOfList(); region = region->GetNext()) {
            float delta_x = region->PositionX - camera_x;
            float delta_y = region->PositionY - camera_y;
            float distance_squared = delta_x * delta_x + delta_y * delta_y;

            if (region->Radius * region->Radius <= distance_squared) {
                region->inFlags = 4;
                region->effect = DAT_80409c34;
            } else {
                float falloff = DAT_80409c34;
                if (BaseFogFalloffX < distance_squared) {
                    float inverse_distance = 1.0f / bSqrt(distance_squared);
                    inverse_distance = -(distance_squared * inverse_distance * inverse_distance - DAT_80409c44) * inverse_distance * DAT_80409c40 +
                                       inverse_distance;
                    falloff = (-(distance_squared * inverse_distance * inverse_distance - DAT_80409c44) * inverse_distance * DAT_80409c40 +
                               inverse_distance) *
                              distance_squared;
                }

                float blend = DAT_80409c34;
                if (region->FarFalloffStart <= falloff) {
                    blend = (falloff - region->FarFalloffStart) / (region->Radius - region->FarFalloffStart);
                }

                region->effect = DAT_80409c44 - blend;
                if (blend == DAT_80409c34) {
                    region->inFlags = 1;
                    if (region->Radius < smallest_radius) {
                        smallest_radius = region->Radius;
                    }
                } else {
                    region->inFlags = 2;
                }
            }
        }

        float total_effect = DAT_80409c34;
        for (GenericRegion *region = region_list.GetHead(); region != region_list.EndOfList(); region = region->GetNext()) {
            region->effect = region->effect * region->modifier;
            total_effect += region->effect;
        }

        for (GenericRegion *region = region_list.GetHead(); region != region_list.EndOfList(); region = region->GetNext()) {
            if (total_effect == DAT_80409c34) {
                region->effect = DAT_80409c34;
            } else {
                region->effect = region->effect / total_effect;
            }

            if (region->effect != DAT_80409c34) {
                FogFalloff += region->FogFalloff * region->effect;
                FogFalloffX += region->FogFalloffX * region->effect;
                FogFalloffY += region->FogFalloffY * region->effect;
                DistFogStart += region->FogStart * region->effect;
                DistFogPower += region->Intensity * region->effect;

                red += static_cast<unsigned int>(static_cast<unsigned char>(region->FogColour & 0xff) * region->effect);
                green += static_cast<int>(static_cast<unsigned char>(region->FogColour >> 8 & 0xff) * region->effect);
                blue += static_cast<int>(static_cast<unsigned char>(region->FogColour >> 16 & 0xff) * region->effect);
            }
        }

        DistFogColour = red | green << 8 | blue << 16 | 0x80000000;
        if (oldDistFogColour_27397 == DistFogColour && oldDistFogPower_27398 == DistFogPower && oldDistFogStart_27399 == DistFogStart) {
            return 0;
        }
    } else {
        DistFogStart = BaseWeatherFogStart;
        FogFalloff = BaseFogFalloff;
        FogFalloffX = BaseFogFalloffX;
        FogFalloffY = BaseFogFalloffY;
        DistFogPower = BaseWeatherFog;
        DistFogColour = (BaseWeatherFogColourB << 16 | BaseWeatherFogColourG << 8 | BaseWeatherFogColourR) | 0x80000000;
        if (oldDistFogColour_27397 == DistFogColour && oldDistFogPower_27398 == DistFogPower && oldDistFogStart_27399 == DistFogStart) {
            return 0;
        }
    }

    oldDistFogStart_27399 = DistFogStart;
    oldDistFogPower_27398 = DistFogPower;
    oldDistFogColour_27397 = DistFogColour;
    return 1;
}

void AddRegion(GenericRegion *region) {
    unsigned int region_type = static_cast<unsigned int>(region->Type);
    if (region_type == 0 && region->Blend == 0) {
        region_type = REGION_TUNNEL;
        region->Type = REGION_TUNNEL;
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
    bVector3 before_position(before->PositionX - cPos.x, before->PositionY - cPos.y, before->PositionZ - cPos.z);
    bVector3 after_position(after->PositionX - cPos.x, after->PositionY - cPos.y, after->PositionZ - cPos.z);
    float before_distance = bSqrt(before_position.x * before_position.x + before_position.y * before_position.y +
                                  before_position.z * before_position.z);
    float after_distance = bSqrt(after_position.x * after_position.x + after_position.y * after_position.y +
                                 after_position.z * after_position.z);
    return before_distance <= after_distance;
}

GenericRegion *GetClosestRegionInView(eView *view, bVector3 *endVector, float *angleCos) {
    Camera *camera = view ? view->GetCamera() : 0;
    if (!camera || view->CameraMoverList.IsEmpty()) {
        return 0;
    }

    cPos = *camera->GetPosition();
    bVector3 camera_direction = *camera->GetDirection();
    RegionLists[REGION_BLOOM].Sort(DepthRegion);

    GenericRegion *closest_region = 0;
    float closest_distance = 99999.0f;
    for (GenericRegion *region = RegionLists[REGION_BLOOM].GetHead(); region != RegionLists[REGION_BLOOM].EndOfList();
         region = region->GetNext()) {
        bVector3 to_region(region->PositionX - cPos.x, region->PositionY - cPos.y, region->PositionZ - cPos.z);
        bVector3 direction;
        bNormalize(&direction, &to_region);

        float dot = direction.x * camera_direction.x + direction.y * camera_direction.y + direction.z * camera_direction.z;
        if (0.0f < dot) {
            float distance =
                bSqrt(to_region.x * to_region.x + to_region.y * to_region.y + to_region.z * to_region.z);
            if (distance < closest_distance) {
                *angleCos = dot;
                closest_region = region;
                closest_distance = distance;
            }
        }
    }

    if (closest_region) {
        endVector->x = closest_region->PositionX;
        endVector->y = closest_region->PositionY;
        endVector->z = closest_region->PositionZ;
    }
    return closest_region;
}
