#include "LocalPlayer.hpp"
#include "Speed/Indep/Libs/Support/Utility/UCOM.h"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/Frontend/HUD/FEPkg_Hud.hpp"
#include "Speed/Indep/Src/Generated/Events/EPursuitBreaker.hpp"
#include "Speed/Indep/Src/Interfaces/IAttachable.h"
#include "Speed/Indep/Src/Interfaces/Simables/IVehicle.h"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

void LocalPlayer::ReleaseHud() {
    if (mHud) {
        mHud->Release();
        mHud = nullptr;
    }
}

PlayerSettings *LocalPlayer::GetSettings() const {
    if (mSettingIndex >= 0) {
        return FEDatabase->GetPlayerSettings(mSettingIndex);
    }
    return nullptr;
}

void LocalPlayer::SetHud(ePlayerHudType ht) {
    if (ht == PHT_NONE) {
        ReleaseHud();
        return;
    } else if (mSettingIndex < 0) {
        return;
    }
    const char *hud_name = HudResourceManager::GetHudFengName(ht);
    if (!hud_name) {
        ReleaseHud();
    } else {
        ReleaseHud();
        mHud = ::new (__FILE__, __LINE__) FEngHud(ht, hud_name, this, mSettingIndex);
    }
}

void LocalPlayer::OnAttached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        if (mSpeech) {
            mSpeech->Attach(ivehicle);
        }
        Sim::Collision::AddListener(this, ivehicle, "LocalPlayer");
    }
    Sim::Entity::OnAttached(pOther);
}

void LocalPlayer::OnDetached(IAttachable *pOther) {
    IVehicle *ivehicle;
    if (pOther->QueryInterface(&ivehicle)) {
        if (mSpeech) {
            mSpeech->Detach(ivehicle);
        }
        Sim::Collision::RemoveListener(this, ivehicle);
        SetGameBreaker(false);
    }
    if (UTL::COM::ComparePtr(mSpeech, pOther)) {
        mSpeech = nullptr;
    }
    Sim::Entity::OnDetached(pOther);
}

void LocalPlayer::SetGameBreaker(bool on) {

    if (on != mInGameBreaker) {
        new EPursuitBreaker(on ? 1 : 0);
        mInGameBreaker = on;
    }
}

bool LocalPlayer::ToggleGameBreaker() {
    if (!CanDoGameBreaker() && !mInGameBreaker) {
        return false;
    }
    if (mInGameBreaker) {
        SetGameBreaker(false);
    } else if (mGameBreakerCharge > 0.0f) {
        SetGameBreaker(true);
    } else {
        return false;
    }
    return true;
}

bool LocalPlayer::CanRechargeNOS() const {
    return mInGameBreaker == 0;
}
