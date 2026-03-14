#include "feWidget.hpp"

struct FEObject;
void FEngSetVisible(FEObject* obj);
void FEngSetInvisible(FEObject* obj);
void FEngSetScript(FEObject* object, unsigned int script_hash, bool start_at_beginning);
void FEngGetTopLeft(FEObject* object, float& x, float& y);
void FEngSetTopLeft(FEObject* object, float x, float y);

FEWidget::FEWidget(FEObject* backing, bool enabled, bool hidden)
    : vTopLeft(0.0f, 0.0f) //
    , vSize(0.0f, 0.0f) //
    , vBackingOffset(0.0f, 0.0f) //
    , pBacking(backing) //
    , bEnabled(enabled) //
    , bHidden(hidden) //
    , bMovedLastUpdate(false) //
{}

void FEWidget::Act(const char* parent_pkg, unsigned int data) {}
void FEWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEWidget::Draw() {}
void FEWidget::Position() {}

void FEWidget::Show() {
    if (pBacking) {
        FEngSetVisible(pBacking);
    }
}

void FEWidget::Hide() {
    if (pBacking) {
        FEngSetInvisible(pBacking);
    }
}

void FEWidget::SetFocus(const char* parent_pkg) {}
void FEWidget::UnsetFocus() {}

FEButtonWidget::FEButtonWidget(bool enabled)
    : FEWidget(nullptr, enabled, false) //
    , pTitle(nullptr) //
    , vMaxTitleSize(0.0f, 0.0f) //
{}

void FEButtonWidget::Position() {
    float x, y;
    FEngGetTopLeft(pBacking, x, y);
    vTopLeft.x = x;
    vTopLeft.y = y;
    if (pTitle) {
        FEngGetTopLeft(static_cast<FEObject*>(pTitle), x, y);
        vMaxTitleSize.x = x;
        vMaxTitleSize.y = y;
    }
}

void FEButtonWidget::Show() {
    FEWidget::Show();
    if (pTitle) {
        FEngSetVisible(static_cast<FEObject*>(pTitle));
    }
}

void FEButtonWidget::Hide() {
    FEWidget::Hide();
    if (pTitle) {
        FEngSetInvisible(static_cast<FEObject*>(pTitle));
    }
}

void FEButtonWidget::CheckMouse(const char* parent_pkg, float mouse_x, float mouse_y) {}

void FEButtonWidget::SetFocus(const char* parent_pkg) {
    FEWidget::SetFocus(parent_pkg);
    if (pBacking) {
        FEngSetScript(pBacking, 0x37389004, true);
    }
}

void FEButtonWidget::UnsetFocus() {
    FEWidget::UnsetFocus();
    if (pBacking) {
        FEngSetScript(pBacking, 0x7AB5521A, true);
    }
}

void FEButtonWidget::SetPos(bVector2& pos) {
    FEWidget::SetPosX(pos.x);
    FEWidget::SetPosY(pos.y);
}

FEStatWidget::FEStatWidget(bool enabled)
    : FEWidget(nullptr, enabled, false) //
    , pTitle(nullptr) //
    , pData(nullptr) //
    , vMaxTitleSize(0.0f, 0.0f) //
    , vMaxDataSize(0.0f, 0.0f) //
    , vDataPos(0.0f, 0.0f) //
{}

void FEStatWidget::Act(const char* parent_pkg, unsigned int data) {}
void FEStatWidget::Draw() {}
void FEStatWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}

void FEStatWidget::Position() {
    float x, y;
    FEngGetTopLeft(pBacking, x, y);
    vTopLeft.x = x;
    vTopLeft.y = y;
    if (pTitle) {
        FEngGetTopLeft(static_cast<FEObject*>(pTitle), x, y);
        vMaxTitleSize.x = x;
        vMaxTitleSize.y = y;
    }
    if (pData) {
        FEngGetTopLeft(static_cast<FEObject*>(pData), x, y);
        vDataPos.x = x;
        vDataPos.y = y;
    }
}

void FEStatWidget::Show() {
    FEWidget::Show();
    if (pTitle) {
        FEngSetVisible(static_cast<FEObject*>(pTitle));
    }
    if (pData) {
        FEngSetVisible(static_cast<FEObject*>(pData));
    }
}

void FEStatWidget::Hide() {
    FEWidget::Hide();
    if (pTitle) {
        FEngSetInvisible(static_cast<FEObject*>(pTitle));
    }
    if (pData) {
        FEngSetInvisible(static_cast<FEObject*>(pData));
    }
}

void FEStatWidget::SetFocus(const char* parent_pkg) {
    FEWidget::SetFocus(parent_pkg);
}

void FEStatWidget::UnsetFocus() {
    FEWidget::UnsetFocus();
}

void FEStatWidget::SetPos(bVector2& pos) {
    SetPosX(pos.x);
    SetPosY(pos.y);
}

void FEStatWidget::SetPosX(float x) {
    float old_x = GetTopLeftX();
    SetTopLeftX(x);
    vDataPos.x = x + (vDataPos.x - old_x);
    if (pBacking) {
        float bx, by;
        FEngGetTopLeft(pBacking, bx, by);
        FEngSetTopLeft(pBacking, x - GetBackingOffsetX(), by);
    }
}

void FEStatWidget::SetPosY(float y) {
    float old_y = GetTopLeftY();
    SetTopLeftY(y);
    vDataPos.y = y + (vDataPos.y - old_y);
    if (pBacking) {
        float bx, by;
        FEngGetTopLeft(pBacking, bx, by);
        FEngSetTopLeft(pBacking, bx, y - GetBackingOffsetY());
    }
}

FEToggleWidget::FEToggleWidget(bool enabled)
    : FEStatWidget(enabled) //
    , pLeftImage(nullptr) //
    , pRightImage(nullptr) //
    , EnableScript(0x7AB5521A) //
    , DisableScript(0x36819D93) //
{}

void FEToggleWidget::CheckMouse(const char* parent_pkg, const float mouse_x, const float mouse_y) {}
void FEToggleWidget::BlinkArrows(unsigned int data) {}

void FEToggleWidget::Enable() {
    FEWidget::Enable();
    if (pLeftImage) {
        FEngSetScript(static_cast<FEObject*>(pLeftImage), EnableScript, true);
    }
    if (pRightImage) {
        FEngSetScript(static_cast<FEObject*>(pRightImage), EnableScript, true);
    }
}

void FEToggleWidget::Disable() {
    FEWidget::Disable();
    if (pLeftImage) {
        FEngSetScript(static_cast<FEObject*>(pLeftImage), DisableScript, true);
    }
    if (pRightImage) {
        FEngSetScript(static_cast<FEObject*>(pRightImage), DisableScript, true);
    }
}

void FEToggleWidget::SetScript(unsigned int script) {
    FEngSetScript(static_cast<FEObject*>(pTitle), script, true);
    FEngSetScript(static_cast<FEObject*>(pData), script, true);
    FEngSetScript(static_cast<FEObject*>(pLeftImage), script, true);
    FEngSetScript(static_cast<FEObject*>(pRightImage), script, true);
    if (pBacking) {
        FEngSetScript(pBacking, script, true);
    }
}

void FEToggleWidget::Show() {
    FEStatWidget::Show();
    if (pLeftImage) {
        FEngSetVisible(static_cast<FEObject*>(pLeftImage));
    }
    if (pRightImage) {
        FEngSetVisible(static_cast<FEObject*>(pRightImage));
    }
}

void FEToggleWidget::Hide() {
    FEStatWidget::Hide();
    if (pLeftImage) {
        FEngSetInvisible(static_cast<FEObject*>(pLeftImage));
    }
    if (pRightImage) {
        FEngSetInvisible(static_cast<FEObject*>(pRightImage));
    }
}

void FEToggleWidget::SetFocus(const char* parent_pkg) {
    FEStatWidget::SetFocus(parent_pkg);
}

void FEToggleWidget::UnsetFocus() {
    FEStatWidget::UnsetFocus();
}

void FEToggleWidget::Position() {
    FEStatWidget::Position();
    if (pLeftImage) {
        float x, y;
        FEngGetTopLeft(static_cast<FEObject*>(pLeftImage), x, y);
    }
    if (pRightImage) {
        float x, y;
        FEngGetTopLeft(static_cast<FEObject*>(pRightImage), x, y);
    }
}

FESliderWidget::FESliderWidget(bool enabled)
    : FEToggleWidget(enabled) //
    , Slider() //
    , fVertOffset(9.5f) //
{}

void FESliderWidget::Position() {
    FEToggleWidget::Position();
    Slider.SetPos(GetTopLeftX(), GetTopLeftY() + fVertOffset);
}

void FESliderWidget::Show() {
    FEToggleWidget::Show();
    Slider.ToggleVisible(true);
}

void FESliderWidget::Hide() {
    FEToggleWidget::Hide();
    Slider.ToggleVisible(false);
}

void FESliderWidget::Disable() {
    FEWidget::Disable();
}

void FESliderWidget::SetFocus(const char* parent_pkg) {
    FEToggleWidget::SetFocus(parent_pkg);
    Slider.Highlight();
}

void FESliderWidget::UnsetFocus() {
    FEToggleWidget::UnsetFocus();
    Slider.UnHighlight();
}

void FESliderWidget::UpdateSlider(unsigned int msg) {
    if (Slider.Update(msg)) {
        BlinkArrows(msg);
        bMovedLastUpdate = true;
    } else {
        bMovedLastUpdate = false;
    }
}

void FESliderWidget::Enable() {
    FEWidget::Enable();
}
