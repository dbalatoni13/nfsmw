#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "types.h"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"

class uiCareerCrib : public IconScrollerMenu {
  public:
    uiCareerCrib(ScreenConstructorData *sd);
    ~uiCareerCrib() override {};
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;

  private:
    void Setup() override;
};

class CResumeFreeRoam : public IconOption {
  public:
    CResumeFreeRoam(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CResumeFreeRoam() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class CCarSelect : public IconOption {
  public:
    CCarSelect(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CCarSelect() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class CRapSheet : public IconOption {
  public:
    CRapSheet(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CRapSheet() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class CTop15 : public IconOption {
  public:
    CTop15(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CTop15() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class CSave : public IconOption {
  public:
    CSave(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CSave() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

#endif
