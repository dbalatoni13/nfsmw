#include "./CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

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
extern SlotPool *LoadedSkinLayerSlotPool;
void TrackStreamer_FlushHibernatingSections(TrackStreamer *track_streamer) asm("FlushHibernatingSections__13TrackStreamer");
void TrackStreamer_MakeSpaceInPool(TrackStreamer *track_streamer, int size, bool use_callback)
    asm("MakeSpaceInPool__13TrackStreamerib");
int LoadedCar_GetModelHashes(LoadedCar *loaded_car, unsigned int *name_hashes, int max_hashes)
    asm("GetModelHashes__9LoadedCarPUii");
int LoadedSkin_GetTextureHashes(LoadedSkin *loaded_skin, unsigned int *name_hashes, int max_hashes, int load_perm_layers)
    asm("GetTextureHashes__10LoadedSkinPUiii");
int CarLoader_AllocateSkinLayers(CarLoader *car_loader, unsigned int *name_hashes, int num_hashes,
                                 LoadedSkinLayer **loaded_skin_layers, int max_layers, const char *filename)
    asm("AllocateSkinLayers__9CarLoaderPUiiPP15LoadedSkinLayeriPCc");
int CarLoader_LoadSkinLayers(CarLoader *car_loader, unsigned int *name_hashes, int max_hashes,
                             LoadedSkinLayer **loaded_skin_layers, int max_layers)
    asm("LoadSkinLayers__9CarLoaderPUiiPP15LoadedSkinLayeri");
void CarLoader_UnallocateSkinLayers(CarLoader *car_loader, LoadedSkinLayer **loaded_skin_layers, int num_layers)
    asm("UnallocateSkinLayers__9CarLoaderPP15LoadedSkinLayeri");
int CarLoader_MakeSpaceInCarMemoryPool(CarLoader *car_loader, int required_size, int max_allocations, bool stream_textures)
    asm("MakeSpaceInCarMemoryPool__9CarLoaderiib");
void CarLoader_UnloadUnallocatedRideInfos(CarLoader *car_loader, int num_ride_infos)
    asm("UnloadUnallocatedRideInfos__9CarLoaderi");
void CarLoader_ServiceLoading(CarLoader *car_loader) asm("ServiceLoading__9CarLoader");
void CarLoader_LoadedSolidPackCallback(CarLoader *car_loader, LoadedSolidPack *loaded_solid_pack)
    asm("LoadedSolidPackCallback__9CarLoaderP15LoadedSolidPack");
void CarLoader_LoadedCarCallback(CarLoader *car_loader, LoadedCar *loaded_car) asm("LoadedCarCallback__9CarLoaderP9LoadedCar");
void CarLoader_LoadedWheelModelsCallback(CarLoader *car_loader) asm("LoadedWheelModelsCallback__9CarLoader");
void CarLoader_LoadedWheelTexturesCallback(CarLoader *car_loader) asm("LoadedWheelTexturesCallback__9CarLoader");
void CarLoader_LoadedAllTexturesFromPackCallback(CarLoader *car_loader) asm("LoadedAllTexturesFromPackCallback__9CarLoader");
void LoadedCarCallbackBridge(void *param) asm("LoadedCarCallbackBridge__9CarLoaderUi");
void LoadedSolidPackCallbackBridge(void *param) asm("LoadedSolidPackCallbackBridge__9CarLoaderUi");
void LoadedWheelModelsCallbackBridge(void *param) asm("LoadedWheelModelsCallbackBridge__9CarLoaderUi");
void LoadedWheelTexturesCallbackBridge(void *param) asm("LoadedWheelTexturesCallbackBridge__9CarLoaderUi");
void LoadedAllTexturesFromPackCallbackBridge(void *param) asm("LoadedAllTexturesFromPackCallbackBridge__9CarLoaderUi");
void bCloseMemoryPool(int pool_num);
bool bSetMemoryPoolDebugFill(int pool_num, bool on_off);
void bSetMemoryPoolTopDirection(int pool_num, bool top_means_larger_address);
void eLoadStreamingSolid(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
int eLoadStreamingSolidPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num);
void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0,
                           int memory_pool_num);
void eUnloadStreamingTexture(unsigned int *name_hash_table, int num_hashes);

CarLoader::CarLoader()
    : StartLoadingTime(0.0f) {
    this->pCallback = 0;
    this->LoadingMode = MODE_FRONT_END;
    this->InFrontEndFlag = 0;
    this->TwoPlayerFlag = 0;
    this->LoadingInProgress = 0;
    this->NumLoadedRideInfos = 0;
    this->NumAllocatedRideInfos = 0;
    this->MayNeedDefragmentation = 0;
    this->MemoryPoolMem = 0;
    this->MemoryPoolSize = 0;
    this->NumSpongeAllocations = 0;
    this->NumLoadingSkinLayers = 0;
}

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

void CarLoader::SetLoadingMode(eLoadingMode mode, int two_player_flag) {
    this->TwoPlayerFlag = two_player_flag;
    this->InFrontEndFlag = mode == MODE_FRONT_END;
    this->LoadingMode = mode;
}

LoadedSolidPack *CarLoader::FindLoadedSolidPack(const char *filename) {
    for (LoadedSolidPack *loaded_solid_pack = this->LoadedSolidPackList.GetHead();
         loaded_solid_pack != this->LoadedSolidPackList.EndOfList(); loaded_solid_pack = loaded_solid_pack->GetNext()) {
        if (bStrCmp(loaded_solid_pack->Filename, filename) == 0) {
            return loaded_solid_pack;
        }
    }

    return 0;
}

LoadedTexturePack *CarLoader::FindLoadedTexturePack(const char *filename) {
    for (LoadedTexturePack *loaded_texture_pack = this->LoadedTexturePackList.GetHead();
         loaded_texture_pack != this->LoadedTexturePackList.EndOfList(); loaded_texture_pack = loaded_texture_pack->GetNext()) {
        if (bStrCmp(loaded_texture_pack->Filename, filename) == 0) {
            return loaded_texture_pack;
        }
    }

    return 0;
}

LoadedSkinLayer *CarLoader::FindLoadedSkinLayer(unsigned int name_hash) {
    for (LoadedSkinLayer *loaded_skin_layer = this->LoadedSkinLayerList.GetHead();
         loaded_skin_layer != this->LoadedSkinLayerList.EndOfList(); loaded_skin_layer = loaded_skin_layer->GetNext()) {
        if (loaded_skin_layer->NameHash == name_hash) {
            return loaded_skin_layer;
        }
    }

    return 0;
}

void CarLoader::LoadingDoneCallback() {
    this->LoadingInProgress = 0;
    CarLoader_ServiceLoading(this);
}

void CarLoader::LoadedSolidPackCallback(LoadedSolidPack *loaded_solid_pack) {
    loaded_solid_pack->LoadState = CARLOADSTATE_LOADED;
    this->LoadingDoneCallback();
}

void CarLoader::LoadedCarCallback(LoadedCar *loaded_car) {
    loaded_car->LoadState = CARLOADSTATE_LOADED;
    this->LoadingDoneCallback();
}

void CarLoader::LoadedWheelModelsCallback() {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->pLoadedWheel->LoadState == CARLOADSTATE_LOADING) {
            loaded_ride_info->pLoadedWheel->LoadState = CARLOADSTATE_LOADED;
        }
    }

    this->LoadingDoneCallback();
}

void CarLoader::LoadedWheelTexturesCallback() {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;

        if (loaded_wheel->LoadStateSkinPerm == CARLOADSTATE_LOADING) {
            loaded_wheel->LoadStateSkinPerm = CARLOADSTATE_LOADED;
            this->LoadedSkinLayers(loaded_wheel->LoadedSkinLayersPerm, 4);
        }

        if (loaded_wheel->LoadStateSkinTemp == CARLOADSTATE_LOADING) {
            loaded_wheel->LoadStateSkinTemp = CARLOADSTATE_LOADED;
            this->LoadedSkinLayers(loaded_wheel->LoadedSkinLayersTemp, 4);
        }
    }

    this->LoadingDoneCallback();
}

void CarLoader::LoadedAllTexturesFromPackCallback() {
    unsigned int name_hashes[512];

    this->LoadedSkinLayers(this->LoadingSkinLayers, this->NumLoadingSkinLayers);
    CarLoader_UnallocateSkinLayers(this, this->LoadingSkinLayers, this->NumLoadingSkinLayers);

    int unloaded_hashes = this->UnloadSkinLayers(name_hashes, 0x200, this->LoadingSkinLayers, this->NumLoadingSkinLayers);

    if (unloaded_hashes != 0) {
        eUnloadStreamingTexture(name_hashes, unloaded_hashes);
    }

    this->NumLoadingSkinLayers = 0;
    this->LoadingDoneCallback();
}

void CarLoader::LoadedSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers) {
    for (int i = 0; i < num_loaded_skin_layers; i++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[i];

        if (loaded_skin_layer != 0 && loaded_skin_layer->LoadState == CARLOADSTATE_LOADING) {
            loaded_skin_layer->LoadState = CARLOADSTATE_LOADED;
        }
    }
}

int CarLoader::UnloadSkinLayers(unsigned int *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                                int num_loaded_skin_layers) {
    int num_name_hashes = 0;

    for (int i = 0; i < num_loaded_skin_layers; i++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[i];

        if (loaded_skin_layer != 0 && loaded_skin_layer->NumInstances == 0) {
            if (loaded_skin_layer->LoadState == CARLOADSTATE_LOADED) {
                name_hash_table[num_name_hashes] = loaded_skin_layer->NameHash;
                num_name_hashes++;
            }

            loaded_skin_layer->Remove();
            bFree(LoadedSkinLayerSlotPool, loaded_skin_layer);
        }
    }

    return num_name_hashes;
}

void CarLoader::SetMemoryPoolSize(int size) {
    if (this->MemoryPoolSize != size) {
        if (this->MemoryPoolSize != 0) {
            for (int i = 0; i < this->NumSpongeAllocations; i++) {
                bFree(this->SpongeAllocations[i]);
            }

            this->NumSpongeAllocations = 0;
            CarLoader_UnloadUnallocatedRideInfos(this, 0);

            if (this->LoadedRideInfoList.GetHead() != this->LoadedRideInfoList.EndOfList()) {
                return;
            }

            bCloseMemoryPool(CarLoaderMemoryPoolNumber);
            bFree(this->MemoryPoolMem);
            this->MemoryPoolSize = 0;
            this->MemoryPoolMem = 0;
        }

        if (size != 0) {
            TrackStreamer_FlushHibernatingSections(&TheTrackStreamer);
            TrackStreamer_MakeSpaceInPool(&TheTrackStreamer, size, true);

            this->MemoryPoolMem = bMalloc(size, 7);
            this->MemoryPoolSize = size;
            CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum();

            bInitMemoryPool(CarLoaderMemoryPoolNumber, this->MemoryPoolMem, this->MemoryPoolSize, "Cars");
            bSetMemoryPoolDebugFill(CarLoaderMemoryPoolNumber, false);
            bSetMemoryPoolTopDirection(CarLoaderMemoryPoolNumber, true);
            this->NumSpongeAllocations = 0;
        }
    }
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

void CarLoader::LoadSolidPack(LoadedSolidPack *loaded_solid_pack, int stream_solids) {
    if (stream_solids == 0) {
        loaded_solid_pack->pResourceFile = CreateResourceFile(loaded_solid_pack->Filename, RESOURCE_FILE_CAR, 0, 0, 0);

        int allocation_params = 0;

        if (CarLoader_MakeSpaceInCarMemoryPool(this, bFileSize(loaded_solid_pack->Filename), 0, false) != 0) {
            allocation_params = CarLoaderMemoryPoolNumber;
        }

        loaded_solid_pack->pResourceFile->SetAllocationParams((allocation_params & 0xF) | 0x2000, loaded_solid_pack->Filename);
        loaded_solid_pack->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        loaded_solid_pack->pResourceFile->BeginLoading(LoadedSolidPackCallbackBridge, loaded_solid_pack);
    } else {
        CarLoader_MakeSpaceInCarMemoryPool(this, 0x8000, 0, true);
        loaded_solid_pack->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        eLoadStreamingSolidPack(loaded_solid_pack->Filename, LoadedSolidPackCallbackBridge, loaded_solid_pack,
                                CarLoaderMemoryPoolNumber);
        loaded_solid_pack->pStreamingPack = StreamingSolidPackLoader.GetLoadedStreamingPack(loaded_solid_pack->Filename);

        if (loaded_solid_pack->pStreamingPack == 0) {
            LoadedSolidPackCallbackBridge(loaded_solid_pack);
        }
    }
}

int CarLoader::LoadAllTexturesFromPack(const char *filename, int load_perm_layers) {
    unsigned int allocated_name_hashes[128];
    unsigned int name_hashes[512];

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances > 0 && loaded_ride_info->LoadState == CARLOADSTATE_QUEUED) {
            int num_hashes = LoadedSkin_GetTextureHashes(loaded_ride_info->pLoadedSkin, allocated_name_hashes, 0x80, load_perm_layers);
            int allocated_skin_layers = CarLoader_AllocateSkinLayers(this, allocated_name_hashes, num_hashes,
                                                                     &this->LoadingSkinLayers[this->NumLoadingSkinLayers],
                                                                     0x200 - this->NumLoadingSkinLayers, filename);

            this->NumLoadingSkinLayers += allocated_skin_layers;
        }
    }

    int loaded_hashes = CarLoader_LoadSkinLayers(this, name_hashes, 0x200, this->LoadingSkinLayers, this->NumLoadingSkinLayers);
    int memory_pool_num = CarLoaderMemoryPoolNumber;

    if (loaded_hashes == 0) {
        CarLoader_UnallocateSkinLayers(this, this->LoadingSkinLayers, this->NumLoadingSkinLayers);
        this->NumLoadingSkinLayers = 0;
        memory_pool_num = 0;
    } else {
        if (load_perm_layers != 0 || this->LoadingMode == MODE_IN_GAME) {
            do {
                memory_pool_num = CarLoaderMemoryPoolNumber;

                if (StreamingTexturePackLoader.TestLoadStreamingEntry(name_hashes, loaded_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                    break;
                }
            } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);
        } else {
            memory_pool_num = 0;
        }

        this->LoadingInProgress = 1;
        eLoadStreamingTexture(name_hashes, loaded_hashes, LoadedAllTexturesFromPackCallbackBridge, 0, memory_pool_num);
        return 1;
    }

    return memory_pool_num;
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
