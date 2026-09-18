#include "feArrayScrollerMenu.hpp"

#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"

ArrayScripts::ArrayScripts() {
    SetNormalHash(FEHashUpper("INIT"));
    SetGreyHash(FEHashUpper("GREY"));
    SetHighlightHash(FEHashUpper("HIGHLIGHT"));
    SetUnHighlightHash(FEHashUpper("UNHIGHLIGHT"));
}

ArraySlot::ArraySlot(FEObject *obj)
    : FEngObject(obj), //
      scripts(nullptr) {}

void ArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    if (!datum) {
        FEngSetInvisible(GetFEngObject());
    } else {
        FEngSetVisible(GetFEngObject());
        if (datum->IsGreyedOut() || !datum->IsEnabled()) {
            if (!FEngIsScriptSet(GetFEngObject(), scripts->GetGreyHash())) {
                FEngSetScript(GetFEngObject(), scripts->GetGreyHash(), true);
            }
        } else {
            if (isSelected) {
                if (!FEngIsScriptSet(GetFEngObject(), scripts->GetHighlightHash())) {
                    FEngSetScript(GetFEngObject(), scripts->GetHighlightHash(), true);
                }
            } else {
                if (FEngIsScriptSet(GetFEngObject(), scripts->GetHighlightHash())) {
                    FEngSetScript(GetFEngObject(), scripts->GetUnHighlightHash(), true);
                } else {
                    if (!FEngIsScriptSet(GetFEngObject(), scripts->GetNormalHash())) {
                        FEngSetScript(GetFEngObject(), scripts->GetNormalHash(), true);
                    }
                }
            }
        }
    }
}

ImageArraySlot::ImageArraySlot(FEImage *img) : ArraySlot(static_cast<FEObject *>(static_cast<void *>(img))) {}

void ImageArraySlot::SetTexture(unsigned int tex_hash) {
    FEngSetTextureHash(static_cast<FEImage *>(GetFEngObject()), tex_hash);
}

void ImageArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum) {
        SetTexture(datum->GetHash());
    }
}

ArrayDatum::ArrayDatum(uint32 h, uint32 d)
    : hash(h),          //
      desc(d),          //
      enabled(true),    //
      greyedOut(false), //
      locked(false),    //
      checked(false) {}

ArrayScroller::ArrayScroller(const char *name, int w, int h, bool selectable)
    : bShouldPlaySound(false),                         //
      currentDatum(nullptr),                           //
      startDatum(0),                                   //
      pkg_name(name),                                  //
      ScrollBar(name, "scrollbar", true, true, false), //
      bSelectableArray(selectable),                    //
      scripts(),                                       //
      bInClickToSelectMode(false) {
    SetDimensions(w, h);
    SetDescLabel(0);
    SetMouseDownMsg(__BUTTON_PRESSED__);
    SetScrollRegion(FEngFindObject(name, FEHashUpper("ARRAY_SCROLL_REGION")));
    pkg = cFEng::Get()->FindPackage(GetPkgName());
}

void ArrayScroller::RefreshHeader() {
    for (int i = 0; i < GetNumSlots(); i++) {
        ArrayDatum *datum = GetDatumAt(startDatum + i);
        ArraySlot *slot = GetSlotAt(i);
        if (slot) {
            slot->Update(datum, currentDatum == datum);
        }
    }
    if (currentDatum) {
        FEngSetLanguageHash(GetPkgName(), descLabel, currentDatum->GetDesc());
    }
}

void ArrayScroller::AddSlot(ArraySlot *slot) {
    slot->SetScripts(&scripts);
    slots.AddTail(slot);
}

void ArrayScroller::AddDatum(ArrayDatum *datum) {
    data.AddTail(datum);
    if (!currentDatum) {
        SetSelection(datum, 0);
    }
}

void ArrayScroller::SetSelection(ArrayDatum *newDatum, int newStartDatum) {
    if (newDatum->IsEnabled()) {
        startDatum = newStartDatum;
        currentDatum = newDatum;
        if (bSelectableArray) {
            ArraySlot *pSlot = GetSlotAt(data.GetNodeNumber(currentDatum) - (startDatum + 1));
            if (pSlot) {
                FEngSetCurrentButton(GetPkgName(), pSlot->GetFEngObject()->NameHash);
            }
        }
    }
}

int ArrayScroller::ForceSelectionOnScreen(int new_datum, int start) {
    int w = GetWidth();
    int h = GetHeight();
    int ret = start;
    if (new_datum < start) {
        ret = (new_datum / w) * w;
    } else if (new_datum > start + w * h) {
        int rows = h - 1;
        ret = (new_datum / w - rows) * w;
    }
    return ret;
}

void ArrayScroller::ScrollHor(eScrollDir dir) {
    if (data.CountElements() == 0) {
        return;
    }

    ArrayDatum *new_datum = currentDatum;
    int current_num = GetCurrentDatumNum() - 1;
    int new_index = current_num;

    if (dir == eSD_PREV) {

        new_index = current_num - 1;

        if (new_index + 1 == ((new_index + 1) / width) * width) {
            new_index = new_index + width;
        }

        if (new_index >= GetNumDatum()) {
            new_index = GetNumDatum() - 1;
        }
    } else if (dir == eSD_NEXT) {

        new_index = current_num + 1;

        if (new_index == (new_index / width) * width) {
            new_index = new_index - width;
        }

        if (new_index >= GetNumDatum()) {
            new_index = (new_index / width) * width;
        }
    }

    if (current_num < new_index && new_index < GetNumDatum()) {
        for (int i = current_num; i < new_index; i++) {
            new_datum = static_cast<ArrayDatum *>(new_datum->GetNext());
        }
    } else if (current_num > new_index && new_index >= 0) {
        for (int i = new_index; i < current_num; i++) {
            new_datum = static_cast<ArrayDatum *>(new_datum->GetPrev());
        }
    }

    if (new_datum != currentDatum) {
        int start = ForceSelectionOnScreen(new_index, startDatum);
        SetSelection(new_datum, start);

        bShouldPlaySound = true;
    }

    RefreshHeader();
}

void ArrayScroller::ScrollVer(eScrollDir dir) {
    ArrayDatum *new_datum;
    int new_index;
    int new_start;

    if (data.CountElements() == 0) {
        return;
    }
    new_datum = currentDatum;
    new_index = GetCurrentDatumNum() - 1;
    new_start = startDatum;

    if (dir == eSD_PREV) {
        new_index = new_index - width;

        if (!bSelectableArray) {
            if (new_index >= (height - 1) * width && new_index < height * width) {
                new_index = new_index - (height - 1) * width;
            }

            new_start = new_start - width;
        } else if (new_index < new_start) {
            new_start = new_start - width;
        }

        if (new_index >= 0) {
            for (int i = 0; i < GetCurrentDatumNum() - (new_index + 1); i++) {
                new_datum = static_cast<ArrayDatum *>(new_datum->GetPrev());
            }
        }
    } else if (dir == eSD_NEXT) {
        new_index = new_index + width;

        if (!bSelectableArray) {
            if (new_index >= width && new_index < width * 2) {
                new_index = new_index + (height - 1) * width;
            }

            new_start = new_start + width;
        } else {
            if (new_index >= GetNumDatum()) {
                if (((GetNumDatum() - 1) / GetWidth() + 1) * GetHeight() >
                    ((GetCurrentDatumNum() - 1) / GetWidth() + 1) * GetHeight()) {
                    new_index = GetNumDatum() - 1;
                }
            }

            if (new_start + GetNumSlots() <= new_index) {
                new_start = new_start + width;
            }
        }

        if (new_index < GetNumDatum()) {
            for (int i = 0; i < (new_index + 1) - GetCurrentDatumNum(); i++) {
                new_datum = static_cast<ArrayDatum *>(new_datum->GetNext());
            }
        }
    }

    if (new_datum != currentDatum) {
        int start = ForceSelectionOnScreen(new_index, new_start);
        SetSelection(new_datum, start);

        UpdateScrollbar();

        bShouldPlaySound = true;
    }

    RefreshHeader();
}

void ArrayScroller::UpdateScrollbar() {
    int view_size = GetHeight();
    int num_rows_of_data = (GetNumDatum() - 1) / GetWidth() + 1;
    int top_item = startDatum / GetWidth() + 1;
    int selected_item = (GetCurrentDatumNum() - 1) / GetWidth() + 1;

    ScrollBar.Update(view_size, num_rows_of_data, top_item, selected_item);
}

ArraySlot *ArrayScroller::GetSlotAt(int index) {
    if (index >= GetNumSlots()) {
        return nullptr;
    }
    return slots.GetNode(index);
}

ArrayDatum *ArrayScroller::GetDatumAt(int index) {
    if (index >= GetNumDatum()) {
        return nullptr;
    }
    return data.GetNode(index);
}

void ArrayScroller::SetInitialPosition(int index) {
    if (GetNumDatum() == 0) {
        UpdateScrollbar();
    }
    if (index < GetNumDatum()) {
        int size = GetWidth() * GetHeight();
        int newStartDatum = 0;
        if (index > size - 1) {
            int new_index = index / GetWidth();
            new_index = new_index * GetWidth();
            new_index = new_index + 1;
            new_index = new_index - size;
            new_index = new_index / GetWidth();
            new_index = new_index * GetWidth();
            newStartDatum = new_index + GetWidth();
            if (GetWidth() == 1) {
                newStartDatum = new_index;
            }
        }
        ArrayDatum *datum = GetDatumAt(index);
        SetSelection(datum, newStartDatum);
        UpdateScrollbar();
    }
}

void ArrayScroller::UpdateMouse() {}

void ArrayScroller::ClearData() {
    data.DeleteAllElements();
    currentDatum = nullptr;
    startDatum = 0;
}

void ArrayScroller::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    ArrayDatum *currentDatum = GetCurrentDatum();
    if (currentDatum) {
        currentDatum->NotificationMessage(msg, pObj, param1, param2);
    }
    switch (msg) {
        case __PAD_LEFT__:
            ScrollHor(eSD_PREV);
            break;
        case __PAD_RIGHT__:
            ScrollHor(eSD_NEXT);
            break;
        case __PAD_UP__:
            ScrollVer(eSD_PREV);
            break;
        case __PAD_DOWN__:
            ScrollVer(eSD_NEXT);
            break;
        case FEMSG_MOUSE_CHANGED:
            UpdateMouse();
            break;
    }
}

ArrayScrollerMenu::ArrayScrollerMenu(ScreenConstructorData *sd, int w, int h, bool selectable)
    : MenuScreen(sd),                                      //
      ArrayScroller(sd->PackageFilename, w, h, selectable) //
{}

void ArrayScrollerMenu::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    ArrayScroller::NotificationMessage(msg, pObj, param1, param2);
}

eMenuSoundTriggers ArrayScrollerMenu::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == __PAD_LEFT__ || msg == __PAD_RIGHT__ || msg == __PAD_DOWN__ || msg == __PAD_UP__ || msg == FEHASH_SOUND_DOWN ||
        msg == FEHASH_SOUND_UP || msg == FEHASH_SOUND_LEFT || msg == FEHASH_SOUND_RIGHT) {
        if (!bShouldPlaySound) {
            maybe = UISND_NONE;
        }
        bShouldPlaySound = false;
    }
    return maybe;
}

void ArrayScrollerMenu::RefreshHeader() {
    ArrayScroller::RefreshHeader();
}
