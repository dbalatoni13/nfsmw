#ifndef FRONTEND_MENUSCREENS_COMMON_FEMENUSCREEN_H
#define FRONTEND_MENUSCREENS_COMMON_FEMENUSCREEN_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/FEng/FEObject.h"

#include <types.h>

enum eMenuSoundTriggers {
    UISND_NONE = -1,
    UISND_COMMON_UP = 0,
    UISND_COMMON_DOWN = 1,
    UISND_COMMON_LEFT = 2,
    UISND_COMMON_RIGHT = 3,
    UISND_COMMON_SELECT = 4,
    UISND_COMMON_BACK = 5,
    UISND_COMMON_START_PAUSE = 6,
    UISND_COMMON_WRONG = 7,
    UISND_COMMON_DLGBOX_IN = 10,
    UISND_COMMON_DLGBOX_OUT = 11,
    UISND_COMMON_SCROLL_START = 12,
    UISND_COMMON_COOL_DOWN_FLIP = 12,
    UISNN_HUD_ENGAGE_MAIL_POPUP = 13,
    UISND_2DAYS_MOVIE_SCREEN = 14,
    UISND_HUD_ENGAGE_DPAD = 15,
    UISND_HUD_ENGAGE_MAIL = 16,
    UISND_MILESTONE_REWARD_SYMBOL_ON = 17,
    UISND_MILESTONE_REWARD_SYMBOL_MOVE = 18,
    UISND_ENTER_TRIGGER = 19,
    UISND_MAP_LOCK_TARGER = 20,
    UISND_MAP_ZOOM_IN = 21,
    UISND_MAP_ZOOM_OUT = 22,
    UISND_COMMON_MAX_NUM = 23,
    UISND_CARSEL_BASSLOOP_RESERVED00 = 24,
    UISND_CARSEL_BASSLOOP_RESERVED01 = 25,
    UISND_CARSEL_CAMROTATE = 26,
    UISND_CARSEL_CAMROTATE_RESERVED00 = 27,
    UISND_CARSEL_CAMROTATE_RESERVED01 = 28,
    UISND_CUST_INST_PAINT = 38,
    UISND_CUST_PAINT_COLOUR_LEFT = 39,
    UISND_CUST_PAINT_COLOUR_RIGHT = 40,
    UISND_CUST_PAINT_TYPE_LEFT = 41,
    UISND_CUST_PAINT_TYPE_RIGHT = 42,
    UISND_UGNEW_KBTYPE = 46,
    UISND_UGNEW_ENTER = 47,
    UISND_UGNEW_DELETE = 48,
    UISND_CUST_INST_EXHAUST = 49,
    UISND_CUST_INST_GENERIC = 50,
    UISND_CUST_INST_TURBO = 51,
    UISND_CUST_INST_NOS = 52,
    UISND_CUST_INST_TRANSMISSION = 53,
    UISND_CUST_INST_TIRES = 54,
    UISND_EA_MSGR_OPEN = 74,
    UISND_EA_MSGR_LOGOFF = 75,
    UISND_EA_MSGR_CHAT_REQ = 85,
    UISND_EA_MSGR_MAIL_RECEIVE = 86,
    UISND_EA_MSGR_CHALLENGE_REQ = 87,
    UISND_MAIN_MENU = 88,
    UISND_MAIN_SUB = 89,
    UISND_BUSTED_SCREEN = 89,
    UISND_IMPOUNDED = 90,
    UISND_RAPSHEET_LOGIN = 103,
    UISND_RAPSHEET_LOGIN2 = 104,
    UISND_RAPSHEET_MAIN = 105,
    UISND_RAPSHEET_SUMMARY = 106,
    UISND_RAPSHEET_VEHICLE = 107,
    UISND_RAPSHEET_SELECT = 108,
    UISND_RAPSHEET_BACKUP = 109,
    UISND_RAPSHEET_MOVE_BAR_UP = 110,
    UISND_RAPSHEET_MOVE_BAR_DOWN = 111,
    UISND_RAPSHEET_CTS = 112,
    UISND_RAPSHEET_INFRAC = 113,
    UISND_RAPSHEET_PD = 114,
    UISND_RAPSHEET_RANKINGS = 115,
    UISND_RAPSHEET_RANKING_DETAIL = 116,
    UISND_RAPSHEET_TEP = 117,
    UISND_RAPSHEET_EXIT = 118,
    UISND_MAIN_MENU_ENTER = 123,
    UISND_MAIN_MENU_EXIT = 124,
    UISND_MAIN_SUB_ENTER = 125,
    UISND_MAIN_SUB_EXIT = 126,
    UISND_MC_MAIN_ENTER = 127,
    UISND_BLACKLIST_ENTER = 128,
    UISND_BLACKLIST_EXIT = 129,
    UISND_BIO_ENTER = 130,
    UISND_BIO_EXIT = 131,
    UISND_BIO_TO_RIVALCAR_EXIT = 132,
    UISND_CUST_MAIN_ENTER = 133,
    UISND_CUST_ENTER = 134,
    UISND_CUST_EXIT = 135,
    UISND_SHOWCASE_ENTER = 136,
    UISND_SHOWCASE_EXIT = 137,
    UISND_QUICKRACE_BRIEF_ENTER = 138,
    UISND_QUICK_GAMBLE_BLIP = 139,
    UISND_RACESHEET_ENTER = 140,
    UISND_RACESHEET_EXIT = 141,
    UISND_TRACK_SELECT_ENTER = 142,
    UISND_QUICK_BRIEF_EXIT = 143,
    UISND_QUICK_RACE_CAR_ON = 144,
    UISND_CAR_SEL_TO_SHOWCASE = 145,
    UISND_CHAL_SER_ENTER = 146,
    UISND_CAR_SELECT_ENTER = 147,
    UISND_SAFEH_MARK_CONGRATS_OFF = 148,
    UISND_RANDOMIZE_BUTTON = 149,
    UISND_SAFEH_MARK_ENTER = 150,
    UISND_SAFEH_MARK_SPIN_COIN = 151,
    UISND_CHAL_SER_EXIT = 152,
    UISND_MAP_REPOSITION = 153,
    UISND_OPTION_MENU_ENTER = 154,
    UISND_OPTION_MENU_EXIT = 155,
    UISND_RIVAL_BIO_OFF = 156,
    UISND_BIO_ENTER2 = 157,
    UISND_RIV_BIO_CLOUD_ON = 158,
    UISND_RIV_BIO_LOGO_FLY_IN = 159,
    UISND_FRONTEND_MAX_NUM = 160,
};

// total size: 0xC
struct ScreenConstructorData {
    const char *PackageFilename; // offset 0x0, size 0x4
    struct FEPackage *pPackage;  // offset 0x4, size 0x4
    int Arg;                     // offset 0x8, size 0x4
};

// total size: 0x2C
class MenuScreen {
  public:
    MenuScreen(ScreenConstructorData *sd);

    virtual ~MenuScreen();

    void BaseNotify(u32 Message, FEObject *pObject, u32 Param1, u32 Param2);

    const char *FEngGetEditedString();

    void FEngEndTextInput();

    void FEngAbortTextInput();

    void FEngBeginTextInput(uint32 object_hash, uint32 edit_mode, const char *initial_string, const char *title_string, uint32 max_text_length);

    bool CheckKeyboard(uint32 msg);

    void BaseNotifySound(u32 msg, FEObject *obj, u32 controller_mask, u32 pkg_ptr);

    static void UpdateStatusIcons(int iconType, bool on);

    void UpdateEAMIcons();

    static void MaybeShutdownVoIPChat();

    // virtual enum eMenuSoundTriggers NotifySoundMessage(unsigned long msg, enum eMenuSoundTriggers maybe) {}

    const char *GetPackageName() {}

    // FEPackage *GetPackage() {}

    void SetAsGarageScreen() {}

  protected:
    bool mPlaySound;                      // offset 0x0, size 0x1
    unsigned long mDirectionForNextSound; // offset 0x4, size 0x4
    bool bEnableEAMessenger;              // offset 0x8, size 0x1

    static bool ShowingEAMessenger; // size: 0x1, address: 0xFFFFFFFF
    static bool EnteringChallenge;  // size: 0x1, address: 0xFFFFFFFF

  private:
    static int gEAMIconState; // size: 0x4, address: 0xFFFFFFFF

    const char *PackageFilename;                 // offset 0xC, size 0x4
    ScreenConstructorData ConstructData;         // offset 0x10, size 0xC
    bool IsGarageScreen;                         // offset 0x1C, size 0x1
    struct FEngTextInputObject *TextInputObject; // offset 0x20, size 0x4
    uint8 mStartCapturingFromKeyboard;           // offset 0x24, size 0x1
};

#endif
