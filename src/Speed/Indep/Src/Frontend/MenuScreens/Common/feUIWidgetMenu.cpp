#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"

extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern FEString *FEngFindString(const char *pkg_name, int name_hash);
extern FEImage *FEngFindImage(const char *pkg_name, int name_hash);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern unsigned long FEHashUpper(const char *str);
extern int FEngSNPrintf(char *dest, int size, const char *fmt, ...);
extern void FEngGetTopLeft(FEObject *object, float &x, float &y);
extern void FEngSetTopLeft(FEObject *object, float x, float y);
extern void FEngGetSize(FEObject *object, float &x, float &y);
extern void FEngSetSize(FEObject *object, float x, float y);
extern char *bStrCat(char *dest, const char *src1, const char *src2);
extern unsigned int FEngHashString(const char *, ...);

void UIWidgetMenu::Setup() {
}

eMenuSoundTriggers UIWidgetMenu::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if ((msg == 0x48122792 || msg == 0x4AC5E165) && pCurrentOption && !pCurrentOption->IsEnabled()) {
        return static_cast<eMenuSoundTriggers>(-1);
    }
    return maybe;
}

void UIWidgetMenu::StorePrevNotification(unsigned int msg, FEObject *pobj, unsigned int param1, unsigned int param2) {
    iPrevButtonMessage = msg;
    pPrevButtonObj = pobj;
    iPrevParam1 = param1;
    iPrevParam2 = param2;
}

FEWidget *UIWidgetMenu::GetWidget(unsigned int id) {
    if (id == 0) {
        return nullptr;
    }
    return static_cast<FEWidget *>(Options.GetNode(id - 1));
}

unsigned int UIWidgetMenu::GetWidgetIndex(FEWidget *opt) {
    FEWidget *node = Options.GetHead();
    unsigned int index = 1;
    while (node != Options.EndOfList()) {
        if (opt == node) return index;
        index++;
        node = node->GetNext();
    }
    return 0;
}

void UIWidgetMenu::IncrementStartPos() {
    float y = vWidgetSize.y + vWidgetSpacing.y + vLastWidgetPos.y;
    vLastWidgetPos.y = y;
    vDataPos.y = y;
}

FEString *UIWidgetMenu::GetCurrentFEString(const char *string_name) {
    char sztemp[32];
    char sztemp2[32];
    bStrCat(sztemp, string_name, "%d");
    FEngSNPrintf(sztemp2, 0x20, sztemp, iIndexToAdd);
    return FEngFindString(GetPackageName(), FEHashUpper(sztemp2));
}

FEImage *UIWidgetMenu::GetCurrentFEImage(const char *img_name) {
    char sztemp[32];
    char sztemp2[32];
    bStrCat(sztemp, img_name, "%d");
    FEngSNPrintf(sztemp2, 0x20, sztemp, iIndexToAdd);
    FEImage *obj = FEngFindImage(GetPackageName(), FEHashUpper(sztemp2));
    if (!obj) {
        obj = FEngFindImage(GetPackageName(), FEngHashString("%s0", img_name));
    }
    return obj;
}

FEObject *UIWidgetMenu::GetCurrentFEObject(const char *name) {
    char sztemp[32];
    char sztemp2[32];
    bStrCat(sztemp, name, "%d");
    FEngSNPrintf(sztemp2, 0x20, sztemp, iIndexToAdd);
    return FEngFindObject(GetPackageName(), FEHashUpper(sztemp2));
}

void UIWidgetMenu::RefreshWidgets() {
    FEWidget *w = Options.GetHead();
    while (w != Options.EndOfList()) {
        w->Draw();
        w = w->GetNext();
    }
}

void UIWidgetMenu::SetOption(FEWidget *opt) {
    FEWidget *old = pCurrentOption;
    if (old != opt && old) {
        old->UnsetFocus();
    }
    pCurrentOption = opt;
    if (opt) {
        opt->SetFocus(GetPackageName());
    }
    UpdateCursorPos();
}

void UIWidgetMenu::SetInitialPositions() {
    FEngGetTopLeft(pTitleMaster, vWidgetStartPos.x, vWidgetStartPos.y);
    vLastWidgetPos = vWidgetStartPos;
    FEngGetTopLeft(pDataMaster, vDataPos.x, vDataPos.y);
    FEngGetSize(pTitleMaster, vMaxTitleSize.x, vMaxTitleSize.y);
    FEngGetSize(pDataMaster, vMaxDataSize.x, vMaxDataSize.y);
    vWidgetSize.y = vMaxTitleSize.y;
    vWidgetSize.x = bAbs(vWidgetStartPos.x - (vDataPos.x + vMaxDataSize.x));
}

void UIWidgetMenu::ClearWidgets() {
    FEWidget *w = Options.GetHead();
    while (w != Options.EndOfList()) {
        w->Hide();
        w->UnsetFocus();
        w = w->GetNext();
    }
    while (!Options.IsEmpty()) {
        FEWidget *head = Options.RemoveHead();
        delete head;
    }
    pCurrentOption = nullptr;
    iIndexToAdd = 1;
    bCurrentOptionSet = false;
    iLastSelectedIndex = 1;
    SetInitialPositions();
}

void UIWidgetMenu::UpdateCursorPos() {
    if (pCursor) {
        if (pCurrentOption) {
            unsigned int pos = GetWidgetIndex(pCurrentOption);
            pos -= GetWidgetIndex(pViewTop);
            if (pos + 1 && pos + 1 <= iMaxWidgetsOnScreen) {
                FEngSetScript(pCursor, FEngHashString("POS%d", pos + 1), true);
            } else {
                FEngSetScript(pCursor, 0x16a259, true);
            }
        } else {
            FEngSetScript(pCursor, 0x16a259, true);
        }
    }
}

void UIWidgetMenu::Reset() {
    FEWidget *head = Options.GetHead();
    if (head != Options.EndOfList()) {
        bViewNeedsSync = false;
        pCurrentOption = head;
        pViewTop = head;
        SetOption(head);
        Reposition();
    }
}

void UIWidgetMenu::Reposition() {
    unsigned int index = 1;
    unsigned int view_index = GetWidgetIndex(pViewTop);
    float pos = vWidgetStartPos.y;
    FEWidget *w = Options.GetHead();
    while (w != Options.EndOfList()) {
        if (index >= view_index && index < view_index + iMaxWidgetsOnScreen) {
            w->Show();
            w->SetPosY(pos);
            w->Draw();
            w->Position();
            pos += vWidgetSize.y;
        } else {
            w->SetPosY(6969.0f);
            w->Hide();
        }
        index++;
        w = w->GetNext();
    }
    UpdateCursorPos();
}

void UIWidgetMenu::SyncViewToSelection() {
    if (Options.IsEmpty()) {
        return;
    }
    if (!pCurrentOption && !pDone) {
        Reset();
        return;
    }
    if (static_cast< unsigned int >(iIndexToAdd - 1) > iMaxWidgetsOnScreen &&
        GetWidgetIndex(pCurrentOption) <= static_cast< unsigned int >(iIndexToAdd - iMaxWidgetsOnScreen)) {
        pViewTop = pCurrentOption;
    } else {
        int node_index = iIndexToAdd - iMaxWidgetsOnScreen;
        node_index = node_index - 1;
        if (node_index < 0) {
            node_index = 0;
        }
        pViewTop = static_cast< FEWidget * >(Options.GetNode(node_index));
    }
    Reposition();
    bViewNeedsSync = false;
}

unsigned int UIWidgetMenu::AddButtonOption(FEButtonWidget *option) {
    option->SetTitleObject(GetCurrentFEString(pTitleName));
    option->SetBacking(GetCurrentFEObject(pBackingName));
    option->SetTopLeft(vLastWidgetPos);
    option->SetMaxTitleSize(vMaxTitleSize);
    Options.AddTail(option);
    iIndexToAdd++;
    IncrementStartPos();
    if (!option->IsEnabled()) {
        option->Disable();
    }
    option->Show();
    option->Draw();
    option->Position();
    option->SetWidth(bAbs(vWidgetSize.x));
    option->SetHeight(bAbs(vWidgetSize.y));
    return iIndexToAdd - 1;
}