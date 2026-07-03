#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"

#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

KeyboardEditString::KeyboardEditString() {
    TextInputObject = nullptr;
    MaxTextLength = 0;
    bMemSet(EditStringUCS2, 0, 0x200);
    CursorPosUCS2 = 0;
    bMemSet(EditStringPacked, 0, 0x100);
    bMemSet(InitialString, 0, 0x100);
    mEnabled = false;
}

void KeyboardEditString::SyncEditIntoPacked() {
    WideStringToPackedString(EditStringPacked, 0x100, EditStringUCS2);
}

char *KeyboardEditString::GetEditedString() {
    SyncEditIntoPacked();
    return EditStringPacked;
}

void KeyboardEditString::EndCapture() {
    TextInputObject = nullptr;
    bMemSet(EditStringUCS2, 0, 0x200);
    mEnabled = false;
    bMemSet(EditStringPacked, 0, 0x100);
    bMemSet(InitialString, 0, 0x100);
}

void KeyboardEditString::GetStringForDisplay(char *buffer, int size) {
    SyncEditIntoPacked();
    if (ModeFlags == 5) {
        FEngSNMakeHidden(buffer, size, EditStringUCS2);
    } else {
        bStrNCpy(buffer, EditStringPacked, size);
    }
}

void KeyboardEditString::RevertToOriginalString() {
    PackedStringToWideString(EditStringUCS2, 0x200, InitialString);
    SyncEditIntoPacked();
}

FEngTextInputObject::~FEngTextInputObject() {
    gKeyboardManager.EndCapture();
}

void FEngTextInputObject::ReturnPressed() {
    if (gKeyboardManager.GetModeFlags() == 6) {
        if (bStrLen(gKeyboardManager.GetEditedString()) == 0) {
            return;
        }
    }
    cFEngJoyInput::Get()->FlushActions();
    RedrawString(false);
    ParentPackage->NotificationMessage(0xda5b8712, DisplayString, 0, 0);
    gKeyboardManager.EndCapture();
    ParentPackage->FEngEndTextInput();
}

void FEngTextInputObject::EscapePressed() {
    gKeyboardManager.RevertToOriginalString();
    RedrawString(false);
    ParentPackage->NotificationMessage(0xc9d30688, DisplayString, 0, 0);
    gKeyboardManager.EndCapture();
    ParentPackage->FEngEndTextInput();
}

void FEngTextInputObject::RedrawString(bool pIncludeCursor) {
    if (DisplayString) {
        char buffer[156];
        u16 widestring[156];
        FEngFont *font;
        int width;
        int flags;
        i16 *fitstring;

        gKeyboardManager.GetStringForDisplay(buffer, 0x9C);
        if (pIncludeCursor) {
            int blink_time = mBlinkTime;
            mBlinkTime = blink_time + 1;
            if (blink_time + 1 > 0x59) {
                mBlinkTime = 0;
            }
            if (mBlinkTime < 0x2D) {
                bStrCat(buffer, buffer, "|");
            } else {
                bStrCat(buffer, buffer, " ");
            }
        }

        bStrCpy(widestring, buffer);
        font = FindFont(DisplayString->Handle);
        width = DisplayString->MaxWidth;
        flags = DisplayString->Flags;
        fitstring = reinterpret_cast<short *>(widestring);

        for (; *fitstring != 0; fitstring++) {
            if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
                break;
            }
        }

        FESetString(DisplayString, fitstring);
    }
}

void FEngTextInputObject::Notify(uint32 msg) {
    if (msg == 0xc98356ba) {
        RedrawString(true);
    } else if (msg == 0x0c407210) {
        ReturnPressed();
    }
}
