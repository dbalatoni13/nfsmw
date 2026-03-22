#include "./CarLoader.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
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
struct UsedCarTextureInfoMirror {
    unsigned int TexturesToLoadPerm[87];
    unsigned int TexturesToLoadTemp[87];
    int NumTexturesToLoadPerm;
    int NumTexturesToLoadTemp;
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
void GetUsedCarTextureInfo(UsedCarTextureInfo *used_texture_info, RideInfo *ride_info, int front_end_only);
extern int CarLoaderMemoryPoolNumber;
int CompositeSkin(RideInfo *ride_info);
extern SlotPool *LoadedTexturePackSlotPool;
extern SlotPool *LoadedSolidPackSlotPool;
extern SlotPool *LoadedSkinLayerSlotPool;
extern SlotPool *LoadedRideInfoSlotPool;
extern int UsePrecompositeVinyls;
void TrackStreamer_FlushHibernatingSections(TrackStreamer *track_streamer) asm("FlushHibernatingSections__13TrackStreamer");
void TrackStreamer_MakeSpaceInPool(TrackStreamer *track_streamer, int size, bool use_callback)
    asm("MakeSpaceInPool__13TrackStreamerib");
LoadedTexturePack *LoadedTexturePack_Construct(LoadedTexturePack *loaded_texture_pack, const char *filename, int max_header_size)
    asm("__17LoadedTexturePackPCci");
void LoadedTexturePack_Destruct(LoadedTexturePack *loaded_texture_pack, int in_chrg) asm("_._17LoadedTexturePack");
LoadedSolidPack *LoadedSolidPack_Construct(LoadedSolidPack *loaded_solid_pack, const char *filename) asm("__15LoadedSolidPackPCc");
void LoadedSolidPack_Destruct(LoadedSolidPack *loaded_solid_pack, int in_chrg) asm("_._15LoadedSolidPack");
LoadedSkinLayer *LoadedSkinLayer_Construct(LoadedSkinLayer *loaded_skin_layer, unsigned int name_hash) asm("__15LoadedSkinLayerUi");
LoadedRideInfo *LoadedRideInfo_Construct(LoadedRideInfo *loaded_ride_info, RideInfo *ride_info, int in_front_end, int is_two_player,
                                         int is_player_car)
    asm("__14LoadedRideInfoP8RideInfoiii");
int GatherModelHashes(RideInfo *ride_info, unsigned int *model_hashes, int num_hashes, int max_model_hashes, int first, int last)
    asm("GatherModelHashes__FP8RideInfoPUiiiii");
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
void LoadedSkinCallbackBridge(void *param) asm("LoadedSkinCallbackBridge__9CarLoaderUi");
void LoadedTexturePackCallbackBridge(unsigned int param) asm("LoadedTexturePackCallbackBridge__9CarLoaderUi");
void bCloseMemoryPool(int pool_num);
bool bSetMemoryPoolDebugFill(int pool_num, bool on_off);
void bSetMemoryPoolTopDirection(int pool_num, bool top_means_larger_address);
void eLoadStreamingSolid(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0, int memory_pool_num);
int eLoadStreamingSolidPack(const char *filename, void (*callback_function)(void *), void *callback_param, int memory_pool_num);
void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(void *), void *param0,
                           int memory_pool_num);
int eLoadStreamingTexturePack(const char *filename, void (*callback_func)(unsigned int), unsigned int callback_param, int memory_pool_num);
void eUnloadStreamingTexture(unsigned int *name_hash_table, int num_hashes);
void eUnloadStreamingSolid(unsigned int *name_hash_table, int num_hashes);
int eUnloadStreamingSolidPack(const char *filename);
void eUnloadStreamingTexturePack(const char *filename);

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

int LoadedSkin::GetTextureHashes(unsigned int *texture_hashes, int max_texture_hashes, int perm) {
    UsedCarTextureInfoMirror used_texture_info;

    bMemSet(texture_hashes, 0, max_texture_hashes << 2);
    GetUsedCarTextureInfo(reinterpret_cast<UsedCarTextureInfo *>(&used_texture_info), this->pRideInfo, this->InFrontEnd);

    int num_hashes = used_texture_info.NumTexturesToLoadTemp;
    unsigned int *hashes = used_texture_info.TexturesToLoadTemp;

    if (perm != 0) {
        hashes = used_texture_info.TexturesToLoadPerm;
        num_hashes = used_texture_info.NumTexturesToLoadPerm;
    }

    bMemCpy(texture_hashes, hashes, num_hashes << 2);
    return num_hashes;
}

int LoadedCar::GetModelHashes(unsigned int *model_hashes, int max_model_hashes) {
    RideInfo *ride_info = this->pRideInfo;
    RideInfoLayout *ride_layout = reinterpret_cast<RideInfoLayout *>(ride_info);

    bMemSet(model_hashes, 0, max_model_hashes << 2);

    CARPART_LOD minimum_lod = ride_layout->mMinLodLevel;
    CARPART_LOD maximum_lod = ride_layout->mMaxLodLevel;

    if (this->InFrontEnd != 0) {
        minimum_lod = ride_layout->mMinFELodLevel;
        maximum_lod = ride_layout->mMaxFELodLevel;
    }

    int num_hashes = 0;

    for (int slot_id = 0; slot_id < 0x4c; slot_id++) {
        CarPart *car_part = ride_info->GetPart(slot_id);
        CARPART_LOD current_slot_minimum = minimum_lod;
        CARPART_LOD current_slot_maximum = maximum_lod;

        ride_info->GetSpecialLODRangeForCarSlot(slot_id, &current_slot_minimum, &current_slot_maximum, this->InFrontEnd != 0);

        for (int model = 0; model < 1; model++) {
            for (int lod = current_slot_minimum; lod <= current_slot_maximum; lod++) {
                if (car_part != 0) {
                    unsigned int model_name_hash = car_part->GetModelNameHash(model, lod);

                    if (model_name_hash != 0) {
                        if (num_hashes < max_model_hashes) {
                            model_hashes[num_hashes] = model_name_hash;
                        }
                        num_hashes++;
                    }
                }
            }
        }
    }

    num_hashes = GatherModelHashes(ride_info, model_hashes, num_hashes, max_model_hashes, 0x2e, 0x33);
    num_hashes = GatherModelHashes(ride_info, model_hashes, num_hashes, max_model_hashes, 1, 0x17);

    if (max_model_hashes < num_hashes) {
        num_hashes = max_model_hashes;
    }

    unsigned char bitfield[512];

    bMemSet(bitfield, 0, 0x200);

    int num_unique_hashes = 0;

    for (int i = 0; i < num_hashes; i++) {
        unsigned int hash = model_hashes[i];
        int bit = (hash >> 3) & 0x1ff;
        bool duplicate = false;

        if (((bitfield[bit] >> (hash & 7)) & 1U) == 0) {
            bitfield[bit] |= 1 << (hash & 7);
        } else {
            int n = 0;

            if (num_unique_hashes > 0) {
                while (n < num_unique_hashes && model_hashes[n] != hash) {
                    n++;
                }
            }

            if (n != num_unique_hashes) {
                duplicate = true;
            }
        }

        if (!duplicate) {
            model_hashes[num_unique_hashes] = hash;
            num_unique_hashes++;
        }
    }

    return num_unique_hashes;
}

int CarLoader::Load(RideInfo *ride_info) {
    char filename[128];
    bool is_player_car = ride_info->SkinType == 0;

    bSPrintf(filename, "CARS\\%s\\TEXTURES.BIN", CarTypeInfoArray[ride_info->Type].CarTypeName);

    if (!bFileExists(filename)) {
        bBreak();
    }

    LoadedRideInfo *loaded_ride_info = this->AllocateRideInfo(ride_info, is_player_car);

    loaded_ride_info->IsPlayerCar = is_player_car;
    this->LoadedRideInfoList.AddTail(loaded_ride_info);
    return loaded_ride_info->ID;
}

LoadedRideInfo *CarLoader::AllocateRideInfo(RideInfo *ride_info, int is_player_car) {
    LoadedRideInfo *loaded_ride_info = this->FindLoadedRideInfo(ride_info);

    if (loaded_ride_info == 0) {
        while (bIsSlotPoolFull(LoadedRideInfoSlotPool)) {
            while (this->LoadingInProgress != 0) {
                ServiceResourceLoading();
            }

            this->RemoveSomethingFromCarMemoryPool(true);
        }

        loaded_ride_info = LoadedRideInfo_Construct(static_cast<LoadedRideInfo *>(bOMalloc(LoadedRideInfoSlotPool)), ride_info,
                                                    this->InFrontEndFlag, this->TwoPlayerFlag, is_player_car);
        this->NumLoadedRideInfos++;
        loaded_ride_info->pLoadedCar->pLoadedSolidPack = this->AllocateSolidPack(loaded_ride_info->pCarTypeInfo->GeometryFilename);
        this->AllocateSkinLayers(loaded_ride_info->pLoadedWheel->SkinNameHashesPerm, 4,
                                 loaded_ride_info->pLoadedWheel->LoadedSkinLayersPerm, 4, 0);

        char texture_filename[72];

        bSPrintf(texture_filename, "CARS\\%s\\TEXTURES.BIN", loaded_ride_info->pCarTypeInfo->CarTypeName);
        loaded_ride_info->pLoadedSkin->pLoadedTexturesPack = this->AllocateTexturePack(texture_filename, 0x8000);

        if (ride_info->SkinType != 0) {
            char vinyl_filename[72];

            if (UsePrecompositeVinyls == 0 && ride_info->SkinType != 2) {
                bSPrintf(vinyl_filename, "CARS\\%s\\VINYLS.BIN", loaded_ride_info->pCarTypeInfo->CarTypeName);
            } else {
                bSPrintf(vinyl_filename, "CARS\\%s\\PREVINYL.BIN", loaded_ride_info->pCarTypeInfo->CarTypeName);
            }

            if (bFileExists(vinyl_filename)) {
                loaded_ride_info->pLoadedSkin->pLoadedVinylsPack = this->AllocateTexturePack(vinyl_filename, 0x19000);
            }
        }

        unsigned int texture_hashes[129];
        int num_perm_hashes = loaded_ride_info->pLoadedSkin->GetTextureHashes(texture_hashes, 0x80, true);

        loaded_ride_info->pLoadedSkin->NumLoadedSkinLayersPerm = num_perm_hashes;
        this->AllocateSkinLayers(texture_hashes, num_perm_hashes, loaded_ride_info->pLoadedSkin->LoadedSkinLayersPerm,
                                 num_perm_hashes, 0);

        int num_temp_hashes = loaded_ride_info->pLoadedSkin->GetTextureHashes(texture_hashes, 0x80, false);

        loaded_ride_info->pLoadedSkin->NumLoadedSkinLayersTemp = num_temp_hashes;
        this->AllocateSkinLayers(texture_hashes, num_temp_hashes, loaded_ride_info->pLoadedSkin->LoadedSkinLayersTemp,
                                 num_temp_hashes, 0);
    }

    if (loaded_ride_info->NumInstances == 0) {
        this->NumAllocatedRideInfos++;
    }

    loaded_ride_info->NumInstances++;
    return loaded_ride_info;
}

void CarLoader::UnallocateSolidPack(LoadedSolidPack *loaded_solid_pack) {
    loaded_solid_pack->NumInstances--;
}

int CarLoader::UnloadSolidPack(LoadedSolidPack *loaded_solid_pack) {
    if (loaded_solid_pack->NumInstances == 0) {
        if (loaded_solid_pack->LoadState == CARLOADSTATE_LOADED) {
            if (loaded_solid_pack->pResourceFile == 0) {
                eUnloadStreamingSolidPack(loaded_solid_pack->Filename);
            } else {
                UnloadResourceFile(loaded_solid_pack->pResourceFile);
                loaded_solid_pack->pResourceFile = 0;
            }
        }

        loaded_solid_pack->Remove();
        LoadedSolidPack_Destruct(loaded_solid_pack, 3);
        return 1;
    }

    return 0;
}

int CarLoader::UnloadCar(LoadedCar *loaded_car) {
    if (loaded_car->LoadState == CARLOADSTATE_LOADED && CarTypeInfoArray[loaded_car->Type].UsageType != 2) {
        unsigned int name_hashes[800];
        int num_hashes = loaded_car->GetModelHashes(name_hashes, 800);

        eUnloadStreamingSolid(name_hashes, num_hashes);
    }

    this->UnallocateSolidPack(loaded_car->pLoadedSolidPack);
    this->UnloadSolidPack(loaded_car->pLoadedSolidPack);
    loaded_car->pLoadedSolidPack = 0;
    return 1;
}

int CarLoader::UnloadWheel(LoadedWheel *loaded_wheel) {
    unsigned int name_hashes[129];

    this->UnallocateSkinLayers(loaded_wheel->LoadedSkinLayersPerm, 4);

    int num_hashes = this->UnloadSkinLayers(name_hashes, 0x80, loaded_wheel->LoadedSkinLayersPerm, 4);

    if (num_hashes != 0) {
        eUnloadStreamingTexture(name_hashes, num_hashes);
    }

    if (loaded_wheel->LoadState == CARLOADSTATE_LOADED) {
        eUnloadStreamingSolid(reinterpret_cast<unsigned int *>(loaded_wheel->ModelNameHashes), 5);
    }

    return 1;
}

int CarLoader::UnloadSkinPerms(LoadedSkin *loaded_skin) {
    unsigned int name_hashes[89];

    this->UnallocateSkinLayers(loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);

    int num_hashes =
        this->UnloadSkinLayers(name_hashes, 0x57, loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);

    if (num_hashes != 0) {
        eUnloadStreamingTexture(name_hashes, num_hashes);
        loaded_skin->NumLoadedSkinLayersPerm = 0;
    }

    return num_hashes != 0;
}

int CarLoader::UnloadSkin(LoadedSkin *loaded_skin) {
    this->UnloadSkinTemporaries(loaded_skin, 1);
    this->UnloadSkinPerms(loaded_skin);
    this->UnallocateTexturePack(loaded_skin->pLoadedTexturesPack);
    this->UnloadTexturePack(loaded_skin->pLoadedTexturesPack);
    loaded_skin->pLoadedTexturesPack = 0;
    return 1;
}

int CarLoader::UnallocateRideInfo(LoadedRideInfo *loaded_ride_info) {
    loaded_ride_info->NumInstances--;

    if (loaded_ride_info->NumInstances != 0) {
        return 0;
    }

    this->NumAllocatedRideInfos--;
    return 1;
}

int CarLoader::UnloadRideInfo(LoadedRideInfo *loaded_ride_info, int leave_if_in_mempool) {
    if (loaded_ride_info->NumInstances < 1) {
        LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

        if (loaded_skin != 0 && loaded_skin->LoadStatePerm == CARLOADSTATE_LOADED &&
            loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED && loaded_skin->DoneComposite != 0) {
            this->UnloadSkinTemporaries(loaded_skin, 0);
        }

        if (leave_if_in_mempool == 0 || !this->IsLoaded(loaded_ride_info)) {
            this->UnloadSkin(loaded_ride_info->pLoadedSkin);
            this->UnloadWheel(loaded_ride_info->pLoadedWheel);
            this->UnloadCar(loaded_ride_info->pLoadedCar);
            loaded_ride_info->Remove();
            bFree(LoadedRideInfoSlotPool, loaded_ride_info);
            this->NumLoadedRideInfos--;
            this->MayNeedDefragmentation++;
            return 1;
        }
    }

    return 0;
}

void CarLoader::Unload(int handle) {
    LoadedRideInfo *loaded_ride_info = this->FindLoadedRideInfo(handle);

    if (loaded_ride_info != 0 && loaded_ride_info->NumInstances != 0) {
        if (loaded_ride_info->HighPriority != 0) {
            loaded_ride_info->Remove();
            this->LoadedRideInfoList.AddTail(loaded_ride_info);
        }

        this->UnallocateRideInfo(loaded_ride_info);

        if (loaded_ride_info->NumInstances == 0 && loaded_ride_info->LoadState == CARLOADSTATE_QUEUED) {
            this->UnloadRideInfo(loaded_ride_info, 0);
        }
    }
}

int CarLoader::IsLoaded(int handle) {
    LoadedRideInfo *loaded_ride_info = this->FindLoadedRideInfo(handle);

    if (loaded_ride_info == 0 || loaded_ride_info->NumInstances == 0) {
        return 0;
    }

    return this->IsLoaded(loaded_ride_info);
}

int CarLoader::IsLoaded(LoadedRideInfo *loaded_ride_info) {
    if (loaded_ride_info->pLoadedCar != 0 && loaded_ride_info->pLoadedCar->LoadState == CARLOADSTATE_LOADED &&
        loaded_ride_info->pLoadedWheel != 0 && loaded_ride_info->pLoadedWheel->LoadState == CARLOADSTATE_LOADED) {
        LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

        if (loaded_skin != 0 && loaded_skin->LoadStatePerm == CARLOADSTATE_LOADED &&
            loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED && loaded_skin->DoneComposite != 0) {
            return 1;
        }
    }

    return 0;
}

void CarLoader::UnloadEverything() {
    LoadedRideInfo *high_priority_ride_info = 0;

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances > 0) {
            this->UnallocateRideInfo(loaded_ride_info);
        }

        if (loaded_ride_info->HighPriority != 0) {
            high_priority_ride_info = loaded_ride_info;
        }
    }

    if (high_priority_ride_info != 0) {
        high_priority_ride_info->Remove();
        this->LoadedRideInfoList.AddTail(high_priority_ride_info);
    }

    while (this->LoadingInProgress != 0) {
        ServiceResourceLoading();
    }

    this->UnloadOverflowedResources();
}

void CarLoader::UnloadOverflowedResources() {
    LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();

    while (loaded_ride_info != this->LoadedRideInfoList.EndOfList()) {
        LoadedRideInfo *next = loaded_ride_info->GetNext();

        this->UnloadRideInfo(loaded_ride_info, 1);
        loaded_ride_info = next;
    }
}

void CarLoader::UnloadUnallocatedRideInfos(int max_left_unloaded) {
    do {
        if (this->NumLoadedRideInfos - this->NumAllocatedRideInfos < max_left_unloaded) {
            return;
        }
    } while (this->RemoveSomethingFromCarMemoryPool(false) != 0);
}

void CarLoader::UnloadAllSkinTemporaries() {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

        if (loaded_skin->LoadStatePerm == CARLOADSTATE_LOADED && loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED &&
            loaded_skin->DoneComposite != 0) {
            this->UnloadSkinTemporaries(loaded_skin, 0);
        } else if (loaded_ride_info->NumInstances == 0 && loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED) {
            this->UnloadSkinTemporaries(loaded_skin, 1);
        }
    }
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

LoadedSolidPack *CarLoader::AllocateSolidPack(const char *filename) {
    LoadedSolidPack *loaded_solid_pack = this->FindLoadedSolidPack(filename);

    if (loaded_solid_pack == 0) {
        loaded_solid_pack = LoadedSolidPack_Construct(static_cast<LoadedSolidPack *>(bOMalloc(LoadedSolidPackSlotPool)), filename);
        this->LoadedSolidPackList.AddTail(loaded_solid_pack);
    }

    loaded_solid_pack->NumInstances++;
    return loaded_solid_pack;
}

LoadedTexturePack *CarLoader::AllocateTexturePack(const char *filename, int max_header_size) {
    LoadedTexturePack *loaded_texture_pack = this->FindLoadedTexturePack(filename);

    if (loaded_texture_pack == 0) {
        loaded_texture_pack = LoadedTexturePack_Construct(static_cast<LoadedTexturePack *>(bOMalloc(LoadedTexturePackSlotPool)),
                                                         filename, max_header_size);
        this->LoadedTexturePackList.AddTail(loaded_texture_pack);
    }

    loaded_texture_pack->NumInstances++;
    return loaded_texture_pack;
}

int CarLoader::AllocateSkinLayers(unsigned int *name_hash_table, int num_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                                  int max_loaded_skin_layers, const char *filename) {
    eStreamingPack *streaming_pack = 0;

    if (filename != 0) {
        streaming_pack = StreamingTexturePackLoader.GetLoadedStreamingPack(filename);

        if (streaming_pack == 0) {
            return 0;
        }
    }

    int num_skin_layers = 0;

    for (int n = 0; n < num_name_hashes; n++) {
        unsigned int name_hash = name_hash_table[n];

        if (name_hash != 0 &&
            (streaming_pack == 0 || StreamingTexturePackLoader.GetLoadedStreamingPack(name_hash) == streaming_pack)) {
            LoadedSkinLayer *loaded_skin_layer = this->FindLoadedSkinLayer(name_hash);

            if (loaded_skin_layer == 0) {
                loaded_skin_layer = LoadedSkinLayer_Construct(static_cast<LoadedSkinLayer *>(bOMalloc(LoadedSkinLayerSlotPool)), name_hash);
                this->LoadedSkinLayerList.AddTail(loaded_skin_layer);
            }

            loaded_skin_layer->NumInstances++;
            loaded_skin_layer_table[num_skin_layers] = loaded_skin_layer;
            num_skin_layers++;
        }
    }

    for (int i = num_skin_layers; i < max_loaded_skin_layers; i++) {
        loaded_skin_layer_table[i] = 0;
    }

    return num_skin_layers;
}

void CarLoader::UnallocateSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers) {
    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != 0) {
            loaded_skin_layer->NumInstances--;
        }
    }
}

int CarLoader::LoadSkinLayers(unsigned int *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                              int num_loaded_skin_layers) {
    int num_name_hashes = 0;

    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != 0 && loaded_skin_layer->LoadState == CARLOADSTATE_QUEUED) {
            loaded_skin_layer->LoadState = CARLOADSTATE_LOADING;
            name_hash_table[num_name_hashes] = loaded_skin_layer->NameHash;
            num_name_hashes++;
        }
    }

    return num_name_hashes;
}

void CarLoader::UnallocateTexturePack(LoadedTexturePack *loaded_texture_pack) {
    loaded_texture_pack->NumInstances--;
}

int CarLoader::GetMemoryEntries(LoadedSolidPack *loaded_solid_pack, void **memory_entries, int num_memory_entries) {
    if (loaded_solid_pack->pStreamingPack != 0) {
        num_memory_entries = loaded_solid_pack->pStreamingPack->GetHeaderMemoryEntries(memory_entries, num_memory_entries);
    }

    if (loaded_solid_pack->pResourceFile != 0) {
        memory_entries[num_memory_entries] = loaded_solid_pack->pResourceFile->GetMemory();
        num_memory_entries++;
    }

    return num_memory_entries;
}

int CarLoader::GetMemoryEntries(LoadedTexturePack *loaded_texture_pack, void **memory_entries, int num_memory_entries) {
    if (loaded_texture_pack->pStreamingPack != 0) {
        return loaded_texture_pack->pStreamingPack->GetHeaderMemoryEntries(memory_entries, num_memory_entries);
    }

    return 0;
}

int CarLoader::GetMemoryEntries(LoadedWheel *loaded_wheel, void **memory_entries, int num_memory_entries) {
    for (int i = 0; i < 4; i++) {
        num_memory_entries = this->GetMemoryEntries(loaded_wheel->LoadedSkinLayersPerm[i], memory_entries, num_memory_entries);
    }

    for (int i = 0; i < 4; i++) {
        num_memory_entries = this->GetMemoryEntries(loaded_wheel->LoadedSkinLayersTemp[i], memory_entries, num_memory_entries);
    }

    return StreamingSolidPackLoader.GetMemoryEntries(reinterpret_cast<unsigned int *>(loaded_wheel->ModelNameHashes), 5, memory_entries,
                                                     num_memory_entries);
}

int CarLoader::GetMemoryEntries(LoadedSkin *loaded_skin, void **memory_entries, int num_memory_entries) {
    num_memory_entries = this->GetMemoryEntries(loaded_skin->pLoadedTexturesPack, memory_entries, num_memory_entries);

    for (int i = 0; i < loaded_skin->NumLoadedSkinLayersPerm; i++) {
        num_memory_entries = this->GetMemoryEntries(loaded_skin->LoadedSkinLayersPerm[i], memory_entries, num_memory_entries);
    }

    for (int i = 0; i < loaded_skin->NumLoadedSkinLayersTemp; i++) {
        num_memory_entries = this->GetMemoryEntries(loaded_skin->LoadedSkinLayersTemp[i], memory_entries, num_memory_entries);
    }

    return num_memory_entries;
}

int CarLoader::GetMemoryEntries(LoadedSkinLayer *loaded_skin_layer, void **memory_entries, int num_memory_entries) {
    if (loaded_skin_layer != 0 && loaded_skin_layer->LoadState == CARLOADSTATE_LOADED) {
        eStreamingEntry *streaming_entry = StreamingTexturePackLoader.GetStreamingEntry(loaded_skin_layer->NameHash);

        if (streaming_entry != 0 && streaming_entry->ChunkData != 0) {
            memory_entries[num_memory_entries] = streaming_entry->ChunkData;
            num_memory_entries++;
        }
    }

    return num_memory_entries;
}

int CarLoader::GetMemoryEntries(LoadedCar *loaded_car, void **memory_entries, int num_memory_entries) {
    unsigned int name_hashes[800];
    int num_used_entries = this->GetMemoryEntries(loaded_car->pLoadedSolidPack, memory_entries, num_memory_entries);
    int num_hashes = loaded_car->GetModelHashes(name_hashes, 800);

    return StreamingSolidPackLoader.GetMemoryEntries(name_hashes, num_hashes, memory_entries, num_used_entries);
}

int CarLoader::LoadTexturePack(LoadedTexturePack *loaded_texture_pack, int use_memory_pool) {
    int memory_pool_num = 0;

    if (use_memory_pool != 0) {
        CarLoader_MakeSpaceInCarMemoryPool(this, loaded_texture_pack->MaxHeaderSize, 0, true);
        memory_pool_num = CarLoaderMemoryPoolNumber;
    }

    this->LoadingInProgress = 1;
    loaded_texture_pack->LoadState = CARLOADSTATE_LOADING;
    eLoadStreamingTexturePack(loaded_texture_pack->Filename, LoadedTexturePackCallbackBridge,
                              reinterpret_cast<unsigned int>(loaded_texture_pack), memory_pool_num);
    loaded_texture_pack->pStreamingPack = StreamingTexturePackLoader.GetLoadedStreamingPack(loaded_texture_pack->Filename);
    return 1;
}

void CarLoader::LoadedTexturePackCallback(LoadedTexturePack *loaded_texture_pack) {
    loaded_texture_pack->LoadState = CARLOADSTATE_LOADED;
    this->LoadingDoneCallback();
}

int CarLoader::UnloadTexturePack(LoadedTexturePack *loaded_texture_pack) {
    if (loaded_texture_pack->NumInstances == 0) {
        if (loaded_texture_pack->LoadState == CARLOADSTATE_LOADED && loaded_texture_pack->pStreamingPack != 0) {
            eUnloadStreamingTexturePack(loaded_texture_pack->Filename);
        }

        loaded_texture_pack->Remove();
        LoadedTexturePack_Destruct(loaded_texture_pack, 3);
        return 1;
    }

    return 0;
}

int CarLoader::LoadSkin(LoadedSkin *loaded_skin, int load_perm_layers) {
    unsigned int name_hashes[87];
    int loaded_hashes;

    if (load_perm_layers == 0) {
        loaded_hashes = this->LoadSkinLayers(name_hashes, 0x57, loaded_skin->LoadedSkinLayersTemp, loaded_skin->NumLoadedSkinLayersTemp);
        loaded_skin->LoadStateTemp = loaded_hashes != 0 ? CARLOADSTATE_LOADING : CARLOADSTATE_LOADED;
    } else {
        loaded_hashes = this->LoadSkinLayers(name_hashes, 0x57, loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);
        loaded_skin->LoadStatePerm = loaded_hashes != 0 ? CARLOADSTATE_LOADING : CARLOADSTATE_LOADED;
    }

    int memory_pool_num = CarLoaderMemoryPoolNumber;

    if (loaded_hashes > 0) {
        if (load_perm_layers == 0 && this->LoadingMode != MODE_IN_GAME) {
            memory_pool_num = this->LoadingMode == MODE_LOADING_GAME ? 7 : 0;
        } else {
            do {
                if (StreamingTexturePackLoader.TestLoadStreamingEntry(name_hashes, loaded_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                    break;
                }
            } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);
        }

        this->LoadingInProgress = 1;
        eLoadStreamingTexture(name_hashes, loaded_hashes, LoadedSkinCallbackBridge, loaded_skin, memory_pool_num);
        return 1;
    }

    return 0;
}

void CarLoader::LoadedSkinCallback(LoadedSkin *loaded_skin) {
    if (loaded_skin->LoadStatePerm == CARLOADSTATE_LOADING) {
        loaded_skin->LoadStatePerm = CARLOADSTATE_LOADED;
        this->LoadedSkinLayers(loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);
    }

    if (loaded_skin->LoadStateTemp == CARLOADSTATE_LOADING) {
        loaded_skin->LoadStateTemp = CARLOADSTATE_LOADED;
        this->LoadedSkinLayers(loaded_skin->LoadedSkinLayersTemp, loaded_skin->NumLoadedSkinLayersTemp);
    }

    this->LoadingDoneCallback();
}

int CarLoader::UnloadSkinTemporaries(LoadedSkin *loaded_skin, int force_unload) {
    int unloaded = 0;

    if ((loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED && loaded_skin->DoneComposite != 0) || force_unload != 0) {
        unsigned int name_hashes[87];

        this->UnallocateSkinLayers(loaded_skin->LoadedSkinLayersTemp, loaded_skin->NumLoadedSkinLayersTemp);
        int unloaded_hashes = this->UnloadSkinLayers(name_hashes, 0x57, loaded_skin->LoadedSkinLayersTemp,
                                                     loaded_skin->NumLoadedSkinLayersTemp);
        unloaded = unloaded_hashes != 0;
        loaded_skin->NumLoadedSkinLayersTemp = 0;

        if (unloaded != 0) {
            eUnloadStreamingTexture(name_hashes, unloaded_hashes);
        }

        if (loaded_skin->pLoadedVinylsPack != 0) {
            unloaded += 1;
            this->UnallocateTexturePack(loaded_skin->pLoadedVinylsPack);
            this->UnloadTexturePack(loaded_skin->pLoadedVinylsPack);
            loaded_skin->pLoadedVinylsPack = 0;
        }
    }

    return unloaded;
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

LoadedRideInfo *CarLoader::FindLoadedRideInfo(int handle) {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->ID == handle) {
            return loaded_ride_info;
        }
    }

    return 0;
}

LoadedRideInfo *CarLoader::FindLoadedRideInfo(RideInfo *ride_info) {
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
