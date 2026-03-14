#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Misc/Gameflow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

extern int bStrCmp(const char *, const char *);
extern void GetLocalizedString(char *buf, unsigned int maxlen, unsigned int hash);
extern int FEngMapJoyportToJoyParam(int);
extern void FEngSetCreateCallback(const char *, MenuScreen *(*)(ScreenConstructorData *));
extern FEObject *FEngGetCurrentButton(const char *);

extern Timer RealTimer;

struct feDialogScreen : MenuScreen {
    feDialogScreen(ScreenConstructorData *sd);
    ~feDialogScreen() override;
    void NotificationMessage(unsigned long, FEObject *, unsigned long, unsigned long) override;
    eMenuSoundTriggers NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) override;
    void BuildFromConfig();

    unsigned int ReturnWithMessage;  // offset 0x2C
    unsigned int mLastButtonHash;    // offset 0x30
    feDialogConfig Config;           // offset 0x34, size 0x248
    unsigned long ControllerPort;    // offset 0x27C
    Timer tCountdownTimer;           // offset 0x280
};

static feDialogConfig SecretDialogInfo;
static int gDialogHandle = 4;

MenuScreen *DialogCreater(ScreenConstructorData *sd) {
    return new ("", 0) feDialogScreen(sd);
}

feDialogConfig::feDialogConfig() {
    bMemSet(BlurbString, 0, DIALOG_BLURB_MAX_LENGTH);
    Title = dialog_info;
    DialogHandle = 0;
    fCountdown = 0.0f;
    Button1TextHash = 0;
    Button1PressedMessage = 0;
    Button2TextHash = 0;
    Button2PressedMessage = 0;
    Button3TextHash = 0;
    Button3PressedMessage = 0;
    DialogCancelledMessage = 0;
    FirstButton = 0;
    ParentPackage = nullptr;
    DialogPackage = nullptr;
    NumButtons = 0;
    bIsDismissable = false;
    bDetectController = false;
    bBlurbIsUTF8 = false;
}

extern void FormatMessage(char *buffer, int bufsize, const char *fmt, __va_list_tag *arglist);

void DialogInterface::DismissDialog(int handle) {
    if (SecretDialogInfo.DialogHandle == handle) {
        if (cFEng::Get()->IsPackageInControl(SecretDialogInfo.DialogPackage)) {
            if (SecretDialogInfo.Title == dialog_fatalerror) {
                cFEng::Get()->PopErrorPackage();
            } else {
                cFEng::Get()->QueuePackagePop(1);
            }
        }
        SecretDialogInfo.DialogHandle = 0;
        cFEng::Get()->QueuePackageMessage(0xd731d75e, nullptr, nullptr);
    }
}

int DialogInterface::ShowDialog(feDialogConfig *conf) {
    const char *user_passed = conf->DialogPackage;
    if (user_passed && *user_passed != '\0') {
        if (bStrCmp(user_passed, "animating") == 0) {
            conf->DialogPackage = "OL_Dialog.fng";
        } else if (bStrCmp(user_passed, "3button") == 0) {
            conf->DialogPackage = "Dialog.fng";
        } else if (bStrCmp(user_passed, "3buttons_stacked") == 0) {
            conf->DialogPackage = "OL_Dialog_Stacked_Buttons.fng";
        }
    } else {
        if (IsGameFlowInGame()) {
            conf->DialogPackage = "InGameDialog.fng";
        } else {
            conf->DialogPackage = "Dialog.fng";
        }
    }

    if (SecretDialogInfo.DialogHandle != 0) {
        DismissDialog(SecretDialogInfo.DialogHandle);
    }

    SecretDialogInfo = *conf;
    SecretDialogInfo.DialogHandle = gDialogHandle;
    gDialogHandle++;

    FEngSetCreateCallback(SecretDialogInfo.DialogPackage, DialogCreater);

    if (SecretDialogInfo.Title == dialog_fatalerror) {
        int port = FEDatabase->GetPlayersJoystickPort(0);
        cFEng::Get()->PushErrorPackage(SecretDialogInfo.DialogPackage,
                                        reinterpret_cast<int>(&SecretDialogInfo),
                                        FEngMapJoyportToJoyParam(port));
    } else {
        cFEng::Get()->QueuePackagePush(SecretDialogInfo.DialogPackage,
                                        reinterpret_cast<int>(&SecretDialogInfo), 0, false);
    }
    return SecretDialogInfo.DialogHandle;
}

int DialogInterface::ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                            const char *fmt, __va_list_tag *arg_list) {
    feDialogConfig conf;
    FormatMessage(conf.BlurbString, 0x200, fmt, arg_list);
    conf.Button1TextHash = 0x417b2601;
    conf.Button1PressedMessage = 0x34dc1bec;
    conf.DialogCancelledMessage = 0xb4edeb6d;
    conf.FirstButton = 0;
    conf.bIsDismissable = true;
    conf.NumButtons = 1;
    conf.Title = title;
    conf.ParentPackage = from_pkg;
    conf.DialogPackage = dlg_pkg;
    return ShowDialog(&conf);
}

int DialogInterface::ShowOk(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                            unsigned int message_hash, ...) {
    char fmt[512];
    __va_list_tag arg_list[1];
    GetLocalizedString(fmt, 0x200, message_hash);
    va_start(arg_list, message_hash);
    int result = ShowOk(from_pkg, dlg_pkg, title, fmt, arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                   unsigned int button_text_hash, unsigned int button_pressed_message,
                                   unsigned int cancel_message, bool dismissable,
                                   const char *fmt, __va_list_tag *arg_list) {
    feDialogConfig conf;
    FormatMessage(conf.BlurbString, 0x200, fmt, arg_list);
    conf.NumButtons = 1;
    conf.FirstButton = 0;
    conf.Title = title;
    conf.Button1TextHash = button_text_hash;
    conf.Button1PressedMessage = button_pressed_message;
    conf.DialogCancelledMessage = cancel_message;
    conf.ParentPackage = from_pkg;
    conf.DialogPackage = dlg_pkg;
    conf.bIsDismissable = dismissable;
    return ShowDialog(&conf);
}

int DialogInterface::ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                   unsigned int button_text_hash, unsigned int button_pressed_message,
                                   unsigned int cancel_message, unsigned int blurb_fmt, ...) {
    char fmt[512];
    __va_list_tag arg_list[1];
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowOneButton(from_pkg, dlg_pkg, title, button_text_hash, button_pressed_message,
                               cancel_message, false, fmt, arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                   unsigned int button_text_hash, unsigned int button_pressed_message,
                                   unsigned int blurb_fmt, ...) {
    char fmt[512];
    __va_list_tag arg_list[1];
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowOneButton(from_pkg, dlg_pkg, title, button_text_hash, button_pressed_message,
                               button_pressed_message, false, fmt, arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    unsigned int cancel_message, bool dismissable,
                                    eDialogFirstButtons first_button,
                                    const char *fmt, __va_list_tag *arg_list) {
    feDialogConfig conf;
    FormatMessage(conf.BlurbString, 0x200, fmt, arg_list);
    conf.NumButtons = 2;
    conf.Title = title;
    conf.Button1TextHash = button1_text_hash;
    conf.Button1PressedMessage = button1_pressed_message;
    conf.Button2TextHash = button2_text_hash;
    conf.Button2PressedMessage = button2_pressed_message;
    conf.DialogCancelledMessage = cancel_message;
    conf.FirstButton = first_button;
    conf.DialogPackage = dlg_pkg;
    conf.ParentPackage = from_pkg;
    conf.bIsDismissable = dismissable;
    if (dismissable) {
        FEPackage *pkg = cFEng::Get()->FindPackage(from_pkg);
        if (pkg) {
            unsigned long mask = pkg->GetControlMask();
            if (mask != 0xff) {
                conf.bDetectController = true;
            }
        }
    }
    return ShowDialog(&conf);
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    unsigned int cancel_message, eDialogFirstButtons first_button,
                                    const char *fmt, ...) {
    __va_list_tag arg_list[1];
    va_start(arg_list, fmt);
    int result = ShowTwoButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                button1_pressed_message, button2_pressed_message,
                                cancel_message, false, first_button, fmt, arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    unsigned int cancel_message, eDialogFirstButtons first_button,
                                    unsigned int blurb_fmt, ...) {
    char fmt[512];
    __va_list_tag arg_list[1];
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowTwoButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                button1_pressed_message, button2_pressed_message,
                                cancel_message, false, first_button, fmt, arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    eDialogFirstButtons first_button, unsigned int blurb_fmt, ...) {
    char fmt[512];
    __va_list_tag arg_list[1];
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowTwoButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                button1_pressed_message, button2_pressed_message,
                                button1_pressed_message, false, first_button, fmt, arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                      unsigned int button1_text_hash, unsigned int button2_text_hash,
                                      unsigned int button3_text_hash,
                                      unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                      unsigned int button3_pressed_message, unsigned int cancel_message,
                                      eDialogFirstButtons first_button,
                                      const char *fmt, __va_list_tag *arg_list) {
    feDialogConfig conf;
    FormatMessage(conf.BlurbString, 0x200, fmt, arg_list);
    conf.NumButtons = 3;
    conf.Title = title;
    conf.Button1TextHash = button1_text_hash;
    conf.Button2TextHash = button2_text_hash;
    conf.Button3TextHash = button3_text_hash;
    conf.Button1PressedMessage = button1_pressed_message;
    conf.Button2PressedMessage = button2_pressed_message;
    conf.Button3PressedMessage = button3_pressed_message;
    conf.DialogCancelledMessage = cancel_message;
    conf.FirstButton = first_button;
    conf.ParentPackage = from_pkg;
    conf.DialogPackage = dlg_pkg;
    return ShowDialog(&conf);
}

int DialogInterface::ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                      unsigned int button1_text_hash, unsigned int button2_text_hash,
                                      unsigned int button3_text_hash,
                                      unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                      unsigned int button3_pressed_message, unsigned int cancel_message,
                                      eDialogFirstButtons first_button,
                                      unsigned int blurb_fmt, ...) {
    char fmt[512];
    __va_list_tag arg_list[1];
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowThreeButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                  button3_text_hash, button1_pressed_message, button2_pressed_message,
                                  button3_pressed_message, cancel_message, first_button, fmt, arg_list);
    va_end(arg_list);
    return result;
}

feDialogScreen::feDialogScreen(ScreenConstructorData *sd)
    : MenuScreen(sd)
{
    ControllerPort = 0xff;
    ReturnWithMessage = 0;
    tCountdownTimer = Timer(0);
    Config = *reinterpret_cast<feDialogConfig *>(sd->Arg);
    BuildFromConfig();
    tCountdownTimer = RealTimer;
}

feDialogScreen::~feDialogScreen() {
    if (ReturnWithMessage != static_cast<unsigned int>(-1)) {
        cFEng::Get()->QueueGameMessage(ReturnWithMessage, Config.ParentPackage, ControllerPort);
    }
}

eMenuSoundTriggers feDialogScreen::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if (msg != 0x48122792) {
        if (msg < 0x48122793) {
            if (msg != 0x480c9a58) {
                return maybe;
            }
            if (!Config.bIsDismissable) {
                return static_cast<eMenuSoundTriggers>(-1);
            }
            return maybe;
        }
        if (msg != 0x4ac5e165) {
            return maybe;
        }
    }
    FEObject *btn = FEngGetCurrentButton(GetPackageName());
    if (btn && FEngGetCurrentButton(GetPackageName())->NameHash != mLastButtonHash) {
        mLastButtonHash = FEngGetCurrentButton(GetPackageName())->NameHash;
        return maybe;
    }
    return static_cast<eMenuSoundTriggers>(-1);
}