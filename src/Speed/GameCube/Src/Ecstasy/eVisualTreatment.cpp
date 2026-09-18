#include "Speed/Indep/Src/World/VisualTreatment.h"
#include "Speed/Indep/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/GameCube/Src/Ecstasy/Ecstasy.hpp"
#include "Speed/Indep/bWare/Inc/bMath.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include <dolphin.h>

// La parte GameCube del tratamiento visual: vive detras de un unico puntero
// global y programa el TEV de contraste con las curvas del visuallook.

extern GXColor MWDesaturation;
extern GXColor MWPulseBrightness;
extern GXColor MWColourTint;
extern unsigned int *pContrastRampPixels;
extern unsigned int *pContrastLinearSource;
extern int zero_red;
extern int zero_green;

float GetValueFromSpline(float value, bMatrix4 *curve);
int eScreenQuadReplace(eView *view, int opt);
inline void eSwizzle32Bit(unsigned int *dest, unsigned int *src, long XSize, long YSize);

static const int gApplyTestDesaturate = 0;
static const int gApplyTestPulseBrightnessBloom = 1;
static const int gApplyTestColorBloom = 0;

static GXColor TestMWPulseBrightness = { 50, 0, 0, 255 };

static const int VT_IndirectTextureRes = 64;

float black_bloom_color_scale = 255.0f;
float colour_bloom_color_scale = 127.0f;

IVisualTreatment *IVisualTreatment::Get() {
    return pVisualTreatmentPlat;
}

IVisualTreatmentPlat::IVisualTreatmentPlat() {
}

IVisualTreatmentPlat::~IVisualTreatmentPlat() {
}

void OpenVisualTreatment() {

    eView *player1_view = eGetView(1, false);

    pVisualTreatmentPlat = new ("visual treatment", 0) IVisualTreatmentPlat;

    pVisualTreatmentPlat->Update(player1_view);

    pVisualTreatmentPlat->UpdateIndirectTexture();
}

void CloseVisualTreatment() {

    delete pVisualTreatmentPlat;
    pVisualTreatmentPlat = 0;
}

void IVisualTreatmentPlat::RenderMWVisualLook(eView *view) {

    {
        unsigned char desat = (unsigned char)(this->Desaturation * 255.0f);
        MWDesaturation.r = desat;
        MWDesaturation.g = desat;
        MWDesaturation.b = desat;
        MWDesaturation.a = desat;
    }

    {
        unsigned char brightness = gApplyTestPulseBrightnessBloom
                                       ? TestMWPulseBrightness.r
                                       : (unsigned char)(this->CombinedBrightness * 255.0f);
        MWPulseBrightness.r = brightness;
        MWPulseBrightness.g = brightness;
        MWPulseBrightness.b = brightness;
        MWPulseBrightness.a = brightness;
    }

    {
        unsigned char r = (unsigned char)(this->ColourBloomTint.x * 255.0f);
        unsigned char g = (unsigned char)(this->ColourBloomTint.y * 255.0f);
        unsigned char b = (unsigned char)(this->ColourBloomTint.z * 255.0f);

        MWColourTint.r = r;
        MWColourTint.g = g;
        MWColourTint.b = b;
        MWColourTint.a = 0;
    }

    eScreenQuadReplace(view, 0x70);
}

void IVisualTreatmentPlat::UpdateIndirectTexture() {

    for (int i = 0; i < VT_IndirectTextureRes; i++) {

        for (int j = 0; j < VT_IndirectTextureRes; j++) {

            float alpha;
            float red;
            float green = i * (1.0f / VT_IndirectTextureRes);
            float blue;

            float blackCurve = GetValueFromSpline(j * (1.0f / VT_IndirectTextureRes), &this->BlackBloomCurve);
            if (blackCurve > 1.0f) { blackCurve = 0.999f; }

            float colourBloomCurve = GetValueFromSpline(green, &this->ColourBloomCurve);
            if (colourBloomCurve > 1.0f) { colourBloomCurve = 0.999f; }

            red = blackCurve * this->BlackBloomIntensity + (1.0f - this->BlackBloomIntensity);

            green = colourBloomCurve * this->ColourBloomIntensity;

            unsigned char b_red = (unsigned char)(red * black_bloom_color_scale);
            unsigned char b_green = (unsigned char)(green * colour_bloom_color_scale);
            unsigned char b_blue = 0;
            unsigned char b_alpha = 0;

            if (zero_red) { b_red = 0; }
            if (zero_green) { b_green = 0; }

            pContrastLinearSource[j * VT_IndirectTextureRes + i] = (b_red << 16) | (b_green << 8);
        }
    }

    eSwizzle32Bit(pContrastRampPixels, pContrastLinearSource, VT_IndirectTextureRes, VT_IndirectTextureRes);
}
