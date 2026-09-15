#include "feArrayScrollerMenu.hpp"

#include "Speed/Indep/Src/Frontend/FEngFrontend.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/FEHash_FeBonusCards.hpp"
#include "Speed/Indep/Src/Frontend/FEngHashes/SoundHashes.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEImages.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterfaceFEStrings.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"

ArrayScripts::ArrayScripts() {
    SetNormalHash(FEHashUpper("INIT"));
    SetGreyHash(FEHashUpper("GREY"));
    SetHighlightHash(FEHashUpper("HIGHLIGHT"));
    SetUnHighlightHash(FEHashUpper("UNHIGHLIGHT"));
}

ArraySlot::ArraySlot(FEObject *obj) : FEngObject(obj), scripts(nullptr) {}

void ArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    if (datum == nullptr) {
        FEngSetInvisible(FEngObject);
    } else {
        FEngSetVisible(FEngObject);
        if (datum->IsGreyedOut() || !datum->IsEnabled()) {
            if (!FEngIsScriptSet(FEngObject, scripts->GetGreyHash())) {
                FEngSetScript(FEngObject, scripts->GetGreyHash(), true);
            }
        } else {
            if (isSelected) {
                if (!FEngIsScriptSet(FEngObject, scripts->GetHighlightHash())) {
                    FEngSetScript(FEngObject, scripts->GetHighlightHash(), true);
                }
            } else {
                if (FEngIsScriptSet(FEngObject, scripts->GetHighlightHash())) {
                    FEngSetScript(FEngObject, scripts->GetUnHighlightHash(), true);
                } else {
                    if (!FEngIsScriptSet(FEngObject, scripts->GetNormalHash())) {
                        FEngSetScript(FEngObject, scripts->GetNormalHash(), true);
                    }
                }
            }
        }
    }
}

ImageArraySlot::ImageArraySlot(FEImage *img) : ArraySlot(img) {}

void ImageArraySlot::SetTexture(uint32 tex_hash) {
    FEngSetTextureHash(static_cast<FEImage *>(GetFEngObject()), tex_hash);
}

void ImageArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum != nullptr) {
        SetTexture(datum->GetHash());
    }
}

ArrayDatum::ArrayDatum(uint32 hash, uint32 desc) : hash(hash), desc(desc) {
    SetEnabled(true);
    SetGreyedOut(false);
    SetLocked(false);
    SetChecked(false);
}

ArrayScroller::ArrayScroller(const char *pkg_name, int w, int h, bool selectable)
    : ScrollBar(pkg_name, "scrollbar", true, true, false), //
      bShouldPlaySound(false),                             //
      currentDatum(nullptr),                               //
      startDatum(0),                                       //
      pkg_name(pkg_name),                                  //
      bSelectableArray(selectable),                        //
      bInClickToSelectMode(false) {
    SetMouseDownMsg(__BUTTON_PRESSED__);
    SetDimensions(w, h);
    SetDescLabel(0);
    SetScrollRegion(FEngFindObject(pkg_name, FEHashUpper("ARRAY_SCROLL_REGION")));
    pkg = cFEng::Get()->FindPackage(GetPkgName());
}

void ArrayScroller::RefreshHeader() {
    for (int i = 0; i < GetNumSlots(); i++) {
        ArrayDatum *datum = GetDatumAt(startDatum + i);
        ArraySlot *slot = GetSlotAt(i);
        if (slot != nullptr) {
            slot->Update(datum, GetCurrentDatum() == datum);
        }
    }
    if (currentDatum != nullptr) {
        FEngSetLanguageHash(GetPkgName(), descLabel, currentDatum->GetDesc());
    }
}

void ArrayScroller::AddSlot(ArraySlot *slot) {
    slot->SetScripts(this->GetScripts());
    slots.AddTail(slot);
}

void ArrayScroller::AddDatum(ArrayDatum *datum) {
    data.AddTail(datum);
    if (currentDatum == nullptr) {
        SetSelection(datum, 0);
    }
}

void ArrayScroller::SetSelection(ArrayDatum *newDatum, int newStartDatum) {
    if (newDatum->IsEnabled()) {
        startDatum = newStartDatum;
        currentDatum = newDatum;
        if (bSelectableArray) {
            ArraySlot *pSlot = GetSlotAt(GetCurrentDatumNum() - (startDatum + 1));
            if (pSlot != nullptr) {
                FEngSetCurrentButton(GetPkgName(), pSlot->GetFEngObject());
            }
        }
    }
}

// UNSOLVED
int ArrayScroller::ForceSelectionOnScreen(int new_datum, int start) {
    int w = GetWidth();
    int h = GetHeight();
    int ret = start;
    if (new_datum < start) {
        ret = new_datum / w;
    } else if (new_datum > start + w * h) {
        ret = (new_datum / w - (h - 1));
    }
    return ret * w;
}

void ArrayScroller::ScrollHor(eScrollDir dir) {
    if (data.CountElements() == 0) {
        return;
    }

    ArrayDatum *new_datum = currentDatum;
    int current_num = GetCurrentDatumNum() - 1;
    int new_index = current_num;
    if (dir == eSD_PREV) {
        new_index = new_index - 1;
        if ((new_index + 1) == ((new_index + 1) / width) * width) {
            new_index = new_index + width;
        }
        if (new_index >= GetNumDatum()) {
            new_index = GetNumDatum() - 1;
        }
    } else if (dir == eSD_NEXT) {
        new_index = current_num + 1;
        if ((new_index) == ((new_index) / width) * width) {
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

// UNSOLVED
void ArrayScroller::ScrollVer(eScrollDir dir) {
    if (data.CountElements() == 0) {
        return;
    }
    ArrayDatum *new_datum = currentDatum;
    int new_index = GetCurrentDatumNum() - 1;
    int new_start = startDatum;
    if (dir == eSD_PREV) {
        new_index = new_index - width;
        if (pScrollRegion == nullptr) {
            if (new_index >= (height - 1) * width && new_index < height * width) {
                new_index = new_index - (height - 1) * width;
            }
            new_start = new_start - width;
        } else if (new_index < new_start) {
            new_start = new_start - width;
        }
        if (new_index > -1) {
            for (int i = 0; i < GetCurrentDatumNum() - 1 - new_index; i++) {
                new_datum = new_datum->GetPrev();
            }
        }
    } else if (dir == eSD_NEXT) {
        new_index = new_index + width;
        if (pScrollRegion == nullptr) {
            if (new_index >= width && new_index < width * 2) {
                new_index = new_index + (height - 1) * width;
            }
            new_start = new_start + width;
        } else {
            if (GetNumDatum() <= new_index) {
                if (((data.GetNodeNumber(currentDatum) - 1) / width + 1) * height < ((GetNumDatum() - 1) / width + 1) * height) {
                    new_index = GetNumDatum() - 1;
                }
            }
            if (new_start + slots.CountElements() <= new_index) {
                new_start = new_start + width;
            }
        }
        if (new_index < GetNumDatum()) {
            for (int i = 0; i < new_index - (data.GetNodeNumber(currentDatum) - 1); i++) {
                new_datum = new_datum->GetNext();
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
    if (index >= GetNumDatum()) {
        return;
    }
    int newStartDatum = 0;
    int size = GetWidth() * GetHeight();

    if (index > size - 1) {
        int new_index = (index / GetWidth() * GetWidth() - (size - 1)) / GetWidth() * GetWidth();
        newStartDatum = new_index + GetWidth();
        if (GetWidth() == 1) {
            newStartDatum--;
        }
    }

    SetSelection(GetDatumAt(index), newStartDatum);
    UpdateScrollbar();
}

void ArrayScroller::UpdateMouse() {}

void ArrayScroller::ClearData() {
    data.DeleteAllElements();
    startDatum = 0;
    currentDatum = nullptr;
}

void ArrayScroller::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    ArrayDatum *currentDatum = GetCurrentDatum();
    if (currentDatum != nullptr) {
        currentDatum->NotificationMessage(msg, pObj, param1, param2);
    }

    switch (msg) {
        case __PAD_LEFT__:
            ScrollLeft();
            break;
        case __PAD_RIGHT__:
            ScrollRight();
            break;
        case __PAD_UP__:
            ScrollUp();
            break;
        case __PAD_DOWN__:
            ScrollDown();
            break;
        case FEMSG_MOUSE_CHANGED:
            UpdateMouse();
            break;
    }
}

ArrayScrollerMenu::ArrayScrollerMenu(ScreenConstructorData *sd, int w, int h, bool selectable)
    : MenuScreen(sd), ArrayScroller(sd->PackageFilename, w, h, selectable) {}

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
