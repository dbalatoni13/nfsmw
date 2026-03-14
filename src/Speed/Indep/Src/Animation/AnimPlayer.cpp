#include "AnimPlayer.hpp"

bool gPlayAnimStream;

CAnimPlayer TheAnimPlayer;

bool CAnimSettings::mDebugPrintEnabled = false;

bool CAnimSettings::IsDebugPrintEnabled() {
    return mDebugPrintEnabled;
}
