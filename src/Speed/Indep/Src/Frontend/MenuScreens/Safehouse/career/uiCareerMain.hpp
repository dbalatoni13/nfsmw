#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMAIN_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_CAREER_UICAREERMAIN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconScrollerMenu.hpp"

struct uiCareerCrib : public IconScrollerMenu {
    uiCareerCrib(ScreenConstructorData* sd);
    ~uiCareerCrib() override;
    void NotificationMessage(unsigned long msg, FEObject* pobj, unsigned long param1, unsigned long param2) override;
    void Setup() override;
};

struct CResumeFreeRoam : public IconOption {
    CResumeFreeRoam(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CResumeFreeRoam() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CCarSelect : public IconOption {
    CCarSelect(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CCarSelect() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CRapSheet : public IconOption {
    CRapSheet(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CRapSheet() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CTop15 : public IconOption {
    CTop15(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CTop15() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CSave : public IconOption {
    CSave(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CSave() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CResumeCareer : public IconOption {
    CResumeCareer(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CResumeCareer() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CStartNewCareer : public IconOption {
    CStartNewCareer(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CStartNewCareer() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

struct CLoadCareer : public IconOption {
    CLoadCareer(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash)
        : IconOption(tex_hash, name_hash, desc_hash) {}
    ~CLoadCareer() override {}
    void React(const char* pkg_name, unsigned int data, FEObject* obj, unsigned int param1, unsigned int param2) override;
};

#endif
