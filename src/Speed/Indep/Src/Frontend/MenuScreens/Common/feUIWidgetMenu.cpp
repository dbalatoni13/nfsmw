#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feUIWidgetMenu.hpp"
#include "Speed/Indep/Src/FEng/FEObject.h"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feKeyboardInput.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/InGame/FEPkg_MU_Keyboard.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"

extern float g_KBDelaySeconds;

UIWidgetMenu::UIWidgetMenu(ScreenConstructorData *sd)
    : MenuScreen(sd), ScrollBar(GetPackageName(), "scrollbar", true, false, false), pCurrentOption(nullptr), pViewTop(nullptr), pTitleMaster(nullptr),
      pDataMaster(nullptr), pPrevButtonObj(nullptr), pDone(nullptr), iIndexToAdd(1), iLastSelectedIndex(1), bScrollWrapped(true),
      pTitleName("OPTION_NAME_"), pDataName("OPTION_DATA_"), pDataImageName("OPTION_IMAGE_"), pBackingName("OPTION_BACKING_"),
      pLeftArrowName("LEFT_ARROW_"), pRightArrowName("RIGHT_ARROW_"), pSliderName("SLIDER_"), iMaxWidgetsOnScreen(7), iPrevButtonMessage(0),
      iPrevParam1(0), iPrevParam2(0), vMaxTitleSize(175.0f, 24.0f), vWidgetSpacing(4.0f, 0.0f), vMaxDataSize(175.0f, 24.0f),
      vWidgetStartPos(0.0f, 0.0f), vLastWidgetPos(0.0f, 0.0f), vDataPos(0.0f, 0.0f), vWidgetSize(0.0f, 0.0f), bCurrentOptionSet(false),
      bHasScrollBar(true), bViewNeedsSync(false), bAllowScroll(true) {
    const u32 FEObj_TITLEMASTER = 0xA753C46C;
    const u32 FEObj_DATAMASTER = 0xC128B184;
    const u32 FEObj_DONEBUTTON = 0xD79B07A0;
    const u32 FEObj_DONETEXT = 0xF16CF3A9;
    GetPackageName(); // unknown
    GetPackageName();
    pTitleMaster = FEngFindObject(GetPackageName(), FEObj_TITLEMASTER);
    pDataMaster = FEngFindObject(GetPackageName(), FEObj_DATAMASTER);
    pCursor = FEngFindObject(GetPackageName(), 0x06745352);
    pDoneText = FEngFindString(GetPackageName(), FEObj_DONETEXT);
    pDone = FEngFindObject(GetPackageName(), FEObj_DONEBUTTON);
    if ((pTitleMaster != nullptr) && (pDataMaster != nullptr)) {
        SetInitialPositions();
        mPlaySound = false;
    }
}

void UIWidgetMenu::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEHASH_INITCOMPLETE:
            if (pCurrentOption == nullptr)
                return;
            if (!pCurrentOption->IsEnabled())
                return;
            SetOption(pCurrentOption);
            return;
        case __BUTTON_PRESSED__:
        case __PAD_BACK__:
            StorePrevNotification(msg, pobj, param1, param2);
        case __PAD_LEFT__:
        case __PAD_RIGHT__:
            if (!bAllowScroll)
                return;
            if (pCurrentOption == nullptr)
                return;
            if (!pCurrentOption->IsEnabled())
                return;
            pCurrentOption->Act(GetPackageName(), msg);
            return;
        case __PAD_UP__:
            if (!bAllowScroll)
                return;
            if (bScrollWrapped) {
                ScrollWrapped(eSD_PREV);
                return;
            }
            Scroll(eSD_PREV);
            return;
        case __PAD_DOWN__:
            if (!bAllowScroll)
                return;
            if (bScrollWrapped) {
                ScrollWrapped(eSD_NEXT);
                return;
            }
            Scroll(eSD_NEXT);
            return;
        case FEMSG_REFRESH_WIDGETS:
            RefreshWidgets();
            return;
        case FEMSG_RESET_SCREEN:
            ClearWidgets();
            Setup();
            return;
        case FEMSG_BEGIN_INPUT: {
            if ((RealTimer - KBCreationTimer).GetSeconds() < g_KBDelaySeconds)
                return;
            FEInputWidget *widge = static_cast<FEInputWidget *>(pCurrentOption);
            widge->SetInputText("");
            FEngBeginTextInput(widge->GetDataObject()->NameHash, widge->GetEditMode(), widge->GetInputText(), widge->GetTitle(),
                               widge->GetMaxInputLength());
            bAllowScroll = false;
            return;
        }
        case FEMSG_ACCEPT_EDITED_TEXT: {
            KBCreationTimer = RealTimer;
            FEInputWidget *widge = static_cast<FEInputWidget *>(pCurrentOption);
            widge->SetInputText(FEngGetEditedString());
            if ((pCurrentOption != nullptr) && pCurrentOption->IsEnabled()) {
                pCurrentOption->Act(GetPackageName(), 0xda5b8712);
            }
            widge->Draw();
            bAllowScroll = true;
            return;
        }
        case FEMSG_DECLINE_EDITED_TEXT:
            bAllowScroll = true;
            return;
        case FEMSG_EXIT_STARTED:
        default:
            return;
    }
}

eMenuSoundTriggers UIWidgetMenu::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if ((msg == 0x48122792 || msg == 0x4AC5E165) && (pCurrentOption != nullptr) && !pCurrentOption->MovedLastUpdate()) {
        return UISND_NONE;
    }
    return maybe;
}

void UIWidgetMenu::StorePrevNotification(uint32 msg, FEObject *pobj, uint32 param1, uint32 param2) {
    iPrevButtonMessage = msg;
    pPrevButtonObj = pobj;
    iPrevParam1 = param1;
    iPrevParam2 = param2;
}

FEWidget *UIWidgetMenu::GetWidget(uint32 id) {
    if (id == 0) {
        return nullptr;
    }
    return Options.GetNode(id - 1);
}

// UNSOLVED
void UIWidgetMenu::Scroll(eScrollDir dir) {
    if (Options.IsEmpty())
        return;

    if (bViewNeedsSync) {
        SyncViewToSelection();
    } else {

        FEWidget *new_option = pCurrentOption;
        FEWidget *new_view = pViewTop;

        if (dir == eSD_NEXT) {
            if ((new_option != nullptr) && new_option == Options.GetTail() && (pDone != nullptr)) {
                new_option = nullptr;
                FEngSetCurrentButton(GetPackageName(), pDone);
            } else {
                if (new_option != Options.GetTail()) {
                    int min = iIndexToAdd - 1;
                    do {
                        new_option = new_option->GetNext();
                        iLastSelectedIndex = bMin(min, static_cast<int>(iLastSelectedIndex + 1));
                    } while ((new_option != nullptr) && !new_option->IsEnabled() && new_option != Options.GetTail());

                    if (GetWidgetIndex(new_option) >= static_cast<unsigned int>(GetWidgetIndex(pViewTop) + iMaxWidgetsOnScreen)) {
                        new_view = pViewTop->GetNext();
                    }
                }
            }
        } else {
            if (new_option == nullptr) {
                new_option = Options.GetTail();
            } else {
                if (new_option != Options.GetHead()) {
                    do {
                        new_option = new_option->GetPrev();
                        iLastSelectedIndex = bMax(1, static_cast<int>(iLastSelectedIndex - 1));
                    } while ((new_option != nullptr) && !new_option->IsEnabled() && new_option != Options.GetHead());
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
            if (bHasScrollBar && (pCurrentOption != nullptr)) {
                ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1, GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
            }
        }
    }
}

// UNSOLVED (dwarf)
void UIWidgetMenu::ScrollWrapped(eScrollDir dir) {
    if (Options.IsEmpty())
        return;

    if (bViewNeedsSync) {
        SyncViewToSelection();
    } else {
        FEWidget *new_option = pCurrentOption;
        FEWidget *new_view = pViewTop;

        if (dir == eSD_NEXT) {
            do {
                if ((new_option == nullptr) || (new_option == Options.GetTail() && (pDone == nullptr))) {
                    new_view = Options.GetHead();
                    new_option = new_view;
                } else if (new_option == Options.GetTail() && (pDone != nullptr)) {
                    new_option = nullptr;
                    FEngSetCurrentButton(GetPackageName(), pDone);
                } else {
                    new_option = new_option->GetNext();
                    iLastSelectedIndex++;
                    if (iLastSelectedIndex > iIndexToAdd - 1) {
                        iLastSelectedIndex = 1;
                    }
                }
            } while ((new_option != nullptr) && !new_option->IsEnabled());

            if (GetWidgetIndex(new_option) >= static_cast<unsigned int>(GetWidgetIndex(pViewTop) + iMaxWidgetsOnScreen)) {
                new_view = pViewTop->GetNext();
            }
        } else {
            do {
                if ((new_option == nullptr) || (new_option == Options.GetHead() && (pDone == nullptr))) {
                    new_option = Options.GetTail();
                    new_view = Options.GetNode(bMax(0, static_cast<int>(iIndexToAdd - iMaxWidgetsOnScreen) - 1));
                } else if (new_option == Options.GetHead() && (pDone != nullptr)) {
                    new_option = nullptr;
                    FEngSetCurrentButton(GetPackageName(), pDone);
                } else {
                    new_option = new_option->GetPrev();
                    iLastSelectedIndex--;
                    if (iLastSelectedIndex == 0) {
                        iLastSelectedIndex = iIndexToAdd - 1;
                    }
                }
            } while ((new_option != nullptr) && !new_option->IsEnabled());

            if (iIndexToAdd - 1 > iMaxWidgetsOnScreen && new_option == pViewTop->GetPrev()) {
                new_view = new_option;
            }
        }

        if (pViewTop != new_view) {
            pViewTop = new_view;
            Reposition();
        }
        if (pCurrentOption != new_option) {
            SetOption(new_option);
            if (bHasScrollBar && (pCurrentOption != nullptr)) {
                ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1, GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
            }
        }
    }
}

uint32 UIWidgetMenu::AddButtonOption(FEButtonWidget *option) {
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

uint32 UIWidgetMenu::AddToggleOption(FEToggleWidget *option, bool use_arrow) {
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
    float img_left = FEngGetTopLeftX(option->GetRightImage());
    float img_right = img_left + bAbs(FEngGetSizeX(option->GetRightImage()));
    option->SetWidth(bAbs(option->GetTopLeftX() - img_right));
    option->SetHeight(bAbs(FEngGetSizeY(option->GetRightImage())));
    return iIndexToAdd - 1;
}

uint32 UIWidgetMenu::AddSliderOption(FESliderWidget *option, bool use_arrow) {
    char sztemp[64];
    FEngSNPrintf(sztemp, sizeof(sztemp), "%s%d", pSliderName, iIndexToAdd);
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
    float img_left = FEngGetTopLeftX(option->GetRightImage());
    float img_right = img_left + bAbs(FEngGetSizeX(option->GetRightImage()));
    option->SetWidth(bAbs(option->GetTopLeftX() - img_right));
    option->SetHeight(bAbs(FEngGetSizeY(option->GetTitleObject())));
    return iIndexToAdd - 1;
}

FEString *UIWidgetMenu::GetCurrentFEString(const char *string_name) {
    char sztemp[32];
    bStrCat(sztemp, string_name, "%d");
    char sztemp2[32];
    FEngSNPrintf(sztemp2, sizeof(sztemp2), sztemp, iIndexToAdd);
    FEString *obj = FEngFindString(GetPackageName(), FEHashUpper(sztemp2));
    return obj;
}

FEImage *UIWidgetMenu::GetCurrentFEImage(const char *img_name) {
    char sztemp[32];
    bStrCat(sztemp, img_name, "%d");
    char sztemp2[32];
    FEngSNPrintf(sztemp2, sizeof(sztemp2), sztemp, iIndexToAdd);
    FEImage *obj = FEngFindImage(GetPackageName(), sztemp2);
    if (obj == nullptr) {
        obj = FEngFindImage(GetPackageName(), FEngHashString("%s0", img_name));
    }
    return obj;
}

FEObject *UIWidgetMenu::GetCurrentFEObject(const char *name) {
    char sztemp[32];
    bStrCat(sztemp, name, "%d");
    char sztemp2[32];
    FEngSNPrintf(sztemp2, sizeof(sztemp2), sztemp, iIndexToAdd);
    FEObject *obj = FEngFindObject(GetPackageName(), FEHashUpper(sztemp2));
    return obj;
}

void UIWidgetMenu::ClearWidgets() {
    for (FEWidget *w = Options.GetHead(); w != Options.EndOfList(); w = w->GetNext()) {
        w->Hide();
        w->UnsetFocus();
    }

    while (!Options.IsEmpty()) {
        delete Options.RemoveHead();
    }

    iIndexToAdd = 1;
    pCurrentOption = nullptr;
    iLastSelectedIndex = 1;
    bCurrentOptionSet = false;
    SetInitialPositions();
}

void UIWidgetMenu::RefreshWidgets() {
    for (FEWidget *w = Options.GetHead(); w != Options.EndOfList(); w = w->GetNext()) {
        w->Draw();
    }
}

// UNSOLVED
void UIWidgetMenu::SetInitialOption(int number) {
    if (Options.IsEmpty()) {
        if (bHasScrollBar) {
            ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1, GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
        }
        return;
    }
    if (!bCurrentOptionSet) {
        FEWidget *w;
        bool need_first_avail = false;

        if (number != 0) {
            w = GetWidget(number);
            if ((w == nullptr) || w == Options.EndOfList() || !w->IsEnabled()) {
                need_first_avail = true;
            } else {
                SetOption(w);
                iLastSelectedIndex = number;
                bCurrentOptionSet = true;
            }
        } else {
            if (pDone != nullptr) {
                if (pCurrentOption != nullptr) {
                    pCurrentOption->UnsetFocus();
                    pCurrentOption = nullptr;
                }
                FEngSetCurrentButton(GetPackageName(), pDone);
            } else {
                need_first_avail = true;
            }
        }

        if (need_first_avail) {
            w = Options.GetHead();
            iLastSelectedIndex = 1;
            while (w != nullptr) {
                if (!w->IsEnabled() && w != Options.EndOfList()) {
                    w = w->GetNext();
                    iLastSelectedIndex++;
                } else {
                    SetOption(w);
                    bCurrentOptionSet = true;
                    break;
                }
            }
        }
        SyncViewToSelection();
    }

    if (bHasScrollBar) {
        ScrollBar.Update(iMaxWidgetsOnScreen, iIndexToAdd - 1, GetWidgetIndex(pViewTop), GetWidgetIndex(pCurrentOption));
    }
}

void UIWidgetMenu::SetOption(FEWidget *opt) {
    if (pCurrentOption != opt && (pCurrentOption != nullptr)) {
        pCurrentOption->UnsetFocus();
    }
    pCurrentOption = opt;
    if (pCurrentOption != nullptr) {
        pCurrentOption->SetFocus(GetPackageName());
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

void UIWidgetMenu::Reposition() {
    uint32 index = 1;
    uint32 view_index = GetWidgetIndex(pViewTop);
    float pos = vWidgetStartPos.y;
    for (FEWidget *w = Options.GetHead(); w != Options.EndOfList(); w = w->GetNext()) {
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
    }
    UpdateCursorPos();
}

void UIWidgetMenu::Reset() {
    if (!Options.IsEmpty()) {
        bViewNeedsSync = false;
        pCurrentOption = Options.GetHead();
        pViewTop = pCurrentOption;
        SetOption(pCurrentOption);
        Reposition();
    }
}

void UIWidgetMenu::UpdateCursorPos() {
    if (pCursor != nullptr) {
        if (pCurrentOption != nullptr) {
            uint32 pos = GetWidgetIndex(pCurrentOption) - GetWidgetIndex(pViewTop) + 1;
            if (pos != 0 && pos <= iMaxWidgetsOnScreen) {
                FEngSetScript(pCursor, FEngHashString("POS%d", pos), true);
            } else {
                FEngSetScript(pCursor, FEHASH_HIDE, true);
            }
        } else {
            FEngSetScript(pCursor, FEHASH_HIDE, true);
        }
    }
}

void UIWidgetMenu::IncrementStartPos() {
    vLastWidgetPos.y += vWidgetSize.y + vWidgetSpacing.y;
    vDataPos.y = vLastWidgetPos.y;
}

void UIWidgetMenu::SyncViewToSelection() {
    if (Options.IsEmpty()) {
        return;
    }
    if ((pCurrentOption == nullptr) && (pDone == nullptr)) {
        Reset();
        return;
    }
    if (iIndexToAdd - 1 > iMaxWidgetsOnScreen && GetWidgetIndex(pCurrentOption) <= iIndexToAdd - iMaxWidgetsOnScreen) {
        pViewTop = pCurrentOption;
    } else {
        uint32 index = bMax(0, iIndexToAdd - iMaxWidgetsOnScreen - 1);
        pViewTop = Options.GetNode(index);
    }
    Reposition();
    bViewNeedsSync = false;
}

uint32 UIWidgetMenu::GetWidgetIndex(FEWidget *opt) {
    uint32 index = 1;
    for (FEWidget *w = Options.GetHead(); w != Options.EndOfList(); w = w->GetNext()) {
        if (opt == w) {
            return index;
        }
        index++;
    }
    return 0;
}
