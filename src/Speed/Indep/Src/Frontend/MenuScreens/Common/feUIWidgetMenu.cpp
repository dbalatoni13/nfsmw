#include "Speed/Indep/Src/Frontend/MenuScreens/Common/UIWidgetMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEInputWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/IconPanel.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

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
extern void FEngSetCurrentButton(const char *pkg_name, unsigned int hash);

extern char *bStrNCpy(char *dest, const char *src, int n);

extern Timer RealTimer;
extern Timer KBCreationTimer;
extern float g_KBDelaySeconds;

UIWidgetMenu::UIWidgetMenu(ScreenConstructorData *sd)
    : MenuScreen(sd) //
    , pCurrentOption(nullptr) //
    , pViewTop(nullptr) //
    , pTitleMaster(nullptr) //
    , pDataMaster(nullptr) //
    , pPrevButtonObj(nullptr) //
    , pDone(nullptr) //
{
    ScrollBar.~FEScrollBar();
    new (&ScrollBar) FEScrollBar(GetPackageName(), "scrollbar", true, false, false);
    iIndexToAdd = 1;
    iLastSelectedIndex = 1;
    bScrollWrapped = true;
    pTitleName = "OPTION_NAME_";
    pDataName = "OPTION_DATA_";
    pDataImageName = "OPTION_IMAGE_";
    pBackingName = "OPTION_BACKING_";
    pLeftArrowName = "LEFT_ARROW_";
    pRightArrowName = "RIGHT_ARROW_";
    pSliderName = "SLIDER_";
    vWidgetStartPos = bVector2(0.0f, 0.0f);
    vLastWidgetPos = bVector2(0.0f, 0.0f);
    vWidgetSize = bVector2(0.0f, 0.0f);
    vMaxTitleSize = bVector2(175.0f, 24.0f);
    vMaxDataSize = bVector2(175.0f, 24.0f);
    vDataPos = bVector2(0.0f, 0.0f);
    vWidgetSpacing = bVector2(4.0f, 0.0f);
    iMaxWidgetsOnScreen = 7;
    iPrevButtonMessage = 0;
    iPrevParam1 = 0;
    iPrevParam2 = 0;
    bCurrentOptionSet = false;
    bHasScrollBar = true;
    bViewNeedsSync = false;
    bAllowScroll = true;
    pTitleMaster = FEngFindObject(GetPackageName(), 0xA753C46C);
    pDataMaster = FEngFindObject(GetPackageName(), 0xC128B184);
    pCursor = FEngFindObject(GetPackageName(), 0x06745352);
    pDoneText = FEngFindString(GetPackageName(), 0xF16CF3A9);
    pDone = FEngFindObject(GetPackageName(), 0xD79B07A0);
    if (pTitleMaster && pDataMaster) {
        SetInitialPositions();
        mPlaySound = false;
    }
}

void UIWidgetMenu::Setup() {
}

void UIWidgetMenu::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    switch (msg) {
    case 0x35f8620b:
        if (!pCurrentOption) return;
        if (!pCurrentOption->IsEnabled()) return;
        SetOption(pCurrentOption);
        return;
    case 0xc407210:
    case 0x911ab364:
        StorePrevNotification(msg, pobj, param1, param2);
    case 0x9120409e:
    case 0xb5971bf1:
        if (!bAllowScroll) return;
        if (!pCurrentOption) return;
        if (!pCurrentOption->IsEnabled()) return;
        pCurrentOption->Act(GetPackageName(), msg);
        return;
    case 0x72619778:
        if (!bAllowScroll) return;
        if (bScrollWrapped) {
            ScrollWrapped(eSD_PREV);
            return;
        }
        Scroll(eSD_PREV);
        return;
    case 0x911c0a4b:
        if (!bAllowScroll) return;
        if (bScrollWrapped) {
            ScrollWrapped(eSD_NEXT);
            return;
        }
        Scroll(eSD_NEXT);
        return;
    case 0x92b703b5:
        RefreshWidgets();
        return;
    case 0xaf0bbd92:
        ClearWidgets();
        Setup();
        return;
    case 0x81017864: {
        if ((RealTimer - KBCreationTimer).GetSeconds() < g_KBDelaySeconds) return;
        FEInputWidget *widge = static_cast<FEInputWidget *>(pCurrentOption);
        widge->SetInputText("");
        FEngBeginTextInput(widge->GetDataObject()->NameHash, widge->GetEditMode(), widge->GetInputText(), widge->GetTitle(), widge->GetMaxInputLength());
        bAllowScroll = false;
        return;
    }
    case 0xda5b8712: {
        KBCreationTimer = RealTimer;
        FEInputWidget *widge = static_cast<FEInputWidget *>(pCurrentOption);
        widge->SetInputText(FEngGetEditedString());
        if (pCurrentOption && pCurrentOption->IsEnabled()) {
            pCurrentOption->Act(GetPackageName(), 0xda5b8712);
        }
        widge->Draw();
        bAllowScroll = true;
        return;
    }
    case 0xc9d30688:
        bAllowScroll = true;
        return;
    case 0x84378bef:
    default:
        return;
    }
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
    unsigned int index = 1;
    {
        FEWidget *w = Options.GetHead();
        while (w != Options.EndOfList()) {
            if (opt == w) {
                return index;
            }
            index++;
            w = w->GetNext();
        }
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
    {
        FEWidget *w = Options.GetHead();
        while (w != Options.EndOfList()) {
            w->Draw();
            w = w->GetNext();
        }
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
    {
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

unsigned int UIWidgetMenu::AddToggleOption(FEToggleWidget *option, bool use_arrow) {
    float img_left;
    float img_right;

    option->SetTitleObject(GetCurrentFEString(pTitleName));
    option->SetDataObject(GetCurrentFEString(pDataName));
    option->SetBacking(GetCurrentFEObject(pBackingName));
    option->SetTopLeft(vLastWidgetPos);
    option->SetMaxTitleSize(vMaxTitleSize);
    option->SetMaxDataSize(vMaxDataSize);
    option->SetDataPos(vDataPos);
    option->SetLeftImage(GetCurrentFEImage(pLeftArrowName));
    option->SetRightImage(GetCurrentFEImage(pRightArrowName));
    Options.AddTail(option);
    iIndexToAdd++;
    IncrementStartPos();
    if (!option->IsEnabled()) {
        option->Disable();
    }
    option->Show();
    option->Draw();
    option->Position();
    img_left = FEngGetTopLeftX(option->GetRightImage()) + bAbs(FEngGetSizeX(option->GetRightImage()));
    option->SetWidth(bAbs(option->GetTopLeftX() - img_left));
    img_right = bAbs(FEngGetSizeY(option->GetRightImage()));
    option->SetHeight(img_right);
    return iIndexToAdd - 1;
}

unsigned int UIWidgetMenu::AddSliderOption(FESliderWidget *option, bool use_arrow) {
    char sztemp[64];
    float img_left;
    float img_right;

    FEngSNPrintf(sztemp, 0x40, "%s%d", pSliderName, iIndexToAdd);
    option->SetTitleObject(GetCurrentFEString(pTitleName));
    option->InitSliderObjects(GetPackageName(), sztemp);
    option->SetInitialValues();
    option->SetTopLeft(vLastWidgetPos);
    option->SetMaxTitleSize(vMaxTitleSize);
    option->SetMaxDataSize(vMaxDataSize);
    option->SetDataPos(vDataPos);
    option->SetLeftImage(GetCurrentFEImage(pLeftArrowName));
    option->SetRightImage(GetCurrentFEImage(pRightArrowName));
    Options.AddTail(option);
    iIndexToAdd++;
    IncrementStartPos();
    if (!option->IsEnabled()) {
        option->Disable();
    }
    option->Show();
    option->Draw();
    option->Position();
    img_left = FEngGetTopLeftX(option->GetRightImage()) + bAbs(FEngGetSizeX(option->GetRightImage()));
    option->SetWidth(bAbs(option->GetTopLeftX() - img_left));
    img_right = bAbs(FEngGetSizeY(option->GetTitleObject()));
    option->SetHeight(img_right);
    return iIndexToAdd - 1;
}

void UIWidgetMenu::SetInitialOption(int number) {
    if (Options.IsEmpty()) {
        if (bHasScrollBar) {
            ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1,
                             GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
        }
        return;
    }
    if (bCurrentOptionSet) goto update_scrollbar;

    bool need_first_avail;
    need_first_avail = false;
    if (number != 0) {
        FEWidget *w = GetWidget(number);
        if (!w || w == Options.EndOfList() || !w->IsEnabled()) {
            need_first_avail = true;
        } else {
            SetOption(w);
            iLastSelectedIndex = number;
            bCurrentOptionSet = true;
        }
    } else {
        if (pDone) {
            if (pCurrentOption) {
                pCurrentOption->UnsetFocus();
                pCurrentOption = nullptr;
            }
            FEngSetCurrentButton(GetPackageName(), pDone->NameHash);
        } else {
            need_first_avail = true;
        }
    }
    if (need_first_avail) {
        FEWidget *w = Options.GetHead();
        iLastSelectedIndex = 1;
        while (w) {
            if (w->IsEnabled() || w == Options.EndOfList()) {
                SetOption(w);
                bCurrentOptionSet = true;
                break;
            }
            w = w->GetNext();
            iLastSelectedIndex++;
        }
    }
    SyncViewToSelection();
update_scrollbar:
    if (bHasScrollBar) {
        ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1,
                         GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
    }
}

void UIWidgetMenu::Scroll(eScrollDir dir) {
    if (Options.IsEmpty()) return;

    if (bViewNeedsSync) {
        SyncViewToSelection();
        return;
    }

    FEWidget *new_option = pCurrentOption;
    FEWidget *new_view = pViewTop;

    if (dir == eSD_NEXT) {
        if (new_option && new_option == Options.GetTail() && pDone) {
            new_option = nullptr;
            FEngSetCurrentButton(GetPackageName(), pDone->NameHash);
        } else {
            if (new_option != Options.GetTail()) {
                do {
                    new_option = new_option->GetNext();
                    iLastSelectedIndex = bMin(static_cast<int>(iIndexToAdd - 1),
                                              static_cast<int>(iLastSelectedIndex + 1));
                } while (new_option && !new_option->IsEnabled() &&
                         new_option != Options.GetTail());

                unsigned int sel_idx = GetWidgetIndex(new_option);
                int view_idx = GetWidgetIndex(pViewTop);
                if (sel_idx >= static_cast<unsigned int>(view_idx + iMaxWidgetsOnScreen)) {
                    new_view = pViewTop->GetNext();
                }
            }
        }
    } else {
        if (!new_option) {
            new_option = Options.GetTail();
        } else {
            if (new_option != Options.GetHead()) {
                do {
                    new_option = new_option->GetPrev();
                    iLastSelectedIndex = bMax(1, static_cast<int>(iLastSelectedIndex - 1));
                } while (new_option && !new_option->IsEnabled() &&
                         new_option != Options.GetHead());
            }
            if (new_option == pViewTop->GetPrev()) {
                new_view = new_option;
            }
        }
    }

    if (pViewTop != new_view) {
        pViewTop = new_view;
        Reposition();
    }
    if (pCurrentOption != new_option) {
        SetOption(new_option);
        if (bHasScrollBar && pCurrentOption) {
            ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1,
                             GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
        }
    }
}

void UIWidgetMenu::ScrollWrapped(eScrollDir dir) {
    if (Options.IsEmpty()) return;

    if (bViewNeedsSync) {
        SyncViewToSelection();
        return;
    }

    FEWidget *new_option = pCurrentOption;
    FEWidget *new_view = pViewTop;

    if (dir == eSD_NEXT) {
        do {
            if (!new_option ||
                (new_option == Options.GetTail() && !pDone)) {
                new_view = Options.GetHead();
                new_option = new_view;
            } else if (new_option == Options.GetTail() && pDone) {
                new_option = nullptr;
                FEngSetCurrentButton(GetPackageName(), pDone->NameHash);
            } else {
                unsigned int idx = iLastSelectedIndex + 1;
                new_option = new_option->GetNext();
                iLastSelectedIndex = idx;
                if (idx > iIndexToAdd - 1) {
                    iLastSelectedIndex = 1;
                }
            }
        } while (new_option && !new_option->IsEnabled());

        unsigned int sel_idx = GetWidgetIndex(new_option);
        int view_idx = GetWidgetIndex(pViewTop);
        if (sel_idx >= static_cast<unsigned int>(view_idx + iMaxWidgetsOnScreen)) {
            new_view = pViewTop->GetNext();
        }
    } else {
        do {
            if (!new_option ||
                (new_option == Options.GetHead() && !pDone)) {
                new_option = Options.GetTail();
                int idx = bMax(0, static_cast<int>(iIndexToAdd - iMaxWidgetsOnScreen) - 1);
                new_view = Options.GetNode(idx);
            } else if (new_option == Options.GetHead() && pDone) {
                new_option = nullptr;
                FEngSetCurrentButton(GetPackageName(), pDone->NameHash);
            } else {
                new_option = new_option->GetPrev();
                int idx = iLastSelectedIndex - 1;
                iLastSelectedIndex = idx;
                if (idx == 0) {
                    iLastSelectedIndex = iIndexToAdd - 1;
                }
            }
        } while (new_option && !new_option->IsEnabled());

        if (iIndexToAdd - 1 > iMaxWidgetsOnScreen &&
            new_option == pViewTop->GetPrev()) {
            new_view = new_option;
        }
    }

    if (pViewTop != new_view) {
        pViewTop = new_view;
        Reposition();
    }
    if (pCurrentOption != new_option) {
        SetOption(new_option);
        if (bHasScrollBar && pCurrentOption) {
            ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1,
                             GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
        }
    }
}
