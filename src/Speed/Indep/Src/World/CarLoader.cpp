#include "./CarLoader.hpp"
#include "Speed/Indep/Src/Ecstasy/DefragFixer.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
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
        return static_cast<unsigned int>(this->NumAttributes * sizeof(short) + sizeof(short));
    }

    void EndianSwap() {
        bPlatEndianSwap(&this->NumAttributes);

        for (int i = 0; i < this->NumAttributes; i++) {
            bPlatEndianSwap(&this->AttributeOffsetTable[i]);
        }
    }
};
struct CarPartModelTable {
    char TemplatedNameHashes;
    char pad;
    unsigned short MiddleStringOffset;
    const char *ModelNames[1][5];

    unsigned int GetModelNameHash(unsigned int base_namehash, int model_num, int lod);

    void EndianSwap() {
        bPlatEndianSwap(&this->MiddleStringOffset);

        for (int i = 0; i < 1; i++) {
            for (int j = 0; j < 5; j++) {
                bPlatEndianSwap(reinterpret_cast<unsigned int *>(const_cast<const char **>(&this->ModelNames[i][j])));
            }
        }
    }
};

void CarPartAttribute::EndianSwap() {
    bPlatEndianSwap(&this->Params.iParam);
    bPlatEndianSwap(&this->NameHash);
}
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

    void EndianSwap() {
        bPlatEndianSwap(&this->Version);
        bPlatEndianSwap(&this->NumParts);
        bPlatEndianSwap(&this->NumAttributes);
        bPlatEndianSwap(&this->NumTypeNames);
        bPlatEndianSwap(&this->NumModelTables);
        bPlatEndianSwap(&this->NumAttributeTables);
    }

    void InPlaceInit() {
        this->Next = this;
        this->Prev = this;
    }
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
struct _DefragmentParams {
    int NumCopyStorage;
    int CopyStorageSize[8];
    void *CopyStorageMem[8];
    int LargestAllocationSize;
    char LargestAllocationName[64];
    void *pAllocation;
    void *pNewAllocation;
    char AllocationName[64];
};
struct CarMemoryInfoEntryLayout {
    const char *Name;
    int pad0;
    int Size;
    int pad1[3];
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
extern CarMemoryInfoEntryLayout CarMemoryInfoTable[6];
extern const char lbl_8040A594[] asm("lbl_8040A594");

unsigned int CarPartModelTable::GetModelNameHash(unsigned int base_namehash, int model_num, int lod) {
    const char *model_name = reinterpret_cast<const char *const *>(reinterpret_cast<unsigned char *>(this) + 4)[lod + model_num * 5];

    if (reinterpret_cast<unsigned int>(model_name) == 0xFFFFFFFF) {
        return 0;
    }

    if (TemplatedNameHashes != 0) {
        char lod_suffix[3];

        lod_suffix[0] = '_';
        lod_suffix[1] = static_cast<char>(lod + 'A');
        lod_suffix[2] = '\0';

        if (MiddleStringOffset != 0xFFFF) {
            base_namehash = bStringHash(MasterCarPartPack->StringTable + MiddleStringOffset * 4);
        }

        base_namehash = bStringHash(model_name, base_namehash);
        return bStringHash(lod_suffix, base_namehash);
    }

    return reinterpret_cast<unsigned int>(model_name);
}

int ConvertVinylGroupNumberToVinylType(int vinyl_group_number) {
    if (vinyl_group_number != 9) {
        if (vinyl_group_number < 10) {
            if (vinyl_group_number < 6) {
                if (vinyl_group_number < 4) {
                    if (vinyl_group_number == 1) {
                        return 1;
                    }

                    if (vinyl_group_number < 2) {
                        if (vinyl_group_number != 0) {
                            return 0;
                        }
                    } else if (vinyl_group_number != 2) {
                        if (vinyl_group_number != 3) {
                            return 0;
                        }

                        return 1;
                    }
                }
            } else if (vinyl_group_number != 7) {
                return 1;
            }
        } else if (vinyl_group_number != 0xE) {
            if (vinyl_group_number > 0xE) {
                if (vinyl_group_number != 0x10) {
                    if (vinyl_group_number < 0x10) {
                        return 4;
                    }

                    if (vinyl_group_number == 0x11) {
                        return 2;
                    }

                    if (vinyl_group_number != 0x12) {
                        return 0;
                    }
                }

                return 3;
            }

            if (vinyl_group_number > 0xC) {
                return 1;
            }

            if (vinyl_group_number < 0xB) {
                return 1;
            }
        }
    }

    return 0;
}
int CarInfo_GetMaxCompositingBufferSize();
void GetUsedCarTextureInfo(UsedCarTextureInfo *used_texture_info, RideInfo *ride_info, int front_end_only);
extern int CarLoaderMemoryPoolNumber;
int CompositeSkin(RideInfo *ride_info);
extern SlotPool *LoadedTexturePackSlotPool;
extern SlotPool *LoadedSolidPackSlotPool;
extern SlotPool *LoadedSkinLayerSlotPool;
extern SlotPool *LoadedRideInfoSlotPool;
extern int UsePrecompositeVinyls;
extern _DefragmentParams DefragmentParams;
int bMemoryGetAllocations(int pool_num, void **allocations, int max_allocations);
int bGetMallocSize(const void *ptr);
const char *bGetMallocName(void *ptr);
int bGetMallocPool(void *ptr);
void ScratchPadMemCpy(void *dest, const void *src, unsigned int numbytes);
int CarInfo_GetResourceCost(CarType car_type, bool is_player_car, bool two_player);
float GetDebugRealTime();
extern int QueuedFileDefaultPriority;
extern int CarLoaderServiceLoadingDepth;
void SetDelayedResourceCallback(void (*callback)(int), int param);
void eFixupReplacementTextureTables();
void RefreshAllRenderInfo(CarType car_type);
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
int GatherModelHashes(RideInfo *ride_info, unsigned int *model_hashes, int num_hashes, int max_model_hashes, int first, int last);
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
inline void LoadedTexturePack::operator delete(void *ptr) {
    bFree(LoadedTexturePackSlotPool, ptr);
}

LoadedTexturePack::~LoadedTexturePack() {
    bFreeSharedString(this->Filename);
}

inline void LoadedSolidPack::operator delete(void *ptr) {
    bFree(LoadedSolidPackSlotPool, ptr);
}

LoadedSolidPack::~LoadedSolidPack() {
    bFreeSharedString(this->Filename);
}

inline void LoadedSkinLayer::operator delete(void *ptr) {
    bFree(LoadedSkinLayerSlotPool, ptr);
}

inline void LoadedRideInfo::operator delete(void *ptr) {
    bFree(LoadedRideInfoSlotPool, ptr);
}

inline int LoadedSkin::IsLoaded() {
    return this->LoadStatePerm == CARLOADSTATE_LOADED && this->LoadStateTemp == CARLOADSTATE_LOADED && this->DoneComposite != 0;
}

inline int LoadedCar::ShouldWeStream() {
    CarTypeInfo *car_type_info = &CarTypeInfoArray[this->Type];
    int should_stream = 1;

    if (car_type_info->GetCarUsageType() == 2) {
        should_stream = 0;
    }

    return should_stream;
}

void CarLoader_UnallocateSkinLayers(CarLoader *car_loader, LoadedSkinLayer **loaded_skin_layers, int num_layers)
    asm("UnallocateSkinLayers__9CarLoaderPP15LoadedSkinLayeri");
int CarLoader_MakeSpaceInCarMemoryPool(CarLoader *car_loader, int required_size, int max_allocations, bool stream_textures)
    asm("MakeSpaceInCarMemoryPool__9CarLoaderiib");
void CarLoader_UnloadUnallocatedRideInfos(CarLoader *car_loader, int num_ride_infos)
    asm("UnloadUnallocatedRideInfos__9CarLoaderi");
void CarLoader_LoadedSolidPackCallback(CarLoader *car_loader, LoadedSolidPack *loaded_solid_pack)
    asm("LoadedSolidPackCallback__9CarLoaderP15LoadedSolidPack");
void CarLoader_LoadedCarCallback(CarLoader *car_loader, LoadedCar *loaded_car) asm("LoadedCarCallback__9CarLoaderP9LoadedCar");
void CarLoader_LoadedWheelModelsCallback(CarLoader *car_loader) asm("LoadedWheelModelsCallback__9CarLoader");
void CarLoader_LoadedWheelTexturesCallback(CarLoader *car_loader) asm("LoadedWheelTexturesCallback__9CarLoader");
void CarLoader_LoadedAllTexturesFromPackCallback(CarLoader *car_loader) asm("LoadedAllTexturesFromPackCallback__9CarLoader");
void bCloseMemoryPool(int pool_num);
bool bSetMemoryPoolDebugFill(int pool_num, bool on_off);
void bSetMemoryPoolTopDirection(int pool_num, bool top_means_larger_address);
void eLoadStreamingSolid(unsigned int *name_hash_table, int num_hashes, void (*callback)(unsigned int), unsigned int param0,
                         int memory_pool_num);
int eLoadStreamingSolidPack(const char *filename, void (*callback_function)(int), int callback_param, int memory_pool_num);
void eLoadStreamingTexture(unsigned int *name_hash_table, int num_hashes, void (*callback)(unsigned int), unsigned int param0,
                           int memory_pool_num);
int eLoadStreamingTexturePack(const char *filename, void (*callback_func)(unsigned int), unsigned int callback_param, int memory_pool_num);
void eUnloadStreamingTexture(unsigned int *name_hash_table, int num_hashes);
void eUnloadStreamingSolid(unsigned int *name_hash_table, int num_hashes);
int eUnloadStreamingSolidPack(const char *filename);
void eUnloadStreamingTexturePack(const char *filename);

struct QueuedFilePrioritySetter {
    int SavedPriority;

    QueuedFilePrioritySetter()
        : SavedPriority(QueuedFileDefaultPriority)
    {
        CarLoaderServiceLoadingDepth++;
        QueuedFileDefaultPriority = 4;
    }

    ~QueuedFilePrioritySetter() {
        CarLoaderServiceLoadingDepth--;
        QueuedFileDefaultPriority = SavedPriority;
    }
};

int GatherModelHashes(RideInfo *ride_info, unsigned int *model_hashes, int num_hashes, int max_model_hashes, int first, int last) {
    ProfileNode profile_node("GatherModelHashes", 0);
    CARPART_LOD minLodLevel = ride_info->GetMinLodLevel();
    CARPART_LOD maxLodLevel = ride_info->GetMaxLodLevel();

    for (int slotIx = first; slotIx < last; slotIx++) {
        for (CarPart *part = CarPartDB.NewGetFirstCarPart(ride_info->Type, slotIx, 0, -1); part != 0;
             part = CarPartDB.NewGetNextCarPart(part, ride_info->Type, slotIx, 0, -1)) {
            for (int modelIx = 0; modelIx < 1; modelIx++) {
                for (int lod = minLodLevel; lod <= maxLodLevel; lod++) {
                    unsigned int model_name_hash = 0;

                    if (part != 0) {
                        model_name_hash = part->GetModelNameHash(modelIx, lod);
                    }

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

    return num_hashes;
}

void *MoveDefragmentAllocation(void *allocation) {
    ProfileNode profile_node("MoveDefragmentAllocation", 0);
    int allocation_size = bGetMallocSize(allocation);

    if (allocation_size > DefragmentParams.LargestAllocationSize) {
        return allocation;
    }

    bool use_copy_storage =
        allocation_size + 0x480 > reinterpret_cast<unsigned int>(allocation) - reinterpret_cast<unsigned int>(DefragmentParams.pNewAllocation);
    void *new_allocation = allocation;

    if (use_copy_storage) {
        int amount_copied = 0;
        int n = 0;
        if (allocation_size > 0) {
            do {
                int amount_to_copy = allocation_size - amount_copied;

                if (DefragmentParams.CopyStorageSize[n] < amount_to_copy) {
                    amount_to_copy = DefragmentParams.CopyStorageSize[n];
                }

                ScratchPadMemCpy(DefragmentParams.CopyStorageMem[n], reinterpret_cast<unsigned char *>(allocation) + amount_copied, amount_to_copy);
                amount_copied += amount_to_copy;
                n++;
            } while (amount_copied < allocation_size);
        }

        bFree(allocation);
        new_allocation = 0;
        DefragmentParams.pAllocation = 0;
    }

    allocation = bMalloc(allocation_size, (CarLoaderMemoryPoolNumber & 0xF) | 0x2000);

    if (allocation != DefragmentParams.pNewAllocation) {
        bMemoryPrintAllocationsByAddress(CarLoaderMemoryPoolNumber, 0, 0x7FFFFFFF);
        bBreak();
    }

    if (use_copy_storage) {
        int amount_copied = 0;
        int n = 0;
        if (allocation_size > 0) {
            do {
                int amount_to_copy = allocation_size - amount_copied;

                if (DefragmentParams.CopyStorageSize[n] < amount_to_copy) {
                    amount_to_copy = DefragmentParams.CopyStorageSize[n];
                }

                ScratchPadMemCpy(reinterpret_cast<unsigned char *>(allocation) + amount_copied, DefragmentParams.CopyStorageMem[n], amount_to_copy);
                amount_copied += amount_to_copy;
                n++;
            } while (amount_copied < allocation_size);
        }
    } else {
        ScratchPadMemCpy(allocation, new_allocation, allocation_size);
        bFree(new_allocation);
        DefragmentParams.pAllocation = 0;
    }

    DCStoreRange(allocation, allocation_size);

    return allocation;
}

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

LoadedTexturePack::LoadedTexturePack(const char *filename, int max_header_size)
    : Filename(bAllocateSharedString(filename)), //
      NumInstances(0),                          //
      LoadState(0),                             //
      Pad0(0),                                  //
      MaxHeaderSize(max_header_size),           //
      pStreamingPack(0) {
    if (bFileSize(this->Filename) == 0) {
        this->LoadState = 2;
    }
}

LoadedSolidPack::LoadedSolidPack(const char *filename)
    : Filename(bAllocateSharedString(filename)), //
      NumInstances(0),                          //
      LoadState(0),                             //
      pStreamingPack(0),                        //
      pResourceFile(0) {}

int CarInfo_GetResourceCost(CarType car_type, bool is_player_car, bool two_player) {
    CarTypeInfo *car_type_info = &CarTypeInfoArray[car_type];
    int car_memory_info = car_type_info->CarMemTypeHash;
    int i;
    CarMemoryInfoEntryLayout *entry;

    (void)two_player;
    if (is_player_car != 0) {
        car_memory_info = bStringHash(lbl_8040A594);
    }

    i = 0;
    do {
        entry = &CarMemoryInfoTable[i];
        if (bStringHash(entry->Name) != car_memory_info) {
            i++;
        } else {
            return entry->Size << 10;
        }
    } while (i < 6);

    return 0;
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
    ProfileNode profile_node("GetModelHashes", 0);
    RideInfo *ride_info;

    bMemSet(model_hashes, 0, max_model_hashes << 2);
    ride_info = this->pRideInfo;

    CARPART_LOD minLodLevel = ride_info->GetMinLodLevel();
    CARPART_LOD maxLodLevel = ride_info->GetMaxLodLevel();

    if (this->InFrontEnd != 0) {
        minLodLevel = ride_info->GetMinFELodLevel();
        maxLodLevel = ride_info->GetMaxFELodLevel();
    }

    int num_hashes = 0;

    for (int slot_id = 0; slot_id < 0x4c; slot_id++) {
        CarPart *car_part = ride_info->GetPart(slot_id);
        CARPART_LOD currentSlotMinLodLevel = minLodLevel;
        CARPART_LOD currentSlotMaxLodLevel = maxLodLevel;

        ride_info->GetSpecialLODRangeForCarSlot(slot_id, &currentSlotMinLodLevel, &currentSlotMaxLodLevel, this->InFrontEnd != 0);

        for (int model = 0; model < 1; model++) {
            for (int lod = currentSlotMinLodLevel; lod <= currentSlotMaxLodLevel; lod++) {
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

    int new_num_hashes = GatherModelHashes(ride_info, model_hashes, num_hashes, max_model_hashes, 0x2e, 0x33);
    num_hashes = new_num_hashes;
    new_num_hashes = GatherModelHashes(ride_info, model_hashes, num_hashes, max_model_hashes, 1, 0x17);
    num_hashes = new_num_hashes;

    if (num_hashes > max_model_hashes) {
        num_hashes = max_model_hashes;
    }

    unsigned char bitfield[512];

    bMemSet(bitfield, 0, 0x200);
    profile_node.Begin("GetModelHashes", 0);

    int num_hits = 0;

    int n = 0;

    while (n < num_hashes) {
        unsigned int hash = model_hashes[n];
        int bit = (hash >> 3) & 0x1ff;
        bool duplicate = false;

        if (((bitfield[bit] >> (hash & 7)) & 1U) == 0) {
            bitfield[bit] |= 1 << (hash & 7);
        } else {
            int i = 0;

            if (num_hits > 0) {
                while (i < num_hits && model_hashes[i] != hash) {
                    i++;
                }
            }

            if (i != num_hits) {
                duplicate = true;
            }
        }

        if (!duplicate) {
            model_hashes[num_hits] = hash;
            num_hits++;
        }

        n++;
    }

    return num_hits;
}

LoadedSkinLayer::LoadedSkinLayer(unsigned int name_hash) {
    this->NameHash = name_hash;
    this->NumInstances = 0;
    this->LoadState = 0;
    this->pad0 = 0;
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
            if (loaded_solid_pack->pResourceFile != 0) {
                UnloadResourceFile(loaded_solid_pack->pResourceFile);
                loaded_solid_pack->pResourceFile = 0;
            } else {
                eUnloadStreamingSolidPack(loaded_solid_pack->Filename);
            }
        }

        delete this->LoadedSolidPackList.Remove(loaded_solid_pack);
        return 1;
    }

    return 0;
}

int CarLoader::UnloadCar(LoadedCar *loaded_car) {
    if (loaded_car->LoadState == CARLOADSTATE_LOADED && loaded_car->ShouldWeStream()) {
        unsigned int model_hashes[800];
        int num_model_hashes = loaded_car->GetModelHashes(model_hashes, 800);

        eUnloadStreamingSolid(model_hashes, num_model_hashes);
    }

    this->UnallocateSolidPack(loaded_car->pLoadedSolidPack);
    this->UnloadSolidPack(loaded_car->pLoadedSolidPack);
    loaded_car->pLoadedSolidPack = 0;
    return 1;
}

int CarLoader::UnloadWheel(LoadedWheel *loaded_wheel) {
    unsigned int name_hashes[128];

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
    unsigned int name_hash_table[87];

    this->UnallocateSkinLayers(loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);

    int num_name_hashes =
        this->UnloadSkinLayers(name_hash_table, 0x57, loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);

    if (num_name_hashes == 0) {
        return 0;
    }

    eUnloadStreamingTexture(name_hash_table, num_name_hashes);
    loaded_skin->NumLoadedSkinLayersPerm = 0;
    return 1;
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

        if (loaded_skin != 0 && loaded_skin->IsLoaded()) {
            this->UnloadSkinTemporaries(loaded_skin, 0);
        }

        int in_mempool = this->IsLoaded(loaded_ride_info);

        if (leave_if_in_mempool != 0) {
            if (in_mempool != 0) {
                return 0;
            }
        }

        this->UnloadSkin(loaded_ride_info->pLoadedSkin);
        this->UnloadWheel(loaded_ride_info->pLoadedWheel);
        this->UnloadCar(loaded_ride_info->pLoadedCar);
        delete this->LoadedRideInfoList.Remove(loaded_ride_info);
        this->NumLoadedRideInfos--;
        this->MayNeedDefragmentation++;
        return 1;
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

bool CarLoader::MakeSpaceInPool(int size) {
    while (this->LoadingInProgress != 0) {
        ServiceResourceLoading();
    }

    return this->MakeSpaceInCarMemoryPool(size, 0, false) != 0;
}

int CarLoader::MakeSpaceInCarMemoryPool(int largest_malloc_needed, int amount_free_needed, bool allocating_stream_header_chunks) {
    if (amount_free_needed == 0) {
        amount_free_needed = largest_malloc_needed;
    }

    int free_memory = bCountFreeMemory(CarLoaderMemoryPoolNumber);
    int largest_malloc = bLargestMalloc((CarLoaderMemoryPoolNumber & 0xF) | 0x2000);

    while ((free_memory < amount_free_needed || largest_malloc < largest_malloc_needed) &&
           this->RemoveSomethingFromCarMemoryPool(true) != 0) {
        free_memory = bCountFreeMemory(CarLoaderMemoryPoolNumber);
        largest_malloc = bLargestMalloc((CarLoaderMemoryPoolNumber & 0xF) | 0x2000);
    }

    if (free_memory < amount_free_needed || largest_malloc < largest_malloc_needed) {
        return 0;
    }

    if (allocating_stream_header_chunks) {
        unsigned int lowest_header_chunks = 0;

        for (int i = 0; i < 2; i++) {
            eStreamPackLoader *loader = &StreamingTexturePackLoader;

            if (i == 1) {
                loader = &StreamingSolidPackLoader;
            }

            for (eStreamingPack *streaming_pack = loader->LoadedStreamingPackList.GetHead();
                 streaming_pack != loader->LoadedStreamingPackList.EndOfList(); streaming_pack = streaming_pack->GetNext()) {
                if (streaming_pack->HeaderChunks != 0 && bGetMallocPool(streaming_pack->HeaderChunks) == CarLoaderMemoryPoolNumber &&
                    (lowest_header_chunks == 0 ||
                     reinterpret_cast<unsigned int>(streaming_pack->HeaderChunks) < lowest_header_chunks)) {
                    lowest_header_chunks = reinterpret_cast<unsigned int>(streaming_pack->HeaderChunks);
                }
            }
        }

        void *allocated_memory = bMalloc(largest_malloc_needed, (CarLoaderMemoryPoolNumber & 0xF) | 0x2040);

        bFree(allocated_memory);

        if (lowest_header_chunks != 0 && allocated_memory != 0) {
            int distance = reinterpret_cast<unsigned int>(allocated_memory) + largest_malloc_needed - lowest_header_chunks;

            if (distance < 0) {
                distance = -distance;
            }

            if (distance > 0x100) {
                this->DefragmentPool();
            }
        }
    }

    return 1;
}

int CarLoader::RemoveSomethingFromCarMemoryPool(bool force_unload) {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (this->UnloadRideInfo(loaded_ride_info, 0) != 0) {
            return 1;
        }
    }

    if (force_unload != 0) {
        if (this->DefragmentPool() != 0) {
            return 1;
        }

        if (this->NumSpongeAllocations == 0) {
            for (int pass = 0; pass < 2; pass++) {
                for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
                     loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
                    if (loaded_ride_info->HighPriority == 0 || pass == 1) {
                        LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

                        if (loaded_skin->LoadStatePerm == CARLOADSTATE_LOADED && this->UnloadSkinPerms(loaded_skin) != 0) {
                            if (this->LoadingMode == MODE_FRONT_END) {
                                bBreak();
                            }

                            eFixupReplacementTextureTables();
                            RefreshAllRenderInfo(loaded_skin->pRideInfo->Type);
                            return 1;
                        }
                    }
                }
            }

            this->PrintMemoryUsage(false);
            bBreak();
            return 0;
        }

        this->NumSpongeAllocations--;
        bFree(this->SpongeAllocations[this->NumSpongeAllocations]);
        this->MayNeedDefragmentation++;
        return 1;
    }

    return 0;
}

void CarLoader::PrintMemoryUsage(bool on_screen) {
    static float lastTime;

    if (2.5f <= GetDebugRealTime() - lastTime) {
        void *allocations[1152];
        char allocation_uses[1024];
        void *memory_entries[256];
        int total_unique_loaded_size = 0;
        int num_allocations;

        lastTime = GetDebugRealTime();
        num_allocations = bMemoryGetAllocations(CarLoaderMemoryPoolNumber, allocations, 0x480);
        bMemSet(allocation_uses, 0, 0x400);

        for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
             loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
            if (!on_screen) {
                this->IsLoaded(loaded_ride_info);
            }

            int num_memory_entries = this->GetMemoryEntries(loaded_ride_info->pLoadedCar, memory_entries, 0);
            int total_memory_size = 0;
            int unique_memory_size = 0;

            num_memory_entries = this->GetMemoryEntries(loaded_ride_info->pLoadedSkin, memory_entries, num_memory_entries);
            num_memory_entries = this->GetMemoryEntries(loaded_ride_info->pLoadedWheel, memory_entries, num_memory_entries);

            for (int i = 0; i < num_memory_entries; i++) {
                void *memory_entry = memory_entries[i];
                int allocation_size = bGetMallocSize(memory_entry);

                for (int j = 0; j < num_allocations; j++) {
                    if (memory_entry == allocations[j]) {
                        total_memory_size += allocation_size;

                        if (allocation_uses[j] == 0) {
                            unique_memory_size += allocation_size;
                        }

                        allocation_uses[j]++;
                        break;
                    }
                }
            }

            if (loaded_ride_info->NumInstances > 0) {
                total_unique_loaded_size += unique_memory_size;
            }

            int resource_cost = CarInfo_GetResourceCost(loaded_ride_info->TheRideInfo.Type, loaded_ride_info->IsPlayerCar != 0,
                                                        this->TwoPlayerFlag != 0);

            if (on_screen && loaded_ride_info->NumInstances > 0) {
                bReleasePrintf("%s: %dK %dK %dK\n", loaded_ride_info->Name, (total_memory_size + 0x3FF) >> 10,
                               (unique_memory_size + 0x3FF) >> 10, (resource_cost + 0x3FF) >> 10);
            }
        }

        if (on_screen) {
            bReleasePrintf("Loaded cars: %dK %dK\n", (total_unique_loaded_size + 0x3FF) >> 10,
                           (this->MemoryPoolSize + 0x3FF) >> 10);
        }

        if (num_allocations > 0) {
            int num_sponge_allocations = this->NumSpongeAllocations;

            for (int i = 0; i < num_allocations; i++) {
                void *allocation = allocations[i];

                for (int j = 0; j < num_sponge_allocations; j++) {
                    if (this->SpongeAllocations[j] == allocation) {
                        allocation_uses[i] = 1;
                    }
                }
            }
        }
    }
}

bool CarLoader::DefragmentAllocation(void *allocation) {
    static int last_result_was_textures;

    for (int i = 0; i < this->NumSpongeAllocations; i++) {
        if (this->SpongeAllocations[i] == allocation) {
            this->SpongeAllocations[i] = MoveDefragmentAllocation(allocation);
            return true;
        }
    }

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        ResourceFile *resource_file = loaded_ride_info->pLoadedCar->pLoadedSolidPack->pResourceFile;

        if (resource_file != 0 && resource_file->GetMemory() == allocation) {
            resource_file->ManualUnload();
            resource_file->ManualReload(reinterpret_cast<bChunk *>(MoveDefragmentAllocation(allocation)));
            return true;
        }
    }

    if (last_result_was_textures != 0 && StreamingTexturePackLoader.DefragmentAllocation(allocation)) {
        return true;
    }

    if (StreamingSolidPackLoader.DefragmentAllocation(allocation)) {
        last_result_was_textures = 0;
        return true;
    }

    if (StreamingTexturePackLoader.DefragmentAllocation(allocation)) {
        last_result_was_textures = 1;
        return true;
    }

    return false;
}

bool CarLoader::AllocateDefragmentStorage() {
    const char *memory_pool_names[5] = {
        "Main Pool",
        "Track Streaming",
        "Audio Memory Pool",
        "Speech Cache Memory Pool",
        "FEngMemoryPool",
    };
    int total_size = 0;
    int num_copy_storage = 0;

    if (DefragmentParams.LargestAllocationSize > 0) {
        while (true) {
            int largest_malloc = 0;
            int memory_pool_num = -1;
            int required_size = DefragmentParams.LargestAllocationSize - total_size;

            for (int i = 0; i < 5; i++) {
                int pool_num = bGetMemoryPoolNum(memory_pool_names[i]);

                if (pool_num > -1) {
                    int pool_largest_malloc = bLargestMalloc(pool_num);

                    if (largest_malloc < pool_largest_malloc) {
                        memory_pool_num = pool_num;
                        largest_malloc = pool_largest_malloc;
                    }
                }
            }

            if (largest_malloc == 0) {
                break;
            }

            if (required_size < largest_malloc) {
                largest_malloc = required_size;
            }

            DefragmentParams.CopyStorageSize[num_copy_storage] = largest_malloc;
            DefragmentParams.CopyStorageMem[num_copy_storage] = bMalloc(largest_malloc, memory_pool_num & 0xF);
            total_size += largest_malloc;
            num_copy_storage++;

            if (DefragmentParams.LargestAllocationSize <= total_size || num_copy_storage > 7) {
                break;
            }
        }
    }

    DefragmentParams.NumCopyStorage = num_copy_storage;
    return total_size == DefragmentParams.LargestAllocationSize;
}

void CarLoader::FreeDefragmentStorage() {
    for (int i = 0; i < DefragmentParams.NumCopyStorage; i++) {
        bFree(DefragmentParams.CopyStorageMem[i]);
    }

    DefragmentParams.NumCopyStorage = 0;
}

int CarLoader::DefragmentPool() {
    static int loop_number;
    void *allocations[1152];
    void *probe_allocations[33];

    if (this->MayNeedDefragmentation == 0) {
        return 0;
    }

    bGetTicker();

    int num_allocations = bMemoryGetAllocations(CarLoaderMemoryPoolNumber, allocations, 0x480);

    bMemSet(&DefragmentParams, 0, sizeof(DefragmentParams));

    if (num_allocations > 0) {
        for (int i = 0; i < num_allocations; i++) {
            void *allocation = allocations[i];
            int allocation_size = bGetMallocSize(allocation);

            if (DefragmentParams.LargestAllocationSize < allocation_size) {
                DefragmentParams.LargestAllocationSize = allocation_size;
                bSafeStrCpy(DefragmentParams.LargestAllocationName, bGetMallocName(allocation), 0x40);
            }
        }
    }

    if (!this->AllocateDefragmentStorage()) {
        this->FreeDefragmentStorage();
        return 0;
    }

    eWaitUntilRenderingDone();
    gDefragFixer.NumRanges = 0;
    gDefragFixer.MemLow = 0;
    gDefragFixer.MemHigh = 0;

    int num_probe_allocations = 0;
    int upper_allocation = reinterpret_cast<int>(bMalloc(0x80, (CarLoaderMemoryPoolNumber & 0xF) | 0x2000));

    bFree(reinterpret_cast<void *>(upper_allocation));

    for (int i = 0; i < num_allocations; i++) {
        void *allocation = allocations[i];
        int movement_offset = 0;
        int allocation_size = bGetMallocSize(allocation);

        if (upper_allocation < reinterpret_cast<int>(allocation)) {
            DefragmentParams.pAllocation = allocation;
            bStrNCpy(DefragmentParams.AllocationName, bGetMallocName(allocation), 0x3F);

            while (true) {
                void *probe_allocation = bMalloc(1, (CarLoaderMemoryPoolNumber & 0xF) | 0x2000);

                if (reinterpret_cast<int>(probe_allocation) >= upper_allocation - 0x80) {
                    bFree(probe_allocation);
                    movement_offset = reinterpret_cast<int>(probe_allocation) - reinterpret_cast<int>(allocation);
                    ChunkMovementOffset = movement_offset;
                    DefragmentParams.pNewAllocation = probe_allocation;

                    if (!this->DefragmentAllocation(allocation)) {
                        movement_offset = 0;
                    }

                    ChunkMovementOffset = 0;
                    break;
                }

                probe_allocations[num_probe_allocations] = probe_allocation;
                num_probe_allocations++;
            }
        }

        gDefragFixer.Add(allocation, allocation_size, movement_offset);
        loop_number++;
    }

    for (int i = 0; i < num_probe_allocations; i++) {
        bFree(probe_allocations[i]);
    }

    this->FreeDefragmentStorage();
    bMemSet(&DefragmentParams, 0, sizeof(DefragmentParams));
    eFixupReplacementTextureTables();
    RefreshAllRenderInfo(static_cast<CarType>(-1));
    gDefragFixer.MemLow = 0;
    gDefragFixer.NumRanges = 0;
    gDefragFixer.MemHigh = 0;
    this->MayNeedDefragmentation = 0;
    return 1;
}

LoadedWheel::LoadedWheel(RideInfo *ride_info, bool in_fe) {
    this->LoadState = CARLOADSTATE_QUEUED;
    this->LoadStateSkinPerm = CARLOADSTATE_QUEUED;
    this->LoadStateSkinTemp = CARLOADSTATE_QUEUED;
    this->PartNameHash = 0;
    this->TextureBaseNameHash = 0;
    this->pCarPart = 0;

    if (in_fe) {
        this->mMinLodLevel = ride_info->GetMinFELodLevel();
        this->mMaxLodLevel = ride_info->GetMaxFELodLevel();
    } else {
        this->mMinLodLevel = ride_info->GetMinLodLevel();
        this->mMaxLodLevel = ride_info->GetMaxLodLevel();
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
        this->TextureBaseNameHash = car_part->GetTextureNameHash();

        if (car_part->GetCarTypeNameHash() == bStringHash("WHEELS")) {
            for (int model = 0; model < 1; model++) {
                for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                    reinterpret_cast<unsigned int (*)[5]>(this->ModelNameHashes)[model][lod] = car_part->GetModelNameHash(model, lod);
                }
            }

            if (this->TextureBaseNameHash != 0) {
                this->SkinNameHashesPerm[0] = bStringHash("_WHEEL", this->TextureBaseNameHash);
            }
            return;
        }
    }

    this->LoadState = CARLOADSTATE_LOADED;
    this->LoadStateSkinPerm = CARLOADSTATE_LOADED;
    this->LoadStateSkinTemp = CARLOADSTATE_LOADED;
}

LoadedSkin::LoadedSkin(RideInfo *ride_info, int in_front_end, int is_player_skin)
    : pRideInfo(ride_info),           //
      LoadStatePerm(0),               //
      LoadStateTemp(0),               //
      DoneComposite(0),               //
      IsPlayerSkin(is_player_skin),   //
      InFrontEnd(in_front_end),       //
      pLoadedTexturesPack(0),         //
      pLoadedVinylsPack(0),           //
      NumLoadedSkinLayersPerm(0) {
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
    this->NumInstances = 0;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->PrintedLoading = 0;
    this->HighPriority = 0;
    this->pCarTypeInfo = &CarTypeInfoArray[ride_info->Type];
    this->ID = sNextID;
    sNextID = this->ID + 1;
    this->pLoadedCar = &this->TheLoadedCar;
    this->pLoadedWheel = &this->TheLoadedWheel;
    this->pLoadedSkin = &this->TheLoadedSkin;
    bSPrintf(this->Name, "%s(%d)", this->pCarTypeInfo->GetName(), this->ID);
}

void InitCarLoader() {
    LoadedTexturePackSlotPool = bNewSlotPool(0x18, 0x1e, "CarLoadedTexturePackSlotPool", 0);
    LoadedSolidPackSlotPool = bNewSlotPool(0x18, 0x1e, "CarLoadedSolidPackSlotPool", 0);
    LoadedSkinLayerSlotPool = bNewSlotPool(0x10, 0x2ee, "CarLoadedSkinLayerSlotPool", 0);
    LoadedRideInfoSlotPool = bNewSlotPool(0x6d4, 0x14, "CarLoadedRideInfoSlotPool", 0);
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
        NumSlotTypeOverrides = static_cast<unsigned int>(chunk->GetSize() - 0x458) >> 4;

        for (int i = 0; i < 0x116; i++) {
            bEndianSwap32(&DefaultSlotTypeNameTable[i]);
        }

        for (int i = 0; i < NumSlotTypeOverrides; i++) {
            bEndianSwap32(&SlotTypeOverrideTable[i].CarType);
            bEndianSwap32(&SlotTypeOverrideTable[i].SlotId);

            for (int j = 0; j < 2; j++) {
                bEndianSwap32(&SlotTypeOverrideTable[i].LookupType[j]);
            }
        }
    } else if (chunk_id == 0x80034602) {
        bChunk *car_pack_chunk = chunk->GetFirstChunk();
        bChunk *car_string_table_chunk = car_pack_chunk->GetNext();
        bChunk *car_attributetable_table_chunk = car_string_table_chunk->GetNext();
        bChunk *car_attributes_table_chunk = car_attributetable_table_chunk->GetNext();
        bChunk *car_model_table_chunk = car_attributes_table_chunk->GetNext();
        bChunk *car_typename_table_chunk = car_model_table_chunk->GetNext();
        bChunk *car_parts_table_chunk = car_typename_table_chunk->GetNext();
        CarPartPack *car_part_pack = reinterpret_cast<CarPartPack *>(car_pack_chunk->GetData());
        unsigned char *track;
        unsigned char *end_track;

        car_part_pack->EndianSwap();
        car_part_pack->AttributesTable = reinterpret_cast<CarPartAttribute *>(car_attributes_table_chunk->GetData());
        car_part_pack->InPlaceInit();
        car_part_pack->AttributeTableTable = reinterpret_cast<CarPartAttributeTable *>(car_attributetable_table_chunk->GetData());
        car_part_pack->PartsTable = reinterpret_cast<CarPart *>(car_parts_table_chunk->GetData());
        car_part_pack->TypeNameTable = reinterpret_cast<unsigned int *>(car_typename_table_chunk->GetData());
        car_part_pack->ModelTable = reinterpret_cast<CarPartModelTable *>(car_model_table_chunk->GetData());
        car_part_pack->StringTable = reinterpret_cast<const char *>(car_string_table_chunk->GetData());
        car_part_pack->StringTableSize = car_string_table_chunk->GetSize();
        CarPartStringTable = car_part_pack->StringTable;
        CarPartTypeNameHashTable = car_part_pack->TypeNameTable;
        CarPartStringTableSize = car_part_pack->StringTableSize;
        CarPartTypeNameHashTableSize = car_part_pack->NumTypeNames;
        CarPartPartsTable = car_part_pack->PartsTable;
        CarPartModelsTable = car_part_pack->ModelTable;
        MasterCarPartPack = car_part_pack;
        track = reinterpret_cast<unsigned char *>(car_part_pack->AttributeTableTable);
        end_track = track + car_attributetable_table_chunk->GetSize();

        while (track < end_track) {
            reinterpret_cast<CarPartAttributeTable *>(track)->EndianSwap();
            track += reinterpret_cast<CarPartAttributeTable *>(track)->GetByteSize();
        }

        for (unsigned int attribute_index = 0; attribute_index < car_part_pack->NumAttributes; attribute_index++) {
            car_part_pack->AttributesTable[attribute_index].EndianSwap();
        }

        for (unsigned int typename_hash_index = 0; typename_hash_index < car_part_pack->NumTypeNames; typename_hash_index++) {
            bEndianSwap32(&CarPartTypeNameHashTable[typename_hash_index]);
        }

        for (unsigned int model_table_index = 0; model_table_index < car_part_pack->NumModelTables; model_table_index++) {
            CarPartModelTable *model_table =
                reinterpret_cast<CarPartModelTable *>(reinterpret_cast<char *>(car_part_pack->ModelTable) + model_table_index * 0x18);

            model_table->EndianSwap();

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

        for (unsigned int i = 0; i < car_part_pack->NumParts; i++) {
            char *car_part_bytes = reinterpret_cast<char *>(car_part_pack->PartsTable) + i * 0xE;
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
            int upgrade_level = (static_cast<unsigned char>(car_part_bytes[5]) >> 5) - 1;
            int group_number = static_cast<unsigned char>(car_part_bytes[5]) & 0x1F;

            if (upgrade_level < 0) {
                upgrade_level = 0;
            } else if (upgrade_level > 2) {
                upgrade_level = 2;
            }

            if (part_id == 'L') {
                if (brand_name == 0x03437A52) {
                    index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->PaintPart_Metallic[upgrade_level];
                } else if (brand_name < 0x03437A53) {
                    if (brand_name == 0x0000DA27) {
                        index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->PaintPart_Rims[upgrade_level];
                    } else if (brand_name == 0x02DAAB07) {
                        index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->PaintPart_Gloss[upgrade_level];
                    }
                } else if (brand_name == 0x03E871F1) {
                    index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->PaintPart_Vinyl[upgrade_level];
                } else if (brand_name < 0x03E871F2) {
                    if (brand_name == 0x03797533) {
                        index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->PaintPart_Pearl[upgrade_level];
                    }
                } else if (brand_name == 0xD6640DFF) {
                    index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->PaintPart_Caliper[upgrade_level];
                }
            } else if (part_id == 'O') {
                int vinyl_type = ConvertVinylGroupNumberToVinylType(group_number);

                if (vinyl_type == 1) {
                    index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->VinylPart_Hood[upgrade_level];
                } else if (vinyl_type < 2) {
                    if (vinyl_type == 0) {
                        index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->VinylPart_Side[upgrade_level];
                    }
                } else if (vinyl_type == 2) {
                    index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->VinylPart_Body[upgrade_level];
                } else if (vinyl_type == 3) {
                    index0 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->VinylPart_Manufacturer[upgrade_level];
                }

                index1 = &reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB)->VinylPart_All[upgrade_level];
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

        CarPartDatabaseLayout *database = reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB);
        MasterCarPartPack = car_part_pack;
        database->CarPartPackList.AddTail(car_part_pack);
        database->NumPacks += 1;
        database->NumParts += car_part_pack->NumParts;
        database->NumBytes += chunk->GetSize();
    } else {
        return 0;
    }

    return 1;
}

int UnloaderCarInfo(bChunk *chunk) {
    unsigned int chunk_id = chunk->GetID();

    if (chunk_id == 0x34600) {
        CarTypeInfoArray = 0;
        return 1;
    }

    if (chunk_id == 0x80034602) {
        int *chunk_words = reinterpret_cast<int *>(chunk);
        CarPartPack *car_part_pack = reinterpret_cast<CarPartPack *>(chunk_words + 4);
        CarPartDatabaseLayout *database = reinterpret_cast<CarPartDatabaseLayout *>(&CarPartDB);

        car_part_pack->Remove();
        database->NumPacks -= 1;
        database->NumParts -= car_part_pack->NumParts;
        database->NumBytes -= chunk_words[1];
        return 1;
    }

    return 0;
}

void CarLoader::SetLoadingMode(eLoadingMode mode, int two_player_flag) {
    this->InFrontEndFlag = mode == MODE_FRONT_END;
    this->LoadingMode = mode;
    this->TwoPlayerFlag = two_player_flag;
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
    int result = num_memory_entries;

    if (loaded_texture_pack->pStreamingPack != 0) {
        result = loaded_texture_pack->pStreamingPack->GetHeaderMemoryEntries(memory_entries, result);
    }

    return result;
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

        delete this->LoadedTexturePackList.Remove(loaded_texture_pack);
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
        eLoadStreamingTexture(name_hashes, loaded_hashes, LoadedSkinCallbackBridge, reinterpret_cast<unsigned int>(loaded_skin),
                              memory_pool_num);
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
    this->ServiceLoading();
}

void CarLoader::BeginLoading(void (*callback)(unsigned int), unsigned int param) {
    if (this->LoadingInProgress == 0) {
        this->StartLoadingTime = GetDebugRealTime();

        if (callback != 0) {
            this->pCallback = callback;
            this->Param = param;
        }

        if (this->LoadingInProgress == 0) {
            this->ServiceLoading();
        }
    } else if (this->LoadingInProgress == 2) {
        this->LoadingInProgress = 1;
    }
}

void CarLoader::ServiceLoading() {
    int num_unallocated_ride_infos = this->NumLoadedRideInfos - this->NumAllocatedRideInfos;

    if (num_unallocated_ride_infos > 0) {
        int free_slots = bCountFreeSlots(LoadedRideInfoSlotPool);

        if (free_slots < 10) {
            int slots_to_leave = 10 - free_slots;

            if (slots_to_leave > num_unallocated_ride_infos) {
                slots_to_leave = num_unallocated_ride_infos;
            }

            this->UnloadUnallocatedRideInfos(num_unallocated_ride_infos - slots_to_leave);
        }
    }

    QueuedFilePrioritySetter queued_file_priority_setter;

    if (this->LoadAllWheelModels() != 0) {
        return;
    }

    if (this->LoadAllWheelTextures() != 0) {
        return;
    }

    if (this->LoadAllTexturesFromPack("CARS\\TEXTURES.BIN", 1) != 0) {
        return;
    }

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();
         loaded_ride_info != this->LoadedRideInfoList.EndOfList(); loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances > 0 && loaded_ride_info->LoadState != CARLOADSTATE_LOADED) {
            loaded_ride_info->LoadState = CARLOADSTATE_LOADING;

            if (loaded_ride_info->PrintedLoading == 0) {
                loaded_ride_info->PrintedLoading = 1;
            }

            LoadedCar *loaded_car = loaded_ride_info->pLoadedCar;

            if (loaded_car->pLoadedSolidPack->LoadState == CARLOADSTATE_QUEUED) {
                CarTypeInfo *car_type_info = &CarTypeInfoArray[loaded_car->Type];

                this->LoadSolidPack(loaded_car->pLoadedSolidPack, car_type_info->UsageType != 2);
                return;
            }

            if (loaded_car->LoadState == CARLOADSTATE_QUEUED) {
                if (this->LoadCar(loaded_car) != 0) {
                    return;
                }
            }

            LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

            if (loaded_skin->LoadStatePerm == CARLOADSTATE_QUEUED) {
                if (loaded_skin->pLoadedTexturesPack->LoadState == CARLOADSTATE_QUEUED) {
                    if (this->LoadTexturePack(loaded_skin->pLoadedTexturesPack, 1) != 0) {
                        return;
                    }
                }

                if (this->LoadSkin(loaded_skin, 1) != 0) {
                    return;
                }
            }

            if (loaded_skin->LoadStateTemp == CARLOADSTATE_QUEUED) {
                if (this->LoadingMode == MODE_FRONT_END) {
                    this->UnloadAllSkinTemporaries();
                }

                LoadedTexturePack *loaded_vinyls_pack = loaded_skin->pLoadedVinylsPack;

                if (loaded_vinyls_pack != 0) {
                    if (loaded_vinyls_pack->LoadState == CARLOADSTATE_QUEUED) {
                        this->LoadTexturePack(loaded_vinyls_pack, this->LoadingMode == MODE_IN_GAME);
                        return;
                    }
                }

                if (this->LoadSkin(loaded_skin, 0) != 0) {
                    return;
                }
            }

            if (loaded_skin->DoneComposite == 0) {
                this->CompositeSkin(loaded_skin);
            }

            if (this->LoadingMode != MODE_FRONT_END) {
                this->UnloadSkinTemporaries(loaded_skin, 0);
            }

            loaded_ride_info->LoadState = CARLOADSTATE_LOADED;
        }
    }

    if (this->pCallback != 0) {
        this->LoadingInProgress = 2;
        SetDelayedResourceCallback(CallUserCallback, reinterpret_cast<int>(this));
    }
}

void CarLoader::CallUserCallback(int param) {
    CarLoader *car_loader = reinterpret_cast<CarLoader *>(param);

    if (car_loader->LoadingInProgress == 1) {
        car_loader->LoadingDoneCallback();
    } else {
        void (*callback)(unsigned int) = car_loader->pCallback;

        car_loader->LoadingInProgress = 0;
        car_loader->pCallback = 0;
        callback(car_loader->Param);
    }
}

void CarLoader::LoadedSolidPackCallbackBridge(unsigned int param) {
    TheCarLoader.LoadedSolidPackCallback(reinterpret_cast<LoadedSolidPack *>(param));
}

void CarLoader::LoadedSolidPackCallbackBridge(int param) {
    TheCarLoader.LoadedSolidPackCallback(reinterpret_cast<LoadedSolidPack *>(param));
}

void CarLoader::LoadedTexturePackCallbackBridge(unsigned int param) {
    TheCarLoader.LoadedTexturePackCallback(reinterpret_cast<LoadedTexturePack *>(param));
}

void CarLoader::LoadedCarCallbackBridge(unsigned int param) {
    TheCarLoader.LoadedCarCallback(reinterpret_cast<LoadedCar *>(param));
}

void CarLoader::LoadedWheelModelsCallbackBridge(unsigned int) {
    TheCarLoader.LoadedWheelModelsCallback();
}

void CarLoader::LoadedWheelTexturesCallbackBridge(unsigned int) {
    TheCarLoader.LoadedWheelTexturesCallback();
}

void CarLoader::LoadedAllTexturesFromPackCallbackBridge(unsigned int) {
    TheCarLoader.LoadedAllTexturesFromPackCallback();
}

void CarLoader::LoadedSkinCallbackBridge(unsigned int param) {
    TheCarLoader.LoadedSkinCallback(reinterpret_cast<LoadedSkin *>(param));
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

    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != 0 && loaded_skin_layer->NumInstances == 0) {
            if (loaded_skin_layer->LoadState == CARLOADSTATE_LOADED) {
                name_hash_table[num_name_hashes] = loaded_skin_layer->NameHash;
                num_name_hashes++;
            }

            delete this->LoadedSkinLayerList.Remove(loaded_skin_layer);
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
            this->MemoryPoolMem = 0;
            this->MemoryPoolSize = 0;
        }

        if (size != 0) {
            TheTrackStreamer.FlushHibernatingSections();
            TheTrackStreamer.MakeSpaceInPool(size, true);

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
        eLoadStreamingSolid(name_hashes, num_hashes, LoadedCarCallbackBridge, reinterpret_cast<unsigned int>(loaded_car),
                           CarLoaderMemoryPoolNumber);
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
        loaded_solid_pack->pResourceFile->BeginLoading(
            reinterpret_cast<void (*)(void *)>(static_cast<void (*)(int)>(LoadedSolidPackCallbackBridge)), loaded_solid_pack);
    } else {
        CarLoader_MakeSpaceInCarMemoryPool(this, 0x8000, 0, true);
        loaded_solid_pack->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        eLoadStreamingSolidPack(loaded_solid_pack->Filename, LoadedSolidPackCallbackBridge, reinterpret_cast<int>(loaded_solid_pack),
                                CarLoaderMemoryPoolNumber);
        loaded_solid_pack->pStreamingPack = StreamingSolidPackLoader.GetLoadedStreamingPack(loaded_solid_pack->Filename);

        if (loaded_solid_pack->pStreamingPack == 0) {
            LoadedSolidPackCallbackBridge(reinterpret_cast<int>(loaded_solid_pack));
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
