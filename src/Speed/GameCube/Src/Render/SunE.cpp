#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"

SunLayer vis_layer_fix;
ePoly sun_vis_poly_fix;

void eBuildSunPolyFix(ePoly *poly, SunLayer *layer, float size, float x, float y);

void eInitSunPat() {
    vis_layer_fix.Texture = SUNTEX_CENTER;
    vis_layer_fix.IntensityScale = 32.0f;
    vis_layer_fix.Size = 1.0f;
    vis_layer_fix.Angle = 0;
    vis_layer_fix.OffsetX = 0.0f;
    vis_layer_fix.OffsetY = 0.0f;
    vis_layer_fix.SweepAngleAmount = 0.0f;
    eBuildSunPolyFix(&sun_vis_poly_fix, &vis_layer_fix, 1.0f, 0.0f, 0.0f);
}
