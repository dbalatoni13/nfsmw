#ifndef WORLD_CAR_LOADER_H
#define WORLD_CAR_LOADER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "CarInfo.hpp"
#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

enum CarLoadState {
    CARLOADSTATE_QUEUED = 0,
    CARLOADSTATE_LOADING = 1,
    CARLOADSTATE_LOADED = 2,
};

// total size: 0x18
class LoadedSolidPack : public bTNode<LoadedSolidPack> {
  public:
    const char *Filename;           // offset 0x8, size 0x4
    eStreamingPack *pStreamingPack; // offset 0xC, size 0x4
    ResourceFile *pResourceFile;    // offset 0x10, size 0x4
    short NumInstances;             // offset 0x14, size 0x2
    char LoadState;                 // offset 0x16, size 0x1
};

// total size: 0x18
struct LoadedTexturePack : public bTNode<LoadedTexturePack> {
  public:
    const char *Filename;           // offset 0x8, size 0x4
    short NumInstances;             // offset 0xC, size 0x2
    char LoadState;                 // offset 0xE, size 0x1
    char Pad0;                      // offset 0xF, size 0x1
    int MaxHeaderSize;              // offset 0x10, size 0x4
    eStreamingPack *pStreamingPack; // offset 0x14, size 0x4
};

// total size: 0x10
class LoadedSkinLayer : public bTNode<LoadedSkinLayer> {
  public:
    LoadedSkinLayer(unsigned int name_hash);

    unsigned int NameHash; // offset 0x8, size 0x4
    short NumInstances;    // offset 0xC, size 0x2
    char LoadState;        // offset 0xE, size 0x1
    char pad0;             // offset 0xF, size 0x1
};

// total size: 0x7C
class LoadedWheel : public bTNode<LoadedWheel> {
  public:
    LoadedWheel(RideInfo *ride_info, bool in_fe);

    CarLoadState LoadState;                   // offset 0x8, size 0x4
    CarLoadState LoadStateSkinPerm;           // offset 0xC, size 0x4
    CarLoadState LoadStateSkinTemp;           // offset 0x10, size 0x4
    unsigned int PartNameHash;                // offset 0x14, size 0x4
    unsigned int TextureBaseNameHash;         // offset 0x18, size 0x4
    CarPart *pCarPart;                        // offset 0x1C, size 0x4
    unsigned int ModelNameHashes[5][1];       // offset 0x20, size 0x14
    unsigned int SkinNameHashesPerm[4];       // offset 0x34, size 0x10
    unsigned int SkinNameHashesTemp[4];       // offset 0x44, size 0x10
    LoadedSkinLayer *LoadedSkinLayersPerm[4]; // offset 0x54, size 0x10
    LoadedSkinLayer *LoadedSkinLayersTemp[4]; // offset 0x64, size 0x10
    CARPART_LOD mMinLodLevel;                 // offset 0x74, size 0x4
    CARPART_LOD mMaxLodLevel;                 // offset 0x78, size 0x4
};

// total size: 0x2E0
class LoadedSkin : public bTNode<LoadedSkin> {
  public:
    LoadedSkin(RideInfo *ride_info, int in_front_end, int is_player_skin);
    int GetTextureHashes(unsigned int *texture_hashes, int max_texture_hashes, int perm);

    RideInfo *pRideInfo;                       // offset 0x8, size 0x4
    char LoadStatePerm;                        // offset 0xC, size 0x1
    char LoadStateTemp;                        // offset 0xD, size 0x1
    char DoneComposite;                        // offset 0xE, size 0x1
    int IsPlayerSkin;                          // offset 0x10, size 0x4
    int InFrontEnd;                            // offset 0x14, size 0x4
    LoadedTexturePack *pLoadedTexturesPack;    // offset 0x18, size 0x4
    LoadedTexturePack *pLoadedVinylsPack;      // offset 0x1C, size 0x4
    int NumLoadedSkinLayersPerm;               // offset 0x20, size 0x4
    LoadedSkinLayer *LoadedSkinLayersPerm[87]; // offset 0x24, size 0x15C
    int NumLoadedSkinLayersTemp;               // offset 0x180, size 0x4
    LoadedSkinLayer *LoadedSkinLayersTemp[87]; // offset 0x184, size 0x15C
};

// total size: 0x20
class LoadedCar : public bTNode<LoadedCar> {
  public:
    LoadedCar(RideInfo *ride_info, int in_front_end, int is_two_player);
    int GetModelHashes(unsigned int *model_hashes, int max_model_hashes);

    CarType Type;                      // offset 0x8, size 0x4
    struct RideInfo *pRideInfo;        // offset 0xC, size 0x4
    int InFrontEnd;                    // offset 0x10, size 0x4
    int IsTwoPlayer;                   // offset 0x14, size 0x4
    CarLoadState LoadState;            // offset 0x18, size 0x4
    LoadedSolidPack *pLoadedSolidPack; // offset 0x1C, size 0x4
};

// total size: 0x6D4
class LoadedRideInfo : public bTNode<LoadedRideInfo> {
  public:
    LoadedRideInfo(RideInfo *ride_info, int in_front_end, int is_two_player, int is_player_car);

    int NumInstances;           // offset 0x8, size 0x4
    CarLoadState LoadState;     // offset 0xC, size 0x4
    int HighPriority;           // offset 0x10, size 0x4
    int PrintedLoading;         // offset 0x14, size 0x4
    int IsPlayerCar;            // offset 0x18, size 0x4
    RideInfo TheRideInfo;       // offset 0x1C, size 0x310
    CarTypeInfo *pCarTypeInfo;  // offset 0x32C, size 0x4
    char Name[24];              // offset 0x330, size 0x18
    LoadedCar *pLoadedCar;      // offset 0x348, size 0x4
    LoadedWheel *pLoadedWheel;  // offset 0x34C, size 0x4
    LoadedSkin *pLoadedSkin;    // offset 0x350, size 0x4
    LoadedCar TheLoadedCar;     // offset 0x354, size 0x20
    LoadedWheel TheLoadedWheel; // offset 0x374, size 0x7C
    LoadedSkin TheLoadedSkin;   // offset 0x3F0, size 0x2E0
    int ID;                     // offset 0x6D0, size 0x4

    static int sNextID;
};

// total size: 0x8B0
class CarLoader {
  public:
    CarLoader();

    enum eLoadingMode {
        MODE_FRONT_END = 0,
        MODE_LOADING_GAME = 1,
        MODE_IN_GAME = 2,
    };

    int IsLoadingInProgress() {
        return LoadingInProgress;
    }

  private:
    void SetLoadingMode(eLoadingMode mode, int two_player_flag);
    LoadedSolidPack *AllocateSolidPack(const char *filename);
    LoadedTexturePack *AllocateTexturePack(const char *filename, int max_header_size);
    void UnallocateSolidPack(LoadedSolidPack *loaded_solid_pack);
    int AllocateSkinLayers(unsigned int *name_hash_table, int num_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                           int max_loaded_skin_layers, const char *filename);
    void UnallocateSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers);
    int LoadSkinLayers(unsigned int *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                       int num_loaded_skin_layers);
    void UnallocateTexturePack(LoadedTexturePack *loaded_texture_pack);
    int GetMemoryEntries(LoadedSolidPack *loaded_solid_pack, void **memory_entries, int num_memory_entries);
    int GetMemoryEntries(LoadedTexturePack *loaded_texture_pack, void **memory_entries, int num_memory_entries);
    int GetMemoryEntries(LoadedWheel *loaded_wheel, void **memory_entries, int num_memory_entries);
    int GetMemoryEntries(LoadedSkin *loaded_skin, void **memory_entries, int num_memory_entries);
    int GetMemoryEntries(LoadedSkinLayer *loaded_skin_layer, void **memory_entries, int num_memory_entries);
    int GetMemoryEntries(LoadedCar *loaded_car, void **memory_entries, int num_memory_entries);
    void PrintMemoryUsage(bool on_screen);
    int LoadTexturePack(LoadedTexturePack *loaded_texture_pack, int use_memory_pool);
    void LoadedTexturePackCallback(LoadedTexturePack *loaded_texture_pack);
    int UnloadTexturePack(LoadedTexturePack *loaded_texture_pack);
    int LoadSkin(LoadedSkin *loaded_skin, int load_perm_layers);
    void LoadedSkinCallback(LoadedSkin *loaded_skin);
    int UnloadSolidPack(LoadedSolidPack *loaded_solid_pack);
    int UnloadCar(LoadedCar *loaded_car);
    int UnloadWheel(LoadedWheel *loaded_wheel);
    int UnloadSkinPerms(LoadedSkin *loaded_skin);
    int UnloadSkinTemporaries(LoadedSkin *loaded_skin, int force_unload);
    int UnloadSkin(LoadedSkin *loaded_skin);
    int UnallocateRideInfo(LoadedRideInfo *loaded_ride_info);
    int UnloadRideInfo(LoadedRideInfo *loaded_ride_info, int leave_if_in_mempool);
    void Unload(int handle);
    int IsLoaded(int handle);
    int IsLoaded(LoadedRideInfo *loaded_ride_info);
    void BeginLoading(void (*callback)(unsigned int), unsigned int param);
    void UnloadEverything();
    void UnloadOverflowedResources();
    void UnloadUnallocatedRideInfos(int max_left_unloaded);
    void UnloadAllSkinTemporaries();
    LoadedSolidPack *FindLoadedSolidPack(const char *filename);
    LoadedTexturePack *FindLoadedTexturePack(const char *filename);
    LoadedSkinLayer *FindLoadedSkinLayer(unsigned int name_hash);
    LoadedRideInfo *FindLoadedRideInfo(int handle);
    LoadedRideInfo *FindLoadedRideInfo(RideInfo *ride_info);
    LoadedRideInfo *AllocateRideInfo(RideInfo *ride_info, int is_player_car);
    void CompositeSkin(LoadedSkin *loaded_skin);
    void LoadingDoneCallback();
    void LoadedSolidPackCallback(LoadedSolidPack *loaded_solid_pack);
    void LoadedCarCallback(LoadedCar *loaded_car);
    void LoadedWheelModelsCallback();
    void LoadedWheelTexturesCallback();
    void LoadedAllTexturesFromPackCallback();
    void LoadedSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers);
    int UnloadSkinLayers(unsigned int *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                         int num_loaded_skin_layers);
    void SetMemoryPoolSize(int size);
    int Load(RideInfo *ride_info);
    int LoadCar(LoadedCar *loaded_car);
    int LoadAllWheelModels();
    int LoadAllWheelTextures();
    void LoadSolidPack(LoadedSolidPack *loaded_solid_pack, int stream_solids);
    int LoadAllTexturesFromPack(const char *filename, int load_perm_layers);
    void ServiceLoading();
    bool MakeSpaceInPool(int size);
    int MakeSpaceInCarMemoryPool(int largest_malloc_needed, int amount_free_needed, bool allocating_stream_header_chunks);
    int RemoveSomethingFromCarMemoryPool(bool force_unload);
    int DefragmentPool();

    void (*pCallback)(unsigned int);                 // offset 0x0, size 0x4
    unsigned int Param;                              // offset 0x4, size 0x4
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
    struct LoadedSkinLayer *LoadingSkinLayers[512];  // offset 0xB0, size 0x800
};

void *MoveDefragmentAllocation(void *allocation);
void InitCarLoader();
void CloseCarLoader();

extern CarLoader TheCarLoader;

#endif
