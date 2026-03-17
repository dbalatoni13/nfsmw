#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "dolphin.h"

SunLayer vis_layer_fix;
ePoly sun_vis_poly_fix;
float sun_vis_poly_fix_ini[16];

extern float SunPosX;
extern float SunPosY;
extern float SunVisibility;
extern int DoSunVisibility;
extern float SunMaxIntensity;
extern unsigned int eFrameCounter;

int eGetScreenWidth();
int eGetScreenHeight();
u16 GXReadDrawSync();
void eSetColourUpdate(Bool bRGB, Bool bAlpha);
void eSetOrthographicMatrixToHW();
void eRecalculateOthographicProjection(int view_id, float far_clip);
void SetCurrentSunInfo();

void RenderViewPoly(eView *view, ePoly *poly, TextureInfo *texture_info, int flags)
    __asm__("Render__18eViewPlatInterfaceP5ePolyP11TextureInfoi");
void GetScreenPosition(eView *view, bVector3 *screen, const bVector3 *world)
    __asm__("GetScreenPosition__18eViewPlatInterfaceP8bVector3PC8bVector3");
void eMulVector(bVector4 *dst, const bMatrix4 *matrix, const bVector4 *src) __asm__("eMulVector__FP8bVector4PC8bMatrix4PC8bVector4");
void ConstructePoly(ePoly *poly) __asm__("__5ePoly");

void eBuildSunPoly(ePoly *poly, SunLayer *layer, float max_size, float x, float y);
void eBuildSunPolyFix(ePoly *poly, SunLayer *layer, float max_size, float x, float y);
void eUpdateSunPolyFix(ePoly *poly, SunLayer *layer, float max_size, float x, float y);
void eCalcSunVisibility(eView *view, float x, float y);

void eBuildSunPoly(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {
    float screen_width = static_cast<float>(eGetScreenWidth());
    float half_size;
    float sin_angle;
    float cos_angle;
    float diagonal0;
    float diagonal1;
    float intensity;
    float center_x;
    float center_y;
    unsigned char alpha;
    unsigned short angle;

    eGetScreenHeight();

    if (layer->Texture == SUNTEX_CENTER && max_size < layer->Size) {
        max_size = layer->Size;
    }

    half_size = layer->Size * 0.5f;
    angle = static_cast<unsigned short>(
        layer->Angle +
        static_cast<int>(layer->SweepAngleAmount * (((x + max_size) / ((screen_width + max_size) + max_size)) * 65536.0f))
    );
    sin_angle = bSin(angle);
    cos_angle = bCos(angle);

    poly->Vertices[0].z = 1.0f;
    poly->Vertices[1].z = 1.0f;
    poly->Vertices[2].z = 1.0f;
    poly->Vertices[3].z = 1.0f;

    diagonal0 = half_size * cos_angle;
    diagonal1 = half_size * sin_angle;
    intensity = layer->IntensityScale * SunVisibility * SunMaxIntensity;
    center_x = x + layer->OffsetX;
    center_y = y + layer->OffsetY;

    if (intensity >= 28.0f) {
        alpha = static_cast<unsigned char>(static_cast<int>(intensity - 28.0f));
    } else {
        alpha = static_cast<unsigned char>(static_cast<int>(intensity));
    }

    poly->Vertices[3].x = center_x - (diagonal0 - diagonal1);
    poly->Vertices[0].x = center_x - (diagonal1 + diagonal0);
    poly->Vertices[3].y = center_y + (diagonal1 + diagonal0);
    poly->Vertices[0].y = center_y - (diagonal0 - diagonal1);
    poly->Vertices[1].y = center_y - (diagonal1 + diagonal0);
    poly->Vertices[1].x = center_x + (diagonal0 - diagonal1);
    poly->Vertices[2].x = center_x + (diagonal1 + diagonal0);
    poly->Vertices[2].y = center_y + (diagonal0 - diagonal1);

    poly->Colours[3][3] = alpha;
    poly->Colours[3][0] = layer->Colour[0];
    poly->Colours[3][1] = layer->Colour[1];
    poly->Colours[3][2] = layer->Colour[2];
    poly->Colours[0][0] = layer->Colour[0];
    poly->Colours[0][1] = layer->Colour[1];
    poly->Colours[0][2] = layer->Colour[2];
    poly->Colours[0][3] = alpha;
    poly->Colours[1][0] = layer->Colour[0];
    poly->Colours[1][1] = layer->Colour[1];
    poly->Colours[1][2] = layer->Colour[2];
    poly->Colours[1][3] = alpha;
    poly->Colours[2][0] = layer->Colour[0];
    poly->Colours[2][1] = layer->Colour[1];
    poly->Colours[2][2] = layer->Colour[2];
    poly->Colours[2][3] = alpha;
}

void eBuildSunPolyFix(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {
    float screen_width = static_cast<float>(eGetScreenWidth());
    float half_size;
    float sin_angle;
    float cos_angle;
    float diagonal0;
    float diagonal1;
    float intensity;
    float center_x;
    float center_y;
    unsigned char alpha;
    unsigned short angle;

    eGetScreenHeight();

    if (layer->Texture == SUNTEX_CENTER && max_size < layer->Size) {
        max_size = layer->Size;
    }

    half_size = layer->Size * 0.5f;
    angle = static_cast<unsigned short>(
        layer->Angle +
        static_cast<int>(layer->SweepAngleAmount * (((x + max_size) / ((screen_width + max_size) + max_size)) * 65536.0f))
    );
    sin_angle = bSin(angle);
    cos_angle = bCos(angle);

    sun_vis_poly_fix_ini[2] = 1.0f;
    poly->Vertices[1].z = 1.0f;
    poly->Vertices[2].z = sun_vis_poly_fix_ini[2];
    poly->Vertices[3].z = sun_vis_poly_fix_ini[2];

    diagonal0 = half_size * cos_angle;
    diagonal1 = half_size * sin_angle;
    intensity = layer->IntensityScale * SunVisibility * SunMaxIntensity;
    center_x = x + layer->OffsetX;
    center_y = y + layer->OffsetY;

    if (intensity >= 28.0f) {
        alpha = static_cast<unsigned char>(static_cast<int>(intensity - 28.0f));
    } else {
        alpha = static_cast<unsigned char>(static_cast<int>(intensity));
    }

    poly->Vertices[3].x = center_x - (diagonal0 - diagonal1);
    poly->Vertices[3].y = center_y + (diagonal1 + diagonal0);
    poly->Vertices[0].y = center_y - (diagonal0 - diagonal1);
    sun_vis_poly_fix_ini[0] = center_x - (diagonal1 + diagonal0);
    poly->Vertices[0].x = sun_vis_poly_fix_ini[0];
    poly->Vertices[1].y = center_y - (diagonal1 + diagonal0);
    poly->Vertices[1].x = center_x + (diagonal0 - diagonal1);
    poly->Vertices[2].y = center_y + (diagonal0 - diagonal1);
    poly->Vertices[2].x = center_x + (diagonal1 + diagonal0);

    sun_vis_poly_fix_ini[1] = poly->Vertices[0].y;
    sun_vis_poly_fix_ini[4] = poly->Vertices[1].x;
    sun_vis_poly_fix_ini[5] = poly->Vertices[1].y;
    sun_vis_poly_fix_ini[8] = poly->Vertices[2].x;
    sun_vis_poly_fix_ini[9] = poly->Vertices[2].y;
    sun_vis_poly_fix_ini[12] = poly->Vertices[3].x;
    sun_vis_poly_fix_ini[13] = poly->Vertices[3].y;

    poly->Colours[0][0] = layer->Colour[0];
    poly->Colours[0][1] = layer->Colour[1];
    poly->Colours[0][2] = layer->Colour[2];
    poly->Colours[0][3] = alpha;
    poly->Colours[1][0] = layer->Colour[0];
    poly->Colours[1][1] = layer->Colour[1];
    poly->Colours[1][2] = layer->Colour[2];
    poly->Colours[1][3] = alpha;
    poly->Colours[2][0] = layer->Colour[0];
    poly->Colours[2][1] = layer->Colour[1];
    poly->Colours[2][2] = layer->Colour[2];
    poly->Colours[2][3] = alpha;
    poly->Colours[3][0] = layer->Colour[0];
    poly->Colours[3][1] = layer->Colour[1];
    poly->Colours[3][2] = layer->Colour[2];
    poly->Colours[3][3] = alpha;
}

void eUpdateSunPolyFix(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {
    float intensity = layer->IntensityScale * SunVisibility * SunMaxIntensity;
    unsigned char alpha;

    if (intensity >= 28.0f) {
        alpha = static_cast<unsigned char>(static_cast<int>(intensity - 28.0f));
    } else {
        alpha = static_cast<unsigned char>(static_cast<int>(intensity));
    }

    poly->Vertices[0].x = sun_vis_poly_fix_ini[0] + x;
    poly->Vertices[0].y = sun_vis_poly_fix_ini[1] + y;
    poly->Vertices[1].x = sun_vis_poly_fix_ini[4] + x;
    poly->Vertices[1].y = sun_vis_poly_fix_ini[5] + y;
    poly->Vertices[2].x = sun_vis_poly_fix_ini[8] + x;
    poly->Vertices[2].y = sun_vis_poly_fix_ini[9] + y;
    poly->Vertices[3].x = sun_vis_poly_fix_ini[12] + x;
    poly->Colours[3][3] = alpha;
    poly->Colours[0][3] = alpha;
    poly->Vertices[3].y = sun_vis_poly_fix_ini[13] + y;
    poly->Colours[1][3] = alpha;
    poly->Colours[2][3] = alpha;
}

void eCalcSunVisibility(eView *view, float x, float y) {
    if (DoSunVisibility) {
        if (eFrameCounter == (eFrameCounter / 5) * 5) {
            u32 top_in;
            u32 top_out;
            u32 bottom_in;
            u32 bottom_out;
            u32 clear_in;
            u32 copy_clocks;

            eUpdateSunPolyFix(&sun_vis_poly_fix, &vis_layer_fix, 1.0f, x, y);
            GXClearPixMetric();
            eSetColourUpdate(0, 0);
            RenderViewPoly(view, &sun_vis_poly_fix, DefaultTextureInfo, 0);
            GXSetDrawSync(0xBEEF);
            GXFlush();

            while (GXReadDrawSync() != 0xBEEF) {
                // nop
            }

            GXReadPixMetric(&top_in, &top_out, &bottom_in, &bottom_out, &clear_in, &copy_clocks);
            eSetColourUpdate(1, 1);
            SunVisibility = static_cast<float>(top_out) / (static_cast<float>(top_in) + 1.0f);
        }
    }
}

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
