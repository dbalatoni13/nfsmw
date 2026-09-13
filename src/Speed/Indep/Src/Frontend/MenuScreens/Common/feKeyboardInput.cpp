#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"

#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

KeyboardEditString::KeyboardEditString() {
    TextInputObject = nullptr;
    MaxTextLength = 0;
    bMemSet(EditStringUCS2, 0, sizeof(EditStringUCS2));
    CursorPosUCS2 = 0;
    bMemSet(EditStringPacked, 0, sizeof(EditStringPacked));
    bMemSet(InitialString, 0, sizeof(InitialString));
    mEnabled = false;
}

void KeyboardEditString::SyncEditIntoPacked() {
    WideStringToPackedString(EditStringPacked, sizeof(EditStringPacked), EditStringUCS2);
}

char *KeyboardEditString::GetEditedString() {
    SyncEditIntoPacked();
    return EditStringPacked;
}

void KeyboardEditString::EndCapture() {
    mEnabled = false;
    TextInputObject = nullptr;
    bMemSet(EditStringUCS2, 0, sizeof(EditStringUCS2));
    bMemSet(EditStringPacked, 0, sizeof(EditStringPacked));
    bMemSet(InitialString, 0, sizeof(InitialString));
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

FEngTextInputObject::FEngTextInputObject(MenuScreen *pkg, FEString *obj, uint32 mode, const char *start_string, uint32 max_text_length)
    : DisplayString(obj), ParentPackage(pkg), mBlinkTime(0) {
    gKeyboardManager.StartCapture(this, mode, start_string, max_text_length);
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
    if (DisplayString != nullptr) {
        char buffer[156];

        gKeyboardManager.GetStringForDisplay(buffer, 0x9C);
        if (pIncludeCursor) {
            mBlinkTime++;
            if (mBlinkTime > 0x59) {
                mBlinkTime = 0;
            }

            if (mBlinkTime < 0x2D) {
                bStrCat(buffer, buffer, "|");
            } else {
                bStrCat(buffer, buffer, " ");
            }
        }

        u16 widestring[156];
        bStrCpy(widestring, buffer);
        FEngFont *font = FindFont(DisplayString->Handle);
        int width = DisplayString->MaxWidth;
        int flags = DisplayString->Flags;
        i16 *fitstring = reinterpret_cast<i16 *>(widestring);

        while (*fitstring != 0) {
            if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
                break;
            }
            fitstring++;
        }

        FESetString(DisplayString, fitstring);
    }
}

void FEngTextInputObject::Notify(uint32 msg) {
    if (msg == FEMSG_SCREEN_TICK) {
        RedrawString(true);
    } else if (msg == __BUTTON_PRESSED__) {
        ReturnPressed();
    }
}
