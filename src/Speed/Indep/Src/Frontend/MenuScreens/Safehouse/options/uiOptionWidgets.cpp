#include "uiOptionWidgets.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsController.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

void OMAudio::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_AUDIO;
}

void OMVideo::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_VIDEO;
}

void OMGameplay::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_GAMEPLAY;
}

void OMPlayer::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_PLAYER;
}

void OMController::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_CONTROLS;
}

void OMEATrax::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_EATRAX;
}

void OMCredits::React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (data != __BUTTON_PRESSED__) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_CREDITS;
}

void AOSFXMasterVol::Act(const char *parent_pkg, uint32 data) {
    UpdateSlider(data);
    FEDatabase->GetAudioSettings()->SoundEffectsVol = GetValue();
    g_pEAXSound->UpdateVolumes(FEDatabase->GetAudioSettings(), GetValue());
    Update(data);
}

void AOSFXMasterVol::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xFD487543);
    SetValue(FEDatabase->GetAudioSettings()->SoundEffectsVol);
    DrawSlider();
}

void AOSFXMasterVol::SetInitialValues() {
    SetSliderValues(0.0f, 1.0f, 0.1f, FEDatabase->GetAudioSettings()->SoundEffectsVol);
}

void AOInteractiveMusicMode::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetAudioSettings()->InteractiveMusicMode = static_cast<int>(FEDatabase->GetAudioSettings()->InteractiveMusicMode == 0);
    }
    Update(data);
}

void AOInteractiveMusicMode::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xA3DBB390);
    uint32 hash = FEDatabase->GetAudioSettings()->InteractiveMusicMode ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void AOEATraxMusicMode::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetAudioSettings()->EATraxMode = static_cast<int>(FEDatabase->GetAudioSettings()->EATraxMode == 0);
    }
    Update(data);
}

void AOEATraxMusicMode::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xDCFB6B36);
    uint32 hash = FEDatabase->GetAudioSettings()->EATraxMode ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void AOCarVol::Act(const char *parent_pkg, uint32 data) {
    UpdateSlider(data);
    FEDatabase->GetAudioSettings()->CarVol = GetValue();
    g_pEAXSound->UpdateVolumes(FEDatabase->GetAudioSettings(), GetValue());
    Update(data);
}

void AOCarVol::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x218E4B08);
    SetValue(FEDatabase->GetAudioSettings()->CarVol);
    DrawSlider();
}

void AOCarVol::SetInitialValues() {
    SetSliderValues(0.0f, 1.0f, 0.1f, FEDatabase->GetAudioSettings()->CarVol);
}

void AOSpeechVol::Act(const char *parent_pkg, uint32 data) {
    UpdateSlider(data);
    FEDatabase->GetAudioSettings()->SpeechVol = GetValue();
    g_pEAXSound->UpdateVolumes(FEDatabase->GetAudioSettings(), GetValue());
    Update(data);
}

void AOSpeechVol::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x9E5FB82A);
    SetValue(FEDatabase->GetAudioSettings()->SpeechVol);
    DrawSlider();
}

void AOSpeechVol::SetInitialValues() {
    SetSliderValues(0.0f, 1.0f, 0.1f, FEDatabase->GetAudioSettings()->SpeechVol);
}

void AOFEMusicVol::Act(const char *parent_pkg, uint32 data) {
    UpdateSlider(data);
    float value = GetValue();
    if (bEqual(0.0f, value, 0.001f)) {
        value = 0.0f;
    }
    FEDatabase->GetAudioSettings()->FEMusicVol = value;
    g_pEAXSound->UpdateVolumes(FEDatabase->GetAudioSettings(), GetValue());
    Update(data);
}

void AOFEMusicVol::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x418E681D);
    SetValue(FEDatabase->GetAudioSettings()->FEMusicVol);
    DrawSlider();
}

void AOFEMusicVol::SetInitialValues() {
    SetSliderValues(0.0f, 1.0f, 0.1f, FEDatabase->GetAudioSettings()->FEMusicVol);
}

void AOIGMusicVol::Act(const char *parent_pkg, uint32 data) {
    UpdateSlider(data);
    FEDatabase->GetAudioSettings()->IGMusicVol = GetValue();
    g_pEAXSound->UpdateVolumes(FEDatabase->GetAudioSettings(), GetValue());
    Update(data);
}

void AOIGMusicVol::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xDF21EAC2);
    SetValue(FEDatabase->GetAudioSettings()->IGMusicVol);
    DrawSlider();
}

void AOIGMusicVol::SetInitialValues() {
    SetSliderValues(0.0f, 1.0f, 0.1f, FEDatabase->GetAudioSettings()->IGMusicVol);
}

void AOAudioMode::Act(const char *parent_pkg, uint32 data) {
    int mode = FEDatabase->GetAudioSettings()->AudioMode;
    if (data == __PAD_LEFT__) {
        mode--;
        if (mode < 0) {
            mode = 2;
        }
    } else if (data == __PAD_RIGHT__) {
        mode++;
        if (mode > 2) {
            mode = 0;
        }
    }
    if (FEDatabase->GetAudioSettings()->AudioMode != mode) {
#ifdef EA_PLATFORM_GAMECUBE
        OSSetSoundMode(mode != 0);
#endif
    }
    FEDatabase->GetAudioSettings()->AudioMode = mode;
    Update(data);
}

void AOAudioMode::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x2881AB87);
    uint32 hash = 0;
    switch (FEDatabase->GetAudioSettings()->AudioMode) {
        case 0:
            hash = 0xC50FA35F;
            break;
        case 1:
            hash = 0x55DA8BF8;
            break;
        case 2:
            hash = 0xF6FAFF24;
            break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void VOWideScreen::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetVideoSettings()->WideScreen = !FEDatabase->GetVideoSettings()->WideScreen;
    }
    Update(data);
}

void VOWideScreen::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xD3588630);
    uint32 hash = FEDatabase->GetVideoSettings()->WideScreen ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GODamage::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetGameplaySettings()->Damage = !FEDatabase->GetGameplaySettings()->Damage;
    }
    Update(data);
}

void GODamage::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x1582ADFF);
    uint32 hash = FEDatabase->GetGameplaySettings()->Damage ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOAutoSave::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetGameplaySettings()->AutoSaveOn = !FEDatabase->GetGameplaySettings()->AutoSaveOn;
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            MemcardEnter(parent_pkg, parent_pkg, 0xA1, nullptr, nullptr, 0, 0);
        }
    }
    Update(data);
}

void GOAutoSave::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xD1056C88);
    uint32 hash = FEDatabase->GetGameplaySettings()->AutoSaveOn ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOJumpCams::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetGameplaySettings()->JumpCam = !FEDatabase->GetGameplaySettings()->JumpCam;
    }
    Update(data);
}

void GOJumpCams::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xF26A5CBF);
    uint32 hash = FEDatabase->GetGameplaySettings()->JumpCam ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GORearview::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetGameplaySettings()->RearviewOn = !FEDatabase->GetGameplaySettings()->RearviewOn;
    }
    Update(data);
}

void GORearview::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x85A6CE05);
    uint32 hash = FEDatabase->GetGameplaySettings()->RearviewOn ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOSpeedoUnits::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
            FEDatabase->GetGameplaySettings()->SpeedoUnits = 1;
        } else if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 1) {
            FEDatabase->GetGameplaySettings()->SpeedoUnits = 0;
        }
    }
    Update(data);
}

void GOSpeedoUnits::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x01E19173);
    uint32 hash = FEDatabase->GetGameplaySettings()->SpeedoUnits == 0 ? 0xFBD74FC5 : 0xAF70E736;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GORacingMiniMap::Act(const char *parent_pkg, uint32 data) {
    int mode = FEDatabase->GetGameplaySettings()->RacingMiniMapMode;
    if (data == __PAD_LEFT__) {
        mode--;
        if (mode < 0) {
            mode = 2;
        }
    } else if (data == __PAD_RIGHT__) {
        mode++;
        if (mode > 2) {
            mode = 0;
        }
    }
    FEDatabase->GetGameplaySettings()->RacingMiniMapMode = static_cast<unsigned char>(mode);
    Update(data);
}

void GORacingMiniMap::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x9FA5EC9E);
    uint32 hash = 0;
    switch (FEDatabase->GetGameplaySettings()->RacingMiniMapMode) {
        case 1:
            hash = 0xF4B00E99;
            break;
        case 0:
            hash = 0xF75595F2;
            break;
        case 2:
            hash = 0x70DFE5C2;
            break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOExploringMiniMap::Act(const char *parent_pkg, uint32 data) {
    int mode = FEDatabase->GetGameplaySettings()->ExploringMiniMapMode;
    if (data == __PAD_LEFT__) {
        mode--;
        if (mode < 0) {
            mode = 2;
        }
    } else if (data == __PAD_RIGHT__) {
        mode++;
        if (mode > 2) {
            mode = 0;
        }
    }
    FEDatabase->GetGameplaySettings()->ExploringMiniMapMode = static_cast<unsigned char>(mode);
    Update(data);
}

void GOExploringMiniMap::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xC6269082);
    uint32 hash = 0;
    switch (FEDatabase->GetGameplaySettings()->ExploringMiniMapMode) {
        case 1:
            hash = 0xF4B00E99;
            break;
        case 0:
            hash = 0xF75595F2;
            break;
        case 2:
            hash = 0x70DFE5C2;
            break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POTransmission::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        int trans = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Transmission;
        if (trans == 0) {
            trans = 1;
        } else if (trans == 1) {
            trans = 0;
        }
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Transmission = trans;
    }
    Update(data);
}

void POTransmission::Draw() {
    uint32 hash = 0;
    FEngSetLanguageHash(GetTitleObject(), 0xD31407E7);
    switch (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Transmission) {
        case 0:
            hash = 0x8CD532A0;
            break;
        case 1:
            hash = 0x317D3005;
            break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void PODriveCam::Act(const char *parent_pkg, uint32 data) {
    int cam = static_cast<int>(FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->CurCam);
    if (data == __PAD_LEFT__) {
        do {
            cam--;
            if (cam < 0) {
                cam = 6;
            }
        } while (!IsPlayerCameraSelectable(GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cam))));
    } else if (data == __PAD_RIGHT__) {
        do {
            cam++;
            if (cam > 6) {
                cam = 0;
            }
        } while (!IsPlayerCameraSelectable(GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cam))));
    }
    FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->CurCam = static_cast<ePlayerSettingsCameras>(cam);
    Update(data);
}

void PODriveCam::Draw() {
    uint32 hash = 0;
    FEngSetLanguageHash(GetTitleObject(), 0xF6CCDC5F);
    switch (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->CurCam) {
        case 3:
            hash = 0x1EA4CEC2;
            break;
        case 2:
            hash = 0x5AE3441F;
            break;
        case 1:
            hash = 0x414F19D7;
            break;
        case 0:
            hash = 0xC3E9AE58;
            break;
        case 4:
            hash = 0x916039B4;
            break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POGauges::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->GaugesOn = !FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->GaugesOn;
    }
    Update(data);
}

void POGauges::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xAC148579);
    uint32 hash = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->GaugesOn ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POPosition::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->PositionOn =
            !FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->PositionOn;
    }
    Update(data);
}

void POPosition::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x82CD8F92);
    uint32 hash = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->PositionOn ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POScore::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->ScoreOn = !FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->ScoreOn;
    }
    Update(data);
}

void POScore::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xC03F9F19);
    uint32 hash = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->ScoreOn ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POSplitTime::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        int type = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->SplitTimeType;
        if (type == 0) {
            type = 4;
        } else {
            type = 0;
        }
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->SplitTimeType = type;
    }
    Update(data);
}

void POSplitTime::Draw() {
    uint32 hash = 0;
    FEngSetLanguageHash(GetTitleObject(), 0x084BC378);
    switch (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->SplitTimeType) {
        case 0:
            hash = 0x417B2604;
            break;
        case 1:
            hash = 0xC44D3943;
            break;
        case 2:
            hash = 0x17FAFC32;
            break;
        case 3:
            hash = 0x1EA459F8;
            break;
        case 4:
            hash = 0x70DFE5C2;
            break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POLeaderBoard::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->LeaderboardOn =
            !FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->LeaderboardOn;
    }
    Update(data);
}

void POLeaderBoard::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xC93FBFB5);
    uint32 hash = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->LeaderboardOn ? 0x417B2604 : 0x70DFE5C2;
    FEngSetLanguageHash(GetDataObject(), hash);
}

void COVibration::Act(const char *parent_pkg, uint32 data) {
    if (data == __PAD_LEFT__ || data == __PAD_RIGHT__) {
        const u32 FEObj_HIDE = FEHASH_HIDE;
        const u32 FEObj_Init = 0x001744B3;
        const u32 FEObj_ArrowMainLeft = 0xBFF41BD9;
        const u32 FEObj_LEFTARROW0 = 0x7BCD6703;
        const u32 FEObj_ArrowMainRight = 0xBEE65E8C;
        const u32 FEObj_RIGHTARROW0 = 0x7C51B6D6;
        switch (data) {
            case __PAD_LEFT__: {
                FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble = false;
                FEngSetInvisible(parent_pkg, FEObj_ArrowMainLeft);
                FEngSetInvisible(parent_pkg, FEObj_LEFTARROW0);
                FEngSetInvisible(GetLeftImage());
                FEngSetVisible(parent_pkg, FEObj_ArrowMainRight);
                FEngSetVisible(parent_pkg, FEObj_RIGHTARROW0);
                FEngSetVisible(GetRightImage());
                break;
            }
            case __PAD_RIGHT__: {
                if (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble) {
                    return;
                }
                FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble = true;
                FEngSetInvisible(parent_pkg, FEObj_ArrowMainRight);
                FEngSetInvisible(parent_pkg, FEObj_RIGHTARROW0);
                FEngSetInvisible(GetRightImage());
                FEngSetVisible(parent_pkg, FEObj_ArrowMainLeft);
                FEngSetVisible(parent_pkg, FEObj_LEFTARROW0);
                FEngSetVisible(GetLeftImage());
                break;
            }
        }

        if (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble) {
            InputDevice *device = IOModule::GetIOModule().GetDevice(FEDatabase->GetPlayersJoystickPort(GetPlayerToEditForOptions()));
            if (device != nullptr) {
                device->StartVibration();
            }
        }
    }

    Update(data);
}

void COVibration::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xDDDC5E1B);
    FEngSetLanguageHash(GetDataObject(), FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble ? 0x417B2604 : 0x70DFE5C2);
}

void COVibration::UnsetFocus() {
    const u32 FEObj_ArrowMainLeft = 0xBFF41BD9;
    const u32 FEObj_LEFTARROW0 = 0x7BCD6703;
    const u32 FEObj_ArrowMainRight = 0xBEE65E8C;
    const u32 FEObj_RIGHTARROW0 = 0x7C51B6D6;
    FEngSetVisible("Pause_Controller.fng", FEObj_ArrowMainLeft);
    FEngSetVisible("Pause_Controller.fng", FEObj_LEFTARROW0);
    FEngSetVisible(GetLeftImage());
    FEngSetVisible("Pause_Controller.fng", FEObj_ArrowMainRight);
    FEngSetVisible("Pause_Controller.fng", FEObj_RIGHTARROW0);
    FEngSetVisible(GetRightImage());
    FEToggleWidget::UnsetFocus();
}

void COVibration::SetFocus(const char *parent_pkg) {
    if (FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Rumble) {
        const u32 FEObj_ArrowMainRight = 0xBEE65E8C;
        const u32 FEObj_RIGHTARROW0 = 0x7C51B6D6;
        FEngSetInvisible("Pause_Controller.fng", FEObj_ArrowMainRight);
        FEngSetInvisible("Pause_Controller.fng", FEObj_RIGHTARROW0);
        FEngSetInvisible(GetRightImage());
    } else {
        const u32 FEObj_ArrowMainLeft = 0xBFF41BD9;
        const u32 FEObj_LEFTARROW0 = 0x7BCD6703;
        FEngSetInvisible("Pause_Controller.fng", FEObj_ArrowMainLeft);
        FEngSetInvisible("Pause_Controller.fng", FEObj_LEFTARROW0);
        FEngSetInvisible(GetLeftImage());
    }
    FEToggleWidget::SetFocus(parent_pkg);
}

void COConfig::Act(const char *parent_pkg, uint32 data) {
    int config = static_cast<int>(FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config);
    bool isAnalogSwiched = FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog;
    if (UIOptionsController::isWheelConfig) {
        config = 0;
        isAnalogSwiched = true;
    } else {
        int max = MAX_CONFIG;
        if (data == __PAD_LEFT__) {
            config--;
            if (config < 0) {
                isAnalogSwiched = !isAnalogSwiched;
                config = max;
            }
        } else if (data == __PAD_RIGHT__) {
            config++;
            if (config > max) {
                isAnalogSwiched = !isAnalogSwiched;
                config = 0;
            }
        }
    }
    FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog = isAnalogSwiched;
    FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config = static_cast<eControllerConfig>(config);
    cFEng::Get()->QueueGameMessage(FEMSG_REFRESH_WIDGETS, parent_pkg, 0xFF);
    Update(data);
}

void COConfig::Draw() {
    int val = static_cast<int>(FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->Config) + 1;
    if (!FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->DriveWithAnalog) {
        val += NUM_CONTROLLER_CONFIGS;
    }
    FEngSetLanguageHash(GetTitleObject(), 0xBA7306AA);
    FEPrintf(GetDataObject(), "%d", val);
}
