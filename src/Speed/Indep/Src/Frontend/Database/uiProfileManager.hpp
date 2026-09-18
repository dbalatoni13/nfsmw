#ifndef FRONTEND_DATABASE_UIPROFILEMANAGER_H
#define FRONTEND_DATABASE_UIPROFILEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

void MemcardEnter(const char* from, const char* to, unsigned int op, void (*pTermFunc)(void*),
                  void* pTermFuncParam, unsigned int msgSuccess, unsigned int msgFailed);

struct PMSave : public IconOption {
    PMSave(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMSave() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            MemcardEnter(pkg_name, pkg_name, 0x2251, 0, 0, 0, 0);
        }
    }

};

struct PMLoad : public IconOption {
    PMLoad(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMLoad() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            MemcardEnter(pkg_name, pkg_name, 0x411, 0, 0, 0x3A2BE557, 0x8867412D);
        }
    }

};

struct PMDelete : public IconOption {
    PMDelete(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMDelete() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            MemcardEnter(pkg_name, pkg_name, 0x31, 0, 0, 0, 0);
        }
    }

};

struct PMCreateNew : public IconOption {
    PMCreateNew(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMCreateNew() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            MemcardEnter(pkg_name, pkg_name, 0x61, 0, 0, 0, 0);
        }
    }

};

struct PMPopDelete : public IconOption {
    PMPopDelete(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMPopDelete() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override {
        if (data == 0x0C407210) {
            MemcardEnter(pkg_name, pkg_name, 0x61, 0, 0, 0, 0);
        }
    }

};

// total size: 0x170
struct UIProfileManager : public IconScrollerMenu {
    PMSave* mpSave; // offset 0x16C

    UIProfileManager(ScreenConstructorData* sd);
    ~UIProfileManager() override {}
    void Refresh();
    void NotificationMessage(u32 msg, FEObject* obj, u32 param1,
                             u32 param2) override;
    void Setup() override;
};

// total size: 0x16C
struct UIDeleteProfile : public IconScrollerMenu {
    UIDeleteProfile(ScreenConstructorData* sd);
    ~UIDeleteProfile() override {}
    void Setup() override;
    void Refresh();
    void NotificationMessage(u32 msg, FEObject* obj, u32 param1,
                             u32 param2) override;
};

MenuScreen* CreateUIProfileManager(ScreenConstructorData* sd);

#endif
