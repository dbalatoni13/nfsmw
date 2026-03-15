#include "IconPanel.hpp"
#include "IconScroller.hpp"
#include "IconScrollerMenu.hpp"

#include "Speed/Indep/Src/Misc/Timer.hpp"

extern void FEngSetTextureHash(FEImage *image, unsigned int hash);
extern void FEngSetCurrentButton(const char *pkg_name, unsigned int hash);
extern void FEngGetCenter(FEObject *object, float &x, float &y);
extern unsigned long FEHash(const char *str);
extern FEColor FEngGetObjectColor(FEObject *object);
extern Timer RealTimer;
extern char *bStrCat(char *dest, const char *str1, const char *str2);
extern FEString *FEngFindString(const char *pkg_name, int hash);

static const char *gTUTORIAL_MOVIE_DRAG = "movies\\Tutorials\\TutDrag.vp6";
static const char *gTUTORIAL_MOVIE_SPEEDTRAP = "movies\\Tutorials\\TutSpeedTrap.vp6";

// ============================================================
// IconOption
// ============================================================

IconOption::IconOption(unsigned int tex_hash, unsigned int name_hash, unsigned int desc_hash) {
    XPos = 0.0f;
    NameHash = name_hash;
    DescHash = desc_hash;
    fScaleAtStart = 1.0f;
    pTutorialMovieName = nullptr;
    Item = tex_hash;
    FEngObject = nullptr;
    YPos = 0.0f;
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
        SetTutorialMovieName(gTUTORIAL_MOVIE_DRAG);
    } else if (tex_hash == 0x66C9A7B6) {
        bIsTutorialAvailable = true;
        SetTutorialMovieName(gTUTORIAL_MOVIE_SPEEDTRAP);
    }
}

void IconOption::SetFEngObject(FEObject *obj) {
    if (obj) {
        FEngObject = obj;
        FEngGetSize(obj, OrigWidth, OrigHeight);
        FEColor color = FEngGetObjectColor(obj);
        OriginalColor = static_cast<unsigned long>(color);
    }
}

void IconOption::StartScale(float scale_to, float duration) {
    fScaleToPcnt = scale_to;
    fScaleDurSecs = duration;
    bAnimComplete = false;
    fScaleStartSecs = RealTimer.GetSeconds();
}

unsigned int IconOption::GetName() { return NameHash; }
unsigned int IconOption::GetDesc() { return DescHash; }
float IconOption::GetScaleToPcnt() { return fScaleToPcnt; }
float IconOption::GetScaleStartSecs() { return fScaleStartSecs; }
float IconOption::GetScaleDurSecs() { return fScaleDurSecs; }
float IconOption::GetScaleAtStart() { return fScaleAtStart; }
void IconOption::SetScaleAtStart(float scale) { fScaleAtStart = scale; }
bool IconOption::IsAnimComplete() { return bAnimComplete; }
void IconOption::SetAnimComplete(bool b) { bAnimComplete = b; }
bool IconOption::ReactsImmediately() { return bReactImmediately; }
bool IconOption::IsLocked() { return Locked; }
void IconOption::SetLocked(bool b) { Locked = b; }
bool IconOption::IsTutorialAvailable() { return bIsTutorialAvailable; }
const char *IconOption::GetTutorialMovieName() { return pTutorialMovieName; }
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
    float num_opts = static_cast<float>(Options.Options.CountElements());
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
    if (Options.Options.CountElements() == 0) {
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
    if (Options.Options.CountElements() == 0) {
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
    if (Options.Options.CountElements() - iNumBookEnds < 1) {
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
    if (Options.Options.CountElements() - iNumBookEnds <= 0) {
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
    float half = fWidth * 0.5f;
    if (pos < fXCenter - half || pos > fXCenter + half) {
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

void IconScrollerMenu::StorePrevNotification(unsigned int msg, FEObject *pobj, unsigned int param1, unsigned int param2) {
    PrevButtonMessage = msg;
    PrevButtonObj = pobj;
    PrevParam1 = param1;
    PrevParam2 = param2;
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



