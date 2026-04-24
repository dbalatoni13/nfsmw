#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEMenuScreen.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/FEImage.h"
#include "Speed/Indep/Src/FEng/FEString.h"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"

extern KeyboardEditString gKeyboardManager;
extern MenuScreen *g_pOLCurrentScreen;

extern void FESoundControl(bool, const char *);
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void FEngSetButtonTexture(FEImage *img, unsigned int tex_hash);
extern int bStrCmp(const char *s1, const char *s2);
extern char *bStrNCpy(char *to, const char *from, int max_count);
extern void bMemSet(void *dst, int value, unsigned int size);

extern const char lbl_803E59BC[];
extern const char lbl_803E5CB8[];
extern const char lbl_803E5EEC[];
extern const char lbl_803E6D54[];
extern const char lbl_803E6D6C[];
extern const char lbl_803E6D8C[];
extern const char lbl_803E7FC4[];
extern const char lbl_803E85A8[];
extern const char lbl_803E85C4[];
extern const char lbl_803E85E0[];
extern const char lbl_803E8600[];

MenuScreen::MenuScreen(ScreenConstructorData *sd)
    : mPlaySound(true) //
    , mDirectionForNextSound(0) //
    , bEnableEAMessenger(false) //
    , PackageFilename(sd->PackageFilename) //
    , ConstructData(*sd) //
    , IsGarageScreen(false) //
    , TextInputObject(nullptr) //
    , mStartCapturingFromKeyboard(0) {
    FESoundControl(true, PackageFilename);
    FEngSetButtonTexture(FEngFindImage(PackageFilename, 0x6B364F8B), 0x5BC);
    FEngSetButtonTexture(FEngFindImage(PackageFilename, 0x79354351), 0x682);

    if (bStrCmp(PackageFilename, lbl_803E5EEC) == 0 || bStrCmp(PackageFilename, lbl_803E6D54) == 0 ||
        bStrCmp(PackageFilename, lbl_803E7FC4) == 0 || bStrCmp(PackageFilename, lbl_803E6D6C) == 0 ||
        bStrCmp(PackageFilename, lbl_803E59BC) == 0 || bStrCmp(PackageFilename, lbl_803E6D8C) == 0 ||
        bStrCmp(PackageFilename, lbl_803E85A8) == 0 || bStrCmp(PackageFilename, lbl_803E85C4) == 0 ||
        bStrCmp(PackageFilename, lbl_803E85E0) == 0) {
    } else if (!cFEng::Get()->IsPackagePushed(lbl_803E8600)) {
        g_pOLCurrentScreen = this;
    }
}

MenuScreen::~MenuScreen() {
    FESoundControl(false, PackageFilename);

    if (bStrCmp(PackageFilename, lbl_803E5EEC) == 0 || bStrCmp(PackageFilename, lbl_803E6D54) == 0 ||
        bStrCmp(PackageFilename, lbl_803E7FC4) == 0 || bStrCmp(PackageFilename, lbl_803E6D6C) == 0 ||
        bStrCmp(PackageFilename, lbl_803E59BC) == 0 || bStrCmp(PackageFilename, lbl_803E6D8C) == 0 ||
        bStrCmp(PackageFilename, lbl_803E85A8) == 0 || bStrCmp(PackageFilename, lbl_803E85C4) == 0 ||
        bStrCmp(PackageFilename, lbl_803E85E0) == 0) {
    } else if (!cFEng::Get()->IsPackagePushed(lbl_803E8600)) {
        g_pOLCurrentScreen = nullptr;
    }
}

void MenuScreen::BaseNotify(u32 Message, FEObject *pObject, u32 Param1, u32 Param2) {
    if (!CheckKeyboard(Message)) {
        if (Message != 0x9803F6E2 || ConstructData.pPackage->Controllers != 0) {
            NotificationMessage(Message, pObject, Param1, Param2);
        }
    }
}

const char *MenuScreen::FEngGetEditedString() {
    return FEDatabase->mFEKeyboardSettings.Buffer;
}

void MenuScreen::FEngEndTextInput() {
    if (TextInputObject != nullptr) {
        delete TextInputObject;
    }

    mStartCapturingFromKeyboard = 0;
    TextInputObject = nullptr;
}

void MenuScreen::FEngBeginTextInput(uint32 /* object_hash */, uint32 edit_mode,
                                    const char *initial_string, const char *title_string,
                                    uint32 max_text_length) {
    FEKeyboardSettings &settings = FEDatabase->mFEKeyboardSettings;

    bStrNCpy(settings.Buffer, initial_string, sizeof(settings.Buffer));
    settings.AcceptCallbackHash = 0xDA5B8712;
    settings.MaxTextLength = max_text_length;
    settings.DeclineCallbackHash = 0xC9D30688;
    settings.DefaultTextHash = 0;
    bMemSet(settings.Title, 0, sizeof(settings.Title));
    bStrNCpy(settings.Title, title_string, sizeof(settings.Title) - 1);

    settings.Mode = 1;
    switch (edit_mode) {
    case 0:
        settings.Mode = 0;
        break;
    case 1:
    case 3:
        settings.Mode = 1;
        break;
    case 2:
        settings.Mode = 5;
        break;
    case 4:
        settings.Mode = 4;
        break;
    case 5:
        settings.Mode = 2;
        break;
    case 6:
        settings.Mode = 3;
        break;
    }

    cFEng::Get()->QueuePackagePush(lbl_803E5CB8, 0, 0, false);
}

bool MenuScreen::CheckKeyboard(uint32 msg) {
    if (msg == 0xC98356BA) {
        if (TextInputObject == nullptr) {
            return false;
        }

        if (mStartCapturingFromKeyboard == 1) {
            mStartCapturingFromKeyboard = 2;
            return true;
        }

        if (mStartCapturingFromKeyboard == 2) {
            mStartCapturingFromKeyboard = 3;
            gKeyboardManager.KeysProcessed = 1;
        }
    }

    if (TextInputObject == nullptr) {
        return false;
    }

    if (gKeyboardManager.KeysProcessed != 0 && gKeyboardManager.MaxTextLength != 0) {
        if (msg == 0x911AB364) {
            TextInputObject->EscapePressed();
        } else if (msg == 0x406415E3) {
            TextInputObject->ReturnPressed();
        } else {
            TextInputObject->Notify(msg);
        }
    }

    return true;
}

void MenuScreen::BaseNotifySound(u32 msg, FEObject * /* obj */, u32 /* controller_mask */, u32 /* pkg_ptr */) {
    eMenuSoundTriggers soundToPlay = UISND_NONE;

    switch (msg) {
    case 0x00B06E92: soundToPlay = static_cast< eMenuSoundTriggers >(0x8c); break;
    case 0x01CD9276: soundToPlay = static_cast< eMenuSoundTriggers >(0x72); break;
    case 0x0217C99F: soundToPlay = static_cast< eMenuSoundTriggers >(0x92); break;
    case 0x08464444: soundToPlay = static_cast< eMenuSoundTriggers >(0x72); break;
    case 0x0A888DDC: soundToPlay = static_cast< eMenuSoundTriggers >(0x6B); break;
    case 0x0BDDC0D2: soundToPlay = static_cast< eMenuSoundTriggers >(0x8e); break;
    case 0x145B13C0: soundToPlay = static_cast< eMenuSoundTriggers >(0x58); break;
    case 0x14C293B4: soundToPlay = static_cast< eMenuSoundTriggers >(0x83); break;
    case 0x14C85C88: soundToPlay = static_cast< eMenuSoundTriggers >(0x7e); break;
    case 0x1ABBE49B: soundToPlay = static_cast< eMenuSoundTriggers >(0x67); break;
    case 0x1B325F9D: soundToPlay = static_cast< eMenuSoundTriggers >(0x8a); break;
    case 0x1CA2354C: soundToPlay = static_cast< eMenuSoundTriggers >(0x94); break;
    case 0x1CFAD52B: soundToPlay = static_cast< eMenuSoundTriggers >(0x14); break;
    case 0x1E9AE519: soundToPlay = static_cast< eMenuSoundTriggers >(0x2a); break;
    case 0x299148C8: soundToPlay = static_cast< eMenuSoundTriggers >(0x57); break;
    case 0x2AC2CD24: soundToPlay = static_cast< eMenuSoundTriggers >(0x2f); break;
    case 0x2DD4FE18: soundToPlay = static_cast< eMenuSoundTriggers >(0x26); break;
    case 0x2E21E843: soundToPlay = static_cast< eMenuSoundTriggers >(0x36); break;
    case 0x2E287C68: soundToPlay = static_cast< eMenuSoundTriggers >(0x33); break;
    case 0x2E70DC92: soundToPlay = static_cast< eMenuSoundTriggers >(0x7f); break;
    case 0x3159062B: soundToPlay = static_cast< eMenuSoundTriggers >(0x28); break;
    case 0x334F3B79: soundToPlay = static_cast< eMenuSoundTriggers >(0x1b); break;
    case 0x334F3B7A: soundToPlay = static_cast< eMenuSoundTriggers >(0x1C); break;
    case 0x3A62513E: soundToPlay = static_cast< eMenuSoundTriggers >(0x6F); break;
    case 0x3B7FAC4C: soundToPlay = static_cast< eMenuSoundTriggers >(0x70); break;
    case 0x3B7FF2AB: soundToPlay = static_cast< eMenuSoundTriggers >(0x75); break;
    case 0x40927E58: soundToPlay = static_cast< eMenuSoundTriggers >(0x74); break;
    case 0x43BE891A: soundToPlay = static_cast< eMenuSoundTriggers >(0x1A); break;
    case 0x44D43738: soundToPlay = static_cast< eMenuSoundTriggers >(0x91); break;
    case 0x4688F23F: soundToPlay = static_cast< eMenuSoundTriggers >(0x73); break;
    case 0x46BB84C6: soundToPlay = static_cast< eMenuSoundTriggers >(0x29); break;
    case 0x478FFB47: soundToPlay = static_cast< eMenuSoundTriggers >(0x59); break;
    case 0x480C9A58: soundToPlay = static_cast< eMenuSoundTriggers >(5); break;
    case 0x480DF13F: soundToPlay = static_cast< eMenuSoundTriggers >(0x67); break;
    case 0x4810A91B: soundToPlay = static_cast< eMenuSoundTriggers >(0x1C); break;
    case 0x48ED7349: soundToPlay = static_cast< eMenuSoundTriggers >(0x90); break;
    case 0x4949060A: soundToPlay = static_cast< eMenuSoundTriggers >(0x6A); break;
    case 0x4A7ACC5A: soundToPlay = static_cast< eMenuSoundTriggers >(5); break;
    case 0x4A7E0941: soundToPlay = static_cast< eMenuSoundTriggers >(0x85); break;
    case 0x4A805994: soundToPlay = static_cast< eMenuSoundTriggers >(2); break;
    case 0x4B680587: soundToPlay = static_cast< eMenuSoundTriggers >(0x9b); break;
    case 0x4BEAF019: soundToPlay = static_cast< eMenuSoundTriggers >(0x32); break;
    case 0x4E1DA7E5: soundToPlay = static_cast< eMenuSoundTriggers >(0x55); break;
    case 0x4E7CCC80: soundToPlay = static_cast< eMenuSoundTriggers >(0x12); break;
    case 0x4F072ECA: soundToPlay = static_cast< eMenuSoundTriggers >(0x9d); break;
    case 0x4FB99CCD: soundToPlay = static_cast< eMenuSoundTriggers >(0x1A); break;
    case 0x4FB99CCE: soundToPlay = static_cast< eMenuSoundTriggers >(0x29); break;
    case 0x4FBFB02B: soundToPlay = static_cast< eMenuSoundTriggers >(0x4A); break;
    case 0x50B3FA79: soundToPlay = static_cast< eMenuSoundTriggers >(0x87); break;
    case 0x52F29A56: soundToPlay = static_cast< eMenuSoundTriggers >(0x76); break;
    case 0x52F69B81: soundToPlay = static_cast< eMenuSoundTriggers >(7); break;
    case 0x54EBD897: soundToPlay = static_cast< eMenuSoundTriggers >(0x7b); break;
    case 0x56D0CB98: soundToPlay = static_cast< eMenuSoundTriggers >(0x27); break;
    case 0x59E562D8: soundToPlay = static_cast< eMenuSoundTriggers >(0x6D); break;
    case 0x5A0FDF7C: soundToPlay = static_cast< eMenuSoundTriggers >(0x18); break;
    case 0x5A0FDF7D: soundToPlay = static_cast< eMenuSoundTriggers >(0x19); break;
    case 0x5C928AC2: soundToPlay = static_cast< eMenuSoundTriggers >(0xe); break;
    case 0x63E5ED6F: soundToPlay = static_cast< eMenuSoundTriggers >(0x96); break;
    case 0x65A03426: soundToPlay = static_cast< eMenuSoundTriggers >(0x11); break;
    case 0x672DFAFD: soundToPlay = static_cast< eMenuSoundTriggers >(0x86); break;
    case 0x6A95F040: soundToPlay = static_cast< eMenuSoundTriggers >(0xf); break;
    case 0x6A9AA0EA: soundToPlay = static_cast< eMenuSoundTriggers >(0x10); break;
    case 0x6B283007: soundToPlay = static_cast< eMenuSoundTriggers >(0xA); break;
    case 0x6BC1CD5D: soundToPlay = static_cast< eMenuSoundTriggers >(0xA); break;
    case 0x6E437A7B: soundToPlay = static_cast< eMenuSoundTriggers >(0x9f); break;
    case 0x7238782D: soundToPlay = static_cast< eMenuSoundTriggers >(0x68); break;
    case 0x746D7CDB: soundToPlay = static_cast< eMenuSoundTriggers >(0x98); break;
    case 0x7B6B89D7: soundToPlay = static_cast< eMenuSoundTriggers >(4); break;
    case 0x7C002DCA: soundToPlay = static_cast< eMenuSoundTriggers >(0x99); break;
    case 0x7D35DA25: soundToPlay = static_cast< eMenuSoundTriggers >(0xC); break;
    case 0x7F63059B: soundToPlay = static_cast< eMenuSoundTriggers >(0x88); break;
    case 0x80F21152: soundToPlay = static_cast< eMenuSoundTriggers >(0x9c); break;
    case 0x81DA4B22: soundToPlay = static_cast< eMenuSoundTriggers >(0x6C); break;
    case 0x84D07AF9: soundToPlay = static_cast< eMenuSoundTriggers >(0x73); break;
    case 0x8C40ED95: soundToPlay = static_cast< eMenuSoundTriggers >(0x59); break;
    case 0x8E2658C6: soundToPlay = static_cast< eMenuSoundTriggers >(0x70); break;
    case 0x8E269F25: soundToPlay = static_cast< eMenuSoundTriggers >(0x75); break;
    case 0x90429475: soundToPlay = static_cast< eMenuSoundTriggers >(0x2e); break;
    case 0x905FB857: soundToPlay = static_cast< eMenuSoundTriggers >(0xC); break;
    case 0x91970159: soundToPlay = static_cast< eMenuSoundTriggers >(0x15); break;
    case 0x958E0BD3: soundToPlay = static_cast< eMenuSoundTriggers >(0x97); break;
    case 0x97B397B0: soundToPlay = static_cast< eMenuSoundTriggers >(0x72); break;
    case 0x997DD91D: soundToPlay = static_cast< eMenuSoundTriggers >(0x9e); break;
    case 0x9AFA53A7: soundToPlay = static_cast< eMenuSoundTriggers >(3); break;
    case 0x9B59E056: soundToPlay = static_cast< eMenuSoundTriggers >(7); break;
    case 0x9F032957: soundToPlay = static_cast< eMenuSoundTriggers >(0x89); break;
    case 0xA07F7EA9: soundToPlay = static_cast< eMenuSoundTriggers >(0x84); break;
    case 0xA3CC3462: soundToPlay = static_cast< eMenuSoundTriggers >(0x6B); break;
    case 0xA6362B4B: soundToPlay = static_cast< eMenuSoundTriggers >(0x8b); break;
    case 0xAB7A5FC7: soundToPlay = static_cast< eMenuSoundTriggers >(0x69); break;
    case 0xAC57BDB2: soundToPlay = static_cast< eMenuSoundTriggers >(0x13); break;
    case 0xAD0FBB98: soundToPlay = static_cast< eMenuSoundTriggers >(0x82); break;
    case 0xADCE9EEC: soundToPlay = static_cast< eMenuSoundTriggers >(0x7d); break;
    case 0xB11FB113: soundToPlay = static_cast< eMenuSoundTriggers >(0x93); break;
    case 0xB1BF9795: soundToPlay = static_cast< eMenuSoundTriggers >(0x67); break;
    case 0xB205316C: soundToPlay = static_cast< eMenuSoundTriggers >(0x67); break;
    case 0xB25C8563: soundToPlay = static_cast< eMenuSoundTriggers >(0x56); break;
    case 0xB4FF8CD3: soundToPlay = static_cast< eMenuSoundTriggers >(0x7c); break;
    case 0xB6471CC6: soundToPlay = static_cast< eMenuSoundTriggers >(0x5a); break;
    case 0xB86367CB: soundToPlay = static_cast< eMenuSoundTriggers >(0x9a); break;
    case 0xB8AFE646: soundToPlay = static_cast< eMenuSoundTriggers >(0x35); break;
    case 0xBA0159B3: soundToPlay = static_cast< eMenuSoundTriggers >(0x81); break;
    case 0xC45AF53B: soundToPlay = static_cast< eMenuSoundTriggers >(0x19); break;
    case 0xC477473A: soundToPlay = static_cast< eMenuSoundTriggers >(0x16); break;
    case 0xC4A06D0F: soundToPlay = static_cast< eMenuSoundTriggers >(0x59); break;
    case 0xC989909F: soundToPlay = static_cast< eMenuSoundTriggers >(0xA); break;
    case 0xCAF4D21E: soundToPlay = static_cast< eMenuSoundTriggers >(0x74); break;
    case 0xD15F7512: soundToPlay = static_cast< eMenuSoundTriggers >(0x6D); break;
    case 0xD792FF31: soundToPlay = static_cast< eMenuSoundTriggers >(0x6E); break;
    case 0xD93BCF6E: soundToPlay = static_cast< eMenuSoundTriggers >(0x8d); break;
    case 0xDC49A694: soundToPlay = static_cast< eMenuSoundTriggers >(6); break;
    case 0xDDD9C41E: soundToPlay = static_cast< eMenuSoundTriggers >(0x31); break;
    case 0xDEDF0B4C: soundToPlay = static_cast< eMenuSoundTriggers >(0x4B); break;
    case 0xE28CAC90: soundToPlay = static_cast< eMenuSoundTriggers >(0x6A); break;
    case 0xE2A0904F: soundToPlay = static_cast< eMenuSoundTriggers >(0x71); break;
    case 0xE7DAFAEE: soundToPlay = static_cast< eMenuSoundTriggers >(0); break;
    case 0xE9B28A67: soundToPlay = static_cast< eMenuSoundTriggers >(0x68); break;
    case 0xEA2747C5: soundToPlay = static_cast< eMenuSoundTriggers >(0x95); break;
    case 0xED78878C: soundToPlay = static_cast< eMenuSoundTriggers >(0x34); break;
    case 0xEE0EEA7E: soundToPlay = static_cast< eMenuSoundTriggers >(0x8f); break;
    case 0xEE54C987: soundToPlay = static_cast< eMenuSoundTriggers >(0xd); break;
    case 0xF4B32D4D: soundToPlay = static_cast< eMenuSoundTriggers >(0xB); break;
    case 0xF9545D5C: soundToPlay = static_cast< eMenuSoundTriggers >(0x6C); break;
    case 0xFA274177: soundToPlay = static_cast< eMenuSoundTriggers >(0x80); break;
    case 0xFABBBF40: soundToPlay = static_cast< eMenuSoundTriggers >(0xB); break;
    case 0xFC543E69: soundToPlay = static_cast< eMenuSoundTriggers >(4); break;
    }

    soundToPlay = NotifySoundMessage(msg, soundToPlay);
    if (soundToPlay != UISND_NONE) {
        g_pEAXSound->PlayUISoundFX(soundToPlay);
    }
}
