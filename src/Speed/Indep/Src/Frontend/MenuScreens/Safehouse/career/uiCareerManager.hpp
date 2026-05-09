#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMANAGER_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMANAGER_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

class uiCareerManager : public IconScrollerMenu {
  public:
    uiCareerManager(ScreenConstructorData *sd);
    ~uiCareerManager() override;
    void NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) override;
    void Setup() override;
};

class CResumeCareer : public IconOption {
  public:
    CResumeCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CResumeCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class CStartNewCareer : public IconOption {
  public:
    CStartNewCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CStartNewCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

class CLoadCareer : public IconOption {
  public:
    CLoadCareer(uint32 tex_hash, uint32 name_hash, uint32 desc_hash) : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CLoadCareer() override {}
    void React(const char *pkg_name, uint32 data, FEObject *obj, uint32 param1, uint32 param2) override;
};

#endif
