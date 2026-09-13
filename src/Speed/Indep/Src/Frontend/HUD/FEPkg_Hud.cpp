#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeBustedMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeCostToState.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeEngineTempGauge.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeGetawayMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHeatMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHudElement.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeOnlineHudSupport.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRaceOverMessage.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeReputation.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeShiftUpdater.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeSpeedBreakerMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeSpeedometer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTachometer.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTachometerDrag.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTimeExtension.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeTurboMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeWrongWIndi.hpp"
#include "Speed/Indep/Src/Frontend/HUD/feMinimap.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FeFadeScreen.hpp"
#include "Speed/Indep/Src/Generated/Events/EFadeScreenOff.hpp"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/World/OnlineManager.hpp"

#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"

#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowSMS.hpp"
#include "Speed/Indep/Src/Generated/Events/EWorldMapOn.hpp"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Interfaces/SimEntities/IPlayer.h"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Misc/Joystick.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <cstdio>

extern bool bIsRestartingRace;
extern int SkipFE;
extern const char *SkipFEPlayerCar;

template void UTL::Vector<IHud *, 16>::push_back(IHud *const &);

int HudResourceManager::mPhase = 0;
int HudResourceManager::mCustIndex = -1;
int HudResourceManager::mTachLinesHash = 0;
ResourceFile *HudResourceManager::pMiniMapTexture = nullptr;
const char *HudResourceManager::mPackageName = nullptr;
char HudResourceManager::mCustHudTexPackName[32];
uint32 HudResourceManager::mCustomizeHUDTexTextureResources[5];

const char *HudDragTexturePackFilename = "GLOBAL\\HUDTEXDRAG.BIN";
const char *HudSingleRaceTexturePackFilename = "GLOBAL\\HUDTEXRACE.BIN";
const char *HudDragSplitScreenTexturePackFilename = "GLOBAL\\HUDTEXSPLIT.BIN";
const char *HudSplitScreenTexturePackFilename = "GLOBAL\\HUDTEXDRAGSPLIT.BIN";

HudResourceManager::HudResourceManager() {
    mHudResourcesState = HRM_NOT_LOADED;
    pHudTextures = nullptr;
}

const char *HudResourceManager::GetHudTexPackFilename(ePlayerHudType ht) {
    const char *hud_tex_file;
    if (ht == PHT_DRAG) {
        hud_tex_file = HudDragTexturePackFilename;
    } else if (ht == PHT_SPLIT1 || ht == PHT_SPLIT2) {
        hud_tex_file = HudSplitScreenTexturePackFilename;
    } else if (ht == PHT_DRAG_SPLIT1 || ht == PHT_DRAG_SPLIT2) {
        hud_tex_file = HudDragSplitScreenTexturePackFilename;
    } else {
        hud_tex_file = HudSingleRaceTexturePackFilename;
    }
    return hud_tex_file;
}

CarPart *HudResourceManager::GetCarPart(ePlayerHudType ht, CAR_SLOT_ID carSlotId) {
    FEPlayerCarDB *stable = FEDatabase->GetPlayerCarStable(0);
    FECarRecord *car = nullptr;

    if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
        car = stable->GetCarRecordByHandle(FEDatabase->GetCareerSettings()->GetCurrentCar());
    } else {
        GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
        if ((raceParams != nullptr) && !raceParams->GetIsPursuitRace()) {
            car = stable->GetCarRecordByHandle(FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->GetSelectedCar(0));
        }
    }

    if (car != nullptr) {
        FECustomizationRecord *record = stable->GetCustomizationRecordByHandle(car->Customization);
        if (record != nullptr) {
            return record->GetInstalledPart(car->GetType(), carSlotId);
        }
    }
    return nullptr;
}

int HudResourceManager::GetCustomHudColour(ePlayerHudType ht, CAR_SLOT_ID carSlotId) {
    int colour = 0;

    if (ht == PHT_STANDARD) {
        CarPart *part = GetCarPart(PHT_STANDARD, carSlotId);
        if (part != nullptr) {
            uint8 r = part->GetAppliedAttributeIParam(bStringHash("RED"), 0);
            uint8 g = part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0);
            uint8 b = part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0);
            colour = 0xFF000000 | (r << 16) | (g << 8) | b;
        }
    }

    return colour;
}

bool HudResourceManager::GetCustomHudTexPackFilename(ePlayerHudType ht, char *const hudTexturePackName) {
    mCustIndex = 0;

    if (ht == PHT_STANDARD) {
        CarPart *part = GetCarPart(PHT_STANDARD, CARSLOTID_CUSTOM_HUD);
        if (part != nullptr) {
            mCustIndex = part->GetAppliedAttributeIParam(FEngHashString("HUDINDEX"), 0);
        }
        bSPrintf(hudTexturePackName, "GLOBAL\\HUDS_Custom_%2.2d.bin", mCustIndex);
        return true;
    }

    bSPrintf(hudTexturePackName, "");
    return false;
}

const char *HudResourceManager::GetHudFengName(ePlayerHudType ht) {
    switch (ht) {
        case PHT_DRAG:
            return "HUD_Drag.fng";
        case PHT_SPLIT1:
            return "HUD_Player1.fng";
        case PHT_SPLIT2:
            return "HUD_Player2.fng";
        case PHT_DRAG_SPLIT1:
            return "HUD_Drag_Player1.fng";
        case PHT_DRAG_SPLIT2:
            return "HUD_Drag_Player2.fng";
        default:
            return "HUD_SingleRace.fng";
    }
}

bool HudResourceManager::ChooseMinimapTextureName(ePlayerHudType hudType, char *texture_name, uint32 texture_name_size, char *minimap_texture_name,
                                                  uint32 minimap_texture_name_size) {
    if (hudType != PHT_DRAG) {
        if (GRaceStatus::Exists()) {
            extern cFrontendDatabase *FEDatabase;
            if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 13) {
                    bSNPrintf(texture_name, texture_name_size, "MINI_MAP_UNLOCK_1");
                } else if (FEDatabase->GetCareerSettings()->GetCurrentBin() >= 9) {
                    bSNPrintf(texture_name, texture_name_size, "MINI_MAP_UNLOCK_2");
                } else {
                    bSNPrintf(texture_name, texture_name_size, "MINI_MAP");
                }
            } else {
                GRaceParameters *raceParams = GRaceStatus::Get().GetRaceParameters();
                if (raceParams != nullptr) {
                    if (raceParams->GetIsPursuitRace()) {
                        if (raceParams->GetRegion() == GRace::kRaceRegion_College) {
                            bSNPrintf(texture_name, texture_name_size, "MINI_MAP_UNLOCK_1");
                        } else if (raceParams->GetRegion() == GRace::kRaceRegion_Coastal) {
                            bSNPrintf(texture_name, texture_name_size, "MINI_MAP_UNLOCK_2");
                        } else {
                            bSNPrintf(texture_name, texture_name_size, "MINI_MAP");
                        }
                    } else {
                        bSNPrintf(texture_name, texture_name_size, "MINI_MAP_%s", raceParams->GetEventID());
                    }
                }
            }
            FixDot(texture_name, texture_name_size);
            bToUpper(texture_name);
            bSNPrintf(minimap_texture_name, minimap_texture_name_size, "TRACKS\\L2RA\\%s.BIN", texture_name);
        }

        if (bFileExists(minimap_texture_name)) {
            return true;
        }
    }

    return false;
}

void HudResourceManager::ChooseLoadableTextures(ePlayerHudType hudType, int &textureHash, float &redlineRotation) {
    unsigned int vehicleKey;

    if (SkipFE) {
        vehicleKey = Attrib::StringToKey(SkipFEPlayerCar);
    } else {
        unsigned int vehicleHandle;
        FEPlayerCarDB *stable;

        if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
            vehicleHandle = FEDatabase->GetCareerSettings()->GetCurrentCar();
        } else {
            vehicleHandle = FEDatabase->GetQuickRaceSettings(GRace::kRaceType_NumTypes)->SelectedCar[0];
        }

        stable = FEDatabase->GetPlayerCarStable(0);
        vehicleKey = stable->GetCarRecordByHandle(vehicleHandle)->VehicleKey;
    }

    Attrib::Gen::pvehicle atr(vehicleKey, 0, nullptr);
    Attrib::Gen::engine atr_engine(atr.engine(0), 0, nullptr);

    float MaxRPM = atr_engine.MAX_RPM();
    float RedLineRPM = atr_engine.RED_LINE();
    bool isDrag = (hudType == PHT_DRAG);

    float maxRpmTextureNum = FEngHud::ChooseMaxRpmTextureNumber(MaxRPM);

    char textureHashString[32];
    if (isDrag) {
        sprintf(textureHashString, "DRAG_RPM_%d_LINES", static_cast<int>(maxRpmTextureNum));
    } else {
        sprintf(textureHashString, "%d_LINES_%2.2d", static_cast<int>(maxRpmTextureNum), mCustIndex);
    }
    textureHash = bStringHash(textureHashString);

    if (MaxRPM < 7000.0f) {
        if (RedLineRPM >= 6500.0f) {
            redlineRotation = isDrag ? 39.25f : 164.5f;
        } else if (RedLineRPM >= 6000.0f) {
            redlineRotation = isDrag ? 32.25f : 149.5f;
        } else if (RedLineRPM >= 5500.0f) {
            redlineRotation = isDrag ? 26.0f : 131.5f;
        } else {
            redlineRotation = isDrag ? 19.25f : 113.5f;
        }
    } else if (MaxRPM < 8000.0f) {
        if (RedLineRPM >= 7500.0f) {
            redlineRotation = isDrag ? 40.0f : 165.0f;
        } else if (RedLineRPM >= 7000.0f) {
            redlineRotation = isDrag ? 34.0f : 152.0f;
        } else if (RedLineRPM >= 6500.0f) {
            redlineRotation = isDrag ? 28.25f : 138.0f;
        } else {
            if (RedLineRPM >= 6000.0f) {
                redlineRotation = isDrag ? 22.5f : 123.0f;
            } else {
                redlineRotation = isDrag ? 17.25f : 110.0f;
            }
        }
    } else if (MaxRPM < 9000.0f) {
        if (RedLineRPM >= 8500.0f) {
            redlineRotation = isDrag ? 42.0f : 166.0f;
        } else if (RedLineRPM >= 8000.0f) {
            redlineRotation = isDrag ? 37.0f : 154.0f;
        } else if (RedLineRPM >= 7500.0f) {
            redlineRotation = isDrag ? 32.25f : 140.5f;
        } else if (RedLineRPM >= 7000.0f) {
            redlineRotation = isDrag ? 27.0f : 127.0f;
        } else {
            redlineRotation = isDrag ? 22.0f : 115.0f;
        }
    } else {
        if (RedLineRPM >= 9500.0f) {
            redlineRotation = isDrag ? 41.5f : 167.0f;
        } else if (RedLineRPM >= 9000.0f) {
            redlineRotation = isDrag ? 37.0f : 156.0f;
        } else if (RedLineRPM >= 8500.0f) {
            redlineRotation = isDrag ? 31.5f : 145.0f;
        } else if (RedLineRPM >= 8000.0f) {
            redlineRotation = isDrag ? 27.0f : 134.0f;
        } else {
            redlineRotation = isDrag ? 22.75f : 123.0f;
        }
    }
}

void HudResourceManager::LoadRequiredResources(ePlayerHudType ht, const char *pkg_name) {
    mPhase = 0;
    const char *hud_tex_file = GetHudTexPackFilename(ht);
    int allocation_params = 0x2000;
    eWaitUntilRenderingDone();
    if (ht == PHT_DRAG) {
        allocation_params = 0x2047;
        TheTrackStreamer.MakeSpaceInPool(bFileSize(hud_tex_file), true);
    }
    pHudTextures = CreateResourceFile(hud_tex_file, RESOURCE_FILE_INGAME, 0, 0, 0);
    pHudTextures->SetAllocationParams(allocation_params, hud_tex_file);
    pHudTextures->BeginLoading(LoadingCompleteCallbackBridge, reinterpret_cast<int>(this));
    mHudResourcesState = HRM_LOADING_IN_PROGRESS;
    LoadingResourcesForHudType = ht;
    mPackageName = pkg_name;
}

void HudResourceManager::LoadingCompleteCallback() {
    mPhase++;
    if (mPhase == 1) {
        char minimap_texture_name[64];
        char texture_name[32];
        bSPrintf(minimap_texture_name, "");
        bSPrintf(texture_name, "");
        if (ChooseMinimapTextureName(LoadingResourcesForHudType, texture_name, 0x20, minimap_texture_name, 0x40)) {
            gChoppedMiniMapManager->SetMapHeader(texture_name);
            pMiniMapTexture = LoadResourceFile(minimap_texture_name, RESOURCE_FILE_TRACK, 0);
            int num_textures_to_load = 1;
            uint32 textures_to_load[16];
            textures_to_load[0] = bStringHash(texture_name);
            eLoadStreamingTexture(textures_to_load, num_textures_to_load, LoadingCompleteCallbackBridge, reinterpret_cast<uint32>(this),
                                  BMEMORY_MAIN_POOL);
        } else {
            LoadingCompleteCallback();
        }
    } else if (mPhase == 2) {
        if (GetCustomHudTexPackFilename(LoadingResourcesForHudType, mCustHudTexPackName)) {
            float redlineRotation;
            ChooseLoadableTextures(LoadingResourcesForHudType, mTachLinesHash, redlineRotation);
            FEngSetMultiImageRot(reinterpret_cast<FEMultiImage *>(FEngFindObject(mPackageName, 0xcdfce1b0)), redlineRotation);
            eLoadStreamingTexturePack(mCustHudTexPackName, LoadedCustomHudTexturePackCallbackBridge, reinterpret_cast<uint32>(this),
                                      BMEMORY_MAIN_POOL);
        } else {
            float redlineRotation;
            ChooseLoadableTextures(LoadingResourcesForHudType, mTachLinesHash, redlineRotation);
            FEngSetMultiImageRot(reinterpret_cast<FEMultiImage *>(FEngFindObject(mPackageName, 0xcdfce1b0)), redlineRotation);
            FEngSetTextureHash(mPackageName, 0x309878bc, static_cast<uint32>(mTachLinesHash));
            eLoadStreamingTexture(static_cast<uint32>(mTachLinesHash), LoadingCompleteCallbackBridge, reinterpret_cast<uint32>(this),
                                  BMEMORY_MAIN_POOL);
        }
    } else if (mPhase == 3) {
        TheHudResourceManager.mHudResourcesState = HRM_LOADED;
        cFEng::Get()->MakeLoadedPackagesDirty();
        SetSoundControlState(false, SNDSTATE_STOP_MUSIC, "HUDLoaded");
    }
}

void HudResourceManager::LoadedCustomHudTexturePackCallback() {
    int hud_num = mCustIndex;
    mCustomizeHUDTexTextureResources[0] = FEngHashString("CUSTOMHUD_TACH_%2.2d", hud_num);
    mCustomizeHUDTexTextureResources[1] = mTachLinesHash;
    mCustomizeHUDTexTextureResources[2] = FEngHashString("CUSTOMHUD_SPEEDO_%2.2d", hud_num);
    mCustomizeHUDTexTextureResources[3] = FEngHashString("CUSTOMHUD_RPMNEEDLE_%2.2d", hud_num);
    mCustomizeHUDTexTextureResources[4] = FEngHashString("CUSTOMHUD_SPEEDNEEDLE_%2.2d", hud_num);
    eLoadStreamingTexture(mCustomizeHUDTexTextureResources, 5, LoadedCustomHudTexturesCallbackBridge, reinterpret_cast<uint32>(this),
                          BMEMORY_MAIN_POOL);
}

void HudResourceManager::LoadedCustomHudTexturesCallback() {
    for (int mPhaseCust = 0; mPhaseCust <= (uint32)5; mPhaseCust++) {
        int fengObjHash = 0;
        CAR_SLOT_ID carSlotIdForColour = CARSLOTID_BASE;
        switch (mPhaseCust) {
            case 0:
                carSlotIdForColour = CARSLOTID_HUD_BACKING_COLOUR;
                fengObjHash = 0x05d19f25;
                break;
            case 1:
                carSlotIdForColour = CARSLOTID_HUD_CHARACTER_COLOUR;
                fengObjHash = 0x309878bc;
                break;
            case 2:
                carSlotIdForColour = CARSLOTID_HUD_CHARACTER_COLOUR;
                fengObjHash = 0xc62ad685;
                break;
            case 3:
                carSlotIdForColour = CARSLOTID_HUD_NEEDLE_COLOUR;
                fengObjHash = 0xf0250dac;
                break;
            case 4:
                carSlotIdForColour = CARSLOTID_HUD_NEEDLE_COLOUR;
                fengObjHash = 0x6d5ece44;
                break;
        }
        int custColour = GetCustomHudColour(LoadingResourcesForHudType, carSlotIdForColour);
        if (custColour) {
            FEngSetColor(mPackageName, fengObjHash, custColour);
        }
        FEngSetTextureHash(mPackageName, fengObjHash, mCustomizeHUDTexTextureResources[mPhaseCust]);
    };
    int custColour = GetCustomHudColour(LoadingResourcesForHudType, CARSLOTID_HUD_CHARACTER_COLOUR);
    if (custColour) {
        FEngSetColor(mPackageName, 0xc3383b63, custColour);
    }
    LoadingCompleteCallback();
}

void HudResourceManager::UnloadRequiredResources(ePlayerHudType ht) {
    eWaitForStreamingTexturePackLoading(nullptr);
    mHudResourcesState = HRM_UNLOADING_IN_PROGRESS;
    eWaitUntilRenderingDone();
    cFEng::Get()->MakeLoadedPackagesDirty();

    eUnloadAllStreamingTextures(HudDragTexturePackFilename);
    eUnloadAllStreamingTextures(HudSingleRaceTexturePackFilename);
    eUnloadAllStreamingTextures(HudSplitScreenTexturePackFilename);
    eUnloadAllStreamingTextures(HudDragSplitScreenTexturePackFilename);

    if (pHudTextures != nullptr) {
        UnloadResourceFile(pHudTextures);
        pHudTextures = nullptr;
    }
    if (pMiniMapTexture != nullptr) {
        UnloadResourceFile(pMiniMapTexture);
        pMiniMapTexture = nullptr;
    }

    if (gChoppedMiniMapManager != nullptr) {
        gChoppedMiniMapManager->RemoveUncompressedMaps();
    }

    if (!bStrCmp(mCustHudTexPackName, "")) {
        if (mTachLinesHash) {
            eUnloadStreamingTexture(mTachLinesHash);
            mTachLinesHash = 0;
        }
    } else {
        eUnloadStreamingTexture(mCustomizeHUDTexTextureResources, 5);
        // TODO: this 5 probably comes from somewhere
        for (int i = 0; i < 5u; i++) {
            mCustomizeHUDTexTextureResources[i] = 0;
        }
        eUnloadStreamingTexturePack(mCustHudTexPackName);
        bSPrintf(mCustHudTexPackName, "");
    }

    eWaitUntilRenderingDone();
    pHudTextures = nullptr;
    mHudResourcesState = HRM_NOT_LOADED;
    mPackageName = nullptr;
}

bool HudResourceManager::AreResourcesLoaded(ePlayerHudType ht) {
    if (mHudResourcesState == HRM_LOADED) {
        if (ht == PHT_SPLIT2) {
            return LoadingResourcesForHudType == PHT_SPLIT1;
        }
        if (ht == PHT_DRAG_SPLIT2) {
            return LoadingResourcesForHudType == PHT_DRAG_SPLIT1;
        }
        if (LoadingResourcesForHudType == ht) {
            return true;
        }
    }
    return false;
}

FEngHud::FEngHud(ePlayerHudType ht, const char *pkg_name, IPlayer *player, int player_number)
    : UTL::COM::Object(20), IHud(this), mPlayerHudType(ht), PlayerNumber(player_number), mActionQ(true), mCurrentWidescreenSetting(false),
      mInPursuit(false), mHasTurbo(false), pSpeedometer(nullptr), pTachometer(nullptr), pTachometerDrag(nullptr), pShiftUpdater(nullptr),
      pCostToState(nullptr), pReputation(nullptr), pHeatMeter(nullptr), pTurboMeter(nullptr), pEngineTemp(nullptr), pNitrous(nullptr),
      pSpeedBreakerMeter(nullptr), pRaceOverMessage(nullptr), pGenericMessage(nullptr), pRaceInformation(nullptr), pLeaderBoard(nullptr),
      pPursuitBoard(nullptr), pMilestoneBoard(nullptr), pBustedMeter(nullptr), pTimeExtension(nullptr), pWrongWIndi(nullptr), pOnlineSupport(nullptr),
      p321Go(nullptr), pRadarDetector(nullptr), pMinimap(nullptr), pGetAwayMeter(nullptr), pMenuZoneTrigger(nullptr), pInfractions(nullptr) {
    pPlayer = player;
    pPackageName = pkg_name;

    if (mPlayerHudType != PHT_SPLIT2 && mPlayerHudType != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.LoadRequiredResources(mPlayerHudType, pkg_name);
    }

    cFEng::Get()->PushNoControlPackage(pkg_name, FE_PACKAGE_PRIORITY_THIRD_CLOSEST);
    FEngSetAllObjectsInPackageVisibility(pkg_name, false);

    pSpeedometer = new ("Hud_Speedometer", 0) Speedometer(this, pPackageName, player_number);
    pRaceInformation = new ("Hud_RaceInformation", 0) RaceInformation(this, pkg_name, player_number);
    pLeaderBoard = new ("Hud_LeaderBoard", 0) LeaderBoard(this, pkg_name, player_number);
    pNitrous = new ("Hud_NitrousGauge", 0) NitrousGauge(this, pkg_name, player_number);
    pRaceOverMessage = new ("Hud_RaceOverMessage", 0) RaceOverMessage(this, pkg_name, player_number);
    pGenericMessage = new ("Hud_GenericMessage", 0) GenericMessage(this, pkg_name, player_number);
    pTurboMeter = new ("Hud_TurboMeter", 0) TurboMeter(this, pkg_name, player_number);
    pWrongWIndi = new ("Hud_WrongWIndi", 0) WrongWIndi(this, pkg_name, player_number);
    p321Go = new ("Hud_Coundown", 0) Countdown(this, pkg_name, player_number);

    if (mPlayerHudType == PHT_DRAG || mPlayerHudType == PHT_DRAG_SPLIT1 || mPlayerHudType == PHT_DRAG_SPLIT2) {
        pEngineTemp = new ("Hud_EngineTempGauge", 0) EngineTempGauge(this, pkg_name, player_number);
        pTachometerDrag = new ("Hud_DragTachometer", 0) DragTachometer(this, pPackageName, player_number);
        pShiftUpdater = new ("Hud_ShiftUpdater", 0) ShiftUpdater(this, pPackageName, player_number);
    } else {
        pTimeExtension = new ("Hud_TimeExtension", 0) TimeExtension(this, pkg_name, player_number);
        pTachometer = new ("Hud_Tachometer", 0) Tachometer(this, pPackageName, player_number);

        if (mPlayerHudType == PHT_STANDARD) {
            if (GRaceStatus::Exists() && GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career) {
                pReputation = new ("Hud_Reputation", 0) Reputation(this, pkg_name, player_number);
            }
            pHeatMeter = new ("Hud_HeatMeter", 0) HeatMeter(this, pkg_name, player_number);
            pCostToState = new ("Hud_CostToState", 0) CostToState(this, pkg_name, player_number);
            pPursuitBoard = new ("Hud_PursuitBoard", 0) PursuitBoard(this, pkg_name, player_number);
            pMilestoneBoard = new ("Hud_MilestoneBoard", 0) MilestoneBoard(this, pkg_name, player_number);
            pBustedMeter = new ("Hud_BustedMeter", 0) BustedMeter(this, pkg_name, player_number);
            pMenuZoneTrigger = new ("Hud_MenuZoneTrigger", 0) MenuZoneTrigger(this, pkg_name, player_number);
            pInfractions = new ("Hud_Infractions", 0) Infractions(this, pkg_name, player_number);
            pRadarDetector = new ("Hud_Infractions", 0) RadarDetector(this, pkg_name, player_number);
        }

        if (mPlayerHudType == PHT_STANDARD || mPlayerHudType == PHT_SPLIT1) {
            pMinimap = new ("Hud_Minimap", 0) Minimap(pkg_name, player_number);
        }
    }

    if (mPlayerHudType == PHT_STANDARD || mPlayerHudType == PHT_DRAG) {
        pSpeedBreakerMeter = new ("Hud_SpeedBreakerMeter", 0) SpeedBreakerMeter(this, pkg_name, player_number);
        pGetAwayMeter = new ("Hud_GetAwayMeter", 0) GetAwayMeter(this, pkg_name, player_number);
    }

    if (TheOnlineManager.IsOnlineRace()) {
        pOnlineSupport = new ("Hud_OnlineHUDSupport", 0) OnlineHUDSupport(pkg_name);
    } else {
        const u32 FEObj_OnlineDATA01 = 0xC18C12FD;
        const u32 FEObj_OnlineDATA02 = 0xC18C12FE;
        FEngSetInvisible(static_cast<FEObject *>(FEngFindString(pkg_name, FEObj_OnlineDATA01)));
        FEngSetInvisible(static_cast<FEObject *>(FEngFindString(pkg_name, FEObj_OnlineDATA02)));
    }

    CurrentHudFeatures = 0;
    SetHudFeatures(0xFFFFFFFF);
    SetHudFeatures(0);
    JoyEnable();
}

FEngHud::~FEngHud() {
    delete pSpeedometer;
    pSpeedometer = nullptr;
    delete pTachometer;
    pTachometer = nullptr;
    delete pTachometerDrag;
    pTachometerDrag = nullptr;
    delete pShiftUpdater;
    pShiftUpdater = nullptr;
    delete pTurboMeter;
    pTurboMeter = nullptr;
    delete pEngineTemp;
    pEngineTemp = nullptr;
    delete pNitrous;
    pNitrous = nullptr;
    delete pSpeedBreakerMeter;
    pSpeedBreakerMeter = nullptr;
    delete pRaceOverMessage;
    pRaceOverMessage = nullptr;
    delete pGenericMessage;
    pGenericMessage = nullptr;
    delete pRaceInformation;
    pRaceInformation = nullptr;
    delete pLeaderBoard;
    pLeaderBoard = nullptr;
    delete pPursuitBoard;
    pPursuitBoard = nullptr;
    delete pMilestoneBoard;
    pMilestoneBoard = nullptr;
    delete pBustedMeter;
    pBustedMeter = nullptr;
    delete pTimeExtension;
    pTimeExtension = nullptr;
    delete pCostToState;
    pCostToState = nullptr;
    delete pReputation;
    pReputation = nullptr;
    delete pHeatMeter;
    pHeatMeter = nullptr;
    delete pWrongWIndi;
    pWrongWIndi = nullptr;
    delete pOnlineSupport;
    pOnlineSupport = nullptr;
    delete p321Go;
    p321Go = nullptr;
    if (pRadarDetector != nullptr) {
        delete pRadarDetector;
        pRadarDetector = nullptr;
    }
    delete pMinimap;
    pMinimap = nullptr;
    delete pGetAwayMeter;
    pGetAwayMeter = nullptr;
    delete pMenuZoneTrigger;
    pMenuZoneTrigger = nullptr;
    delete pInfractions;
    pInfractions = nullptr;

    cFEng::Get()->PopNoControlPackage(pPackageName);

    if (mPlayerHudType != PHT_SPLIT2 && mPlayerHudType != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.UnloadRequiredResources(mPlayerHudType);
    }
}

void FEngHud::Update(IPlayer *player, float dT) {
    ProfileNode profile_node("FEngHud::Update", 0);

    HudFeaturesType hudFeatures = DetermineHudFeatures(player);
    if (hudFeatures != CurrentHudFeatures) {
        SetHudFeatures(hudFeatures);
    }

    if (mActionQ.IsEnabled()) {
        if (!TheGameFlowManager.IsLoading() && !bIsRestartingRace && (UTL::Collections::Singleton<INIS>::Get() == nullptr) &&
            FadeScreen::IsFadeScreenOn()) {
            new EFadeScreenOff(FEHASH_15_IN);
        }
    }

    SetWideScreenMode();

    if (hudFeatures != 0) {
        if ((pSpeedometer != nullptr) && pSpeedometer->IsElementVisible()) {
            pSpeedometer->Update(player);
        }
        if ((pTachometer != nullptr) && pTachometer->IsElementVisible()) {
            pTachometer->Update(player);
        }
        if ((pTachometerDrag != nullptr) && pTachometerDrag->IsElementVisible()) {
            pTachometerDrag->Update(player);
        }
        if ((pShiftUpdater != nullptr) && pShiftUpdater->IsElementVisible()) {
            pShiftUpdater->Update(player);
        }
        if ((pMinimap != nullptr) && pMinimap->IsElementVisible()) {
            pMinimap->Update(player);
        }
        if ((pRaceInformation != nullptr) && pRaceInformation->IsElementVisible()) {
            pRaceInformation->Update(player);
        }
        if ((pLeaderBoard != nullptr) && pLeaderBoard->IsElementVisible()) {
            pLeaderBoard->Update(player);
        }
        if ((pPursuitBoard != nullptr) && pPursuitBoard->IsElementVisible()) {
            pPursuitBoard->Update(player);
        }
        if ((pMilestoneBoard != nullptr) && pMilestoneBoard->IsElementVisible()) {
            pMilestoneBoard->Update(player);
        }
        if ((pBustedMeter != nullptr) && pBustedMeter->IsElementVisible()) {
            pBustedMeter->Update(player);
        }
        if ((pTimeExtension != nullptr) && pTimeExtension->IsElementVisible()) {
            pTimeExtension->Update(player);
        }
        if ((pCostToState != nullptr) && pCostToState->IsElementVisible()) {
            pCostToState->Update(player);
        }
        if ((pReputation != nullptr) && pReputation->IsElementVisible()) {
            pReputation->Update(player);
        }
        if ((pHeatMeter != nullptr) && pHeatMeter->IsElementVisible()) {
            pHeatMeter->Update(player);
        }
        if ((pNitrous != nullptr) && pNitrous->IsElementVisible()) {
            pNitrous->Update(player);
        }
        if ((pSpeedBreakerMeter != nullptr) && pSpeedBreakerMeter->IsElementVisible()) {
            pSpeedBreakerMeter->Update(player);
        }
        if ((pGetAwayMeter != nullptr) && pGetAwayMeter->IsElementVisible()) {
            pGetAwayMeter->Update(player);
        }
        if ((pRaceOverMessage != nullptr) && pRaceOverMessage->IsElementVisible()) {
            pRaceOverMessage->Update(player);
        }
        if ((pGenericMessage != nullptr) && pGenericMessage->IsElementVisible()) {
            pGenericMessage->Update(player);
        }
        if ((pTurboMeter != nullptr) && pTurboMeter->IsElementVisible()) {
            pTurboMeter->Update(player);
        }
        if ((pEngineTemp != nullptr) && pEngineTemp->IsElementVisible()) {
            pEngineTemp->Update(player);
        }
        if ((p321Go != nullptr) && p321Go->IsElementVisible()) {
            p321Go->Update(player);
        }
        if ((pRadarDetector != nullptr) && pRadarDetector->IsElementVisible()) {
            pRadarDetector->Update(player);
        }
        if ((pMenuZoneTrigger != nullptr) && pMenuZoneTrigger->IsElementVisible()) {
            pMenuZoneTrigger->Update(player);
        }
        if ((pWrongWIndi != nullptr) && pWrongWIndi->IsElementVisible()) {
            pWrongWIndi->Update(player);
        }
        if (pOnlineSupport != nullptr) {
            pOnlineSupport->Update(player);
        }
        if ((pInfractions != nullptr) && pInfractions->IsElementVisible()) {
            pInfractions->Update(player);
        }

        if (MemoryCard::GetInstance()->AutoSaveRequested()) {
            MemoryCard::GetInstance()->SetHUDLoaded();
        }
    }

    JoyHandle(player);
}

void FEngHud::FadeAll(bool fadeIn) {
    if (fadeIn) {
        const u32 FEObj_FADEIN = 0xBCC00F05;

        cFEng::Get()->QueuePackageMessage(FEObj_FADEIN, pPackageName, nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("DEACTIVATE"), pPackageName, nullptr);
    } else {
        const u32 FEObj_FADEOUT = 0x54C20A66;

        cFEng::Get()->QueuePackageMessage(FEObj_FADEOUT, pPackageName, nullptr);
        cFEng::Get()->QueuePackageMessage(FEHashUpper("ACTIVATE"), pPackageName, nullptr);
    }
}

void FEngHud::SetInPursuit(bool inPursuit) {
    if (mInPursuit != inPursuit) {
        mInPursuit = inPursuit;
    }
}

void FEngHud::JoyDisable() {
    JoystickPort port = static_cast<JoystickPort>(pPlayer->GetControllerPort());
    if (mActionQ.IsEnabled()) {
        mActionQ.Enable(false);
        mActionQ.Flush();
    }
}

void FEngHud::JoyEnable() {
    JoystickPort port = static_cast<JoystickPort>(pPlayer->GetControllerPort());
    if (!mActionQ.IsEnabled()) {
        mActionQ.SetPort(port);
        mActionQ.Enable(true);
        mActionQ.Flush();
    }
}

void FEngHud::JoyHandle(IPlayer *player) {
    if ((player == nullptr) || (player->GetSettings() == nullptr)) {
        mActionQ.SetPort(-1);
        mActionQ.SetConfig(0, "FEngHud");
    } else {
        bool wheel_connected = false;
        if (player->GetSteeringDevice() != nullptr) {
            if (player->GetSteeringDevice()->IsConnected()) {
                wheel_connected = true;
            }
        }

        mActionQ.SetPort(player->GetControllerPort());
        mActionQ.SetConfig(player->GetSettings()->GetControllerAttribs(CA_HUD, wheel_connected), "FEngHud");

        if (!mActionQ.IsEmpty() && !MemoryCard::GetInstance()->IsAutoSaving() && !MemoryCard::GetInstance()->AutoSaveRequested()) {
            ActionRef aRef = mActionQ.GetAction();

            if (CurrentHudFeatures) {
                switch (aRef.ID()) {
                    case HUDACTION_PAUSEREQUEST:
                        new EPause(player->GetSettingsIndex(), 0, 0);
                        break;

                    case HUDACTION_ENGAGE_EVENT:
                        if (!FEDatabase->IsLANMode() && !FEDatabase->IsOnlineMode()) {
                            ISimable *isimable = player->GetSimable();
                            IVehicleAI *vehicleai;
                            IMenuZoneTrigger *izone;
                            ePursuitStatus pursuitStatus;
                            IPursuit *ipursuit;

                            if (isimable->QueryInterface(&vehicleai)) {
                                ipursuit = vehicleai->GetPursuit();
                                if (ipursuit != nullptr) {
                                    pursuitStatus = ipursuit->GetPursuitStatus();
                                    if (pursuitStatus == PS_COOL_DOWN) {
                                        if (QueryInterface(&izone)) {
                                            if (izone->IsPlayerInsideTrigger()) {
                                                if (izone->IsType("safehouse")) {
                                                    ipursuit->EndPursuitEnteringSafehouse();
                                                    break;
                                                }
                                            }
                                        }
                                    }
                                }
                            }

                            if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing &&
                                !GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()) {
                                new EShowResults(FERESULTTYPE_RACE, true);
                            } else if (mInPursuit) {
                                new EShowResults(FERESULTTYPE_PURSUIT, true);
                            } else {
                                if (QueryInterface(&izone)) {
                                    if (izone->IsPlayerInsideTrigger()) {
                                        izone->RequestEventInfoDialog(mActionQ.GetPort());
                                        izone->RequestZoneInfoDialog(mActionQ.GetPort());
                                    }
                                }
                            }
                        }
                        break;

                    case HUDACTION_PAD_LEFT:
                        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                            new EWorldMapOn();
                        }
                        break;

                    case HUDACTION_PAD_DOWN:
                        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                            if (!FEDatabase->IsDDay()) {
                                new ERaceSheetOn(0);
                            }
                        }
                        break;

                    case HUDACTION_PAD_RIGHT:
                        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                            new EShowSMS(-1);
                        }
                        break;
                }
            }
        }

        while (!mActionQ.IsEmpty()) {
            mActionQ.PopAction();
        }
    }
}

HudFeaturesType FEngHud::DetermineHudFeatures(IPlayer *player) {
    HudFeaturesType hud_features = 0;

    eView *view = eGetView(player->GetRenderPort(), false);
    CameraMover *cammover = nullptr;
    if (view != nullptr) {
        cammover = view->GetCameraMover();
    }

    if (cammover == nullptr) {
        return 0;
    }
    if (cammover->GetType() != CM_DRIVE_CUBIC) {
        return 0;
    }
    if (cammover->GetLookbackAngle()) {
        return 0;
    }

    if (GRaceStatus::Exists()) {
        if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing && bIsRestartingRace) {
            return 0;
        }
    }

    if (FEManager::IsPaused()) {
        return 0;
    }
    if (Sim::GetState() != Sim::STATE_ACTIVE) {
        return 0;
    }
    if (cFEng::Get()->IsPackagePushed("Pause_Main.fng")) {
        return 0;
    }
    if (cFEng::Get()->IsPackagePushed("MC_Main_GC.fng")) {
        return 0;
    }
    if (cFEng::Get()->IsPackagePushed("PostRace_Results.fng")) {
        return 0;
    }
    if (!TheHudResourceManager.AreResourcesLoaded(mPlayerHudType)) {
        return 0;
    }
    if (UTL::Collections::Singleton<INIS>::Get() != nullptr) {
        return 0;
    }
    if (mCurrentWidescreenSetting != FEDatabase->GetVideoSettings()->WideScreen) {
        return 0;
    }
    if (TheICEManager.IsEditorOn()) {
        return 0;
    }
    if (TheGameFlowManager.IsLoading()) {
        return 0;
    }

    bool EnableMinimap = (TrackInfo::GetLoadedTrackInfo()->TrackNumber == 2000);
    if (GRaceStatus::Get().GetRaceParameters() == nullptr) {
        if (FEDatabase->GetGameplaySettings()->ExploringMiniMapMode == 2) {
            EnableMinimap = false;
        }
    } else {
        if (FEDatabase->GetGameplaySettings()->RacingMiniMapMode == 2) {
            EnableMinimap = false;
        }
    }

    if (EnableMinimap && !TheOnlineManager.IsOnlineRace()) {
        hud_features |= 0x10000;
        hud_features |= 0x4000;
    }

    if (GRaceStatus::IsDragRace()) {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->PositionOn) {
            hud_features |= 0x8000000;
        }
    } else {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->GaugesOn) {
            hud_features |= 0x8000000;
        }
    }

    if (FEDatabase->GetPlayerSettings(PlayerNumber)->GaugesOn) {
        hud_features |= 0x2;

        if (mHasTurbo) {
            hud_features |= 0x20000;
        }

        if (GRaceStatus::IsDragRace()) {
            hud_features |= 0x40;
        }

        hud_features |= 0x800;
        hud_features |= 0x40000;
    }

    if (TheOnlineManager.IsOnlineRace()) {
        hud_features |= 0x40800;
    }

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing && !pursuitRace) {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->LeaderboardOn) {
            hud_features |= 0x8;
        }
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->PositionOn) {
            hud_features |= 0x4000000;
        }
    } else {
        hud_features |= 0x100000;
        hud_features |= 0x400000000ULL;
    }

    if (FEDatabase->GetPlayerSettings(PlayerNumber)->SplitTimeType != 4) {
        hud_features |= 0x10;
    }

    if (GRaceStatus::IsTollboothRace()) {
        hud_features |= 0x2000000;
    }

    if (player->GetSettings()->ScoreOn) {
        if (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career ||
            ((GRaceStatus::Get().GetRaceParameters() != nullptr) && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace())) {
            hud_features |= 0x1000;
        }
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        hud_features |= 0x20;
    }

    IRaceOverMessage *iraceover;
    if (QueryInterface(&iraceover)) {
        if (iraceover->ShouldShowRaceOverMessage()) {
            hud_features |= 0x4;
        }
    }

    hud_features |= 0x1000000;
    hud_features |= 0x10000000;
    hud_features |= 0x100000000ULL;
    hud_features |= 0x80000;
    hud_features |= 0x400;
    hud_features |= 0x20000000;
    hud_features |= 0x400000;
    hud_features |= 0x200000000ULL;
    hud_features |= 0x800000;
    hud_features |= 0x200000;

    if (TheOnlineManager.IsOnlineRace()) {
        hud_features |= 0x331e80400ULL;
    }

    return hud_features;
}

bool FEngHud::AreResourcesLoaded() {
    return TheHudResourceManager.AreResourcesLoaded(mPlayerHudType);
}

void FEngHud::SetHudFeatures(HudFeaturesType hud_features) {
    HudFeaturesType xor_hud_features = CurrentHudFeatures ^ hud_features;
    if (pSpeedometer != nullptr && (xor_hud_features & 0x8000000)) {
        pSpeedometer->Toggle(hud_features);
    }
    if (pTachometer != nullptr && (xor_hud_features & 0x2)) {
        pTachometer->Toggle(hud_features);
    }
    if (pTachometerDrag != nullptr && (xor_hud_features & 0x2)) {
        pTachometerDrag->Toggle(hud_features);
    }
    if (pShiftUpdater != nullptr && (xor_hud_features & 0x20000000)) {
        pShiftUpdater->Toggle(hud_features);
    }
    if (pTurboMeter != nullptr && (xor_hud_features & 0x20000)) {
        pTurboMeter->Toggle(hud_features);
    }
    if (pEngineTemp != nullptr && (xor_hud_features & 0x40)) {
        pEngineTemp->Toggle(hud_features);
    }
    if (pNitrous != nullptr && (xor_hud_features & 0x800)) {
        pNitrous->Toggle(hud_features);
    }
    if (pSpeedBreakerMeter != nullptr && (xor_hud_features & 0x40000)) {
        pSpeedBreakerMeter->Toggle(hud_features);
    }
    if (pHeatMeter != nullptr && (xor_hud_features & 0x4000)) {
        pHeatMeter->Toggle(hud_features);
    }
    if (pMinimap != nullptr && (xor_hud_features & 0x10000)) {
        pMinimap->Toggle(hud_features);
    }
    if (pGetAwayMeter != nullptr && (xor_hud_features & 0x200)) {
        pGetAwayMeter->Toggle(hud_features);
    }
    if (pMenuZoneTrigger != nullptr && (xor_hud_features & 0x400000)) {
        pMenuZoneTrigger->Toggle(hud_features);
    }
    if (pRaceInformation != nullptr && (xor_hud_features & 0x4000000)) {
        pRaceInformation->Toggle(hud_features);
    }
    if (pLeaderBoard != nullptr) {
        if ((xor_hud_features & 0x8) || (xor_hud_features & 0x10)) {
            pLeaderBoard->Toggle(hud_features);
        }
    }
    if (pPursuitBoard != nullptr && (xor_hud_features & 0x100000)) {
        pPursuitBoard->Toggle(hud_features);
    }
    if (pMilestoneBoard != nullptr && (xor_hud_features & 0x400000000ULL)) {
        pMilestoneBoard->Toggle(hud_features);
    }
    if (pBustedMeter != nullptr && (xor_hud_features & 0x800000)) {
        pBustedMeter->Toggle(hud_features);
    }
    if (pTimeExtension != nullptr && (xor_hud_features & 0x2000000)) {
        pTimeExtension->Toggle(hud_features);
    }
    if (pCostToState != nullptr && (xor_hud_features & 0x1000)) {
        pCostToState->Toggle(hud_features);
    }
    if (pReputation != nullptr && (xor_hud_features & 0x1000)) {
        pReputation->Toggle(hud_features);
    }
    if (pWrongWIndi != nullptr && (xor_hud_features & 0x20)) {
        pWrongWIndi->Toggle(hud_features);
    }
    if (pRaceOverMessage != nullptr && (xor_hud_features & 0x4)) {
        pRaceOverMessage->Toggle(hud_features);
    }
    if (pGenericMessage != nullptr && (xor_hud_features & 0x1000000)) {
        pGenericMessage->Toggle(hud_features);
    }
    if (pRadarDetector != nullptr && (xor_hud_features & 0x200000)) {
        pRadarDetector->Toggle(hud_features);
    }
    if (p321Go != nullptr && (xor_hud_features & 0x400)) {
        p321Go->Toggle(hud_features);
    }
    if (pInfractions != nullptr && (xor_hud_features & 0x200000000ULL)) {
        pInfractions->Toggle(hud_features);
    }
    CurrentHudFeatures = hud_features;
}

void FEngHud::SetWideScreenMode() {
    if (mCurrentWidescreenSetting != FEDatabase->GetVideoSettings()->WideScreen) {
        mCurrentWidescreenSetting = FEDatabase->GetVideoSettings()->WideScreen;
        if (static_cast<int>(mCurrentWidescreenSetting) != 0) {
            const u32 FEObj_WIDESCREENMODE = 0x62ED04EC;

            cFEng::Get()->QueuePackageMessage(FEObj_WIDESCREENMODE, pPackageName, nullptr);

            if (pMinimap != nullptr) {
                static_cast<Minimap *>(pMinimap)->AdjustForWidescreen(true);
            }
        } else {
            const u32 FEObj_NORMAL_MODE = 0x53EC068C;

            cFEng::Get()->QueuePackageMessage(FEObj_NORMAL_MODE, pPackageName, nullptr);

            if (pMinimap != nullptr) {
                static_cast<Minimap *>(pMinimap)->AdjustForWidescreen(false);
            }
        }
    }
}

void HideEverySingleHud() {
    for (IHud *const *iter = UTL::Collections::Listable<IHud, 2>::GetList().begin(); iter != UTL::Collections::Listable<IHud, 2>::GetList().end();
         iter++) {
        (*iter)->HideAll();
    }
}

void FEngHud::RefreshMiniMapItems() {
    if (pMinimap != nullptr) {
        static_cast<Minimap *>(pMinimap)->RefreshMapItems();
    }
}

bool FEngHud::ShouldRearViewMirrorBeVisible(EVIEW_ID viewId) {
    eView *view = eGetView(viewId, false);
    IPlayer *player = IPlayer::First(PLAYER_LOCAL);

    if (player != nullptr) {
        if ((player->GetHud() != nullptr) && !player->GetHud()->IsHudVisible()) {
            return false;
        }
    }

    CameraMover *camMover = nullptr;
    if (view != nullptr) {
        camMover = view->GetCameraMover();
    }

    if ((camMover != nullptr) && camMover->GetType() == CM_DRIVE_CUBIC) {
        if (camMover->GetLookbackAngle()) {
            return false;
        }
    }

    if (FEManager::ShouldPauseSimulation(true)) {
        return false;
    }

    if (FEDatabase == nullptr) {
        return false;
    }

    if (!FEDatabase->GetGameplaySettings()->RearviewOn) {
        return false;
    }

    ePlayerSettingsCameras playerCam = FEDatabase->GetPlayerSettings(viewId - 1)->CurCam;
    if (playerCam >= PSC_CLOSE && playerCam <= PSC_PURSUIT) {
        return false;
    }

    if (!cFEng::Get()->IsPackagePushed("HUD_SingleRace.fng")) {
        return false;
    }
    return true;
}

float FEngHud::ChooseMaxRpmTextureNumber(float maxRpm) {
    if (maxRpm < 7000.0f) {
        return 7000.0f;
    }
    if (maxRpm < 8000.0f) {
        return 8000.0f;
    }
    if (maxRpm < 9000.0f) {
        return 9000.0f;
    }
    return 10000.0f;
}
