#ifndef FE_INPUT_WIDGET_HPP
#define FE_INPUT_WIDGET_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"

extern char *bStrNCpy(char *dest, const char *src, int n);

// total size: 0x194
struct FEInputWidget : public FEStatWidget {
    char InputText[156];       // offset 0x54, size 0x9C
    char Title[156];           // offset 0xF0, size 0x9C
    unsigned int MaxInputLength; // offset 0x18C, size 0x4
    unsigned int EditMode;     // offset 0x190, size 0x4

    FEInputWidget(unsigned int max_input_length, const char *init_text, unsigned int edit_mode, bool enabled);

    void Act(const char *parent_pkg, unsigned int data) override;
    void CheckMouse(const char *parent_pkg, const float mouse_x, const float mouse_y) override;
    void Draw() override;
    void Enable() override;
    void Disable() override;
    void Show() override;
    void Hide() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;

    void SetInputFocus();
    void SetTitle(const char *text);

    inline void SetInputText(const char *text) {
        bStrNCpy(InputText, text, 0x9b);
    }

    inline const char *GetInputText() {
        return InputText;
    }

    inline void SetEditMode(unsigned int mode) {
        EditMode = mode;
    }

    inline unsigned int GetEditMode() {
        return EditMode;
    }

    inline const char *GetTitle() {
        return Title;
    }

    inline unsigned int GetMaxInputLength() {
        return MaxInputLength;
    }
};

#endif
