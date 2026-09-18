#include "Speed/Indep/Src/Misc/Profiler.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include <cstdio>
#include "Speed/Indep/Libs/Support/Utility/UCollections.h"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMinimapStreamer.hpp"
#include "Speed/Indep/Src/Misc/Config.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/Frontend/Database/RaceDB.hpp"
#include "Speed/Indep/Src/Frontend/Database/VehicleDB.hpp"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/pvehicle.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/engine.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/Camera/ICE/ICEManager.hpp"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/FEng/FEList.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/Event.hpp"
#include "Speed/Indep/Src/Frontend/FEManager.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeBustedMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeCostToState.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeCountdown.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeEngineTempGauge.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeGenericMessage.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeGetawayMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeHeatMeter.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeInfractions.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeLeaderBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMenuZoneTrigger.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeMilestoneBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeNitrousGauge.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FePursuitBoard.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRadarDetector.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRaceInformation.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FeRaceOverMessage.hpp"
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
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_PostRace.hpp"
#include "Speed/Indep/Src/Generated/Events/EPause.hpp"
#include "Speed/Indep/Src/Generated/Events/ERaceSheetOn.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowResults.hpp"
#include "Speed/Indep/Src/Generated/Events/EShowSMS.hpp"
#include "Speed/Indep/Src/Generated/Events/EWorldMapOn.hpp"
#include "Speed/Indep/Src/Gameplay/GRaceStatus.h"
#include "Speed/Indep/Src/Interfaces/IMenuZoneTrigger.h"
#include "Speed/Indep/Src/Interfaces/SimActivities/INIS.h"
#include "Speed/Indep/Src/Interfaces/Simables/IAI.h"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Sim/Simulation.h"
#include "Speed/Indep/Src/World/OnlineManager.hpp"
#include "Speed/Indep/Src/World/TrackInfo.hpp"
#include "Speed/Indep/Src/World/TrackStreamer.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyE.hpp"
#include "Speed/Indep/Src/World/CarPartID.h"

HudResourceManager TheHudResourceManager;

int HudResourceManager::mPhase = 0;
int HudResourceManager::mCustIndex = (int)0xFFFFFFFF;
int HudResourceManager::mTachLinesHash = 0;
ResourceFile *HudResourceManager::pMiniMapTexture = nullptr;
ePlayerHudType HudResourceManager::LoadingResourcesForHudType = PHT_NONE;
const char *HudResourceManager::mPackageName = nullptr;
char HudResourceManager::mCustHudTexPackName[32];
uint32 HudResourceManager::mCustomizeHUDTexTextureResources[5];

extern const char *HudDragTexturePackFilename;
extern const char *HudSingleRaceTexturePackFilename;
extern const char *HudSplitScreenTexturePackFilename;
extern const char *HudDragSplitScreenTexturePackFilename;

bool FEngHud::bIsRestartingRace = false;

// Package message hashes (FEng hashes of the message names)

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
        if (raceParams != nullptr && !raceParams->GetIsPursuitRace()) {
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
            unsigned char r = static_cast<unsigned char>(part->GetAppliedAttributeIParam(bStringHash("RED"), 0));
            unsigned char g = static_cast<unsigned char>(part->GetAppliedAttributeIParam(bStringHash("GREEN"), 0));
            unsigned char b = static_cast<unsigned char>(part->GetAppliedAttributeIParam(bStringHash("BLUE"), 0));
            colour = (0xFF << 24) | (r << 16) | (g << 8) | b;
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
            if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 12) {
                    bSNPrintf(texture_name, texture_name_size, "MINI_MAP_UNLOCK_1");
                } else if (FEDatabase->GetCareerSettings()->GetCurrentBin() > 8) {
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

    bool isDrag = hudType == PHT_DRAG;
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
        } else if (RedLineRPM >= 6000.0f) {
            redlineRotation = isDrag ? 22.5f : 123.0f;
        } else {
            redlineRotation = isDrag ? 17.25f : 110.0f;
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
    pHudTextures->BeginLoading(reinterpret_cast<void (*)(void *)>(static_cast<void (*)(int32)>(LoadingCompleteCallbackBridge)), this);
    mHudResourcesState = HRM_LOADING_IN_PROGRESS;
    LoadingResourcesForHudType = ht;
    mPackageName = pkg_name;
}

void HudResourceManager::LoadingCompleteCallback() {
    mPhase++;

    if (mPhase == 1) {
        char header[64];
        char minimapName[32];
        bSPrintf(header, "");
        bSPrintf(minimapName, "");

        if (ChooseMinimapTextureName(LoadingResourcesForHudType, minimapName, 0x20, header, 0x40)) {
            gChoppedMiniMapManager->SetMapHeader(minimapName);
            pMiniMapTexture = LoadResourceFile(header, RESOURCE_FILE_TRACK, 0, NULL, NULL, 0, 0);

            unsigned int textures_to_load[16];
            int num_textures_to_load = 1;
            textures_to_load[0] = bStringHash(minimapName);
            eLoadStreamingTexture(textures_to_load, num_textures_to_load, LoadingCompleteCallbackBridge, reinterpret_cast<uintptr_t>(this), 0);
        } else {
            LoadingCompleteCallback();
        }
        return;
    }

    if (mPhase == 2) {
        if (GetCustomHudTexPackFilename(LoadingResourcesForHudType, mCustHudTexPackName)) {
            float redlineRotation;
            ChooseLoadableTextures(LoadingResourcesForHudType, mTachLinesHash, redlineRotation);
            FEngSetMultiImageRot(reinterpret_cast<FEMultiImage *>(FEngFindObject(mPackageName, 0xCDFCE1B0)), redlineRotation);
            eLoadStreamingTexturePack(mCustHudTexPackName, LoadedCustomHudTexturePackCallbackBridge, reinterpret_cast<uintptr_t>(this), 0);
        } else {
            float redlineRotation;
            ChooseLoadableTextures(LoadingResourcesForHudType, mTachLinesHash, redlineRotation);
            FEngSetMultiImageRot(reinterpret_cast<FEMultiImage *>(FEngFindObject(mPackageName, 0xCDFCE1B0)), redlineRotation);
            FEngSetTextureHash(mPackageName, 0x309878BC, mTachLinesHash);

            uint32 nameHash = mTachLinesHash;
            eLoadStreamingTexture(&nameHash, 1, LoadingCompleteCallbackBridge, reinterpret_cast<uintptr_t>(this), 0);
        }
        return;
    }

    if (mPhase == 3) {
        TheHudResourceManager.mHudResourcesState = HRM_LOADED;
        cFEng::Get()->MakeLoadedPackagesDirty();
        SetSoundControlState(false, SNDSTATE_STOP_MUSIC, "HUDLoaded");
    }
}

void HudResourceManager::LoadedCustomHudTexturePackCallback() {
    int hud_num = mCustIndex;

    mCustomizeHUDTexTextureResources[0] = FEngHashString("TACH_FILL_%2.2d", hud_num);
    mCustomizeHUDTexTextureResources[1] = mTachLinesHash;
    mCustomizeHUDTexTextureResources[2] = FEngHashString("TURBO_LINES_%2.2d", hud_num);
    mCustomizeHUDTexTextureResources[3] = FEngHashString("TACH_NEEDLE_%2.2d", hud_num);
    mCustomizeHUDTexTextureResources[4] = FEngHashString("TURBO_NEEDLE_%2.2d", hud_num);
    eLoadStreamingTexture(mCustomizeHUDTexTextureResources, 5, LoadedCustomHudTexturesCallbackBridge, reinterpret_cast<uintptr_t>(this), 0);
}

void HudResourceManager::LoadedCustomHudTexturesCallback() {
    for (unsigned int mPhaseCust = 0; mPhaseCust <= 4; mPhaseCust++) {
        int custColour;
        int fengObjHash = 0;
        CAR_SLOT_ID carSlotIdForColour = static_cast<CAR_SLOT_ID>(0);
        switch (static_cast<int>(mPhaseCust)) {
        case 0:
            fengObjHash = 0x05D19F25;
            carSlotIdForColour = CARSLOTID_HUD_BACKING_COLOUR;
            break;
        case 1:
            fengObjHash = 0x309878BC;
            carSlotIdForColour = CARSLOTID_HUD_CHARACTER_COLOUR;
            break;
        case 2:
            fengObjHash = 0xC62AD685;
            carSlotIdForColour = CARSLOTID_HUD_CHARACTER_COLOUR;
            break;
        case 3:
            fengObjHash = 0xF0250DAC;
            carSlotIdForColour = CARSLOTID_HUD_NEEDLE_COLOUR;
            break;
        case 4:
            fengObjHash = 0x6D5ECE44;
            carSlotIdForColour = CARSLOTID_HUD_NEEDLE_COLOUR;
            break;
        default:
            break;
        }

        custColour = GetCustomHudColour(LoadingResourcesForHudType, carSlotIdForColour);
        if (custColour != 0) {
            FEngSetColor(FEngFindObject(mPackageName, fengObjHash), static_cast<unsigned int>(custColour));
        }
        unsigned int tex = mCustomizeHUDTexTextureResources[mPhaseCust];
        FEImage *img = FEngFindImage(mPackageName, fengObjHash);
        FEngSetTextureHash(img, tex);
    }

    int custColour = GetCustomHudColour(LoadingResourcesForHudType, CARSLOTID_HUD_CHARACTER_COLOUR);
    if (custColour != 0) {
        FEngSetColor(FEngFindObject(mPackageName, 0xC3383B63), static_cast<unsigned int>(custColour));
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

    if (bStrCmp(mCustHudTexPackName, "") == 0) {
        if (mTachLinesHash != 0) {
            unsigned int tex = mTachLinesHash;
            eUnloadStreamingTexture(&tex, 1);
            mTachLinesHash = 0;
        }
    } else {
        eUnloadStreamingTexture(mCustomizeHUDTexTextureResources, 5);
        for (unsigned int i = 0; i <= 4; i++) {
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
    : UTL::COM::Object(0x14), IHud(this), mPlayerHudType(ht), PlayerNumber(player_number), mActionQ(true) {
    mInPursuit = false;
    mHasTurbo = false;
    pSpeedometer = nullptr;
    pTachometer = nullptr;
    pTachometerDrag = nullptr;
    pShiftUpdater = nullptr;
    pCostToState = nullptr;
    pReputation = nullptr;
    pHeatMeter = nullptr;
    pTurboMeter = nullptr;
    pEngineTemp = nullptr;
    pNitrous = nullptr;
    pSpeedBreakerMeter = nullptr;
    pRaceOverMessage = nullptr;
    pGenericMessage = nullptr;
    pRaceInformation = nullptr;
    pLeaderBoard = nullptr;
    pPursuitBoard = nullptr;
    pMilestoneBoard = nullptr;
    pBustedMeter = nullptr;
    pTimeExtension = nullptr;
    pWrongWIndi = nullptr;
    pOnlineSupport = nullptr;
    p321Go = nullptr;
    pRadarDetector = nullptr;
    pMinimap = nullptr;
    pGetAwayMeter = nullptr;
    pMenuZoneTrigger = nullptr;
    pInfractions = nullptr;
    mCurrentWidescreenSetting = false;
    pPlayer = player;
    pPackageName = pkg_name;

    if (mPlayerHudType != PHT_SPLIT2 && mPlayerHudType != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.LoadRequiredResources(mPlayerHudType, pPackageName);
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

    FEngSetInvisible(FEngFindString(pkg_name, 0xC18C12FD));
    FEngSetInvisible(FEngFindString(pkg_name, 0xC18C12FE));

    CurrentHudFeatures = 0;
    SetHudFeatures(0xFFFFFFFF);
    SetHudFeatures(0);

    JoyEnable();
}

FEngHud::~FEngHud() {
    delete pSpeedometer, pSpeedometer = nullptr;
    delete pTachometer, pTachometer = nullptr;
    delete pTachometerDrag, pTachometerDrag = nullptr;
    delete pShiftUpdater, pShiftUpdater = nullptr;
    delete pTurboMeter, pTurboMeter = nullptr;
    delete pEngineTemp, pEngineTemp = nullptr;
    delete pNitrous, pNitrous = nullptr;
    delete pSpeedBreakerMeter, pSpeedBreakerMeter = nullptr;
    delete pRaceOverMessage, pRaceOverMessage = nullptr;
    delete pGenericMessage, pGenericMessage = nullptr;
    delete pRaceInformation, pRaceInformation = nullptr;
    delete pLeaderBoard, pLeaderBoard = nullptr;
    delete pPursuitBoard, pPursuitBoard = nullptr;
    delete pMilestoneBoard, pMilestoneBoard = nullptr;
    delete pBustedMeter, pBustedMeter = nullptr;
    delete pTimeExtension, pTimeExtension = nullptr;
    delete pCostToState, pCostToState = nullptr;
    delete pReputation, pReputation = nullptr;
    delete pHeatMeter, pHeatMeter = nullptr;
    delete pWrongWIndi, pWrongWIndi = nullptr;
    delete pOnlineSupport, pOnlineSupport = nullptr;
    delete p321Go, p321Go = nullptr;
    if (pRadarDetector) {
        delete pRadarDetector, pRadarDetector = nullptr;
    }
    delete pMinimap, pMinimap = nullptr;
    delete pGetAwayMeter, pGetAwayMeter = nullptr;
    delete pMenuZoneTrigger, pMenuZoneTrigger = nullptr;
    delete pInfractions, pInfractions = nullptr;

    cFEng::Get()->PopNoControlPackage(pPackageName);

    if (mPlayerHudType != PHT_SPLIT2 && mPlayerHudType != PHT_DRAG_SPLIT2) {
        TheHudResourceManager.UnloadRequiredResources(mPlayerHudType);
    }
}

void FEngHud::Update(IPlayer *player, float dT) {
    ProfileNode profile_node;
    HudFeaturesType hudFeatures = DetermineHudFeatures(player);

    if (hudFeatures != CurrentHudFeatures) {
        SetHudFeatures(hudFeatures);
    }

    if (mActionQ.IsEnabled() && !TheGameFlowManager.IsLoading() && !bIsRestartingRace && !UTL::Collections::Singleton<INIS>::Get() &&
        FadeScreen::IsFadeScreenOn()) {
        new EFadeScreenOff(0x14035FB);
    }

    SetWideScreenMode();

    if (hudFeatures != 0) {
        if (pSpeedometer != nullptr && pSpeedometer->IsElementVisible()) {
        pSpeedometer->Update(player);
    }
    if (pTachometer != nullptr && pTachometer->IsElementVisible()) {
        pTachometer->Update(player);
    }
    if (pTachometerDrag != nullptr && pTachometerDrag->IsElementVisible()) {
        pTachometerDrag->Update(player);
    }
    if (pShiftUpdater != nullptr && pShiftUpdater->IsElementVisible()) {
        pShiftUpdater->Update(player);
    }
    if (pMinimap != nullptr && pMinimap->IsElementVisible()) {
        pMinimap->Update(player);
    }
    if (pRaceInformation != nullptr && pRaceInformation->IsElementVisible()) {
        pRaceInformation->Update(player);
    }
    if (pLeaderBoard != nullptr && pLeaderBoard->IsElementVisible()) {
        pLeaderBoard->Update(player);
    }
    if (pPursuitBoard != nullptr && pPursuitBoard->IsElementVisible()) {
        pPursuitBoard->Update(player);
    }
    if (pMilestoneBoard != nullptr && pMilestoneBoard->IsElementVisible()) {
        pMilestoneBoard->Update(player);
    }
    if (pBustedMeter != nullptr && pBustedMeter->IsElementVisible()) {
        pBustedMeter->Update(player);
    }
    if (pTimeExtension != nullptr && pTimeExtension->IsElementVisible()) {
        pTimeExtension->Update(player);
    }
    if (pCostToState != nullptr && pCostToState->IsElementVisible()) {
        pCostToState->Update(player);
    }
    if (pReputation != nullptr && pReputation->IsElementVisible()) {
        pReputation->Update(player);
    }
    if (pHeatMeter != nullptr && pHeatMeter->IsElementVisible()) {
        pHeatMeter->Update(player);
    }
    if (pNitrous != nullptr && pNitrous->IsElementVisible()) {
        pNitrous->Update(player);
    }
    if (pSpeedBreakerMeter != nullptr && pSpeedBreakerMeter->IsElementVisible()) {
        pSpeedBreakerMeter->Update(player);
    }
    if (pGetAwayMeter != nullptr && pGetAwayMeter->IsElementVisible()) {
        pGetAwayMeter->Update(player);
    }
    if (pRaceOverMessage != nullptr && pRaceOverMessage->IsElementVisible()) {
        pRaceOverMessage->Update(player);
    }
    if (pGenericMessage != nullptr && pGenericMessage->IsElementVisible()) {
        pGenericMessage->Update(player);
    }
    if (pTurboMeter != nullptr && pTurboMeter->IsElementVisible()) {
        pTurboMeter->Update(player);
    }
    if (pEngineTemp != nullptr && pEngineTemp->IsElementVisible()) {
        pEngineTemp->Update(player);
    }
    if (p321Go != nullptr && p321Go->IsElementVisible()) {
        p321Go->Update(player);
    }
    if (pRadarDetector != nullptr && pRadarDetector->IsElementVisible()) {
        pRadarDetector->Update(player);
    }
    if (pMenuZoneTrigger != nullptr && pMenuZoneTrigger->IsElementVisible()) {
        pMenuZoneTrigger->Update(player);
    }
    if (pWrongWIndi != nullptr && pWrongWIndi->IsElementVisible()) {
        pWrongWIndi->Update(player);
    }
    if (pOnlineSupport != nullptr) {
        pOnlineSupport->Update(player);
    }
        if (pInfractions != nullptr && pInfractions->IsElementVisible()) {
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
    pPlayer->GetControllerPort();

    if (mActionQ.IsEnabled()) {
        mActionQ.Enable(false);
        mActionQ.Flush();
    }
}

void FEngHud::JoyEnable() {
    int port = pPlayer->GetControllerPort();

    if (!mActionQ.IsEnabled()) {
        mActionQ.SetPort(port);
        mActionQ.Enable(true);
        mActionQ.Flush();
    }
}

void FEngHud::JoyHandle(IPlayer *player) {
    if (player == nullptr || player->GetSettings() == nullptr) {
        mActionQ.SetPort(-1);
        mActionQ.SetConfig(0, "FEngHud");
        return;
    }

    bool wheel_connected = false;
    if (player->GetSteeringDevice() != nullptr && player->GetSteeringDevice()->IsConnected()) {
        wheel_connected = true;
    }

    mActionQ.SetPort(player->GetControllerPort());
    mActionQ.SetConfig(player->GetSettings()->GetControllerAttribs(CA_HUD, wheel_connected), "FEngHud");

    if (!mActionQ.IsEmpty() && !MemoryCard::GetInstance()->IsAutoSaving() && !MemoryCard::GetInstance()->AutoSaveRequested()) {
        ActionRef action = mActionQ.GetAction();

        if (CurrentHudFeatures != 0) {
            switch (action.ID()) {
                case 0x2D: {
                    new EPause(player->GetSettingsIndex(), 0, 0);
                    break;
                }
                case 0x2E: {
                    if (!(FEDatabase->GetGameMode() & eFE_GAME_MODE_LAN) && !(FEDatabase->GetGameMode() & eFE_GAME_MODE_ONLINE)) {
                        IVehicleAI *vehicleAI;
                        if (player->GetSimable()->QueryInterface(&vehicleAI)) {
                            IPursuit *pursuit = vehicleAI->GetPursuit();
                            if (pursuit != nullptr) {
                                if (pursuit->GetPursuitStatus() == 2) {
                                    IMenuZoneTrigger *mzt;
                                    if (QueryInterface(&mzt)) {
                                        if (mzt->IsPlayerInsideTrigger()) {
                                            if (mzt->IsType("safehouse")) {
                                                pursuit->EndPursuitEnteringSafehouse();
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
                            IMenuZoneTrigger *mzt;
                            if (QueryInterface(&mzt)) {
                                if (mzt->IsPlayerInsideTrigger()) {
                                    mzt->RequestEventInfoDialog(mActionQ.GetPort());
                                    mzt->RequestZoneInfoDialog(mActionQ.GetPort());
                                }
                            }
                        }
                    }
                    break;
                }
                case 0x2F: {
                    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                        new EWorldMapOn();
                    }
                    break;
                }
                case 0x30: {
                    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                        if (FEDatabase->GetCareerSettings()->GetCurrentBin() <= 15) {
                            new ERaceSheetOn(0);
                        }
                    }
                    break;
                }
                case 0x31: {
                    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Roaming) {
                        new EShowSMS(-1);
                    }
                    break;
                }
            }
        }
    }

    while (!mActionQ.IsEmpty()) {
        mActionQ.PopAction();
    }
}

HudFeaturesType FEngHud::DetermineHudFeatures(IPlayer *player) {
    HudFeaturesType hud_features = 0;
    eView *view = eGetView(player->GetRenderPort(), false);
    CameraMover *cammover = nullptr;

    if (view != nullptr) {
        cammover = view->GetCameraMover();
    }

    if (cammover == nullptr || cammover->GetType() != CM_DRIVE_CUBIC || cammover->GetLookbackAngle() != 0 ||
        (GRaceStatus::Exists() && GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing && bIsRestartingRace) ||
        FEManager::IsPaused() || Sim::GetState() != Sim::STATE_ACTIVE || cFEng::Get()->IsPackagePushed("Pause_Main.fng") ||
        cFEng::Get()->IsPackagePushed("MC_Main_GC.fng") || cFEng::Get()->IsPackagePushed("PostRace_Results.fng") ||
        !TheHudResourceManager.AreResourcesLoaded(mPlayerHudType) || UTL::Collections::Singleton<INIS>::Get() != nullptr ||
        mCurrentWidescreenSetting != FEDatabase->GetVideoSettings()->WideScreen || TheICEManager.IsEditorOn() ||
        TheGameFlowManager.IsLoading()) {
        return 0;
    }

    bool EnableMinimap = TrackInfo::GetLoadedTrackInfo()->TrackNumber == 2000;

    if (GRaceStatus::Get().GetRaceParameters() == nullptr) {
        if (FEDatabase->GetGameplaySettings()->ExploringMiniMapMode == 2) {
            EnableMinimap = false;
        }
    } else {
        if (FEDatabase->GetGameplaySettings()->RacingMiniMapMode == 2) {
            EnableMinimap = false;
        }
    }

    if (EnableMinimap || TheOnlineManager.IsOnlineRace()) {
        hud_features |= HUD_FEATURE_MINIMAP;
        hud_features |= HUD_FEATURE_HEAT_METER;
    }

    if (GRaceStatus::IsDragRace()) {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->PositionOn) {
            hud_features |= HUD_FEATURE_SPEEDOMETER;
        }
    } else {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->GaugesOn) {
            hud_features |= HUD_FEATURE_SPEEDOMETER;
        }
    }

    if (FEDatabase->GetPlayerSettings(PlayerNumber)->GaugesOn) {
        hud_features |= HUD_FEATURE_TACHOMETER;
        if (mHasTurbo) {
            hud_features |= HUD_FEATURE_TURBO_METER;
        }
        if (GRaceStatus::IsDragRace()) {
            hud_features |= HUD_FEATURE_ENGINE_TEMP;
        }
        hud_features |= HUD_FEATURE_NITROUS;
        hud_features |= HUD_FEATURE_SPEEDBREAKER_METER;
    }

    bool pursuitRace = false;
    if (GRaceStatus::Get().GetRaceParameters() != nullptr) {
        pursuitRace = GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace();
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing && !pursuitRace) {
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->LeaderboardOn) {
            hud_features |= HUD_FEATURE_LEADERBOARD;
        }
        if (FEDatabase->GetPlayerSettings(PlayerNumber)->PositionOn) {
            hud_features |= HUD_FEATURE_RACEINFORMATION;
        }
    } else {
        hud_features |= HUD_FEATURE_PIP;
        hud_features |= HUD_FEATURE_MILESTONE_BOARD;
    }

    if (FEDatabase->GetPlayerSettings(PlayerNumber)->SplitTimeType != 4) {
        hud_features |= HUD_FEATURE_SPLIT_TIME;
    }

    if (GRaceStatus::IsTollboothRace()) {
        hud_features |= HUD_FEATURE_TIME_EXTENSION;
    }

    if (player->GetSettings()->ScoreOn &&
        (GRaceStatus::Get().GetRaceContext() == GRace::kRaceContext_Career ||
         (GRaceStatus::Get().GetRaceParameters() != nullptr && GRaceStatus::Get().GetRaceParameters()->GetIsPursuitRace()))) {
        hud_features |= HUD_FEATURE_COST_TO_STATE;
    }

    if (GRaceStatus::Get().GetPlayMode() == GRaceStatus::kPlayMode_Racing) {
        hud_features |= HUD_FEATURE_WRONG_WAY;
    }

    IRaceOverMessage *iraceover;
    if (QueryInterface(&iraceover) && iraceover->ShouldShowRaceOverMessage()) {
        hud_features |= HUD_FEATURE_RACE_OVER;
    }

    hud_features |= HUD_FEATURE_GENERIC_MESSAGE;
    hud_features |= HUD_FEATURE_AUTO_SAVE_ICON;
    hud_features |= HUD_FEATURE_FADE_TO_BLACK;
    hud_features |= HUD_FEATURE_RVM;
    hud_features |= HUD_FEATURE_321_GO;
    hud_features |= HUD_FEATURE_PURSUITINFORMATION;
    hud_features |= HUD_FEATURE_MENU_ZONE;
    hud_features |= HUD_FEATURE_INFRACTIONS;
    hud_features |= HUD_FEATURE_BUSTED_METER;
    hud_features |= HUD_FEATURE_WINGMAN_METER;

    return hud_features;
}

bool FEngHud::AreResourcesLoaded() {
    return TheHudResourceManager.AreResourcesLoaded(mPlayerHudType);
}

void FEngHud::SetHudFeatures(HudFeaturesType hud_features) {
    HudFeaturesType xor_hud_features = CurrentHudFeatures ^ hud_features;

    if (pSpeedometer != nullptr && (xor_hud_features & HUD_FEATURE_SPEEDOMETER) != 0) {
        pSpeedometer->Toggle(hud_features);
    }
    if (pTachometer != nullptr && (xor_hud_features & HUD_FEATURE_TACHOMETER) != 0) {
        pTachometer->Toggle(hud_features);
    }
    if (pTachometerDrag != nullptr && (xor_hud_features & HUD_FEATURE_TACHOMETER) != 0) {
        pTachometerDrag->Toggle(hud_features);
    }
    if (pShiftUpdater != nullptr && (xor_hud_features & HUD_FEATURE_PURSUITINFORMATION) != 0) {
        pShiftUpdater->Toggle(hud_features);
    }
    if (pTurboMeter != nullptr && (xor_hud_features & HUD_FEATURE_TURBO_METER) != 0) {
        pTurboMeter->Toggle(hud_features);
    }
    if (pEngineTemp != nullptr && (xor_hud_features & HUD_FEATURE_ENGINE_TEMP) != 0) {
        pEngineTemp->Toggle(hud_features);
    }
    if (pNitrous != nullptr && (xor_hud_features & HUD_FEATURE_NITROUS) != 0) {
        pNitrous->Toggle(hud_features);
    }
    if (pSpeedBreakerMeter != nullptr && (xor_hud_features & HUD_FEATURE_SPEEDBREAKER_METER) != 0) {
        pSpeedBreakerMeter->Toggle(hud_features);
    }
    if (pHeatMeter != nullptr && (xor_hud_features & HUD_FEATURE_HEAT_METER) != 0) {
        pHeatMeter->Toggle(hud_features);
    }
    if (pMinimap != nullptr && (xor_hud_features & HUD_FEATURE_MINIMAP) != 0) {
        pMinimap->Toggle(hud_features);
    }
    if (pGetAwayMeter != nullptr && (xor_hud_features & HUD_FEATURE_GETAWAY_METER) != 0) {
        pGetAwayMeter->Toggle(hud_features);
    }
    if (pMenuZoneTrigger != nullptr && (xor_hud_features & HUD_FEATURE_MENU_ZONE) != 0) {
        pMenuZoneTrigger->Toggle(hud_features);
    }
    if (pRaceInformation != nullptr && (xor_hud_features & HUD_FEATURE_RACEINFORMATION) != 0) {
        pRaceInformation->Toggle(hud_features);
    }
    if (pLeaderBoard != nullptr && ((xor_hud_features & HUD_FEATURE_LEADERBOARD) != 0 || (xor_hud_features & HUD_FEATURE_SPLIT_TIME) != 0)) {
        pLeaderBoard->Toggle(hud_features);
    }
    if (pPursuitBoard != nullptr && (xor_hud_features & HUD_FEATURE_PIP) != 0) {
        pPursuitBoard->Toggle(hud_features);
    }
    if (pMilestoneBoard != nullptr && (xor_hud_features & HUD_FEATURE_MILESTONE_BOARD) != 0) {
        pMilestoneBoard->Toggle(hud_features);
    }
    if (pBustedMeter != nullptr && (xor_hud_features & HUD_FEATURE_BUSTED_METER) != 0) {
        pBustedMeter->Toggle(hud_features);
    }
    if (pTimeExtension != nullptr && (xor_hud_features & HUD_FEATURE_TIME_EXTENSION) != 0) {
        pTimeExtension->Toggle(hud_features);
    }
    if (pCostToState != nullptr && (xor_hud_features & HUD_FEATURE_COST_TO_STATE) != 0) {
        pCostToState->Toggle(hud_features);
    }
    if (pReputation != nullptr && (xor_hud_features & HUD_FEATURE_COST_TO_STATE) != 0) {
        pReputation->Toggle(hud_features);
    }
    if (pWrongWIndi != nullptr && (xor_hud_features & HUD_FEATURE_WRONG_WAY) != 0) {
        pWrongWIndi->Toggle(hud_features);
    }
    if (pRaceOverMessage != nullptr && (xor_hud_features & HUD_FEATURE_RACE_OVER) != 0) {
        pRaceOverMessage->Toggle(hud_features);
    }
    if (pGenericMessage != nullptr && (xor_hud_features & HUD_FEATURE_GENERIC_MESSAGE) != 0) {
        pGenericMessage->Toggle(hud_features);
    }
    if (pRadarDetector != nullptr && (xor_hud_features & HUD_FEATURE_WINGMAN_METER) != 0) {
        pRadarDetector->Toggle(hud_features);
    }
    if (p321Go != nullptr && (xor_hud_features & HUD_FEATURE_321_GO) != 0) {
        p321Go->Toggle(hud_features);
    }
    if (pInfractions != nullptr && (xor_hud_features & HUD_FEATURE_INFRACTIONS) != 0) {
        pInfractions->Toggle(hud_features);
    }

    CurrentHudFeatures = hud_features;
}

void FEngHud::SetWideScreenMode() {
    if (mCurrentWidescreenSetting != FEDatabase->GetVideoSettings()->WideScreen) {
        mCurrentWidescreenSetting = FEDatabase->GetVideoSettings()->WideScreen;

        if (mCurrentWidescreenSetting) {
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
    for (IHud::List::const_iterator iter = IHud::GetList().begin(); iter != IHud::GetList().end(); ++iter) {
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

    if (player != nullptr && player->GetHud() != nullptr && !player->GetHud()->IsHudVisible()) {
        return false;
    }

    CameraMover *camMover = nullptr;
    if (view != nullptr) {
        camMover = view->GetCameraMover();
    }

    if (camMover != nullptr && camMover->GetType() == CM_DRIVE_CUBIC && camMover->GetLookbackAngle() != 0) {
        return false;
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
    if (static_cast<unsigned int>(playerCam - PSC_CLOSE) <= 4) {
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
