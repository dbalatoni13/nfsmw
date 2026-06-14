#include "Speed/Indep/Src/World/CarLoader.hpp"
#include "Speed/Indep/Src/Misc/QueuedFile.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/World/CarSkin.hpp"
#include "Speed/Indep/Src/Ecstasy/DefragFixer.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/eStreamingPack.hpp"
#include "Speed/Indep/Src/World/CarInfo.hpp"
#include "Speed/Indep/Src/World/VehicleRenderConn.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/bWare/Inc/bSlotPool.hpp"
#include "Speed/Indep/bWare/Inc/bTypes.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bMemory.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/World/CarRender.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"

int CarLoaderMemoryPoolNumber = -1;

enum CarLoadState {
    CARLOADSTATE_QUEUED = 0,
    CARLOADSTATE_LOADING = 1,
    CARLOADSTATE_LOADED = 2,
};

// TODO use these
static const int MaxCarLoaderAllocations = 3072;
static const unsigned int kMaxModelHashes = 3000;

static const int PrintCarLoaderOld = 0;
static const int PrintCarLoader = 0;
static const int PrintCarLoaderDetail = 0;
static const int PrintCarLoaderInventory = 0;
static const int PrintCarLoaderSkinLayers = 0;

static const int PrintCarLoaderMemory = 0;
static const int PrintCarLoaderMemoryDetail = 0;
static const int PrintCarLoaderMemoryOnLoading = 0;
int DisplayCarLoaderMemoryOnScreen = 0;
static const int CarMemoryScreenPrintX = -115;
static const int CarMemoryScreenPrintY = -195;
static const int ForceCarReloading = 0;
static const int ForceUnloadSkinTemporaries = 0;

static const int KeepMinimimAmountOfSkinTemporaries = 1;
static const int CarLoaderAgressiveDefragmentation = 0;

static const int CarLoaderMemorySpongeAllocSize = 102400;
static const int CarLoaderMemorySpongeNumAllocs = 0;

static const int DisableCarLoaderDefragmentation = 0;

CarLoader TheCarLoader;

int UsePrecompositeVinyls = 0;
SlotPool *LoadedTexturePackSlotPool = nullptr;
SlotPool *LoadedSolidPackSlotPool = nullptr;
SlotPool *LoadedSkinLayerSlotPool = nullptr;
SlotPool *LoadedRideInfoSlotPool = nullptr;

// total size: 0x18
class LoadedTexturePack : public bTNode<LoadedTexturePack> {
  public:
    LoadedTexturePack(const char *filename, int max_header_size);
    ~LoadedTexturePack();

    USE_SLOTALLOC(LoadedTexturePackSlotPool);

    const char *GetName();

    const char *Filename;           // offset 0x8, size 0x4
    int16 NumInstances;             // offset 0xC, size 0x2
    char LoadState;                 // offset 0xE, size 0x1
    char Pad0;                      // offset 0xF, size 0x1
    int32 MaxHeaderSize;            // offset 0x10, size 0x4
    eStreamingPack *pStreamingPack; // offset 0x14, size 0x4
};

// total size: 0x18
class LoadedSolidPack : public bTNode<LoadedSolidPack> {
  public:
    LoadedSolidPack(const char *filename);
    ~LoadedSolidPack();

    USE_SLOTALLOC(LoadedSolidPackSlotPool);

    const char *GetName();

    const char *Filename;           // offset 0x8, size 0x4
    eStreamingPack *pStreamingPack; // offset 0xC, size 0x4
    ResourceFile *pResourceFile;    // offset 0x10, size 0x4
    int16 NumInstances;             // offset 0x14, size 0x2
    int8 LoadState;                 // offset 0x16, size 0x1
};

// total size: 0x10
class LoadedSkinLayer : public bTNode<LoadedSkinLayer> {
  public:
    LoadedSkinLayer(unsigned int name_hash);

    USE_SLOTALLOC(LoadedSkinLayerSlotPool);

    uint32 NameHash;    // offset 0x8, size 0x4
    int16 NumInstances; // offset 0xC, size 0x2
    char LoadState;     // offset 0xE, size 0x1
    char pad0;          // offset 0xF, size 0x1
};

// total size: 0x2E0
class LoadedSkin : public bTNode<LoadedSkin> {
  public:
    LoadedSkin(RideInfo *ride_info, int in_front_end, int is_player_skin);
    int GetTextureHashes(uint32 *texture_hashes, int max_texture_hashes, int perm);

    int IsLoaded() {
        return static_cast<int>(this->LoadStatePerm == CARLOADSTATE_LOADED && this->LoadStateTemp == CARLOADSTATE_LOADED && this->DoneComposite != 0);
    }

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

// total size: 0x7C
class LoadedWheel : public bTNode<LoadedWheel> {
  public:
    LoadedWheel(RideInfo *ride_info, bool in_fe);

    CARPART_LOD GetMinLodLevel() const {
        return mMinLodLevel;
    };

    CARPART_LOD GetMaxLodLevel() const {
        return mMaxLodLevel;
    };

    void SetLodLevel(CARPART_LOD min, CARPART_LOD max);

    CarLoadState LoadState;                   // offset 0x8, size 0x4
    CarLoadState LoadStateSkinPerm;           // offset 0xC, size 0x4
    CarLoadState LoadStateSkinTemp;           // offset 0x10, size 0x4
    uint32 PartNameHash;                      // offset 0x14, size 0x4
    uint32 TextureBaseNameHash;               // offset 0x18, size 0x4
    CarPart *pCarPart;                        // offset 0x1C, size 0x4
    uint32 ModelNameHashes[1][5];             // offset 0x20, size 0x14
    uint32 SkinNameHashesPerm[4];             // offset 0x34, size 0x10
    uint32 SkinNameHashesTemp[4];             // offset 0x44, size 0x10
    LoadedSkinLayer *LoadedSkinLayersPerm[4]; // offset 0x54, size 0x10
    LoadedSkinLayer *LoadedSkinLayersTemp[4]; // offset 0x64, size 0x10
  private:
    CARPART_LOD mMinLodLevel; // offset 0x74, size 0x4
    CARPART_LOD mMaxLodLevel; // offset 0x78, size 0x4
};

// total size: 0x20
class LoadedCar : public bTNode<LoadedCar> {
  public:
    LoadedCar(RideInfo *ride_info, int in_front_end, int is_two_player);

    int ShouldWeStream() {
        return static_cast<int>(GetCarTypeInfo(this->Type)->GetCarUsageType() != 2);
    }

    int GetModelHashes(unsigned int *model_hashes, int max_model_hashes);

    CarType Type;                      // offset 0x8, size 0x4
    RideInfo *pRideInfo;               // offset 0xC, size 0x4
    int InFrontEnd;                    // offset 0x10, size 0x4
    int IsTwoPlayer;                   // offset 0x14, size 0x4
    CarLoadState LoadState;            // offset 0x18, size 0x4
    LoadedSolidPack *pLoadedSolidPack; // offset 0x1C, size 0x4
};

LoadedTexturePack::LoadedTexturePack(const char *filename, int max_header_size)
    : Filename(bAllocateSharedString(filename)), //
      NumInstances(0),                           //
      LoadState(0),                              //
      Pad0(0),                                   //
      MaxHeaderSize(max_header_size),            //
      pStreamingPack(nullptr) {
    if (bFileSize(this->Filename) == 0) {
        this->LoadState = 2;
    }
}

LoadedTexturePack::~LoadedTexturePack() {
    bFreeSharedString(this->Filename);
}

LoadedSolidPack::LoadedSolidPack(const char *filename)
    : Filename(bAllocateSharedString(filename)), //
      NumInstances(0),                           //
      LoadState(0),                              //
      pStreamingPack(nullptr),                   //
      pResourceFile(nullptr) {}

LoadedSolidPack::~LoadedSolidPack() {
    bFreeSharedString(this->Filename);
}

LoadedSkinLayer::LoadedSkinLayer(unsigned int name_hash) {
    this->NameHash = name_hash;
    this->NumInstances = 0;
    this->LoadState = 0;
    this->pad0 = 0;
}

LoadedSkin::LoadedSkin(RideInfo *ride_info, int in_front_end, int is_player_skin)
    : pRideInfo(ride_info),         //
      LoadStatePerm(0),             //
      LoadStateTemp(0),             //
      DoneComposite(0),             //
      IsPlayerSkin(is_player_skin), //
      InFrontEnd(in_front_end),     //
      pLoadedTexturesPack(nullptr), //
      pLoadedVinylsPack(nullptr),   //
      NumLoadedSkinLayersPerm(0) {
    bMemSet(this->LoadedSkinLayersPerm, 0, sizeof(this->LoadedSkinLayersPerm));
    this->NumLoadedSkinLayersTemp = 0;
    bMemSet(this->LoadedSkinLayersTemp, 0, sizeof(this->LoadedSkinLayersTemp));
}

int LoadedSkin::GetTextureHashes(unsigned int *texture_hashes, int max_texture_hashes, int perm) {
    UsedCarTextureInfo used_texture_info;

    bMemSet(texture_hashes, 0, max_texture_hashes * sizeof(*texture_hashes));
    GetUsedCarTextureInfo(&used_texture_info, this->pRideInfo, this->InFrontEnd);

    int num_hashes = used_texture_info.NumTexturesToLoadTemp;
    unsigned int *hashes = used_texture_info.TexturesToLoadTemp;

    if (perm != 0) {
        hashes = used_texture_info.TexturesToLoadPerm;
        num_hashes = used_texture_info.NumTexturesToLoadPerm;
    }

    bMemCpy(texture_hashes, hashes, num_hashes << 2);
    return num_hashes;
}

LoadedWheel::LoadedWheel(RideInfo *ride_info, bool in_fe) {
    this->LoadState = CARLOADSTATE_QUEUED;
    this->LoadStateSkinPerm = CARLOADSTATE_QUEUED;
    this->LoadStateSkinTemp = CARLOADSTATE_QUEUED;
    this->PartNameHash = 0;
    this->TextureBaseNameHash = 0;
    this->pCarPart = nullptr;

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
    if (car_part != nullptr) {
        this->pCarPart = car_part;
        this->PartNameHash = car_part->GetPartNameHash();
        this->TextureBaseNameHash = car_part->GetTextureNameHash();

        if (car_part->GetCarTypeNameHash() == bStringHash("WHEELS")) {
            for (int model = 0; model < 1; model++) {
                for (int lod = this->mMinLodLevel; lod <= this->mMaxLodLevel; lod++) {
                    this->ModelNameHashes[model][lod] = car_part->GetModelNameHash(model, lod);
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

LoadedCar::LoadedCar(RideInfo *ride_info, int in_front_end, int is_two_player) {
    this->LoadState = CARLOADSTATE_QUEUED;
    this->pRideInfo = ride_info;
    this->Type = ride_info->Type;
    this->InFrontEnd = in_front_end;
    this->IsTwoPlayer = is_two_player;
    this->pLoadedSolidPack = nullptr;
}

int GatherModelHashes(RideInfo *ride_info, unsigned int *model_hashes, int num_hashes, int max_model_hashes, int first, int last) {
    ProfileNode profile_node("TODO", 0);
    CARPART_LOD minLodLevel = ride_info->GetMinLodLevel();
    CARPART_LOD maxLodLevel = ride_info->GetMaxLodLevel();

    for (int slotIx = first; slotIx < last; slotIx++) {
        for (CarPart *part = CarPartDB.NewGetFirstCarPart(ride_info->Type, slotIx, 0, -1); part != 0;
             part = CarPartDB.NewGetNextCarPart(part, ride_info->Type, slotIx, 0, -1)) {
            for (int modelIx = 0; modelIx < 1; modelIx++) {
                for (int lod = minLodLevel; lod <= maxLodLevel; lod++) {
                    unsigned int model_name_hash = 0;

                    if (part != nullptr) {
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

int LoadedCar::GetModelHashes(unsigned int *model_hashes, int max_model_hashes) {
    ProfileNode profile_node("TODO", 0);

    bMemSet(model_hashes, 0, max_model_hashes << 2);

    RideInfo *ride_info = this->pRideInfo;

    CARPART_LOD minLodLevel = ride_info->GetMinLodLevel();
    CARPART_LOD maxLodLevel = ride_info->GetMaxLodLevel();

    if (this->InFrontEnd != 0) {
        minLodLevel = ride_info->GetMinFELodLevel();
        maxLodLevel = ride_info->GetMaxFELodLevel();
    }

    int num_hashes = 0;

    for (int slot_id = 0; slot_id < CARSLOTID_MODEL_NUM; slot_id++) {
        CarPart *car_part = ride_info->GetPart(slot_id);
        int num_lods;
        CARPART_LOD currentSlotMinLodLevel = minLodLevel;
        CARPART_LOD currentSlotMaxLodLevel = maxLodLevel;

        ride_info->GetSpecialLODRangeForCarSlot(slot_id, &currentSlotMinLodLevel, &currentSlotMaxLodLevel, this->InFrontEnd != 0);

        for (int model = 0; model < 1; model++) {
            for (int lod = currentSlotMinLodLevel; lod <= currentSlotMaxLodLevel; lod++) {
                if (car_part != nullptr) {
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

    unsigned char bitfield[512];
    int new_num_hashes =
        GatherModelHashes(ride_info, model_hashes, num_hashes, max_model_hashes, CARSLOTID_DAMAGE0_FRONT, CARSLOTID_DAMAGE0_REARRIGHT);
    num_hashes = new_num_hashes;
    new_num_hashes = GatherModelHashes(ride_info, model_hashes, num_hashes, max_model_hashes, 1, CARSLOTID_BODY);
    num_hashes = new_num_hashes;

    if (num_hashes > max_model_hashes) {
        num_hashes = max_model_hashes;
    }

    bMemSet(bitfield, 0, sizeof(bitfield));
    profile_node.Begin("TODO", 0);

    int num_hits = 0;

    for (int n = 0; n < num_hashes; n++) {
        bool duplicate = false;
        unsigned int hash = model_hashes[n];
        int bit = (hash >> 3) & 0x1FF;

        if (((bitfield[bit] >> (hash & 7)) & 1U) != 0) {
            int i = 0;

            while (i < num_hits && model_hashes[i] != hash) {
                i++;
            }

            duplicate = i != num_hits;
        } else {
            bitfield[bit] |= 1 << (hash & 7);
        }

        if (!duplicate) {
            model_hashes[num_hits] = hash;
            num_hits++;
        }
    }

    return num_hits;
}

// total size: 0x6D4
class LoadedRideInfo : public bTNode<LoadedRideInfo> {
  public:
    LoadedRideInfo(RideInfo *ride_info, int in_front_end, int is_two_player, int is_player_car);

    USE_SLOTALLOC(LoadedRideInfoSlotPool);

    RideInfo *GetRideInfo() {
        return &TheRideInfo;
    };

    char *GetName() {
        return Name;
    };

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

int LoadedRideInfo::sNextID = 1;

LoadedRideInfo::LoadedRideInfo(RideInfo *ride_info, int in_front_end, int is_two_player, int is_player_car)
    : TheRideInfo(*ride_info),                                 //
      TheLoadedCar(&TheRideInfo, in_front_end, is_two_player), //
      TheLoadedWheel(&TheRideInfo, in_front_end != 0),         //
      TheLoadedSkin(&TheRideInfo, in_front_end, is_player_car) {
    this->NumInstances = 0;
    this->LoadState = CARLOADSTATE_QUEUED;
    this->PrintedLoading = 0;
    this->HighPriority = 0;
    this->pCarTypeInfo = GetCarTypeInfo(ride_info->Type);
    this->ID = sNextID++;
    this->pLoadedCar = &this->TheLoadedCar;
    this->pLoadedWheel = &this->TheLoadedWheel;
    this->pLoadedSkin = &this->TheLoadedSkin;
    bSPrintf(this->Name, "%s(%d)", this->pCarTypeInfo->GetName(), this->ID);
}

void InitCarLoader() {
    LoadedTexturePackSlotPool = bNewSlotPool(sizeof(LoadedTexturePack), 30, "CarLoadedTexturePackSlotPool", 0);
    LoadedSolidPackSlotPool = bNewSlotPool(sizeof(LoadedSolidPack), 30, "CarLoadedSolidPackSlotPool", 0);
    LoadedSkinLayerSlotPool = bNewSlotPool(sizeof(LoadedSkinLayer), 750, "CarLoadedSkinLayerSlotPool", 0);
    LoadedRideInfoSlotPool = bNewSlotPool(sizeof(LoadedRideInfo), 20, "CarLoadedRideInfoSlotPool", 0);
}

// STRIPPED
void CloseCarLoader() {}

CarLoader::CarLoader() {
    this->LoadingInProgress = 0;
    this->LoadingMode = MODE_FRONT_END;
    this->InFrontEndFlag = 0;
    this->TwoPlayerFlag = 0;
    this->StartLoadingTime = 0.0f;
    this->NumLoadedRideInfos = 0;
    this->NumAllocatedRideInfos = 0;
    this->MayNeedDefragmentation = 0;
    this->pCallback = nullptr;
    this->MemoryPoolMem = nullptr;
    this->MemoryPoolSize = 0;
    this->NumSpongeAllocations = 0;
    this->NumLoadingSkinLayers = 0;
}

void CarLoader::SetLoadingMode(eLoadingMode mode, int two_player_flag) {
    this->InFrontEndFlag = static_cast<int>(mode == MODE_FRONT_END);
    this->LoadingMode = mode;
    this->TwoPlayerFlag = two_player_flag;
}

// UNSOLVED, scheduling
// TODO dwarf
void CarLoader::SetMemoryPoolSize(int size) {
    if (this->MemoryPoolSize != size) {
        if (this->MemoryPoolSize != 0) {
            for (int i = 0; i < this->NumSpongeAllocations; i++) {
                bFree(this->SpongeAllocations[i]);
            }

            this->NumSpongeAllocations = 0;
            this->UnloadUnallocatedRideInfos(0);

            if (this->LoadedRideInfoList.GetHead() != this->LoadedRideInfoList.EndOfList()) {
                return;
            }

            bCloseMemoryPool(CarLoaderMemoryPoolNumber);
            bFree(this->MemoryPoolMem);
            this->MemoryPoolMem = nullptr;
            this->MemoryPoolSize = 0;
        }

        if (size != 0) {
            TheTrackStreamer.FlushHibernatingSections();
            TheTrackStreamer.MakeSpaceInPool(size, true);

            this->MemoryPoolMem = bMalloc(size, "CarLoaderPool", 0, 7);
            this->MemoryPoolSize = size;
            CarLoaderMemoryPoolNumber = bGetFreeMemoryPoolNum();

            bInitMemoryPool(CarLoaderMemoryPoolNumber, this->MemoryPoolMem, this->MemoryPoolSize, "Cars");
            bSetMemoryPoolDebugFill(CarLoaderMemoryPoolNumber, false);
            bSetMemoryPoolTopDirection(CarLoaderMemoryPoolNumber, true);
            this->NumSpongeAllocations = 0;
        }
    }
}

// STRIPPED
int CarLoader::IsAddressInMemoryPool(void *address) {}

// STRIPPED
int CarLoader::IsInMemoryPool(LoadedTexturePack *loaded_texture_pack) {}

int CarLoader::GetMemoryEntries(LoadedTexturePack *loaded_texture_pack, void **memory_entries, int num_memory_entries) {
    // TODO get rid of this temp
    int result = num_memory_entries;
    if (loaded_texture_pack->pStreamingPack != nullptr) {
        result = loaded_texture_pack->pStreamingPack->GetHeaderMemoryEntries(memory_entries, num_memory_entries);
    }

    return result;
}

// STRIPPED
int CarLoader::IsInMemoryPool(LoadedSolidPack *loaded_solid_pack) {}

int CarLoader::GetMemoryEntries(LoadedSolidPack *loaded_solid_pack, void **memory_entries, int num_memory_entries) {
    if (loaded_solid_pack->pStreamingPack != nullptr) {
        num_memory_entries = loaded_solid_pack->pStreamingPack->GetHeaderMemoryEntries(memory_entries, num_memory_entries);
    }

    if (loaded_solid_pack->pResourceFile != nullptr) {
        memory_entries[num_memory_entries] = loaded_solid_pack->pResourceFile->GetMemory();
        num_memory_entries++;
    }

    return num_memory_entries;
}

// STRIPPED
void CarLoader::PrintInventory() {}

// TODO fix dwarf
void CarLoader::PrintMemoryUsage(bool on_screen) {
    const float kUpdateFreq = 2.5f;
    const float kUpdateOverlap = 0.0f;
    static float lastTime = 0.0f;

    float elapsedTime = GetDebugRealTime() - lastTime;

    if (elapsedTime >= kUpdateFreq) {
        void *allocation_table[1152];
        int num_allocations;
        char allocation_usage_table[1024];
        int total_memory_used = 0;
        int total_memory_used2;
        int screen_x;
        int screen_y;

        lastTime = GetDebugRealTime();
        num_allocations = bMemoryGetAllocations(CarLoaderMemoryPoolNumber, allocation_table, 1152);
        bMemSet(allocation_usage_table, 0, 0x400);

        for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetTail(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
             loaded_ride_info = loaded_ride_info->GetPrev()) {
            if (!on_screen) {
                this->IsLoaded(loaded_ride_info);
            }

            void *memory_entries[256];
            int num_memory_entries = this->GetMemoryEntries(loaded_ride_info->pLoadedCar, memory_entries, 0);
            int memory_used = 0;
            int memory_required = 0;
            int memory_budget;

            num_memory_entries = this->GetMemoryEntries(loaded_ride_info->pLoadedSkin, memory_entries, num_memory_entries);
            num_memory_entries = this->GetMemoryEntries(loaded_ride_info->pLoadedWheel, memory_entries, num_memory_entries);

            for (int n = 0; n < num_memory_entries; n++) {
                void *memory_entry = memory_entries[n];
                int memory_entry_size = bGetMallocSize(memory_entry);
                int i;
                for (i = 0; i < num_allocations; i++) {
                    if (memory_entry == allocation_table[i]) {
                        memory_used += memory_entry_size;

                        if (allocation_usage_table[i] == 0) {
                            memory_required += memory_entry_size;
                        }

                        allocation_usage_table[i]++;
                        break;
                    }
                }
            }

            if (loaded_ride_info->NumInstances > 0) {
                total_memory_used += memory_required;
            }

            memory_budget = CarInfo_GetResourceCost(loaded_ride_info->TheRideInfo.Type, loaded_ride_info->IsPlayerCar != 0, this->TwoPlayerFlag != 0);
        }

        for (int allocation_num = 0; allocation_num < num_allocations; allocation_num++) {
            void *allocation = allocation_table[allocation_num];

            for (int n = 0; n < this->NumSpongeAllocations; n++) {
                if (this->SpongeAllocations[n] == allocation) {
                    allocation_usage_table[allocation_num] = 1;
                }
            }
        }
    }
}

// STRIPPED
int CarLoader::GetMemoryRequired(LoadedRideInfo *loaded_ride_info) {}

// STRIPPED
void CarLoader::ServiceTweakables() {}

// STRIPPED
char *CarLoader::GetPrintHeader() {}

LoadedSolidPack *CarLoader::AllocateSolidPack(const char *filename) {
    LoadedSolidPack *loaded_solid_pack = this->FindLoadedSolidPack(filename);

    if (loaded_solid_pack == nullptr) {
        loaded_solid_pack = new LoadedSolidPack(filename);
        this->LoadedSolidPackList.AddTail(loaded_solid_pack);
    }

    loaded_solid_pack->NumInstances++;
    return loaded_solid_pack;
}

void CarLoader::UnallocateSolidPack(LoadedSolidPack *loaded_solid_pack) {
    loaded_solid_pack->NumInstances--;
}

void CarLoader::LoadSolidPack(LoadedSolidPack *loaded_solid_pack, int stream_solids) {
    if (stream_solids != 0) {
        this->MakeSpaceInCarMemoryPool(32768, 0, true);
        loaded_solid_pack->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        eLoadStreamingSolidPack(loaded_solid_pack->Filename, LoadedSolidPackCallbackBridge, reinterpret_cast<uintptr_t>(loaded_solid_pack),
                                CarLoaderMemoryPoolNumber);
        loaded_solid_pack->pStreamingPack = StreamingSolidPackLoader.GetLoadedStreamingPack(loaded_solid_pack->Filename);

        if (loaded_solid_pack->pStreamingPack == nullptr) {
            LoadedSolidPackCallbackBridge(reinterpret_cast<unsigned int>(loaded_solid_pack));
        }
    } else {
        loaded_solid_pack->pResourceFile = CreateResourceFile(loaded_solid_pack->Filename, RESOURCE_FILE_CAR, 0, 0, 0);

        int file_size = bFileSize(loaded_solid_pack->Filename);
        int pool = 0;

        if (this->MakeSpaceInCarMemoryPool(file_size, 0, false) != 0) {
            pool = CarLoaderMemoryPoolNumber;
        }

        loaded_solid_pack->pResourceFile->SetAllocationParams((pool & 0xF) | 0x2000, loaded_solid_pack->Filename);
        loaded_solid_pack->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        loaded_solid_pack->pResourceFile->BeginLoading(LoadedSolidPackCallbackBridge, reinterpret_cast<intptr_t>(loaded_solid_pack));
    }
}

// STRIPPED
void CarLoader::LoadedTrackStreamerCallback() {}

void CarLoader::LoadedSolidPackCallback(LoadedSolidPack *loaded_solid_pack) {
    loaded_solid_pack->LoadState = CARLOADSTATE_LOADED;
    this->LoadingDoneCallback();
}

int CarLoader::UnloadSolidPack(LoadedSolidPack *loaded_solid_pack) {
    if (loaded_solid_pack->NumInstances == 0) {
        if (loaded_solid_pack->LoadState == CARLOADSTATE_LOADED) {
            if (loaded_solid_pack->pResourceFile != nullptr) {
                UnloadResourceFile(loaded_solid_pack->pResourceFile);
                loaded_solid_pack->pResourceFile = nullptr;
            } else {
                eUnloadStreamingSolidPack(loaded_solid_pack->Filename);
            }
        }

        delete this->LoadedSolidPackList.Remove(loaded_solid_pack);
        return 1;
    }

    return 0;
}

LoadedTexturePack *CarLoader::AllocateTexturePack(const char *filename, int max_header_size) {
    LoadedTexturePack *loaded_texture_pack = this->FindLoadedTexturePack(filename);

    if (loaded_texture_pack == nullptr) {
        loaded_texture_pack = new LoadedTexturePack(filename, max_header_size);
        this->LoadedTexturePackList.AddTail(loaded_texture_pack);
    }

    loaded_texture_pack->NumInstances++;
    return loaded_texture_pack;
}

void CarLoader::UnallocateTexturePack(LoadedTexturePack *loaded_texture_pack) {
    loaded_texture_pack->NumInstances--;
}

int CarLoader::LoadTexturePack(LoadedTexturePack *loaded_texture_pack, int use_memory_pool) {
    int memory_pool_num = 0;

    if (use_memory_pool != 0) {
        memory_pool_num = CarLoaderMemoryPoolNumber;
        this->MakeSpaceInCarMemoryPool(loaded_texture_pack->MaxHeaderSize, 0, true);
    }

    this->LoadingInProgress = 1;
    loaded_texture_pack->LoadState = CARLOADSTATE_LOADING;
    eLoadStreamingTexturePack(loaded_texture_pack->Filename, LoadedTexturePackCallbackBridge, reinterpret_cast<uintptr_t>(loaded_texture_pack),
                              memory_pool_num);
    loaded_texture_pack->pStreamingPack = StreamingTexturePackLoader.GetLoadedStreamingPack(loaded_texture_pack->Filename);
    return 1;
}

void CarLoader::LoadedTexturePackCallback(LoadedTexturePack *loaded_texture_pack) {
    StreamingTexturePackLoader.DisableStreamingPack(loaded_texture_pack->Filename);
    loaded_texture_pack->LoadState = CARLOADSTATE_LOADED;
    this->LoadingDoneCallback();
}

int CarLoader::UnloadTexturePack(LoadedTexturePack *loaded_texture_pack) {
    if (loaded_texture_pack->NumInstances == 0) {
        if (loaded_texture_pack->LoadState == CARLOADSTATE_LOADED && loaded_texture_pack->pStreamingPack != nullptr) {
            eUnloadStreamingTexturePack(loaded_texture_pack->Filename);
        }

        delete this->LoadedTexturePackList.Remove(loaded_texture_pack);
        return 1;
    }

    return 0;
}

int CarLoader::LoadCar(LoadedCar *loaded_car) {
    if (!loaded_car->ShouldWeStream()) {
        loaded_car->LoadState = CARLOADSTATE_LOADED;
        return 0;
    } else {
        ProfileNode profile_node("TODO", 0);
        unsigned int model_hashes[800];
        int num_model_hashes = loaded_car->GetModelHashes(model_hashes, 800);

        do {
            if (StreamingSolidPackLoader.TestLoadStreamingEntry(model_hashes, num_model_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                break;
            }
        } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);

        loaded_car->LoadState = CARLOADSTATE_LOADING;
        this->LoadingInProgress = 1;
        StreamingSolidPackLoader.EnableStreamingPack(nullptr);
        profile_node.End();
        eLoadStreamingSolid(model_hashes, num_model_hashes, LoadedCarCallbackBridge, reinterpret_cast<unsigned int>(loaded_car),
                            CarLoaderMemoryPoolNumber);
        return 1;
    }
}

void CarLoader::LoadedCarCallback(LoadedCar *loaded_car) {
    StreamingSolidPackLoader.DisableStreamingPack(loaded_car->pLoadedSolidPack->Filename);
    loaded_car->LoadState = CARLOADSTATE_LOADED;
    this->LoadingDoneCallback();
}

int CarLoader::UnloadCar(LoadedCar *loaded_car) {
    if (loaded_car->LoadState == CARLOADSTATE_LOADED && loaded_car->ShouldWeStream()) {
        unsigned int model_hashes[800];
        int num_model_hashes = loaded_car->GetModelHashes(model_hashes, NUM_ELEMENTS(model_hashes));

        StreamingSolidPackLoader.EnableStreamingPack(loaded_car->pLoadedSolidPack->Filename);
        eUnloadStreamingSolid(model_hashes, num_model_hashes);
        StreamingSolidPackLoader.DisableStreamingPack(loaded_car->pLoadedSolidPack->Filename);
    }

    this->UnallocateSolidPack(loaded_car->pLoadedSolidPack);
    this->UnloadSolidPack(loaded_car->pLoadedSolidPack);
    loaded_car->pLoadedSolidPack = nullptr;
    return 1;
}

int CarLoader::LoadAllWheelModels() {
    unsigned int name_hash_table[128];
    int num_name_hashes = 0;

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances != 0 && loaded_ride_info->pLoadedWheel->LoadState == CARLOADSTATE_QUEUED) {
            LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;

            loaded_wheel->LoadState = CARLOADSTATE_LOADING;
            loaded_ride_info->LoadState = CARLOADSTATE_LOADING;

            for (int model = 0; model < 1; model++) {
                for (int lod = loaded_wheel->GetMinLodLevel(); lod <= loaded_wheel->GetMaxLodLevel(); lod++) {
                    unsigned int model_name_hash = loaded_wheel->ModelNameHashes[model][lod];

                    if (model_name_hash != 0) {
                        name_hash_table[num_name_hashes] = model_name_hash;
                        num_name_hashes++;
                    }
                }
            }
        }
    }

    if (num_name_hashes > 0) {
        do {
            if (StreamingSolidPackLoader.TestLoadStreamingEntry(name_hash_table, num_name_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                break;
            }
        } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);

        this->LoadingInProgress = 1;
        eLoadStreamingSolid(name_hash_table, num_name_hashes, LoadedWheelModelsCallbackBridge, 0, CarLoaderMemoryPoolNumber);
        return 1;
    }

    return 0;
}

void CarLoader::LoadedWheelModelsCallback() {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;
        if (loaded_wheel->LoadState == CARLOADSTATE_LOADING) {
            loaded_wheel->LoadState = CARLOADSTATE_LOADED;
        }
    }

    this->LoadingDoneCallback();
}

int CarLoader::LoadAllWheelTextures() {
    unsigned int name_hash_table[128];
    int num_name_hashes = 0;

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances != 0) {
            LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;

            if (loaded_wheel->LoadStateSkinPerm == CARLOADSTATE_QUEUED) {
                int num_loaded = this->LoadSkinLayers(&name_hash_table[num_name_hashes], NUM_ELEMENTS(name_hash_table) - num_name_hashes,
                                                      loaded_wheel->LoadedSkinLayersPerm, 4);

                if (num_loaded != 0) {
                    loaded_wheel->LoadStateSkinPerm = CARLOADSTATE_LOADING;
                } else {
                    loaded_wheel->LoadStateSkinPerm = CARLOADSTATE_LOADED;
                }

                loaded_ride_info->LoadState = CARLOADSTATE_LOADING;
                num_name_hashes += num_loaded;
            }

            if (loaded_wheel->LoadStateSkinTemp == CARLOADSTATE_QUEUED) {
                loaded_wheel->LoadStateSkinTemp = CARLOADSTATE_LOADED;
            }
        }
    }

    if (num_name_hashes > 0) {
        int memory_pool_num = CarLoaderMemoryPoolNumber;

        do {
            if (StreamingTexturePackLoader.TestLoadStreamingEntry(name_hash_table, num_name_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                break;
            }
        } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);

        this->LoadingInProgress = 1;

        eLoadStreamingTexture(name_hash_table, num_name_hashes, LoadedWheelTexturesCallbackBridge, 0, memory_pool_num);

        return 1;
    }

    return 0;
}

void CarLoader::LoadedWheelTexturesCallback() {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
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

int CarLoader::UnloadWheel(LoadedWheel *loaded_wheel) {
    {
        unsigned int name_hash_table[128];

        this->UnallocateSkinLayers(loaded_wheel->LoadedSkinLayersPerm, 4);

        int num_name_hashes = this->UnloadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), loaded_wheel->LoadedSkinLayersPerm, 4);

        if (num_name_hashes != 0) {
            eUnloadStreamingTexture(name_hash_table, num_name_hashes);
        }

        if (loaded_wheel->LoadState == CARLOADSTATE_LOADED) {
            eUnloadStreamingSolid(reinterpret_cast<unsigned int *>(loaded_wheel->ModelNameHashes), 5);
        }

        return 1;
    }
}

int CarLoader::GetMemoryEntries(LoadedWheel *loaded_wheel, void **memory_entries, int num_memory_entries) {
    int n;
    for (n = 0; n < 4; n++) {
        num_memory_entries = this->GetMemoryEntries(loaded_wheel->LoadedSkinLayersPerm[n], memory_entries, num_memory_entries);
    }

    for (n = 0; n < 4; n++) {
        num_memory_entries = this->GetMemoryEntries(loaded_wheel->LoadedSkinLayersTemp[n], memory_entries, num_memory_entries);
    }

    return StreamingSolidPackLoader.GetMemoryEntries(*loaded_wheel->ModelNameHashes, 5, memory_entries, num_memory_entries);
}

int CarLoader::LoadAllTexturesFromPack(const char *filename, int load_perm_layers) {
    ProfileNode profile_node("TODO", 0);
    unsigned int texture_hashes[128];
    unsigned int name_hash_table[512];

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances > 0 && loaded_ride_info->LoadState == CARLOADSTATE_QUEUED) {
            LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;
            int num_texture_hashes = loaded_skin->GetTextureHashes(texture_hashes, NUM_ELEMENTS(texture_hashes), load_perm_layers);

            this->NumLoadingSkinLayers +=
                this->AllocateSkinLayers(texture_hashes, num_texture_hashes, &this->LoadingSkinLayers[this->NumLoadingSkinLayers],
                                         NUM_ELEMENTS(name_hash_table) - this->NumLoadingSkinLayers, filename);
        }
    }

    int num_name_hashes = this->LoadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), this->LoadingSkinLayers, this->NumLoadingSkinLayers);

    if (num_name_hashes == 0) {
        this->UnallocateSkinLayers(this->LoadingSkinLayers, this->NumLoadingSkinLayers);
        this->NumLoadingSkinLayers = 0;

        return 0;
    } else {
        int memory_pool_num = 0;

        if (load_perm_layers != 0 || this->LoadingMode == MODE_IN_GAME) {
            memory_pool_num = CarLoaderMemoryPoolNumber;

            do {
                if (StreamingTexturePackLoader.TestLoadStreamingEntry(name_hash_table, num_name_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                    break;
                }
            } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);
        }

        this->LoadingInProgress = 1;
        eLoadStreamingTexture(name_hash_table, num_name_hashes, LoadedAllTexturesFromPackCallbackBridge, 0, memory_pool_num);
        return 1;
    }
}

void CarLoader::LoadedAllTexturesFromPackCallback() {
    unsigned int name_hash_table[512];

    this->LoadedSkinLayers(this->LoadingSkinLayers, this->NumLoadingSkinLayers);
    this->UnallocateSkinLayers(this->LoadingSkinLayers, this->NumLoadingSkinLayers);

    int num_name_hashes = this->UnloadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), this->LoadingSkinLayers, this->NumLoadingSkinLayers);

    if (num_name_hashes != 0) {
        eUnloadStreamingTexture(name_hash_table, num_name_hashes);
    }

    this->NumLoadingSkinLayers = 0;
    this->LoadingDoneCallback();
}

int CarLoader::LoadSkin(LoadedSkin *loaded_skin, int load_perm_layers) {
    ProfileNode profile_node("TODO", 0);
    unsigned int name_hash_table[87];
    int num_name_hashes;

    if (load_perm_layers != 0) {
        num_name_hashes = this->LoadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), loaded_skin->LoadedSkinLayersPerm,
                                               loaded_skin->NumLoadedSkinLayersPerm);

        loaded_skin->LoadStatePerm = num_name_hashes != 0 ? CARLOADSTATE_LOADING : CARLOADSTATE_LOADED;
    } else {
        num_name_hashes = this->LoadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), loaded_skin->LoadedSkinLayersTemp,
                                               loaded_skin->NumLoadedSkinLayersTemp);

        loaded_skin->LoadStateTemp = num_name_hashes != 0 ? CARLOADSTATE_LOADING : CARLOADSTATE_LOADED;
    }

    if (num_name_hashes > 0) {
        int memory_pool_num = 0;

        if (load_perm_layers != 0 || this->LoadingMode == MODE_IN_GAME) {
            memory_pool_num = CarLoaderMemoryPoolNumber;

            do {
                if (StreamingTexturePackLoader.TestLoadStreamingEntry(name_hash_table, num_name_hashes, CarLoaderMemoryPoolNumber, true) == 0) {
                    break;
                }
            } while (this->RemoveSomethingFromCarMemoryPool(true) != 0);
        } else if (this->LoadingMode == MODE_LOADING_GAME) {
            memory_pool_num = 7;
        }

        this->LoadingInProgress = 1;
        eLoadStreamingTexture(name_hash_table, num_name_hashes, LoadedSkinCallbackBridge, reinterpret_cast<uintptr_t>(loaded_skin), memory_pool_num);

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

// UNSOLVED
void CarLoader::CompositeSkin(LoadedSkin *loaded_skin) {
    if (loaded_skin->pRideInfo->IsUsingCompositeSkin() != 0) {
        if (this->LoadingMode == MODE_IN_GAME) {
            int required_size = CarInfo_GetMaxCompositingBufferSize();

            if (required_size > bCountFreeMemory(CarLoaderMemoryPoolNumber)) {
                do {
                    if (!this->RemoveSomethingFromCarMemoryPool(false)) {
                        break;
                    }
                } while (required_size > bCountFreeMemory(CarLoaderMemoryPoolNumber));

                this->DefragmentPool();
            }
        }

        ::CompositeSkin(loaded_skin->pRideInfo);
    }

    loaded_skin->DoneComposite = 1;
}

int CarLoader::UnloadSkinTemporaries(LoadedSkin *loaded_skin, int force_unload) {
    if ((loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED && loaded_skin->DoneComposite != 0) || force_unload != 0) {
        int unloaded_something = 0;
        unsigned int name_hash_table[87];

        this->UnallocateSkinLayers(loaded_skin->LoadedSkinLayersTemp, loaded_skin->NumLoadedSkinLayersTemp);
        int num_name_hashes = this->UnloadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), loaded_skin->LoadedSkinLayersTemp,
                                                     loaded_skin->NumLoadedSkinLayersTemp);
        loaded_skin->NumLoadedSkinLayersTemp = 0;

        if (num_name_hashes != 0) {
            unloaded_something = 1;
            eUnloadStreamingTexture(name_hash_table, num_name_hashes);
        }

        if (loaded_skin->pLoadedVinylsPack != nullptr) {
            unloaded_something += 1;
            this->UnallocateTexturePack(loaded_skin->pLoadedVinylsPack);
            this->UnloadTexturePack(loaded_skin->pLoadedVinylsPack);
            loaded_skin->pLoadedVinylsPack = nullptr;
        }

        return unloaded_something;
    }

    return 0;
}

int CarLoader::UnloadSkinPerms(LoadedSkin *loaded_skin) {
    unsigned int name_hash_table[87];

    this->UnallocateSkinLayers(loaded_skin->LoadedSkinLayersPerm, loaded_skin->NumLoadedSkinLayersPerm);

    int num_name_hashes = this->UnloadSkinLayers(name_hash_table, NUM_ELEMENTS(name_hash_table), loaded_skin->LoadedSkinLayersPerm,
                                                 loaded_skin->NumLoadedSkinLayersPerm);

    if (num_name_hashes == 0) {
        return 0;
    }

    eUnloadStreamingTexture(name_hash_table, num_name_hashes);
    loaded_skin->NumLoadedSkinLayersPerm = 0;
    return 1;
}

int CarLoader::UnloadSkin(LoadedSkin *loaded_skin) {
    {
        int force_unload;
    }
    this->UnloadSkinTemporaries(loaded_skin, 1);
    this->UnloadSkinPerms(loaded_skin);
    this->UnallocateTexturePack(loaded_skin->pLoadedTexturesPack);
    this->UnloadTexturePack(loaded_skin->pLoadedTexturesPack);
    loaded_skin->pLoadedTexturesPack = nullptr;
    return 1;
}

// STRIPPED
int CarLoader::IsSkinInMemoryPool(LoadedSkin *loaded_skin) {}

int CarLoader::GetMemoryEntries(LoadedSkin *loaded_skin, void **memory_entries, int num_memory_entries) {
    num_memory_entries = this->GetMemoryEntries(loaded_skin->pLoadedTexturesPack, memory_entries, num_memory_entries);

    int n;
    for (n = 0; n < loaded_skin->NumLoadedSkinLayersPerm; n++) {
        num_memory_entries = this->GetMemoryEntries(loaded_skin->LoadedSkinLayersPerm[n], memory_entries, num_memory_entries);
    }

    for (n = 0; n < loaded_skin->NumLoadedSkinLayersTemp; n++) {
        num_memory_entries = this->GetMemoryEntries(loaded_skin->LoadedSkinLayersTemp[n], memory_entries, num_memory_entries);
    }

    return num_memory_entries;
}

int CarLoader::AllocateSkinLayers(unsigned int *name_hash_table, int num_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                                  int max_loaded_skin_layers, const char *filename) {
    ProfileNode profile_node("TODO", 0);

    eStreamingPack *streaming_pack = nullptr;
    if (filename != nullptr) {
        streaming_pack = StreamingTexturePackLoader.GetLoadedStreamingPack(filename);

        if (streaming_pack == nullptr) {
            return 0;
        }
    }

    int num_skin_layers = 0;

    for (int n = 0; n < num_name_hashes; n++) {
        unsigned int name_hash = name_hash_table[n];

        if (name_hash != 0 && (streaming_pack == nullptr || StreamingTexturePackLoader.GetLoadedStreamingPack(name_hash) == streaming_pack)) {
            LoadedSkinLayer *loaded_skin_layer = this->FindLoadedSkinLayer(name_hash);

            if (loaded_skin_layer == nullptr) {
                loaded_skin_layer = new LoadedSkinLayer(name_hash);
                this->LoadedSkinLayerList.AddTail(loaded_skin_layer);
            }

            loaded_skin_layer->NumInstances++;
            loaded_skin_layer_table[num_skin_layers] = loaded_skin_layer;
            num_skin_layers++;
        }
    }

    for (int i = num_skin_layers; i < max_loaded_skin_layers; i++) {
        loaded_skin_layer_table[i] = nullptr;
    }

    return num_skin_layers;
}

void CarLoader::UnallocateSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers) {
    ProfileNode profile_node("TODO", 0);
    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != nullptr) {
            loaded_skin_layer->NumInstances--;
        }
    }
}

int CarLoader::LoadSkinLayers(unsigned int *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                              int num_loaded_skin_layers) {
    int num_name_hashes = 0;

    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != nullptr && loaded_skin_layer->LoadState == CARLOADSTATE_QUEUED) {
            loaded_skin_layer->LoadState = CARLOADSTATE_LOADING;
            name_hash_table[num_name_hashes] = loaded_skin_layer->NameHash;
            num_name_hashes++;
        }
    }

    return num_name_hashes;
}

void CarLoader::LoadedSkinLayers(LoadedSkinLayer **loaded_skin_layer_table, int num_loaded_skin_layers) {
    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != nullptr && loaded_skin_layer->LoadState == CARLOADSTATE_LOADING) {
            loaded_skin_layer->LoadState = CARLOADSTATE_LOADED;
        }
    }
}

int CarLoader::UnloadSkinLayers(unsigned int *name_hash_table, int max_name_hashes, LoadedSkinLayer **loaded_skin_layer_table,
                                int num_loaded_skin_layers) {
    int num_name_hashes = 0;

    for (int n = 0; n < num_loaded_skin_layers; n++) {
        LoadedSkinLayer *loaded_skin_layer = loaded_skin_layer_table[n];

        if (loaded_skin_layer != nullptr && loaded_skin_layer->NumInstances == 0) {
            if (loaded_skin_layer->LoadState == CARLOADSTATE_LOADED) {
                name_hash_table[num_name_hashes] = loaded_skin_layer->NameHash;
                num_name_hashes++;
            }

            delete this->LoadedSkinLayerList.Remove(loaded_skin_layer);
        }
    }

    return num_name_hashes;
}

// STRIPPED
int CarLoader::IsSkinLayerInMemoryPool(struct LoadedSkinLayer *loaded_skin_layer) {}

int CarLoader::GetMemoryEntries(LoadedSkinLayer *loaded_skin_layer, void **memory_entries, int num_memory_entries) {
    if (loaded_skin_layer != nullptr && loaded_skin_layer->LoadState == CARLOADSTATE_LOADED) {
        eStreamingEntry *streaming_entry = StreamingTexturePackLoader.GetStreamingEntry(loaded_skin_layer->NameHash);

        if (streaming_entry != nullptr && streaming_entry->ChunkData != nullptr) {
            memory_entries[num_memory_entries] = streaming_entry->ChunkData;
            num_memory_entries++;
        }
    }

    return num_memory_entries;
}

LoadedSolidPack *CarLoader::FindLoadedSolidPack(const char *filename) {
    for (LoadedSolidPack *loaded_solid_pack = this->LoadedSolidPackList.GetHead(); loaded_solid_pack != this->LoadedSolidPackList.EndOfList();
         loaded_solid_pack = loaded_solid_pack->GetNext()) {
        if (bStrCmp(loaded_solid_pack->Filename, filename) == 0) {
            return loaded_solid_pack;
        }
    }

    return nullptr;
}

LoadedTexturePack *CarLoader::FindLoadedTexturePack(const char *filename) {
    for (LoadedTexturePack *loaded_texture_pack = this->LoadedTexturePackList.GetHead();
         loaded_texture_pack != this->LoadedTexturePackList.EndOfList(); loaded_texture_pack = loaded_texture_pack->GetNext()) {
        if (bStrCmp(loaded_texture_pack->Filename, filename) == 0) {
            return loaded_texture_pack;
        }
    }

    return nullptr;
}

LoadedSkinLayer *CarLoader::FindLoadedSkinLayer(unsigned int name_hash) {
    for (LoadedSkinLayer *loaded_layer = this->LoadedSkinLayerList.GetHead(); loaded_layer != this->LoadedSkinLayerList.EndOfList();
         loaded_layer = loaded_layer->GetNext()) {
        if (loaded_layer->NameHash == name_hash) {
            return loaded_layer;
        }
    }

    return nullptr;
}

LoadedRideInfo *CarLoader::FindLoadedRideInfo(CarLoaderHandle handle) {
    int id = handle;
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->ID == id) {
            return loaded_ride_info;
        }
    }

    return nullptr;
}

LoadedRideInfo *CarLoader::FindLoadedRideInfo(RideInfo *ride_info) {
    return nullptr;
}

int CarLoader::Load(RideInfo *ride_info) {
    int high_priority = 0;
    int is_player_car = static_cast<int>(ride_info->GetCarRenderUsage() == CarRenderUsage_Player);
    char filename[128];

    bSPrintf(filename, "CARS\\%s\\TEXTURES.BIN", GetCarTypeName(ride_info->Type));

    if (!bFileExists(filename)) {
        bBreak();
    }

    LoadedRideInfo *loaded_ride_info = this->AllocateRideInfo(ride_info, is_player_car);

    loaded_ride_info->HighPriority = high_priority;
    loaded_ride_info->IsPlayerCar = is_player_car;
    this->LoadedRideInfoList.Remove(loaded_ride_info);
    // AddHead missing
    this->LoadedRideInfoList.AddTail(loaded_ride_info);
    return loaded_ride_info->ID;
}

void CarLoader::Unload(int handle) {
    LoadedRideInfo *loaded_ride_info = this->FindLoadedRideInfo(handle);

    if (loaded_ride_info != nullptr && loaded_ride_info->NumInstances != 0) {
        if (loaded_ride_info->HighPriority != 0) {
            this->LoadedRideInfoList.Remove(loaded_ride_info);
            this->LoadedRideInfoList.AddTail(loaded_ride_info);
        }

        this->UnallocateRideInfo(loaded_ride_info);

        if (loaded_ride_info->NumInstances == 0 && loaded_ride_info->LoadState == CARLOADSTATE_QUEUED) {
            this->UnloadRideInfo(loaded_ride_info, 0);
        }
    }
}

int CarLoader::IsLoaded(CarLoaderHandle handle) {
    LoadedRideInfo *loaded_ride_info = this->FindLoadedRideInfo(handle);

    if (loaded_ride_info == nullptr || loaded_ride_info->NumInstances == 0) {
        return 0;
    }

    return this->IsLoaded(loaded_ride_info);
}

int CarLoader::IsLoaded(LoadedRideInfo *loaded_ride_info) {
    if (loaded_ride_info->pLoadedCar == nullptr || loaded_ride_info->pLoadedCar->LoadState != CARLOADSTATE_LOADED) {
        return 0;
    }

    if (loaded_ride_info->pLoadedWheel == nullptr || loaded_ride_info->pLoadedWheel->LoadState != CARLOADSTATE_LOADED) {
        return 0;
    }

    if (loaded_ride_info->pLoadedSkin == nullptr || loaded_ride_info->pLoadedSkin->IsLoaded() == 0) {
        return 0;
    }

    return 1;
}

// STRIPPED
void CarLoader::EnsureHasLoaded(int handle) {}

LoadedRideInfo *CarLoader::AllocateRideInfo(RideInfo *ride_info, int is_player_car) {
    LoadedRideInfo *loaded_ride_info = this->FindLoadedRideInfo(ride_info);

    if (loaded_ride_info == nullptr) {
        while (bIsSlotPoolFull(LoadedRideInfoSlotPool)) {
            while (this->LoadingInProgress != 0) {
                ServiceResourceLoading();
            }

            this->RemoveSomethingFromCarMemoryPool(true);
        }

        loaded_ride_info = new LoadedRideInfo(ride_info, this->InFrontEndFlag, this->TwoPlayerFlag, is_player_car);
        this->LoadedRideInfoList.AddTail(loaded_ride_info);
        this->NumLoadedRideInfos++;

        CarTypeInfo *car_type_info = loaded_ride_info->pCarTypeInfo;
        LoadedCar *loaded_car = loaded_ride_info->pLoadedCar;

        car_type_info->HasArtwork(); // ?
        loaded_car->pLoadedSolidPack = this->AllocateSolidPack(car_type_info->GeometryFilename);

        LoadedWheel *loaded_wheel = loaded_ride_info->pLoadedWheel;

        this->AllocateSkinLayers(loaded_wheel->SkinNameHashesPerm, 4, loaded_wheel->LoadedSkinLayersPerm, 4, nullptr);

        LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;
        char textures_filename[72];

        bSPrintf(textures_filename, "CARS\\%s\\TEXTURES.BIN", car_type_info->GetCarTypeName());
        loaded_skin->pLoadedTexturesPack = this->AllocateTexturePack(textures_filename, 0x8000);

        if (ride_info->SkinType != 0) {
            char vinyls_filename[72];

            if (UsePrecompositeVinyls != 0 || ride_info->SkinType == 2) {
                bSPrintf(vinyls_filename, "CARS\\%s\\PREVINYL.BIN", car_type_info->GetCarTypeName());
            } else {
                bSPrintf(vinyls_filename, "CARS\\%s\\VINYLS.BIN", car_type_info->GetCarTypeName());
            }
            car_type_info->GetCarTypeName(); // ?

            if (bFileExists(vinyls_filename)) {
                loaded_skin->pLoadedVinylsPack = this->AllocateTexturePack(vinyls_filename, 0x19000);
            }
        }

        unsigned int texture_hashes[128];
        int num_texture_hashes = loaded_skin->GetTextureHashes(texture_hashes, NUM_ELEMENTS(texture_hashes), TRUE);

        loaded_skin->NumLoadedSkinLayersPerm = num_texture_hashes;
        this->AllocateSkinLayers(texture_hashes, num_texture_hashes, loaded_skin->LoadedSkinLayersPerm, num_texture_hashes, nullptr);

        num_texture_hashes = loaded_skin->GetTextureHashes(texture_hashes, NUM_ELEMENTS(texture_hashes), FALSE);

        loaded_skin->NumLoadedSkinLayersTemp = num_texture_hashes;
        this->AllocateSkinLayers(texture_hashes, num_texture_hashes, loaded_skin->LoadedSkinLayersTemp, num_texture_hashes, nullptr);
    }

    if (loaded_ride_info->NumInstances == 0) {
        this->NumAllocatedRideInfos++;
    }

    loaded_ride_info->NumInstances++;
    return loaded_ride_info;
}

int CarLoader::UnallocateRideInfo(LoadedRideInfo *loaded_ride_info) {
    loaded_ride_info->NumInstances--;

    if (loaded_ride_info->NumInstances == 0) {
        this->NumAllocatedRideInfos--;
        return 1;
    }

    return 0;
}

int CarLoader::UnloadRideInfo(LoadedRideInfo *loaded_ride_info, int leave_if_in_mempool) {
    ProfileNode profile_node("TODO", 0);

    if (loaded_ride_info->NumInstances > 0) {
        return 0;
    }

    if (loaded_ride_info->pLoadedSkin != nullptr && loaded_ride_info->pLoadedSkin->IsLoaded()) {
        this->UnloadSkinTemporaries(loaded_ride_info->pLoadedSkin, 0);
    }

    int in_mempool = this->IsLoaded(loaded_ride_info);

    if (leave_if_in_mempool != 0 && in_mempool != 0) {
        return 0;
    }

    this->UnloadSkin(loaded_ride_info->pLoadedSkin);
    this->UnloadWheel(loaded_ride_info->pLoadedWheel);
    this->UnloadCar(loaded_ride_info->pLoadedCar);
    delete this->LoadedRideInfoList.Remove(loaded_ride_info);
    this->NumLoadedRideInfos--;
    this->MayNeedDefragmentation++;
    return 1;
}

void CarLoader::UnloadEverything() {
    LoadedRideInfo *high_loaded_ride_info = nullptr;

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances > 0) {
            this->UnallocateRideInfo(loaded_ride_info);
        }

        if (loaded_ride_info->HighPriority != 0) {
            high_loaded_ride_info = loaded_ride_info;
        }
    }

    if (high_loaded_ride_info != nullptr) {
        this->LoadedRideInfoList.Remove(high_loaded_ride_info);
        this->LoadedRideInfoList.AddTail(high_loaded_ride_info);
    }

    while (this->LoadingInProgress != 0) {
        ServiceResourceLoading();
    }

    this->UnloadOverflowedResources();
}

void CarLoader::UnloadOverflowedResources() {
    LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead();

    while (loaded_ride_info != this->LoadedRideInfoList.EndOfList()) {
        LoadedRideInfo *next_loaded_ride_info = loaded_ride_info->GetNext();
        int leave_if_in_mempool;

        this->UnloadRideInfo(loaded_ride_info, 1);
        loaded_ride_info = next_loaded_ride_info;
    }
}

void CarLoader::UnloadUnallocatedRideInfos(int max_left_unloaded) {
    {
        bool force_unload = false;
        while (NumLoadedRideInfos - NumAllocatedRideInfos >= max_left_unloaded) {
            if (!RemoveSomethingFromCarMemoryPool(force_unload)) {
                return;
            }
        }
    }
}

void CarLoader::UnloadAllSkinTemporaries() {
    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

        if (loaded_skin->IsLoaded()) {
            this->UnloadSkinTemporaries(loaded_skin, FALSE);
        } else if (loaded_ride_info->NumInstances == 0 && loaded_skin->LoadStateTemp == CARLOADSTATE_LOADED) {
            int force_unload = 1;
            this->UnloadSkinTemporaries(loaded_skin, force_unload);
        }
    }
}

// STRIPPED
int CarLoader::IsCarInMemoryPool(LoadedCar *loaded_car) {}

int CarLoader::GetMemoryEntries(LoadedCar *loaded_car, void **memory_entries, int num_memory_entries) {
    unsigned int name_hashes[800];
    // TODO get rid of temporary
    int num_used_entries = this->GetMemoryEntries(loaded_car->pLoadedSolidPack, memory_entries, num_memory_entries);
    int num_model_hashes = loaded_car->GetModelHashes(name_hashes, NUM_ELEMENTS(name_hashes));

    return StreamingSolidPackLoader.GetMemoryEntries(name_hashes, num_model_hashes, memory_entries, num_used_entries);
}

int CarLoader::RemoveSomethingFromCarMemoryPool(bool force_unload) {
    LoadedRideInfo *loaded_ride_info;

    for (loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        int leave_if_in_mempool = this->UnloadRideInfo(loaded_ride_info, 0);

        if (leave_if_in_mempool != 0) {
            return 1;
        }
    }

    if (!force_unload) {
        return 0;
    }

    if (this->DefragmentPool() != 0) {
        return 1;
    }

    if (this->NumSpongeAllocations != 0) {
        int mem_remaining;
        this->NumSpongeAllocations--;
        bFree(this->SpongeAllocations[this->NumSpongeAllocations]);
        this->MayNeedDefragmentation++;
        return 1;
    }

    for (int pass = 0; pass < 2; pass++) {
        for (loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
             loaded_ride_info = loaded_ride_info->GetNext()) {
            if (loaded_ride_info->HighPriority == 0 || pass == 1) {
                LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

                if (loaded_skin->LoadStatePerm == CARLOADSTATE_LOADED) {
                    if (this->UnloadSkinPerms(loaded_skin) != 0) {
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
    }

    this->PrintMemoryUsage(false);
    bBreak();
    return 0;
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

    int amount_free = bCountFreeMemory(CarLoaderMemoryPoolNumber);
    int largest_malloc = bLargestMalloc((CarLoaderMemoryPoolNumber & 0xF) | 0x2000);
    int unloaded_something;

    // TODO simply while loop?
    if (amount_free < amount_free_needed || largest_malloc < largest_malloc_needed) {
        do {
            unloaded_something = this->RemoveSomethingFromCarMemoryPool(true);

            if (unloaded_something == 0) {
                break;
            }

            amount_free = bCountFreeMemory(CarLoaderMemoryPoolNumber);
            largest_malloc = bLargestMalloc((CarLoaderMemoryPoolNumber & 0xF) | 0x2000);
        } while (amount_free < amount_free_needed || largest_malloc < largest_malloc_needed);
    }

    if (amount_free < amount_free_needed || largest_malloc < largest_malloc_needed) {
        return 0;
    }

    if (allocating_stream_header_chunks) {
        char *min_stream_header_chunks = nullptr;

        for (int type = 0; type <= 1; type++) {
            bTList<eStreamingPack> *list = StreamingTexturePackLoader.GetLoadedStreamingPackList();

            if (type == 1) {
                list = StreamingSolidPackLoader.GetLoadedStreamingPackList();
            }

            for (eStreamingPack *streaming_pack = list->GetHead(); streaming_pack != list->EndOfList(); streaming_pack = streaming_pack->GetNext()) {
                if (streaming_pack->HeaderChunks != nullptr && bGetMallocPool(streaming_pack->HeaderChunks) == CarLoaderMemoryPoolNumber &&
                    (min_stream_header_chunks == nullptr || reinterpret_cast<char *>(streaming_pack->HeaderChunks) < min_stream_header_chunks)) {
                    min_stream_header_chunks = reinterpret_cast<char *>(streaming_pack->HeaderChunks);
                }
            }
        }

        char *test_alloc = static_cast<char *>(bMalloc(largest_malloc_needed, "TestAlloc", 0, (CarLoaderMemoryPoolNumber & 0xF) | 0x2040));

        bFree(test_alloc);

        int overlap = test_alloc + largest_malloc_needed - min_stream_header_chunks;

        if (min_stream_header_chunks != nullptr && test_alloc != nullptr) {
            if (bAbs(overlap) > 256) {
                this->DefragmentPool();
            }
        }
    }

    return 1;
}

struct _DefragmentParams {
    int NumCopyStorage;             // offset 0x0, size 0x4
    int CopyStorageSize[8];         // offset 0x4, size 0x20
    void *CopyStorageMem[8];        // offset 0x24, size 0x20
    int LargestAllocationSize;      // offset 0x44, size 0x4
    char LargestAllocationName[64]; // offset 0x48, size 0x40
    void *pAllocation;              // offset 0x88, size 0x4
    void *pNewAllocation;           // offset 0x8C, size 0x4
    char AllocationName[64];        // offset 0x90, size 0x40
};

_DefragmentParams DefragmentParams;

void *MoveDefragmentAllocation(void *allocation) {
    ProfileNode profile_node("TODO", 0);
    int allocation_size = bGetMallocSize(allocation);

    if (allocation_size > DefragmentParams.LargestAllocationSize) {
        return allocation;
    }

    bool use_copy_storage =
        reinterpret_cast<intptr_t>(allocation) - reinterpret_cast<intptr_t>(DefragmentParams.pNewAllocation) < allocation_size + 1152;
    void *new_allocation;

    if (use_copy_storage) {
        int amount_copied = 0;
        int n = 0;

        while (amount_copied < allocation_size) {
            int amount_to_copy = allocation_size - amount_copied;

            if (amount_to_copy > DefragmentParams.CopyStorageSize[n]) {
                amount_to_copy = DefragmentParams.CopyStorageSize[n];
            }

            ScratchPadMemCpy(DefragmentParams.CopyStorageMem[n], reinterpret_cast<unsigned char *>(allocation) + amount_copied, amount_to_copy);
            amount_copied += amount_to_copy;
            n++;
        }

        bFree(allocation);
        allocation = nullptr;
        DefragmentParams.pAllocation = allocation;
    }

    new_allocation = bMalloc(allocation_size, (CarLoaderMemoryPoolNumber & 0xF) | 0x2000);

    if (new_allocation != DefragmentParams.pNewAllocation) {
        bMemoryPrintAllocationsByAddress(CarLoaderMemoryPoolNumber, 0, 0x7FFFFFFF);
        bBreak();
    }

    if (use_copy_storage) {
        int amount_copied = 0;
        int n = 0;

        while (amount_copied < allocation_size) {
            int amount_to_copy = allocation_size - amount_copied;

            if (amount_to_copy > DefragmentParams.CopyStorageSize[n]) {
                amount_to_copy = DefragmentParams.CopyStorageSize[n];
            }

            ScratchPadMemCpy(reinterpret_cast<unsigned char *>(new_allocation) + amount_copied, DefragmentParams.CopyStorageMem[n], amount_to_copy);
            amount_copied += amount_to_copy;
            n++;
        }
    } else {
        ScratchPadMemCpy(new_allocation, allocation, allocation_size);
        bFree(allocation);
        allocation = nullptr;
        DefragmentParams.pAllocation = allocation;
    }

#ifdef EA_PLATFORM_GAMECUBE
    DCStoreRange(new_allocation, allocation_size);
#endif

    return new_allocation;
}

bool CarLoader::DefragmentAllocation(void *allocation) {
    ProfileNode profile_node("TODO", 0);
    static bool last_result_was_textures = false;

    for (int n = 0; n < this->NumSpongeAllocations; n++) {
        if (this->SpongeAllocations[n] == allocation) {
            this->SpongeAllocations[n] = MoveDefragmentAllocation(allocation);
            return true;
        }
    }

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        LoadedSolidPack *loaded_solid_pack = loaded_ride_info->pLoadedCar->pLoadedSolidPack;

        if (loaded_solid_pack->pResourceFile != nullptr && loaded_solid_pack->pResourceFile->GetMemory() == allocation) {
            loaded_solid_pack->pResourceFile->ManualUnload();

            ProfileNode profile_node("TODO", 0);
            bChunk *new_chunks = reinterpret_cast<bChunk *>(MoveDefragmentAllocation(allocation));

            loaded_solid_pack->pResourceFile->ManualReload(new_chunks);

            return true;
        }
    }

    ProfileNode profile_node2;

    profile_node2.Begin("TODO", 0);
    if (last_result_was_textures && StreamingTexturePackLoader.DefragmentAllocation(allocation)) {
        return true;
    }

    profile_node2.Begin("TODO", 0);
    if (StreamingSolidPackLoader.DefragmentAllocation(allocation)) {
        last_result_was_textures = false;
        return true;
    }

    profile_node2.Begin("TODO", 0);
    if (StreamingTexturePackLoader.DefragmentAllocation(allocation)) {
        last_result_was_textures = true;
        return true;
    }

    return false;
}

bool CarLoader::AllocateDefragmentStorage() {
    int copy_storage_size = 0;
    int num_copy_storage = 0;

    if (copy_storage_size < DefragmentParams.LargestAllocationSize) {
        do {
            int amount_needed = DefragmentParams.LargestAllocationSize - copy_storage_size;
            int best_pool_num = -1;
            int best_size = 0;
            char *best_pool;
            char *pools_to_try[5] = {
                "Main Pool", "Track Streaming", "Audio Memory Pool", "Speech Cache Memory Pool", "FEngMemoryPool",
            };

            for (int n = 0; n < 5; n++) {
                int pool_num = bGetMemoryPoolNum(pools_to_try[n]);

                if (pool_num > -1) {
                    int amount_free = bLargestMalloc(pool_num);

                    if (amount_free > best_size) {
                        best_size = amount_free;
                        best_pool_num = pool_num;
                    }
                }
            }

            if (best_size == 0) {
                break;
            }

            if (best_size > amount_needed) {
                best_size = amount_needed;
            }

            DefragmentParams.CopyStorageSize[num_copy_storage] = best_size;
            DefragmentParams.CopyStorageMem[num_copy_storage] = bMalloc(best_size, "CarLoaderDefragment", 0, best_pool_num & 0xF);
            copy_storage_size += best_size;
            num_copy_storage++;
        } while (copy_storage_size < DefragmentParams.LargestAllocationSize && num_copy_storage <= 7);
    }

    DefragmentParams.NumCopyStorage = num_copy_storage;
    return copy_storage_size == DefragmentParams.LargestAllocationSize;
}

void CarLoader::FreeDefragmentStorage() {
    for (int n = 0; n < DefragmentParams.NumCopyStorage; n++) {
        bFree(DefragmentParams.CopyStorageMem[n]);
    }

    DefragmentParams.NumCopyStorage = 0;
}

// UNSOLVED
int CarLoader::DefragmentPool() {
    ProfileNode profile_node("TODO", 0);
    if (this->MayNeedDefragmentation == 0) {
        return 0;
    }

    int ticks = bGetTicker();
    void *allocation_table[1152];
    int allocation_num = 0;
    int num_allocations = bMemoryGetAllocations(CarLoaderMemoryPoolNumber, allocation_table, NUM_ELEMENTS(allocation_table));

    bMemSet(&DefragmentParams, 0, sizeof(DefragmentParams));
    DefragmentParams.LargestAllocationSize = 0;

    while (allocation_num < num_allocations) {
        void *allocation = allocation_table[allocation_num];

        DefragmentParams.LargestAllocationSize = bMax(DefragmentParams.LargestAllocationSize, bGetMallocSize(allocation));
        bSafeStrCpy(DefragmentParams.LargestAllocationName, bGetMallocName(allocation), 64);
        allocation_num++;
    }

    if (!this->AllocateDefragmentStorage()) {
        this->FreeDefragmentStorage();
        return 0;
    }

    eWaitUntilRenderingDone();
    gDefragFixer.Init();

    void *first_hole = bMalloc(128, (CarLoaderMemoryPoolNumber & 0xF) | 0x2000);
    int num_hole_filling_allocations = 0;

    bFree(first_hole);

    allocation_num = 0;

    void *hole_filling_allocations[32];
    static int loop_number = 0;

    while (allocation_num < num_allocations) {
        void *allocation = allocation_table[allocation_num];
        int allocation_size = bGetMallocSize(allocation);
        int movement = 0;

        if (reinterpret_cast<intptr_t>(allocation) > reinterpret_cast<intptr_t>(first_hole)) {
            DefragmentParams.pAllocation = allocation;
            bStrNCpy(DefragmentParams.AllocationName, bGetMallocName(allocation), 0x3F);

            while (true) {
                void *hole = bMalloc(1, (CarLoaderMemoryPoolNumber & 0xF) | 0x2000);

                if (reinterpret_cast<intptr_t>(hole) < reinterpret_cast<intptr_t>(first_hole) - 128) {
                    hole_filling_allocations[num_hole_filling_allocations] = hole;
                    num_hole_filling_allocations++;
                } else {
                    bFree(hole);
                    DefragmentParams.pNewAllocation = hole;
                    movement = reinterpret_cast<intptr_t>(hole) - reinterpret_cast<intptr_t>(DefragmentParams.pAllocation);
                    ChunkMovementOffset = movement;

                    if (!this->DefragmentAllocation(allocation)) {
                        movement = 0;
                    }

                    ChunkMovementOffset = 0;
                    break;
                }
            }
        }

        gDefragFixer.Add(allocation, allocation_size, movement);
        allocation_num++;
        loop_number++;
    }

    for (int n = 0; n < num_hole_filling_allocations; n++) {
        bFree(hole_filling_allocations[n]);
    }

    this->FreeDefragmentStorage();
    bMemSet(&DefragmentParams, 0, sizeof(DefragmentParams));
    eFixupReplacementTextureTables();
    RefreshAllRenderInfo(CARTYPE_NONE);
    gDefragFixer.Init();
    this->MayNeedDefragmentation = 0;
    return 1;
}

void CarLoader::BeginLoading(void (*callback)(uintptr_t), uintptr_t param) {
    if (this->LoadingInProgress != 0) {
        if (this->LoadingInProgress == 2) {
            this->LoadingInProgress = 1;
        }
        return;
    }

    this->StartLoadingTime = GetDebugRealTime();

    if (callback != nullptr) {
        this->pCallback = callback;
        this->Param = param;
    }

    if (this->LoadingInProgress == 0) {
        this->ServiceLoading();
    }
}

int CarLoaderServiceLoadingDepth = 0;

void CarLoader::ServiceLoading() {
    ProfileNode profile_node("TODO", 0);
    int num_can_unload = this->NumLoadedRideInfos - this->NumAllocatedRideInfos;

    if (num_can_unload > 0) {
        int num_free_slots = bCountFreeSlots(LoadedRideInfoSlotPool);

        if (num_free_slots < 10) {
            int num_to_unload = 10 - num_free_slots;
            int num_to_leave;

            if (num_to_unload > num_can_unload) {
                num_to_unload = num_can_unload;
            }

            num_can_unload -= num_to_unload;
            this->UnloadUnallocatedRideInfos(num_can_unload);
        }
    }

    class QueuedFilePrioritySetter {
      public:
        QueuedFilePrioritySetter(int priority) {
            CarLoaderServiceLoadingDepth++;
            this->SavedPriority = QueuedFileDefaultPriority;
            QueuedFileDefaultPriority = priority;
        }

        ~QueuedFilePrioritySetter() {
            CarLoaderServiceLoadingDepth--;
            QueuedFileDefaultPriority = this->SavedPriority;
        }

      private:
        int SavedPriority;
    };

    QueuedFilePrioritySetter qf_priority_setter(4);

    if (this->LoadAllWheelModels() != 0) {
        return;
    }

    if (this->LoadAllWheelTextures() != 0) {
        return;
    }

    if (this->LoadAllTexturesFromPack("CARS\\TEXTURES.BIN", 1) != 0) {
        return;
    }

    for (LoadedRideInfo *loaded_ride_info = this->LoadedRideInfoList.GetHead(); loaded_ride_info != this->LoadedRideInfoList.EndOfList();
         loaded_ride_info = loaded_ride_info->GetNext()) {
        if (loaded_ride_info->NumInstances > 0 && loaded_ride_info->LoadState != CARLOADSTATE_LOADED) {
            loaded_ride_info->LoadState = CARLOADSTATE_LOADING;

            if (loaded_ride_info->PrintedLoading == 0) {
                loaded_ride_info->PrintedLoading = 1;
            }
            CarTypeInfo *car_type_info = loaded_ride_info->pCarTypeInfo;
            LoadedSolidPack *loaded_solid_pack = loaded_ride_info->pLoadedCar->pLoadedSolidPack;
            LoadedCar *loaded_car = loaded_ride_info->pLoadedCar;

            if (loaded_solid_pack->LoadState == CARLOADSTATE_QUEUED) {
                this->LoadSolidPack(loaded_solid_pack, loaded_car->ShouldWeStream());
                return;
            }

            if (loaded_car->LoadState == CARLOADSTATE_QUEUED) {
                if (this->LoadCar(loaded_car) != 0) {
                    return;
                }
            }

            LoadedSkin *loaded_skin = loaded_ride_info->pLoadedSkin;

            if (loaded_skin->LoadStatePerm == CARLOADSTATE_QUEUED) {
                int load__layers = 1;

                if (loaded_skin->pLoadedTexturesPack->LoadState == CARLOADSTATE_QUEUED) {
                    int use_memory_pool = 1;

                    if (this->LoadTexturePack(loaded_skin->pLoadedTexturesPack, use_memory_pool) != 0) {
                        return;
                    }
                }

                if (this->LoadSkin(loaded_skin, load__layers) != 0) {
                    return;
                }
            }

            if (loaded_skin->LoadStateTemp == CARLOADSTATE_QUEUED) {
                int load__layers = 0;

                if (this->LoadingMode == MODE_FRONT_END) {
                    this->UnloadAllSkinTemporaries();
                }

                if (loaded_skin->pLoadedVinylsPack != nullptr) {
                    if (loaded_skin->pLoadedVinylsPack->LoadState == CARLOADSTATE_QUEUED) {
                        int use_memory_pool = static_cast<int>(this->LoadingMode == MODE_IN_GAME);

                        this->LoadTexturePack(loaded_skin->pLoadedVinylsPack, use_memory_pool);
                        return;
                    }
                }

                if (this->LoadSkin(loaded_skin, load__layers) != 0) {
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

    if (this->pCallback != nullptr) {
        this->LoadingInProgress = 2;
        SetDelayedResourceCallback(CallUserCallback, reinterpret_cast<intptr_t>(this));
    }
}

void CarLoader::CallUserCallback(intptr_t param) {
    CarLoader *car_loader = reinterpret_cast<CarLoader *>(param);

    if (car_loader->LoadingInProgress == 1) {
        car_loader->LoadingDoneCallback();
    } else {
        void (*callback)(unsigned int) = car_loader->pCallback;

        car_loader->LoadingInProgress = 0;
        car_loader->pCallback = nullptr;
        callback(car_loader->Param);
    }
}

void CarLoader::LoadingDoneCallback() {
    this->LoadingInProgress = 0;
    this->ServiceLoading();
}

void CarLoader::LoadedSolidPackCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedSolidPackCallback(reinterpret_cast<LoadedSolidPack *>(param));
}

void CarLoader::LoadedSolidPackCallbackBridge(intptr_t param) {
    TheCarLoader.LoadedSolidPackCallback(reinterpret_cast<LoadedSolidPack *>(param));
}

void CarLoader::LoadedTexturePackCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedTexturePackCallback(reinterpret_cast<LoadedTexturePack *>(param));
}

void CarLoader::LoadedCarCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedCarCallback(reinterpret_cast<LoadedCar *>(param));
}

void CarLoader::LoadedWheelModelsCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedWheelModelsCallback();
}

void CarLoader::LoadedWheelTexturesCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedWheelTexturesCallback();
}

void CarLoader::LoadedAllTexturesFromPackCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedAllTexturesFromPackCallback();
}

void CarLoader::LoadedSkinCallbackBridge(uintptr_t param) {
    TheCarLoader.LoadedSkinCallback(reinterpret_cast<LoadedSkin *>(param));
}
