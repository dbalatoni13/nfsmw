#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feScrollerina.hpp"

#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"

extern EAXSound *g_pEAXSound;
extern FEImage *FEngFindImage(const char *pkg_name, int hash);
extern void FEngGetTopLeft(FEObject *object, float &x, float &y);
extern void FEngGetSize(FEObject *object, float &x, float &y);
extern unsigned long FEHashUpper(const char *string);
extern int FEPrintf(FEString *text, const char *fmt, ...);
extern void FEngSetLanguageHash(FEString *text, unsigned int hash);

inline float FEngGetSizeX(FEObject *obj) {
    float x, y;
    FEngGetSize(obj, x, y);
    return x;
}

Scrollerina::Scrollerina(const char *parent_pkg, const char *backing, const char *scrollbar,
                          bool vert, bool resize, bool wrapped, bool alwaysShowBacking)
    : pParentPkg(parent_pkg) //
    , iNumSlots(0) //
    , iNumData(0) //
    , iViewHeadDataIndex(0) //
    , SelectedDatum(nullptr) //
    , TopDatum(nullptr) //
    , SelectedSlot(nullptr) //
    , pBacking(nullptr) //
    , ScrollBar(parent_pkg, scrollbar, vert, resize, false) //
    , vTopLeft(0.0f, 0.0f) //
    , vSize(0.0f, 0.0f) //
    , bHasScrollBar(true) //
    , bViewNeedsSync(false) //
    , bWrapped(wrapped) //
    , bAlwaysShowBacking(alwaysShowBacking) //
    , bVertical(vert) //
    , mouseDownMsg(0x406415e3) //
    , bInClickToSelectMode(false)
    , pScrollRegion(nullptr)
{
    if (!backing) {
        bHasScrollBar = false;
    } else {
        unsigned int hash = FEHashUpper(backing);
        pBacking = FEngFindImage(parent_pkg, hash);
        FEngGetTopLeft(pBacking, vTopLeft.x, vTopLeft.y);
        FEngGetSize(pBacking, vSize.x, vSize.y);
    }
}

void Scrollerina::FindSize() {
    bVector2 top_left;
    bVector2 size;
    ScrollerSlot *slot;
    float top;
    float bottom;
    float left;
    float right;

    if (!pBacking) {
        slot = Slots.GetHead();
        slot->GetTopLeft(top_left);
        slot->GetSize(size);
        top = top_left.y;
        bottom = top + vSize.y;
        left = top_left.x;
        right = left + vSize.x;

        while (slot != Slots.EndOfList()) {
            slot->GetTopLeft(top_left);
            slot->GetSize(size);
            left = bMin(left, top_left.x);
            top = bMin(top, top_left.y);
            bottom = bMax(bottom, top_left.y + size.y);
            right = bMax(right, top_left.x + size.x);
            slot = slot->GetNext();
        }

        if (pScrollRegion) {
            FEngGetTopLeft(pScrollRegion, top_left.x, top_left.y);
            FEngGetSize(pScrollRegion, size.x, size.y);
            top = bMin(top, top_left.y);
            bottom = bMax(bottom, top_left.y + size.y);
            left = bMin(left, top_left.x);
            right = bMax(right, top_left.x + size.x);
        }

        vTopLeft.x = left;
        vSize.x = bAbs(right - left);
        vSize.y = bAbs(bottom - top);
        vTopLeft.y = top;
    }
}

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
        ScrollBar.Update(iNumSlots, iNumData, iViewHeadDataIndex, GetSelectedNodeIndex());
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

void ScrollerSlot::FindSize() {
    float top = 0.0f;
    float left = 0.0f;
    float right = 0.0f;
    float bottom = 0.0f;
    if (pBacking) {
        top = FEngGetTopLeftY(pBacking);
        left = FEngGetTopLeftX(pBacking);
        right = left + FEngGetSizeX(pBacking);
        bottom = top + FEngGetSizeY(pBacking);
    }
    vTopLeft.x = left;
    vSize.x = bAbs(left - right);
    vSize.y = bAbs(top - bottom);
    vTopLeft.y = top;
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
extern int FEngSNPrintf(char *dest, int size, const char *fmt, ...);
extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);
extern void FEngSetScript(FEObject *object, unsigned int script_hash, bool start_at_beginning);
extern void FEngSetTopLeft(FEObject *object, float x, float y);
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
    FEngSetScript(pFirstArrow, dim ? 0x9E99 : 0x6EBBFB68, true);
}

void FEScrollBar::SetArrow2Dim(bool dim) {
    FEngSetScript(pSecondArrow, dim ? 0x9E99 : 0x6EBBFB68, true);
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

void Scrollerina::ScrollNext() {
    if (bWrapped) {
        g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0));
        ScrollWrapped(eSD_NEXT);
    } else {
        eMenuSoundTriggers snd = static_cast<eMenuSoundTriggers>(0);
        if (SelectedDatum == GetLastDatum()) {
            snd = static_cast<eMenuSoundTriggers>(7);
        }
        g_pEAXSound->PlayUISoundFX(snd);
        Scroll(eSD_NEXT);
    }
}

void Scrollerina::ScrollPrev() {
    if (bWrapped) {
        g_pEAXSound->PlayUISoundFX(static_cast<eMenuSoundTriggers>(0));
        ScrollWrapped(eSD_PREV);
    } else {
        eMenuSoundTriggers snd = static_cast<eMenuSoundTriggers>(0);
        if (SelectedDatum == GetFirstDatum()) {
            snd = static_cast<eMenuSoundTriggers>(7);
        }
        g_pEAXSound->PlayUISoundFX(snd);
        Scroll(eSD_PREV);
    }
}

bool Scrollerina::Scroll(eScrollDir dir) {
    bool ret = false;
    if (Slots.IsEmpty() || Data.IsEmpty()) {
        return false;
    }

    if (bViewNeedsSync) {
        SyncViewToSelection();
    } else {
        ScrollerDatum *new_datum = SelectedDatum;
        ScrollerDatum *new_view = TopDatum;
        unsigned int new_view_head = iViewHeadDataIndex;

        if (dir == eSD_NEXT) {
            do {
                if (new_datum == GetLastDatum()) return false;
                new_datum = new_datum->GetNext();
                unsigned int idx = GetNodeIndex(new_datum);
                if (idx >= new_view_head + iNumSlots) {
                    new_view = new_view->GetNext();
                    new_view_head++;
                }
            } while (!new_datum->bEnabled);
        } else if (dir == eSD_PREV) {
            do {
                if (new_datum == GetFirstDatum()) return false;
                new_datum = new_datum->GetPrev();
                if (new_datum == new_view->GetPrev()) {
                    new_view_head--;
                    new_view = new_datum;
                }
            } while (!new_datum->bEnabled);
        }

        if (new_datum != SelectedDatum) {
            SelectedDatum = new_datum;
            iViewHeadDataIndex = new_view_head;
            TopDatum = new_view;
            ret = true;
            ScrollSelection(dir);
            SetDisabledScripts();
        }
    }
    Update(true);
    return ret;
}

bool Scrollerina::ScrollWrapped(eScrollDir dir) {
    bool ret = false;
    if (Slots.IsEmpty() || Data.IsEmpty()) {
        return false;
    }

    if (bViewNeedsSync) {
        SyncViewToSelection();
    } else {
        ScrollerDatum *new_datum = SelectedDatum;
        ScrollerDatum *new_view = TopDatum;
        unsigned int new_view_head = iViewHeadDataIndex;

        if (dir == eSD_NEXT) {
            do {
                if (new_datum == GetLastDatum()) {
                    new_view = GetFirstDatum();
                    new_view_head = 1;
                    new_datum = new_view;
                } else {
                    new_datum = new_datum->GetNext();
                    unsigned int idx = GetNodeIndex(new_datum);
                    if (idx >= new_view_head + iNumSlots) {
                        new_view = new_view->GetNext();
                        new_view_head++;
                    }
                }
            } while (!new_datum->bEnabled);
        } else if (dir == eSD_PREV) {
            do {
                if (new_datum == GetFirstDatum()) {
                    new_datum = GetLastDatum();
                    new_view_head = iNumData - iNumSlots + 1;
                    new_view = new_datum;
                } else {
                    new_datum = new_datum->GetPrev();
                    if (new_datum == new_view->GetPrev()) {
                        new_view_head--;
                        new_view = new_datum;
                    }
                }
            } while (!new_datum->bEnabled);
        }

        if (new_datum != SelectedDatum) {
            SelectedDatum = new_datum;
            iViewHeadDataIndex = new_view_head;
            TopDatum = new_view;
            ret = true;
            ScrollSelection(dir);
            SetDisabledScripts();
        }
    }
    Update(true);
    return ret;
}

bool Scrollerina::MoveSelected(eScrollDir dir, bool bprint) {
    bool ret = false;
    if (Slots.IsEmpty() || Data.IsEmpty()) {
        return false;
    }

    if (bViewNeedsSync) {
        SyncViewToSelection();
    } else {
        if (dir == eSD_NEXT) {
            eMenuSoundTriggers snd = static_cast<eMenuSoundTriggers>(0);
            if (SelectedDatum == GetLastDatum()) {
                snd = static_cast<eMenuSoundTriggers>(7);
            }
            g_pEAXSound->PlayUISoundFX(snd);
            ScrollerDatum *nextDatum = SelectedDatum;
            if (nextDatum == GetLastDatum()) return false;
            ScrollerDatum *removedDatum = nextDatum;
            removedDatum->Remove();
            nextDatum = SelectedDatum;
            removedDatum->AddAfter(nextDatum);
            if (TopDatum == SelectedDatum) {
                TopDatum = SelectedDatum->GetPrev();
            } else {
                unsigned int idx = GetNodeIndex(SelectedDatum);
                if (idx >= iViewHeadDataIndex + iNumSlots) {
                    iViewHeadDataIndex++;
                    TopDatum = TopDatum->GetNext();
                }
            }
        } else if (dir == eSD_PREV) {
            eMenuSoundTriggers snd = static_cast<eMenuSoundTriggers>(0);
            if (SelectedDatum == GetFirstDatum()) {
                snd = static_cast<eMenuSoundTriggers>(7);
            }
            g_pEAXSound->PlayUISoundFX(snd);
            ScrollerDatum *removedDatum = SelectedDatum;
            if (removedDatum == GetFirstDatum()) return false;
            removedDatum->Remove();
            ScrollerDatum *nextDatum = SelectedDatum;
            removedDatum->AddBefore(nextDatum);
            if (TopDatum == SelectedDatum) {
                TopDatum = SelectedDatum;
                iViewHeadDataIndex--;
            } else {
                ScrollerDatum *prev = TopDatum->GetPrev();
                if (prev != SelectedDatum) {
                    TopDatum = prev;
                }
            }
        }
        ScrollSelection(dir);
        ret = true;
    }
    Update(bprint);
    return ret;
}

bool Scrollerina::ScrollSelection(eScrollDir dir) {
    bool ret = false;
    ScrollerSlot *slot = SelectedSlot;

    if (dir == eSD_NEXT) {
        if (slot == Slots.GetTail()) {
            return false;
        }
        ScrollerDatum *datum = FindDatumInSlot(slot);
        do {
            datum = datum->GetNext();
            slot = slot->GetNext();
            if (!slot || slot == Slots.EndOfList()) break;
        } while (!datum->bEnabled);
    } else if (dir == eSD_PREV) {
        if (slot == Slots.GetHead()) {
            return false;
        }
        ScrollerDatum *datum = FindDatumInSlot(slot);
        do {
            datum = datum->GetPrev();
            slot = slot->GetPrev();
            if (!slot || slot == Slots.GetHead()) break;
        } while (!datum->bEnabled);
    }

    ScrollerSlot *old_slot = SelectedSlot;
    if (slot != old_slot) {
        old_slot->SetScript(0x7ab5521a);
        SelectedSlot = slot;
        slot->SetScript(0x249db7b7);
    }
    return slot != old_slot;
}

void Scrollerina::SyncViewToSelection() {
    if (Data.IsEmpty() || Slots.IsEmpty()) {
        return;
    }
    if (iNumSlots >= iNumData) {
        return;
    }

    unsigned int idx = GetNodeIndex(SelectedDatum);
    if (idx > iNumData - iNumSlots + 1) {
        TopDatum = Data.GetNode(iNumData - iNumSlots);
        SelectedSlot = FindSlotWithDatum(SelectedDatum);
    } else {
        TopDatum = SelectedDatum;
        SelectedSlot = Slots.GetHead();
    }

    bViewNeedsSync = false;
    SetDisabledScripts();
    if (SelectedSlot) {
        SelectedSlot->SetScript(0x249db7b7);
    }
    CountListIndices();
}

void Scrollerina::Print() {
    ScrollerDatum *datum = TopDatum;
    ScrollerSlot *slot = Slots.GetHead();

    while (slot != Slots.EndOfList()) {
        if (!datum || datum == Data.EndOfList()) {
            slot->Hide();
        } else {
            slot->Show();
            ScrollerDatumNode *dnode = datum->Strings.GetHead();
            ScrollerSlotNode *snode = slot->FEStrings.GetHead();
            while (snode != slot->FEStrings.EndOfList()) {
                if (!dnode->LanguageHash) {
                    FEPrintf(static_cast<FEString *>(snode->String), "%s", dnode->String);
                } else {
                    FEngSetLanguageHash(static_cast<FEString *>(snode->String), dnode->LanguageHash);
                }
                dnode = dnode->GetNext();
                snode = snode->GetNext();
                if (dnode == datum->Strings.EndOfList() || snode == slot->FEStrings.EndOfList()) break;
            }
            datum = datum->GetNext();
        }
        slot = slot->GetNext();
    }
}

void Scrollerina::CountListIndices() {
    bool found_view = false;
    iNumSlots = 0;
    iViewHeadDataIndex = 1;
    iNumData = 0;
    {
        ScrollerSlot *slot = Slots.GetHead();
        while (slot != Slots.EndOfList()) {
            iNumSlots++;
            slot = slot->GetNext();
        }
    }
    {
        ScrollerDatum *datum = Data.GetHead();
        while (datum != Data.EndOfList()) {
            iNumData++;
            if (!found_view && datum != TopDatum) {
                iViewHeadDataIndex++;
            } else {
                found_view = true;
            }
            datum = datum->GetNext();
        }
    }
}

void Scrollerina::SetSelected(ScrollerSlot *slot) {
    if (!slot) return;
    if (!slot->IsEnabled()) return;
    ScrollerDatum *datum = FindDatumInSlot(slot);
    if (!datum) return;
    UnHighlightSelected();
    SelectedDatum = datum;
    SelectedSlot = slot;
    HighlightSelected();
    Update(true);
    bViewNeedsSync = false;
}
