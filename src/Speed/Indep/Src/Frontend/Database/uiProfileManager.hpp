#ifndef FRONTEND_DATABASE_UIPROFILEMANAGER_H
#define FRONTEND_DATABASE_UIPROFILEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

struct PMSave : public IconOption {
    PMSave(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMSave() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct PMLoad : public IconOption {
    PMLoad(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMLoad() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct PMDelete : public IconOption {
    PMDelete(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMDelete() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct PMCreateNew : public IconOption {
    PMCreateNew(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMCreateNew() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct PMPopDelete : public IconOption {
    PMPopDelete(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMPopDelete() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

// total size: 0x16C
struct UIDeleteProfile : public IconScrollerMenu {
    UIDeleteProfile(ScreenConstructorData* sd);
    ~UIDeleteProfile() override;
    void Setup() override;
    void Refresh();
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
};

// total size: 0x170
struct UIProfileManager : public IconScrollerMenu {
    PMSave* mpSave; // offset 0x16C

    UIProfileManager(ScreenConstructorData* sd);
    ~UIProfileManager() override;
    void Refresh();
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    void Setup() override;
};

MenuScreen* CreateUIProfileManager(ScreenConstructorData* sd);

#endif
