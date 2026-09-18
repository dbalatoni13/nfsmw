#ifndef FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARDBASE_H
#define FRONTEND_MENUSCREENS_MEMCARD_UIMEMCARDBASE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"

struct FEString;
struct MemoryCardMessage;
class UIMemcardList;

// total size: 0x40
class UIMemcardKeyboard : public MenuScreen {
  public:
    UIMemcardKeyboard(ScreenConstructorData *sd);
    ~UIMemcardKeyboard() override {}

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    void ShowKeyboard();

    virtual void Setup();

    virtual void Abort() {}

  protected:
    FEString *m_pTitleMaster;     // offset 0x2C, size 0x4
    FEString *m_pDisplayMsg;      // offset 0x30, size 0x4
    FEString *m_pDisplayMsgShadow; // offset 0x34, size 0x4
    FEString *m_pOK;              // offset 0x38, size 0x4
    FEString *m_pCancel;          // offset 0x3C, size 0x4
};

// total size: 0x94
class UIMemcardBase : public UIMemcardKeyboard {
  public:
    // total size: 0x50
    struct Item : public bTNode<UIMemcardBase::Item> {
        Item() {}
        ~Item() {}

        char m_Name[32];        // offset 0x8, size 0x20
        char m_Data[32];        // offset 0x28, size 0x20
        MemCardFileFlag m_Flag; // offset 0x48, size 0x4
        int m_Size;             // offset 0x4C, size 0x4
    };

    UIMemcardBase(ScreenConstructorData *sd);
    ~UIMemcardBase() override;

    void NotificationMessageGoThroughAll(u32 msg, FEObject *obj, u32 param1, u32 param2);
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

    void TranslateButton(FEObject *obj);
    void SetStringCheckingCard();
    void SetMessage(short *pMsg);
    void SetButtonText(short *b1, short *b2, short *b3);
    void SetIcon(unsigned int icon);
    void AddJoySelect(FEObject *obj);
    void RemoveJoySelect(FEObject *obj);
    int BuildDeleteList(const char *pName, const char **pList);
    bool IsProfile(const char *pName);
    void ShowYesNo(unsigned int language_main, unsigned int flag);
    void ShowOK(unsigned int language_main, unsigned int flag);
    void SetScreenVisible(bool visible, int nButtons);

    virtual void ShowKeyboard();
    virtual void DoSelect(const char *pFileName) {}

    void InitCompleteDoList();
    void InitComplete();
    void ExitComplete();

    void Setup() override;
    void Abort() override;

    void HandleAutoSaveError();
    void HandleAutoSaveOverwriteMessage();
    void HandleButtonPressed(u32 msg, FEObject *obj, u32 param1, u32 param2, bool bPadBack);
    void DoSaveFlow(int flow);

    bool IsExpectingInput() {
        return m_ExpectingInput;
    }

    void ShowMessage(MemoryCardMessage *msg);
    void ShowMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t *option1, const wchar_t *option2, const wchar_t *option3);

    bool IsInButtonAnimation() {
        return m_bInButtonAnimation;
    }

    void ActivateChild();
    void PopChild();

  protected:
    void SetupPromptNoProfileFound();
    void SetupPromptSaveConfirm();
    void SetupPromptForSave();
    void SetupPromptBootloadOK();
    void SetupAutoSaveOverwritePrompt();
    void SetupAutoSaveConfirmPrompt();
    void SetupPromptAutoSaveEnableFailedNoCard();
    void SetupPromptCorruptProfile();
    void HideAllButtons();
    void ShowButton(int idx, bool bShow, short *pText);
    Item *FindItem(const char *pName);

  public:
    bool AddItem(const char *pName, const char *pDate, int size, int flag);
    void EmptyFileList();

  private:
    void SetMessageBlurbText(short *pText);
    void SetMessageBlurbText(char *pText);
    void SetMessageBlurbText(unsigned int textHash);
    void FindScreenSize(const wchar_t *msg);
    unsigned int GetAutoSaveWarning();
    unsigned int GetAutoSaveWarning2();

  public:
    char m_FileName[32]; // offset 0x40, size 0x20

  public:
    int mIndex;                // offset 0x60, size 0x4
    int m_Flow;                // offset 0x64, size 0x4
    bool m_ExpectingInput;     // offset 0x68, size 0x1
    int m_LoadedNetConfig;     // offset 0x6C, size 0x4
    int m_nMsgOptions;         // offset 0x70, size 0x4
    bool m_bVisible;           // offset 0x74, size 0x1
    bool m_bDelayedFailed;     // offset 0x78, size 0x1
    bool m_bInButtonAnimation; // offset 0x7C, size 0x1
    bool m_bAnyButtonVisible;  // offset 0x80, size 0x1
    UIMemcardList *m_pChild;   // offset 0x84, size 0x4
    bTList<Item> m_Items;      // offset 0x88, size 0x8

  private:
    bool m_SimPausedForMemcard; // offset 0x90, size 0x1
};

#endif
