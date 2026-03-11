#ifndef _UIMEMCARDBASE
#define _UIMEMCARDBASE

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

struct UIMemcardBase : public MenuScreen {
    char m_FileName[60];
    bool m_bInButtonAnimation;
    int m_Dummy80;

    UIMemcardBase(ScreenConstructorData* sd) : MenuScreen(sd) {}

    bool AddItem(const char* pName, const char* pDate, int size, int flag);
    bool IsProfile(const char* pName);
    void ShowMessage(const int* msg, unsigned int nOptions,
                     const int* opt0, const int* opt1, const int* opt2);
    void SetStringCheckingCard();
    void EmptyFileList();
    void HandleAutoSaveError();
    void HandleAutoSaveOverwriteMessage();
    void InitCompleteDoList();
    virtual void DoSelect(const char* filename);
    virtual void Abort() {}
    void ShowKeyboard();
    unsigned int GetAutoSaveWarning();
    unsigned int GetAutoSaveWarning2();
};

struct UIMemcardKeyboard : public MenuScreen {
    UIMemcardKeyboard(ScreenConstructorData* sd) : MenuScreen(sd) {}
    ~UIMemcardKeyboard() override {}
    virtual void Abort() {}
    void Setup() override;
    void ShowKeyboard();
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
};

#endif
