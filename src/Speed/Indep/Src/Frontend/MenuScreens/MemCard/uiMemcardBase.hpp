#ifndef _UIMEMCARDBASE
#define _UIMEMCARDBASE

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct UIMemcardBase : FEMenuScreen {
    char m_FileName[60];
    bool m_bInButtonAnimation;
    int m_Dummy80;

    bool AddItem(const char* pName, const char* pDate, int size, int flag);
    bool IsProfile(const char* pName);
    void ShowMessage(const int* msg, unsigned int nOptions,
                     const int* opt0, const int* opt1, const int* opt2);
    void SetStringCheckingCard();
    void EmptyFileList();
    void HandleAutoSaveError();
    void HandleAutoSaveOverwriteMessage();
    void InitCompleteDoList();
    void DoSelect(const char* filename);
    unsigned int GetAutoSaveWarning();
    unsigned int GetAutoSaveWarning2();
};

#endif
