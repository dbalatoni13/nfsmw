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
