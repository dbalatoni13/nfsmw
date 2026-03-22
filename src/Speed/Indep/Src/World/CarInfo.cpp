#include "./CarInfo.hpp"
#include "./CarRender.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include <cstring>

struct CarPart {
    unsigned int GetTextureNameHash();
    unsigned int GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value);
    int HasAppliedAttribute(unsigned int namehash);
};

CarTypeInfo *CarTypeInfoArray;
extern int iRam8047ff04;

int UsedCarTextureAddToTable(unsigned int *table, int num_used, int max_textures, unsigned int texture_hash = 0);
int GetTempCarSkinTextures(unsigned int *table, int num_used, int max_textures, RideInfo *ride_info);

struct UsedCarTextureInfoLayout {
    unsigned int TexturesToLoadPerm[87];
    unsigned int TexturesToLoadTemp[87];
    int NumTexturesToLoadPerm;
    int NumTexturesToLoadTemp;
    unsigned int MappedSkinHash;
    unsigned int MappedSkinBHash;
    unsigned int MappedGlobalHash;
    unsigned int MappedWheelHash;
    unsigned int MappedSpinnerHash;
    unsigned int MappedBadging;
    unsigned int MappedSpoilerHash;
    unsigned int MappedRoofScoopHash;
    unsigned int MappedDashSkinHash;
    unsigned int MappedLightHash[11];
    unsigned int MappedTireHash;
    unsigned int MappedRimHash;
    unsigned int MappedRimBlurHash;
    unsigned int MappedLicensePlateHash;
    unsigned int ReplaceSkinHash;
    unsigned int ReplaceSkinBHash;
    unsigned int ReplaceGlobalHash;
    unsigned int ReplaceWheelHash;
    unsigned int ReplaceSpinnerHash;
    unsigned int ReplaceSpoilerHash;
    unsigned int ReplaceRoofScoopHash;
    unsigned int ReplaceDashSkinHash;
    unsigned int ReplaceHeadlightHash[3];
    unsigned int ReplaceHeadlightGlassHash[3];
    unsigned int ReplaceBrakelightHash[3];
    unsigned int ReplaceBrakelightGlassHash[3];
    unsigned int ReplaceReverselightHash[3];
    unsigned int ShadowHash;
};

unsigned int RideInfo::GetSkinNameHash() {
    if (this->IsUsingCompositeSkin() != 0) {
        return this->mCompositeSkinHash;
    }

    CarPart *skin_base = this->GetPart(0x4C);
    if (skin_base == 0) {
        return 0;
    }

    return skin_base->GetAppliedAttributeUParam(0x10C98090, 0);
}

void RideInfo::SetCompositeNameHash(int skin_number) {
    char temp[64];
    unsigned int dummy_skin_hash;
    unsigned int dummy_wheel_hash;
    unsigned int dummy_spinner_hash;

    if (static_cast<unsigned int>(skin_number - 1) < 4) {
        this->SkinType = 1;
    } else if (static_cast<unsigned int>(skin_number - 5) < 8) {
        this->SkinType = 2;
    } else {
        this->SkinType = 0;
    }

    bSPrintf(temp, "DUMMY_SKIN%d", skin_number);
    dummy_skin_hash = bStringHash(temp);
    this->SetCompositeSkinNameHash(dummy_skin_hash);

    bSPrintf(temp, "DUMMY_WHEEL%d", skin_number);
    dummy_wheel_hash = bStringHash(temp);
    this->SetCompositeWheelNameHash(dummy_wheel_hash);

    bSPrintf(temp, "DUMMY_SPINNER%d", skin_number);
    dummy_spinner_hash = bStringHash(temp);
    this->SetCompositeSpinnerNameHash(dummy_spinner_hash);
}

unsigned int RideInfo::GetCompositeSkinNameHash() {
    return this->mCompositeSkinHash;
}

void RideInfo::SetCompositeSkinNameHash(unsigned int namehash) {
    CarTypeInfo *type_info = &CarTypeInfoArray[this->Type];

    if (type_info->Skinnable != 0) {
        this->mCompositeSkinHash = namehash;
    } else {
        this->mCompositeSkinHash = 0;
    }
}

unsigned int RideInfo::GetCompositeWheelNameHash() {
    return this->mCompositeWheelHash;
}

void RideInfo::SetCompositeWheelNameHash(unsigned int namehash) {
    this->mCompositeWheelHash = namehash;
}

unsigned int RideInfo::GetCompositeSpinnerNameHash() {
    return this->mCompositeSpinnerHash;
}

void RideInfo::SetCompositeSpinnerNameHash(unsigned int namehash) {
    this->mCompositeSpinnerHash = namehash;
}

int RideInfo::IsUsingCompositeSkin() {
    return this->GetCompositeSkinNameHash() != 0;
}

void GetUsedCarTextureInfo(UsedCarTextureInfo *used_texture_info, RideInfo *ride_info, int front_end_only) {
    UsedCarTextureInfoLayout *info = reinterpret_cast<UsedCarTextureInfoLayout *>(used_texture_info);
    CarTypeInfo *car_type_info = &CarTypeInfoArray[ride_info->Type];
    char *car_base_name = car_type_info->BaseModelName;
    int max_perm_textures = 0x57;
    int max_temp_textures = 0x57;
    int num_perm_textures = 0;
    int num_temp_textures;
    int debug_print;
    char buffer[64];
    CarPart *wheel_part = ride_info->GetPart(0x42);
    unsigned int composite_skin_hash;
    unsigned int composite_wheel_hash;
    unsigned int composite_spinner_hash;
    unsigned int skin2_namehash;
    unsigned int skin3_namehash;
    unsigned int skin4_namehash;
    CarPart *carpart_headlight;
    CarPart *carpart_brakelight;
    unsigned int base_headlight_hash;
    unsigned int base_brakelight_hash;
    unsigned int misc_namehash;
    unsigned int misc_n_namehash;
    unsigned int interior_namehash;
    unsigned int interior_n_namehash;
    unsigned int badging_namehash;
    unsigned int badging_n_namehash;
    unsigned int driver_namehash;
    unsigned int engine_namehash;
    unsigned int badging_eu_namehash;
    unsigned int license_plate_namehash;
    unsigned int tire_n_namehash;
    unsigned int tread_namehash;
    unsigned int tread_n_namehash;
    unsigned int size_hash;
    unsigned int shape_hash;
    unsigned int shape_hashes[3];

    bMemSet(info, 0, sizeof(*info));

    bSPrintf(buffer, "%s_SKIN1", car_base_name);
    info->MappedSkinHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN1B", car_base_name);
    info->MappedSkinBHash = bStringHash(buffer);
    info->MappedGlobalHash = bStringHash("GLOBAL_SKIN1");

    if (wheel_part == 0) {
        info->MappedWheelHash = 0;
        info->MappedSpinnerHash = 0;
    } else {
        info->MappedWheelHash = bStringHash("_WHEEL", wheel_part->GetAppliedAttributeUParam(0x10C98090, 0));
        composite_spinner_hash = wheel_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);
        if (composite_spinner_hash == 0) {
            info->MappedSpinnerHash = 0;
        } else {
            info->MappedSpinnerHash = composite_spinner_hash;
        }
    }

    info->MappedSpoilerHash = bStringHash("SPOILER_SKIN1");
    info->MappedRoofScoopHash = bStringHash("ROOF_SKIN");

    if (ride_info->IsUsingCompositeSkin() == 0) {
        info->ReplaceSpinnerHash = 0;
        info->ReplaceRoofScoopHash = info->MappedSkinHash;
        info->ReplaceSkinHash = info->MappedSkinHash;
        info->ReplaceSpoilerHash = info->MappedSkinHash;
        info->ReplaceWheelHash = 0;
    } else {
        info->ReplaceSkinHash = ride_info->GetSkinNameHash();
        info->ReplaceWheelHash = ride_info->GetCompositeWheelNameHash();
        info->ReplaceSpinnerHash = ride_info->GetCompositeSpinnerNameHash();
        info->ReplaceSpoilerHash = ride_info->GetSkinNameHash();
        info->ReplaceRoofScoopHash = ride_info->GetSkinNameHash();
    }

    info->ReplaceSkinBHash = 0;
    info->ReplaceGlobalHash = info->ReplaceSkinHash;

    composite_skin_hash = ride_info->GetCompositeSkinNameHash();
    if (composite_skin_hash == 0) {
        if (info->ReplaceSkinHash != 0) {
            num_perm_textures = UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), 0, max_perm_textures);
        }
        if (info->ReplaceSkinBHash != 0) {
            num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures);
        }
    } else {
        num_perm_textures = UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), 0, max_perm_textures, composite_skin_hash);
    }

    composite_wheel_hash = ride_info->GetCompositeWheelNameHash();
    if (composite_wheel_hash != 0 || (composite_wheel_hash = info->ReplaceWheelHash, composite_wheel_hash != 0)) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, composite_wheel_hash);
    }

    composite_spinner_hash = ride_info->GetCompositeSpinnerNameHash();
    if (composite_spinner_hash != 0 || (composite_spinner_hash = info->ReplaceSpinnerHash, composite_spinner_hash != 0)) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, composite_spinner_hash);
    }

    num_temp_textures = GetTempCarSkinTextures(info->TexturesToLoadTemp, 0, max_temp_textures, ride_info);

    bSPrintf(buffer, "%s_SKIN2", car_base_name);
    skin2_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN3", car_base_name);
    skin3_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN4", car_base_name);
    skin4_namehash = bStringHash(buffer);

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, skin2_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, skin3_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, skin4_namehash);

    carpart_headlight = ride_info->GetPart(0x40);
    carpart_brakelight = ride_info->GetPart(0x41);

    bSPrintf(buffer, "%s_KIT00_HEADLIGHT", car_base_name);
    base_headlight_hash = bStringHash(buffer);
    bSPrintf(buffer, "%s_KIT00_BRAKELIGHT", car_base_name);
    base_brakelight_hash = bStringHash(buffer);

    if (carpart_headlight != 0) {
        composite_skin_hash = carpart_headlight->GetAppliedAttributeUParam(0x10C98090, 0);
        if (composite_skin_hash != 0) {
            base_headlight_hash = composite_skin_hash;
        }
    }

    if (carpart_brakelight != 0) {
        composite_skin_hash = carpart_brakelight->GetAppliedAttributeUParam(0x10C98090, 0);
        if (composite_skin_hash != 0) {
            base_brakelight_hash = composite_skin_hash;
        }
    }

    info->MappedLightHash[0] = bStringHash("HEADLIGHT_LEFT");
    info->MappedLightHash[1] = bStringHash("HEADLIGHT_RIGHT");
    info->MappedLightHash[5] = bStringHash("HEADLIGHT_GLASS_LEFT");
    info->MappedLightHash[6] = bStringHash("HEADLIGHT_GLASS_RIGHT");
    info->MappedLightHash[2] = bStringHash("BRAKELIGHT_LEFT");
    info->MappedLightHash[3] = bStringHash("BRAKELIGHT_RIGHT");
    info->MappedLightHash[4] = bStringHash("BRAKELIGHT_CENTRE");
    info->MappedLightHash[7] = bStringHash("BRAKELIGHT_GLASS_LEFT");
    info->MappedLightHash[8] = bStringHash("BRAKELIGHT_GLASS_RIGHT");
    info->MappedLightHash[9] = bStringHash("BRAKELIGHT_GLASS_CENTRE");
    info->MappedLightHash[10] = 0;

    info->ReplaceHeadlightHash[0] = bStringHash("_OFF", base_headlight_hash);
    info->ReplaceHeadlightHash[1] = bStringHash("_ON", base_headlight_hash);
    info->ReplaceHeadlightHash[2] = bStringHash("_DAMAGE0", base_headlight_hash);
    info->ReplaceHeadlightGlassHash[0] = bStringHash("_GLASS_OFF", base_headlight_hash);
    info->ReplaceHeadlightGlassHash[1] = bStringHash("_GLASS_ON", base_headlight_hash);
    info->ReplaceHeadlightGlassHash[2] = bStringHash("_GLASS_DAMAGE0", base_headlight_hash);
    info->ReplaceBrakelightHash[0] = bStringHash("_OFF", base_brakelight_hash);
    info->ReplaceBrakelightHash[1] = bStringHash("_ON", base_brakelight_hash);
    info->ReplaceBrakelightHash[2] = bStringHash("_DAMAGE0", base_brakelight_hash);
    info->ReplaceBrakelightGlassHash[0] = bStringHash("_GLASS_OFF", base_brakelight_hash);
    info->ReplaceBrakelightGlassHash[1] = bStringHash("_GLASS_ON", base_brakelight_hash);
    info->ReplaceBrakelightGlassHash[2] = bStringHash("_GLASS_DAMAGE0", base_brakelight_hash);
    info->ReplaceReverselightHash[0] = 0;
    info->ReplaceReverselightHash[1] = 0;
    info->ReplaceReverselightHash[2] = 0;

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightGlassHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceHeadlightGlassHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightGlassHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ReplaceBrakelightGlassHash[1]);

    bSPrintf(buffer, "%s_MISC", car_base_name);
    misc_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_MISC_N", car_base_name);
    misc_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_INTERIOR", car_base_name);
    interior_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_INTERIOR_N", car_base_name);
    interior_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING", car_base_name);
    badging_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING_N", car_base_name);
    badging_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_DRIVER", car_base_name);
    driver_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_ENGINE", car_base_name);
    engine_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING_EU", car_base_name);
    badging_eu_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_LICENSE_PLATE", car_base_name);
    license_plate_namehash = bStringHash(buffer);
    info->MappedLicensePlateHash = license_plate_namehash;
    info->MappedBadging = badging_namehash;

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, misc_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, misc_n_namehash);

    if (front_end_only != 0 || strstr(car_base_name, "COP") != 0) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, interior_namehash);
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, interior_n_namehash);
    }

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, driver_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, engine_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, badging_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, badging_n_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, badging_eu_namehash);

    bSPrintf(buffer, "%s_SIDELIGHT", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));
    bSPrintf(buffer, "%s_DOOR_HANDLE", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));
    bSPrintf(buffer, "%s_LOGO", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));

    if (iRam8047ff04 != 6) {
        num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash("AUDIO_SKIN"));
    }

    bSPrintf(buffer, "%s_SHADOW%s", car_base_name, front_end_only == 0 ? "IG" : "FE");
    info->ShadowHash = bStringHash(buffer);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->ShadowHash);

    bSPrintf(buffer, "%s_NEON", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, bStringHash(buffer));

    bSPrintf(buffer, "%s_TIRE", car_base_name);
    info->MappedTireHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TIRE_N", car_base_name);
    tire_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_RIM", car_base_name);
    info->MappedRimHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_RIM_BLUR", car_base_name);
    info->MappedRimBlurHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TREAD", car_base_name);
    tread_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TREAD_N", car_base_name);
    tread_n_namehash = bStringHash(buffer);

    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->MappedTireHash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->MappedRimHash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, info->MappedRimBlurHash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, tire_n_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, tread_namehash);
    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures, tread_n_namehash);

    size_hash = bStringHash("SIZE");
    shape_hash = bStringHash("SHAPE");
    shape_hashes[0] = bStringHash("SQUARE");
    shape_hashes[1] = bStringHash("RECT");
    shape_hashes[2] = bStringHash("WIDE");

    for (int i = 0x46; i < 0x4C; i++) {
        CarPart *decal_model_part = ride_info->GetPart(i);

        if (decal_model_part != 0 && decal_model_part->HasAppliedAttribute(size_hash) != 0 &&
            decal_model_part->HasAppliedAttribute(shape_hash) != 0) {
            unsigned int decal_shape = decal_model_part->GetAppliedAttributeUParam(shape_hash, 0);

            for (int j = 0; j < 8; j++) {
                CarPart *decal_texture_part = ride_info->GetPart(j + i * 8 - 0x1DD);

                if (decal_texture_part != 0) {
                    unsigned int texture_hash = decal_texture_part->GetAppliedAttributeUParam(bStringHash("NAME"), 0);

                    if (decal_shape == shape_hashes[0]) {
                        texture_hash = bStringHash("_SQUARE", texture_hash);
                    } else if (decal_shape == shape_hashes[1]) {
                        texture_hash = bStringHash("_RECT", texture_hash);
                    } else if (decal_shape == shape_hashes[2]) {
                        texture_hash = bStringHash("_WIDE", texture_hash);
                    }

                    num_perm_textures += UsedCarTextureAddToTable(reinterpret_cast<unsigned int *>(info), num_perm_textures, max_perm_textures,
                                                                  texture_hash);
                }
            }
        }
    }

    info->NumTexturesToLoadTemp = num_temp_textures;
    info->NumTexturesToLoadPerm = num_perm_textures;
}
