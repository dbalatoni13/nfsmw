#include "./CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

struct RideInfoLayout {
    CarType Type;
    char InstanceIndex;
    char HasDash;
    char CanBeVertexDamaged;
    char SkinType;
    CARPART_LOD mMinLodLevel;
    CARPART_LOD mMaxLodLevel;
    CARPART_LOD mMinFELodLevel;
    CARPART_LOD mMaxFELodLevel;
};
struct CarPartAttributeTable {
    short NumAttributes;
    short AttributeOffsetTable[1];

    unsigned int GetByteSize() {
        return static_cast<unsigned int>((this->NumAttributes + 1) * sizeof(short));
    }
};
struct CarPartAttribute {
    unsigned int NameHash;
    union {
        float fParam;
        int iParam;
        unsigned int uParam;
    } Params;
};
struct CarPartModelTable {
    char TemplatedNameHashes;
    char pad;
    unsigned short MiddleStringOffset;
    const char *ModelNames[5][1];
};
struct CarPartPack : public bTNode<CarPartPack> {
    unsigned int Version;
    const char *StringTable;
    unsigned int StringTableSize;
    CarPartAttributeTable *AttributeTableTable;
    unsigned int NumAttributeTables;
    CarPartAttribute *AttributesTable;
    unsigned int NumAttributes;
    unsigned int *TypeNameTable;
    unsigned int NumTypeNames;
    CarPartModelTable *ModelTable;
    unsigned int NumModelTables;
    CarPart *PartsTable;
    unsigned int NumParts;
};
struct CarPartIndex {
    CarPart *Part;
    int NumParts;
};
struct CarSlotTypeOverride {
    unsigned int CarType;
    unsigned int SlotId;
    unsigned int LookupType[2];
};
struct CarPartDatabaseLayout {
    bTList<CarPartPack> CarPartPackList;
    int NumPacks;
    int NumParts;
    int NumBytes;
    CarPartIndex PaintPart_Gloss[3];
    CarPartIndex PaintPart_Metallic[3];
    CarPartIndex PaintPart_Pearl[3];
    CarPartIndex PaintPart_Vinyl[3];
    CarPartIndex PaintPart_Rims[3];
    CarPartIndex PaintPart_Caliper[3];
    CarPartIndex VinylPart_All[3];
    CarPartIndex VinylPart_Body[3];
    CarPartIndex VinylPart_Hood[3];
    CarPartIndex VinylPart_Side[3];
    CarPartIndex VinylPart_Manufacturer[3];
};

extern CarPartDatabase CarPartDB;
extern CarTypeInfo *CarTypeInfoArray;
signed char *CarSlotAnimHookupTable;
unsigned int *CarSlotAnimHideOpenTable;
unsigned int *CarSlotAnimHideClosedTable;
unsigned int *DefaultSlotTypeNameTable;
CarSlotTypeOverride *SlotTypeOverrideTable;
int NumSlotTypeOverrides;
const char *CarPartStringTable;
unsigned int CarPartStringTableSize;
unsigned int *CarPartTypeNameHashTable;
unsigned int CarPartTypeNameHashTableSize;
CarPart *CarPartPartsTable;
CarPartModelTable *CarPartModelsTable;
CarPartPack *MasterCarPartPack;

int ConvertVinylGroupNumberToVinylType(int vinyl_group_number);
int CarInfo_GetMaxCompositingBufferSize();
extern int CarLoaderMemoryPoolNumber;
int CompositeSkin(RideInfo *ride_info);
int LoadedCar_GetModelHashes(LoadedCar *loaded_car, unsigned int *name_hashes, int max_hashes)
    asm("GetModelHashes__9LoadedCarPUii");
int CarLoader_LoadSkinLayers(CarLoader *car_loader, unsigned int *name_hashes, int max_hashes,
                             LoadedSkinLayer **loaded_skin_layers, int max_layers)
    asm("LoadSkinLayers__9CarLoaderPUiiPP15LoadedSkinLayeri");
void LoadedCarCallbackBridge(void *param) asm("LoadedCarCallbackBridge__9CarLoaderUi");
void LoadedWheelModelsCallbackBridge(void *param) asm("LoadedWheelModelsCallbackBridge__9CarLoaderUi");
void LoadedWheelTexturesCallbackBridge(void *param) asm("LoadedWheelTexturesCallbackBridge__9CarLoaderUi");
void eLoadStreamingSolid(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0,
                           int memory_pool_num);

LoadedWheel::LoadedWheel(RideInfo *ride_info, bool in_fe) {
    RideInfoLayout *ride_layout = reinterpret_cast<RideInfoLayout *>(ride_info);

    this->pCarPart = 0;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->LoadStateSkinPerm = CARLOADSTATE_QUEUED;
    this->LoadStateSkinTemp = CARLOADSTATE_QUEUED;
    this->PartNameHash = 0;
    this->TextureBaseNameHash = 0;

    if (!in_fe) {
        this->mMinLodLevel = ride_layout->mMinLodLevel;
        this->mMaxLodLevel = ride_layout->mMaxLodLevel;
    } else {
        this->mMinLodLevel = ride_layout->mMinFELodLevel;
        this->mMaxLodLevel = ride_layout->mMaxFELodLevel;
    }

    bMemSet(this->ModelNameHashes, 0, sizeof(this->ModelNameHashes));
    bMemSet(this->SkinNameHashesPerm, 0, sizeof(this->SkinNameHashesPerm));
    bMemSet(this->SkinNameHashesTemp, 0, sizeof(this->SkinNameHashesTemp));
    bMemSet(this->LoadedSkinLayersPerm, 0, sizeof(this->LoadedSkinLayersPerm));
    bMemSet(this->LoadedSkinLayersTemp, 0, sizeof(this->LoadedSkinLayersTemp));

    CarPart *car_part = ride_info->GetPart(0x42);
    if (car_part != 0) {
        this->pCarPart = car_part;
        this->PartNameHash = car_part->GetPartNameHash();
        this->TextureBaseNameHash = car_part->GetAppliedAttributeUParam(0x10C98090, 0);

        if (car_part->GetCarTypeNameHash() == bStringHash("WHEELS")) {
            for (int model = 0; model < 1; model++) {
                for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                    reinterpret_cast<unsigned int *>(this->ModelNameHashes)[lod + model * 5] = car_part->GetModelNameHash(model, lod);
                }
            }

            if (this->TextureBaseNameHash != 0) {
                this->SkinNameHashesPerm[0] = bStringHash("_WHEEL", this->TextureBaseNameHash);
            }
            return;
        }
    }

    this->LoadStateSkinTemp = CARLOADSTATE_LOADED;
    this->LoadState = CARLOADSTATE_LOADED;
    this->LoadStateSkinPerm = CARLOADSTATE_LOADED;
}

LoadedSkin::LoadedSkin(RideInfo *ride_info, int in_front_end, int is_player_skin) {
    this->pRideInfo = ride_info;
    this->InFrontEnd = in_front_end;
    this->pLoadedTexturesPack = 0;
    this->pLoadedVinylsPack = 0;
    this->NumLoadedSkinLayersPerm = 0;
    this->IsPlayerSkin = is_player_skin;
    this->DoneComposite = 0;
    this->LoadStatePerm = 0;
    this->LoadStateTemp = 0;
    bMemSet(this->LoadedSkinLayersPerm, 0, sizeof(this->LoadedSkinLayersPerm));
    this->NumLoadedSkinLayersTemp = 0;
    bMemSet(this->LoadedSkinLayersTemp, 0, sizeof(this->LoadedSkinLayersTemp));
}

LoadedCar::LoadedCar(RideInfo *ride_info, int in_front_end, int is_two_player) {
    this->pRideInfo = ride_info;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->Type = ride_info->Type;
    this->InFrontEnd = in_front_end;
    this->IsTwoPlayer = is_two_player;
    this->pLoadedSolidPack = 0;
}

int LoadedRideInfo::sNextID;

LoadedRideInfo::LoadedRideInfo(RideInfo *ride_info, int in_front_end, int is_two_player, int is_player_car)
    : TheRideInfo(*ride_info),            //
      TheLoadedCar(&TheRideInfo, in_front_end, is_two_player), //
      TheLoadedWheel(&TheRideInfo, in_front_end != 0),         //
      TheLoadedSkin(&TheRideInfo, in_front_end, is_player_car) {
    this->HighPriority = 0;
    this->NumInstances = 0;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->PrintedLoading = 0;
    this->ID = sNextID;
    sNextID++;
    this->pCarTypeInfo = &CarTypeInfoArray[ride_info->Type];
    this->pLoadedCar = &this->TheLoadedCar;
    this->pLoadedWheel = &this->TheLoadedWheel;
    this->pLoadedSkin = &this->TheLoadedSkin;
    bSPrintf(this->Name, "%s(%d)", this->pCarTypeInfo->CarTypeName, this->ID);
}

static int ClampUpgradeLevel(int level) {
    if (level < 0) {
        return 0;
    }

    if (level > 2) {
        return 2;
    }

    return level;
}

int LoaderCarInfo(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();

    if (chunk_id == 0x34600) {
        CarTypeInfoArray = reinterpret_cast<CarTypeInfo *>(chunk->GetAlignedData(16));

        for (int j = 0; j < 0x54; j++) {
            CarTypeInfo *pCarInfo = &CarTypeInfoArray[j];

            bEndianSwap32(&pCarInfo->CarTypeNameHash);
            bEndianSwap32(&pCarInfo->HeadlightFOV);
            bPlatEndianSwap(&pCarInfo->HeadlightPosition);
            bPlatEndianSwap(&pCarInfo->DriverRenderingOffset);
            bPlatEndianSwap(&pCarInfo->InCarSteeringWheelRenderingOffset);
            bEndianSwap32(&pCarInfo->DefaultBasePaint);
            bEndianSwap32(&pCarInfo->Type);
            bEndianSwap32(&pCarInfo->UsageType);
            bEndianSwap32(&pCarInfo->CarMemTypeHash);

            for (int i = 0; i < 5; i++) {
                bEndianSwap32(&pCarInfo->MinTimeBetweenUses[i]);
            }
        }
    } else if (chunk_id == 0x34608) {
        CarSlotAnimHookupTable = reinterpret_cast<signed char *>(chunk->GetData());
    } else if (chunk_id == 0x34609) {
        CarSlotAnimHideOpenTable = reinterpret_cast<unsigned int *>(chunk->GetData());
        CarSlotAnimHideClosedTable = reinterpret_cast<unsigned int *>(chunk->GetData()) + 0x20;
        return 1;
    } else if (chunk_id == 0x34607) {
        DefaultSlotTypeNameTable = reinterpret_cast<unsigned int *>(chunk->GetData());
        SlotTypeOverrideTable = reinterpret_cast<CarSlotTypeOverride *>(reinterpret_cast<unsigned int *>(chunk->GetData()) + 0x116);
        NumSlotTypeOverrides = (chunk->GetSize() - 0x458) >> 4;

        for (int i = 0; i < 0x116; i++) {
            bEndianSwap32(&DefaultSlotTypeNameTable[i]);
        }

        for (int i = 0; i < NumSlotTypeOverrides; i++) {
            bEndianSwap32(&SlotTypeOverrideTable[i].CarType);
            bEndianSwap32(&SlotTypeOverrideTable[i].SlotId);
            bEndianSwap32(&SlotTypeOverrideTable[i].LookupType[0]);
            bEndianSwap32(&SlotTypeOverrideTable[i].LookupType[1]);
        }
    } else {
        if (chunk_id != 0x34602) {
            return 0;
        }

        int *chunk_words = reinterpret_cast<int *>(chunk);
        int string_table_offset = chunk_words[3];
        CarPartPack *car_part_pack = reinterpret_cast<CarPartPack *>(chunk_words + 4);
        int attribute_table_table_offset =
            *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + string_table_offset + 0x14) + string_table_offset + 0x10;
        int attributes_table_offset =
            *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + attribute_table_table_offset + 0xC) + attribute_table_table_offset + 8;
        int model_table_offset =
            *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + attributes_table_offset + 0xC) + attributes_table_offset + 8;
        int typename_table_offset =
            *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + model_table_offset + 0xC) + model_table_offset + 8;
        int parts_table_offset = *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + typename_table_offset + 0xC);

        bEndianSwap32(&car_part_pack->Version);
        bEndianSwap32(&car_part_pack->NumParts);
        bEndianSwap32(&car_part_pack->NumAttributes);
        bEndianSwap32(&car_part_pack->NumTypeNames);
        bEndianSwap32(&car_part_pack->NumModelTables);
        bEndianSwap32(&car_part_pack->NumAttributeTables);

        car_part_pack->AttributeTableTable =
            reinterpret_cast<CarPartAttributeTable *>(reinterpret_cast<char *>(chunk_words) + attribute_table_table_offset + 0x10);
        CarPartPartsTable = reinterpret_cast<CarPart *>(reinterpret_cast<char *>(chunk_words) + typename_table_offset + parts_table_offset + 0x18);
        CarPartTypeNameHashTable = reinterpret_cast<unsigned int *>(reinterpret_cast<char *>(chunk_words) + typename_table_offset + 0x10);
        CarPartModelsTable = reinterpret_cast<CarPartModelTable *>(reinterpret_cast<char *>(chunk_words) + model_table_offset + 0x10);
        CarPartStringTable = reinterpret_cast<const char *>(reinterpret_cast<char *>(chunk_words) + string_table_offset + 0x18);
        car_part_pack->AttributesTable = reinterpret_cast<CarPartAttribute *>(reinterpret_cast<char *>(chunk_words) + attributes_table_offset + 0x10);
        car_part_pack->Next = car_part_pack;
        car_part_pack->Prev = car_part_pack;
        car_part_pack->StringTable = CarPartStringTable;
        car_part_pack->PartsTable = CarPartPartsTable;
        car_part_pack->TypeNameTable = CarPartTypeNameHashTable;
        car_part_pack->ModelTable = CarPartModelsTable;
        CarPartTypeNameHashTableSize = car_part_pack->NumTypeNames;
        car_part_pack->StringTableSize = *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + string_table_offset + 0x14);
        CarPartStringTableSize = *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + string_table_offset + 0x14);

        short *attribute_offset_table = reinterpret_cast<short *>(reinterpret_cast<char *>(chunk_words) + attribute_table_table_offset + 0x10);
        short *attribute_offset_table_end = reinterpret_cast<short *>(
            reinterpret_cast<char *>(attribute_offset_table) +
            *reinterpret_cast<int *>(reinterpret_cast<char *>(chunk_words) + attribute_table_table_offset + 0xC));

        while (attribute_offset_table < attribute_offset_table_end) {
            bEndianSwap16(attribute_offset_table);
            for (int i = 0; i < *attribute_offset_table; i++) {
                bEndianSwap16(attribute_offset_table + i + 1);
            }
            attribute_offset_table += *attribute_offset_table + 1;
        }

        for (unsigned int i = 0; i < car_part_pack->NumAttributes; i++) {
            bEndianSwap32(&car_part_pack->AttributesTable[i].NameHash);
            bEndianSwap32(&car_part_pack->AttributesTable[i].Params.iParam);
        }

        for (unsigned int i = 0; i < car_part_pack->NumTypeNames; i++) {
            bEndianSwap32(&CarPartTypeNameHashTable[i]);
        }

        for (unsigned int model_table_index = 0; model_table_index < car_part_pack->NumModelTables; model_table_index++) {
            CarPartModelTable *model_table = reinterpret_cast<CarPartModelTable *>(reinterpret_cast<char *>(CarPartModelsTable) + model_table_index * 0x18);

            bEndianSwap16(&model_table->MiddleStringOffset);
            for (int model_number = 0; model_number < 1; model_number++) {
                for (int model_lod = 0; model_lod < 5; model_lod++) {
                    bEndianSwap32(const_cast<const char **>(&model_table->ModelNames[model_number][model_lod]));
                }
            }

            if (model_table->TemplatedNameHashes != 0) {
                for (int model_number = 0; model_number < 1; model_number++) {
                    for (int model_lod = 0; model_lod < 5; model_lod++) {
                        if (reinterpret_cast<int>(model_table->ModelNames[model_number][model_lod]) != -1) {
                            model_table->ModelNames[model_number][model_lod] = reinterpret_cast<const char *>(
                                const_cast<char *>(CarPartStringTable) + reinterpret_cast<int>(model_table->ModelNames[model_number][model_lod]) * 4);
                        }
                    }
                }
            }
        }

        CarPartDatabaseLayout *database = reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB);
        for (unsigned int i = 0; i < car_part_pack->NumParts; i++) {
            char *car_part_bytes = reinterpret_cast<char *>(CarPartPartsTable) + i * 0xE;
            CarPart *car_part = reinterpret_cast<CarPart *>(car_part_bytes);
            CarPartIndex *index0 = 0;
            CarPartIndex *index1 = 0;

            bEndianSwap16(car_part_bytes);
            bEndianSwap16(car_part_bytes + 2);
            bEndianSwap16(car_part_bytes + 8);
            bEndianSwap16(car_part_bytes + 10);
            bEndianSwap16(car_part_bytes + 12);

            int part_id = car_part_bytes[4];
            unsigned int brand_name = car_part->GetAppliedAttributeUParam(0xEBB03E66, 0);
            int upgrade_level = ClampUpgradeLevel((static_cast<unsigned char>(car_part_bytes[5]) >> 5) - 1);
            int group_number = static_cast<unsigned char>(car_part_bytes[5]) & 0x1F;

            if (part_id == 'L') {
                if (brand_name == 0x03437A52) {
                    index0 = &database->PaintPart_Metallic[upgrade_level];
                } else if (brand_name < 0x03437A53) {
                    if (brand_name == 0x0000DA27) {
                        index0 = &database->PaintPart_Rims[upgrade_level];
                    } else if (brand_name == 0x02DAAB07) {
                        index0 = &database->PaintPart_Gloss[upgrade_level];
                    }
                } else if (brand_name == 0x03E871F1) {
                    index0 = &database->PaintPart_Vinyl[upgrade_level];
                } else if (brand_name < 0x03E871F2) {
                    if (brand_name == 0x03797533) {
                        index0 = &database->PaintPart_Pearl[upgrade_level];
                    }
                } else if (brand_name == 0xD6640DFF) {
                    index0 = &database->PaintPart_Caliper[upgrade_level];
                }
            } else if (part_id == 'O') {
                int vinyl_type = ConvertVinylGroupNumberToVinylType(group_number);

                if (vinyl_type == 1) {
                    index0 = &database->VinylPart_Hood[upgrade_level];
                } else if (vinyl_type < 2) {
                    if (vinyl_type == 0) {
                        index0 = &database->VinylPart_Side[upgrade_level];
                    }
                } else if (vinyl_type == 2) {
                    index0 = &database->VinylPart_Body[upgrade_level];
                } else if (vinyl_type == 3) {
                    index0 = &database->VinylPart_Manufacturer[upgrade_level];
                }

                index1 = &database->VinylPart_All[upgrade_level];
            }

            if (index0 != 0) {
                if (index0->Part == 0) {
                    index0->Part = car_part;
                }
                index0->NumParts++;
            }

            if (index1 != 0) {
                if (index1->Part == 0) {
                    index1->Part = car_part;
                }
                index1->NumParts++;
            }
        }

        MasterCarPartPack = car_part_pack;
        database->CarPartPackList.AddTail(car_part_pack);
        database->NumPacks += 1;
        database->NumParts += car_part_pack->NumParts;
        database->NumBytes += chunk->GetSize();
    }

    return 1;
}

int CarLoader::LoadCar(LoadedCar *loaded_car) {
    if (CarTypeInfoArray[loaded_car->Type].UsageType == 2) {
        loaded_car->LoadState = CARLOADSTATE_LOADED;
        return 0;
    }

    {
        unsigned int name_hashes[800];
        int num_hashes = LoadedCar_GetModelHashes(loaded_car, name_hashes, 800);

        do {
            if (StreamingSolidPackLoader.TestLoadStreamingEntry(name_hashes, num_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                break;
            }
        } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);

        loaded_car->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        eLoadStreamingSolid(name_hashes, num_hashes, LoadedCarCallbackBridge, loaded_car, CarLoaderMemoryPoolNumber);
    }

    return 1;
}

int CarLoader::LoadAllWheelModels() {
    unsigned int name_hashes[128];
    int num_hashes = 0;

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances != 0 && loaded_ride_info->pLoadedWheel != 0 &&
            loaded_ride_info->pLoadedWheel->LoadState == CARLOADSTATE_QUEUED) {
            LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;

            loaded_wheel->LoadState = CARLOADSTATE_LOADING;
            loaded_ride_info->LoadState = CARLOADSTATE_LOADING;

            for (int model = 0; model < 1; model++) {
                for (int lod = loaded_wheel->mMinLodLevel; lod <= loaded_wheel->mMaxLodLevel; lod++) {
                    unsigned int model_name_hash = loaded_wheel->ModelNameHashes[lod][model];

                    if (model_name_hash != 0) {
                        name_hashes[num_hashes] = model_name_hash;
                        num_hashes++;
                    }
                }
            }
        }
    }

    if (num_hashes < 1) {
        return 0;
    }

    do {
        if (StreamingSolidPackLoader.TestLoadStreamingEntry(name_hashes, num_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
            break;
        }
    } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);

    this->LoadingInProgress = 1;
    eLoadStreamingSolid(name_hashes, num_hashes, LoadedWheelModelsCallbackBridge, 0, CarLoaderMemoryPoolNumber);
    return 1;
}

int CarLoader::LoadAllWheelTextures() {
    unsigned int name_hashes[128];
    int num_hashes = 0;

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances != 0) {
            LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;

            if (loaded_wheel->LoadStateSkinPerm == CARLOADSTATE_QUEUED) {
                int loaded_hashes = CarLoader_LoadSkinLayers(this, &name_hashes[num_hashes], 0x80 - num_hashes,
                                                             loaded_wheel->LoadedSkinLayersPerm, 4);

                if (loaded_hashes == 0) {
                    loaded_wheel->LoadStateSkinPerm = CARLOADSTATE_LOADED;
                } else {
                    loaded_wheel->LoadStateSkinPerm = CARLOADSTATE_LOADING;
                }
                loaded_ride_info->LoadState = CARLOADSTATE_LOADING;
                num_hashes += loaded_hashes;
            }

            if (loaded_wheel->LoadStateSkinTemp == CARLOADSTATE_QUEUED) {
                loaded_wheel->LoadStateSkinTemp = CARLOADSTATE_LOADED;
            }
        }
    }

    if (num_hashes < 1) {
        return 0;
    }

    do {
        if (StreamingTexturePackLoader.TestLoadStreamingEntry(name_hashes, num_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
            break;
        }
    } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);

    this->LoadingInProgress = 1;
    eLoadStreamingTexture(name_hashes, num_hashes, LoadedWheelTexturesCallbackBridge, 0, CarLoaderMemoryPoolNumber);
    return 1;
}

void CarLoader::CompositeSkin(LoadedSkin *loaded_skin) {
    if (loaded_skin->pRideInfo->IsUsingCompositeSkin() != 0) {
        if (this->LoadingMode == MODE_IN_GAME) {
            int required_size = CarInfo_GetMaxCompositingBufferSize();

            if (bCountFreeMemory(CarLoaderMemoryPoolNumber) < required_size) {
                do {
                    if (required_size <= bCountFreeMemory(CarLoaderMemoryPoolNumber)) {
                        break;
                    }
                } while (this->RemoveSomethingFromCarMemoryPool(false) != 0);

                this->DefragmentPool();
            }
        }

        ::CompositeSkin(loaded_skin->pRideInfo);
    }

    loaded_skin->DoneComposite = 1;
}
