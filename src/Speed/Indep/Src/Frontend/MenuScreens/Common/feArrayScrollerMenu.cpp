#include "feArrayScrollerMenu.hpp"

#include "Speed/Indep/Src/FEng/cFEng.h"

extern void FEngSetTextureHash(FEImage *image, unsigned int hash);
extern void FEngSetVisible(FEObject *object);
extern void FEngSetInvisible(FEObject *object);
extern bool FEngIsScriptSet(FEObject *object, unsigned int hash);
extern void FEngSetScript(FEObject *object, unsigned int hash, bool play);
extern unsigned long FEHashUpper(const char *string);
extern FEObject *FEngFindObject(const char *pkg_name, unsigned int obj_hash);
extern void FEngSetLanguageHash(const char *pkg_name, unsigned int obj_hash, unsigned int language);
extern void FEngSetCurrentButton(const char *pkg_name, unsigned int hash);

// ============================================================
// ArrayScripts
// ============================================================

ArrayScripts::ArrayScripts() {
    SetNormalHash(FEHashUpper("INIT"));
    SetGreyHash(FEHashUpper("GREY"));
    SetHighlightHash(FEHashUpper("HIGHLIGHT"));
    SetUnHighlightHash(FEHashUpper("UNHIGHLIGHT"));
}

// ============================================================
// ArraySlot
// ============================================================

ArraySlot::ArraySlot(FEObject *obj)
    : FEngObject(obj) //
    , scripts(nullptr) {
}

void ArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    if (!datum) {
        FEngSetInvisible(GetFEngObject());
    } else {
        FEngSetVisible(GetFEngObject());
        if (!datum->IsGreyedOut() && datum->IsEnabled()) {
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
        } else {
            if (!FEngIsScriptSet(GetFEngObject(), scripts->GetGreyHash())) {
                FEngSetScript(GetFEngObject(), scripts->GetGreyHash(), true);
            }
        }
    }
}

// ============================================================
// ImageArraySlot
// ============================================================

ImageArraySlot::ImageArraySlot(FEImage *img)
    : ArraySlot(static_cast< FEObject * >(static_cast< void * >(img))) {
}

void ImageArraySlot::SetTexture(unsigned int tex_hash) {
    FEngSetTextureHash(static_cast< FEImage * >(GetFEngObject()), tex_hash);
}

void ImageArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum) {
        SetTexture(datum->GetHash());
    }
}

// ============================================================
// ArrayDatum
// ============================================================

ArrayDatum::ArrayDatum(uint32 h, uint32 d)
    : hash(h) //
    , desc(d) //
    , enabled(true) //
    , greyedOut(false) //
    , locked(false) //
    , checked(false) {
}

// ============================================================
// ArrayScroller
// ============================================================

ArrayScroller::ArrayScroller(const char *name, int w, int h, bool selectable)
    : bShouldPlaySound(false) //
    , currentDatum(nullptr) //
    , startDatum(0) //
    , width(w) //
    , height(h) //
    , descLabel(0) //
    , pkg_name(name) //
    , pScrollRegion(nullptr) //
    , bSelectableArray(selectable) //
    , mouseDownMsg(0) //
    , bInClickToSelectMode(false) //
{
    ScrollBar.~FEScrollBar();
    new (&ScrollBar) FEScrollBar(name, "scrollbar", true, true, false);
    scripts.~ArrayScripts();
    new (&scripts) ArrayScripts();
    pScrollRegion = FEngFindObject(name, FEHashUpper("ARRAY_SCROLL_REGION"));
    pkg = cFEng::mInstance->FindPackage(name);
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
        currentDatum = datum;
    }
}

void ArrayScroller::SetSelection(ArrayDatum *newDatum, int newStartDatum) {
    if (newDatum->IsEnabled()) {
        startDatum = newStartDatum;
        currentDatum = newDatum;
        if (bSelectableArray) {
            ArraySlot *pSlot = GetSlotAt(data.GetNodeNumber(currentDatum) - startDatum);
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
        ret = new_datum / w;
    } else if (new_datum > start + w * h) {
        ret = (new_datum / w - (h - 1));
    }
    return ret * w;
}

void ArrayScroller::ScrollHor(eScrollDir dir) {
    int num_data = data.CountElements();
    if (num_data == 0) {
        return;
    }
    ArrayDatum *new_datum = currentDatum;
    int current_num = data.GetNodeNumber(currentDatum) - 1;
    int new_index = current_num;
    if (dir == eSD_PREV) {
        int w = width;
        new_index = current_num - 2;
        if (current_num == (current_num / w) * w) {
            new_index = new_index + w;
        }
        int num_data2 = data.CountElements();
        if (num_data2 <= new_index) {
            new_index = data.CountElements() - 1;
        }
    } else if (dir == eSD_NEXT) {
        int w = width;
        new_index = current_num;
        if (current_num == (current_num / w) * w) {
            new_index = current_num - w;
        }
        int num_data2 = data.CountElements();
        if (num_data2 <= new_index) {
            new_index = (new_index / width) * width;
        }
    }
    if (new_index > current_num && new_index < data.CountElements()) {
        ArrayDatum *old = currentDatum;
        do {
            current_num++;
            new_datum = static_cast< ArrayDatum * >(new_datum->GetNext());
        } while (current_num < new_index);
    } else if (new_index < current_num && new_index > -1) {
        ArrayDatum *old = currentDatum;
        for (int i = new_index; i < current_num; i++) {
            new_datum = static_cast< ArrayDatum * >(new_datum->GetPrev());
        }
    }
    if (new_datum != currentDatum) {
        int forced = ForceSelectionOnScreen(new_index, startDatum);
        SetSelection(new_datum, forced);
        bShouldPlaySound = true;
    }
    RefreshHeader();
}

void ArrayScroller::ScrollVer(eScrollDir dir) {
    int num_data = data.CountElements();
    if (num_data == 0) {
        return;
    }
    ArrayDatum *new_datum = currentDatum;
    int current_num = data.GetNodeNumber(currentDatum) - 1;
    int new_start = startDatum;
    int new_index;
    if (dir == eSD_PREV) {
        int w = width;
        new_index = current_num - w;
        if (!pScrollRegion) {
            int last_row_start = (height - 1) * w;
            if (new_index >= last_row_start && new_index < height * w) {
                new_index = new_index - last_row_start;
            }
            new_start = new_start - w;
        } else if (new_index < new_start) {
            new_start = new_start - w;
        }
        if (new_index > -1) {
            int old_num = data.GetNodeNumber(currentDatum) - 1;
            for (int i = 0; i < old_num - new_index; i++) {
                new_datum = static_cast< ArrayDatum * >(new_datum->GetPrev());
            }
        }
    } else if (dir == eSD_NEXT) {
        int w = width;
        new_index = current_num + w;
        if (!pScrollRegion) {
            if (new_index >= w && new_index < w * 2) {
                new_index = new_index + (height - 1) * w;
            }
            new_start = new_start + w;
        } else {
            int total = data.CountElements();
            if (total <= new_index) {
                int total2 = data.CountElements();
                int w2 = width;
                int h2 = height;
                int cur_page = (data.GetNodeNumber(currentDatum) - 1) / width + 1;
                if (cur_page * height < ((total2 - 1) / w2 + 1) * h2) {
                    new_index = data.CountElements() - 1;
                }
            }
            int num_slots = slots.CountElements();
            if (new_start + num_slots <= new_index) {
                new_start = new_start + width;
            }
        }
        if (new_index < data.CountElements()) {
            int old_num = data.GetNodeNumber(currentDatum);
            for (int i = 0; i < new_index - (old_num - 1); i++) {
                new_datum = static_cast< ArrayDatum * >(new_datum->GetNext());
            }
        }
    }
    if (new_datum != currentDatum) {
        int forced = ForceSelectionOnScreen(new_index, new_start);
        SetSelection(new_datum, forced);
        UpdateScrollbar();
        bShouldPlaySound = true;
    }
    RefreshHeader();
}

void ArrayScroller::UpdateScrollbar() {
    int h = height;
    int num_data = data.CountElements();
    int w = GetWidth();
    int top_item = startDatum / w + 1;
    int selected_item = data.GetNodeNumber(currentDatum) / width + 1;
    ScrollBar.Update(h, (num_data - 1) / w + 1, top_item, selected_item);
}

ArraySlot *ArrayScroller::GetSlotAt(int index) {
    if (index < GetNumSlots()) {
        return slots.GetNode(index);
    }
    return nullptr;
}

ArrayDatum *ArrayScroller::GetDatumAt(int index) {
    if (index < GetNumDatum()) {
        return data.GetNode(index);
    }
    return nullptr;
}

void ArrayScroller::SetInitialPosition(int index) {
    if (GetNumDatum() == 0) {
        UpdateScrollbar();
    }
    if (index < GetNumDatum()) {
        int size = width * height;
        int newStartDatum = 0;
        if (index > size - 1) {
            int new_start = ((index / width * width + 1) - size) / width * width;
            newStartDatum = new_start + width;
            if (width == 1) {
                newStartDatum = new_start;
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
    startDatum = 0;
    currentDatum = nullptr;
}

void ArrayScroller::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    ArrayDatum *currentDatum = GetCurrentDatum();
    if (currentDatum) {
        currentDatum->NotificationMessage(msg, pObj, param1, param2);
    }
    if (msg == 0x9120409E) {
        ScrollHor(eSD_PREV);
    } else if (msg == 0x72619778) {
        ScrollVer(eSD_PREV);
    } else if (msg == 0x911C0A4B) {
        ScrollVer(eSD_NEXT);
    } else if (msg == 0x9803F6E2) {
        UpdateMouse();
    } else if (msg == 0xB5971BF1) {
        ScrollHor(eSD_NEXT);
    }
}

// ============================================================
// ArrayScrollerMenu
// ============================================================

ArrayScrollerMenu::ArrayScrollerMenu(ScreenConstructorData *sd, int w, int h, bool selectable)
    : MenuScreen(sd) //
    , ArrayScroller(sd->PackageFilename, w, h, selectable) //
{
}

void ArrayScrollerMenu::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    ArrayScroller::NotificationMessage(msg, pObj, param1, param2);
}

eMenuSoundTriggers ArrayScrollerMenu::NotifySoundMessage(u32 msg, eMenuSoundTriggers maybe) {
    if (msg == 0x9120409E || msg == 0xB5971BF1 || msg == 0x911C0A4B ||
        msg == 0x72619778 || msg == 0x480DF13F ||
        msg == 0xB205316C || msg == 0x48122792 || msg == 0x4AC5E165) {
        if (!bShouldPlaySound) {
            maybe = static_cast< eMenuSoundTriggers >(-1);
        }
        bShouldPlaySound = false;
    }
    return maybe;
}

void ArrayScrollerMenu::RefreshHeader() {
    ArrayScroller::RefreshHeader();
}