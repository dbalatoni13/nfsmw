#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/DialogInterface.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Misc/Gameflow.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

extern int bStrCmp(const char *, const char *);
extern int bStrLen(const char *);
extern void GetLocalizedString(char *buf, unsigned int maxlen, unsigned int hash);
extern int FEngMapJoyportToJoyParam(int);
extern void FEngSetCreateCallback(const char *, MenuScreen *(*)(ScreenConstructorData *));
extern FEObject *FEngGetCurrentButton(const char *);
extern FEObject *FEngFindObject(const char *, unsigned int);
extern void FEngSetInvisible(FEObject *);
extern FEImage *FEngFindImage(const char *, int);
extern void FEngSetTextureHash(FEImage *, unsigned int);
extern FEString *FEngFindString(const char *, int);
extern void FEngSetLanguageHash(const char *, unsigned int, unsigned int);
extern int FEPrintf(const char *, int, const char *, ...);
extern void FEngSetCurrentButton(const char *, unsigned int);
extern void FEngSetButtonState(const char *, unsigned int, bool);

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

extern void FormatMessage(char *buffer, int bufsize, const char *fmt, va_list *arglist);

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
                            const char *fmt, va_list *arg_list) {
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
    va_list arg_list;
    GetLocalizedString(fmt, 0x200, message_hash);
    va_start(arg_list, message_hash);
    int result = ShowOk(from_pkg, dlg_pkg, title, fmt, &arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                   unsigned int button_text_hash, unsigned int button_pressed_message,
                                   unsigned int cancel_message, bool dismissable,
                                   const char *fmt, va_list *arg_list) {
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
    va_list arg_list;
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowOneButton(from_pkg, dlg_pkg, title, button_text_hash, button_pressed_message,
                               cancel_message, false, fmt, &arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowOneButton(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                   unsigned int button_text_hash, unsigned int button_pressed_message,
                                   unsigned int blurb_fmt, ...) {
    char fmt[512];
    va_list arg_list;
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowOneButton(from_pkg, dlg_pkg, title, button_text_hash, button_pressed_message,
                               button_pressed_message, false, fmt, &arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    unsigned int cancel_message, bool dismissable,
                                    eDialogFirstButtons first_button,
                                    const char *fmt, va_list *arg_list) {
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
    va_list arg_list;
    va_start(arg_list, fmt);
    int result = ShowTwoButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                button1_pressed_message, button2_pressed_message,
                                cancel_message, false, first_button, fmt, &arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    unsigned int cancel_message, eDialogFirstButtons first_button,
                                    unsigned int blurb_fmt, ...) {
    char fmt[512];
    va_list arg_list;
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowTwoButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                button1_pressed_message, button2_pressed_message,
                                cancel_message, false, first_button, fmt, &arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowTwoButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                    unsigned int button1_text_hash, unsigned int button2_text_hash,
                                    unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                    eDialogFirstButtons first_button, unsigned int blurb_fmt, ...) {
    char fmt[512];
    va_list arg_list;
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowTwoButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                button1_pressed_message, button2_pressed_message,
                                button1_pressed_message, false, first_button, fmt, &arg_list);
    va_end(arg_list);
    return result;
}

int DialogInterface::ShowThreeButtons(const char *from_pkg, const char *dlg_pkg, eDialogTitle title,
                                      unsigned int button1_text_hash, unsigned int button2_text_hash,
                                      unsigned int button3_text_hash,
                                      unsigned int button1_pressed_message, unsigned int button2_pressed_message,
                                      unsigned int button3_pressed_message, unsigned int cancel_message,
                                      eDialogFirstButtons first_button,
                                      const char *fmt, va_list *arg_list) {
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
    va_list arg_list;
    GetLocalizedString(fmt, 0x200, blurb_fmt);
    va_start(arg_list, blurb_fmt);
    int result = ShowThreeButtons(from_pkg, dlg_pkg, title, button1_text_hash, button2_text_hash,
                                  button3_text_hash, button1_pressed_message, button2_pressed_message,
                                  button3_pressed_message, cancel_message, first_button, fmt, &arg_list);
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

void feDialogScreen::BuildFromConfig() {
    const unsigned long FEObj_messageblurb = 0x1e2640fa;

    if (Config.bBlurbIsUTF8) {
        FEString *pFEStr = FEngFindString(GetPackageName(), FEObj_messageblurb);
        pFEStr->SetStringFromUTF8(Config.BlurbString);
    } else {
        FEPrintf(GetPackageName(), FEObj_messageblurb, Config.BlurbString);
    }

    if (Config.NumButtons == 3) {
        FEngSetLanguageHash(GetPackageName(), 0xf9363f30, Config.Button1TextHash);
        FEngSetLanguageHash(GetPackageName(), 0xfb8b67d1, Config.Button2TextHash);
        FEngSetLanguageHash(GetPackageName(), 0xfde09072, Config.Button3TextHash);
    } else if (Config.NumButtons == 2) {
        FEngDisableButton(GetPackageName(), 0xb8a7c6ce);
        FEngSetInvisible(GetPackageName(), 0xb8a7c6ce);
        FEPrintf(GetPackageName(), 0xfde09072, "");
        FEngSetLanguageHash(GetPackageName(), 0xf9363f30, Config.Button1TextHash);
        FEngSetLanguageHash(GetPackageName(), 0xfb8b67d1, Config.Button2TextHash);
    } else if (Config.NumButtons == 1) {
        FEngDisableButton(GetPackageName(), 0xb8a7c6ce);
        FEngDisableButton(GetPackageName(), 0xb8a7c6cd);
        FEngSetInvisible(GetPackageName(), 0xb8a7c6ce);
        FEngSetInvisible(GetPackageName(), 0xb8a7c6cd);
        FEPrintf(GetPackageName(), 0xfde09072, "");
        FEPrintf(GetPackageName(), 0xfb8b67d1, "");
        FEngSetLanguageHash(GetPackageName(), 0xf9363f30, Config.Button1TextHash);
    } else {
        FEngDisableButton(GetPackageName(), 0xb8a7c6cc);
        FEngDisableButton(GetPackageName(), 0xb8a7c6cd);
        FEngDisableButton(GetPackageName(), 0xb8a7c6ce);
        FEngSetInvisible(GetPackageName(), 0xb8a7c6cc);
        FEngSetInvisible(GetPackageName(), 0xb8a7c6cd);
        FEngSetInvisible(GetPackageName(), 0xb8a7c6ce);
        FEngSetInvisible(GetPackageName(), 0x7f9dca9);
        FEPrintf(GetPackageName(), 0xf9363f30, "");
        FEPrintf(GetPackageName(), 0xfb8b67d1, "");
        FEPrintf(GetPackageName(), 0xfde09072, "");
    }

    FEngFont *font = FindFont(0x545570c6);
    float numLines = static_cast<float>(bStrLen(Config.BlurbString)) * font->GetHeight();

    const int MAX_SIZE_SMALL = 2200;
    const int MAX_SIZE_MED = 4400;
    const unsigned long FEObj_dialogsmall = 0x79b0c1c7;
    const unsigned long FEObj_dialogmedium = 0xa13adcaf;
    const unsigned long FEObj_dialoglarge = 0x792bc959;

    if (numLines < MAX_SIZE_SMALL) {
        cFEng::Get()->QueuePackageMessage(FEObj_dialogsmall, GetPackageName(), nullptr);
    } else if (numLines < MAX_SIZE_MED) {
        cFEng::Get()->QueuePackageMessage(FEObj_dialogmedium, GetPackageName(), nullptr);
    } else {
        cFEng::Get()->QueuePackageMessage(FEObj_dialoglarge, GetPackageName(), nullptr);
    }

    switch (Config.Title) {
    case dialog_alert:
    case dialog_fatalerror:
        if (TheGameFlowManager.IsInGame()) {
            FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0x2bd146d3);
        } else {
            FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0x6fd91524);
        }
        FEngSetTextureHash(GetPackageName(), 0xd4f4069, 0x6948e2b3);
        FEngSetTextureHash(GetPackageName(), 0xfac88427, 0x6948e2b3);
        break;
    case dialog_none:
    case dialog_info:
    case dialog_countdown:
        FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0xdbe419d4);
        FEngSetTextureHash(GetPackageName(), 0xd4f4069, 0x1a7afe27);
        FEngSetTextureHash(GetPackageName(), 0xfac88427, 0x1a7afe27);
        break;
    case dialog_confirmation:
        FEngSetLanguageHash(GetPackageName(), 0x42adb44c, 0x60249a74);
        FEngSetTextureHash(GetPackageName(), 0xd4f4069, 0x39949433);
        FEngSetTextureHash(GetPackageName(), 0xfac88427, 0x39949433);
        break;
    }
}

void feDialogScreen::NotificationMessage(unsigned long msg, FEObject *obj, unsigned long param1, unsigned long param2) {
    switch (msg) {
    case 0x35f8620b:
        if (Config.NumButtons != 0) {
            if (Config.FirstButton == 0) {
                FEngSetCurrentButton(GetPackageName(), 0xb8a7c6cc);
            } else if (Config.FirstButton == 1) {
                FEngSetCurrentButton(GetPackageName(), 0xb8a7c6cd);
            } else if (Config.FirstButton == 2) {
                FEngSetCurrentButton(GetPackageName(), 0xb8a7c6ce);
            }
            mLastButtonHash = Config.FirstButton;
        }
        break;

    case 0xc98356ba:
        if (Config.Title == dialog_countdown) {
            const unsigned long FEObj_messageblurb = 0x1e2640fa;
            int elapsed = static_cast<int>((RealTimer - tCountdownTimer).GetSeconds());
            FEPrintf(GetPackageName(), FEObj_messageblurb, Config.BlurbString, static_cast<int>(Config.fCountdown) - elapsed);
            if (static_cast<float>(elapsed) >= Config.fCountdown) {
                NotificationMessage(0x911ab364, nullptr, 0, 0);
            }
        }
        break;

    case 0x911ab364:
        if (Config.bIsDismissable) {
            ReturnWithMessage = Config.DialogCancelledMessage;
            DialogInterface::DismissDialog(Config.DialogHandle);
        }
        break;

    case 0x72619778:
        if (obj->NameHash == 0xb8a7c6cd && Config.NumButtons == 3) {
            FEngSetCurrentButton(GetPackageName(), 0xb8a7c6cc);
        } else if (obj->NameHash == 0xb8a7c6ce && Config.NumButtons != 1) {
            FEngSetCurrentButton(GetPackageName(), 0xb8a7c6cd);
        }
        break;

    case 0x911c0a4b:
        if (obj->NameHash == 0xb8a7c6cc) {
            FEngSetCurrentButton(GetPackageName(), 0xb8a7c6cd);
        } else if (obj->NameHash == 0xb8a7c6cd) {
            FEngSetCurrentButton(GetPackageName(), 0xb8a7c6ce);
        }
        break;

    case 0x1b91ebf4:
        ControllerPort = param1;
        break;

    case 0x0c407210:
        switch (obj->NameHash) {
        case 0xb8a7c6cc:
            if (Config.NumButtons != 0) {
                ReturnWithMessage = Config.Button1PressedMessage;
                DialogInterface::DismissDialog(Config.DialogHandle);
            }
            break;
        case 0xb8a7c6cd:
            if (Config.NumButtons != 0) {
                ReturnWithMessage = Config.Button2PressedMessage;
                DialogInterface::DismissDialog(Config.DialogHandle);
            }
            break;
        case 0xb8a7c6ce:
            if (Config.NumButtons != 0) {
                ReturnWithMessage = Config.Button3PressedMessage;
                DialogInterface::DismissDialog(Config.DialogHandle);
            }
            break;
        }
        break;
    }
}
