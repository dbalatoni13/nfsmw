#ifndef _DIALOGINTERFACE
#define _DIALOGINTERFACE

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include <types.h>
#include <stdarg.h>

enum eDialogTitle {
    dialog_none = 0,
    dialog_alert = 1,
    dialog_info = 2,
    dialog_confirmation = 3,
    dialog_fatalerror = 4,
    dialog_countdown = 5,
};

typedef enum {
    dialog_message_ok = 886840300,
    dialog_message_yes = -799030365,
    dialog_message_no = 886840271,
    dialog_message_cancel = 531323288,
    dialog_message_cancelled = -1259476115,
    DIALOG_MESSAGE_NOTHING = -1
} eUsefulDialogMessages;

typedef enum { text_hash_ok = 1098589697, text_hash_cancel = 438914477 } eUsefulDialogTextHashes;

typedef enum { first_dialog_button1 = 0, first_dialog_button2 = 1, first_dialog_button3 = 2 } eDialogFirstButtons;

typedef int dialog_handle;

class feDialogConfig {
  public:
    enum {
        DIALOG_BLURB_MAX_LENGTH = 512,
    };

    feDialogConfig();

    char BlurbString[512];            // offset 0x0, size 0x200
    eDialogTitle Title;               // offset 0x200, size 0x4
    uint32 Button1TextHash;           // offset 0x204, size 0x4
    uint32 Button1PressedMessage;     // offset 0x208, size 0x4
    uint32 Button2TextHash;           // offset 0x20C, size 0x4
    uint32 Button2PressedMessage;     // offset 0x210, size 0x4
    uint32 Button3TextHash;           // offset 0x214, size 0x4
    uint32 Button3PressedMessage;     // offset 0x218, size 0x4
    uint32 DialogCancelledMessage;    // offset 0x21C, size 0x4
    uint32 FirstButton;               // offset 0x220, size 0x4
    const char *ParentPackage;        // offset 0x224, size 0x4
    const char *DialogPackage;        // offset 0x228, size 0x4
    int NumButtons;                   // offset 0x22C, size 0x4
    bool bIsDismissable;              // offset 0x230, size 0x1
    bool bDetectController;           // offset 0x234, size 0x1
    bool bBlurbIsUTF8;                // offset 0x238, size 0x1
    uint32 DismissedByOtherDialogMsg; // offset 0x23C, size 0x4
    dialog_handle DialogHandle;       // offset 0x240, size 0x4
    float fCountdown;                 // offset 0x244, size 0x4
};

// TODO
class DialogInterface {
  private:
    static dialog_handle ShowOkCancel(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, const char *fmt, va_list *arg_list);
    static dialog_handle ShowMessage(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, const char *fmt, va_list *arg_list);
    static dialog_handle ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, const char *fmt, va_list *arg_list);
    static dialog_handle ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button_text_hash,
                                       uint32 button_pressed_message, uint32 cancel_message, bool dismissable, const char *fmt, va_list *arg_list);

    static dialog_handle ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                        uint32 button2_text_hash, uint32 button1_pressed_message, uint32 button2_pressed_message,
                                        uint32 cancel_message, bool dismissable, eDialogFirstButtons first_button, const char *fmt,
                                        va_list *arg_list);
    static dialog_handle ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                          uint32 button2_text_hash, uint32 button3_text_hash, uint32 button1_pressed_message,
                                          uint32 button2_pressed_message, uint32 button3_pressed_message, uint32 cancel_message,
                                          eDialogFirstButtons first_button, const char *fmt, va_list *arg_list);

  public:
    static dialog_handle ShowDialog(struct feDialogConfig *conf);
    static dialog_handle ShowMessage(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, const char *msg_fmt);
    static dialog_handle ShowMessage(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 message_hash);
    static dialog_handle ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, const char *fmt, ...);
    static dialog_handle ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 message_hash, ...);
    static dialog_handle ShowOkCancel(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, const char *fmt);
    static dialog_handle ShowOkCancel(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 message_hash);
    static dialog_handle ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button_text_hash,
                                       uint32 button_pressed_message, uint32 cancel_message, const char *fmt, ...);
    static dialog_handle ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button_text_hash,
                                       uint32 button_pressed_message, uint32 cancel_message, uint32 blurb_fmt, ...);
    static dialog_handle ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button_text_hash,
                                       uint32 button_pressed_message, const char *fmt, ...);
    static dialog_handle ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button_text_hash,
                                       uint32 button_pressed_message, uint32 blurb_fmt, ...);
    static dialog_handle ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                        uint32 button2_text_hash, uint32 button1_pressed_message, uint32 button2_pressed_message,
                                        uint32 cancel_message, eDialogFirstButtons first_button, const char *fmt, ...);
    static dialog_handle ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                        uint32 button2_text_hash, uint32 button1_pressed_message, uint32 button2_pressed_message,
                                        uint32 cancel_message, eDialogFirstButtons first_button, uint32 blurb_fmt, ...);
    static dialog_handle ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                        uint32 button2_text_hash, uint32 button1_pressed_message, uint32 button2_pressed_message,
                                        eDialogFirstButtons first_button, const char *fmt, ...);
    static dialog_handle ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                        uint32 button2_text_hash, uint32 button1_pressed_message, uint32 button2_pressed_message,
                                        eDialogFirstButtons first_button, uint32 blurb_fm, ...);
    static dialog_handle ShowTwoButtonCountdown(const char *from_pkg, const char *dlg_pkg, uint32 button1_text_hash, uint32 button2_text_hash,
                                                uint32 button1_pressed_message, uint32 button2_pressed_message, uint32 cancel_message,
                                                eDialogFirstButtons first_button, float timer, uint32 blurb_fmt, ...);
    static dialog_handle ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                          uint32 button2_text_hash, uint32 button3_text_hash, uint32 button1_pressed_message,
                                          uint32 button2_pressed_message, uint32 button3_pressed_message, uint32 cancel_message,
                                          eDialogFirstButtons first_button, const char *fmt, ...);
    static dialog_handle ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title, uint32 button1_text_hash,
                                          uint32 button2_text_hash, uint32 button3_text_hash, uint32 button1_pressed_message,
                                          uint32 button2_pressed_message, uint32 button3_pressed_message, uint32 cancel_message,
                                          eDialogFirstButtons first_button, uint32 blurb_fmt, ...);
    static void DismissDialog(int handle);
    static struct feDialogScreen *FindDialogBox(int handle);
    static bool SetButtonText(int ButtonNum, const char *pText, bool bTextIsUTF8);
    static void SetBlurbIsUTF8();
    static void SetDismissedByAnotherDialogMsg(uint32 Msg);
    static void SetThereIsADisconnectDialogUp(/* parameters unknown */);
    static bool PackageNameIsUsedAsDialog(/* parameters unknown */);
};

class feDialogScreen : public MenuScreen {
  public:
    feDialogScreen(ScreenConstructorData *sd);
    ~feDialogScreen() override;
    void NotificationMessage(u32, FEObject *, u32, u32) override;
    eMenuSoundTriggers NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) override;

  private:
    void BuildFromConfig();

    uint32 ReturnWithMessage; // offset 0x2C
    uint32 mLastButtonHash;   // offset 0x30
    feDialogConfig Config;    // offset 0x34, size 0x248
    u32 ControllerPort;       // offset 0x27C
    Timer tCountdownTimer;    // offset 0x280
};

#endif
