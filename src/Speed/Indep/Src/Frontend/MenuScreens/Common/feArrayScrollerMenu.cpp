#include "feArrayScrollerMenu.hpp"

extern void FEngSetTextureHash(FEImage *image, unsigned int hash);

// ============================================================
// ArraySlot
// ============================================================

ArraySlot::ArraySlot(FEObject *obj)
    : FEngObject(obj) //
    , scripts(nullptr) {
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

// ============================================================
// ArrayScroller
// ============================================================

void ArrayScroller::AddSlot(ArraySlot *slot) {
    slot->SetScripts(&scripts);
    slots.AddTail(slot);
}

void ArrayScroller::UpdateMouse() {}

// ============================================================
// ArrayScrollerMenu
// ============================================================

void ArrayScrollerMenu::NotificationMessage(u32 msg, FEObject *pObj, u32 param1, u32 param2) {
    ArrayScroller::NotificationMessage(msg, pObj, param1, param2);
}

void ArrayScrollerMenu::RefreshHeader() {
    ArrayScroller::RefreshHeader();
}

ArrayDatum::ArrayDatum(uint32 h, uint32 d)
    : hash(h) //
    , desc(d) //
    , enabled(true) //
    , greyedOut(false) //
    , locked(false) //
    , checked(false)
{
}

void ImageArraySlot::Update(ArrayDatum *datum, bool isSelected) {
    ArraySlot::Update(datum, isSelected);
    if (datum) {
        SetTexture(datum->GetHash());
    }
}