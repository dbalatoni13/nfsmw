#include "IconPanel.hpp"
#include "IconScroller.hpp"
#include "IconScrollerMenu.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

extern void FEngSetTextureHash(FEImage *image, unsigned int hash);
extern void FEngSetCurrentButton(const char *pkg_name, unsigned int hash);
extern void FEngGetCenter(FEObject *object, float &x, float &y);
extern void FEngSetCenter(FEObject *object, float x, float y);
extern void FEngSetLanguageHash(FEString *text, unsigned int hash);
extern unsigned long FEHash(const char *str);
extern FEColor FEngGetObjectColor(FEObject *object);
extern void FEngSetColor(FEObject *obj, unsigned int color);
extern void FEngSetLastButton(const char *pkg_name, unsigned char button_hash);
extern void FEngSetScript(const char *pkg_name, unsigned int obj_hash, unsigned int script_hash, bool unk);
extern Timer RealTimer;
extern float RealTimeElapsed;
extern char *bStrCat(char *dest, const char *str1, const char *str2);
extern FEString *FEngFindString(const char *pkg_name, int hash);

struct tCubic1D {
    float Val;
    float dVal;
    float ValDesired;
    float dValDesired;
    float Coeff[4];
    float time;
    float duration;
    short state;
    short flags;

    void Snap();
    void SetValDesired(float v);
};

struct cPoint {
    static void SplineSeek(tCubic1D *p, float time, float lower, float upper);
};

static const char *gTUTORIAL_MOVIE_DRAG = "TUT_DRAG";
static const char *gTUTORIAL_MOVIE_SPEEDTRAP = "TUT_SPEEDTRAP";
static const char *gTUTORIAL_MOVIE_TOLLBOOTH = "TUT_TOLLBOOTH";

// ============================================================
// IconOption
// ============================================================

IconOption::IconOption(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash) {
    YPos = 0.0f;
    NameHash = name_hash;
    DescHash = desc_hash;
    fScaleAtStart = 1.0f;
    pTutorialMovieName = nullptr;
    Item = tex_hash;
    FEngObject = nullptr;
    XPos = 0.0f;
    IsGreyOut = false;
    IsFlashable = true;
    fScaleToPcnt = 1.0f;
    fScaleStartSecs = 1.0f;
    fScaleDurSecs = 1.0f;
    bAnimComplete = true;
    bReactImmediately = false;
    bIsTutorialAvailable = false;

    if (tex_hash == 0xAAAB31E9) {
        bIsTutorialAvailable = true;
        pTutorialMovieName = gTUTORIAL_MOVIE_DRAG;
    } else if (tex_hash == 0x66C9A7B6) {
        bIsTutorialAvailable = true;
        pTutorialMovieName = gTUTORIAL_MOVIE_SPEEDTRAP;
    }
}

void IconOption::SetFEngObject(FEObject *obj) {
    if (obj) {
        FEngObject = obj;
        FEngGetSize(obj, OrigWidth, OrigHeight);
        OriginalColor = FEngGetColor(obj);
    }
}

void IconOption::StartScale(float scale_to, float duration) {
    fScaleToPcnt = scale_to;
    fScaleDurSecs = duration;
    bAnimComplete = false;
    fScaleStartSecs = RealTimer.GetSeconds();
}

unsigned int IconOption::GetDesc() { return DescHash; }
float IconOption::GetScaleToPcnt() { return fScaleToPcnt; }
float IconOption::GetScaleStartSecs() { return fScaleStartSecs; }
float IconOption::GetScaleDurSecs() { return fScaleDurSecs; }
float IconOption::GetScaleAtStart() { return fScaleAtStart; }
void IconOption::SetScaleAtStart(float scale) { fScaleAtStart = scale; }
bool IconOption::IsAnimComplete() { return bAnimComplete; }
void IconOption::SetAnimComplete(bool b) { bAnimComplete = b; }
bool IconOption::IsLocked() { return Locked; }
void IconOption::SetLocked(bool b) { Locked = b; }
void IconOption::SetTutorialMovieName(const char *name) { pTutorialMovieName = name; }

// ============================================================
// IconPanel
// ============================================================

IconPanel::IconPanel(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region, bool wrap) {
    pPackageName = pkg_name;
    pButtonName = fe_button;
    bWrap = wrap;
    pCurrentNode = nullptr;
    fIconSpacing = 10.0f;
    bReactToInput = true;
    iIndexToAdd = 1;
    bHorizontal = true;
    bJustScrolled = true;
    pMaster = static_cast<FEObject *>(FEngFindObject(pkg_name, FEHash(master)));
    pScrollRegion = static_cast<FEObject *>(FEngFindObject(pkg_name, FEHash(scroll_region)));
}

FEImage *IconPanel::AddOption(IconOption *option) {
    char sztemp[32];
    char sztemp2[32];

    bStrCat(sztemp, pButtonName, "%d");
    FEngSNPrintf(sztemp2, 32, sztemp, iIndexToAdd);
    FEImage *obj = FEngFindImage(pPackageName, FEHashUpper(sztemp2));
    if (!obj) {
        return nullptr;
    }
    iIndexToAdd++;
    if (!option) {
        return nullptr;
    }
    option->SetFEngObject(obj);
    Options.AddTail(option);
    if (!pCurrentNode) {
        pCurrentNode = Options.GetHead();
        FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject->NameHash);
    }
    return obj;
}

void IconPanel::Act(unsigned int data, FEObject *obj, unsigned int param1, unsigned int param2) {
    if (pCurrentNode && obj == pCurrentNode->FEngObject) {
        pCurrentNode->React(pPackageName, data, obj, param1, param2);
    }
}

bool IconPanel::SetSelection(IconOption *option) {
    if (option->IsGreyOut) {
        return false;
    }
    pCurrentNode->StartScale(0.614f, 0.2f);
    pCurrentNode = option;
    FEngSetCurrentButton(pPackageName, option->FEngObject->NameHash);
    pCurrentNode->StartScale(0.95f, 0.2f);
    return true;
}

void IconPanel::SetInitialPos() {
    float num_opts = static_cast<float>(Options.CountElements());
    float size_x, size_y;
    FEngGetSize(Options.GetHead()->FEngObject, size_x, size_y);
    float master_x;
    float master_y;
    FEngGetCenter(pScrollRegion, master_x, master_y);
    float first_x = master_x - (size_x * num_opts + fIconSpacing * (num_opts - 1.0f)) * 0.5f;
    float first_y = master_y - (size_y * num_opts + fIconSpacing * (num_opts - 1.0f)) * 0.5f;
    float i = 0.0f;
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        if (!bHorizontal) {
            FEngSetTopLeft(opt->FEngObject, master_x - size_y * 0.5f, (size_y + fIconSpacing) * i + first_y);
        } else {
            FEngSetTopLeft(opt->FEngObject, (size_x + fIconSpacing) * i + first_x, master_y - size_y * 0.5f);
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
        do {
            if (new_option == Options.GetHead()) {
                goto check;
            }
            new_option = new_option->GetPrev();
        } while (new_option->IsGreyOut);
    } else if (dir == eSD_NEXT) {
        do {
            if (new_option == Options.GetTail()) {
                goto check;
            }
            new_option = new_option->GetNext();
        } while (new_option->IsGreyOut);
    } else {
check:
        if (new_option->IsGreyOut) {
            return;
        }
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
                    opt->SetScaleAtStart(scale);
                    opt->SetAnimComplete(true);
                    goto next;
                }
            } else if (scale >= opt->GetScaleToPcnt()) {
                if (delta_scale < 0.0f) {
                    if (scale <= opt->GetScaleToPcnt()) {
                        FEngSetSize(opt->FEngObject, opt->OrigWidth * opt->GetScaleToPcnt(), opt->OrigHeight * opt->GetScaleToPcnt());
                        opt->SetScaleAtStart(scale);
                        opt->SetAnimComplete(true);
                        goto next;
                    }
                } else {
                    FEngSetSize(opt->FEngObject, opt->OrigWidth * opt->GetScaleToPcnt(), opt->OrigHeight * opt->GetScaleToPcnt());
                    opt->SetScaleAtStart(scale);
                    opt->SetAnimComplete(true);
                    goto next;
                }
            }
            FEngSetSize(opt->FEngObject, opt->OrigWidth * scale, opt->OrigHeight * scale);
            bJustScrolled = true;
        }
next:
        list_width = opt->OrigWidth * scale + list_width;
        list_height = opt->OrigHeight * scale + list_height;
        if (opt != Options.GetTail()) {
            list_width += fIconSpacing;
            list_height += fIconSpacing;
        }
    }
}

void IconPanel::RemoveAll() {
    while (Options.GetHead() != Options.EndOfList()) {
        IconOption *node = Options.GetHead();
        node->Remove();
        delete node;
    }
    iIndexToAdd = 1;
}

// ============================================================
// IconScroller
// ============================================================

IconScroller::IconScroller(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region, float width)
    : IconPanel(pkg_name, master, fe_button, scroll_region, false) //
    , ScrollBar(pkg_name, "ScrollBar", false, false, true) //
{
    HeadBookEnd = nullptr;
    TailBookEnd = nullptr;
    AlignmentToSelected = static_cast<eScrollerAlignment>(1);
    iNumBookEnds = 4;
    fCurFadeTime = 0.0f;
    fMaxFadeTime = 9.0f;
    bAllowColorAnim = true;
    IdleColor = 0xFFFFFFFF;
    FadeColor = 0x00FFFFFF;
    fWidth = 0.0f;
    fHeight = 0.0f;
    fXCenter = 0.0f;
    fYCenter = 0.0f;
    fPulseState = 0.0f;
    fCurrentAddPos = 0.0f;
    AlignmentToSelected = static_cast<eScrollerAlignment>(1);
    iCurSelectedIndex = 1;
    fWidth = width;
    bFadingIn = false;
    bFadingOut = false;
    bInitialized = false;
    bDelayUpdate = false;
    fIconSpacing = -5.0f;

    FEObject *scroll_obj = FEngFindObject(pkg_name, FEHashUpper(scroll_region));
    if (scroll_obj) {
        FEngGetCenter(scroll_obj, fXCenter, fYCenter);
        FEngSetInvisible(scroll_obj);
    }
    AddInitialBookEnds();
    AlignmentToSelected = static_cast<eScrollerAlignment>(0);
    iNumBookEnds = 0;
}

void IconScroller::Update() {
    if (!Options.IsEmpty() && pCurrentNode && !bDelayUpdate) {
        if (bJustScrolled) {
            bJustScrolled = false;
            ScrollBar.Update(1, iIndexToAdd - (iNumBookEnds + 1), iCurSelectedIndex - iNumBookEnds,
                             iCurSelectedIndex - iNumBookEnds);
            reinterpret_cast<tCubic1D *>(AnimateCubicData)->SetValDesired(-pCurrentNode->XPos);
            if (-pCurrentNode->XPos != reinterpret_cast<tCubic1D *>(AnimateCubicData)->Val) {
                reinterpret_cast<tCubic1D *>(AnimateCubicData)->state = 2;
            }
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

        cPoint::SplineSeek(reinterpret_cast<tCubic1D *>(AnimateCubicData), RealTimeElapsed, 0.0f, 0.0f);
    }
}

void IconScroller::AddInitialBookEnds() {
    for (int i = 0; i < iNumBookEnds / 2; i++) {
        FEScrollyBookEnd *bookend = new FEScrollyBookEnd(0x43B6310F);
        FEImage *img = AddOption(bookend);
        if (img) {
            FEngSetTextureHash(img, bookend->Item);
        }
    }
    HeadBookEnd = Options.GetTail();
}

FEImage *IconScroller::AddOption(IconOption *option) {
    char sztemp[32];
    FEngSNPrintf(sztemp, 32, "%s%d", pButtonName, iIndexToAdd);
    FEImage *obj = FEngFindImage(pPackageName, FEHashUpper(sztemp));
    if (!obj) {
        if (option) {
            delete option;
        }
    } else if (option) {
        iIndexToAdd++;
        option->SetFEngObject(obj);
        option->XPos = fCurrentAddPos;
        option->OriginalColor = IdleColor;
        float size_w, size_h;
        FEngGetSize(obj, option->OrigWidth, option->OrigHeight);
        FEngGetSize(obj, size_w, size_h);
        fCurrentAddPos += size_w + fIconSpacing;
        Options.AddTail(option);
        if (!pCurrentNode) {
            if (iIndexToAdd > iNumBookEnds + 1) {
                pCurrentNode = static_cast<IconOption *>(HeadBookEnd->GetNext());
                FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject->NameHash);
            }
        }
        return obj;
    }
    return nullptr;
}

void IconScroller::SetInitialPos(int index) {
    TailBookEnd = Options.GetTail();
    for (int i = 0; i < iNumBookEnds / 2; i++) {
        FEScrollyBookEnd *option = new(__FILE__, __LINE__) FEScrollyBookEnd(0x43B6310F);
        FEImage *img = AddOption(option);
        if (img) {
            FEngSetTextureHash(img, option->Item);
        }
    }
    TailBookEnd = TailBookEnd->GetNext();

    if (index > 0) {
        index += iNumBookEnds / 2;
    }

    IconOption *option = Options.GetNode(index - 1);
    if (index == 0 || !option) {
        SetSelection(static_cast<IconOption *>(HeadBookEnd->GetNext()));
    } else {
        if (option->Item == 0x43B6310F) {
            option = static_cast<IconOption *>(TailBookEnd->GetPrev());
        }
        SetSelection(option);
    }

    if (!bHorizontal) {
        reinterpret_cast<tCubic1D *>(AnimateCubicData)->SetValDesired(-pCurrentNode->YPos);
    } else {
        reinterpret_cast<tCubic1D *>(AnimateCubicData)->SetValDesired(-pCurrentNode->XPos);
    }
    reinterpret_cast<tCubic1D *>(AnimateCubicData)->Snap();

    if (!bDelayUpdate) {
        for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
            PositionOption(opt);
        }
    }

    bInitialized = true;
}

bool IconScroller::SetSelection(IconOption *option) {
    if (option->IsGreyOut) {
        return false;
    }
    pCurrentNode->StartScale(0.614f, 0.2f);
    pCurrentNode = option;
    FEngSetCurrentButton(pPackageName, option->FEngObject->NameHash);
    pCurrentNode->StartScale(0.95f, 0.2f);
    return true;
}

void IconScroller::RemoveAll() {
    for (IconOption *opt = Options.GetHead(); opt != Options.EndOfList(); opt = opt->GetNext()) {
        FEngSetSize(opt->FEngObject, opt->OrigWidth, opt->OrigHeight);
        FEngSetTopLeft(opt->FEngObject, 696969.0f, 696969.0f);
    }
    while (Options.GetHead() != Options.EndOfList()) {
        IconOption *node = Options.GetHead();
        node->Remove();
        delete node;
    }
    iIndexToAdd = 1;
    fCurrentAddPos = 0.0f;
}

int IconScroller::GetOptionIndex(IconOption *to_find) {
    if (!to_find) {
        return -1;
    }
    IconOption *node = static_cast<IconOption *>(HeadBookEnd->GetNext());
    int i = 1;
    while (node != TailBookEnd) {
        if (node == to_find) {
            return i;
        }
        i++;
        node = node->GetNext();
    }
    return -1;
}

void IconScroller::Scroll(eScrollDir dir) {
    if (Options.CountElements() - iNumBookEnds < 1) {
        return;
    }
    IconOption *new_option = pCurrentNode;
    if (dir == eSD_PREV) {
        if (new_option != static_cast<IconOption *>(HeadBookEnd->GetNext())) {
            do {
                new_option = new_option->GetPrev();
                if (!new_option->IsGreyOut) {
                    goto done;
                }
            } while (new_option != static_cast<IconOption *>(HeadBookEnd->GetNext()));
        }
    } else if (dir == eSD_NEXT) {
        do {
            if (new_option == static_cast<IconOption *>(TailBookEnd->GetPrev())) {
                goto check;
            }
            new_option = new_option->GetNext();
        } while (new_option->IsGreyOut);
        goto done;
    }
check:
    if (new_option->IsGreyOut) {
        return;
    }
done:
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
            if (new_option == static_cast<IconOption *>(HeadBookEnd->GetNext())) {
                new_option = static_cast<IconOption *>(TailBookEnd->GetPrev());
            } else {
                new_option = new_option->GetPrev();
            }
        } while (new_option->IsGreyOut);
    } else if (dir == eSD_NEXT) {
        do {
            if (new_option == static_cast<IconOption *>(TailBookEnd->GetPrev())) {
                new_option = static_cast<IconOption *>(HeadBookEnd->GetNext());
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
    float neg_far_clip = center - scroll_size * 0.5f;
    float pos_far_clip = center + scroll_size * 0.5f;

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
    if (option) {
        float xpos = fXCenter + reinterpret_cast<tCubic1D *>(AnimateCubicData)->Val + option->XPos;
        FEngSetSize(option->FEngObject, option->OrigWidth, option->OrigHeight);
        float scale = Scale(xpos, fXCenter, fWidth, option->OrigWidth);

        if (fXCenter <= xpos) {
            float aligned_pos = 1.0f - scale;
            xpos -= option->OrigWidth * aligned_pos * aligned_pos * aligned_pos;
        } else {
            float aligned_pos = 1.0f - scale;
            xpos += option->OrigWidth * aligned_pos * aligned_pos * aligned_pos;
        }

        ClipEdges(option, xpos);
        FEngSetCenter(option->FEngObject, xpos, fYCenter);

        if (bFadingIn || bFadingOut) {
            scale *= fCurFadeTime / fMaxFadeTime;
        }

        float aligned_pos = 0.0f;
        if (AlignmentToSelected == eSA_MIDDLE) {
            aligned_pos = (option->OrigHeight - option->OrigHeight * scale) * 0.5f + FEngGetTopLeftY(option->FEngObject);
        } else if (AlignmentToSelected == eSA_TOP) {
            aligned_pos = FEngGetTopLeftY(option->FEngObject);
        } else if (AlignmentToSelected == eSA_BOTTOM) {
            aligned_pos = FEngGetTopLeftY(option->FEngObject) + (option->OrigHeight - option->OrigHeight * scale);
        }

        FEngSetSize(option->FEngObject, option->OrigWidth * scale, option->OrigHeight * scale);
        FEngSetTopLeftY(option->FEngObject, aligned_pos);

        if (bAllowColorAnim) {
            UpdateFade(option, scale);
        }
    }
}

void IconScroller::UpdateFade(IconOption *option, float scale) {
    FEObject *object;

    if (option != nullptr && (object = option->FEngObject) != nullptr && object->pData != nullptr) {
        unsigned int idle_color = IdleColor;
        unsigned int fade_color = FadeColor;
        float a1 = static_cast<float>(idle_color >> 24);
        float r1 = static_cast<float>(idle_color >> 16 & 0xFF);
        float g1 = static_cast<float>(idle_color >> 8 & 0xFF);
        float b1 = static_cast<float>(idle_color & 0xFF);
        float a2 = static_cast<float>(fade_color >> 24);
        float r2 = static_cast<float>(fade_color >> 16 & 0xFF);
        float g2 = static_cast<float>(fade_color >> 8 & 0xFF);
        float b2 = static_cast<float>(fade_color & 0xFF);
        unsigned char alpha = static_cast<int>(a1 * scale + a2 * (1.0f - scale)) & 0xFF;

        if (option->IsGreyOut) {
            alpha = 0x96;
        } else {
            alpha = bClamp(alpha, 0, 0xFF);
        }

        float inverse_scale = 1.0f - scale;
        unsigned char red = static_cast<int>(r1 * scale + r2 * inverse_scale) & 0xFF;
        unsigned char green = static_cast<int>(g1 * scale + g2 * inverse_scale) & 0xFF;
        unsigned char blue = static_cast<int>(b1 * scale + b2 * inverse_scale) & 0xFF;
        red = bClamp(red, 0, 0xFF);
        green = bClamp(green, 0, 0xFF);
        blue = bClamp(blue, 0, 0xFF);
        unsigned int color = alpha * 0x1000000 + red * 0x10000 + green * 0x100 + blue;
        FEngSetColor(object, color);
    }
}

void IconScroller::UpdateArrows() {
    if (pCurrentNode == Options.GetHead()) {
        ScrollBar.SetArrowVisibility(1, false);
    } else if (pCurrentNode == Options.GetTail()) {
        ScrollBar.SetArrowVisibility(2, false);
    } else {
        ScrollBar.SetArrowVisibility(1, true);
        ScrollBar.SetArrowVisibility(2, true);
    }
}

IconOption *IconScroller::GetHead() {
    return static_cast<IconOption *>(HeadBookEnd->GetNext());
}

bool IconScroller::IsHead(IconOption *option) {
    return option == static_cast<IconOption *>(HeadBookEnd->GetNext());
}

bool IconScroller::IsTail(IconOption *option) {
    return option == static_cast<IconOption *>(TailBookEnd->GetPrev());
}

bool IconScroller::IsEndOfList(IconOption *opt) {
    return opt == TailBookEnd || opt == HeadBookEnd;
}

// ============================================================
// IconScrollerMenu
// ============================================================

IconScrollerMenu::IconScrollerMenu(ScreenConstructorData *sd)
    : MenuScreen(sd) //
{
    new (&Options) IconScroller(GetPackageName(), "OPTION_MASTER", "option_", "ICON_SCROLL_REGION", 350.0f);
    bWasLeftMouseDown = false;
    bFadeInIconsImmediately = true;
    pOptionName = nullptr;
    pOptionNameShadow = nullptr;
    pOptionDesc = nullptr;
    PrevButtonMessage = 0;
    PrevButtonObj = nullptr;
    PrevParam1 = 0;
    PrevParam2 = 0;
    pOptionName = FEngFindString(GetPackageName(), 0x5E7B09C9);
    pOptionNameShadow = FEngFindString(GetPackageName(), 0x0DFB7A2E);
    pOptionDesc = FEngFindString(GetPackageName(), 0);
}

void IconScrollerMenu::NotificationMessage(unsigned long msg, FEObject *pobj, unsigned long param1, unsigned long param2) {
    unsigned long message = msg;
    FEObject *object = pobj;
    unsigned long previous_param1 = param1;
    unsigned long previous_param2 = param2;

    switch (message) {
    case 0xC98356BA:
        Options.Update();
        return;
    case 0x84378BEF:
        Options.bFadingIn = false;
        Options.fCurFadeTime = Options.fMaxFadeTime;
        Options.bFadingOut = true;
        return;
    case 0x35F8620B:
        Options.bAllowColorAnim = true;
        return;
    case 0xE1FDE1D1:
        Options.IconPanel::Act(PrevButtonMessage, PrevButtonObj, PrevParam1, PrevParam2);
        return;
    case 0x911AB364:
        StorePrevNotification(0x911AB364, object, previous_param1, previous_param2);
        Options.bReactToInput = false;
        FEngSetLastButton(GetPackageName(), 0);
        return;
    case 0x0C407210: {
        if (!Options.bReactToInput) {
            return;
        }

        IconPanel *panel = &Options;
        IconOption *cur_option = Options.pCurrentNode;
        if (cur_option->IsGreyOut) {
            return;
        }
        if (object != cur_option->FEngObject) {
            return;
        }

        const char *pkg_name = GetPackageName();
        unsigned char current_index = 0;
        if (cur_option) {
            current_index = static_cast<unsigned char>(panel->GetOptionIndex(cur_option));
        }
        FEngSetLastButton(pkg_name, current_index);

        bool reacts_immediately = false;
        if (Options.pCurrentNode) {
            reacts_immediately = cur_option->ReactsImmediately();
        }
        if (reacts_immediately) {
            Options.IconPanel::Act(0x0C407210, object, previous_param1, previous_param2);
            return;
        }

        StorePrevNotification(0x0C407210, object, previous_param1, previous_param2);
        Options.bReactToInput = false;
        cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
        return;
    }
    case 0x9120409E:
        if (!Options.bHorizontal) {
            return;
        }
        if (!Options.bReactToInput) {
            return;
        }
        {
            IconPanel *panel = &Options;
            if (Options.bWrap) {
                panel->ScrollWrapped(eSD_PREV);
            } else {
                panel->Scroll(eSD_PREV);
            }
        }
        RefreshHeader();
        return;
    case 0xB5971BF1:
        if (!Options.bHorizontal) {
            return;
        }
        if (!Options.bReactToInput) {
            return;
        }
        {
            IconPanel *panel = &Options;
            if (Options.bWrap) {
                panel->ScrollWrapped(eSD_NEXT);
            } else {
                panel->Scroll(eSD_NEXT);
            }
        }
        RefreshHeader();
        return;
    case 0x72619778: {
        if (Options.bHorizontal) {
            return;
        }
        if (!Options.bReactToInput) {
            return;
        }
        IconPanel *panel = &Options;
        if (!panel->IsHead(Options.pCurrentNode)) {
            if (Options.bWrap) {
                panel->ScrollWrapped(eSD_PREV);
            } else {
                panel->Scroll(eSD_PREV);
            }
        }
        RefreshHeader();
        return;
    }
    case 0x911C0A4B: {
        if (Options.bHorizontal) {
            return;
        }
        if (!Options.bReactToInput) {
            return;
        }
        IconPanel *panel = &Options;
        if (Options.bWrap) {
            panel->ScrollWrapped(eSD_NEXT);
        } else {
            panel->Scroll(eSD_NEXT);
        }
        RefreshHeader();
        return;
    }
    case 0xC519BFC3: {
        IconOption *cur_option = Options.pCurrentNode;
        if (!cur_option->IsTutorialAvailable()) {
            return;
        }
        FEngSetScript(GetPackageName(), 0x99344537, 0x16A259, true);
        g_pEAXSound->PlayUISoundFX(UISND_COMMON_SELECT);
        FEAnyTutorialScreen::LaunchMovie(cur_option->GetTutorialMovieName(), GetPackageName());

        UserProfile *profile = FEDatabase->GetMultiplayerProfile(0);
        CareerSettings *career = profile->GetCareer();
        unsigned int name_hash = cur_option->GetName();
        if (name_hash == 0xA15E4505) {
            career->SetHasDoneTollBoothTutorial();
        } else if (name_hash > 0xA15E4505) {
            if (name_hash == 0xEE1EDC76) {
                career->SetHasDoneSpeedTrapTutorial();
            }
        } else if (name_hash == 0x6F547E4C) {
            career->SetHasDoneDragTutorial();
        }
        return;
    }
    case 0xC3960EB9:
        FEngSetScript(GetPackageName(), 0x99344537, 0x1744B3, true);
        return;
    default:
        return;
    }
}

void IconScrollerMenu::StorePrevNotification(unsigned int msg, FEObject *pobj, unsigned int param1, unsigned int param2) {
    PrevButtonMessage = msg;
    PrevButtonObj = pobj;
    PrevParam1 = param1;
    PrevParam2 = param2;
}

void IconScrollerMenu::RefreshHeader() {
    const unsigned long FEObj_TUTORIALGROUP = 0x9C7D33FF;

    FEngSetLanguageHash(pOptionName, Options.GetCurrentName());
    FEngSetLanguageHash(pOptionNameShadow, Options.GetCurrentName());
    FEngSetLanguageHash(pOptionDesc, Options.GetCurrentDesc());

    if (Options.AtHead()) {
        const unsigned long FEObj_ENDPADLEFT = 0xD7118934;
        cFEng::Get()->QueuePackageMessage(FEObj_ENDPADLEFT, GetPackageName(), nullptr);
    }

    if (Options.AtTail()) {
        const unsigned long FEObj_ENDPADRIGHT = 0xB9B17747;
        cFEng::Get()->QueuePackageMessage(FEObj_ENDPADRIGHT, GetPackageName(), nullptr);
    }

    if (!Options.GetCurrentOption()->IsTutorialAvailable()) {
        FEngSetScript(GetPackageName(), FEObj_TUTORIALGROUP, 0x16A259, true);
    } else {
        FEngSetScript(GetPackageName(), FEObj_TUTORIALGROUP, 0x1CA7C0, true);
    }
}

eMenuSoundTriggers IconScrollerMenu::NotifySoundMessage(unsigned long msg, eMenuSoundTriggers maybe) {
    if ((msg == 0x48122792 || msg == 0x4ac5e165) && !Options.JustScrolled()) {
        return static_cast<eMenuSoundTriggers>(-1);
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
    int index = 1;
    {
        IconOption *opt = Options.GetHead();
        while (opt != Options.EndOfList()) {
            if (to_find == index) {
                return opt;
            }
            index++;
            opt = opt->GetNext();
        }
    }
    return nullptr;
}

int IconPanel::GetOptionIndex(IconOption *to_find) {
    if (!to_find) {
        return -1;
    }
    int index = 1;
    {
        IconOption *opt = Options.GetHead();
        while (opt != Options.EndOfList()) {
            if (opt == to_find) {
                return index;
            }
            index++;
            opt = opt->GetNext();
        }
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
