#include "FEIconScrollerMenu.hpp"

#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEButtons.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEObjects.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/FEAnyTutorialScreen.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

const char *gTUTORIAL_MOVIE_DRAG = "drag_tutorial";
const char *gTUTORIAL_MOVIE_SPEEDTRAP = "speedtrap_tutorial";

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
    fScaleStartSecs = static_cast<float>(RealTimer.GetPackedTime()) * 0.00025f;
    bAnimComplete = false;
}

IconPanel::IconPanel(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region,
                     bool wrap)
    : pPackageName(pkg_name), //
      pButtonName(fe_button), //
      bWrap(wrap),            //
      pMaster(nullptr),       //
      fIconSpacing(10.0f),    //
      bReactToInput(true),    //
      pCurrentNode(nullptr),  //
      iIndexToAdd(1),         //
      bHorizontal(true),      //
      bJustScrolled(true) {
    pMaster = FEngFindObject(pkg_name, FEHash(master));
    pScrollRegion = FEngFindObject(pPackageName, FEHash(scroll_region));
}

FEImage *IconPanel::AddOption(IconOption *option) {
    char concat[32];
    char name[32];
    bStrCat(concat, pButtonName, "%d");
    FEngSNPrintf(name, 32, concat, iIndexToAdd);
    FEImage *image = FEngFindImage(pPackageName, FEHashUpper(name));
    if (image == nullptr) {
        return nullptr;
    }
    iIndexToAdd = iIndexToAdd + 1;
    if (option == nullptr) {
        return nullptr;
    }
    option->SetFEngObject(image);
    Options.AddTail(option);
    if (pCurrentNode == nullptr) {
        pCurrentNode = Options.GetHead();
        FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject->NameHash);
    }
    return image;
}

void IconPanel::Act(uint32 data, FEObject *obj, uint32 param1, uint32 param2) {
    if (pCurrentNode != nullptr && obj == pCurrentNode->FEngObject) {
        pCurrentNode->React(pPackageName, data, obj, param1, param2);
    }
}

IconOption *IconPanel::GetOption(int to_find) {
    if (to_find <= 0) {
        return nullptr;
    }
    int i = 1;
    for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
        if (to_find == i) {
            return node;
        }
        i = i + 1;
    }
    return nullptr;
}

int IconPanel::GetOptionIndex(IconOption *to_find) {
    if (to_find == nullptr) {
        return -1;
    }
    int i = 1;
    for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
        if (node == to_find) {
            return i;
        }
        i = i + 1;
    }
    return -1;
}

bool IconPanel::SetSelection(IconOption *option) {
    if (!option->IsGreyOut) {
        pCurrentNode->StartScale(0.614f, 0.2f);
        pCurrentNode = option;
        FEngSetCurrentButton(pPackageName, option->FEngObject->NameHash);
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
        i = i + 1.0f;
    }
    SetSelection(pCurrentNode);
}

void IconPanel::Scroll(eScrollDir dir) {
    if (Options.CountElements() != 0) {
        IconOption *new_option = pCurrentNode;
        if (dir == eSD_PREV) {
            if (new_option != Options.GetHead()) {
                do {
                    new_option = new_option->GetPrev();
                    if (!new_option->IsGreyOut) {
                        break;
                    }
                } while (new_option != Options.GetHead());
            }
        } else if (dir == eSD_NEXT) {
            if (new_option != Options.GetTail()) {
                do {
                    new_option = new_option->GetNext();
                    if (!new_option->IsGreyOut) {
                        break;
                    }
                } while (new_option != Options.GetTail());
            }
        }
        if (!new_option->IsGreyOut && new_option != pCurrentNode) {
            SetSelection(new_option);
            bJustScrolled = true;
        }
    }
}

void IconPanel::ScrollWrapped(eScrollDir dir) {
    if (Options.CountElements() != 0) {
        IconOption *node = pCurrentNode;
        if (dir == eSD_PREV) {
            do {
                if (node == Options.GetHead()) {
                    node = Options.GetTail();
                } else {
                    node = node->GetPrev();
                }
            } while (node->IsGreyOut);
        } else if (dir == eSD_NEXT) {
            do {
                if (node == Options.GetTail()) {
                    node = Options.GetHead();
                } else {
                    node = node->GetNext();
                }
            } while (node->IsGreyOut);
        }
        if (!node->IsGreyOut && node != pCurrentNode) {
            SetSelection(node);
            bJustScrolled = true;
        }
    }
}

void IconPanel::Update() {
    AnimateList();
}

void IconPanel::AnimateList() {
    float width = 0.0f;
    float height = 0.0f;
    AnimateSelected(width, height);
}

void IconPanel::AnimateSelected(float &width, float &height) {
    bJustScrolled = false;
    width = 0.0f;
    height = 0.0f;
    for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
        float scale = 1.0f;
        if (!node->IsAnimComplete()) {
            float pcnt_complete = (RealTimer.GetSeconds() - node->GetScaleStartSecs()) / node->GetScaleDurSecs();
            float delta_scale = node->GetScaleToPcnt() - node->GetScaleAtStart();
            scale = node->GetScaleAtStart() + delta_scale * pcnt_complete;
            if ((delta_scale >= 0.0f && scale < node->GetScaleToPcnt()) || (delta_scale < 0.0f && scale > node->GetScaleToPcnt())) {
                FEngSetSize(node->FEngObject, node->OrigWidth * scale, node->OrigHeight * scale);
                bJustScrolled = true;
            } else {
                FEngSetSize(node->FEngObject, node->OrigWidth * node->GetScaleToPcnt(), node->OrigHeight * node->GetScaleToPcnt());
                node->SetScaleAtStart(scale);
                node->SetAnimComplete(true);
            }
        }
        width = width + node->OrigWidth * scale;
        height = height + node->OrigHeight * scale;
        if (node != Options.GetTail()) {
            width = width + fIconSpacing;
            height = height + fIconSpacing;
        }
    }
}

/* CERRADA en la r46 (100 %, 384 B). La veda de la r36f daba por buena la
 * fuente y culpaba al planificador; la causa real es que `sched1` parte los
 * stores en DOS GRUPOS por `INSN_REG_WEIGHT` --el store que MATA el registro
 * del valor (ultimo uso de f0=0.0f, de f12=9.0f, de r0=4, r9, r11, r10) pesa
 * -1 y va ANTES que todos los que pesan 0-- y dentro de cada grupo respeta el
 * orden de la fuente. Con `fCurFadeTime` como ULTIMO 0.0f, su store entra en
 * el grupo de peso -1 y `fCurrentAddPos` se queda en el de peso 0, que es
 * justo lo que emite el objetivo. NO TOCAR EL ORDEN de estas 13 sentencias.
 * Medido: orden anterior 11 filas; solo mover fCurFadeTime al final de los
 * 0.0f 8 filas; con fMaxFadeTime/bAllowColorAnim/IdleColor/FadeColor detras
 * 0 filas. */
IconScroller::IconScroller(const char *pkg_name, const char *master, const char *fe_button, const char *scroll_region,
                           float width)
    : IconPanel(pkg_name, master, fe_button, scroll_region, false), //
      HeadBookEnd(nullptr),                                         //
      TailBookEnd(nullptr),                                         //
      ScrollBar(pkg_name, "ScrollBar", false, false, true),         //
      AnimateCubic(1, 1.0f) {
    AlignmentToSelected = eSA_MIDDLE;
    iCurSelectedIndex = 1;
    fHeight = 0.0f;
    iNumBookEnds = 4;
    fXCenter = 0.0f;
    fYCenter = 0.0f;
    fCurrentAddPos = 0.0f;
    fCurFadeTime = 0.0f;
    fMaxFadeTime = 9.0f;
    bAllowColorAnim = true;
    IdleColor = 0xFFFFFFFF;
    FadeColor = 0x00FFFFFF;
    fWidth = width;
    bDelayUpdate = bInitialized = bFadingOut = bFadingIn = false;
    fIconSpacing = -5.0f;
    FEObject *master_obj = FEngFindObject(pkg_name, FEHashUpper(master));
    if (master_obj != nullptr) {
        FEngGetCenter(master_obj, fXCenter, fYCenter);
        FEngSetInvisible(master_obj);
    }
    AddInitialBookEnds();
    AnimateCubic.SetDuration(0.2f);
    AnimateCubic.SetFlags(0);
}

void IconScroller::Update() {
    if (Options.GetHead() == Options.EndOfList()) {
        return;
    }
    if (pCurrentNode == nullptr) {
        return;
    }
    if (bDelayUpdate) {
        return;
    }
    if (bJustScrolled) {
        bJustScrolled = false;
        int numBookEnds = iNumBookEnds + 1;
        ScrollBar.Update(1, iIndexToAdd - numBookEnds, iCurSelectedIndex - iNumBookEnds,
                         iCurSelectedIndex - iNumBookEnds);
        AnimateCubic.ValDesired = -pCurrentNode->XPos;
        if (AnimateCubic.ValDesired != AnimateCubic.Val) {
            AnimateCubic.state = 2;
        }
        UpdateArrows();
    }
    for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
        PositionOption(node);
    }
    if (bFadingIn) {
        fCurFadeTime = fCurFadeTime + 1.0f;
        if (fCurFadeTime >= fMaxFadeTime) {
            fCurFadeTime = fMaxFadeTime;
            bFadingIn = false;
        }
    } else if (bFadingOut) {
        fCurFadeTime = fCurFadeTime - 1.0f;
        if (fCurFadeTime <= 0.0f) {
            fCurFadeTime = 0.0f;
        }
    }
    cPoint::SplineSeek(&AnimateCubic, RealTimeElapsed, 0.0f, 0.0f);
}






void IconScroller::AddInitialBookEnds() {
    for (int i = 0; i < iNumBookEnds / 2; i++) {
        FEScrollyBookEnd *bookEnd = new ("FEScrollyBookEnd", 0) FEScrollyBookEnd(0x43B6310F);
        FEImage *image = AddOption(bookEnd);
        if (image != nullptr) {
            FEngSetTextureHash(image, bookEnd->Item);
        }
    }
    HeadBookEnd = Options.GetTail();
}

FEImage *IconScroller::AddOption(IconOption *option) {
    char name[32];
    FEngSNPrintf(name, 32, "%s%d", pButtonName, iIndexToAdd);
    FEImage *image = FEngFindImage(pPackageName, FEHashUpper(name));
    if (image == nullptr) {
        if (option != nullptr) {
            delete option;
        }
        return nullptr;
    }
    if (option == nullptr) {
        return nullptr;
    }
    iIndexToAdd = iIndexToAdd + 1;
    option->SetFEngObject(image);
    option->XPos = fCurrentAddPos;
    option->OriginalColor = IdleColor;
    FEngGetSize(option->FEngObject, option->OrigWidth, option->OrigHeight);
    float width;
    float height;
    FEngGetSize(option->FEngObject, width, height);
    fCurrentAddPos += width + fIconSpacing;
    Options.AddTail(option);
    if (pCurrentNode == nullptr) {
        if (iIndexToAdd > iNumBookEnds + 1) {
            pCurrentNode = HeadBookEnd->GetNext();
            FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject->NameHash);
        }
    }
    return image;
}

void IconScroller::SetInitialPos(int index) {
    TailBookEnd = Options.GetTail();
    for (int i = 0; i < iNumBookEnds / 2; i++) {
        FEScrollyBookEnd *bookEnd = new ("FEScrollyBookEnd", 0) FEScrollyBookEnd(0x43B6310F);
        FEImage *image = AddOption(bookEnd);
        if (image != nullptr) {
            FEngSetTextureHash(image, bookEnd->Item);
        }
    }
    TailBookEnd = TailBookEnd->GetNext();
    if (index > 0) {
        index = index + iNumBookEnds / 2;
    }
    IconOption *node = Options.GetNode(index - 1);
    if (index != 0 && node != nullptr) {
        if (node->Item == 0x43B6310F) {
            SetSelection(TailBookEnd->GetPrev());
        } else {
            SetSelection(node);
        }
    } else {
        SetSelection(HeadBookEnd->GetNext());
    }
    if (bHorizontal) {
        AnimateCubic.SetValDesired(-pCurrentNode->XPos);
    } else {
        AnimateCubic.SetValDesired(-pCurrentNode->YPos);
    }
    AnimateCubic.Val = AnimateCubic.ValDesired;
    AnimateCubic.dVal = AnimateCubic.dValDesired;
    AnimateCubic.state = 0;
    if (!bDelayUpdate) {
        for (IconOption *option = Options.GetHead(); option != Options.EndOfList(); option = option->GetNext()) {
            PositionOption(option);
        }
    }
    bInitialized = true;
}

bool IconScroller::SetSelection(IconOption *option) {
    int index = GetOptionIndex(option);

    if (index >= 0 && index < iIndexToAdd) {

        for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
            FEngSetButtonState(pPackageName, node->FEngObject->NameHash, false);
        }

        if (option->IsGreyOut) {
            return false;
        }

        pCurrentNode = option;
        FEngSetButtonState(pPackageName, option->FEngObject->NameHash, true);
        FEngSetCurrentButton(pPackageName, pCurrentNode->FEngObject);
        iCurSelectedIndex = GetOptionIndex(option);
    } else {
        return false;
    }

    return true;
}

void IconScroller::RemoveAll() {
    for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
        FEngSetSize(node->FEngObject, node->OrigWidth, node->OrigHeight);
        FEngSetTopLeft(node->FEngObject, 696969.0f, 696969.0f);
    }
    IconPanel::RemoveAll();
    fCurrentAddPos = 0.0f;
}

int IconScroller::GetOptionIndex(IconOption *to_find) {
    if (to_find == nullptr) {
        return -1;
    }
    int i = 1;
    for (IconOption *node = Options.GetHead(); node != Options.EndOfList(); node = node->GetNext()) {
        if (node == to_find) {
            return i - iNumBookEnds / 2;
        }
        i = i + 1;
    }
    return -1;
}

void IconScroller::Scroll(eScrollDir dir) {
    if (Options.CountElements() - iNumBookEnds > 0) {
        IconOption *new_option = pCurrentNode;
        if (dir == eSD_PREV) {
            if (new_option != HeadBookEnd->GetNext()) {
                do {
                    new_option = new_option->GetPrev();
                    if (!new_option->IsGreyOut) {
                        break;
                    }
                } while (new_option != HeadBookEnd->GetNext());
            }
        } else if (dir == eSD_NEXT) {
            if (new_option != TailBookEnd->GetPrev()) {
                do {
                    new_option = new_option->GetNext();
                    if (!new_option->IsGreyOut) {
                        break;
                    }
                } while (new_option != TailBookEnd->GetPrev());
            }
        }
        if (!new_option->IsGreyOut && new_option != pCurrentNode) {
            SetSelection(new_option);
            bJustScrolled = true;
        }
    }
}

void IconScroller::ScrollWrapped(eScrollDir dir) {
    if (Options.CountElements() - iNumBookEnds > 0) {
        IconOption *node = pCurrentNode;
        if (dir == eSD_PREV) {
            do {
                if (node == HeadBookEnd->GetNext()) {
                    node = TailBookEnd->GetPrev();
                } else {
                    node = node->GetPrev();
                }
            } while (node->IsGreyOut);
        } else if (dir == eSD_NEXT) {
            do {
                if (node == TailBookEnd->GetPrev()) {
                    node = HeadBookEnd->GetNext();
                } else {
                    node = node->GetNext();
                }
            } while (node->IsGreyOut);
        }
        if (!node->IsGreyOut && node != pCurrentNode) {
            SetSelection(node);
            bJustScrolled = true;
        }
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
    float half = scroll_size * 0.5f;
    float minPos = center - half;
    float maxPos = center + half;
    if (x < minPos || x > maxPos) {
        return 0.0f;
    }
    if (x >= minPos && x < center - 1.5f) {
        return (x - minPos) / half;
    }
    if (x <= maxPos && x > center + 1.5f) {
        return (maxPos - x) / (scroll_size * 0.5f);
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
        xpos = xpos + option->OrigWidth * (1.0f - scale) * (1.0f - scale) * (1.0f - scale);
    } else {
        xpos = xpos - option->OrigWidth * (1.0f - scale) * (1.0f - scale) * (1.0f - scale);
    }
    ClipEdges(option, xpos);
    FEngSetCenter(option->FEngObject, xpos, fYCenter);
    if (bFadingIn || bFadingOut) {
        scale = scale * (fCurFadeTime / fMaxFadeTime);
    }
    float aligned_pos = 0.0f;
    switch (AlignmentToSelected) {
        case eSA_TOP:
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
        float r1 = static_cast<float>((IdleColor >> 16) & 0xFF);
        float g1 = static_cast<float>((IdleColor >> 8) & 0xFF);
        float b1 = static_cast<float>(IdleColor & 0xFF);

        float a2 = static_cast<float>(FadeColor >> 24);
        float r2 = static_cast<float>((FadeColor >> 16) & 0xFF);
        float g2 = static_cast<float>((FadeColor >> 8) & 0xFF);
        float b2 = static_cast<float>(FadeColor & 0xFF);

        unsigned char a = static_cast<unsigned char>(a1 * scale + a2 * (1.0f - scale));
        if (option->IsGreyOut) {
            a = 0x96;
        } else {
            a = static_cast<unsigned char>(bClamp(a, 0, 255));
        }

        unsigned char r = static_cast<unsigned char>(r1 * scale + r2 * (1.0f - scale));
        unsigned char g = static_cast<unsigned char>(g1 * scale + g2 * (1.0f - scale));
        unsigned char b = static_cast<unsigned char>(b1 * scale + b2 * (1.0f - scale));

        r = static_cast<unsigned char>(bClamp(r, 0, 255));
        g = static_cast<unsigned char>(bClamp(g, 0, 255));
        b = static_cast<unsigned char>(bClamp(b, 0, 255));

        unsigned int color = (a << 24) + (r << 16) + (g << 8);
        FEngSetColor(option->FEngObject, color | b);
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

IconScrollerMenu::IconScrollerMenu(ScreenConstructorData *sd)
    : MenuScreen(sd),                                                                          //
      Options(GetPackageName(), "OPTION_MASTER", "option_", "ICON_SCROLL_REGION", 350.0f), //
      bWasLeftMouseDown(false),                                                                //
      bFadeInIconsImmediately(true),                                                           //
      pOptionName(nullptr),                                                                    //
      pOptionNameShadow(nullptr),                                                              //
      pOptionDesc(nullptr),                                                                    //
      PrevButtonMessage(0),                                                                    //
      PrevButtonObj(nullptr),                                                                  //
      PrevParam1(0),                                                                           //
      PrevParam2(0) {
    pOptionName = FEngFindString(GetPackageName(), 0x5E7B09C9);
    pOptionNameShadow = FEngFindString(GetPackageName(), 0x0DFB7A2E);
    pOptionDesc = FEngFindString(GetPackageName(), 0);
    mPlaySound = false;
}

void IconScrollerMenu::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    switch (msg) {
        case FEMSG_SCREEN_TICK:
            Options.Update();
            break;
        case FEMSG_EXIT_STARTED:
            Options.StartFadeOut();
            break;
        case 0x35F8620B:
            Options.SetAllowFade(true);
            break;
        case __EXIT_COMPLETE__:
            Options.Act(PrevButtonMessage, PrevButtonObj, PrevParam1, PrevParam2);
            break;
        case __PAD_BACK__:
            StorePrevNotification(__PAD_BACK__, pObj, param1, param2);
            Options.SetReactToInput(false);
            FEngSetLastButton(GetPackageName(), 0);
            break;
        case 0x0C407210:
            if (Options.ReactsToInput() && !Options.GetCurrentOption()->IsGreyOut &&
                pObj == Options.GetCurrentOption()->FEngObject) {
                FEngSetLastButton(GetPackageName(), Options.GetCurrentIndex());
                if (Options.CurrentReactsImmediately()) {
                    Options.Act(msg, pObj, param1, param2);
                } else {
                    StorePrevNotification(msg, pObj, param1, param2);
                    Options.SetReactToInput(false);
                    cFEng::Get()->QueuePackageMessage(0x587C018B, GetPackageName(), nullptr);
                }
            }
            break;
        case __PAD_LEFT__:
            if (Options.IsHorizontal() && Options.ReactsToInput()) {
                Options.ScrollPrev();
                RefreshHeader();
            }
            break;
        case __PAD_RIGHT__:
            if (Options.IsHorizontal() && Options.ReactsToInput()) {
                Options.ScrollNext();
                RefreshHeader();
            }
            break;
        case __PAD_UP__:
            if (!Options.IsHorizontal() && Options.ReactsToInput()) {
                if (!Options.AtHead()) {
                    Options.ScrollPrev();
                }
                RefreshHeader();
            }
            break;
        case __PAD_DOWN__:
            if (!Options.IsHorizontal() && Options.ReactsToInput()) {
                Options.ScrollNext();
                RefreshHeader();
            }
            break;
        case __PAD_BUTTON4__: {
            IconOption *cur_option = Options.GetCurrentOption();
            if (cur_option->IsTutorialAvailable()) {
                const u32 FEObj_MASTERBLASTER = 0x99344537;
                const u32 FEObj_HIDE = 0x16A259;
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
            break;
        }
        case FEMSG_MOVIE_FINISHED: {
            const u32 FEObj_MASTERBLASTER = 0x99344537;
            const u32 FEObj_Init = FEHash_Init;
            FEngSetScript(GetPackageName(), FEObj_MASTERBLASTER, FEObj_Init, true);
            break;
        }
    }
}

eMenuSoundTriggers IconScrollerMenu::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == FEHASH_SOUND_LEFT || msg == FEHASH_SOUND_RIGHT) {
        if (!Options.JustScrolled()) {
            return UISND_NONE;
        }
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
        cFEng::Get()->QueuePackageMessage(0xD7118934, GetPackageName(), nullptr);
    }
    if (Options.AtTail()) {
        cFEng::Get()->QueuePackageMessage(0xB9B17747, GetPackageName(), nullptr);
    }
    if (Options.GetCurrentOption()->IsTutorialAvailable()) {
        FEngSetScript(GetPackageName(), 0x9C7D33FF, 0x1CA7C0, true);
    } else {
        FEngSetScript(GetPackageName(), 0x9C7D33FF, 0x16A259, true);
    }
}

void IconScrollerMenu::AddOption(IconOption *option) {
    FEImage *image = Options.AddOption(option);
    FEngSetTextureHash(image, option->Item);
}

inline IconOption *IconPanel::GetHead() {
    return Options.GetHead();
}

inline bool IconPanel::IsHead(IconOption *option) {
    return option == Options.GetHead();
}

inline bool IconPanel::IsTail(IconOption *option) {
    return option == Options.GetTail();
}

inline bool IconPanel::IsEndOfList(IconOption *opt) {
    return opt == Options.EndOfList();
}

inline bool IconScroller::IsHead(IconOption *option) {
    return option == static_cast<IconOption *>(HeadBookEnd->GetNext());
}

inline bool IconScroller::IsTail(IconOption *option) {
    return option == static_cast<IconOption *>(TailBookEnd->GetPrev());
}
