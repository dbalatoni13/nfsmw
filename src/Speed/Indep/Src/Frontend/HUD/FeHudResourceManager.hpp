#ifndef FEHUDRESOURCEMANAGER_HPP
#define FEHUDRESOURCEMANAGER_HPP

#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Misc/ResourceLoader.hpp"

// File: speed/indep/src/frontend/hud/FeHudResourceManager.hpp
// total size: 0xC
// Decl: speed/indep/src/frontend/hud/FeHudResourceManager.hpp:10
class HudResourceManager {
  public:
    typedef enum { HRM_NOT_LOADED = 0, HRM_LOADING_IN_PROGRESS = 1, HRM_LOADED = 2, HRM_UNLOADING_IN_PROGRESS = 3 } HudResourceLoadStates;

  private:
    HudResourceLoadStates mHudResourcesState; // offset 0x0, size 0x4, Decl: speed/indep/src/frontend/hud/FeHudResourceManager.hpp:20

    ResourceFile *pHudTextures; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/hud/FeHudResourceManager.hpp:22

    static const char *mPackageName;                   // size: 0x4, address: 0x8041BE44
    static int mTachLinesHash;                         // size: 0x4, address: 0x8041BE38
    static ResourceFile *pMiniMapTexture;              // size: 0x4, address: 0x8041BE3C
    static ePlayerHudType LoadingResourcesForHudType;  // size: 0x4, address: 0x8041BE40
    static int mPhase;                                 // size: 0x4, address: 0x8041BE30
    static char mCustHudTexPackName[32];               // size: 0x20, address: 0x804F42BC
    static int mCustIndex;                             // size: 0x4, address: 0x8041BE34
    static uint32 mCustomizeHUDTexTextureResources[5]; // size: 0x14, address: 0x804F42DC

  public:
    HudResourceManager();
    virtual ~HudResourceManager() {} // Decl: speed/indep/src/frontend/hud/FeHudResourceManager.hpp:31

    void LoadRequiredResources(ePlayerHudType ht, const char *pkg_name);
    void UnloadRequiredResources(ePlayerHudType ht);
    bool AreResourcesLoaded(ePlayerHudType ht);
    char *const GetHudTexPackFilename(ePlayerHudType ht);
    static CarPart *GetCarPart(ePlayerHudType ht, CAR_SLOT_ID carSlotId);
    static int GetCustomHudColour(ePlayerHudType ht, CAR_SLOT_ID carSlotId);
    static bool GetCustomHudTexPackFilename(ePlayerHudType ht, char *const hudTexturePackName);
    static char *const GetHudFengName(ePlayerHudType ht);
    static void ChooseLoadableTextures(ePlayerHudType hudType, int &textureHash, float &redlineRotation);
    static bool ChooseMinimapTextureName(ePlayerHudType hudType, char *texture_name, uint32 texture_name_size, char *minimap_texture_name,
                                         uint32 minimap_texture_name_size);
    void LoadingCompleteCallback();
    void LoadedCustomHudTexturePackCallback();
    void LoadedCustomHudTexturesCallback();
    static void LoadingCompleteCallbackBridge(int32 param);
    static void LoadingCompleteCallbackBridge(uint32 param);
    static void LoadedCustomHudTexturePackCallbackBridge(uint32 param);
    static void LoadedCustomHudTexturesCallbackBridge(uint32 param);
    static void LoadingCompleteCallbackBridge(void *param) {} // Decl: speed/indep/src/frontend/hud/FeHudResourceManager.hpp:47
};

#endif
