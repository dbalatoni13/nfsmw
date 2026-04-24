#ifndef _UIOPTIONWIDGETS
#define _UIOPTIONWIDGETS

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// ===== OM* classes (extend IconOption) =====

// 0x5C
struct OMAudio : public IconOption {
    OMAudio(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMAudio() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// 0x5C
struct OMVideo : public IconOption {
    OMVideo(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMVideo() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// 0x5C
struct OMGameplay : public IconOption {
    OMGameplay(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMGameplay() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// 0x5C
struct OMPlayer : public IconOption {
    OMPlayer(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMPlayer() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// 0x5C
struct OMController : public IconOption {
    OMController(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMController() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// 0x5C
struct OMEATrax : public IconOption {
    OMEATrax(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMEATrax() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// 0x5C
struct OMCredits : public IconOption {
    OMCredits(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMCredits() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// ===== AO* slider classes (extend FESliderWidget) =====

// 0xA4
struct AOSFXMasterVol : public FESliderWidget {
    AOSFXMasterVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOSFXMasterVol() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
struct AOCarVol : public FESliderWidget {
    AOCarVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOCarVol() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
struct AOSpeechVol : public FESliderWidget {
    AOSpeechVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOSpeechVol() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
struct AOFEMusicVol : public FESliderWidget {
    AOFEMusicVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOFEMusicVol() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
struct AOIGMusicVol : public FESliderWidget {
    AOIGMusicVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOIGMusicVol() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// ===== AO* toggle classes (extend FEToggleWidget) =====

// 0x64
struct AOInteractiveMusicMode : public FEToggleWidget {
    AOInteractiveMusicMode(bool enabled) : FEToggleWidget(enabled) {}
    ~AOInteractiveMusicMode() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct AOEATraxMusicMode : public FEToggleWidget {
    AOEATraxMusicMode(bool enabled) : FEToggleWidget(enabled) {}
    ~AOEATraxMusicMode() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct AOAudioMode : public FEToggleWidget {
    AOAudioMode(bool enabled) : FEToggleWidget(enabled) {}
    ~AOAudioMode() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// ===== VO* classes (extend FEToggleWidget) =====

// 0x64
struct VOWideScreen : public FEToggleWidget {
    VOWideScreen(bool enabled) : FEToggleWidget(enabled) {}
    ~VOWideScreen() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// ===== GO* classes (extend FEToggleWidget) =====

// 0x64
struct GODamage : public FEToggleWidget {
    GODamage(bool enabled) : FEToggleWidget(enabled) {}
    ~GODamage() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct GOAutoSave : public FEToggleWidget {
    GOAutoSave(bool enabled) : FEToggleWidget(enabled) {}
    ~GOAutoSave() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct GOJumpCams : public FEToggleWidget {
    GOJumpCams(bool enabled) : FEToggleWidget(enabled) {}
    ~GOJumpCams() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct GORearview : public FEToggleWidget {
    GORearview(bool enabled) : FEToggleWidget(enabled) {}
    ~GORearview() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct GOSpeedoUnits : public FEToggleWidget {
    GOSpeedoUnits(bool enabled) : FEToggleWidget(enabled) {}
    ~GOSpeedoUnits() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct GORacingMiniMap : public FEToggleWidget {
    GORacingMiniMap(bool enabled) : FEToggleWidget(enabled) {}
    ~GORacingMiniMap() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct GOExploringMiniMap : public FEToggleWidget {
    GOExploringMiniMap(bool enabled) : FEToggleWidget(enabled) {}
    ~GOExploringMiniMap() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// ===== PO* classes (extend FEToggleWidget) =====

// 0x64
struct POTransmission : public FEToggleWidget {
    POTransmission(bool enabled) : FEToggleWidget(enabled) {}
    ~POTransmission() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct PODriveCam : public FEToggleWidget {
    PODriveCam(bool enabled) : FEToggleWidget(enabled) {}
    ~PODriveCam() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct POGauges : public FEToggleWidget {
    POGauges(bool enabled) : FEToggleWidget(enabled) {}
    ~POGauges() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct POPosition : public FEToggleWidget {
    POPosition(bool enabled) : FEToggleWidget(enabled) {}
    ~POPosition() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct POScore : public FEToggleWidget {
    POScore(bool enabled) : FEToggleWidget(enabled) {}
    ~POScore() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct POSplitTime : public FEToggleWidget {
    POSplitTime(bool enabled) : FEToggleWidget(enabled) {}
    ~POSplitTime() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// 0x64
struct POLeaderBoard : public FEToggleWidget {
    POLeaderBoard(bool enabled) : FEToggleWidget(enabled) {}
    ~POLeaderBoard() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

// ===== CO* classes (extend FEToggleWidget) =====

// 0x64
struct COVibration : public FEToggleWidget {
    COVibration(int player_num, bool enabled) : FEToggleWidget(enabled) {}
    ~COVibration() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
    void SetFocus(const char* parent_pkg) override;
    void UnsetFocus() override;
};

// 0x64
struct COConfig : public FEToggleWidget {
    COConfig(bool enabled) : FEToggleWidget(enabled) {}
    ~COConfig() override {}
    void Act(const char* parent_pkg, unsigned int data) override;
    void Draw() override;
};

#endif
