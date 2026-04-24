#ifndef _DIALOGINTERFACE
#define _DIALOGINTERFACE

#include <stdarg.h>

enum eDialogTitle {
    dialog_none = 0,
    dialog_alert = 1,
    dialog_info = 2,
    dialog_confirmation = 3,
    dialog_fatalerror = 4,
    dialog_countdown = 5,
};

enum eDialogFirstButtons {};

struct feDialogConfig {
    enum {
        DIALOG_BLURB_MAX_LENGTH = 512,
    };

    feDialogConfig();

    char BlurbString[512];                    // offset 0x0, size 0x200
    eDialogTitle Title;                       // offset 0x200, size 0x4
    unsigned int Button1TextHash;             // offset 0x204, size 0x4
    unsigned int Button1PressedMessage;       // offset 0x208, size 0x4
    unsigned int Button2TextHash;             // offset 0x20C, size 0x4
    unsigned int Button2PressedMessage;       // offset 0x210, size 0x4
    unsigned int Button3TextHash;             // offset 0x214, size 0x4
    unsigned int Button3PressedMessage;       // offset 0x218, size 0x4
    unsigned int DialogCancelledMessage;      // offset 0x21C, size 0x4
    unsigned int FirstButton;                 // offset 0x220, size 0x4
    const char *ParentPackage;                // offset 0x224, size 0x4
    const char *DialogPackage;                // offset 0x228, size 0x4
    int NumButtons;                           // offset 0x22C, size 0x4
    bool bIsDismissable;                      // offset 0x230, size 0x1
    bool bDetectController;                   // offset 0x234, size 0x1
    bool bBlurbIsUTF8;                        // offset 0x238, size 0x1
    unsigned int DismissedByOtherDialogMsg;   // offset 0x23C, size 0x4
    int DialogHandle;                         // offset 0x240, size 0x4
    float fCountdown;                         // offset 0x244, size 0x4
};

struct DialogInterface {
    static int ShowDialog(feDialogConfig *config);
    static void DismissDialog(int handle);

    static int ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                      const char *fmt, va_list *arg_list);
    static int ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                      unsigned int message_hash, ...);

    static int ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, bool dismissable,
                             const char *fmt, va_list *arg_list);
    static int ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, unsigned int blurb_fmt, ...);
    static int ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int blurb_fmt, ...);

    static int ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                              unsigned int cancel_message, bool dismissable,
                              eDialogFirstButtons first_button,
                              const char *fmt, va_list *arg_list);
    static int ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                              unsigned int cancel_message, eDialogFirstButtons first_button,
                              const char *fmt, ...);
    static int ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                              unsigned int cancel_message, eDialogFirstButtons first_button,
                              unsigned int blurb_fmt, ...);
    static int ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                              eDialogFirstButtons first_button, unsigned int blurb_fmt, ...);

    static int ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                unsigned int button1_text_hash, unsigned int button2_text_hash,
                                unsigned int button3_text_hash,
                                unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                unsigned int button3_pressed_message, unsigned int cancel_message,
                                eDialogFirstButtons first_button,
                                 const char *fmt, va_list *arg_list);
    static int ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                unsigned int button1_text_hash, unsigned int button2_text_hash,
                                unsigned int button3_text_hash,
                                unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                unsigned int button3_pressed_message, unsigned int cancel_message,
                                eDialogFirstButtons first_button,
                                unsigned int blurb_fmt, ...);
};

#endif
