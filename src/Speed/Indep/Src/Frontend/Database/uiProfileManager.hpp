#ifndef FRONTEND_DATABASE_UIPROFILEMANAGER_H
#define FRONTEND_DATABASE_UIPROFILEMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

class PMSave : public IconOption {
  public:
    PMSave(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMSave() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class PMLoad : public IconOption {
  public:
    PMLoad(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMLoad() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class PMDelete : public IconOption {
  public:
    PMDelete(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMDelete() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class PMCreateNew : public IconOption {
  public:
    PMCreateNew(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMCreateNew() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class PMPopDelete : public IconOption {
  public:
    PMPopDelete(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~PMPopDelete() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

// total size: 0x170
class UIProfileManager : public IconScrollerMenu {
  public:
    UIProfileManager(ScreenConstructorData *sd);
    ~UIProfileManager() override {};
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
    void Refresh();

    PMSave *mpSave; // offset 0x16C
};

// total size: 0x16C
class UIDeleteProfile : public IconScrollerMenu {
  public:
    UIDeleteProfile(ScreenConstructorData *sd);
    ~UIDeleteProfile() override {};
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
    void Refresh();
};

MenuScreen *CreateUIProfileManager(ScreenConstructorData *sd);

#endif
