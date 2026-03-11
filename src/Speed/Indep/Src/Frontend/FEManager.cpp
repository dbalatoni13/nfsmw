#include "FEManager.hpp"

#include "Speed/Indep/Src/Frontend/FEJoyInput.hpp"

FEManager::FEManager()
    : bSuppressControllerError(false) //
    , bAllowControllerError(false) //
    , mFirstScreen(nullptr) //
    , mFirstScreenArg(0) //
    , mFirstScreenMask(0) //
    , mGarageType(GARAGETYPE_NONE) //
    , mPreviousGarageType(GARAGETYPE_NONE) //
    , mGarageBackground(nullptr) //
    , mFirstBoot(true) //
    , mEATraxDelay(0) //
    , mEATraxFirstButton(false)
{
    for (int port = 0; port < 8; port++) {
        bWantControllerError[port] = false;
    }
}

void FEManager::InitInput() {
    if (cFEngJoyInput::mInstance == nullptr) {
        cFEngJoyInput::mInstance = new cFEngJoyInput();
    }
}

void FEManager::SetGarageType(eGarageType pGarageType) {
    mPreviousGarageType = mGarageType;
    mGarageType = pGarageType;
    mGarageBackground = nullptr;
}
