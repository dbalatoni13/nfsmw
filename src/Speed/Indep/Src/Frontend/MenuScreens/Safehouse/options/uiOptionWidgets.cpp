#include "uiOptionWidgets.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsController.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/options/uiOptionsMain.hpp"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

#ifdef EA_PLATFORM_GAMECUBE
#include "Speed/GameCube/bWare/GameCube/dolphinsdk/include/dolphin/os.h"
#endif

struct FEString;
struct FEObject;
#include "Speed/Indep/Src/FEng/feimage.h"

void FEngSetLanguageHash(FEString* text, unsigned int hash);
int FEPrintf(FEString* text, const char* fmt, ...);
FEObject* FEngFindObject(const char* pkg_name, unsigned int obj_hash);
void FEngSetInvisible(FEObject* obj);
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(const char* pkg_name, unsigned int obj_hash);
void FEngSetVisible(const char* pkg_name, unsigned int obj_hash);

enum POVTypes {
    POV_BUMPER = 0,
    POV_HOOD = 1,
    POV_OUTSIDE_CLOSE = 2,
    POV_OUTSIDE_FAR = 3,
    POV_SUPER_FAR = 4,
    POV_DRIFT = 5,
    POV_PURSUIT = 6,
    NUM_POV_TYPES = 7,
};
POVTypes GetPOVTypeFromPlayerCamera(ePlayerSettingsCameras cam);
bool IsPlayerCameraSelectable(POVTypes pov_type);

void MemcardEnter(const char* from, const char* to, unsigned int op,
                  void (*pTermFunc)(void*), void* pTermFuncParam,
                  unsigned int msgSuccess, unsigned int msgFailed);

void OMAudio::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
                    unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_AUDIO;
}

void OMVideo::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
                    unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_VIDEO;
}

void OMGameplay::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
                       unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_GAMEPLAY;
}

void OMPlayer::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
                     unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_PLAYER;
}

void OMController::React(const char* pkg_name, unsigned int data, FEObject* obj,
                         unsigned int param1, unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_CONTROLS;
}

void OMEATrax::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
                     unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_EATRAX;
}

void OMCredits::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1,
                      unsigned int param2) {
    if (data != 0xC407210) {
        return;
    }
    FEDatabase->GetOptionsSettings()->CurrentCategory = OC_CREDITS;
}

void AOSFXMasterVol::Act(const char* parent_pkg, unsigned int data) {
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

void AOInteractiveMusicMode::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetAudioSettings()->InteractiveMusicMode =
            static_cast<int>(FEDatabase->GetAudioSettings()->InteractiveMusicMode == 0);
    }
    Update(data);
}

void AOInteractiveMusicMode::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xA3DBB390);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetAudioSettings()->InteractiveMusicMode != 0) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void AOEATraxMusicMode::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetAudioSettings()->EATraxMode =
            static_cast<int>(FEDatabase->GetAudioSettings()->EATraxMode == 0);
    }
    Update(data);
}

void AOEATraxMusicMode::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xDCFB6B36);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetAudioSettings()->EATraxMode != 0) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void AOCarVol::Act(const char* parent_pkg, unsigned int data) {
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

void AOSpeechVol::Act(const char* parent_pkg, unsigned int data) {
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

void AOFEMusicVol::Act(const char* parent_pkg, unsigned int data) {
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

void AOIGMusicVol::Act(const char* parent_pkg, unsigned int data) {
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

void AOAudioMode::Act(const char* parent_pkg, unsigned int data) {
    int mode = FEDatabase->GetAudioSettings()->AudioMode;
    if (data == 0x9120409E) {
        mode--;
        if (mode < 0) {
            mode = 2;
        }
    } else if (data == 0xB5971BF1) {
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
    unsigned int hash = 0;
    int mode = FEDatabase->GetAudioSettings()->AudioMode;
    switch (mode) {
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

void VOWideScreen::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetVideoSettings()->WideScreen ^= 1;
    }
    Update(data);
}

void VOWideScreen::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xD3588630);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetVideoSettings()->WideScreen) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GODamage::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetGameplaySettings()->Damage ^= 1;
    }
    Update(data);
}

void GODamage::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x1582ADFF);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetGameplaySettings()->Damage) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOAutoSave::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetGameplaySettings()->AutoSaveOn ^= 1;
        if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
            MemcardEnter(parent_pkg, parent_pkg, 0xA1, nullptr, nullptr, 0, 0);
        }
    }
    Update(data);
}

void GOAutoSave::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xD1056C88);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetGameplaySettings()->AutoSaveOn) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOJumpCams::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetGameplaySettings()->JumpCam ^= 1;
    }
    Update(data);
}

void GOJumpCams::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xF26A5CBF);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetGameplaySettings()->JumpCam) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GORearview::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        FEDatabase->GetGameplaySettings()->RearviewOn ^= 1;
    }
    Update(data);
}

void GORearview::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x85A6CE05);
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetGameplaySettings()->RearviewOn) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GOSpeedoUnits::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        unsigned char units = FEDatabase->GetGameplaySettings()->SpeedoUnits;
        if (units == 0) {
            FEDatabase->GetGameplaySettings()->SpeedoUnits = 1;
        } else if (units == 1) {
            FEDatabase->GetGameplaySettings()->SpeedoUnits = 0;
        }
    }
    Update(data);
}

void GOSpeedoUnits::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x01E19173);
    unsigned int hash = 0xAF70E736;
    if (FEDatabase->GetGameplaySettings()->SpeedoUnits == 0) {
        hash = 0xFBD74FC5;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void GORacingMiniMap::Act(const char* parent_pkg, unsigned int data) {
    int mode = FEDatabase->GetGameplaySettings()->RacingMiniMapMode;
    if (data == 0x9120409E) {
        mode--;
        if (mode < 0) {
            mode = 2;
        }
    } else if (data == 0xB5971BF1) {
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
    unsigned int hash = 0;
    unsigned char mode = FEDatabase->GetGameplaySettings()->RacingMiniMapMode;
    switch (mode) {
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

void GOExploringMiniMap::Act(const char* parent_pkg, unsigned int data) {
    int mode = FEDatabase->GetGameplaySettings()->ExploringMiniMapMode;
    if (data == 0x9120409E) {
        mode--;
        if (mode < 0) {
            mode = 2;
        }
    } else if (data == 0xB5971BF1) {
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
    unsigned int hash = 0;
    unsigned char mode = FEDatabase->GetGameplaySettings()->ExploringMiniMapMode;
    switch (mode) {
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

void POTransmission::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        int player = GetPlayerToEditForOptions();
        unsigned char trans = FEDatabase->GetPlayerSettings(player)->Transmission;
        if (trans == 0) {
            trans = 1;
        } else if (trans == 1) {
            trans = 0;
        }
        player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->Transmission = trans;
    }
    Update(data);
}

void POTransmission::Draw() {
    unsigned int hash = 0;
    FEngSetLanguageHash(GetTitleObject(), 0xD31407E7);
    int player = GetPlayerToEditForOptions();
    unsigned char trans = FEDatabase->GetPlayerSettings(player)->Transmission;
    switch (trans) {
    case 0:
        hash = 0x8CD532A0;
        break;
    case 1:
        hash = 0x317D3005;
        break;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void PODriveCam::Act(const char* parent_pkg, unsigned int data) {
    int player = GetPlayerToEditForOptions();
    int cam = static_cast<int>(FEDatabase->GetPlayerSettings(player)->CurCam);
    if (data == 0x9120409E) {
        do {
            cam--;
            if (cam < 0) {
                cam = 6;
            }
        } while (!IsPlayerCameraSelectable(
            GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cam))));
    } else if (data == 0xB5971BF1) {
        do {
            cam++;
            if (cam > 6) {
                cam = 0;
            }
        } while (!IsPlayerCameraSelectable(
            GetPOVTypeFromPlayerCamera(static_cast<ePlayerSettingsCameras>(cam))));
    }
    player = GetPlayerToEditForOptions();
    FEDatabase->GetPlayerSettings(player)->CurCam = static_cast<ePlayerSettingsCameras>(cam);
    Update(data);
}

void PODriveCam::Draw() {
    unsigned int hash = 0;
    FEngSetLanguageHash(GetTitleObject(), 0xF6CCDC5F);
    int player = GetPlayerToEditForOptions();
    int cam = static_cast<int>(FEDatabase->GetPlayerSettings(player)->CurCam);
    switch (cam) {
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

void POGauges::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        int player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->GaugesOn =
            FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->GaugesOn ^ 1;
    }
    Update(data);
}

void POGauges::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xAC148579);
    int player = GetPlayerToEditForOptions();
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetPlayerSettings(player)->GaugesOn) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POPosition::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        int player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->PositionOn =
            FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->PositionOn ^ 1;
    }
    Update(data);
}

void POPosition::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0x82CD8F92);
    int player = GetPlayerToEditForOptions();
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetPlayerSettings(player)->PositionOn) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POScore::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        int player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->ScoreOn =
            FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->ScoreOn ^ 1;
    }
    Update(data);
}

void POScore::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xC03F9F19);
    int player = GetPlayerToEditForOptions();
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetPlayerSettings(player)->ScoreOn) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void POSplitTime::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        int type = (!FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->SplitTimeType)
                   << 2;
        FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->SplitTimeType = type;
    }
    Update(data);
}

void POSplitTime::Draw() {
    unsigned int hash = 0;
    FEngSetLanguageHash(GetTitleObject(), 0x084BC378);
    int player = GetPlayerToEditForOptions();
    unsigned char splitTime = FEDatabase->GetPlayerSettings(player)->SplitTimeType;
    switch (splitTime) {
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

void POLeaderBoard::Act(const char* parent_pkg, unsigned int data) {
    if (data == 0x9120409E || data == 0xB5971BF1) {
        int player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->LeaderboardOn =
            FEDatabase->GetPlayerSettings(GetPlayerToEditForOptions())->LeaderboardOn ^ 1;
    }
    Update(data);
}

void POLeaderBoard::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xC93FBFB5);
    int player = GetPlayerToEditForOptions();
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetPlayerSettings(player)->LeaderboardOn) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(GetDataObject(), hash);
}

void COVibration::Act(const char* parent_pkg, unsigned int data) {
    switch (data) {
    case 0x9120409E: {
        int player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->Rumble = 0;
        FEngSetInvisible(parent_pkg, 0xBFF41BD9);
        FEngSetInvisible(parent_pkg, 0x7BCD6703);
        FEngSetInvisible(GetLeftImage());
        FEngSetVisible(parent_pkg, 0xBEE65E8C);
        FEngSetVisible(parent_pkg, 0x7C51B6D6);
        FEngSetVisible(GetRightImage());
        break;
    }
    default:
        goto end;
    case 0xB5971BF1: {
        int player = GetPlayerToEditForOptions();
        if (FEDatabase->GetPlayerSettings(player)->Rumble) {
            return;
        }
        player = GetPlayerToEditForOptions();
        FEDatabase->GetPlayerSettings(player)->Rumble = 1;
        FEngSetInvisible(parent_pkg, 0xBEE65E8C);
        FEngSetInvisible(parent_pkg, 0x7C51B6D6);
        FEngSetInvisible(GetRightImage());
        FEngSetVisible(parent_pkg, 0xBFF41BD9);
        FEngSetVisible(parent_pkg, 0x7BCD6703);
        FEngSetVisible(GetLeftImage());
        break;
    }
    }
    {
        int player = GetPlayerToEditForOptions();
        if (FEDatabase->GetPlayerSettings(player)->Rumble) {
            InputDevice* device = IOModule::GetIOModule().GetDevice(
                FEDatabase->GetPlayersJoystickPort(GetPlayerToEditForOptions()));
            if (device != nullptr) {
                device->StartVibration();
            }
        }
    }
end:
    Update(data);
}

void COVibration::Draw() {
    FEngSetLanguageHash(GetTitleObject(), 0xDDDC5E1B);
    FEString* dataObj = GetDataObject();
    int player = GetPlayerToEditForOptions();
    unsigned int hash = 0x70DFE5C2;
    if (FEDatabase->GetPlayerSettings(player)->Rumble) {
        hash = 0x417B2604;
    }
    FEngSetLanguageHash(dataObj, hash);
}

void COVibration::UnsetFocus() {
    const unsigned long FEObj_ArrowMainLeft = 0xBFF41BD9;
    const unsigned long FEObj_LEFTARROW0 = 0x7BCD6703;
    const unsigned long FEObj_ArrowMainRight = 0xBEE65E8C;
    const unsigned long FEObj_RIGHTARROW0 = 0x7C51B6D6;
    FEngSetVisible("Pause_Controller.fng", FEObj_ArrowMainLeft);
    FEngSetVisible("Pause_Controller.fng", FEObj_LEFTARROW0);
    FEngSetVisible(GetLeftImage());
    FEngSetVisible("Pause_Controller.fng", FEObj_ArrowMainRight);
    FEngSetVisible("Pause_Controller.fng", FEObj_RIGHTARROW0);
    FEngSetVisible(GetRightImage());
    FEToggleWidget::UnsetFocus();
}

void COVibration::SetFocus(const char* parent_pkg) {
    int player = GetPlayerToEditForOptions();
    if (FEDatabase->GetPlayerSettings(player)->Rumble) {
        const unsigned long FEObj_ArrowMainRight = 0xBEE65E8C;
        const unsigned long FEObj_RIGHTARROW0 = 0x7C51B6D6;
        FEngSetInvisible("Pause_Controller.fng", FEObj_ArrowMainRight);
        FEngSetInvisible("Pause_Controller.fng", FEObj_RIGHTARROW0);
        FEngSetInvisible(GetRightImage());
    } else {
        const unsigned long FEObj_ArrowMainLeft = 0xBFF41BD9;
        const unsigned long FEObj_LEFTARROW0 = 0x7BCD6703;
        FEngSetInvisible("Pause_Controller.fng", FEObj_ArrowMainLeft);
        FEngSetInvisible("Pause_Controller.fng", FEObj_LEFTARROW0);
        FEngSetInvisible(GetLeftImage());
    }
    FEToggleWidget::SetFocus(parent_pkg);
}

void COConfig::Act(const char* parent_pkg, unsigned int data) {
    int player = GetPlayerToEditForOptions();
    int config = static_cast<int>(FEDatabase->GetPlayerSettings(player)->Config);
    player = GetPlayerToEditForOptions();
    int isAnalogSwiched = FEDatabase->GetPlayerSettings(player)->DriveWithAnalog;
    if (UIOptionsController::isWheelConfig) {
        config = 0;
        isAnalogSwiched = 1;
    } else {
        if (data == 0x9120409E) {
            config--;
            if (config < 0) {
                isAnalogSwiched ^= 1;
                config = MAX_CONFIG;
            }
        } else if (data == 0xB5971BF1) {
            config++;
            if (config > MAX_CONFIG) {
                isAnalogSwiched ^= 1;
                config = 0;
            }
        }
    }
    player = GetPlayerToEditForOptions();
    FEDatabase->GetPlayerSettings(player)->DriveWithAnalog = isAnalogSwiched;
    player = GetPlayerToEditForOptions();
    FEDatabase->GetPlayerSettings(player)->Config = static_cast<eControllerConfig>(config);
    cFEng::Get()->QueueGameMessage(0x92B703B5, parent_pkg, 0xFF);
    Update(data);
}

void COConfig::Draw() {
    int player = GetPlayerToEditForOptions();
    int config = static_cast<int>(FEDatabase->GetPlayerSettings(player)->Config);
    player = GetPlayerToEditForOptions();
    int val = config + 1;
    if (!FEDatabase->GetPlayerSettings(player)->DriveWithAnalog) {
        val = config + 6;
    }
    FEngSetLanguageHash(GetTitleObject(), 0xBA7306AA);
    FEPrintf(GetDataObject(), "%d", val);
}
