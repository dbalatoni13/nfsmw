#include "Speed/Indep/Src/Frontend/MenuScreens/Safehouse/quickrace/uiTrackMapStreamer.hpp"

void UITrackMapStreamer::SetZoomSpeed(float sec) {
    ZoomCubic.SetDuration(sec);
}

void UITrackMapStreamer::SetPanSpeed(float sec) {
    PanCubic.SetDuration(sec);
}
