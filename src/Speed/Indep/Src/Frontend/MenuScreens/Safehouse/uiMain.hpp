#ifndef _UIMAIN
#define _UIMAIN

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

// total size: 0x170
struct UIMain : public IconScrollerMenu {
    bool m_bStatsShowing; // offset 0x16C, size 0x1

    UIMain(ScreenConstructorData* sd);
    ~UIMain() override {}

    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
    void UpdateProfileData();
};

struct MainCareer : public IconOption {
    MainCareer(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~MainCareer() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct Challenge : public IconOption {
    Challenge(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~Challenge() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

#endif
