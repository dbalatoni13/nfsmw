#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"

#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"
#include "Speed/Indep/Src/Frontend/FEngFont.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

KeyboardEditString gKeyboardManager;

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
    mEnabled = false;
    TextInputObject = nullptr;
    bMemSet(EditStringUCS2, 0, 0x200);
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
    if (gKeyboardManager.GetModeFlags() == 6 && bStrLen(gKeyboardManager.GetEditedString()) == 0) {
        return;
    }

    cFEngJoyInput::Get()->FlushActions();
    RedrawString(false);
    ParentPackage->NotificationMessage(0xDA5B8712, reinterpret_cast<FEObject *>(DisplayString), 0, 0);
    gKeyboardManager.EndCapture();
    ParentPackage->FEngEndTextInput();
}

void FEngTextInputObject::EscapePressed() {
    gKeyboardManager.RevertToOriginalString();
    RedrawString(false);
    ParentPackage->NotificationMessage(0xC9D30688, reinterpret_cast<FEObject *>(DisplayString), 0, 0);
    gKeyboardManager.EndCapture();
    ParentPackage->FEngEndTextInput();
}

void FEngTextInputObject::RedrawString(bool pIncludeCursor) {
    if (DisplayString == nullptr) {
        return;
    }

    char buffer[156];
    unsigned short widestring[156];
    gKeyboardManager.GetStringForDisplay(buffer, 0x9C);

    if (pIncludeCursor) {
        mBlinkTime++;
        if (mBlinkTime > 0x59) {
            mBlinkTime = 0;
        }
        if (mBlinkTime <= 0x2C) {
            bStrCat(buffer, buffer, "|");
        } else {
            bStrCat(buffer, buffer, " ");
        }
    }

    bStrCpy(widestring, buffer);
    short *fitstring = reinterpret_cast<short *>(widestring);
    FEngFont *font = FindFont(DisplayString->Handle);
    int width = DisplayString->MaxWidth;
    int flags = DisplayString->Flags;
    while (fitstring[0] != 0) {
        if (font->GetLineWidth(fitstring, flags, 0, false) <= static_cast<float>(width)) {
            break;
        }
        fitstring++;
    }
    FESetString(DisplayString, fitstring);
}

void FEngTextInputObject::Notify(uint32 msg) {
    if (msg == 0xC98356BA) {
        RedrawString(true);
    } else if (msg == 0x0C407210) {
        ReturnPressed();
    }
}
