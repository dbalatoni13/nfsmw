#ifndef CARLOADER_HPP
#define CARLOADER_HPP

#define REMOVE_CARPART_ANIMS

#include "CarInfo.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

typedef int32 CarLoaderHandle;

extern int CarLoaderMemoryPoolNumber;

class LoadedTexturePack;
class LoadedSolidPack;
class LoadedSkinLayer;
class LoadedSkin;
class LoadedWheel;
class LoadedCar;
class LoadedRideInfo;

// total size: 0x8B0
class CarLoader {
  public:
    enum eLoadingMode {
        MODE_FRONT_END = 0,
        MODE_LOADING_GAME = 1,
        MODE_IN_GAME = 2,
    };

    CarLoader();

    void SetLoadingMode(eLoadingMode mode, int two_player_flag);

    void SetMemoryPoolSize(int size);

    int GetMemoryPoolSize() {
        return this->MemoryPoolSize;
    }

    bool MakeSpaceInPool(int size);

    void *AllocateUserMemory(int size, const char *debug_name) {
        return bMalloc(size, debug_name, 0, CarLoaderMemoryPoolNumber & 0xF | 0x2000);
    };

    void FreeUserMemory(void *mem);

    CarLoaderHandle Load(RideInfo *ride_info);

    void BeginLoading(void (*callback)(uintptr_t), uintptr_t param);

    int IsLoaded(CarLoaderHandle handle);

    void Unload(CarLoaderHandle handle);

    void UnloadEverything();

    int DefragmentPool();

    void ServiceTweakables();

    int IsLoadingInProgress() {
        return LoadingInProgress;
    }

    void EnsureHasLoaded(CarLoaderHandle handle);

  private:
    LoadedSolidPack *AllocateSolidPack(const char *filename);

    LoadedTexturePack *AllocateTexturePack(const char *filename, int max_header_size);

    int AllocateSkinLayers(uint32 *name_hash_table, int num_name_hashes, LoadedSkinLayer **loaded_skin_layer_table, int max_loaded_skin_layers,
                           const char *filename);

    LoadedRideInfo *AllocateRideInfo(RideInfo *ride_info, int is_player_car);

    void UnallocateSolidPack(LoadedSolidPack *loaded_solid_pack);

    void UnallocateTexturePack(LoadedTexturePack *loaded_texture_pack);

    void UnallocateSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers);

    int UnallocateRideInfo(LoadedRideInfo *loaded_ride_info);

    void ServiceLoading();

    void LoadSolidPack(LoadedSolidPack *loaded_solid_pack, int stream_solids);

    int LoadTexturePack(LoadedTexturePack *loaded_texture_pack, int use_memory_pool);

    int LoadCar(LoadedCar *loaded_car);

    int LoadAllWheelModels();

    int LoadAllWheelTextures();

    int LoadAllTexturesFromPack(const char *filename, int load_perm_layers);

    int LoadSkin(LoadedSkin *loaded_skin, int load_perm_layers);

    int LoadSkinLayers(uint32 *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers);

    int UnloadSolidPack(LoadedSolidPack *loaded_solid_pack);

    int UnloadTexturePack(LoadedTexturePack *loaded_texture_pack);

    int UnloadCar(LoadedCar *loaded_car);

    int UnloadWheel(LoadedWheel *loaded_wheel);

    int UnloadSkinTemporaries(LoadedSkin *loaded_skin, int force_unload);

    int UnloadSkinPerms(LoadedSkin *loaded_skin);

    int UnloadSkin(LoadedSkin *loaded_skin);

    int UnloadSkinLayers(uint32 *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers);

    void LoadingDoneCallback();

    void LoadedTrackStreamerCallback();

    void LoadedSolidPackCallback(LoadedSolidPack *loaded_solid_pack);

    void LoadedTexturePackCallback(LoadedTexturePack *loaded_texture_pack);

    void LoadedCarCallback(LoadedCar *loaded_car);

    void LoadedWheelModelsCallback();

    void LoadedWheelTexturesCallback();

    void LoadedAllTexturesFromPackCallback();

    void LoadedSkinCallback(LoadedSkin *loaded_skin);

    void LoadedSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers);

    static void LoadedTrackStreamerCallbackBridge(intptr_t param);

    static void LoadedTexturePackCallbackBridge(uintptr_t param);

    static void LoadedCarCallbackBridge(uintptr_t param);

    static void LoadedWheelModelsCallbackBridge(uintptr_t param);

    static void LoadedWheelTexturesCallbackBridge(uintptr_t param);

    static void LoadedAllTexturesFromPackCallbackBridge(uintptr_t param);

    static void LoadedSkinCallbackBridge(uintptr_t param);

    static void LoadedSolidPackCallbackBridge(uintptr_t param);

    static void LoadedSolidPackCallbackBridge(intptr_t param);

    LoadedSolidPack *FindLoadedSolidPack(const char *filename);

    LoadedTexturePack *FindLoadedTexturePack(const char *filename);

    LoadedSkinLayer *FindLoadedSkinLayer(uint32 name_hash);

    LoadedRideInfo *FindLoadedRideInfo(CarLoaderHandle handle);

    LoadedRideInfo *FindLoadedRideInfo(RideInfo *ride_info);

    int IsLoaded(LoadedRideInfo *loaded_ride_info);

    int IsAddressInMemoryPool(void *address);

    int IsInMemoryPool(LoadedTexturePack *loaded_texture_pack);

    int IsInMemoryPool(LoadedSolidPack *loaded_solid_pack);

    int IsCarInMemoryPool(LoadedCar *loaded_car);

    int IsSkinInMemoryPool(LoadedSkin *loaded_skin);

    int IsSkinLayerInMemoryPool(LoadedSkinLayer *loaded_skin_layer);

    int GetMemoryEntries(LoadedTexturePack *loaded_texture_pack, void **memory_entries, int num_memory_entries);

    int GetMemoryEntries(LoadedSolidPack *loaded_solid_pack, void **memory_entries, int num_memory_entries);

    int GetMemoryEntries(LoadedWheel *loaded_wheel, void **memory_entries, int num_memory_entries);

    int GetMemoryEntries(LoadedSkin *loaded_skin, void **memory_entries, int num_memory_entries);

    int GetMemoryEntries(LoadedSkinLayer *loaded_skin_layer, void **memory_entries, int num_memory_entries);

    int GetMemoryEntries(LoadedCar *loaded_car, void **memory_entries, int num_memory_entries);

    void CompositeSkin(LoadedSkin *loaded_skin);

    void PrintInventory();

    void PrintMemoryUsage(bool on_screen);

    int GetMemoryRequired(LoadedRideInfo *loaded_ride_info);

    char *GetPrintHeader();

    int RemoveSomethingFromCarMemoryPool(bool force_unload);

    void UnloadUnallocatedRideInfos(int max_left_unloaded);

    void UnloadAllSkinTemporaries();

    int MakeSpaceInCarMemoryPool(int largest_malloc_needed, int amount_free_needed, bool allocating_stream_header_chunks);

    void UnloadOverflowedResources();

    int UnloadRideInfo(LoadedRideInfo *loaded_ride_info, int leave_if_in_mempool);

    bool DefragmentAllocation(void *allocation);

    static void CallUserCallback(intptr_t param);

    bool AllocateDefragmentStorage();

    void FreeDefragmentStorage();

    void (*pCallback)(uint32);                       // offset 0x0, size 0x4
    uint32 Param;                                    // offset 0x4, size 0x4
    eLoadingMode LoadingMode;                        // offset 0x8, size 0x4
    int InFrontEndFlag;                              // offset 0xC, size 0x4
    int TwoPlayerFlag;                               // offset 0x10, size 0x4
    int LoadingInProgress;                           // offset 0x14, size 0x4
    float StartLoadingTime;                          // offset 0x18, size 0x4
    int NumLoadedRideInfos;                          // offset 0x1C, size 0x4
    int NumAllocatedRideInfos;                       // offset 0x20, size 0x4
    int MayNeedDefragmentation;                      // offset 0x24, size 0x4
    bTList<LoadedSolidPack> LoadedSolidPackList;     // offset 0x28, size 0x8
    bTList<LoadedTexturePack> LoadedTexturePackList; // offset 0x30, size 0x8
    bTList<LoadedSkinLayer> LoadedSkinLayerList;     // offset 0x38, size 0x8
    bTList<LoadedWheel> LoadedWheelList;             // offset 0x40, size 0x8
    bTList<LoadedSkin> LoadedSkinList;               // offset 0x48, size 0x8
    bTList<LoadedCar> LoadedCarList;                 // offset 0x50, size 0x8
    bTList<LoadedRideInfo> LoadedRideInfoList;       // offset 0x58, size 0x8
    void *MemoryPoolMem;                             // offset 0x60, size 0x4
    int MemoryPoolSize;                              // offset 0x64, size 0x4
    int NumSpongeAllocations;                        // offset 0x68, size 0x4
    void *SpongeAllocations[16];                     // offset 0x6C, size 0x40
    int NumLoadingSkinLayers;                        // offset 0xAC, size 0x4
    LoadedSkinLayer *LoadingSkinLayers[512];         // offset 0xB0, size 0x800
};

void *MoveDefragmentAllocation(void *allocation);
void InitCarLoader();
void CloseCarLoader();
int CarInfo_GetMaxCompositingBufferSize();

extern CarLoader TheCarLoader;
extern int UsePrecompositeVinyls;

#endif
