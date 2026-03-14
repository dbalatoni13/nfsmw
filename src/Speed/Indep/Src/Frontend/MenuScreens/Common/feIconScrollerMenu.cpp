#include "IconPanel.hpp"
#include "IconScroller.hpp"
#include "IconScrollerMenu.hpp"

extern void FEngSetTextureHash(FEImage *image, unsigned int hash);

// ============================================================
// IconPanel
// ============================================================

void IconPanel::Update() {
    AnimateList();
}

void IconPanel::AnimateList() {
    float list_width = 0.0f;
    float list_height = 0.0f;
    AnimateSelected(list_width, list_height);
}

// ============================================================
// IconScrollerMenu
// ============================================================

void IconScrollerMenu::StorePrevNotification(unsigned int msg, FEObject *pobj, unsigned int param1, unsigned int param2) {
    PrevButtonMessage = msg;
    PrevButtonObj = pobj;
    PrevParam1 = param1;
    PrevParam2 = param2;
}

eMenuSoundTriggers IconScrollerMenu::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if ((msg == 0x48122792 || msg == 0x4ac5e165) && !Options.JustScrolled()) {
        return static_cast< eMenuSoundTriggers >(-1);
    }
    return maybe;
}

void IconScrollerMenu::AddOption(IconOption *option) {
    FEImage *img = Options.AddOption(option);
    FEngSetTextureHash(img, option->Item);
}

IconOption *IconPanel::GetOption(int to_find) {
    if (to_find < 1) {
        return nullptr;
    }
    IconOption *node = Options.GetHead();
    int i = 1;
    while (node != Options.EndOfList()) {
        if (to_find == i) {
            return node;
        }
        i++;
        node = node->GetNext();
    }
    return nullptr;
}

int IconPanel::GetOptionIndex(IconOption *to_find) {
    if (!to_find) {
        return -1;
    }
    IconOption *node = Options.GetHead();
    int i = 1;
    while (node != Options.EndOfList()) {
        if (node == to_find) {
            return i;
        }
        i++;
        node = node->GetNext();
    }
    return -1;
}

IconOption *IconPanel::GetHead() {
    return Options.GetHead();
}

bool IconPanel::IsHead(IconOption *option) {
    return option == Options.GetHead();
}

bool IconPanel::IsTail(IconOption *option) {
    return option == Options.GetTail();
}

bool IconPanel::IsEndOfList(IconOption *opt) {
    return opt == Options.EndOfList();
}

bool IconScroller::IsHead(IconOption *option) {
    return option == static_cast<IconOption *>(HeadBookEnd->GetNext());
}

bool IconScroller::IsTail(IconOption *option) {
    return option == static_cast<IconOption *>(TailBookEnd->GetPrev());
}

bool IconScroller::IsEndOfList(IconOption *option) {
    return option == HeadBookEnd || option == TailBookEnd;
}