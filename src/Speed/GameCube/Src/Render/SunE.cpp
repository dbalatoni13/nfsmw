#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/Sun.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"

void eSetColourUpdate(Bool bRGB, Bool bAlpha);

extern "C" {
void GXClearPixMetric();
void GXSetDrawSync(unsigned short token);
void GXFlush();
unsigned short GXReadDrawSync();
}

int eGetScreenWidth();
int eGetScreenHeight();
void eRecalculateOthographicProjection(int nWhich, float fScale);
void eSetOrthographicMatrixToHW();
void SetCurrentSunInfo();
extern TextureInfo *SunTextures[5];

void eBuildSunPoly(ePoly *poly, SunLayer *layer, float size, float offset_x, float offset_y);
void eBuildSunPolyFix(ePoly *poly, SunLayer *layer, float size, float offset_x, float offset_y);
void eRenderSun(eView *view);
void eUpdateSunPolyFix(ePoly *poly, SunLayer *layer, float size, float x, float y);

SunLayer vis_layer_fix;
ePoly sun_vis_poly_fix;
static bVector3 sun_something_vecs[4];
float sun_vis_poly_fix_ini[16];

float SunPosX = 0.0f;
float SunPosY = 0.0f;
float SunVisibility = 1.0f;
int DoSunVisibility = 1;
float SunMaxIntensity = 127.0f;

void eBuildSunPoly(ePoly *poly, SunLayer *layer, float max_size, float x, float y) {

    float PixelFadeAmount;
    float PixelFadeLimit;

    float screen_widthf = (float)eGetScreenWidth();
    float screen_heightf = (float)eGetScreenHeight();

    float layer_intensity = layer->IntensityScale;
    float main_intensity;

    if (layer->Texture == SUNTEX_CENTER) {

        if (layer->Size > max_size) {

            max_size = layer->Size;
        }
    }

    float delta_center_x = 1.0f;
    float delta_center_y = 1.0f;

    unsigned short angle;

    float max_sweep_angle = layer->SweepAngleAmount;
    float scale_x = (x + max_size) / (screen_widthf + max_size + max_size);
    float sweep_angle = max_sweep_angle * scale_x;

    angle = ((unsigned short *)&layer->Angle)[1] + (int)(sweep_angle * 65536.0f);

    float rx = layer->Size * 0.5f;
    float ry = layer->Size * 0.5f;

    float angle_sin = bSin(angle);
    float angle_cos = bCos(angle);

    float dx = rx * angle_sin + ry * angle_cos;
    float dy = ry * angle_cos - rx * angle_sin;

    unsigned int a;

    poly->Vertices[0].z = 1.0f;
    poly->Vertices[1].z = 1.0f;
    poly->Vertices[2].z = 1.0f;
    poly->Vertices[3].z = 1.0f;

    sun_vis_poly_fix_ini[2] = 1.0f;

    float lx = x + layer->OffsetX;
    float ly = y + layer->OffsetY;
    unsigned char r = layer->Colour[0];
    unsigned char g = layer->Colour[1];
    unsigned char b = layer->Colour[2];

    a = (unsigned int)(layer_intensity * SunVisibility * SunMaxIntensity);

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

    float PixelFadeAmount;
    float PixelFadeLimit;

    float screen_widthf = (float)eGetScreenWidth();
    float screen_heightf = (float)eGetScreenHeight();

    float layer_intensity = layer->IntensityScale;
    float main_intensity;

    if (layer->Texture == SUNTEX_CENTER) {

        if (layer->Size > max_size) {

            max_size = layer->Size;
        }
    }

    float delta_center_x = 1.0f;
    float delta_center_y = 1.0f;

    unsigned short angle;
    float max_sweep_angle = layer->SweepAngleAmount;
    float scale_x = (x + max_size) / (screen_widthf + max_size + max_size);
    float sweep_angle = max_sweep_angle * scale_x;
    angle = ((unsigned short *)&layer->Angle)[1] + (int)(sweep_angle * 65536.0f);
    float rx = layer->Size * 0.5f;
    float ry = layer->Size * 0.5f;
    float angle_sin = bSin(angle);
    float angle_cos = bCos(angle);

    float dx = rx * angle_sin + ry * angle_cos;
    float dy = ry * angle_cos - rx * angle_sin;

    poly->Vertices[1].z = 1.0f;
    poly->Vertices[2].z = 1.0f;
    poly->Vertices[3].z = 1.0f;
    poly->Vertices[0].z = 1.0f;
    sun_vis_poly_fix_ini[2] = 1.0f;

    sun_vis_poly_fix_ini[6] = poly->Vertices[1].z;
    sun_vis_poly_fix_ini[10] = poly->Vertices[2].z;
    sun_vis_poly_fix_ini[14] = poly->Vertices[3].z;

    unsigned int a;

    float lx = x + layer->OffsetX;
    float ly = y + layer->OffsetY;
    unsigned char r = layer->Colour[0];
    unsigned char g = layer->Colour[1];
    unsigned char b = layer->Colour[2];

    a = (unsigned int)(layer_intensity * SunVisibility * SunMaxIntensity);

    poly->Vertices[0].x = lx - dx;
    poly->Vertices[0].y = ly - dy;
    poly->Vertices[1].x = lx + dy;
    poly->Vertices[1].y = ly - dx;
    poly->Vertices[2].x = lx + dx;
    poly->Vertices[2].y = ly + dy;
    poly->Vertices[3].x = lx - dy;
    poly->Vertices[3].y = ly + dx;

    sun_vis_poly_fix_ini[0] = poly->Vertices[0].x;
    sun_vis_poly_fix_ini[4] = poly->Vertices[1].x;
    sun_vis_poly_fix_ini[8] = poly->Vertices[2].x;
    sun_vis_poly_fix_ini[12] = poly->Vertices[3].x;
    sun_vis_poly_fix_ini[1] = poly->Vertices[0].y;
    sun_vis_poly_fix_ini[5] = poly->Vertices[1].y;
    sun_vis_poly_fix_ini[9] = poly->Vertices[2].y;
    sun_vis_poly_fix_ini[13] = poly->Vertices[3].y;

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

void eUpdateSunPolyFix(ePoly *poly, SunLayer *layer, float size, float x, float y) {
    unsigned int colour = (unsigned int)(layer->IntensityScale * SunVisibility * SunMaxIntensity);

    poly->Vertices[0].x = sun_vis_poly_fix_ini[0] + x;
    poly->Vertices[0].y = sun_vis_poly_fix_ini[1] + y;
    poly->Vertices[1].x = sun_vis_poly_fix_ini[4] + x;
    poly->Vertices[1].y = sun_vis_poly_fix_ini[5] + y;
    poly->Vertices[2].x = sun_vis_poly_fix_ini[8] + x;
    poly->Vertices[2].y = sun_vis_poly_fix_ini[9] + y;
    poly->Vertices[3].x = sun_vis_poly_fix_ini[12] + x;
    poly->Vertices[3].y = sun_vis_poly_fix_ini[13] + y;
    poly->Colours[0][3] = colour;
    poly->Colours[1][3] = colour;
    poly->Colours[2][3] = colour;
    poly->Colours[3][3] = colour;
}

void eCalcSunVisibility(eView *view, float x, float y) {
    if (DoSunVisibility != 0 && eFrameCounter % 5 == 0) {
        eUpdateSunPolyFix(&sun_vis_poly_fix, &vis_layer_fix, vis_layer_fix.Size, x, y);

        GXClearPixMetric();

        eSetColourUpdate(0, 0);

        view->Render(&sun_vis_poly_fix, DefaultTextureInfo, 0);

        GXSetDrawSync(0xbeef);
        GXFlush();
        while (GXReadDrawSync() != 0xbeef) {
        }

        unsigned long tpi;
        unsigned long tpo;
        unsigned long bpi;
        unsigned long bpo;
        unsigned long cpi;
        unsigned long cc;

        GXReadPixMetric(reinterpret_cast<u32 *>(&tpi), reinterpret_cast<u32 *>(&tpo), reinterpret_cast<u32 *>(&bpi), reinterpret_cast<u32 *>(&bpo), reinterpret_cast<u32 *>(&cpi), reinterpret_cast<u32 *>(&cc));

        eSetColourUpdate(1, 1);

        SunVisibility = (float)tpo / ((float)tpi + 1.0f);
    }
}

void eRenderSun(eView *view) {
    SunChunkInfo *sun_info = SunInfo;

    SetCurrentSunInfo();

    if (TheGameFlowManager.IsInGame()) {
        bMatrix4 *camera = (bMatrix4 *)view->pCamera;
        bVector4 sun_position;
        bVector3 screen_position;
        bVector4 view_position;

        sun_position.x = sun_info->PositionX;
        sun_position.y = sun_info->PositionY;
        sun_position.z = sun_info->PositionZ;
        sun_position.w = 1.0f;

        view->GetScreenPosition(&screen_position, (const bVector3 *)&sun_position);
        eMulVector(&view_position, camera, &sun_position);

        float screen_width = (float)eGetScreenWidth();
        float screen_height = (float)eGetScreenHeight();

        float x = screen_position.x;
        float y = screen_position.y;

        if (SunPosX != 0.0f || SunPosY != 0.0f) {
            x = SunPosX;
            y = SunPosY;
        }

        float biggest = 0.0f;

        for (int i = 0; i < 4; i++) {
            SunLayer *layer = &sun_info->SunLayers[i];

            if (layer->IntensityScale > 0.0f && layer->Texture == SUNTEX_CENTER && layer->Size > biggest) {
                biggest = layer->Size;
            }
        }

        if (view_position.z < 0.0f || x < -biggest || x > screen_width + biggest || y < -biggest || y > screen_height + biggest) {
            return;
        }

        eRecalculateOthographicProjection(1, 99995.0f);
        eSetOrthographicMatrixToHW();

        eCalcSunVisibility(eViews, x, y);

        eRecalculateOthographicProjection(1, 0.0f);
        eSetOrthographicMatrixToHW();

        for (int i = 0; i < 4; i++) {
            SunLayer *layer = &sun_info->SunLayers[i];
            TextureInfo *texture = SunTextures[layer->Texture];

            if (texture != NULL) {
                ePoly poly;

                eBuildSunPoly(&poly, layer, biggest, x, y);
                view->Render(&poly, texture, 0);
            }
        }
    }
}

void eInitSunPat() {
    vis_layer_fix.Angle = 0;
    vis_layer_fix.Texture = SUNTEX_CENTER;
    vis_layer_fix.Size = 32.0f;
    vis_layer_fix.IntensityScale = 1.0f;
    vis_layer_fix.SweepAngleAmount = 0.0f;
    vis_layer_fix.OffsetX = 0.0f;
    vis_layer_fix.OffsetY = 0.0f;
    eBuildSunPolyFix(&sun_vis_poly_fix, &vis_layer_fix, vis_layer_fix.Size, 0.0f, 0.0f);
}
