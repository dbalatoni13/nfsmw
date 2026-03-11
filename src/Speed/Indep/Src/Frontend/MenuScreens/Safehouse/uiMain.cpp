#include "uiMain.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

struct MainQuickRace : public IconOption {
    MainQuickRace(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainQuickRace() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

void MainQuickRace::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) {
    if (data != 0x0C407210) return;
    FEDatabase->SetGameMode(eFE_GAME_MODE_QUICK_RACE);
}

struct MainCustomize : public IconOption {
    MainCustomize(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainCustomize() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

void MainCustomize::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) {
    if (data != 0x0C407210) return;
    FEDatabase->SetGameMode(eFE_GAME_MODE_CUSTOMIZE);
}

struct MainProfileManager : public IconOption {
    MainProfileManager(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainProfileManager() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

void MainProfileManager::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) {
    if (data != 0x0C407210) return;
    FEDatabase->SetGameMode(eFE_GAME_MODE_PROFILE_MANAGER);
}

struct MainOptions : public IconOption {
    MainOptions(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainOptions() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

void MainOptions::React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) {
    if (data != 0x0C407210) return;
    FEDatabase->SetGameMode(eFE_GAME_MODE_OPTIONS);
}
