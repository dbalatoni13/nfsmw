#ifndef FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARDBASE_H
#define FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARDBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

struct UIMemcardList;
struct MemoryCardMessage;

enum MemCardFileFlag {
    MCFF_None = 0,
};

// total size: 0x40
struct UIMemcardKeyboard : public MenuScreen {
    FEString* m_pTitleMaster;    // offset 0x2C, size 0x4
    FEString* m_pDisplayMsg;    // offset 0x30, size 0x4
    FEString* m_pDisplayMsgShadow; // offset 0x34, size 0x4
    FEString* m_pOK;            // offset 0x38, size 0x4
    FEString* m_pCancel;        // offset 0x3C, size 0x4

    UIMemcardKeyboard(ScreenConstructorData* sd);
    ~UIMemcardKeyboard() override {}
    virtual void Setup();
    virtual void Abort() {}
    void ShowKeyboard();
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
};

// total size: 0x94
struct UIMemcardBase : public UIMemcardKeyboard {
    // total size: 0x50
    struct Item : public bTNode<UIMemcardBase::Item> {
        char m_Name[32];          // offset 0x8, size 0x20
        char m_Data[32];          // offset 0x28, size 0x20
        MemCardFileFlag m_Flag;   // offset 0x48, size 0x4
        int m_Size;               // offset 0x4C, size 0x4

        ~Item() {}
        Item() {}
    };

    char m_FileName[32];          // offset 0x40, size 0x20
    int mIndex;                   // offset 0x60, size 0x4
    int m_Flow;                   // offset 0x64, size 0x4
    bool m_ExpectingInput;        // offset 0x68, size 0x1
    int m_LoadedNetConfig;        // offset 0x6C, size 0x4
    int m_nMsgOptions;            // offset 0x70, size 0x4
    bool m_bVisible;              // offset 0x74, size 0x1
    bool m_bDelayedFailed;        // offset 0x78, size 0x1
    bool m_bInButtonAnimation;    // offset 0x7C, size 0x1
    bool m_bAnyButtonVisible;     // offset 0x80, size 0x1
    UIMemcardList* m_pChild;      // offset 0x84, size 0x4
    bTList<UIMemcardBase::Item> m_Items; // offset 0x88, size 0x8
    bool m_SimPausedForMemcard;   // offset 0x90, size 0x1

    bool IsInButtonAnimation() { return m_bInButtonAnimation; }

    UIMemcardBase(ScreenConstructorData* sd);
    ~UIMemcardBase() override;
    void Abort() override;
    virtual void ShowKeyboard();
    virtual void DoSelect(const char* pFileName);

    eMenuSoundTriggers NotifySoundMessage(unsigned long msg,
                                           eMenuSoundTriggers maybe) override;
    void NotificationMessage(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2) override;
    void HandleButtonPressed(unsigned long msg, FEObject* obj, unsigned long param1,
                             unsigned long param2, bool bSound);
    void HideAllButtons();
    void ShowButton(int index, bool bShow, short* pText);
    void SetButtonText(short* pBtn1, short* pBtn2, short* pBtn3);
    void SetMessage(short* pText);
    void ShowOK(unsigned int textHash, unsigned int iconHash);
    void ShowYesNo(unsigned int textHash, unsigned int iconHash);
    void SetScreenVisible(bool bVisible, int delay);
    void SetIcon(unsigned int iconHash);
    void TranslateButton(FEObject* pButton);
    bool AddItem(const char* pName, const char* pDate, int size, int flag);
    bool IsProfile(const char* pName);
    int BuildDeleteList(const char* pName, const char** pList);
    void EmptyFileList();
    Item* FindItem(const char* pName);
    void InitCompleteDoList();
    void InitComplete();
    void ExitComplete();
    void NotificationMessageGoThroughAll(unsigned long msg, FEObject* obj,
                                         unsigned long param1, unsigned long param2);
    void SetupPromptSaveCorrupt();
    void SetupPromptOverwrite();
    void SetupPromptDelete();
    void SetupPromptLoadingCorrupt();
    void SetupPromptFormatCard();
    void SetupPromptAutoSaveEnable();
    void SetupPromptAutoSaveDisable();
    void SetupPromptNoProfileFound();
    void SetupPromptAutoSaveEnableFailed();
    void SetupPromptOverwriteNoSaves();
    void SetupPromptSaveConfirm();
    void SetupAutoSaveConfirmPrompt();
    void SetupPromptForSave();
    void SetupPromptCorruptProfile();
    void SetupPromptAutoSaveEnableFailedNoCard();
    void Setup() override;
    void SetStringCheckingCard();
    void DoSaveFlow(int flow);
    void SetMessageBlurbText(short* pText);
    void SetMessageBlurbText(char* pText);
    void SetMessageBlurbText(unsigned int textHash);
    void FindScreenSize(const wchar_t* msg);
    unsigned int GetAutoSaveWarning();
    unsigned int GetAutoSaveWarning2();
    void ShowMessage(MemoryCardMessage* msg);
    void ShowMessage(const wchar_t* msg, unsigned int nOptions, const wchar_t* option1,
                     const wchar_t* option2, const wchar_t* option3);
    void ActivateChild();
    void PopChild();
    void HandleAutoSaveError();
    void HandleAutoSaveOverwriteMessage();
};

#endif
