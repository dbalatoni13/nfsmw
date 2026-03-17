#include "WeatherMan.hpp"

#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern bTList<GenericRegion> RegionLists[NUM_REGION_TYPES];
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
