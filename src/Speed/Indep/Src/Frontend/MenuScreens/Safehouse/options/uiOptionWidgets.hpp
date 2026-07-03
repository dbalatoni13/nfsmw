#ifndef _UIOPTIONWIDGETS
#define _UIOPTIONWIDGETS

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

// ===== OM* classes (extend IconOption) =====

// 0x5C
class OMAudio : public IconOption {
  public:
    OMAudio(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMAudio() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// 0x5C
class OMVideo : public IconOption {
  public:
    OMVideo(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMVideo() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// 0x5C
class OMGameplay : public IconOption {
  public:
    OMGameplay(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMGameplay() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// 0x5C
class OMPlayer : public IconOption {
  public:
    OMPlayer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMPlayer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// 0x5C
class OMController : public IconOption {
  public:
    OMController(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMController() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// 0x5C
class OMEATrax : public IconOption {
  public:
    OMEATrax(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMEATrax() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// 0x5C
class OMCredits : public IconOption {
  public:
    OMCredits(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~OMCredits() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// ===== AO* slider classes (extend FESliderWidget) =====

// 0xA4
class AOSFXMasterVol : public FESliderWidget {
  public:
    AOSFXMasterVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOSFXMasterVol() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
class AOCarVol : public FESliderWidget {
  public:
    AOCarVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOCarVol() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
class AOSpeechVol : public FESliderWidget {
  public:
    AOSpeechVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOSpeechVol() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
class AOFEMusicVol : public FESliderWidget {
  public:
    AOFEMusicVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOFEMusicVol() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// 0xA4
class AOIGMusicVol : public FESliderWidget {
  public:
    AOIGMusicVol(bool enabled) : FESliderWidget(enabled) {}
    ~AOIGMusicVol() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
    void SetInitialValues() override;
};

// ===== AO* toggle classes (extend FEToggleWidget) =====

// 0x64
class AOInteractiveMusicMode : public FEToggleWidget {
  public:
    AOInteractiveMusicMode(bool enabled) : FEToggleWidget(enabled) {}
    ~AOInteractiveMusicMode() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class AOEATraxMusicMode : public FEToggleWidget {
  public:
    AOEATraxMusicMode(bool enabled) : FEToggleWidget(enabled) {}
    ~AOEATraxMusicMode() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class AOAudioMode : public FEToggleWidget {
  public:
    AOAudioMode(bool enabled) : FEToggleWidget(enabled) {}
    ~AOAudioMode() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// ===== VO* classes (extend FEToggleWidget) =====

// 0x64
class VOWideScreen : public FEToggleWidget {
  public:
    VOWideScreen(bool enabled) : FEToggleWidget(enabled) {}
    ~VOWideScreen() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// ===== GO* classes (extend FEToggleWidget) =====

// 0x64
class GODamage : public FEToggleWidget {
  public:
    GODamage(bool enabled) : FEToggleWidget(enabled) {}
    ~GODamage() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class GOAutoSave : public FEToggleWidget {
  public:
    GOAutoSave(bool enabled) : FEToggleWidget(enabled) {}
    ~GOAutoSave() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class GOJumpCams : public FEToggleWidget {
  public:
    GOJumpCams(bool enabled) : FEToggleWidget(enabled) {}
    ~GOJumpCams() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class GORearview : public FEToggleWidget {
  public:
    GORearview(bool enabled) : FEToggleWidget(enabled) {}
    ~GORearview() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class GOSpeedoUnits : public FEToggleWidget {
  public:
    GOSpeedoUnits(bool enabled) : FEToggleWidget(enabled) {}
    ~GOSpeedoUnits() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class GORacingMiniMap : public FEToggleWidget {
  public:
    GORacingMiniMap(bool enabled) : FEToggleWidget(enabled) {}
    ~GORacingMiniMap() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class GOExploringMiniMap : public FEToggleWidget {
  public:
    GOExploringMiniMap(bool enabled) : FEToggleWidget(enabled) {}
    ~GOExploringMiniMap() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// ===== PO* classes (extend FEToggleWidget) =====

// 0x64
class POTransmission : public FEToggleWidget {
  public:
    POTransmission(bool enabled) : FEToggleWidget(enabled) {}
    ~POTransmission() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class PODriveCam : public FEToggleWidget {
  public:
    PODriveCam(bool enabled) : FEToggleWidget(enabled) {}
    ~PODriveCam() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class POGauges : public FEToggleWidget {
  public:
    POGauges(bool enabled) : FEToggleWidget(enabled) {}
    ~POGauges() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class POPosition : public FEToggleWidget {
  public:
    POPosition(bool enabled) : FEToggleWidget(enabled) {}
    ~POPosition() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class POScore : public FEToggleWidget {
  public:
    POScore(bool enabled) : FEToggleWidget(enabled) {}
    ~POScore() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class POSplitTime : public FEToggleWidget {
  public:
    POSplitTime(bool enabled) : FEToggleWidget(enabled) {}
    ~POSplitTime() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// 0x64
class POLeaderBoard : public FEToggleWidget {
  public:
    POLeaderBoard(bool enabled) : FEToggleWidget(enabled) {}
    ~POLeaderBoard() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

// ===== CO* classes (extend FEToggleWidget) =====

// 0x64
class COVibration : public FEToggleWidget {
  public:
    COVibration(int player_num, bool enabled) : FEToggleWidget(enabled) {}
    ~COVibration() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;
};

// 0x64
class COConfig : public FEToggleWidget {
  public:
    COConfig(bool enabled) : FEToggleWidget(enabled) {}
    ~COConfig() override {}
    void Act(const char *parent_pkg, uint32 data) override;
    void Draw() override;
};

#endif
