#ifndef __MEMCARDBASE_HPP__
#define __MEMCARDBASE_HPP__

#include <types.h>
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/bWare/Inc/bList.hpp"
#include "Speed/Indep/Src/FEng/FEString.h"

class UIMemcardList;

// total size: 0x24
class FEMemWidget : public ScrollerDatum {
  public:
    static const int MAX_SIZE; // size: 0x4, address: 0xFFFFFFFF
    MemCardFileFlag m_Flag;    // offset 0x18, size 0x4
    int m_Size;                // offset 0x1C, size 0x4
    UIMemcardList *m_pParent;  // offset 0x20, size 0x4

    FEMemWidget() {}
    ~FEMemWidget() override {}

    void Act(const char *parent_pkg, uint32 data) {}
    bool IsCorrupt() {}
    int GetSize() const {}
    const char *GetFileName() {}
};

// total size: 0x104
class UIMemcardList : public MenuScreen {
  public:
    Scrollerina m_SaveGameList; // offset 0x2C, size 0xC8

    enum ListOp {
        MCLO_Load = 0,
        MCLO_Delete = 1,
    };

  private:
    int m_Initialized;         // offset 0xF4, size 0x4
    int m_ListOp;              // offset 0xF8, size 0x4
    uint32 m_LastMsg;          // offset 0xFC, size 0x4
    FEMemWidget *m_pCreateNew; // offset 0x100, size 0x4

  public:
    UIMemcardList(ScreenConstructorData *sd);
    ~UIMemcardList() override;

    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;

    FEMemWidget *AddItem(const char *pName, const char *pDate, int size, int flag);

    const char *GetFileName(int find);

    int GetSize() {
        return m_SaveGameList.GetNumData();
    }

    bool IsReady() {
        return m_Initialized != 0;
    }

    enum ListOp GetListOp() {
        return static_cast<ListOp>(m_ListOp);
    }
};

// total size: 0x40
class UIMemcardKeyboard : public MenuScreen {
  protected:
    FEString *m_pTitleMaster;      // offset 0x2C, size 0x4
    FEString *m_pDisplayMsg;       // offset 0x30, size 0x4
    FEString *m_pDisplayMsgShadow; // offset 0x34, size 0x4
    FEString *m_pOK;               // offset 0x38, size 0x4
    FEString *m_pCancel;           // offset 0x3C, size 0x4

  public:
    UIMemcardKeyboard(ScreenConstructorData *sd);
    ~UIMemcardKeyboard() override {}
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    void ShowKeyboard();
    virtual void Setup();
    virtual void Abort() {}
};

// total size: 0x94
class UIMemcardBase : public UIMemcardKeyboard {
  public:
    // total size: 0x50
    class Item : public bTNode<UIMemcardBase::Item> {
      public:
        char m_Name[32];        // offset 0x8, size 0x20
        char m_Data[32];        // offset 0x28, size 0x20
        MemCardFileFlag m_Flag; // offset 0x48, size 0x4
        int m_Size;             // offset 0x4C, size 0x4
    };

    char m_FileName[32]; // offset 0x40, size 0x20
  protected:
    int mIndex;                          // offset 0x60, size 0x4
    int m_Flow;                          // offset 0x64, size 0x4
    bool m_ExpectingInput;               // offset 0x68, size 0x1
    int m_LoadedNetConfig;               // offset 0x6C, size 0x4
    int m_nMsgOptions;                   // offset 0x70, size 0x4
    bool m_bVisible;                     // offset 0x74, size 0x1
    bool m_bDelayedFailed;               // offset 0x78, size 0x1
    bool m_bInButtonAnimation;           // offset 0x7C, size 0x1
    bool m_bAnyButtonVisible;            // offset 0x80, size 0x1
    UIMemcardList *m_pChild;             // offset 0x84, size 0x4
    bTList<UIMemcardBase::Item> m_Items; // offset 0x88, size 0x8

  private:
    bool m_SimPausedForMemcard; // offset 0x90, size 0x1

  public:
    UIMemcardBase(ScreenConstructorData *sd);
    ~UIMemcardBase() override;
    void NotificationMessageGoThroughAll(u32 msg, FEObject *obj, u32 param1, u32 param2);
    void NotificationMessage(u32 msg, FEObject *obj, u32 param1, u32 param2) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;
    void TranslateButton(FEObject *obj);
    void SetStringCheckingCard();
    void SetMessage(i16 *pMsg);
    void SetButtonText(i16 *b1, i16 *b2, i16 *b3);
    void SetIcon(uint32 icon);
    void AddJoySelect(FEObject *obj);
    void RemoveJoySelect(FEObject *obj);
    int BuildDeleteList(const char *pName, const char **pList);
    bool IsProfile(const char *pName);
    void ShowYesNo(uint32 language_main, uint32 flag);
    void ShowOK(uint32 language_main, uint32 flag);
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
    void ShowMessage(const wchar_t *msg, uint32_t nOptions, const wchar_t *option1, const wchar_t *option2, const wchar_t *option3);
    bool IsInButtonAnimation() {
        return m_bInButtonAnimation;
    }
    void ActivateChild();
    void PopChild();

  protected:
    void SetupPromptNoProfileFound();
    void SetupAutoSaveConfirmPrompt();
    void SetupPromptSaveConfirm();
    void SetupPromptForSave();
    void SetupPromptBootloadOK();
    void SetupAutoSaveOverwritePrompt();
    void SetupPromptAutoSaveEnableFailedNoCard();
    void SetupPromptCorruptProfile();
    void HideAllButtons();
    void ShowButton(int idx, bool bShow, i16 *pText);
    Item *FindItem(const char *pName);

  public:
    bool AddItem(const char *pName, const char *pDate, int size, int flag);
    void EmptyFileList();

  private:
    void SetMessageBlurbText(i16 *pText);
    void SetMessageBlurbText(char *pText);
    void SetMessageBlurbText(uint32 textHash);
    void FindScreenSize(const wchar_t *msg);
    uint32 GetAutoSaveWarning();
    uint32 GetAutoSaveWarning2();

    friend class UIMemcardList;
};

#endif
