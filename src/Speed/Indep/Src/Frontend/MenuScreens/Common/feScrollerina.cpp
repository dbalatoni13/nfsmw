#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

unsigned int Scrollerina::GetNodeIndex(ScrollerDatum* datum) {
    ScrollerDatum* node = Data.GetHead();
    unsigned int index = 1;
    while (node != Data.EndOfList()) {
        if (datum == node) return index;
        index++;
        node = node->GetNext();
    }
    return 0;
}

unsigned int Scrollerina::GetNodeIndex(ScrollerSlot* slot) {
    ScrollerSlot* node = Slots.GetHead();
    unsigned int index = 1;
    while (node != Slots.EndOfList()) {
        if (slot == node) return index;
        index++;
        node = node->GetNext();
    }
    return 0;
}

void Scrollerina::AddData(ScrollerDatum* datum) {
    Data.AddTail(datum);
    iNumData++;
    if (!TopDatum) {
        iViewHeadDataIndex = 1;
        TopDatum = Data.GetHead();
    }
    if (!SelectedDatum) {
        SelectedDatum = Data.GetHead();
    }
}

ScrollerDatum* Scrollerina::FindDatumInSlot(ScrollerSlot* to_find) {
    ScrollerSlot* slot_node = Slots.GetHead();
    if (slot_node == Slots.EndOfList() || Data.GetHead() == Data.EndOfList() || !to_find) {
        return nullptr;
    }
    ScrollerDatum* datum_node = TopDatum;
    while (slot_node != Slots.EndOfList()) {
        if (slot_node == to_find) return datum_node;
        if (datum_node == Data.EndOfList()) return nullptr;
        datum_node = datum_node->GetNext();
        slot_node = slot_node->GetNext();
    }
    return nullptr;
}

void Scrollerina::DrawScrollBar() {
    if (bHasScrollBar) {
        ScrollBar.Update(iNumSlots, iNumData, iViewHeadDataIndex, GetNodeIndex(SelectedDatum));
    }
}

void Scrollerina::Update(bool print) {
    if (print) {
        Print();
    }
    DrawScrollBar();
}

void ScrollerSlot::SetScript(unsigned int script_hash) {
    ScrollerSlotNode *node = FEStrings.GetHead();
    while (node != FEStrings.EndOfList()) {
        FEngSetScript(node->String, script_hash, true);
        node = node->GetNext();
    }
    if (pBacking) {
        FEngSetScript(pBacking, script_hash, true);
    }
}

void ScrollerSlot::Show() {
    if (!FEStrings.IsEmpty()) {
        ScrollerSlotNode *node = FEStrings.GetHead();
        while (node != FEStrings.EndOfList()) {
            FEngSetVisible(node->String);
            node = node->GetNext();
        }
    }
    FEngSetVisible(pBacking);
}

void ScrollerSlot::Hide() {
    if (!FEStrings.IsEmpty()) {
        ScrollerSlotNode *node = FEStrings.GetHead();
        while (node != FEStrings.EndOfList()) {
            FEngSetInvisible(node->String);
            node = node->GetNext();
        }
    }
    FEngSetInvisible(pBacking);
}

void Scrollerina::AddSlot(ScrollerSlot *slot) {
    Slots.AddTail(slot);
    iNumSlots++;
    if (!SelectedSlot) {
        SelectedSlot = Slots.GetHead();
    }
    slot->FindSize();
}

ScrollerSlot *Scrollerina::FindSlotWithDatum(ScrollerDatum *to_find) {
    if (Slots.IsEmpty() || Data.IsEmpty()) {
        return nullptr;
    }
    ScrollerSlot *slot = Slots.GetHead();
    ScrollerDatum *datum = TopDatum;
    while (slot != Slots.EndOfList()) {
        if (datum == to_find) {
            return slot;
        }
        if (datum == Data.EndOfList()) {
            return nullptr;
        }
        datum = datum->GetNext();
        slot = slot->GetNext();
    }
    return nullptr;
}

void Scrollerina::SetDisabledScripts() {
    ScrollerSlot *slot = Slots.GetHead();
    ScrollerDatum *datum = TopDatum;
    while (slot != Slots.EndOfList()) {
        if (datum->IsEnabled()) {
            slot->bEnabled = true;
        } else {
            slot->bEnabled = false;
        }
        datum = datum->GetNext();
        slot = slot->GetNext();
    }
}

void Scrollerina::Enable(ScrollerDatum *datum) {
    if (!datum) {
        return;
    }
    datum->bEnabled = true;
    if (Slots.IsEmpty() || Data.IsEmpty()) {
        return;
    }
    ScrollerSlot *slot = FindSlotWithDatum(datum);
    if (slot) {
        slot->bEnabled = true;
    }
}

extern FEObject *FEngFindObject(const char *pkg_name, unsigned int hash);
extern unsigned long FEHashUpper(const char *str);
extern int FEngSNPrintf(char *dest, int size, const char *fmt, ...);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern void FEngGetTopLeft(FEObject *object, float &x, float &y);
extern void FEngSetTopLeft(FEObject *object, float x, float y);
extern void FEngGetSize(FEObject *object, float &x, float &y);
extern void FEngSetSize(FEObject *object, float x, float y);

FEScrollBar::FEScrollBar(const char *parent_pkg, const char *name, bool vert, bool resize, bool arrows_only) {
    bVertical = vert;
    bResizeHandle = resize;
    bArrowsOnly = arrows_only;
    bHandleGrabbed = false;
    bVisible = false;
    vGrabbedPos = bVector2(0.0f, 0.0f);
    vCurPos = bVector2(0.0f, 0.0f);
    vGrabOffset = bVector2(0.0f, 0.0f);
    vBackingPos = bVector2(0.0f, 0.0f);
    vBackingSize = bVector2(0.0f, 0.0f);
    vHandleMinSize = bVector2(0.0f, 0.0f);
    fSegSize = 0.0f;
    pBacking = nullptr;
    pHandle = nullptr;
    pFirstArrow = nullptr;
    pSecondArrow = nullptr;
    pFirstBackingEnd = nullptr;
    pSecondBackingEnd = nullptr;

    if (name) {
        char sztemp[32];
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Backing");
        pBacking = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Handle");
        pHandle = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Arrow_1");
        pFirstArrow = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Arrow_2");
        pSecondArrow = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Backing_End_1");
        pFirstBackingEnd = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngSNPrintf(sztemp, 32, "%s%s", name, "_Backing_End_2");
        pSecondBackingEnd = FEngFindObject(parent_pkg, FEHashUpper(sztemp));
        FEngGetTopLeft(pBacking, vBackingPos.x, vBackingPos.y);
        FEngGetSize(pBacking, vBackingSize.x, vBackingSize.y);
        if (!bResizeHandle) {
            FEngGetSize(pHandle, vHandleMinSize.x, vHandleMinSize.y);
        } else {
            if (!bVertical) {
                vHandleMinSize.x = vBackingSize.y;
            } else {
                vHandleMinSize.x = vBackingSize.x;
            }
            vHandleMinSize.y = vHandleMinSize.x;
        }
    }
}

void FEScrollBar::SetGroupVisible(bool visible) {
    if (visible) {
        bVisible = true;
        SetVisible(pBacking);
        SetVisible(pHandle);
        SetVisible(pFirstArrow);
        SetVisible(pSecondArrow);
    } else {
        bVisible = false;
        SetInvisible(pBacking);
        SetInvisible(pHandle);
        SetInvisible(pFirstArrow);
        SetInvisible(pSecondArrow);
        SetInvisible(pFirstBackingEnd);
        SetInvisible(pSecondBackingEnd);
    }
}

void FEScrollBar::Update(int num_view_items, int num_list_items, int view_head_index, int selected_item) {
    if (selected_item == -1) {
        selected_item = view_head_index;
    }
    if (num_list_items <= num_view_items || num_list_items == 0) {
        SetGroupVisible(false);
    } else {
        SetGroupVisible(true);
        SetPosResized(num_view_items, num_list_items, view_head_index);
        if (selected_item == 1) {
            SetArrow1Dim(true);
        } else if (selected_item == num_list_items) {
            SetArrow2Dim(true);
        }
    }
}

void FEScrollBar::SetArrowVisibility(int arrow_num, bool visible) {
    if (visible) {
        if (arrow_num == 1) {
            SetVisible(pFirstArrow);
        } else if (arrow_num == 2) {
            SetVisible(pSecondArrow);
        }
    } else {
        if (arrow_num == 1) {
            SetInvisible(pFirstArrow);
        } else if (arrow_num == 2) {
            SetInvisible(pSecondArrow);
        }
    }
}

void FEScrollBar::SetVisible(FEObject *obj) {
    FEngSetVisible(obj);
    FEngSetScript(obj, 0x001CA7C0, true);
}

void FEScrollBar::SetInvisible(FEObject *obj) {
    FEngSetInvisible(obj);
    FEngSetScript(obj, 0x0016A259, true);
}

void FEScrollBar::SetArrow1Dim(bool dim) {
    FEObject *arrow = pFirstArrow;
    unsigned int hash = 0x6EBBFB68;
    if (dim) {
        hash = 0x9E99;
    }
    FEngSetScript(arrow, hash, true);
}

void FEScrollBar::SetArrow2Dim(bool dim) {
    FEObject *arrow = pSecondArrow;
    unsigned int hash = 0x6EBBFB68;
    if (dim) {
        hash = 0x9E99;
    }
    FEngSetScript(arrow, hash, true);
}

inline float FEngGetSizeX(FEObject *obj) {
    float x, y;
    FEngGetSize(obj, x, y);
    return x;
}

inline void FEngSetSizeY(FEObject *obj, float y) {
    float x = FEngGetSizeX(obj);
    FEngSetSize(obj, x, y);
}

inline void FEngSetTopLeftY(FEObject *obj, float y) {
    float x = FEngGetTopLeftX(obj);
    FEngSetTopLeft(obj, x, y);
}

void FEScrollBar::SetPosResized(int num_view_items, int num_list_items, int view_head_index) {
    if (bVertical) {
        float barsize = (static_cast<float>(num_view_items) / static_cast<float>(num_list_items)) * vBackingSize.y;
        FEngSetSizeY(pHandle, barsize);
        float range = static_cast<float>(num_list_items) - static_cast<float>(num_view_items);
        float num_segs = (vBackingSize.y - barsize) / bMax(1.0f, range);
        fSegSize = num_segs;
        float view_dist_to_head = (static_cast<float>(view_head_index) - 1.0f) * num_segs + vBackingPos.y;
        vCurPos.y = view_dist_to_head;
        if (!bHandleGrabbed) {
            FEngSetTopLeftY(pHandle, view_dist_to_head);
        }
    } else {
        float barsize = (static_cast<float>(num_view_items) / static_cast<float>(num_list_items)) * vBackingSize.x;
        FEngSetSizeX(pHandle, barsize);
        float range = static_cast<float>(num_list_items) - static_cast<float>(num_view_items);
        float num_segs = (vBackingSize.x - barsize) / bMax(1.0f, range);
        fSegSize = num_segs;
        float view_dist_to_head = (static_cast<float>(view_head_index) - 1.0f) * num_segs + vBackingPos.x;
        vCurPos.x = view_dist_to_head;
        if (!bHandleGrabbed) {
            FEngSetTopLeftX(pHandle, view_dist_to_head);
        }
    }
}
