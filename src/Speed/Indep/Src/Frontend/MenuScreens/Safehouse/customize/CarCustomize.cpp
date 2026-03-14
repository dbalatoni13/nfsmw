// OWNED BY zFeOverlay AGENT - DO NOT MODIFY OR EMPTY
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CarCustomize.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeManager.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/Src/FEng/feimage.h"
#include "Speed/Indep/Src/FEng/FEString.h"

extern void FEngSetVisible(FEObject *obj);
extern void FEngSetInvisible(FEObject *obj);

// --- CustomizeMeter ---

void CustomizeMeter::SetCurrent(float current) {
    Current = bMin(bMax(current, Min), Max);
}

void CustomizeMeter::SetPreview(float preview) {
    PreviousPreview = Preview;
    Preview = bMin(bMax(preview, Min), Max);
}

void CustomizeMeter::SetVisibility(bool b) {
    if (b) {
        FEngSetVisible(pMeterGroup);
    } else {
        FEngSetInvisible(pMeterGroup);
    }
}

// --- FEShoppingCartItem ---

void FEShoppingCartItem::Show() {
    FEStatWidget::Show();
    FEngSetVisible(pTradeInPrice);
    FEngSetVisible(pCheckIcon);
}

void FEShoppingCartItem::Hide() {
    FEStatWidget::Hide();
    FEngSetInvisible(pTradeInPrice);
    FEngSetInvisible(pCheckIcon);
}

// --- CustomizeSub ---

CustomizeMainOption *CustomizeSub::FindInCartOption() {
    if (!InCartPartOptionIndex)
        return nullptr;
    return static_cast<CustomizeMainOption *>(Options.GetOption(InCartPartOptionIndex));
}

// --- CustomizeParts ---

void CustomizeParts::LoadHudTextures() {
    PacksLoadedCount = 0;
    LoadNextHudTexturePack();
}

// --- CustomizePaint ---

void CustomizePaint::SetupBasePaint() {
    BuildSwatchList(0x4C);
}
