#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/Src/Camera/Camera.hpp"
#include "dolphin.h"

SunLayer vis_layer_fix;
ePoly sun_vis_poly_fix;
bVector3 sun_vis_poly_fix_ini[4];

extern float SunPosX;
extern float SunPosY;
extern float SunVisibility;
extern int DoSunVisibility;
extern float SunMaxIntensity;
extern unsigned int eFrameCounter;
extern TextureInfo *SunTextures[5];

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
void eRenderSun(eView *view);

void eBuildSunPoly(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {
    const float PixelFadeAmount = 0.1f;
    const float PixelFadeLimit = 0.5f;
    float screen_widthf = static_cast<float>(eGetScreenWidth());
    float screen_heightf = static_cast<float>(eGetScreenHeight());
    float layer_intensity;
    float main_intensity;
    float delta_center_x;
    float delta_center_y;
    unsigned short angle;
    float max_sweep_angle;
    float scale_x;
    float sweep_angle;
    float rx;
    float ry;
    float angle_sin;
    float angle_cos;
    float dx;
    float dy;
    int a;
    float lx;
    float ly;
    int r;
    int g;
    int b;

    layer_intensity = layer->IntensityScale;

    if (layer->Texture == SUNTEX_CENTER && layer->Size > max_size) {
        max_size = layer->Size;
    }

    main_intensity = 1.0f;

    angle = layer->Angle;
    max_sweep_angle = layer->SweepAngleAmount;
    scale_x = (x + max_size) / ((screen_widthf + max_size) + max_size);
    sweep_angle = max_sweep_angle * scale_x;
    angle = angle + static_cast<int>(sweep_angle * 65536.0f);

    rx = layer->Size * 0.5f;

    angle_sin = bSin(angle);
    angle_cos = bCos(angle);

    dx = rx * angle_sin + rx * angle_cos;
    dy = rx * angle_cos - rx * angle_sin;

    poly->Vertices[0].z = 1.0f;
    poly->Vertices[1].z = 1.0f;
    poly->Vertices[2].z = 1.0f;
    poly->Vertices[3].z = 1.0f;

    sun_vis_poly_fix_ini[0].z = 1.0f;

    lx = x + layer->OffsetX;
    ly = y + layer->OffsetY;
    r = layer->Colour[0];
    g = layer->Colour[1];
    b = layer->Colour[2];

    a = static_cast<unsigned int>(layer_intensity * SunVisibility * SunMaxIntensity);

    poly->Vertices[0].x = lx - dx;
    poly->Vertices[0].y = ly - dy;
    poly->Vertices[1].x = lx + dy;
    poly->Vertices[1].y = ly - dx;
    poly->Vertices[2].x = lx + dx;
    poly->Vertices[2].y = ly + dy;
    poly->Vertices[3].x = lx - dy;
    poly->Vertices[3].y = ly + dx;

    poly->Colours[0][0] = r;
    poly->Colours[0][1] = g;
    poly->Colours[0][2] = b;
    poly->Colours[0][3] = a;
    poly->Colours[1][0] = r;
    poly->Colours[1][1] = g;
    poly->Colours[1][2] = b;
    poly->Colours[1][3] = a;
    poly->Colours[2][0] = r;
    poly->Colours[2][1] = g;
    poly->Colours[2][2] = b;
    poly->Colours[2][3] = a;
    poly->Colours[3][0] = r;
    poly->Colours[3][1] = g;
    poly->Colours[3][2] = b;
    poly->Colours[3][3] = a;
}

void eBuildSunPolyFix(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {
    const float PixelFadeAmount = 0.1f;
    const float PixelFadeLimit = 0.5f;
    float screen_widthf = static_cast<float>(eGetScreenWidth());
    float screen_heightf = static_cast<float>(eGetScreenHeight());
    float layer_intensity;
    float main_intensity;
    float delta_center_x;
    float delta_center_y;
    unsigned short angle;
    float max_sweep_angle;
    float scale_x;
    float sweep_angle;
    float rx;
    float ry;
    float angle_sin;
    float angle_cos;
    float dx;
    float dy;
    int a;
    float lx;
    float ly;
    int r;
    int g;
    int b;

    layer_intensity = layer->IntensityScale;

    if (layer->Texture == SUNTEX_CENTER && layer->Size > max_size) {
        max_size = layer->Size;
    }

    main_intensity = 1.0f;

    angle = layer->Angle;
    max_sweep_angle = layer->SweepAngleAmount;
    scale_x = (x + max_size) / ((screen_widthf + max_size) + max_size);
    sweep_angle = max_sweep_angle * scale_x;
    angle = angle + static_cast<int>(sweep_angle * 65536.0f);

    rx = layer->Size * 0.5f;

    angle_sin = bSin(angle);
    angle_cos = bCos(angle);

    dx = rx * angle_sin + rx * angle_cos;
    dy = rx * angle_cos - rx * angle_sin;

    poly->Vertices[0].z = 1.0f;
    poly->Vertices[1].z = 1.0f;
    poly->Vertices[2].z = 1.0f;
    poly->Vertices[3].z = 1.0f;

    sun_vis_poly_fix_ini[0].z = 1.0f;
    sun_vis_poly_fix_ini[1].z = poly->Vertices[1].z;
    sun_vis_poly_fix_ini[2].z = poly->Vertices[2].z;
    sun_vis_poly_fix_ini[3].z = poly->Vertices[3].z;

    lx = x + layer->OffsetX;
    ly = y + layer->OffsetY;
    r = layer->Colour[0];
    g = layer->Colour[1];
    b = layer->Colour[2];

    a = static_cast<unsigned int>(layer_intensity * SunVisibility * SunMaxIntensity);

    poly->Vertices[0].x = lx - dx;
    poly->Vertices[0].y = ly - dy;
    poly->Vertices[1].x = lx + dy;
    poly->Vertices[1].y = ly - dx;
    poly->Vertices[2].x = lx + dx;
    poly->Vertices[2].y = ly + dy;
    poly->Vertices[3].x = lx - dy;
    poly->Vertices[3].y = ly + dx;

    sun_vis_poly_fix_ini[0].x = poly->Vertices[0].x;
    sun_vis_poly_fix_ini[1].x = poly->Vertices[1].x;
    sun_vis_poly_fix_ini[2].x = poly->Vertices[2].x;
    sun_vis_poly_fix_ini[3].x = poly->Vertices[3].x;

    sun_vis_poly_fix_ini[0].y = poly->Vertices[0].y;
    sun_vis_poly_fix_ini[1].y = poly->Vertices[1].y;
    sun_vis_poly_fix_ini[2].y = poly->Vertices[2].y;
    sun_vis_poly_fix_ini[3].y = poly->Vertices[3].y;

    poly->Colours[0][0] = r;
    poly->Colours[0][1] = g;
    poly->Colours[0][2] = b;
    poly->Colours[0][3] = a;
    poly->Colours[1][0] = r;
    poly->Colours[1][1] = g;
    poly->Colours[1][2] = b;
    poly->Colours[1][3] = a;
    poly->Colours[2][0] = r;
    poly->Colours[2][1] = g;
    poly->Colours[2][2] = b;
    poly->Colours[2][3] = a;
    poly->Colours[3][0] = r;
    poly->Colours[3][1] = g;
    poly->Colours[3][2] = b;
    poly->Colours[3][3] = a;
}

void eUpdateSunPolyFix(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {
    float intensity = layer->IntensityScale * SunVisibility * SunMaxIntensity;
    unsigned int alpha = static_cast<unsigned int>(intensity);

    poly->Vertices[0].x = sun_vis_poly_fix_ini[0].x + x;
    poly->Vertices[0].y = sun_vis_poly_fix_ini[0].y + y;
    poly->Vertices[1].x = sun_vis_poly_fix_ini[1].x + x;
    poly->Vertices[1].y = sun_vis_poly_fix_ini[1].y + y;
    poly->Vertices[2].x = sun_vis_poly_fix_ini[2].x + x;
    poly->Vertices[2].y = sun_vis_poly_fix_ini[2].y + y;
    poly->Vertices[3].x = sun_vis_poly_fix_ini[3].x + x;
    poly->Vertices[3].y = sun_vis_poly_fix_ini[3].y + y;
    poly->Colours[0][3] = alpha;
    poly->Colours[1][3] = alpha;
    poly->Colours[2][3] = alpha;
    poly->Colours[3][3] = alpha;
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

            eUpdateSunPolyFix(&sun_vis_poly_fix, &vis_layer_fix, vis_layer_fix.Size, x, y);
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

void eRenderSun(eView *view) {
    SunChunkInfo *sun_info = SunInfo;
    Camera *camera;
    bMatrix4 *world_view;
    bVector4 position3d;
    bVector4 position2d;
    bVector4 view3d;
    float screen_widthf;
    float screen_heightf;
    float x;
    float y;
    float max_size;

    SetCurrentSunInfo();

    if (IsGameFlowInGame()) {
        camera = view->GetCamera();
        world_view = camera->GetCameraMatrix();
        position3d.x = sun_info->PositionX;
        position3d.y = sun_info->PositionY;
        position3d.z = sun_info->PositionZ;
        position3d.w = 1.0f;
        GetScreenPosition(view, reinterpret_cast<bVector3 *>(&position2d), reinterpret_cast<const bVector3 *>(&position3d));
        eMulVector(&view3d, world_view, &position3d);

        screen_widthf = static_cast<float>(eGetScreenWidth());
        screen_heightf = static_cast<float>(eGetScreenHeight());

        x = position2d.x;
        y = position2d.y;
        if (SunPosX != 0.0f || SunPosY != 0.0f) {
            x = SunPosX;
            y = SunPosY;
        }

        max_size = 0.0f;

        {
            int i;

            for (i = 0; i < 4; i++) {
                SunLayer *layer = &sun_info->SunLayers[i];

                if (layer->IntensityScale > 0.0f && layer->Texture == SUNTEX_CENTER && layer->Size > max_size) {
                    max_size = layer->Size;
                }
            }
        }

        if (view3d.z >= 0.0f && x >= -max_size && x <= screen_widthf + max_size && y >= -max_size && y <= screen_heightf + max_size) {
            eRecalculateOthographicProjection(1, 100000.0f);
            eSetOrthographicMatrixToHW();
            eCalcSunVisibility(eGetView(0, false), x, y);
            eRecalculateOthographicProjection(1, 0.0f);
            eSetOrthographicMatrixToHW();

            {
                int i;

                for (i = 0; i < 4; i++) {
                    SunLayer *layer = &sun_info->SunLayers[i];
                    TextureInfo *texture_info = SunTextures[layer->Texture];

                    if (texture_info) {
                        ePoly sun_poly;

                        eBuildSunPoly(&sun_poly, layer, max_size, x, y);
                        RenderViewPoly(view, &sun_poly, texture_info, 0);
                    }
                }
            }
        }
    }
}

void eInitSunPat() {
    vis_layer_fix.Angle = 0;
    vis_layer_fix.Texture = SUNTEX_CENTER;
    vis_layer_fix.Size = 1.0f;
    vis_layer_fix.IntensityScale = 32.0f;
    vis_layer_fix.SweepAngleAmount = 0.0f;
    vis_layer_fix.OffsetX = 0.0f;
    vis_layer_fix.OffsetY = 0.0f;
    eBuildSunPolyFix(&sun_vis_poly_fix, &vis_layer_fix, 1.0f, 0.0f, 0.0f);
}
