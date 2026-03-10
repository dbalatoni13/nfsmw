#include "WorldAnimCtrl.hpp"

CWorldAnimCtrl::CWorldAnimCtrl() {
    Clear();
}

CWorldAnimCtrl::~CWorldAnimCtrl() {
    Purge();
}

void CWorldAnimCtrl::Purge() {
    Cleanup();
    Clear();
}

void CWorldAnimCtrl::Play() {
    PlayState = eACPS_PLAYING;
}

void CWorldAnimCtrl::Pause() {
    PlayState = eACPS_PAUSED;
}
