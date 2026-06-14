#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "CarPartID.h"
#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/CarPartID.h"
#include "Speed/Indep/Src/World/CarSkin.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/ecar.h"
#include "Speed/Indep/Src/Generated/Hash.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/SpeedChunks.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstring>

#ifdef CLANGD_DAMNIT
#include "CarRender.cpp"
#endif

int bForceNeonsToBeCopCars = 1;

static const int ForceLODALoadingOnCurrentGen = 0;

static const CarType CurrentTweakableCarInfo = CARTYPE_911TURBO;
static const int32 PrintCarPartLoading = 0;
static const int PrintRideInfoInit = 0;                                           // TODO use in RideInfo::Init
static const int PrintSetCarParts = 0;                                            // TODO use
static const CAR_SLOT_ID BreakOnSetPartCarSlotId = CARSLOTID_DAMAGE_FRONT_WINDOW; // TODO use

CarPartDatabase CarPartDB;
const char *CarPartStringTable = nullptr;
uint32 CarPartStringTableSize = 0;
uint32 *CarPartTypeNameHashTable = nullptr;
uint32 CarPartTypeNameHashTableSize = 0;
CarPart *CarPartPartsTable = nullptr;
CarPartModelTable *CarPartModelsTable = nullptr;
CarPartPack *MasterCarPartPack = nullptr;

CAR_PART_ID CarPartSlotMap[CARSLOTID_NUM] = {
    CARPARTID_BASE,
    CARPARTID_DAMAGE_FRONT_WINDOW,
    CARPARTID_DAMAGE_BODY,
    CARPARTID_DAMAGE_COP_LIGHTS,
    CARPARTID_DAMAGE_COP_SPOILER,
    CARPARTID_DAMAGE_FRONT_WHEEL,
    CARPARTID_DAMAGE_LEFT_BRAKELIGHT,
    CARPARTID_DAMAGE_RIGHT_BRAKELIGHT,
    CARPARTID_DAMAGE_LEFT_HEADLIGHT,
    CARPARTID_DAMAGE_RIGHT_HEADLIGHT,
    CARPARTID_DAMAGE_HOOD,
    CARPARTID_DAMAGE_BUSHGUARD,
    CARPARTID_DAMAGE_FRONT_BUMPER,
    CARPARTID_DAMAGE_RIGHT_DOOR,
    CARPARTID_DAMAGE_RIGHT_REAR_DOOR,
    CARPARTID_DAMAGE_TRUNK,
    CARPARTID_DAMAGE_REAR_BUMPER,
    CARPARTID_DAMAGE_REAR_LEFT_WINDOW,
    CARPARTID_DAMAGE_FRONT_LEFT_WINDOW,
    CARPARTID_DAMAGE_FRONT_RIGHT_WINDOW,
    CARPARTID_DAMAGE_REAR_RIGHT_WINDOW,
    CARPARTID_DAMAGE_LEFT_DOOR,
    CARPARTID_DAMAGE_LEFT_REAR_DOOR,
    CARPARTID_BODY,
    CARPARTID_FRONT_BRAKE,
    CARPARTID_FRONT_LEFT_WINDOW,
    CARPARTID_FRONT_RIGHT_WINDOW,
    CARPARTID_FRONT_WINDOW,
    CARPARTID_INTERIOR,
    CARPARTID_LEFT_BRAKELIGHT,
    CARPARTID_LEFT_BRAKELIGHT_GLASS,
    CARPARTID_LEFT_HEADLIGHT,
    CARPARTID_LEFT_HEADLIGHT_GLASS,
    CARPARTID_LEFT_SIDE_MIRROR,
    CARPARTID_REAR_BRAKE,
    CARPARTID_REAR_LEFT_WINDOW,
    CARPARTID_REAR_RIGHT_WINDOW,
    CARPARTID_REAR_WINDOW,
    CARPARTID_RIGHT_BRAKELIGHT,
    CARPARTID_RIGHT_BRAKELIGHT_GLASS,
    CARPARTID_RIGHT_HEADLIGHT,
    CARPARTID_RIGHT_HEADLIGHT_GLASS,
    CARPARTID_RIGHT_SIDE_MIRROR,
    CARPARTID_DRIVER,
    CARPARTID_SPOILER,
    CARPARTID_UNIVERSAL_SPOILER_BASE,
    CARPARTID_DAMAGE0_FRONT,
    CARPARTID_DAMAGE0_FRONTLEFT,
    CARPARTID_DAMAGE0_FRONTRIGHT,
    CARPARTID_DAMAGE0_REAR,
    CARPARTID_DAMAGE0_REARLEFT,
    CARPARTID_DAMAGE0_REARRIGHT,
    CARPARTID_ATTACHMENT0,
    CARPARTID_ATTACHMENT1,
    CARPARTID_ATTACHMENT2,
    CARPARTID_ATTACHMENT3,
    CARPARTID_ATTACHMENT4,
    CARPARTID_ATTACHMENT5,
    CARPARTID_ATTACHMENT6,
    CARPARTID_ATTACHMENT7,
    CARPARTID_ATTACHMENT8,
    CARPARTID_ATTACHMENT9,
    CARPARTID_ROOF,
    CARPARTID_HOOD,
    CARPARTID_HEADLIGHT,
    CARPARTID_BRAKELIGHT,
    CARPARTID_WHEEL,
    CARPARTID_WHEEL,
    CARPARTID_SPINNER,
    CARPARTID_LICENSE_PLATE,
    CARPARTID_DECAL_FRONT_WINDOW,
    CARPARTID_DECAL_REAR_WINDOW,
    CARPARTID_DECAL_LEFT_DOOR,
    CARPARTID_DECAL_RIGHT_DOOR,
    CARPARTID_DECAL_LEFT_QUARTER,
    CARPARTID_DECAL_RIGHT_QUARTER,
    CARPARTID_PAINT,
    CARPARTID_VINYL,
    CARPARTID_RIM_PAINT,
    CARPARTID_VINYL_PAINT,
    CARPARTID_VINYL_PAINT,
    CARPARTID_VINYL_PAINT,
    CARPARTID_VINYL_PAINT,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_DECAL_TEXTURE,
    CARPARTID_WINDOW_TINT,
    CARPARTID_CUSTOM_HUD,
    CARPARTID_CUSTOM_HUD_PAINT,
    CARPARTID_CUSTOM_HUD_PAINT,
    CARPARTID_CUSTOM_HUD_PAINT,
    CARPARTID_CV,
    CARPARTID_WHEEL_MANUFACTURER,
    CARPARTID_MISC,
};

CAR_PART_ID GetCarPartFromSlot(CAR_SLOT_ID slot) {
    return CarPartSlotMap[slot];
}

int ConvertVinylGroupNumberToVinylType(int vinyl_group_number) {
    switch (vinyl_group_number) {
        case 0:
        case 2:
        case 4:
        case 5:
        case 7:
        case 9:
        case 11:
        case 12:
        case 14:
            return 0;
        case 1:
        case 3:
        case 6:
        case 8:
        case 10:
        case 13:
            return 1;
        case 17:
            return 2;
        case 16:
        case 18:
            return 3;
        case 15:
            return 4;
        default:
            return 0;
    }
}

// STRIPPED
char GetCarPartLODChar(int car_part_lod) {}

CarTypeInfo *CarTypeInfoArray = nullptr;
int32 CarTypeInfoArrayUpdated = 0;

int8 *CarSlotAnimHookupTable = nullptr;
uint32 *CarSlotAnimHideOpenTable = nullptr;
uint32 *CarSlotAnimHideClosedTable = nullptr;
uint32 *DefaultSlotTypeNameTable = nullptr;

struct CarSlotTypeOverride *SlotTypeOverrideTable = nullptr;
int NumSlotTypeOverrides = 0;

int LoaderCarInfo(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SPEED_CARTYPEINFO_TABLE) {
        CarTypeInfoArray = reinterpret_cast<CarTypeInfo *>(chunk->GetAlignedData(16));

        for (int j = 0; j < NUM_CARTYPES; j++) {
            int k;
            int i;
            CarTypeInfo *pCarInfo = GetCarTypeInfo(static_cast<CarType>(j));

            bPlatEndianSwap(&pCarInfo->CarTypeNameHash);
            bPlatEndianSwap(&pCarInfo->HeadlightFOV);
            bPlatEndianSwap(&pCarInfo->HeadlightPosition);
            bPlatEndianSwap(&pCarInfo->DriverRenderingOffset);
            bPlatEndianSwap(&pCarInfo->InCarSteeringWheelRenderingOffset);
            bPlatEndianSwap(&pCarInfo->DefaultBasePaint);
            bPlatEndianSwap(reinterpret_cast<int *>(&pCarInfo->Type));
            bPlatEndianSwap(reinterpret_cast<int *>(&pCarInfo->UsageType));
            bPlatEndianSwap(&pCarInfo->CarMemTypeHash);

            for (k = 0; k < 5; k++) {
                bPlatEndianSwap(&pCarInfo->MaxInstances[k]);
                bPlatEndianSwap(&pCarInfo->WantToKeepLoaded[k]);
                bPlatEndianSwap(&pCarInfo->MinTimeBetweenUses[k]);
            }
            bPlatEndianSwap(&pCarInfo->DefaultSkinNumber);
            bPlatEndianSwap(&pCarInfo->Skinnable);
        }
        return 1;
    } else if (0) {
        // this is made up to match the dwarf
        CarType allowed_types[1];
        for (int i = 0; i < 10; i++) {
            bool allowed = false;
            for (int n = 0; n < 10; n++) {
            }
            if (allowed) {
                CarTypeInfo *info = GetCarTypeInfo(((volatile CarType *)allowed_types)[0]);
            }
        }
    } else if (chunk->GetID() == BCHUNK_SPEED_CARPART_ANIMHOOKUP_TABLE) {
        CarSlotAnimHookupTable = reinterpret_cast<signed char *>(chunk->GetData());
        return 1;
    } else if (chunk->GetID() == BCHUNK_SPEED_CARPART_ANIMHIDE_TABLE) {
        CarSlotAnimHideOpenTable = reinterpret_cast<unsigned int *>(chunk->GetData());
        CarSlotAnimHideClosedTable = reinterpret_cast<unsigned int *>(chunk) + 34;
        return 1;
    } else if (chunk->GetID() == BCHUNK_SPEED_CARPART_TYPENAME_TABLE) {
        DefaultSlotTypeNameTable = reinterpret_cast<unsigned int *>(chunk->GetData());
        SlotTypeOverrideTable = reinterpret_cast<CarSlotTypeOverride *>(reinterpret_cast<unsigned int *>(chunk) + 280);
        NumSlotTypeOverrides =
            static_cast<unsigned int>(chunk->GetSize() + sizeof(bChunk) - 280 * sizeof(unsigned int)) / sizeof(CarSlotTypeOverride);

        for (int i = 0; i < 0x116; i++) {
            bPlatEndianSwap(&DefaultSlotTypeNameTable[i]);
        }

        for (int i = 0; i < NumSlotTypeOverrides; i++) {
            bPlatEndianSwap(&SlotTypeOverrideTable[i].CarType);
            bPlatEndianSwap(&SlotTypeOverrideTable[i].SlotId);

            for (int j = 0; j < 2; j++) {
                bPlatEndianSwap(&SlotTypeOverrideTable[i].LookupType[j]);
            }
        }
        return 1;
    } else if (chunk->GetID() == BCHUNK_SPEED_CARPART_PACK_HEADER) {
        bChunk *car_pack_chunk = chunk->GetFirstChunk();
        bChunk *car_string_table_chunk = car_pack_chunk->GetNext();
        bChunk *car_attributetable_table_chunk = car_string_table_chunk->GetNext();
        bChunk *car_attributes_table_chunk = car_attributetable_table_chunk->GetNext();
        bChunk *car_model_table_chunk = car_attributes_table_chunk->GetNext();
        bChunk *car_typename_table_chunk = car_model_table_chunk->GetNext();
        bChunk *car_parts_table_chunk = car_typename_table_chunk->GetNext();
        CarPartPack *car_part_pack = reinterpret_cast<CarPartPack *>(car_pack_chunk->GetData());

        car_part_pack->EndianSwap();
        car_part_pack->InPlaceInit();
        car_part_pack->AttributesTable = reinterpret_cast<CarPartAttribute *>(car_attributes_table_chunk->GetData());
        car_part_pack->AttributeTableTable = reinterpret_cast<CarPartAttributeTable *>(car_attributetable_table_chunk->GetData());
        car_part_pack->PartsTable = reinterpret_cast<CarPart *>(car_parts_table_chunk->GetData());
        car_part_pack->TypeNameTable = reinterpret_cast<unsigned int *>(car_typename_table_chunk->GetData());
        car_part_pack->ModelTable = reinterpret_cast<CarPartModelTable *>(car_model_table_chunk->GetData());
        car_part_pack->StringTable = reinterpret_cast<char *>(car_string_table_chunk->GetData());
        car_part_pack->StringTableSize = car_string_table_chunk->GetSize();

        MasterCarPartPack = car_part_pack;
        CarPartStringTable = reinterpret_cast<const char *>(car_string_table_chunk->GetData());
        CarPartStringTableSize = car_string_table_chunk->GetSize();
        CarPartTypeNameHashTable = reinterpret_cast<unsigned int *>(car_typename_table_chunk->GetData());
        CarPartTypeNameHashTableSize = car_part_pack->NumTypeNames;
        CarPartPartsTable = reinterpret_cast<CarPart *>(car_parts_table_chunk->GetData());
        CarPartModelsTable = reinterpret_cast<CarPartModelTable *>(car_model_table_chunk->GetData());

        uint8 *track = reinterpret_cast<unsigned char *>(car_part_pack->AttributeTableTable);
        uint8 *end_track = track + car_attributetable_table_chunk->GetSize();

        while (track < end_track) {
            reinterpret_cast<CarPartAttributeTable *>(track)->EndianSwap();
            track += reinterpret_cast<CarPartAttributeTable *>(track)->GetByteSize();
        }

        for (unsigned int attribute_index = 0; attribute_index < car_part_pack->NumAttributes; attribute_index++) {
            car_part_pack->AttributesTable[attribute_index].EndianSwap();
        }

        for (unsigned int typename_hash_index = 0; typename_hash_index < car_part_pack->NumTypeNames; typename_hash_index++) {
            bPlatEndianSwap(&CarPartTypeNameHashTable[typename_hash_index]);
        }

        for (unsigned int model_table_index = 0; model_table_index < car_part_pack->NumModelTables; model_table_index++) {
            CarPartModelTable *model_table = &car_part_pack->ModelTable[model_table_index];

            model_table->EndianSwap();

            if (model_table->TemplatedNameHashes != 0) {
                for (int model_number = 0; model_number < 1; model_number++) {
                    for (int model_lod = 0; model_lod < 5; model_lod++) {
                        if ((model_table->ModelNames[model_number][model_lod]) != (char *)-1) {
                            model_table->ModelNames[model_number][model_lod] =
                                const_cast<char *>(&CarPartStringTable[reinterpret_cast<int>(model_table->ModelNames[model_number][model_lod]) * 4]);
                        }
                    }
                }
            }
        }

        for (unsigned int i = 0; i < car_part_pack->NumParts; i++) {
            CarPart *car_part = &car_part_pack->PartsTable[i];
            car_part->EndianSwap();

            int part_id = car_part->GetPartID();
            unsigned int brand_name = car_part->GetBrandNameHash();
            int upgrade_level = car_part->GetUpgradeLevel() - 1;
            int group_number = car_part->GetGroupNumber();
            CarPartIndex *index0 = nullptr;
            CarPartIndex *index1 = nullptr;

            upgrade_level = bClamp(upgrade_level, 0, 2);

            if (part_id == 'L') {
                switch (brand_name) {
                    case STRINGHASH_GLOSS:
                        index0 = &CarPartDB.PaintPart_Gloss[upgrade_level];
                        break;
                    case STRINGHASH_METAL:
                        index0 = &CarPartDB.PaintPart_Metallic[upgrade_level];
                        break;
                    case STRINGHASH_PEARL:
                        index0 = &CarPartDB.PaintPart_Pearl[upgrade_level];
                        break;
                    case STRINGHASH_VINYL:
                        index0 = &CarPartDB.PaintPart_Vinyl[upgrade_level];
                        break;
                    case STRINGHASH_RIM:
                        index0 = &CarPartDB.PaintPart_Rims[upgrade_level];
                        break;
                    case STRINGHASH_CALIPER:
                        index0 = &CarPartDB.PaintPart_Caliper[upgrade_level];
                        break;
                }
            } else if (part_id == 'O') {
                int vinyl_type = ConvertVinylGroupNumberToVinylType(group_number);

                switch (vinyl_type) {
                    case 0:
                        index0 = &CarPartDB.VinylPart_Side[upgrade_level];
                        break;
                    case 1:
                        index0 = &CarPartDB.VinylPart_Hood[upgrade_level];
                        break;
                    case 2:
                        index0 = &CarPartDB.VinylPart_Body[upgrade_level];
                        break;
                    case 3:
                        index0 = &CarPartDB.VinylPart_Manufacturer[upgrade_level];
                        break;
                }

                index1 = &CarPartDB.VinylPart_All[upgrade_level];
            }

            if (index0 != nullptr) {
                if (index0->Part == nullptr) {
                    index0->Part = car_part;
                }

                index0->NumParts++;
            }

            if (index1 != nullptr) {
                if (index1->Part == nullptr) {
                    index1->Part = car_part;
                }

                index1->NumParts++;
            }
        }

        CarPartDB.CarPartPackList.AddTail(car_part_pack);
        CarPartDB.NumPacks++;
        CarPartDB.NumParts += car_part_pack->NumParts;
        CarPartDB.NumBytes += chunk->GetSize();
        // STRIPPED
        if (0) {
            CarPart *car_part;
            int num_parts;
            for (int i = 0; i < 1; i++) {
            }
        }
        return 1;
    }
    return 0;
}

int UnloaderCarInfo(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_SPEED_CARTYPEINFO_TABLE) {
        CarTypeInfoArray = nullptr;
        return 1;
    }

    if (chunk->GetID() == BCHUNK_SPEED_CARPART_PACK_HEADER) {
        bChunk *car_pack_chunk = chunk->GetFirstChunk();
        CarPartPack *car_part_pack = reinterpret_cast<CarPartPack *>(car_pack_chunk->GetData());

        car_part_pack->Remove();
        CarPartDB.NumPacks--;
        CarPartDB.NumParts -= car_part_pack->NumParts;
        CarPartDB.NumBytes -= chunk->GetSize();
        return 1;
    }

    return 0;
}

CarPartAttribute *CarPart::GetAttribute(uint32 namehash, CarPartAttribute *prev_attribute) {
    if (this->AttributeTableOffset == (uint16)-1) {
        return nullptr;
    }

    CarPartAttributeTable *attribute_table =
        reinterpret_cast<CarPartAttributeTable *>(reinterpret_cast<int16 *>(MasterCarPartPack->AttributeTableTable) + this->AttributeTableOffset);
    int num_attributes = attribute_table->NumAttributes;
    int attribute_index = 0;

    if (prev_attribute != nullptr) {
        while (attribute_index < num_attributes) {
            attribute_index++;

            if (prev_attribute == &MasterCarPartPack->AttributesTable[attribute_table->AttributeOffsetTable[attribute_index - 1]]) {
                break;
            }
        }

        if (attribute_index == num_attributes) {
            return nullptr;
        }
    }

    while (attribute_index < num_attributes) {
        CarPartAttribute *attribute = &MasterCarPartPack->AttributesTable[attribute_table->AttributeOffsetTable[attribute_index]];

        if (namehash == 0 || attribute->GetNameHash() == namehash) {
            return attribute;
        }

        attribute_index++;
    }

    return nullptr;
}

CarPartAttribute *CarPart::GetFirstAppliedAttribute(unsigned int namehash) {
    return GetNextAppliedAttribute(namehash, nullptr);
}

CarPartAttribute *CarPart::GetNextAppliedAttribute(unsigned int namehash, CarPartAttribute *prev_attribute) {
    return GetAttribute(namehash, prev_attribute);
}

int CarPart::HasAppliedAttribute(unsigned int namehash) {
    return static_cast<int>(GetFirstAppliedAttribute(namehash) != nullptr);
}

const char *CarPart::GetAppliedAttributeString(unsigned int namehash, const char *default_string) {
    CarPartAttribute *attribute = GetFirstAppliedAttribute(namehash);
    return attribute != nullptr ? CarPartStringTable + attribute->GetUParam() * 4 : default_string;
}

int CarPart::GetAppliedAttributeIParam(unsigned int namehash, int default_value) {
    CarPartAttribute *attribute = this->GetFirstAppliedAttribute(namehash);

    if (attribute != nullptr) {
        return attribute->GetIParam();
    }

    return default_value;
}

unsigned int CarPart::GetAppliedAttributeUParam(unsigned int namehash, unsigned int default_value) {
    CarPartAttribute *attribute = this->GetFirstAppliedAttribute(namehash);

    if (attribute != nullptr) {
        return attribute->GetUParam();
    }

    return default_value;
}

unsigned int CarPartModelTable::GetModelNameHash(unsigned int base_namehash, int model_num, int lod) {
    if (reinterpret_cast<int>(this->ModelNames[model_num][lod]) == -1) {
        return 0;
    }

    if (!this->TemplatedNameHashes) {
        return reinterpret_cast<unsigned int>(this->ModelNames[model_num][lod]);
    }

    unsigned int namehash = base_namehash;
    char lod_name[3] = {'_', static_cast<char>(lod + 'A'), '\0'};

    if (this->MiddleStringOffset != (uint16)-1) {
        namehash = bStringHash(&MasterCarPartPack->StringTable[this->MiddleStringOffset * 4], namehash);
    }

    namehash = bStringHash(this->ModelNames[model_num][lod], namehash);
    return bStringHash(lod_name, namehash);
}

const char *CarPart::GetName() {
    return &CarPartStringTable[this->NameOffset * 4];
}

unsigned int CarPart::GetCarTypeNameHash() {
    return CarPartTypeNameHashTable[this->CarTypeNameHashIndex];
}

unsigned char MapCarTypeNameHashToIndex(unsigned int namehash) {
    for (unsigned int i = 0; i < CarPartTypeNameHashTableSize; i++) {
        if (CarPartTypeNameHashTable[i] == namehash) {
            return static_cast<unsigned char>(i);
        }
    }

    return 0xFF;
}

uint32 CarPart::GetModelNameHash(int model_num, int lod) {
    if (this->ModelNameHashTableOffset == 0xFFFF) {
        return 0;
    }

    CarPartModelTable *model_table = &MasterCarPartPack->ModelTable[this->ModelNameHashTableOffset];
    unsigned int base_namehash;

    if (this->BaseModelNameHashSelector == 0) {
        base_namehash = 0xFFFFFFFF;
    } else if (this->BaseModelNameHashSelector == 1) {
        base_namehash = this->GetCarTypeNameHash();
    } else {
        base_namehash = this->GetBrandNameHash();
    }

    return model_table->GetModelNameHash(base_namehash, model_num, lod);
}

// STRIPPED
void CarPart::Print() {}

// STRIPPED
const char *CarPartDatabase::GetCarPartIDName(int32 car_part_id) {}

// STRIPPED
int CarPartDatabase::GetPartIndex(uint32 cartype_namehash, int32 car_part_id, uint32 car_part_namehash) {}

int CarPartDatabase::GetPartIndex(CarPart *car_part) {
    int index = 0;

    for (CarPartPack *car_part_pack = this->CarPartPackList.GetHead(); car_part_pack != this->CarPartPackList.EndOfList();
         car_part_pack = car_part_pack->GetNext()) {
        int local_index = car_part - car_part_pack->PartsTable;

        if (local_index >= 0 && local_index < static_cast<int>(car_part_pack->NumParts)) {
            return index + local_index;
        }

        index += car_part_pack->NumParts;
    }

    return -1;
}

CarPart *CarPartDatabase::GetCarPartByIndex(int index) {
    if (index > -1) {
        for (CarPartPack *car_part_pack = this->CarPartPackList.GetHead(); car_part_pack != this->CarPartPackList.EndOfList();
             car_part_pack = car_part_pack->GetNext()) {
            if (index < static_cast<int>(car_part_pack->NumParts)) {
                return car_part_pack->PartsTable + index;
            }

            index -= car_part_pack->NumParts;
        }
    }

    return nullptr;
}

int CarPartDatabase::NewGetNumCarParts(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upgrade_level) {
    int num_parts = 0;
    CarPart *part = nullptr;

    while (true) {
        part = this->NewGetCarPart(car_type, car_slot_id, car_part_namehash, part, upgrade_level);
        if (part == nullptr) {
            break;
        }

        num_parts++;
    }

    return num_parts;
}

uint32 *GetTypesFromSlot(CAR_SLOT_ID slot, CarType car_type);

// UNSOLVED
CarPart *CarPartDatabase::NewGetCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, CarPart *prev_part, int upg_level) {
    unsigned int *slot_types = GetTypesFromSlot(static_cast<CAR_SLOT_ID>(car_slot_id), car_type);
    CAR_PART_ID car_part_id = GetCarPartFromSlot(static_cast<CAR_SLOT_ID>(car_slot_id));
    int cur_type = 0;

    if (prev_part != nullptr) {
        while (cur_type < 2 && prev_part->GetCarTypeNameHash() != slot_types[cur_type]) {
            cur_type++;
        }

        if (cur_type == 2) {
            return nullptr;
        }
    } else {
        cur_type = 0;
    }

    for (int i = cur_type; i <= 1; i++) {
        if (slot_types[i] != 0) {
            for (CarPartPack *car_part_pack = this->CarPartPackList.GetHead(); car_part_pack != this->CarPartPackList.EndOfList();
                 car_part_pack = car_part_pack->GetNext()) {
                int num_parts = car_part_pack->NumParts;
                CarPart *first_part = car_part_pack->PartsTable;
                CarPart *last_part = first_part + num_parts;
                CarPart *car_part = first_part;

                if (prev_part != nullptr && i == cur_type) {
                    if (prev_part < first_part || prev_part >= last_part) {
                        continue;
                    }

                    car_part = prev_part + 1;
                } else {
                    unsigned char typename_index = MapCarTypeNameHashToIndex(slot_types[i]);

                    car_part = static_cast<CarPart *>(ScanHashTableKey8(typename_index, first_part, num_parts, 7, sizeof(CarPart)));

                    if (car_part == nullptr) {
                        continue;
                    }
                }

                while (car_part < last_part) {
                    if (car_part->PartID == car_part_id && car_part->GetCarTypeNameHash() == slot_types[i] &&
                        (car_part_namehash == 0 || car_part->GetPartNameHash() == car_part_namehash) &&
                        (car_part->GetUpgradeLevel() == upg_level || upg_level == -1)) {
                        return car_part;
                    }

                    car_part++;
                }
            }
        }
    }

    return nullptr;
}

CarPart *CarPartDatabase::NewGetFirstCarPart(CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upg_level) {
    return this->NewGetCarPart(car_type, car_slot_id, car_part_namehash, nullptr, upg_level);
}

CarPart *CarPartDatabase::NewGetNextCarPart(CarPart *car_part, CarType car_type, int car_slot_id, unsigned int car_part_namehash, int upg_level) {
    return this->NewGetCarPart(car_type, car_slot_id, car_part_namehash, car_part, upg_level);
}

// STRIPPED
unsigned int CarPartDatabase::GetCarTypeNameHash(CarType car_type) {}

CarType CarPartDatabase::GetCarType(unsigned int car_type_name_hash) {
    for (int i = 0; i < NUM_CARTYPES;) {
        CarTypeInfo *car_info = GetCarTypeInfo(static_cast<CarType>(i));

        if (bStringHash(car_info->BaseModelName) != car_type_name_hash) {
            i++;
        } else {
            return car_info->GetCarType();
        }
    }

    return NUM_CARTYPES;
}

char *GetCarTypeName(CarType car_type) {
    CarTypeInfo *info = GetCarTypeInfo(car_type);

    if (info->GetName() != nullptr) {
        return info->CarTypeName;
    }

    return nullptr;
}

CarTypeInfo *GetCarTypeInfoFromHash(uint32 car_type_hash) {
    for (int i = 0; i < NUM_CARTYPES; i++) {
        if (CarTypeInfoArray[i].CarTypeNameHash == car_type_hash) {
            return &CarTypeInfoArray[i];
        }
    }

    return nullptr;
}

// STRIPPED
int CarTypeInfo::GetRandomSkin(unsigned int *pseed) {}

// STRIPPED
int CarTypeInfo::GetNumAvailableSkins() {}

// STRIPPED
int CarTypeInfo::GetAvailableSkin(int index) {}

// STRIPPED
int GetCarSlotAnim(CAR_SLOT_ID slot, int model_num) {}

uint32 TempSlotTable[2];

uint32 *GetTypesFromSlot(CAR_SLOT_ID slot, CarType car_type) {
    char *car_type_name = const_cast<char *>(GetCarTypeName(car_type));
    unsigned int car_type_name_hash = bStringHash(car_type_name);

    for (int i = 0; i < NumSlotTypeOverrides; i++) {
        CarSlotTypeOverride *ooverride = &SlotTypeOverrideTable[i];

        if (ooverride->CarType == car_type_name_hash && ooverride->SlotId == slot) {
            return ooverride->LookupType;
        }
    }

    int offset = slot * 2;
    bMemCpy(&TempSlotTable, &DefaultSlotTypeNameTable[offset], 8);

    for (int i = 0; i < 2; i++) {
        if (TempSlotTable[i] == 0xFFFFFFFF) {
            TempSlotTable[i] = car_type_name_hash;
        }
    }

    return TempSlotTable;
}

void RideInfo::Init(CarType type, CarRenderUsage usage, int has_dash, int can_be_vertex_damaged) {
    this->Type = type;
    this->InstanceIndex = 0;
    this->HasDash = has_dash;
    this->CanBeVertexDamaged = can_be_vertex_damaged;
    this->SkinType = 0;
    this->mCompositeSkinHash = 0;
    this->mMyCarLoaderHandle = 0;
    this->mMyCarRenderUsage = usage;
    this->mSpecialLODBehavior = 0;

    if (usage < static_cast<unsigned int>(CarRenderUsage_AIRacer)) {
        this->mMinLodLevel = CARPART_LOD_B;
        this->mMinFELodLevel = CARPART_LOD_B;
        this->mMaxLodLevel = CARPART_LOD_B;
        this->mMaxFELodLevel = CARPART_LOD_B;
    } else if (usage == CarRenderUsage_AIRacer) {
        this->mMinLodLevel = CARPART_LOD_C;
        this->mMinFELodLevel = CARPART_LOD_B;
        this->mMaxLodLevel = CARPART_LOD_E;
        this->mMaxFELodLevel = CARPART_LOD_E;
    } else {
        this->mMinLodLevel = CARPART_LOD_C;
        this->mMinFELodLevel = CARPART_LOD_B;
        this->mMaxLodLevel = CARPART_LOD_E;
        this->mMaxFELodLevel = CARPART_LOD_E;
    }

    if (this->Type != CARTYPE_NONE && CarTypeInfoArray != nullptr) {
        CarTypeInfo *info = GetCarTypeInfo(this->Type);

        if (info != nullptr && info->GetCarUsageType() == CAR_USAGE_TYPE_TRAFFIC) {
            this->mMinLodLevel = CARPART_LOD_C;
            this->mMinFELodLevel = CARPART_LOD_C;
            this->mMaxLodLevel = CARPART_LOD_E;
            this->mMaxFELodLevel = CARPART_LOD_E;
        }
    }

    if (Sim::GetUserMode() == Sim::USER_SPLIT_SCREEN) {
        this->mMinLodLevel = CARPART_LOD_C;
        this->mMaxLodLevel = CARPART_LOD_C;
    }

    this->mMaxLicenseLodLevel = CARPART_LOD_D;
    this->mMinTrafficDiffuseLodLevel = CARPART_LOD_D;
    this->mMinShadowLodLevel = CARPART_LOD_C;
    this->mMaxShadowLodLevel = CARPART_LOD_D;
    this->mMaxTireLodLevel = CARPART_LOD_D;
    this->mMaxBrakeLodLevel = CARPART_LOD_D;
    this->mMaxSpoilerLodLevel = CARPART_LOD_E;
    this->mMaxRoofScoopLodLevel = CARPART_LOD_E;
    this->mMinReflectionLodLevel = CARPART_LOD_E;

    if (usage == carRenderUsage_NISCar) {
        this->mMinLodLevel = CARPART_LOD_A;
        this->mMaxLodLevel = CARPART_LOD_A;
        this->mMaxTireLodLevel = CARPART_LOD_A;
        this->mSpecialLODBehavior = 2;
    }

    bMemSet(this->mPartsTable, 0, sizeof(this->mPartsTable));
    bMemSet(this->mPartsEnabled, 1, sizeof(this->mPartsEnabled));
    this->PreviewPart = nullptr;
}

int RideInfo::GetSpecialLODRangeForCarSlot(int slot_id, CARPART_LOD *special_minimum, CARPART_LOD *special_maximum, bool in_front_end) {
    switch (slot_id) {
        case CARSLOTID_DECAL_FRONT_WINDOW:
        case CARSLOTID_DECAL_REAR_WINDOW:
        case CARSLOTID_DECAL_LEFT_DOOR:
        case CARSLOTID_DECAL_RIGHT_DOOR:
        case CARSLOTID_DECAL_LEFT_QUARTER:
        case CARSLOTID_DECAL_RIGHT_QUARTER:
            *special_minimum = CARPART_LOD_A;
            *special_maximum = CARPART_LOD_A;
            return 1;

        case CARSLOTID_INTERIOR:
        case CARSLOTID_DRIVER:
            if (this->mSpecialLODBehavior == 2) {
                *special_minimum = CARPART_LOD_A;
                *special_maximum = CARPART_LOD_A;
            } else {
                if (this->mSpecialLODBehavior == 1) {
                    *special_minimum = CARPART_LOD_A;
                } else {
                    *special_minimum = in_front_end ? CARPART_LOD_B : CARPART_LOD_C;
                }

                *special_maximum = slot_id == CARSLOTID_DRIVER ? CARPART_LOD_C : CARPART_LOD_E;
            }

            return 1;

        default:
            return 0;
    }
}

// STRIPPED
void RideInfo::Print() {}

CarPart *FindPartWithLevel(CarType car_type, CAR_SLOT_ID slot_id, int level) {
    CarPart *part = CarPartDB.NewGetCarPart(car_type, slot_id, 0, nullptr, -1);

    while (part != nullptr) {
        if (part->GetUpgradeLevel() == level) {
            return part;
        }
        part = CarPartDB.NewGetNextCarPart(part, car_type, slot_id, 0, -1);
    }

    return nullptr;
}

void RideInfo::SetUpgradePart(CAR_SLOT_ID car_slot_id, int upg_level) {
    CAR_PART_ID car_part_id = GetCarPartFromSlot(car_slot_id);

    CarPart *part = FindPartWithLevel(this->Type, car_slot_id, upg_level);
    if (part != nullptr) {
        this->SetPart(car_slot_id, part, true);
    }
}

void RideInfo::SetStockParts() {
    ProfileNode profile_node("TODO", 0);

    for (int car_slot_id = 0; car_slot_id <= CARSLOTID_MISC; car_slot_id++) {
        if ((this->Type == CARTYPE_COPHELI && car_slot_id == CARSLOTID_ATTACHMENT6) || car_slot_id == CARSLOTID_VINYL_LAYER0 ||
            (car_slot_id >= CARSLOTID_DECAL_FRONT_WINDOW_TEX0 && car_slot_id <= CARSLOTID_DECAL_RIGHT_QUARTER_TEX7)) {
            continue;
        }

        switch (car_slot_id) {
            case CARSLOTID_BASE_PAINT: {
                CarTypeInfo *type_info = GetCarTypeInfo(this->Type);
                CarPart *paint_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_BASE_PAINT, type_info->GetDefaultBasePaint(), nullptr, -1);

                if (paint_part != nullptr) {
                    this->SetPart(CARSLOTID_BASE_PAINT, paint_part, true);
                }
                break;
            }

            case CARSLOTID_HUD_BACKING_COLOUR: {
                CarPart *hud_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_HUD_BACKING_COLOUR, bStringHash("ORANGE"), nullptr, -1);

                if (hud_part != nullptr) {
                    this->SetPart(CARSLOTID_HUD_BACKING_COLOUR, hud_part, true);
                } else {
                    this->SetUpgradePart(CARSLOTID_HUD_BACKING_COLOUR, 0);
                }
                break;
            }

            case CARSLOTID_HUD_NEEDLE_COLOUR: {
                CarPart *hud_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_HUD_NEEDLE_COLOUR, bStringHash("ORANGE"), nullptr, -1);

                if (hud_part != nullptr) {
                    this->SetPart(CARSLOTID_HUD_NEEDLE_COLOUR, hud_part, true);
                } else {
                    this->SetUpgradePart(CARSLOTID_HUD_NEEDLE_COLOUR, 0);
                }
                break;
            }

            case CARSLOTID_HUD_CHARACTER_COLOUR: {
                CarPart *hud_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_HUD_CHARACTER_COLOUR, bStringHash("WHITE"), nullptr, -1);

                if (hud_part != nullptr) {
                    this->SetPart(CARSLOTID_HUD_CHARACTER_COLOUR, hud_part, true);
                } else {
                    this->SetUpgradePart(CARSLOTID_HUD_CHARACTER_COLOUR, 0);
                }
                break;
            }

            default:
                this->SetUpgradePart(static_cast<CAR_SLOT_ID>(car_slot_id), 0);
                break;
        }
    }

    unsigned int stock_vinyl_colours[4] = {bStringHash("VINYL_L1_COLOR01"), bStringHash("VINYL_L1_COLOR03"), bStringHash("VINYL_L2_COLOR11"),
                                           bStringHash("VINYL_L1_COLOR01")};

    for (int j = 0; j < 4; j++) {
        int car_slot_id = j + CARSLOTID_VINYL_COLOUR0_0;
        CarPart *vinyl_paint_part = CarPartDB.NewGetCarPart(this->Type, car_slot_id, stock_vinyl_colours[j], nullptr, -1);

        this->SetPart(car_slot_id, vinyl_paint_part, true);
    }
}

// TODO it uses enums in A124
struct MissingCarPart {
    int16 Type;
    int16 Slot;
    uint32 PartNameHash;
};

// TODO we have these hashes but there are no macros for them yet
MissingCarPart MissingCarPartTable[330] = {
    {CARTYPE_911TURBO, CARSLOTID_BODY, 0xE8D2E5FF},      {CARTYPE_CARRERAGT, CARSLOTID_BODY, 0xE956A4F1},
    {CARTYPE_CARRERAGT, CARSLOTID_BODY, 0xE956A4F2},     {CARTYPE_CARRERAGT, CARSLOTID_BODY, 0xE956A4F3},
    {CARTYPE_VIPER, CARSLOTID_BODY, 0x8F168B7E},         {CARTYPE_VIPER, CARSLOTID_SPOILER, 0xD9ABD542},
    {CARTYPE_RX8, CARSLOTID_SPOILER, 0xC784955E},        {CARTYPE_CAMARO, CARSLOTID_BODY, 0x2BD81027},
    {CARTYPE_CAMARO, CARSLOTID_BODY, 0x2BD81028},        {CARTYPE_CAMARO, CARSLOTID_BODY, 0x2BD81029},
    {CARTYPE_CAMARO, CARSLOTID_BODY, 0x2BD8102A},        {CARTYPE_CAMARO, CARSLOTID_BODY, 0x2BD8102B},
    {CARTYPE_CAMARO, CARSLOTID_SPOILER, 0x3B1AD10F},     {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x991CEE62},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x7F1053D1},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x81657C72},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xC50B16AF},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xCC0A9092},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xCE5FB933},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x0B05D98D},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x75BBB14D},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x7810D9EE},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x7A66028F},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x12055370},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x1904CD53},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x5CAA6790},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x1307AB39},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x87841C3A},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xB99CE957},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x17123C5A},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x8B8EAD5B},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x60322775},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x7CBB2B30},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x860FCDB4},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xBE0B9CCC},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x0D5B022E},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x5A553EEF},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x4115E735},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xB5925836},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x2A0EC937},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xBDA77A78},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x1B1CCD7B},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x4D359A98},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x9E8B3A38},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x707CFE3C},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xD4AE9876},
    {CARTYPE_CAMARO, CARSLOTID_HOOD, 0x5C279654},        {CARTYPE_CAMARO, CARSLOTID_HOOD, 0xD8B92997},
    {CARTYPE_SLR, CARSLOTID_BODY, 0xB2C9E987},           {CARTYPE_SLR, CARSLOTID_BODY, 0xB2C9E988},
    {CARTYPE_SLR, CARSLOTID_BODY, 0xB2C9E989},           {CARTYPE_SLR, CARSLOTID_SPOILER, 0xF105F72D},
    {CARTYPE_CORVETTE, CARSLOTID_BODY, 0xABA9EEC4},      {CARTYPE_CORVETTE, CARSLOTID_SPOILER, 0xB58A8E48},
    {CARTYPE_A3, CARSLOTID_SPOILER, 0x1807EFB0},         {CARTYPE_BMWM3GTR, CARSLOTID_BODY, 0x8E197287},
    {CARTYPE_BMWM3GTR, CARSLOTID_BODY, 0x8E197288},      {CARTYPE_BMWM3GTR, CARSLOTID_BODY, 0x8E197289},
    {CARTYPE_BMWM3GTR, CARSLOTID_BODY, 0x8E19728A},      {CARTYPE_BMWM3GTR, CARSLOTID_BODY, 0x8E19728B},
    {CARTYPE_BMWM3GTR, CARSLOTID_SPOILER, 0x74304F6F},   {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xFB5E50C2},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x7733CE31},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x7988F6D2},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xBD2E910F},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xC42E0AF2},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xC6833393},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x032953ED},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x6DDF2BAD},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x7034544E},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x72897CEF},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x0A28CDD0},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x112847B3},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x54CDE1F0},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x7F658999},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xF3E1FA9A},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x25FAC7B7},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x83701ABA},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xF7EC8BBB},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xCC9005D5},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x74DEA590},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x7E334814},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xB62F172C},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x057E7C8E},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x5278B94F},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xAD73C595},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x21F03696},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x966CA797},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x2A0558D8},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x877AABDB},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xB99378F8},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x0AE91898},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xDCDADC9C},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x410C76D6},
    {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0xC88574B4},      {CARTYPE_BMWM3GTR, CARSLOTID_HOOD, 0x451707F7},
    {CARTYPE_SL500, CARSLOTID_SPOILER, 0xA03CC930},      {CARTYPE_911GT2, CARSLOTID_BODY, 0x46E0B5BC},
    {CARTYPE_911GT2, CARSLOTID_BODY, 0x46E0B5BD},        {CARTYPE_911GT2, CARSLOTID_BODY, 0x46E0B5BE},
    {CARTYPE_911GT2, CARSLOTID_BODY, 0x46E0B5BF},        {CARTYPE_911GT2, CARSLOTID_BODY, 0x46E0B5C0},
    {CARTYPE_911GT2, CARSLOTID_SPOILER, 0x903256C4},     {CARTYPE_911GT2, CARSLOTID_HOOD, 0xB42593F7},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x7ED8B2A6},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x812DDB47},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0xC4D37584},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0xCBD2EF67},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0xCE281808},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x0ACE3862},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x75841022},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x77D938C3},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x7A2E6164},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x11CDB245},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x18CD2C28},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x5C72C665},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x91D015EE},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x064C86EF},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x3865540C},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x95DAA70F},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x0A571810},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0xDEFA922A},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x7C838A05},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x85D82C89},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0xBDD3FBA1},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x0D236103},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x5A1D9DC4},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0xBFDE51EA},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x345AC2EB},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0xA8D733EC},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0x3C6FE52D},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x99E53830},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0xCBFE054D},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x1D53A4ED},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0xEF4568F1},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x5377032B},
    {CARTYPE_911GT2, CARSLOTID_HOOD, 0xDAF00109},        {CARTYPE_911GT2, CARSLOTID_HOOD, 0x5781944C},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_BODY, 0x804DA256},   {CARTYPE_BMWM3GTRE46, CARSLOTID_BODY, 0x804DA257},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_BODY, 0x804DA258},   {CARTYPE_BMWM3GTRE46, CARSLOTID_BODY, 0x804DA259},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_BODY, 0x804DA25A},   {CARTYPE_BMWM3GTRE46, CARSLOTID_SPOILER, 0x6452F99E},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xED928091},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC7332DC0},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC9885661},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x0D2DF09E},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x142D6A81},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x16829322},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x5328B37C},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xBDDE8B3C},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC033B3DD},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC288DC7E},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x5A282D5F},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x6127A742},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xA4CD417F},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x776AF2C8},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xEBE763C9},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x1E0030E6},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x7B7583E9},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xEFF1F4EA},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC4956F04},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC4DE051F},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xCE32A7A3},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x062E76BB},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x557DDC1D},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xA27818DE},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xA5792EC4},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x19F59FC5},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x8E7210C6},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x220AC207},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x7F80150A},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xB198E227},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x02EE81C7},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xD4E045CB},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x3911E005},   {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0xC08ADDE3},
    {CARTYPE_BMWM3GTRE46, CARSLOTID_HOOD, 0x3D1C7126},   {CARTYPE_IS300, CARSLOTID_BODY, 0x97C932E7},
    {CARTYPE_IS300, CARSLOTID_SPOILER, 0xD2748D4B},      {CARTYPE_SL65, CARSLOTID_BODY, 0x3D86CE7E},
    {CARTYPE_SL65, CARSLOTID_BODY, 0x3D86CE7F},          {CARTYPE_SL65, CARSLOTID_BODY, 0x3D86CE80},
    {CARTYPE_SL65, CARSLOTID_BODY, 0x3D86CE81},          {CARTYPE_SL65, CARSLOTID_BODY, 0x3D86CE82},
    {CARTYPE_SL65, CARSLOTID_SPOILER, 0x80F756C6},       {CARTYPE_SL65, CARSLOTID_HOOD, 0xAACBACB9},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xB76803E8},          {CARTYPE_SL65, CARSLOTID_HOOD, 0xB9BD2C89},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xFD62C6C6},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x046240A9},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x06B7694A},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x435D89A4},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xAE136164},          {CARTYPE_SL65, CARSLOTID_HOOD, 0xB0688A05},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xB2BDB2A6},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x4A5D0387},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x515C7D6A},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x950217A7},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x608DF7F0},          {CARTYPE_SL65, CARSLOTID_HOOD, 0xD50A68F1},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x0723360E},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x64988911},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xD914FA12},          {CARTYPE_SL65, CARSLOTID_HOOD, 0xADB8742C},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xB512DB47},          {CARTYPE_SL65, CARSLOTID_HOOD, 0xBE677DCB},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xF6634CE3},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x45B2B245},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x92ACEF06},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x8E9C33EC},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x0318A4ED},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x779515EE},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x0B2DC72F},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x68A31A32},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0x9ABBE74F},          {CARTYPE_SL65, CARSLOTID_HOOD, 0xEC1186EF},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xBE034AF3},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x2234E52D},
    {CARTYPE_SL65, CARSLOTID_HOOD, 0xA9ADE30B},          {CARTYPE_SL65, CARSLOTID_HOOD, 0x263F764E},
    {CARTYPE_ELISE, CARSLOTID_SPOILER, 0xCFEF89CE},      {CARTYPE_FORDGT, CARSLOTID_BODY, 0xFD1F047C},
    {CARTYPE_FORDGT, CARSLOTID_BODY, 0xFD1F047D},        {CARTYPE_FORDGT, CARSLOTID_BODY, 0xFD1F047E},
    {CARTYPE_CORVETTEC6R, CARSLOTID_BODY, 0x3AD7B64B},   {CARTYPE_CORVETTEC6R, CARSLOTID_BODY, 0x3AD7B64C},
    {CARTYPE_CORVETTEC6R, CARSLOTID_BODY, 0x3AD7B64D},   {CARTYPE_CORVETTEC6R, CARSLOTID_BODY, 0x3AD7B64E},
    {CARTYPE_CORVETTEC6R, CARSLOTID_BODY, 0x3AD7B64F},   {CARTYPE_CORVETTEC6R, CARSLOTID_SPOILER, 0x3CDB89B3},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xA81C9486},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x4C9212F5},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x4EE73B96},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x928CD5D3},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x998C4FB6},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x9BE17857},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xD88798B1},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x433D7071},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x45929912},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x47E7C1B3},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xDF871294},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xE6868C77},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x2A2C26B4},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xE5B8C7DD},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x5A3538DE},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x8C4E05FB},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xE9C358FE},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x5E3FC9FF},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x32E34419},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x4A3CEA54},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x53918CD8},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x8B8D5BF0},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xDADCC152},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x27D6FE13},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x13C703D9},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x884374DA},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xFCBFE5DB},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x9058971C},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xEDCDEA1F},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x1FE6B73C},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x713C56DC},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x432E1AE0},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xA75FB51A},   {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0x2ED8B2F8},
    {CARTYPE_CORVETTEC6R, CARSLOTID_HOOD, 0xAB6A463B},   {CARTYPE_RX8SPEEDT, CARSLOTID_BODY, 0x6267CEFB},
    {CARTYPE_RX8SPEEDT, CARSLOTID_BODY, 0x6267CEFC},     {CARTYPE_RX8SPEEDT, CARSLOTID_BODY, 0x6267CEFD},
    {CARTYPE_RX8SPEEDT, CARSLOTID_BODY, 0x6267CEFE},     {CARTYPE_RX8SPEEDT, CARSLOTID_BODY, 0x6267CEFF},
    {CARTYPE_RX8SPEEDT, CARSLOTID_SPOILER, 0xDA42E063},  {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x988B17A5},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x9AE04046},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xDE85DA83},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xE5855466},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xE7DA7D07},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x24809D61},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x8F367521},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x918B9DC2},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x93E0C663},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x2B801744},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x327F9127},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x76252B64},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xDDA3CE8D},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x52203F8E},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x84390CAB},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xE1AE5FAE},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x562AD0AF},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x2ACE4AC9},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x9635EF04},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x9F8A9188},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xD78660A0},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x26D5C602},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x73D002C3},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x0BB20A89},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x802E7B8A},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xF4AAEC8B},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x88439DCC},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xE5B8F0CF},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x17D1BDEC},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x69275D8C},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x3B192190},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x9F4ABBCA},     {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0x26C3B9A8},
    {CARTYPE_RX8SPEEDT, CARSLOTID_HOOD, 0xA3554CEB},     {CARTYPE_MURCIELAGO, CARSLOTID_BODY, 0x5C7E635E},
    {CARTYPE_MURCIELAGO, CARSLOTID_BODY, 0x5C7E635F},    {CARTYPE_MURCIELAGO, CARSLOTID_BODY, 0x5C7E6360},
    {CARTYPE_MURCIELAGO, CARSLOTID_SPOILER, 0x74D5E864}, {CARTYPE_A4, CARSLOTID_SPOILER, 0x8C8460B1},
    {CARTYPE_GTI, CARSLOTID_BODY, 0xAE31399C},           {CARTYPE_CTS, CARSLOTID_BODY, 0xAAB1F8E2},
    {CARTYPE_CTS, CARSLOTID_SPOILER, 0x300B9D26},        {CARTYPE_DB9, CARSLOTID_SPOILER, 0x9806A3FB},
    {CARTYPE_GALLARDO, CARSLOTID_BODY, 0x6EA1C69E},      {CARTYPE_BMWM3, CARSLOTID_BODY, 0x544EDD9A},
    {CARTYPE_BMWM3, CARSLOTID_BODY, 0x544EDD9B},         {CARTYPE_BMWM3, CARSLOTID_BODY, 0x544EDD9C},
    {CARTYPE_BMWM3, CARSLOTID_BODY, 0x544EDD9D},         {CARTYPE_BMWM3, CARSLOTID_BODY, 0x544EDD9E},
    {CARTYPE_BMWM3, CARSLOTID_SPOILER, 0x14D85B62},      {CARTYPE_BMWM3, CARSLOTID_HOOD, 0x7E0A5DC3},
    {CARTYPE_BMWM3, CARSLOTID_HOOD, 0x7BB53522},         {CARTYPE_BMWM3, CARSLOTID_HOOD, 0x6D35C7EB},
    {CARTYPE_BMWM3, CARSLOTID_HOOD, 0xF8B956EA},         {CARTYPE_CLIO, CARSLOTID_SPOILER, 0x932DC043},
    {CARTYPE_PUNTO, CARSLOTID_BODY, 0x8A67D06E},         {CARTYPE_PUNTO, CARSLOTID_SPOILER, 0x1D49C032},
};

void GenerateMissingCarParts() {
    if (0) {
        for (int car_type = 0; car_type < NUM_CARTYPES; car_type++) {
            CarTypeInfo *car_type_info = GetCarTypeInfo(static_cast<CarType>(car_type));

            // if (car_type_info->GetCarUsageType() != CAR_USAGE_TYPE_PLAYER)
            {
                char filename_textures[100];
                char filename_vinyls[100];
                int slots_to_check[5];
                int num_exist = 0;
                int num_parts = 0;

                eLoadStreamingTexturePack(filename_textures);
                eLoadStreamingTexturePack(filename_vinyls);
                // eLoadStreamingSolidPack(filename_textures); // TODO create this inline somewhere

                for (int n = 0; n < 5; n++) {
                    int slot = slots_to_check[n];
                    CarPart *part = CarPartDB.NewGetCarPart(car_type_info->GetCarType(), slot, 0, nullptr, -1);

                    if (part != nullptr) {
                        bool skip = false;

                        if (part->GetGroupNumber() == 0) {
                            skip = true;
                        }

                        if (bStrEqual(filename_textures, filename_vinyls)) {
                            skip = true;
                        }

                        {
                            bool exists = false;

                            if (!skip && !exists) {
                                num_parts++;
                            }
                        }

                        if (!skip) {
                            num_exist++;
                        }

                        car_type_info->GetCarType();
                    }
                }
            }
        }
    }
}

void RideInfo::SetRandomParts() {
    this->SetStockParts();
    int randomset = bRandom(3);

    switch (randomset) {
        case 0:
            this->SetRandomPart(CARSLOTID_BODY, -1);
            this->SetRandomPart(CARSLOTID_SPOILER, -1);
            this->SetRandomPart(CARSLOTID_BASE_PAINT, -1);
            this->SetRandomPart(CARSLOTID_FRONT_WHEEL, -1);
            this->SetRandomPart(CARSLOTID_PAINT_RIM, -1);
            break;
        case 1:
            this->SetRandomPart(CARSLOTID_BASE_PAINT, -1);
            this->SetRandomPart(CARSLOTID_VINYL_LAYER0, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_0, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_1, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_2, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_3, -1);
            break;
        case 2:
            this->SetRandomPart(CARSLOTID_BODY, -1);
            this->SetRandomPart(CARSLOTID_SPOILER, -1);
            this->SetRandomPart(CARSLOTID_FRONT_WHEEL, -1);
            this->SetRandomPart(CARSLOTID_PAINT_RIM, -1);
            this->SetRandomPart(CARSLOTID_WINDOW_TINT, -1);
            this->SetRandomPart(CARSLOTID_ROOF, -1);
            this->SetRandomPart(CARSLOTID_HOOD, -1);
            this->SetRandomPart(CARSLOTID_BASE_PAINT, -1);
            this->SetRandomPart(CARSLOTID_VINYL_LAYER0, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_0, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_1, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_2, -1);
            this->SetRandomPart(CARSLOTID_VINYL_COLOUR0_3, -1);
            break;
    }
}

void RideInfo::SetRandomPart(CAR_SLOT_ID slot, int upgrade_level) {
    int num_parts = CarPartDB.NewGetNumCarParts(this->Type, slot, 0, upgrade_level);
    CarPart *part = nullptr;
    bool foundModel = false;
    int attempt = 1;

    do {
        int part_number = bRandom(num_parts) + 1;

        for (int i = 0; i < part_number; i++) {
            part = CarPartDB.NewGetNextCarPart(part, this->Type, slot, 0, upgrade_level);
        }

        if (part != nullptr) {
            bool part_missing;

            if (slot != CARSLOTID_VINYL_LAYER0 ||
                (part->GetGroupNumber() != 8 && (part->GetBrandNameHash() != bStringHash("CEO") || GetIsCollectorsEdition() != 0))) {
                if (slot != CARSLOTID_BASE_PAINT || this->mMyCarRenderUsage != CarRenderUsage_AIRacer ||
                    part->GetBrandNameHash() != STRINGHASH_PEARL) {
                    part_missing = false;

                    for (int n = 0; n < NUM_ELEMENTS(MissingCarPartTable); n++) {
                        MissingCarPart *missing_part = &MissingCarPartTable[n];

                        if (missing_part->Type == this->Type && missing_part->Slot == slot && missing_part->PartNameHash == part->GetPartNameHash()) {
                            part_missing = true;
                            break;
                        }
                    }

                    if (!part_missing) {
                        foundModel = true;
                        this->SetPart(slot, part, true);
                    }
                }
            }
        }

        if (!foundModel) {
            attempt++;
        }
    } while (!foundModel && attempt < 16);
}

void RideInfo::SetRandomPaint() {
    int num_paints = CarPartDB.NewGetNumCarParts(this->Type, CARSLOTID_BASE_PAINT, 0, -1);
    CarPart *paint_part = nullptr;
    int paint_number = bRandom(num_paints) + 1;
    for (int i = 0; i < paint_number; i++) {
        paint_part = CarPartDB.NewGetNextCarPart(paint_part, this->Type, CARSLOTID_BASE_PAINT, 0, -1);
    }

    this->SetPart(CARSLOTID_BASE_PAINT, paint_part, true);
    for (int i = CARSLOTID_VINYL_LAYER0; i < CARSLOTID_PAINT_RIM; i++) {
        this->SetPart(i, nullptr, true);
    }
}

CarPart *RideInfo::GetPart(int car_slot_id) const {
    return this->mPartsTable[car_slot_id];
}

CarPart *RideInfo::SetPart(int car_slot_id, CarPart *car_part, bool update_enabled) {
    switch (car_slot_id) {
        case CARSLOTID_DAMAGE0_FRONT:
        case CARSLOTID_DAMAGE0_FRONTLEFT:
        case CARSLOTID_DAMAGE0_FRONTRIGHT:
        case CARSLOTID_DAMAGE0_REAR:
        case CARSLOTID_DAMAGE0_REARLEFT:
        case CARSLOTID_DAMAGE0_REARRIGHT:
        case CARSLOTID_REAR_WHEEL:
        case CARSLOTID_DECAL_LEFT_DOOR:
        case CARSLOTID_DECAL_RIGHT_DOOR:
        case CARSLOTID_DECAL_LEFT_QUARTER:
        case CARSLOTID_DECAL_RIGHT_QUARTER:
            return this->mPartsTable[car_slot_id];

        case CARSLOTID_BODY:
            if (car_part == nullptr) {
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONT] = nullptr;
                this->mPartsTable[CARSLOTID_DAMAGE0_REAR] = nullptr;
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTLEFT] = nullptr;
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTRIGHT] = nullptr;
                this->mPartsTable[CARSLOTID_DAMAGE0_REARLEFT] = nullptr;
                this->mPartsTable[CARSLOTID_DAMAGE0_REARRIGHT] = nullptr;
            } else {
                int kit_number = car_part->GetAppliedAttributeIParam(STRINGHASH_KITNUMBER, 0);
                char buffer[64];

                bSPrintf(buffer, "%s_KIT%02d_", GetCarTypeName(this->Type), kit_number);

                unsigned int base_hash = bStringHash(buffer);
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_FRONT, bStringHash("DAMAGE0_FRONT", base_hash), nullptr, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_REAR] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_REAR, bStringHash("DAMAGE0_REAR", base_hash), nullptr, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTLEFT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_FRONTLEFT, bStringHash("DAMAGE0_FRONTLEFT", base_hash), nullptr, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_FRONTRIGHT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_FRONTRIGHT, bStringHash("DAMAGE0_FRONTRIGHT", base_hash), nullptr, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_REARLEFT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_REARLEFT, bStringHash("DAMAGE0_REARLEFT", base_hash), nullptr, -1);
                this->mPartsTable[CARSLOTID_DAMAGE0_REARRIGHT] =
                    CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DAMAGE0_REARRIGHT, bStringHash("DAMAGE0_REARRIGHT", base_hash), nullptr, -1);
            }

            if (car_part == nullptr) {
                this->mPartsTable[CARSLOTID_DECAL_LEFT_DOOR] = nullptr;
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_DOOR] = nullptr;
                this->mPartsTable[CARSLOTID_DECAL_LEFT_QUARTER] = nullptr;
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_QUARTER] = nullptr;
            } else {
                int kit_number = car_part->GetAppliedAttributeIParam(STRINGHASH_KITNUMBER, 0);
                char buffer[64];

                bSPrintf(buffer, "%s_KIT%02d_", GetCarTypeName(this->Type), kit_number);

                unsigned int base_hash = bStringHash(buffer);
                CarPart *left_door_decal_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_LEFT_DOOR,
                                                                        bStringHash("DECAL_LEFT_DOOR_RECT_MEDIUM", base_hash), nullptr, -1);
                CarPart *right_door_decal_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_RIGHT_DOOR,
                                                                         bStringHash("DECAL_RIGHT_DOOR_RECT_MEDIUM", base_hash), nullptr, -1);
                CarPart *left_quarter_decal_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_LEFT_QUARTER,
                                                                           bStringHash("DECAL_LEFT_QUARTER_RECT_MEDIUM", base_hash), nullptr, -1);
                CarPart *right_quarter_decal_part = CarPartDB.NewGetCarPart(this->Type, CARSLOTID_DECAL_RIGHT_QUARTER,
                                                                            bStringHash("DECAL_RIGHT_QUARTER_RECT_MEDIUM", base_hash), nullptr, -1);

                this->mPartsTable[CARSLOTID_DECAL_LEFT_DOOR] = left_door_decal_part;
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_DOOR] = right_door_decal_part;
                this->mPartsTable[CARSLOTID_DECAL_LEFT_QUARTER] = left_quarter_decal_part;
                this->mPartsTable[CARSLOTID_DECAL_RIGHT_QUARTER] = right_quarter_decal_part;
            }
            break;

        default:
            break;
    }

    CarPart *previous_part = this->mPartsTable[car_slot_id];
    this->mPartsTable[car_slot_id] = car_part;

    if (update_enabled) {
        this->UpdatePartsEnabled();
    }

    return previous_part;
}

void RideInfo::UpdatePartsEnabled() {
    bMemSet(this->mPartsEnabled, 1, sizeof(this->mPartsEnabled));
    int num_car_slot_names = GetNumCarSlotIDNames();

    for (int i = 0; i < CARSLOTID_NUM; i++) {
        CarPart *part;
        if (i == CARSLOTID_FRONT_WHEEL) {
            const unsigned int brake_paint_hash = bStringHash("CALIPER");
            part = this->PreviewPart;
            bool is_part_brake_paint = false;
            if (part != nullptr) {
                if (part->PartID == CARPARTID_PAINT) {
                    is_part_brake_paint = part->GetBrandNameHash() == brake_paint_hash;
                }
                part = this->PreviewPart;
                if ((part != nullptr) && (part->PartID == CARPARTID_BRAKE || is_part_brake_paint)) {
                    this->mPartsEnabled[i] = 0;
                }
            }
        }
    }
}

int RideInfo::IsPartEnabled(int car_part_id) {
    return this->mPartsEnabled[car_part_id];
}

unsigned int RideInfo::GetSkinNameHash() {
    CarPart *skin_base;

    if (this->IsUsingCompositeSkin() == 0) {
        skin_base = this->GetPart(CARSLOTID_BASE_PAINT);

        if (skin_base == nullptr) {
            return 0;
        }

        return skin_base->GetTextureNameHash();
    }

    return this->mCompositeSkinHash;
}

// TODO move?
enum eSkinType {
    SKIN_NONE = 0,
    SKIN_COMPOSITE = 1,
    SKIN_PRECOMPOSITE = 2,
};

enum eSkinID {
    SKIN_ID_COMPOSITE_1 = 1,
    SKIN_ID_COMPOSITE_2 = 2,
    SKIN_ID_COMPOSITE_3 = 3,
    SKIN_ID_COMPOSITE_4 = 4,
    SKIN_ID_COMPOSITE_LAST = 4,
    SKIN_ID_PRECOMPOSITE_1 = 5,
    SKIN_ID_PRECOMPOSITE_2 = 6,
    SKIN_ID_PRECOMPOSITE_3 = 7,
    SKIN_ID_PRECOMPOSITE_4 = 8,
    SKIN_ID_PRECOMPOSITE_5 = 9,
    SKIN_ID_PRECOMPOSITE_6 = 10,
    SKIN_ID_PRECOMPOSITE_7 = 11,
    SKIN_ID_PRECOMPOSITE_8 = 12,
    SKIN_ID_PRECOMPOSITE_LAST = 12,
};

void RideInfo::SetCompositeNameHash(int skin_number) {
    if (skin_number >= SKIN_ID_COMPOSITE_1 && skin_number <= SKIN_ID_COMPOSITE_LAST) {
        this->SkinType = SKIN_COMPOSITE;
    } else if (skin_number >= SKIN_ID_PRECOMPOSITE_1 && skin_number <= SKIN_ID_PRECOMPOSITE_LAST) {
        this->SkinType = SKIN_PRECOMPOSITE;
    } else {
        this->SkinType = SKIN_NONE;
    }

    char temp[64];
    bSPrintf(temp, "DUMMY_SKIN%d", skin_number);
    unsigned int dummy_skin_hash = bStringHash(temp);
    this->SetCompositeSkinNameHash(dummy_skin_hash);

    bSPrintf(temp, "DUMMY_WHEEL%d", skin_number);
    unsigned int dummy_wheel_hash = bStringHash(temp);
    this->SetCompositeWheelNameHash(dummy_wheel_hash);

    bSPrintf(temp, "DUMMY_SPINNER%d", skin_number);
    unsigned int dummy_spinner_hash = bStringHash(temp);
    this->SetCompositeSpinnerNameHash(dummy_spinner_hash);
}

unsigned int RideInfo::GetCompositeSkinNameHash() {
    return this->mCompositeSkinHash;
}

static const int UseVinylSystem = 1;

void RideInfo::SetCompositeSkinNameHash(unsigned int namehash) {
    CarTypeInfo *type_info = GetCarTypeInfo(this->Type);

    if (type_info->Skinnable != 0 && UseVinylSystem) {
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
    return static_cast<int>(this->GetCompositeSkinNameHash() != 0);
}

// STRIPPED
uint32 RideInfo::GetCollisionVolumeNameHash() {}

// STRIPPED
uint32 RideInfo::GetDefaultCollisionVolumeNameHash() {}

void RideInfo::DumpForPreset(FECarRecord *car) {
    CarTypeInfo *cti = GetCarTypeInfo(this->Type);
    for (int i = 0; i < CARSLOTID_NUM; i++) {
        const char *slot_name = GetCarSlotNameFromID(i);
        CarPart *part = this->mPartsTable[i];

        if (part != nullptr) {
            unsigned int part_hash;
            const char *display_name = part->GetName();
        }
    }
}

bTList<PresetCar> PresetCarList;

int LoaderFEPresetCars(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_FE_CAR_PRESETS) {
        int num_presets = chunk->GetSize() / sizeof(PresetCar);
        PresetCar *preset_car = reinterpret_cast<PresetCar *>(chunk->GetData());

        while (num_presets-- != 0) {
            for (int i = 0; i < CARSLOTID_NUM; i++) {
                bPlatEndianSwap(&preset_car->PartNameHashes[i]);
            }

            bPlatEndianSwap(&preset_car->VehicleKey);
            bPlatEndianSwap(&preset_car->FEKey);
            bPlatEndianSwap(&preset_car->PhysicsLevel);
            bPlatEndianSwap(&preset_car->FilterBits);

            PresetCarList.AddTail(preset_car);
            preset_car++;
        }

        return 1;
    }

    return 0;
}

int UnloaderFEPresetCars(bChunk *chunk) {
    if (chunk->GetID() == BCHUNK_FE_CAR_PRESETS) {
        while (!PresetCarList.IsEmpty()) {
            PresetCarList.RemoveHead();
        }

        return 1;
    }

    return 0;
}

int GetNumPresetCars() {
    return PresetCarList.CountElements();
}

PresetCar *GetPresetCarAt(int index) {
    return PresetCarList.GetNode(index);
}

PresetCar *FindFEPresetCar(unsigned int hash) {
    for (PresetCar *p = PresetCarList.GetHead(); p != PresetCarList.EndOfList(); p = p->GetNext()) {
        if (hash == FEHashUpper(p->PresetName)) {
            return p;
        }
    }

    return nullptr;
}

void RideInfo::FillWithPreset(unsigned int preset_name_hash) {
    PresetCar *preset = FindFEPresetCar(preset_name_hash);

    if (preset != nullptr) {
        this->SkinType = 1;
        // TODO GetCarTypeInfo inline call missing
        CarTypeInfo *cti = GetCarTypeInfoFromHash(FEHashUpper(preset->CarTypeName));
        CarType type = cti->Type;

        if (type != this->Type) {
            this->Init(type, CarRenderUsage_Player, 0, 0);
            this->SetStockParts();
        }

        for (int i = 0; i < CARSLOTID_NUM; i++) {
            unsigned int part_name_hash = preset->PartNameHashes[i];

            CarPart *part;
            if (part_name_hash != 0) {
                if (part_name_hash != 1) {
                    part = CarPartDB.NewGetCarPart(type, i, part_name_hash, nullptr, -1);
                    this->SetPart(i, part, true);
                }
            } else {
                this->SetPart(i, nullptr, true);
            }
        }
    }
}

// STRIPPED
char *GetDriverTypeName(DriverTypeEnum driver_type) {}

// STRIPPED
DriverInfo::DriverInfo(int unique_driver_number, DriverTypeEnum driver_type, RideInfo *ride_info, const char *character_name, int skill_level,
                       int personality_type, int starting_position) {}

// STRIPPED
void DriverInfo::DoSnapshot(ReplaySnapshot *snapshot) {}

static const int OverrideAISkillLevels = 0;

static const int NewAISkillLevel = 0;

// STRIPPED
int8 DriverInfo::GetSkillLevel() {}

int UsedCarTextureAddToTable(unsigned int *table, int num_used, int max_textures, unsigned int texture_hash) {
    if (texture_hash == 0) {
        return 0;
    }

    for (int i = 0; i < num_used; i++) {
        if (table[i] == texture_hash) {
            return 0;
        }
    }

    if (num_used >= max_textures) {
        return num_used;
    }

    table[num_used] = texture_hash;
    return 1;
}

// STRIPPED
void PrintUsedCarTextureInfo(UsedCarTextureInfo *info) {}

static const int PrintUsedCarTexture = 0; // TODO use

void GetUsedCarTextureInfo(UsedCarTextureInfo *info, RideInfo *ride_info, int front_end_only) {
    CarTypeInfo *car_type_info = GetCarTypeInfo(ride_info->Type);
    char *car_base_name = car_type_info->BaseModelName;
    int max_perm_textures = CARPARTID_NUM;
    int max_temp_textures = CARPARTID_NUM;
    int num_perm_textures = 0;
    int num_temp_textures = 0;
    int debug_print;

    bMemSet(info, 0, sizeof(UsedCarTextureInfo));

    char buffer[64];
    bSPrintf(buffer, "%s_SKIN1", car_base_name);
    info->MappedSkinHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN1B", car_base_name);
    info->MappedSkinBHash = bStringHash(buffer);
    info->MappedGlobalHash = bStringHash("GLOBAL_SKIN1");

    CarPart *wheel_part = ride_info->GetPart(CARSLOTID_FRONT_WHEEL);

    if (wheel_part != nullptr) {
        info->MappedWheelHash = bStringHash("_WHEEL", wheel_part->GetTextureNameHash());
        unsigned int spinner_hash = wheel_part->GetAppliedAttributeUParam(bStringHash("SPINNER_TEXTURE"), 0);

        if (spinner_hash != 0) {
            info->MappedSpinnerHash = spinner_hash;
        } else {
            info->MappedSpinnerHash = 0;
        }
    } else {
        info->MappedWheelHash = 0;
        info->MappedSpinnerHash = 0;
    }

    info->MappedSpoilerHash = bStringHash("SPOILER_SKIN1");
    info->MappedRoofScoopHash = bStringHash("ROOF_SKIN");

    if (ride_info->IsUsingCompositeSkin() != 0) {
        info->ReplaceSkinHash = ride_info->GetSkinNameHash();
        info->ReplaceWheelHash = ride_info->GetCompositeWheelNameHash();
        info->ReplaceSpinnerHash = ride_info->GetCompositeSpinnerNameHash();
        info->ReplaceSpoilerHash = ride_info->GetSkinNameHash();
        info->ReplaceRoofScoopHash = ride_info->GetSkinNameHash();
    } else {
        info->ReplaceSkinHash = info->MappedSkinHash;
        info->ReplaceSpoilerHash = info->MappedSkinHash;
        info->ReplaceRoofScoopHash = info->MappedSkinHash;
        info->ReplaceWheelHash = 0;
        info->ReplaceSpinnerHash = 0;
    }

    info->ReplaceSkinBHash = 0;
    info->ReplaceGlobalHash = info->ReplaceSkinHash;

    unsigned int composite_skin_hash = ride_info->GetCompositeSkinNameHash();

    if (composite_skin_hash != 0) {
        num_perm_textures = UsedCarTextureAddToTable(info->TexturesToLoadPerm, 0, max_perm_textures, composite_skin_hash);
    } else {
        if (info->ReplaceSkinHash != 0) {
            num_perm_textures = UsedCarTextureAddToTable(info->TexturesToLoadPerm, 0, max_perm_textures, info->ReplaceSkinHash);
        }

        if (info->ReplaceSkinBHash != 0) {
            num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceSkinBHash);
        }
    }

    unsigned int composite_wheel_hash = ride_info->GetCompositeWheelNameHash();

    if (composite_wheel_hash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, composite_wheel_hash);
    } else if (info->ReplaceWheelHash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceWheelHash);
    }

    unsigned int composite_spinner_hash = ride_info->GetCompositeSpinnerNameHash();

    if (composite_spinner_hash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, composite_spinner_hash);
    } else if (info->ReplaceSpinnerHash != 0) {
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceSpinnerHash);
    }

    num_temp_textures = GetTempCarSkinTextures(info->TexturesToLoadTemp, 0, max_temp_textures, ride_info);

    bSPrintf(buffer, "%s_SKIN2", car_base_name);
    unsigned int skin2_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN3", car_base_name);
    unsigned int skin3_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_SKIN4", car_base_name);
    unsigned int skin4_namehash = bStringHash(buffer);

    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, skin2_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, skin3_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, skin4_namehash);

    CarPart *carpart_headlight = ride_info->GetPart(CARSLOTID_HEADLIGHT);
    CarPart *carpart_brakelight = ride_info->GetPart(CARSLOTID_BRAKELIGHT);

    bSPrintf(buffer, "%s_KIT00_HEADLIGHT", car_base_name);
    unsigned int base_headlight_hash = bStringHash(buffer);
    bSPrintf(buffer, "%s_KIT00_BRAKELIGHT", car_base_name);
    unsigned int base_brakelight_hash = bStringHash(buffer);

    if (carpart_headlight != nullptr && carpart_headlight->GetTextureNameHash() != 0) {
        base_headlight_hash = carpart_headlight->GetTextureNameHash();
    }

    if (carpart_brakelight != nullptr && carpart_brakelight->GetTextureNameHash() != 0) {
        base_brakelight_hash = carpart_brakelight->GetTextureNameHash();
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

    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceHeadlightHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceHeadlightHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceHeadlightGlassHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceHeadlightGlassHash[1]);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceBrakelightHash[0]);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceBrakelightHash[1]);
    num_perm_textures +=
        UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceBrakelightGlassHash[0]);
    num_perm_textures +=
        UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ReplaceBrakelightGlassHash[1]);

    bSPrintf(buffer, "%s_MISC", car_base_name);
    unsigned int misc_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_MISC_N", car_base_name);
    unsigned int misc_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_INTERIOR", car_base_name);
    unsigned int interior_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_INTERIOR_N", car_base_name);
    unsigned int interior_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING", car_base_name);
    unsigned int badging_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_BADGING_N", car_base_name);
    unsigned int badging_n_namehash = bStringHash(buffer);
#ifndef EA_BUILD_A124
    bSPrintf(buffer, "%s_DRIVER", car_base_name);
    unsigned int driver_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_ENGINE", car_base_name);
    unsigned int engine_namehash = bStringHash(buffer);
#endif
    bSPrintf(buffer, "%s_BADGING_EU", car_base_name);
    unsigned int badging_eu_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_LICENSE_PLATE", car_base_name);
    unsigned int license_plate_namehash = bStringHash(buffer);

    info->MappedLicensePlateHash = license_plate_namehash;
    info->MappedBadging = badging_namehash;

    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, misc_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, misc_n_namehash);

    if (front_end_only != 0 || strstr(car_base_name, "COP") != nullptr) {
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, interior_namehash);
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, interior_n_namehash);
    }

#ifndef EA_BUILD_A124
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, driver_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, engine_namehash);
#endif
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, badging_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, badging_n_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, badging_eu_namehash);

    bSPrintf(buffer, "%s_SIDELIGHT", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, bStringHash(buffer));
    bSPrintf(buffer, "%s_DOOR_HANDLE", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, bStringHash(buffer));
    bSPrintf(buffer, "%s_LOGO", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, bStringHash(buffer));

    if (!IsGameFlowInGame()) {
        num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, bStringHash("AUDIO_SKIN"));
    }

    bSPrintf(buffer, "%s_SHADOW%s", car_base_name, front_end_only != 0 ? "FE" : "IG");
    info->ShadowHash = bStringHash(buffer);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->ShadowHash);

    bSPrintf(buffer, "%s_NEON", car_base_name);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, bStringHash(buffer));

    bSPrintf(buffer, "%s_TIRE", car_base_name);
    info->MappedTireHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TIRE_N", car_base_name);
    unsigned int tire_n_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_RIM", car_base_name);
    info->MappedRimHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_RIM_BLUR", car_base_name);
    info->MappedRimBlurHash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TREAD", car_base_name);
    unsigned int tread_namehash = bStringHash(buffer);
    bSPrintf(buffer, "%s_TREAD_N", car_base_name);
    unsigned int tread_n_namehash = bStringHash(buffer);

    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->MappedTireHash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->MappedRimHash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, info->MappedRimBlurHash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, tire_n_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, tread_namehash);
    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, tread_n_namehash);

    unsigned int size_hash = bStringHash("SIZE");
    unsigned int shape_hash = bStringHash("SHAPE");

    unsigned int size_hashes[3] = {bStringHash("SMALL"), bStringHash("MEDIUM"), bStringHash("LARGE")};
    unsigned int shape_hashes[3] = {bStringHash("SQUARE"), bStringHash("RECT"), bStringHash("WIDE")};

    for (int i = CARSLOTID_DECAL_MODEL_FIRST; i <= CARSLOTID_MODEL_LAST; i++) {
        CarPart *decal_model_part = ride_info->GetPart(i);

        if (decal_model_part != nullptr && decal_model_part->HasAppliedAttribute(size_hash) != 0 &&
            decal_model_part->HasAppliedAttribute(shape_hash) != 0) {
            unsigned int decal_size = decal_model_part->GetAppliedAttributeUParam(size_hash, 0);
            unsigned int decal_shape = decal_model_part->GetAppliedAttributeUParam(shape_hash, 0);
            int decal_model_index = i - CARSLOTID_DECAL_MODEL_FIRST;
            int num_decal_slots = 8;
            int first_tex_part = i * num_decal_slots - 477;

            for (int j = 0; j < num_decal_slots; j++) {
                CarPart *decal_texture_part = ride_info->GetPart(j + first_tex_part);

                if (decal_texture_part != nullptr) {
                    unsigned int texture_hash = decal_texture_part->GetAppliedAttributeUParam(bStringHash("NAME"), 0);

                    if (decal_shape == shape_hashes[0]) {
                        texture_hash = bStringHash("_SQUARE", texture_hash);
                    } else if (decal_shape == shape_hashes[1]) {
                        texture_hash = bStringHash("_RECT", texture_hash);
                    } else if (decal_shape == shape_hashes[2]) {
                        texture_hash = bStringHash("_WIDE", texture_hash);
                    }

                    num_perm_textures += UsedCarTextureAddToTable(info->TexturesToLoadPerm, num_perm_textures, max_perm_textures, texture_hash);
                }
            }
        }
    }

    info->NumTexturesToLoadPerm = num_perm_textures;
    info->NumTexturesToLoadTemp = num_temp_textures;
}

int CarInfo_GetMaxCompositingBufferSize() {
    return 0;
}

// RandomCarPartUpgrade RandomPartUpgrades[12]; // TODO

int NumRandomPartUpgrades = 12;

struct CarMemoryInfo {
    char *CarMemType; // offset 0x0, size 0x4
    int Sizes[5];     // offset 0x4, size 0x14

    static int GetPlatformIndex() {
#ifdef EA_PLATFORM_GAMECUBE
        return 1;
#elif defined(EA_PLATFORM_PLAYSTATION2)
        return 0;
#elif defined(EA_PLATFORM_XENON)
        return 2; // TODO
#endif
    }
};

CarMemoryInfo CarMemoryInfoTable[6] = {
    {"Player", {0x320, 0x29e, 0x3c9, 0x18fd, 0x1ae0}}, {"Racing", {0x320, 0x276, 0x3c9, 0x18fd, 0x1ae0}},
    {"Cop", {0x316, 0x1cc, 0x297, 0x3ff, 0x4cb}},      {"Traffic", {0x78, 0x41, 0x99, 0x177, 0x1b6}},
    {"BigTraffic", {0xdc, 0x69, 0xfb, 0x1d8, 0x23a}},  {"HugeTraffic", {0x172, 0xcd, 0xfb, 0x1d8, 0x23a}},
};

unsigned int CarInfo_GetResourcePool(bool needs_compositing) {
    if (needs_compositing) {
        return TheCarLoader.GetMemoryPoolSize() - CarInfo_GetMaxCompositingBufferSize();
    }

    return TheCarLoader.GetMemoryPoolSize();
}

unsigned int CarInfo_GetResourceCost(CarType type, bool is_player, bool split_screen) {
    CarTypeInfo *car_type_info = GetCarTypeInfo(type);
    unsigned int mem_type_hash = car_type_info->CarMemTypeHash;

    if (is_player) {
        mem_type_hash = bStringHash("Player");
    }
    int n;
    for (n = 0; n < 6; n++) {
        CarMemoryInfo *mem_info = &CarMemoryInfoTable[n];

        if (bStringHash(mem_info->CarMemType) == mem_type_hash) {
            return mem_info->Sizes[CarMemoryInfo::GetPlatformIndex()] << 10; // TODO magic
        }
    }

    return 0;
}

bool CarInfo_IsSkinned(CarType type) {
    CarTypeInfo *info = GetCarTypeInfo(type);

    if (info != nullptr) {
        Attrib::Gen::ecar ecar(Attrib::StringToLowerCaseKey(info->GetCarTypeName()), 0, nullptr);

        return ecar.IsSkinned();
    }

    return false;
}
