#ifndef FRONTEND_MEMORYCARD_MEMORYCARD_H
#define FRONTEND_MEMORYCARD_MEMORYCARD_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

#include "MemoryCardHelper.hpp"
#include "RealmcIface.hpp"

struct MemoryCardImp;

enum GCImageFormat {
    GC_IMAGE_FORMAT_CI8 = 0,
};

enum GCAnimationImageLoop {
    GC_ANIMATION_LOOP_NONE = 0,
};

struct GCIconDataInfo {
    int numIconFrames;
    char *imageData;
    GCAnimationImageLoop animationLoop;
};

struct GCBannerDataInfo {
    char *imageData;
    GCImageFormat imageFormat;
};

struct UIMemcardBase;

enum eLanguages {
    eLANGUAGE_NONE = -1,
    eLANGUAGE_FIRST = 0,
    eLANGUAGE_ENGLISH = 0,
    eLANGUAGE_FRENCH = 1,
    eLANGUAGE_GERMAN = 2,
    eLANGUAGE_ITALIAN = 3,
    eLANGUAGE_SPANISH = 4,
    eLANGUAGE_DUTCH = 5,
    eLANGUAGE_SWEDISH = 6,
    eLANGUAGE_DANISH = 7,
    eLANGUAGE_KOREAN = 8,
    eLANGUAGE_CHINESE = 9,
    eLANGUAGE_JAPANESE = 10,
    eLANGUAGE_THAI = 11,
    eLANGUAGE_POLISH = 12,
    eLANGUAGE_FINNISH = 13,
    eLANGUAGE_LARGEST = 14,
    eLANGUAGE_LABELS = 15,
    eLANGUAGE_MAX = 16,
};

using RealmcIface::BootupCheckParams;

// total size: 0x1804
enum MessageChoices {
    CHOICE_NONE = 0,
    CHOICE_OPTION1 = 1,
    CHOICE_OPTION2 = 2,
    CHOICE_OPTION3 = 3,
    CHOICE_OPTION4 = 4,
};

struct MemoryCardMessage {
    int mMsg[1024];           // offset 0x0, size 0x1000
    unsigned int mnOptions;   // offset 0x1000, size 0x4
    int mOptions[4][128];     // offset 0x1004, size 0x800

    MemoryCardMessage(const wchar_t *msg, unsigned int nOptions, const wchar_t **options);
};

// total size: 0x198
struct MemoryCard {
    enum SaveType {
        ST_PROFILE = 0,
        ST_THUMBNAIL = 1,
        ST_IMAGE = 2,
        ST_MAX = 3,
    };

    enum _MemOp {
        MO_NONE = 0,
        MO_BootUp = 1,
        MO_CheckCard = 2,
        MO_Save = 3,
        MO_AutoSave = 4,
        MO_Load = 5,
        MO_Delete = 6,
        MO_List = 7,
        MO_FakeLoad = 8,
        MO_LoadYNCF = 9,
        MO_SetMonitor = 10,
    };

    // Members
    BootupCheckParams m_BootupParams;       // offset 0x0, size 0x10
    GCIconDataInfo *m_pRMIcon;              // offset 0x10, size 0x4
    GCBannerDataInfo *m_pRMBanner;          // offset 0x14, size 0x4
    void *m_pLocaleFileHandler;             // offset 0x18, size 0x4
    bool m_bWaitingForResponse;             // offset 0x1C, size 0x1
    bool m_bBootFoundFile;                  // offset 0x20, size 0x1
    bool m_bAutoSave;                       // offset 0x24, size 0x1
    bool m_bAutoSaveCardPulled;             // offset 0x28, size 0x1
    bool m_bInBootSequence;                 // offset 0x2C, size 0x1
    bool m_bRetryBootCheck;                 // offset 0x30, size 0x1
    bool m_bManualSave;                     // offset 0x34, size 0x1
    bool m_bAutoSaveCardPulledDuringSave;   // offset 0x38, size 0x1
    bool m_bOldSaveFileExists;              // offset 0x3C, size 0x1
    bool m_bListingOldSaveFiles;            // offset 0x40, size 0x1
    bool m_bInAutoSave;                     // offset 0x44, size 0x1
    bool m_bAutoSaveRequested;              // offset 0x48, size 0x1
    bool m_bCheckingCardForAutoSave;        // offset 0x4C, size 0x1
    bool m_bFoundAutoSaveFile;              // offset 0x50, size 0x1
    bool m_bCheckingCardForOverwrite;       // offset 0x54, size 0x1
    bool m_bMemcardScreenShowing;           // offset 0x58, size 0x1
    bool m_bCardRemoved;                    // offset 0x5C, size 0x1
    bool m_bRetryAutoSave;                  // offset 0x60, size 0x1
    bool m_bInitialized;                    // offset 0x64, size 0x1
    bool m_bDisablingAutoSaveForSave;       // offset 0x68, size 0x1
    bool m_bAutoLoading;                    // offset 0x6C, size 0x1
    bool m_bListingForCreate;              // offset 0x70, size 0x1
    bool m_bHUDLoaded;                      // offset 0x74, size 0x1
    bool m_bCancelNextAutoSave;             // offset 0x78, size 0x1
    bool m_bMonitorOn;                      // offset 0x7C, size 0x1
    bool m_bAutoSaveIconShowing;            // offset 0x80, size 0x1
    bool m_bNeedToAllowControllerErrors;    // offset 0x84, size 0x1
    bool m_bNonSilentAutoSave;              // offset 0x88, size 0x1
    bool m_bAutoLoadDone;                   // offset 0x8C, size 0x1
    bool m_bMemcardScreenExiting;           // offset 0x90, size 0x1
    MemoryCardMessage *m_PendingMessage;    // offset 0x94, size 0x4
    GameInfo *m_pGameInfo;                  // offset 0x98, size 0x4
    int m_ReqOp;                            // offset 0x9C, size 0x4
    const char *m_ReqFilename;              // offset 0xA0, size 0x4
    int m_MemOp;                            // offset 0xA4, size 0x4
    char *m_pBuffer;                        // offset 0xA8, size 0x4
    unsigned short m_LastError;             // offset 0xAC, size 0x2
    unsigned short m_SpecialError;          // offset 0xAE, size 0x2
    int m_EntryCount;                       // offset 0xB0, size 0x4
    int m_nPlayer;                          // offset 0xB4, size 0x4
    unsigned int m_Header[2];               // offset 0xB8, size 0x8
    char m_Filename[32];                    // offset 0xC0, size 0x20
    char m_BootupFilename[32];              // offset 0xE0, size 0x20
    int m_GameTitle[32];                    // offset 0x100, size 0x80
    SaveType m_Type;                        // offset 0x180, size 0x4
    unsigned int m_DataSize;                // offset 0x184, size 0x4
    int m_TimeOffsetSec;                    // offset 0x188, size 0x4
    RealmcIface::MemcardInterface *m_pIMemcard; // offset 0x18C, size 0x4
    UIMemcardBase *m_pFEScreen;             // offset 0x190, size 0x4
    MemoryCardImp *m_pImp;                  // offset 0x194, size 0x4

    // Static members
    static MemoryCard *s_pThis;

    // Inline functions
    static inline MemoryCard *GetInstance() { return s_pThis; }
    inline void FEngLinkObjects(UIMemcardBase *pMenuScreen) { m_pFEScreen = pMenuScreen; }
    inline bool IsTypeProfile() { return m_Type == ST_PROFILE; }
    inline bool IsAutoSave() { return m_bAutoSave; }
    inline int GetOp() { return m_MemOp; }
    inline unsigned int GetSize() { return m_DataSize; }
    inline bool InBootSequence() { return m_bInBootSequence; }
    inline void SetPlayerNum(int player) { m_nPlayer = player; }
    inline int GetPlayerNum() { return m_nPlayer; }
    inline bool PromptForAutoSave() { return !m_bAutoSave; }
    inline void CancelNextAutoSave() { m_bCancelNextAutoSave = true; }
    inline void RequestAutoSave() { m_bAutoSaveRequested = true; }
    inline bool AutoSaveRequested() { return m_bAutoSaveRequested; }
    inline bool IsAutoSaving() { return m_bInAutoSave; }
    inline bool IsCheckingCardForAutoSave() { return m_bCheckingCardForAutoSave; }
    inline bool IsCheckingCardForOverwrite() { return m_bCheckingCardForOverwrite; }
    inline bool IsRetryingAutoSave() { return m_bRetryAutoSave; }
    inline void SetRetryAutoSave(bool bRetry) { m_bRetryAutoSave = bRetry; }
    inline void SetHUDLoaded() { m_bHUDLoaded = true; }
    inline bool IsMonitorOn() { return m_bMonitorOn; }
    inline bool IsAutoLoading() { return m_bAutoLoading; }
    inline bool IsMemcardScreenShowing() { return m_bMemcardScreenShowing; }
    inline void SetMemcardScreenShowing(bool bShowing) { m_bMemcardScreenShowing = bShowing; }
    inline bool IsMemcardScreenInitialized() { return m_bInitialized; }
    inline void SetMemcardScreenInitialized(bool bInit) { m_bInitialized = bInit; }
    inline bool IsListingForCreate() { return m_bListingForCreate; }
    inline void SetListingForCreate(bool bListing) { m_bListingForCreate = bListing; }
    inline void ResetAutoSaveCardPulled() { m_bAutoSaveCardPulled = false; }
    inline bool CardPulledInAutoSave() { return m_bAutoSaveCardPulled; }
    inline bool WasCardRemovedWithAutoSaveEnabled() { return m_bCardRemoved; }
    inline void SetCardRemovedWithAutoSaveEnabled(bool bRemoved) { m_bCardRemoved = bRemoved; }
    inline bool IsManualSave() { return m_bManualSave; }
    inline bool IsListingOldSaveFiles() { return m_bListingOldSaveFiles; }
    inline bool IsMemcardScreenExiting() { return m_bMemcardScreenExiting; }
    inline void SetMemcardScreenExiting(bool bExiting) { m_bMemcardScreenExiting = bExiting; }
    inline bool IsAutoLoadDone() { return m_bAutoLoadDone; }
    inline void SetAutoLoadDone(bool bDone) { m_bAutoLoadDone = bDone; }
    inline MemoryCardMessage *GetPendingMessage() { return m_PendingMessage; }
    inline void SetWaitingForResponse(bool bWaiting) { m_bWaitingForResponse = bWaiting; }
    inline bool IsWaitingForResponse() { return m_bWaitingForResponse; }
    inline GCIconDataInfo *GetSaveIcon() { return m_pRMIcon; }
    inline GCBannerDataInfo *GetSaveBanner() { return m_pRMBanner; }
    static inline int GetLastError() { return s_pThis->m_LastError; }
    static inline int GetSpecialError() { return s_pThis->m_SpecialError; }
    static inline bool IsProfile(const char *name);
    inline void SetBootFound(bool b) { m_bBootFoundFile = b; }
    inline UIMemcardBase *GetScreen() { return m_pFEScreen; }
    inline char *GetHeader() { return reinterpret_cast< char * >(m_Header); }
    inline char *GetData() { return m_pBuffer; }

    // Non-inline functions
    MemoryCard();
    ~MemoryCard();
    static bool IsCardAvailable();
    static void SetExtraParam(SaveType t, const char *filename, void *buf, unsigned int size);
    static int GetEntryCount();
    void InitCommand(int op);
    static bool FoundInBoot();
    void RequestTask(int op, const char *name);
    void ProcessTask();
    static int GetImageIndex(const char *name);
    static int GetImageFileProfileNameLength(const char *name);
    void BuildImageDisplayName();
    static bool IsCardBusy();
    void Init();
    void Stop();
    void StartBootSequence();
    void EndBootSequence();
    bool CheckForOldSaveFile();
    static void LoadLocale(eLanguages eLang);
    int GetPrefixLength();
    const char *GetPrefix();
    static const char *GetLocaleString(int strID);
    static const unsigned short *GetCheckCardString();
    void RefreshActiveCard();
    void SetActiveCard(RealmcIface::CardId cardId);
    static void SetMessageMode(unsigned int msg, bool flag);
    static void TickCardRemoval();
    void Tick(int TickCount);
    void MessageDone(RealmcIface::MessageChoices nInput);
    void BootupCheck(const char *entry);
    bool ShouldDoAutoSave(bool bForce);
    void StartAutoSave(bool bForce);
    void DoAutoSave();
    void EndAutoSave();
    void StartListingOldSaveFiles();
    void EndListingOldSaveFiles();
    void SetMonitor(bool bEnabled);
    void SetAutoSaveEnabled(bool bEnabled);
    void ShowOnlyAutoSaveMessages();
    void ShowOnlyCreateListMessages();
    void ShowMessages(bool bShow);
    void CheckCard(int iSlot);
    void FakeLoad(int iSlot);
    void LoadYNCF(int iSlot);
    void Save(const char *entryName);
    void List(const char *filter, RealmcIface::TitleInfo *titleInfo);
    void Load(const char *filename);
    void Delete(const char *filename);
    void ListOldSaveFilesNGC();
    void ListOldSaveFilesPC();
    void ListOldSaveFilesPS2();
    void ListOldSaveFilesXbox();
    void ReleasePendingMessage();
    void HandleAutoSaveError();
    void HandleAutoSaveOverwriteMessage();
    void ShowAutoSaveIcon();
    void HideAutoSaveIcon();
    bool IsAutoSaveIconVisible();
};

void InitMemoryCard();
void CaptureJoyOp(MemoryCardJoyLoggableEvents op);
void DisplayStatus(int status);

#include "MemoryCardImp.hpp"

inline bool MemoryCard::IsProfile(const char *name) {
    return name[s_pThis->m_pImp->GetPrefix() - name] == 'P';
}

#endif
