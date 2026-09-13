#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEIconScrollerMenu.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEPackageData.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/ScriptHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Misc/Point.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

static const char *gTUTORIAL_MOVIE_DRAG = "drag_tutorial";
static const char *gTUTORIAL_MOVIE_SPEEDTRAP = "speedtrap_tutorial";
static const char *gTUTORIAL_MOVIE_TOLLBOOTH = "tollbooth_tutorial";

IconOption::IconOption(uint32 tex_hash, uint32 name_hash, uint32 desc_hash)
    : XPos(0.0f), YPos(0.0f), NameHash(name_hash), DescHash(desc_hash), fScaleAtStart(1.0f), pTutorialMovieName(nullptr), Item(tex_hash),
      FEngObject(nullptr), IsGreyOut(false), IsFlashable(true), fScaleToPcnt(1.0f), fScaleStartSecs(1.0f), fScaleDurSecs(1.0f), bAnimComplete(true),
      bReactImmediately(false), bIsTutorialAvailable(false) {
    if (tex_hash == 0xAAAB31E9) {
        SetTutorialMovieName(gTUTORIAL_MOVIE_DRAG);
    } else if (tex_hash == 0x66C9A7B6) {
        SetTutorialMovieName(gTUTORIAL_MOVIE_SPEEDTRAP);
    }
}

void IconOption::SetFEngObject(FEObject *obj) {
    if (obj != nullptr) {
        FEngObject = obj;
        FEngGetSize(obj, OrigWidth, OrigHeight);
        OriginalColor = FEngGetColor(obj);
    }
}

void IconOption::StartScale(float scale_to, float duration) {
    fScaleToPcnt = scale_to;
    fScaleDurSecs = duration;
    fScaleStartSecs = RealTimer.GetSeconds();
    bAnimComplete = false;
}

IconPanel::IconPanel(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region, bool wrap)
    : pPackageName(pkg_name), pButtonName(fe_button), bWrap(wrap), pCurrentNode(nullptr), pMaster(nullptr), fIconSpacing(10.0f), bReactToInput(true),
      iIndexToAdd(1), bHorizontal(true), bJustScrolled(true) {
    pMaster = FEngFindObject(pkg_name, FEHash(master));
    pScrollRegion = FEngFindObject(pPackageName, FEHash(scroll_region));
}

FEImage *IconPanel::AddOption(IconOption *option) {
    char sztemp[32];
    char sztemp2[32];

    bStrCat(sztemp, pButtonName, "%d");
    FEngSNPrintf(sztemp2, 32, sztemp, iIndexToAdd);
    FEImage *obj = FEngFindImage(pPackageName, sztemp2);
    if (obj == nullptr) {
        return nullptr;
    }
    iIndexToAdd++;
    if (option == nullptr) {
        return nullptr;
    }
    option->SetFEngObject(obj);
    Options.AddTail(option);
    if (pCurrentNode == nullptr) {
        pCurrentNode = Options.GetHead();
        FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject);
    }
    return obj;
}

void IconPanel::Act(uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if ((pCurrentNode != nullptr) && obj == pCurrentNode->FEngObject) {
        pCurrentNode->React(pPackageName, data, obj, param1, param2);
    }
}

IconOption *IconPanel::GetOption(int to_find) {
    if (to_find < 1) {
        return nullptr;
    }

    int index = 1;
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        if (to_find == index) {
            return opt;
        }
        index++;
    }
    return nullptr;
}

int IconPanel::GetOptionIndex(IconOption *to_find) {
    if (to_find == nullptr) {
        return -1;
    }

    int index = 1;
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        if (opt == to_find) {
            return index;
        }
        index++;
    }
    return -1;
}

bool IconPanel::SetSelection(IconOption *option) {
    if (!option->IsGreyOut) {
        pCurrentNode->StartScale(0.614f, 0.2f);
        pCurrentNode = option;
        FEngSetCurrentButton(pPackageName, option->FEngObject);
        pCurrentNode->StartScale(0.95f, 0.2f);
        return true;
    }
    return false;
}

void IconPanel::SetInitialPos() {
    float num_opts = static_cast<float>(Options.CountElements());
    float size_x = Options.GetHead()->FEngObject->GetObjData()->Size.x;
    float size_y = Options.GetHead()->FEngObject->GetObjData()->Size.y;

    float master_x = 0.0f;
    float master_y = 0.0f;
    FEngGetCenter(pMaster, master_x, master_y);
    float first_x = master_x - (size_x * num_opts + fIconSpacing * (num_opts - 1.0f)) * 0.5f;
    float first_y = master_y - (size_y * num_opts + fIconSpacing * (num_opts - 1.0f)) * 0.5f;
    float i = 0.0f;
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        if (bHorizontal) {
            FEngSetTopLeft(opt->FEngObject, (size_x + fIconSpacing) * i + first_x, master_y - size_y * 0.5f);
        } else {
            FEngSetTopLeft(opt->FEngObject, master_x - size_y * 0.5f, (size_y + fIconSpacing) * i + first_y);
        }
        i += 1.0f;
    }
    SetSelection(pCurrentNode);
}

void IconPanel::Scroll(eScrollDir dir) {
    if (Options.CountElements() == 0) {
        return;
    }
    IconOption *new_option = pCurrentNode;
    if (dir == eSD_PREV) {
        if (new_option != Options.GetHead()) {
            do {
                new_option = new_option->GetPrev();
            } while (new_option->IsGreyOut && new_option != Options.GetHead());
        }
    } else if (dir == eSD_NEXT) {
        if (new_option != Options.GetTail()) {
            do {
                new_option = new_option->GetNext();
            } while (new_option->IsGreyOut && new_option != Options.GetTail());
        }
    }

    if (new_option->IsGreyOut) {
        return;
    }

    if (new_option != pCurrentNode) {
        SetSelection(new_option);
        bJustScrolled = true;
    }
}

void IconPanel::ScrollWrapped(eScrollDir dir) {
    if (Options.CountElements() == 0) {
        return;
    }
    IconOption *new_option = pCurrentNode;
    if (dir == eSD_PREV) {
        do {
            if (new_option == Options.GetHead()) {
                new_option = Options.GetTail();
            } else {
                new_option = new_option->GetPrev();
            }
        } while (new_option->IsGreyOut);
    } else if (dir == eSD_NEXT) {
        do {
            if (new_option == Options.GetTail()) {
                new_option = Options.GetHead();
            } else {
                new_option = new_option->GetNext();
            }
        } while (new_option->IsGreyOut);
    }
    if (!new_option->IsGreyOut && new_option != pCurrentNode) {
        SetSelection(new_option);
        bJustScrolled = true;
    }
}

void IconPanel::Update() {
    AnimateList();
}

void IconPanel::AnimateList() {
    float list_width = 0.0f;
    float list_height = 0.0f;
    AnimateSelected(list_width, list_height);
}

// UNSOLVED
void IconPanel::AnimateSelected(float &list_width, float &list_height) {
    bJustScrolled = false;
    list_width = 0.0f;
    list_height = 0.0f;
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        float scale = 1.0f;
        if (!opt->IsAnimComplete()) {
            float pcnt_complete = (RealTimer.GetSeconds() - opt->GetScaleStartSecs()) / opt->GetScaleDurSecs();
            float delta_scale = opt->GetScaleToPcnt() - opt->GetScaleAtStart();
            scale = pcnt_complete * delta_scale + opt->GetScaleAtStart();
            if (delta_scale < 0.0f) {
                if (scale <= opt->GetScaleToPcnt()) {
                    FEngSetSize(opt->FEngObject, opt->OrigWidth * opt->GetScaleToPcnt(), opt->OrigHeight * opt->GetScaleToPcnt());
                }
            } else if (scale >= opt->GetScaleToPcnt()) {
                if (delta_scale < 0.0f) {
                    if (scale <= opt->GetScaleToPcnt()) {
                        FEngSetSize(opt->FEngObject, opt->OrigWidth * opt->GetScaleToPcnt(), opt->OrigHeight * opt->GetScaleToPcnt());
                    }
                } else {
                    FEngSetSize(opt->FEngObject, opt->OrigWidth * opt->GetScaleToPcnt(), opt->OrigHeight * opt->GetScaleToPcnt());
                }
            } else {
                FEngSetSize(opt->FEngObject, opt->OrigWidth * scale, opt->OrigHeight * scale);
                bJustScrolled = true;
            }
        }
        opt->SetScaleAtStart(scale);
        opt->SetAnimComplete(true);

        list_width = opt->OrigWidth * scale + list_width;
        list_height = opt->OrigHeight * scale + list_height;
        if (opt != Options.GetTail()) {
            list_width += fIconSpacing;
            list_height += fIconSpacing;
        }
    }
}

// UNSOLVED
IconScroller::IconScroller(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region, float width)
    : IconPanel(pkg_name, master, fe_button, scroll_region, false), //
      ScrollBar(pkg_name, "ScrollBar", false, false, true),         //
      AnimateCubic(0, 1.0f), HeadBookEnd(nullptr), TailBookEnd(nullptr), AlignmentToSelected(eSA_MIDDLE), iNumBookEnds(4), fCurFadeTime(0.0f),
      fMaxFadeTime(9.0f), IdleColor(0xFFFFFFFF), FadeColor(0x00FFFFFF), fWidth(width), fHeight(0.0f), fXCenter(0.0f), fYCenter(0.0f),
      fCurrentAddPos(0.0f), iCurSelectedIndex(1), bFadingIn(false), bFadingOut(false), bInitialized(false), bAllowColorAnim(true),
      bDelayUpdate(false) {

    fIconSpacing = -5.0f;

    FEObject *master_obj = FEngFindObject(pkg_name, FEHashUpper(scroll_region));
    if (master_obj != nullptr) {
        FEngGetCenter(master_obj, fXCenter, fYCenter);
        FEngSetInvisible(master_obj);
    }
    AddInitialBookEnds();
    AnimateCubic.SetDuration(0.2f);
    AnimateCubic.SetFlags(0);
}

void IconScroller::Update() {
    if (!Options.IsEmpty() && (pCurrentNode != nullptr) && !bDelayUpdate) {
        if (bJustScrolled) {
            bJustScrolled = false;
            ScrollBar.Update(1, iIndexToAdd - 1 - iNumBookEnds, iCurSelectedIndex - iNumBookEnds, iCurSelectedIndex - iNumBookEnds);
            AnimateCubic.SetValDesired(-pCurrentNode->XPos);
            UpdateArrows();
        }

        for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
            PositionOption(opt);
        }

        if (bFadingIn) {
            fCurFadeTime += 1.0f;
            if (fCurFadeTime >= fMaxFadeTime) {
                fCurFadeTime = fMaxFadeTime;
                bFadingIn = false;
            }
        } else if (bFadingOut) {
            fCurFadeTime -= 1.0f;
            if (fCurFadeTime <= 0.0f) {
                fCurFadeTime = 0.0f;
            }
        }

        cPoint::SplineSeek(&AnimateCubic, RealTimeElapsed, 0.0f, 0.0f);
    }
}

void IconScroller::AddInitialBookEnds() {
    for (int i = 0; i < iNumBookEnds / 2; i++) {
        FEScrollyBookEnd *option = new ("FEScrollyBookEnd", 0) FEScrollyBookEnd(STRINGHASH_END_OF_SCROLLER);
        FEImage *img = AddOption(option);
        if (img != nullptr) {
            FEngSetTextureHash(img, option->Item);
        }
    }
    HeadBookEnd = Options.GetTail();
}

FEImage *IconScroller::AddOption(IconOption *option) {
    char sztemp[32];
    FEngSNPrintf(sztemp, 32, "%s%d", pButtonName, iIndexToAdd);
    FEImage *obj = FEngFindImage(pPackageName, sztemp);
    if (obj == nullptr) {
        if (option != nullptr) {
            delete option;
        }
        return nullptr;
    }

    if (option == nullptr) {
        return nullptr;
    }

    iIndexToAdd++;
    option->SetFEngObject(obj);
    option->XPos = this->fCurrentAddPos;
    option->OriginalColor = this->IdleColor;
    FEngGetSize(option->FEngObject, option->OrigWidth, option->OrigHeight);
    fCurrentAddPos += FEngGetSizeX(option->FEngObject) + fIconSpacing;
    Options.AddTail(option);
    if (pCurrentNode == nullptr) {
        if (iIndexToAdd > iNumBookEnds + 1) {
            pCurrentNode = static_cast<IconOption *>(HeadBookEnd->GetNext());
            FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject);
        }
    }
    return obj;
}

void IconScroller::SetInitialPos(int index) {
    TailBookEnd = Options.GetTail();
    for (int i = 0; i < iNumBookEnds / 2; i++) {
        FEScrollyBookEnd *option = new ("FEScrollyBookEnd", 0) FEScrollyBookEnd(STRINGHASH_END_OF_SCROLLER);
        FEImage *img = AddOption(option);
        if (img != nullptr) {
            FEngSetTextureHash(img, option->Item);
        }
    }
    TailBookEnd = TailBookEnd->GetNext();

    if (index > 0) {
        index += iNumBookEnds / 2;
    }

    IconOption *option = Options.GetNode(index - 1);
    if (index != 0 && (option != nullptr)) {
        if (option->Item == STRINGHASH_END_OF_SCROLLER) {
            SetSelection(TailBookEnd->GetPrev());
        } else {
            SetSelection(option);
        }
    } else {
        SetSelection(HeadBookEnd->GetNext());
    }

    if (bHorizontal) {
        AnimateCubic.SetValDesired(-pCurrentNode->XPos);
    } else {
        AnimateCubic.SetValDesired(-pCurrentNode->YPos);
    }
    AnimateCubic.Snap();

    if (!bDelayUpdate) {
        for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
            PositionOption(opt);
        }
    }

    bInitialized = true;
}

// UNSOLVED regswap
bool IconScroller::SetSelection(IconOption *option) {
    int index = this->GetOptionIndex(option);

    if (!(0 <= index && index < this->iIndexToAdd)) {
        return false;
    }

    for (IconOption *opt = this->Options.GetHead(); opt != this->Options.EndOfList(); opt = opt->GetNext()) {
        FEngSetButtonState(this->pPackageName, pMaster->NameHash, false);
    }

    if (!option->IsGreyOut) {
        this->pCurrentNode = option;
        FEngSetButtonState(this->pPackageName, option->FEngObject->NameHash, true);
        FEngSetCurrentButton(this->pPackageName, pCurrentNode->FEngObject);
        this->iCurSelectedIndex = this->GetOptionIndex(option);
        return true;
    }

    return false;
}

void IconScroller::RemoveAll() {
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        FEngSetSize(opt->FEngObject, opt->OrigWidth, opt->OrigHeight);
        FEngSetTopLeft(opt->FEngObject, 696969.0f, 696969.0f);
    }

    IconPanel::RemoveAll();
    iIndexToAdd = 1;

    fCurrentAddPos = 0.0f;
}

int IconScroller::GetOptionIndex(IconOption *to_find) {
    if (to_find == nullptr) {
        return -1;
    }

    int index = 1;
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        if (opt == to_find) {
            return index - iNumBookEnds / 2;
        }
        index++;
    }
    return -1;
}

void IconScroller::Scroll(eScrollDir dir) {
    if (Options.CountElements() - iNumBookEnds < 1) {
        return;
    }

    IconOption *new_option = pCurrentNode;
    if (dir == eSD_PREV) {
        if (new_option != HeadBookEnd->GetNext()) {
            do {
                new_option = new_option->GetPrev();
            } while (new_option->IsGreyOut && new_option != HeadBookEnd->GetNext());
        }
    } else if (dir == eSD_NEXT) {
        if (new_option != TailBookEnd->GetPrev()) {
            do {
                new_option = new_option->GetNext();
            } while (new_option->IsGreyOut && new_option != TailBookEnd->GetPrev());
        }
    }

    if (new_option->IsGreyOut) {
        return;
    }

    if (new_option != pCurrentNode) {
        SetSelection(new_option);
        bJustScrolled = true;
    }
}

void IconScroller::ScrollWrapped(eScrollDir dir) {
    if (Options.CountElements() - iNumBookEnds <= 0) {
        return;
    }

    IconOption *new_option = pCurrentNode;
    if (dir == eSD_PREV) {
        do {
            if (new_option == HeadBookEnd->GetNext()) {
                new_option = TailBookEnd->GetPrev();
            } else {
                new_option = new_option->GetPrev();
            }
        } while (new_option->IsGreyOut);
    } else if (dir == eSD_NEXT) {
        do {
            if (new_option == TailBookEnd->GetPrev()) {
                new_option = HeadBookEnd->GetNext();
            } else {
                new_option = new_option->GetNext();
            }
        } while (new_option->IsGreyOut);
    }
    if (!new_option->IsGreyOut && new_option != pCurrentNode) {
        SetSelection(new_option);
        bJustScrolled = true;
    }
}

void IconScroller::ClipEdges(IconOption *option, float pos) {
    if (pos < fXCenter - fWidth * 0.5f || pos > fXCenter + fWidth * 0.5f) {
        FEngSetInvisible(option->FEngObject);
    } else {
        FEngSetVisible(option->FEngObject);
    }
}

float IconScroller::Scale(float x, float center, float scroll_size, float thumb_size) {
    float pos_far_clip = center + scroll_size * 0.5f;
    float neg_far_clip = center - scroll_size * 0.5f;

    if (x < neg_far_clip || x > pos_far_clip) {
        return 0.0f;
    }
    if (x >= neg_far_clip && x < center - 1.5f) {
        return (x - neg_far_clip) / (scroll_size * 0.5f);
    }
    if (x <= pos_far_clip && x > center + 1.5f) {
        return (pos_far_clip - x) / (scroll_size * 0.5f);
    }
    return 1.0f;
}

void IconScroller::PositionOption(IconOption *option) {
    if (option == nullptr) {
        return;
    }

    float xpos = fXCenter + (AnimateCubic.Val + option->XPos);
    FEngSetSize(option->FEngObject, option->OrigWidth, option->OrigHeight);
    float scale = Scale(xpos, fXCenter, fWidth, option->OrigWidth);

    if (xpos < fXCenter) {
        xpos += option->OrigWidth * (1.0f - scale) * (1.0f - scale) * (1.0f - scale);
    } else {
        xpos -= option->OrigWidth * (1.0f - scale) * (1.0f - scale) * (1.0f - scale);
    }

    ClipEdges(option, xpos);
    FEngSetCenter(option->FEngObject, xpos, fYCenter);

    if (bFadingIn || bFadingOut) {
        scale *= fCurFadeTime / fMaxFadeTime;
    }

    float aligned_pos = 0.0f;
    switch (AlignmentToSelected) {
        case eSA_LEFT:
            aligned_pos = FEngGetTopLeftY(option->FEngObject);
            break;
        case eSA_MIDDLE:
            aligned_pos = (option->OrigHeight - option->OrigHeight * scale) * 0.5f + FEngGetTopLeftY(option->FEngObject);
            break;
        case eSA_BOTTOM:
            aligned_pos = FEngGetTopLeftY(option->FEngObject) + (option->OrigHeight - option->OrigHeight * scale);
            break;
    }

    FEngSetSize(option->FEngObject, option->OrigWidth * scale, option->OrigHeight * scale);
    FEngSetTopLeftY(option->FEngObject, aligned_pos);

    if (bAllowColorAnim) {
        UpdateFade(option, scale);
    }
}

void IconScroller::UpdateFade(IconOption *option, float scale) {
    if (option != nullptr && option->FEngObject != nullptr && option->FEngObject->GetObjData() != nullptr) {
        float a1 = static_cast<float>(IdleColor >> 24);
        float r1 = static_cast<float>(IdleColor >> 16 & 0xFF);
        float g1 = static_cast<float>(IdleColor >> 8 & 0xFF);
        float b1 = static_cast<float>(IdleColor & 0xFF);
        float a2 = static_cast<float>(FadeColor >> 24);
        float r2 = static_cast<float>(FadeColor >> 16 & 0xFF);
        float g2 = static_cast<float>(FadeColor >> 8 & 0xFF);
        float b2 = static_cast<float>(FadeColor & 0xFF);
        uint8 a = static_cast<int>(a1 * scale + a2 * (1.0f - scale)) & 0xFF;

        if (option->IsGreyOut) {
            a = 150;
        } else {
            a = bClamp(a, 0, 0xFF);
        }

        uint8 r = static_cast<int>(r1 * scale + r2 * (1.0f - scale)) & 0xFF;
        uint8 g = static_cast<int>(g1 * scale + g2 * (1.0f - scale)) & 0xFF;
        uint8 b = static_cast<int>(b1 * scale + b2 * (1.0f - scale)) & 0xFF;
        r = bClamp(r, 0, 0xFF);
        g = bClamp(g, 0, 0xFF);
        b = bClamp(b, 0, 0xFF);
        uint32 color = a * 0x1000000 + r * 0x10000 + g * 0x100 + b;
        FEngSetColor(option->FEngObject, color);
    }
}

void IconScroller::UpdateArrows() {
    if (pCurrentNode == Options.GetHead()) {
        ScrollBar.SetArrow1Visibility(false);
    } else if (pCurrentNode == Options.GetTail()) {
        ScrollBar.SetArrow2Visibility(false);
    } else {
        ScrollBar.SetArrow1Visibility(true);
        ScrollBar.SetArrow2Visibility(true);
    }
}

IconScrollerMenu::IconScrollerMenu(ScreenConstructorData *sd)
    : MenuScreen(sd), //
      Options(GetPackageName(), "OPTION_MASTER", "option_", "ICON_SCROLL_REGION", 350.0f), bWasLeftMouseDown(false), bFadeInIconsImmediately(true),
      pOptionName(nullptr), pOptionNameShadow(nullptr), pOptionDesc(nullptr), PrevButtonMessage(0), PrevButtonObj(nullptr), PrevParam1(0),
      PrevParam2(0) {
    const u32 FEObj_ICONTITLE = 0x5E7B09C9;
    const u32 FEObj_ICONTITLESHADOW = 0x0DFB7A2E;

    pOptionName = FEngFindString(GetPackageName(), FEObj_ICONTITLE);
    pOptionNameShadow = FEngFindString(GetPackageName(), FEObj_ICONTITLESHADOW);
    pOptionDesc = FEngFindString(GetPackageName(), 0);
    mPlaySound = false;
}

void IconScrollerMenu::NotificationMessage(u32 msg, FEObject *pobj, u32 param1, u32 param2) {
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            Options.Update();
            return;
        case FEMSG_EXIT_STARTED:
            Options.StartFadeOut();
            return;
        case FEHASH_INITCOMPLETE:
            Options.SetAllowFade(true);
            return;
        case FEHASH_EXITCOMPLETE:
            Options.IconPanel::Act(PrevButtonMessage, PrevButtonObj, PrevParam1, PrevParam2);
            return;
        case __PAD_BACK__:
            StorePrevNotification(msg, pobj, param1, param2);
            Options.SetReactToInput(false);
            FEngSetLastButton(GetPackageName(), 0);
            return;
        case __BUTTON_PRESSED__:
            if (!Options.ReactsToInput()) {
                return;
            }

            if (Options.GetCurrentOption()->IsGreyOut) {
                return;
            }
            if (pobj != Options.GetCurrentOption()->FEngObject) {
                return;
            }

            FEngSetLastButton(GetPackageName(), Options.GetCurrentIndex());

            if (Options.CurrentReactsImmediately()) {
                Options.IconPanel::Act(msg, pobj, param1, param2);
                return;
            }

            StorePrevNotification(msg, pobj, param1, param2);
            Options.SetReactToInput(false);
            cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
            return;
        case __PAD_LEFT__:
            if (!Options.IsHorizontal()) {
                return;
            }
            if (!Options.ReactsToInput()) {
                return;
            }

            Options.ScrollPrev();

            RefreshHeader();
            return;
        case __PAD_RIGHT__:
            if (!Options.IsHorizontal()) {
                return;
            }
            if (!Options.ReactsToInput()) {
                return;
            }

            Options.ScrollNext();

            RefreshHeader();
            return;
        case __PAD_UP__: {
            if (Options.IsHorizontal()) {
                return;
            }
            if (!Options.ReactsToInput()) {
                return;
            }

            if (!Options.AtHead()) {
                Options.ScrollPrev();
            }
            RefreshHeader();
            return;
        }
        case __PAD_DOWN__: {
            if (Options.IsHorizontal()) {
                return;
            }
            if (!Options.ReactsToInput()) {
                return;
            }

            Options.ScrollNext();
            RefreshHeader();
            return;
        }
        case __PAD_BUTTON4__: {
            IconOption *cur_option = Options.GetCurrentOption();
            if (cur_option->IsTutorialAvailable()) {
                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = FEHASH_HIDE;
                FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_HIDE, true);
                g_pEAXSound->PlayUISoundFX(UISND_COMMON_SELECT);
                FEAnyTutorialScreen::LaunchMovie(cur_option->GetTutorialMovieName(), GetPackageName());

                UserProfile *prof = FEDatabase->GetMultiplayerProfile(0);
                CareerSettings *career = prof->GetCareer();

                switch (cur_option->GetName()) {
                    case 0x6F547E4C:
                        career->SetHasDoneDragTutorial();
                        break;
                    case 0xEE1EDC76:
                        career->SetHasDoneSpeedTrapTutorial();
                        break;
                    case 0xA15E4505:
                        career->SetHasDoneTollBoothTutorial();
                        break;
                }
            }
            return;
        }
        case FEMSG_MOVIE_FINISHED: {
            const u32 FEObj_MASTERBLASTER = 0x99344537;
            const u32 FEObj_Init = FEHASH_INIT;
            FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_Init, true);
            return;
        }
    }
}

eMenuSoundTriggers IconScrollerMenu::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if ((msg == FEHASH_SOUND_LEFT || msg == FEHASH_SOUND_RIGHT) && !Options.JustScrolled()) {
        return UISND_NONE;
    }
    return maybe;
}

void IconScrollerMenu::StorePrevNotification(uint32 msg, FEObject *pobj, uint32 param1, uint32 param2) {
    PrevButtonMessage = msg;
    PrevButtonObj = pobj;
    PrevParam1 = param1;
    PrevParam2 = param2;
}

void IconScrollerMenu::RefreshHeader() {
    FEngSetLanguageHash(pOptionName, Options.GetCurrentName());
    FEngSetLanguageHash(pOptionNameShadow, Options.GetCurrentName());
    FEngSetLanguageHash(pOptionDesc, Options.GetCurrentDesc());

    if (Options.AtHead()) {
        const u32 FEObj_ENDPADLEFT = 0xD7118934;
        cFEng::Get()->QueuePackageMessage(FEObj_ENDPADLEFT, GetPackageName(), nullptr);
    }

    if (Options.AtTail()) {
        const u32 FEObj_ENDPADRIGHT = 0xB9B17747;
        cFEng::Get()->QueuePackageMessage(FEObj_ENDPADRIGHT, GetPackageName(), nullptr);
    }

    const u32 FEObj_TUTORIALGROUP = 0x9C7D33FF;
    if (Options.GetCurrentOption()->IsTutorialAvailable()) {
        FEngSetScript(GetPackageName(), FEObj_TUTORIALGROUP, FEHASH_SHOW, true);
    } else {
        FEngSetScript(GetPackageName(), FEObj_TUTORIALGROUP, FEHASH_HIDE, true);
    }
}

void IconScrollerMenu::AddOption(IconOption *option) {
    FEImage *img = Options.AddOption(option);
    FEngSetTextureHash(img, option->Item);
}
