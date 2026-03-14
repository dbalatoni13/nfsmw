// OWNED BY zFeOverlay AGENT - do not empty or overwrite
#ifndef FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_FECUSTOMIZE_H
#define FRONTEND_MENUSCREENS_SAFEHOUSE_QUICKRACE____CUSTOMIZE_FECUSTOMIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Src/Frontend/MenuScreens/Common/feWidget.hpp"
#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/customize/CustomizeTypes.hpp"

#include <types.h>

struct FEImage;
struct FEString;

// total size: extends FEStatWidget
struct FEShoppingCartItem : public FEStatWidget {
    FEShoppingCartItem(ShoppingCartItem *item)
        : pItem(item) //
        , pCheckIcon(nullptr) //
        , pTradeInString(nullptr) {}

    ~FEShoppingCartItem() override {}

    void SetCheckIcon(FEImage *img) { pCheckIcon = img; }
    void SetTradeInString(FEString *string) { pTradeInString = string; }
    ShoppingCartItem *GetItem() { return pItem; }

    void Show() override;
    void Hide() override;
    void Draw() override;
    void Position() override;
    void SetFocus(const char *parent_pkg) override;
    void UnsetFocus() override;

    void SetCheckScripts();
    void SetActiveScripts();
    void DrawPartName();
    unsigned int GetPerfPkgCatHash(enum Type phys_type);
    unsigned int GetPerfPkgLevelHash(int level);
    unsigned int GetCarPartCatHash(unsigned int slot_id);

    ShoppingCartItem *pItem;
    FEImage *pCheckIcon;
    FEString *pTradeInString;
};

#endif
