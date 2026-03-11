#ifndef _DIALOGINTERFACE
#define _DIALOGINTERFACE

enum eDialogTitle {};
enum eDialogFirstButtons {};

struct DialogInterface {
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, ...);
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, const char* fmt, ...);
    static int ShowTwoButtons(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                              unsigned int button1_text_hash, unsigned int button2_text_hash,
                              unsigned int button1_pressed_message,
                              unsigned int button2_pressed_message, unsigned int cancel_message,
                              eDialogFirstButtons first_button, unsigned int lang_hash, ...);
    static int ShowOneButton(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, ...);
    static int ShowOneButton(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, const char* fmt, ...);
    static int ShowOneButton(const char* from_pkg, const char* dlg_pkg, eDialogTitle title,
                             unsigned int button_text_hash, unsigned int button_pressed_message,
                             unsigned int cancel_message, unsigned int lang_hash, ...);
};

#endif
