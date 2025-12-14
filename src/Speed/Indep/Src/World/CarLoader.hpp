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
    unsigned int NameHash; // offset 0x8, size 0x4
    short NumInstances;    // offset 0xC, size 0x2
    char LoadState;        // offset 0xE, size 0x1
    char pad0;             // offset 0xF, size 0x1
};

// total size: 0x7C
class LoadedWheel : public bTNode<LoadedWheel> {
  public:
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
};

// total size: 0x8B0
class CarLoader {
  public:
    enum eLoadingMode {
        MODE_FRONT_END = 0,
        MODE_LOADING_GAME = 1,
        MODE_IN_GAME = 2,
    };

    int IsLoadingInProgress() {
        return LoadingInProgress;
    }

  private:
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

extern CarLoader TheCarLoader;

#endif
