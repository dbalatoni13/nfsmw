#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

void UITrackMapStreamer::SetZoomSpeed(float sec) {
    ZoomCubic.SetDuration(sec);
}

void UITrackMapStreamer::SetPanSpeed(float sec) {
    PanCubic.SetDuration(sec);
}

void UITrackMapStreamer::ZoomTo(const bVector2& factor) {
    ZoomCubic.SetValDesired(const_cast< bVector2* >(&factor));
}

void UITrackMapStreamer::PanTo(const bVector2& pos) {
    PanCubic.SetValDesired(const_cast< bVector2* >(&pos));
}

void UITrackMapStreamer::MapPackLoadCallback(unsigned int screenPtr) {
    reinterpret_cast< UITrackMapStreamer* >(screenPtr)->SetMapPackLoaded();
}

void UITrackMapStreamer::MakeSpaceInPoolCallbackBridge(int param) {
    reinterpret_cast< UITrackMapStreamer* >(param)->MakeSpaceInPoolCallback();
}
