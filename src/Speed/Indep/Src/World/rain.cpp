#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/World/ScreenEffects.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

extern uint32 numCopsActive;
extern float TUNHEIGHT;
extern float WorldTimeElapsed;
extern int TweakerPauseWorld;
extern int PrecipitationEnable;
extern float BaseDampness;
extern void fInvertMatrix(bMatrix4 *d, bMatrix4 *s);
int GetXYviewCar(eView *view, float *x, float *y);

uint32 precipDEBUG = 0;
float precipPERCENT = 1.0f;

float SNOWLINE = 350.0f;
float SNOWLINErange = 100.0f;

float RAINX = 0.02f;
float RAINY = 0.02f;
float RAINZ = 0.03f;
float RAINZconstant = 0.35f;

float SNOWX = 0.05f;
float SNOWY = 0.05f;
float SNOWZ = 0.03f;
float SNOWZconstant = 0.1f;

float SLEETX = 0.05;
float SLEETY = 0.05;
float SLEETZ = 0.03f;
float SLEETZconstant = 0.1f;

float HAILX = 0.05f;
float HAILY = 0.05f;
float HAILZ = 0.03f;
float HAILZconstant = 0.1f;

float precipBoundX = 20.0f;
float precipBoundY = 7.0f;
float precipBoundZ = 6.0f;

float precipAheadX = 8.0f;
float precipAheadY = 0.0f;
float precipAheadZ = 0.0f;

float RAINstick = 0.4f;
float SNOWstick = 2.0f;
float SLEETstick = 0.1f;
float HAILstick = 0.5f;

float RAINwindEffect = 0.3f;
float SNOWwindEffect = 1.0f;
float SLEETwindEffect = 0.4f;
float HAILwindEffect = 0.3f;

float RAINRadiusX = 0.01f;
float RAINRadiusY = 0.45f;
float RAINRadiusZ = 0.05f;

float SNOWRadiusX = 0.1f;
float SNOWRadiusY = 0.1f;
float SNOWRadiusZ = 0.1f;

float SLEETRadiusX = 0.05f;
float SLEETRadiusY = 0.05f;
float SLEETRadiusZ = 0.05f;

float HAILRadiusX = 0.05f;
float HAILRadiusY = 0.05f;
float HAILRadiusZ = 0.05f;

float DistBoundSq0 = 64.0f;

static const int32 PRECIPFogDEBUG = 0;

// TODO are the values right?
static const float PRECIPFogIntense = 0.9f;
static const float PRECIPFogRED = 111.0f;
static const float PRECIPFogGREEN = 128.0f;
static const float PRECIPFogBLUE = 137.0f;
static const float PRECIPFogStart = 74.0f;
static const float PRECIPFogFalloff = 0.75f;
static const float PRECIPFogFalloffX = 0.1f;
static const float PRECIPFogFalloffY = 1.0f;

static const int ShowPreciData = 0;

float twkCloudsMinAmount = 0.0f;
float twkRainRateOfChange = 1.0f;

float twkCloudsRateOfChange = 1.0f;

float driveFactor = -0.03f;

ParameterAccessor RainAccessor("Rain");
ParameterAccessor CloudAccessor("Clouds");

uint32 precipRENDER = 1;

GenericRegion TempRegions[1];

// TODO
extern bool EnableRainIn2P;

void TempInits() {}

void SetRainBase() {
#ifdef EA_BUILD_A124
    eGetView(EVIEW_PLAYER1, false)->Precipitation->SetRoadDampness(0.0f);
    eGetView(EVIEW_PLAYER2, false)->Precipitation->SetRoadDampness(0.0f);

    eGetView(EVIEW_PLAYER1, false)->Precipitation->Init(INACTIVE, 1.0f);
    eGetView(EVIEW_PLAYER2, false)->Precipitation->Init(INACTIVE, 1.0f);
#else
    eGetView(EVIEW_PLAYER1, false)->Precipitation->SetRoadDampness(0.0f);
    eGetView(EVIEW_PLAYER1, false)->Precipitation->Init(INACTIVE, 1.0f);

    if (EnableRainIn2P) {
        eGetView(EVIEW_PLAYER2, false)->Precipitation->SetRoadDampness(0.0f);
        eGetView(EVIEW_PLAYER2, false)->Precipitation->Init(INACTIVE, 1.0f);
    }
#endif
}

// UNSOLVED
Rain::Rain(eView *view, RainType StartType) {
    this->CloudIntensity = twkCloudsMinAmount;
    this->RoadDampness = 0.0f;
    this->intensity = 0.0f;
    this->percentPrecip[RAIN] = 0.0f;
    this->percentPrecip[INACTIVE] = 0.0f;
    this->percentPrecip[StartType] = 1.0f;
    this->NumRainPoints = -1;
    this->MyView = view;
    this->NoRain = 0;
    this->NoRainAhead = 0;
    this->PRECIPpoly[0].UVs[0][0] = 0.0f;
    this->PRECIPpoly[0].UVs[0][1] = 1.0f;
    this->PRECIPpoly[0].UVs[0][2] = 0.1f;
    this->PRECIPpoly[0].UVs[0][3] = 1.0f;
    this->PRECIPpoly[0].UVs[1][0] = 0.1f;
    this->PRECIPpoly[0].UVs[1][1] = 0.0f;
    this->PRECIPpoly[0].UVs[1][2] = 0.0f;
    this->PRECIPpoly[0].UVs[1][3] = 0.0f;

    unsigned char r = 128;
    unsigned char g = 128;
    unsigned char b = 128;
    unsigned char a = 128;

    this->PRECIPpoly[0].Colours[0][0] = r;
    this->PRECIPpoly[0].Colours[0][1] = g;
    this->PRECIPpoly[0].Colours[0][2] = b;
    this->PRECIPpoly[0].Colours[0][3] = a;

    this->PRECIPpoly[0].Colours[1][0] = r;
    this->PRECIPpoly[0].Colours[1][1] = g;
    this->PRECIPpoly[0].Colours[1][2] = b;
    this->PRECIPpoly[0].Colours[1][3] = a;

    this->PRECIPpoly[0].Colours[2][0] = r;
    this->PRECIPpoly[0].Colours[2][1] = g;
    this->PRECIPpoly[0].Colours[2][2] = b;
    this->PRECIPpoly[0].Colours[2][3] = a;

    this->PRECIPpoly[0].Colours[3][0] = r;
    this->PRECIPpoly[0].Colours[3][1] = g;
    this->PRECIPpoly[0].Colours[3][2] = b;
    this->PRECIPpoly[0].Colours[3][3] = a;

    this->PRECIPpoly[1].UVs[0][0] = 0.0f;
    this->PRECIPpoly[1].UVs[0][1] = 1.0f;
    this->PRECIPpoly[1].UVs[0][2] = 0.1f;
    this->PRECIPpoly[1].UVs[0][3] = 1.0f;
    this->PRECIPpoly[1].UVs[1][0] = 0.1f;
    this->PRECIPpoly[1].UVs[1][1] = 0.0f;
    this->PRECIPpoly[1].UVs[1][2] = 0.0f;
    this->PRECIPpoly[1].UVs[1][3] = 0.0f;

    unsigned char r1 = 100;
    unsigned char g1 = 100;
    unsigned char b1 = 100;
    unsigned char a1 = 28;

    float v;

    this->PRECIPpoly[1].Colours[0][0] = r1;
    this->PRECIPpoly[1].Colours[0][1] = g1;
    this->PRECIPpoly[1].Colours[0][2] = b1;
    this->PRECIPpoly[1].Colours[0][3] = a1;

    this->PRECIPpoly[1].Colours[1][0] = r1;
    this->PRECIPpoly[1].Colours[1][1] = g1;
    this->PRECIPpoly[1].Colours[1][2] = b1;
    this->PRECIPpoly[1].Colours[1][3] = a1;

    this->PRECIPpoly[1].Colours[2][0] = r1;
    this->PRECIPpoly[1].Colours[2][1] = g1;
    this->PRECIPpoly[1].Colours[2][2] = b1;
    this->PRECIPpoly[1].Colours[2][3] = a1;

    this->PRECIPpoly[1].Colours[3][0] = r1;
    this->PRECIPpoly[1].Colours[3][1] = g1;
    this->PRECIPpoly[1].Colours[3][2] = b1;
    this->PRECIPpoly[1].Colours[3][3] = a1;
    this->SetPrecipFogColour(0, 0, 0);
    this->inTunnel = 0;
    this->inOverpass = 0;
    this->IsValidRainCurtainPos = CT_INACTIVE;
}

void Rain::Init(RainType type, float percent) {
    TempInits();

    this->texture_info[RAIN] = GetTextureInfo(bStringHash("RAINDROP"), 0, 0);
    this->NumRainPoints = MAXRAINPOINTS;

    for (int j = 0; j < 2; j++) {
        this->NumOfType[j] = 0;
        this->DesiredNumOfType[j] = 0;
    }

    float radiusZ = RAINRadiusZ;
    float radiusY = RAINRadiusY;
    float radiusX = RAINRadiusX;
    float windEffect = RAINwindEffect;

    this->NewSwapBuffer = 0;
    this->OldSwapBuffer = 1;
    this->NumOfType[type] = this->NumRainPoints;
    this->precipWindEffect[INACTIVE][0] = 1.0f;
    this->precipWindEffect[INACTIVE][1] = 1.0f;
    this->precipWindEffect[RAIN][1] = windEffect * 0.5f;
    this->precipRadius[RAIN] = bVector3(radiusX, radiusY, radiusZ);
    this->precipSpeedRange[RAIN] = bVector3(RAINX, RAINY, RAINZ);
    this->precipWindEffect[RAIN][0] = windEffect;
    this->precipZconstant[RAIN] = RAINZconstant;
    this->windType[RAIN] = VECTOR_WIND;
    this->windTime = 0.0f;
    this->windSpeed = bVector3(0.0f, 0.0f, 0.0f);

#ifndef DISABLE_RAIN
    for (int32 i = 0; i < this->NumRainPoints; i++) {
        this->RainPointsInf[i].status = 2;
        this->RainPointsInf[i].type = static_cast<uint8>(type);
        this->RainPointsInf[i].subType = static_cast<uint8>(bRandom(2));

        this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer].x = precipAheadX + (bRandom(precipBoundX) - precipBoundX * 0.5f);
        this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer].y = precipAheadY + (bRandom(precipBoundY) - precipBoundY * 0.5f);
        this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer].z = precipAheadZ + bRandom(precipBoundZ);

        this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer] = this->RainPoints[i].NormalizedPoint[this->OldSwapBuffer];
    }

    for (int j = 0; j < 2; j++) {
        for (int i = 0; i < 10; i++) {
            this->Velocities[j][i].x = bRandom(this->precipSpeedRange[j].x) - this->precipSpeedRange[j].x * 0.5f;
            this->Velocities[j][i].y = bRandom(this->precipSpeedRange[j].y) - this->precipSpeedRange[j].y * 0.5f;
            this->Velocities[j][i].z = -bRandom(this->precipSpeedRange[j].z) - this->precipZconstant[j];
            this->Velocities[j][i].pad = -99999.9f;
        }
    }
#endif
}

// STRIPPED
Rain::~Rain() {}

float snowPercent = 1.0f;
float rainPercent = 1.0f;
float fogPercent = 0.0f;

int Chance100 = 0;
float ChancePercent = 1.0f;

float rainOverrideIntensity = 0.0f;

void SetOverRideRainIntensity(float rov) {
    rainOverrideIntensity = rov;
}

static const int TimeTestRain = 3;
static const float TimeTestFactor = 1.4f;
static const float DistanceTestFactor = 50.0f;

float IsRainingAt(float x, float y);

float GetDesiredRainIntensity(float x, float y) {
    float rolly;

    if (numCopsActive < 3) {
        if (rainOverrideIntensity > 0.0f) {
            return rainOverrideIntensity;
        }
        if (Chance100 != 0) {
            return ChancePercent;
        }
        if (rainOverrideIntensity < 0.0f) {
            return 0.0f;
        }
        rolly = IsRainingAt(x, y);
        if (!RainAccessor.IsValid()) {
            return 0.0f;
        }
        RainAccessor.CaptureData(x, y);
        if (rolly > RainAccessor.GetDataFloat(1)) {
            return 0.0f;
        }
        return RainAccessor.GetDataFloat(0);
    }
    return 0.0f;
}

static const uint32 CloudsPlease = 0;
static const uint32 NoCloudsPlease = 0;

// STRIPPED
float GetDesiredCloudyness(float x, float y) {}

// STRIPPED
float WorldWeatherTime() {}

float IsRainingAt(float x, float y) {
    float result = bCos(static_cast<bAngle>(
        static_cast<int>((x * 0.02f + WorldTimeSeconds) * 65536.0f) / 360));
    return 1.0f - bAbs(result);
}

// STRIPPED
float IsCloudyAt(float x, float y) {}

void Rain::AttachRainCurtain(float x0, float y0, float z0, float x1, float y1, float z1, float x2, float y2, float z2, float x3, float y3, float z3) {
#ifndef DISABLE_RAIN
    this->RainCurtainPos[0].x = x0;
    this->RainCurtainPos[0].y = y0;
    this->RainCurtainPos[0].z = z0;

    this->RainCurtainPos[1].x = x1;
    this->RainCurtainPos[1].y = y1;
    this->RainCurtainPos[1].z = z1;

    this->RainCurtainPos[2].x = x2;
    this->RainCurtainPos[2].y = y2;
    this->RainCurtainPos[2].z = z2;

    this->RainCurtainPos[3].x = x3;
    this->RainCurtainPos[3].y = y3;
    this->RainCurtainPos[3].z = z3;

    bLength(&this->CurtainLength, &this->RainCurtainPos[1], &this->RainCurtainPos[0]);
#endif
}

void Rain::FindCurtains() {
    CameraMover *cameraMover = this->MyView->GetCameraMover();
    if (cameraMover != nullptr && cameraMover->RenderCarPOV()) {
        bVector3 EntryPosition;

        bCopy(&EntryPosition, this->MyView->pCamera->GetPosition());
        EntryPosition -= *this->MyView->pCamera->GetDirection() * 8.0f;
        bVector2 twoDentry(EntryPosition.x, EntryPosition.y);

        static_cast<TrackPathZone *>(this->the_zone)->GetSegmentNextTo(&twoDentry, &this->ent0, &this->ent1);

        bVector2 diff = this->ent0 - this->ent1;
        bVector2 direction(diff.y, -diff.x);
        bNormalize(&direction, &direction);

        bVector2 entrancePt((static_cast<TrackPathZone *>(this->the_zone)->BBoxMin.x + static_cast<TrackPathZone *>(this->the_zone)->BBoxMax.x) * 0.5f,
                            (static_cast<TrackPathZone *>(this->the_zone)->BBoxMin.y + static_cast<TrackPathZone *>(this->the_zone)->BBoxMax.y) * 0.5f);
        bVector2 exitPt;
        static_cast<TrackPathZone *>(this->the_zone)->GetOpposite(&this->ent0, &this->ent1, &this->ext0, &this->ext1);
    }
}

void Rain::FindCurtain() {
    CameraMover *cameraMover = this->MyView->GetCameraMover();
    if (cameraMover != nullptr) {
        CameraAnchor *cameraAnchor = cameraMover->GetAnchor();
        if (cameraAnchor != nullptr) {
            bVector2 CameraDirection(this->MyView->pCamera->GetDirection()->x,
                                     this->MyView->pCamera->GetDirection()->y);
            bNormalize(&CameraDirection, &CameraDirection);

            bVector3 Position;
            bCopy(&Position, cameraAnchor->GetGeometryPosition());

            bVector2 Pos2D((this->ent0.x + this->ent1.x) * 0.5f - Position.x,
                           (this->ent0.y + this->ent1.y) * 0.5f - Position.y);
            bNormalize(&Pos2D, &Pos2D);

            if (Pos2D.x * CameraDirection.x + Pos2D.y * CameraDirection.y > 0.0f) {
                this->MyView->Precipitation->AttachRainCurtain(
                    this->ent0.x, this->ent0.y, Position.z + TUNHEIGHT,
                    this->ent1.x, this->ent1.y, Position.z + TUNHEIGHT,
                    this->ent0.x, this->ent0.y, Position.z,
                    this->ent0.x, this->ent0.y, Position.z);
            } else {
                this->MyView->Precipitation->AttachRainCurtain(
                    this->ext0.x, this->ext0.y, Position.z + TUNHEIGHT,
                    this->ext1.x, this->ext1.y, Position.z + TUNHEIGHT,
                    this->ext0.x, this->ext0.y, Position.z,
                    this->ext0.x, this->ext0.y, Position.z);
            }
        }
    }
}

void Rain::SeedCurtainXZ(RainPointsDef *rainpoints) {
    float distX = bRandom(this->CurtainLength);
    float distZ = bRandom(TUNHEIGHT);
    float distY = bRandom(10.0f);
    bVector3 Along;
    bSub(&Along, &this->RainCurtainPos[1], &this->RainCurtainPos[0]);
    bNormalize(&Along, &Along);
    bScaleAdd(&rainpoints->NormalizedPoint[this->NewSwapBuffer],
              &this->RainCurtainPos[0], &Along, distX);
    bScaleAdd(&rainpoints->NormalizedPoint[this->NewSwapBuffer],
              &rainpoints->NormalizedPoint[this->NewSwapBuffer], &this->outvex, distY);
    rainpoints->NormalizedPoint[this->NewSwapBuffer].z -= distZ;
    bCopy(&rainpoints->NormalizedPoint[this->OldSwapBuffer],
          &rainpoints->NormalizedPoint[this->NewSwapBuffer]);
}

void Rain::SeedCurtainX(RainPointsDef *rainpoints) {
    float distX = bRandom(this->CurtainLength);
    float distY = bRandom(10.0f);
    bVector3 Along;
    bSub(&Along, &this->RainCurtainPos[1], &this->RainCurtainPos[0]);
    bNormalize(&Along, &Along);
    bScaleAdd(&rainpoints->NormalizedPoint[this->NewSwapBuffer],
              &this->RainCurtainPos[0], &Along, distX);
    bScaleAdd(&rainpoints->NormalizedPoint[this->NewSwapBuffer],
              &rainpoints->NormalizedPoint[this->NewSwapBuffer], &this->outvex, distY);
    bCopy(&rainpoints->NormalizedPoint[this->OldSwapBuffer], &rainpoints->NormalizedPoint[this->NewSwapBuffer]);
}

static const int showCurtain = 0;
static const float curtainWidthMod = 2.0f;

static float zspeed = 0.0f;

void Rain::UpdateAndRenderCurtain() {
    int numCurtainPoints;
    bVector3 downV;
    bVector3 acrossV;
    bVector3 *CameraDirection;
    ePoly *PRECIPpolyPoint;
    bVector3 CamVelloc;
    bMatrix4 l2w;
    l2w.v0 = this->local2world.v0;
    l2w.v1 = this->local2world.v1;
    l2w.v2 = this->local2world.v2;
    l2w.v3 = this->local2world.v3;
    bVector2 outvex2;
    bVector2 cp;
    int i;

    CameraDirection = this->MyView->pCamera->GetDirection();
    PRECIPpolyPoint = &this->PRECIPpoly[0];
    numCurtainPoints = static_cast<int>(this->intensity * 200.0f);

    l2w.v3.x = 0.0f;
    l2w.v3.y = 0.0f;
    l2w.v3.z = 0.0f;
    l2w.v3.w = 1.0f;
    eMulVector(&CamVelloc, &l2w, &this->CamVelLOCAL);

    cp.x = (this->RainCurtainPos[0].x + this->RainCurtainPos[1].x) * 0.5f -
           (static_cast<TrackPathZone *>(this->the_zone)->BBoxMin.x +
            static_cast<TrackPathZone *>(this->the_zone)->BBoxMax.x) *
               0.5f;
    cp.y = (this->RainCurtainPos[0].y + this->RainCurtainPos[1].y) * 0.5f -
           (static_cast<TrackPathZone *>(this->the_zone)->BBoxMin.y +
            static_cast<TrackPathZone *>(this->the_zone)->BBoxMax.y) *
               0.5f;
    outvex2 = cp;
    bNormalize(&outvex2, &outvex2);
    this->outvex = bVector3(outvex2.x, outvex2.y, 0.0f);

    for (i = 0; i < numCurtainPoints; ++i) {
        bVector3 *RpointN = &this->CurtainRainPoints[i].NormalizedPoint[this->NewSwapBuffer];
        bVector3 *RpointNold = &this->CurtainRainPoints[i].NormalizedPoint[this->OldSwapBuffer];

        bScale(&acrossV, &this->windSpeed, this->precipWindEffect[RAIN][1]);
        bAdd(&downV, &this->Velocities[RAIN][i % 10], &acrossV);
        bAdd(RpointN, RpointNold, &CamVelloc);
        bAdd(RpointN, RpointN, &downV);

        if (RpointN->z < this->RainCurtainPos[3].z || this->RainCurtainPos[0].z < RpointN->z) {
            this->SeedCurtainX(&this->CurtainRainPoints[i]);
        } else {
            bSub(&downV, RpointNold, RpointN);
            bNormalize(&downV, &downV);
            bCross(&acrossV, CameraDirection, &downV);
            acrossV *= this->precipRadius[RAIN].x * 2.0f;
            downV *= this->precipRadius[RAIN].y + this->LenModifier;

            bAdd(&PRECIPpolyPoint->Vertices[0], &acrossV, &downV);
            bSub(&PRECIPpolyPoint->Vertices[0], RpointN, &PRECIPpolyPoint->Vertices[0]);
            bSub(&PRECIPpolyPoint->Vertices[1], &acrossV, &downV);
            bAdd(&PRECIPpolyPoint->Vertices[1], RpointN, &PRECIPpolyPoint->Vertices[1]);
            bSub(&PRECIPpolyPoint->Vertices[3], &downV, &acrossV);
            bAdd(&PRECIPpolyPoint->Vertices[3], RpointN, &PRECIPpolyPoint->Vertices[3]);
            bAdd(&PRECIPpolyPoint->Vertices[2], &downV, &acrossV);
            bAdd(&PRECIPpolyPoint->Vertices[2], RpointN, &PRECIPpolyPoint->Vertices[2]);
            this->MyView->Render(PRECIPpolyPoint, this->texture_info[RAIN], &eMathIdentityMatrix, 0, 0.5f);
        }
    }
}

void Rain::Debug() {
    float radiusZ = RAINRadiusZ;
    float radiusY = RAINRadiusY;
    float radiusX = RAINRadiusX;
    float windEffect = RAINwindEffect;

    snowPercent = 1.0f - rainPercent;
    this->precipWindEffect[RAIN][1] = windEffect * 0.5f;
    this->precipRadius[RAIN] = bVector3(radiusX, radiusY, radiusZ);
    this->precipWindEffect[RAIN][0] = windEffect;
    this->precipSpeedRange[RAIN] = bVector3(RAINX, RAINY, RAINZ);
    this->precipZconstant[RAIN] = RAINZconstant;

    if (zspeed != RAINZconstant) {
        for (int i = 0; i < 2; ++i) {
            for (int j = 0; j < 10; ++j) {
                this->Velocities[i][j].x = bRandom(this->precipSpeedRange[i].x) -
                                           this->precipSpeedRange[i].x * 0.5f;
                this->Velocities[i][j].y = bRandom(this->precipSpeedRange[i].y) -
                                           this->precipSpeedRange[i].y * 0.5f;
                this->Velocities[i][j].z = -bRandom(this->precipSpeedRange[i].z) - this->precipZconstant[i];
                this->Velocities[i][j].pad = -99999.9f;
            }
        }
        zspeed = RAINZconstant;
    }

    this->Change(RAIN, rainPercent * (1.0f - fogPercent));
}

static const int windON = 1;
static const int freezing = 0;
static const float WetRamp = 0.005f;
static const float DryRamp = 0.005f;

static const float AngMuly = 20.0f;
static const float RainAngMult = 0.00005;
static const float FloatClamp = 400.0f;
static const int ForceUpdate = 0;

static const float RainAheadCut = 9.5f;
int32 ONEpNoRainConnexion = 0;

float cameraMod = 1.0f;
static const float lookbackoffset = 10.0f;

void Rain::Update() {
    float timeMod;
    float tempMod;
    bVector3 MyCarPos;
    bVector3 MyCarDir;
    Camera *view_camera;
    bVector3 *CameraPosition;
    bVector3 *CameraVelocity;
    CameraMover *cameraMover;
    CameraAnchor *cameraAnchor;
    float curElev;
    TrackPathZone *zone;
    bVector3 Ahead;
    bAngle matAng;
    bMatrix4 matmat;
    bVector3 CarCross;
    static bAngle oldangle[2];
    float FloatAngle;
    uint32 tb;
    int total;
    uint32 diff;
    uint32 steadyPrecip;

    if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
        return;
    }

    if (TweakerPauseWorld != 0) {
        return;
    }

    timeMod = WorldTimeElapsed * 30.0f;
    if (timeMod > 1.0f) {
        timeMod = 1.0f;
    }
    timeMod = 1.0f - timeMod;
    tempMod = timeMod * timeMod;
    timeMod = 1.0f - tempMod * timeMod;

    cameraMover = this->MyView->GetCameraMover();
    view_camera = this->MyView->GetCamera();
    CameraPosition = view_camera->GetPosition();
    CameraVelocity = view_camera->GetVelocityPosition();
    cameraAnchor = cameraMover->GetAnchor();

    if (cameraMover == nullptr) {
        return;
    }

    if (cameraAnchor == nullptr) {
        this->local2world = *view_camera->GetCameraMatrix();
        this->local2world.v3.w = 1.0f;
        this->local2world.v3.x = 0.0f;
        this->local2world.v3.y = 0.0f;
        this->local2world.v3.z = 0.0f;
        MyCarDir = *view_camera->GetDirection();
        bScaleAdd(&MyCarPos, CameraPosition, &MyCarDir, cameraMod);
    } else {
        this->local2world = *cameraAnchor->GetGeometryOrientation();
        MyCarPos = *cameraAnchor->GetGeometryPosition();
        MyCarDir = *cameraAnchor->GetForwardVector();
        if (cameraMover->GetLookbackAngle() != 0) {
            bScaleAdd(&MyCarPos, &MyCarPos, &MyCarDir, -lookbackoffset);
        }
    }

    this->twoDpos.x = CameraPosition->x;
    this->twoDpos.y = CameraPosition->y;
    this->inTunnel = 0;
    this->inOverpass = 0;
    curElev = 9999.0f;

    zone = TheTrackPathManager.FindZone(&this->twoDpos, TRACK_PATH_ZONE_TUNNEL, nullptr);
    if (zone != nullptr && MyCarPos.z < zone->GetElevation()) {
        this->the_zone = zone;
        this->inTunnel = 1;
        curElev = zone->GetElevation();
    }

    zone = TheTrackPathManager.FindZone(&this->twoDpos, TRACK_PATH_ZONE_OVERPASS, nullptr);
    if (zone != nullptr && MyCarPos.z < zone->GetElevation() && zone->GetElevation() < curElev) {
        this->the_zone = zone;
        this->inOverpass = 1;
        curElev = zone->GetElevation();
    }

    zone = TheTrackPathManager.FindZone(&this->twoDpos, TRACK_PATH_ZONE_OVERPASS_SMALL, nullptr);
    if (zone != nullptr && MyCarPos.z < zone->GetElevation() && zone->GetElevation() < curElev) {
        this->the_zone = zone;
        this->inOverpass = 1;
        curElev = zone->GetElevation();
    }

    zone = TheTrackPathManager.FindZone(&this->twoDpos, TRACK_PATH_ZONE_GARAGE, nullptr);
    if (zone != nullptr && MyCarPos.z < zone->GetElevation() && zone->GetElevation() < curElev) {
        this->the_zone = zone;
        this->inOverpass = 1;
    }

    fInvertMatrix(&this->world2localrot, &this->local2world);
    this->Wind(WorldTimeElapsed);
    this->Change(RAIN, rainPercent);

    if (cameraMover->IsDriveCamera()) {
        eMulVector(&this->CamVelLOCAL, &this->world2localrot, CameraVelocity);
        this->CamVelLOCAL *= driveFactor;
        this->CamVelLOCAL.y = 0.0f;
        this->LenModifier = bLength(&this->CamVelLOCAL);
        if (this->LenModifier > 1.0f) {
            this->LenModifier = 1.0f;
            bNormalize(&this->CamVelLOCAL, &this->CamVelLOCAL);
        } else {
            bFill(&this->CamVelLOCAL, 0.0f, 0.0f, 0.0f);
        }
    } else {
        bFill(&this->CamVelLOCAL, 0.0f, 0.0f, 0.0f);
    }

    if (precipDEBUG != 0) {
        this->Debug();
    }

    bNormalize(&MyCarDir, &MyCarDir);
    bFill(&Ahead, 1.0f, 0.0f, 0.0f);
    matAng = bACos(bDot(MyCarDir, Ahead));
    if (MyCarDir.y < 0.0f) {
        matAng = -0x44c - matAng;
    }

    eCreateRotationZ(&matmat, matAng);
    bCopy(&matmat.v3, &MyCarPos, 1.0f);
    this->local2world = matmat;
    bCross(&CarCross, &this->OldCarDirection, &MyCarDir);

    FloatAngle = static_cast<float>(oldangle[this->MyView->ID != EVIEW_FIRST_PLAYER] - matAng);
    oldangle[this->MyView->ID != EVIEW_FIRST_PLAYER] = matAng;
    if (FloatAngle >= 32768.0f) {
        FloatAngle -= 65536.0f;
    }
    if (FloatAngle > FloatClamp) {
        FloatAngle = FloatClamp;
    }
    if (FloatAngle < -FloatClamp) {
        FloatAngle = -FloatClamp;
    }

    this->aabbMin.x = precipAheadX - precipBoundX * 0.5f;
    this->aabbMin.y = precipAheadY - precipBoundY * 0.5f;
    this->aabbMax.x = precipAheadX + precipBoundX * 0.5f;
    this->aabbMax.y = precipAheadY + precipBoundY * 0.5f;

    tb = this->OldSwapBuffer;
    this->OldSwapBuffer = this->NewSwapBuffer;
    this->NewSwapBuffer = tb;

    total = 0;
    for (int j = 0; j < 2; ++j) {
        total += this->DesiredNumOfType[j];
    }
    diff = 400 - total;
    if (diff != 0) {
        if (diff <= this->DesiredNumOfType[0]) {
            this->DesiredNumOfType[0] -= diff;
        } else {
            for (int j = 1; j < 2; ++j) {
                if (diff <= this->DesiredNumOfType[j]) {
                    this->DesiredNumOfType[j] -= diff;
                    break;
                }
            }
        }
    }

    steadyPrecip = 1;
    if (this->NumOfType[0] == this->DesiredNumOfType[0]) {
        for (int j = 1; j < 2; ++j) {
            if (this->NumOfType[j] != this->DesiredNumOfType[j]) {
                steadyPrecip = 0;
                break;
            }
        }
    } else {
        steadyPrecip = 0;
    }

    this->twoDpos.x = MyCarPos.x;
    this->twoDpos.y = MyCarPos.y;
    this->NoRain = 0;
    if (this->inOverpass != 0 || this->inTunnel != 0) {
        this->NoRain = 1;
    }
    this->NoRainAhead = this->NoRain;
    if (this->MyView->ID == EVIEW_FIRST_PLAYER) {
        ONEpNoRainConnexion = this->NoRain;
    }

    if (this->NumRainPoints > 0) {
        int32 i = 0;
        do {
            RainType rType = static_cast<RainType>(this->RainPointsInf[i].type);
            RainSubType rSubType = static_cast<RainSubType>(this->RainPointsInf[i].subType);

            if (this->RainPointsInf[i].status == CT_INACTIVE) {
                bVector3 *RpointN = &this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer];
                bVector3 *RpointNold = &this->RainPoints[i].NormalizedPoint[this->OldSwapBuffer];
                bVector3 velocity;
                RpointN->y += RpointN->x * RainAngMult * FloatAngle;
                RpointNold->y += RpointNold->x * RainAngMult * FloatAngle;
                bScale(&velocity, &this->windSpeed, this->precipWindEffect[rType][rSubType]);
                bAdd(&velocity, &this->Velocities[rType][i % 10], &velocity);
                bAdd(RpointN, &this->CamVelLOCAL, RpointNold);
                bScaleAdd(RpointN, RpointN, &velocity, timeMod);

                if (RpointN->z < 0.0f) {
                    this->RainPointsInf[i].status = CT_ACTIVE;
                } else if (RpointN->x > this->aabbMax.x) {
                    RpointN->x -= precipBoundX;
                    RpointNold->x -= precipBoundX;
                } else if (RpointN->x < this->aabbMin.x) {
                    RpointN->x += precipBoundX;
                    RpointNold->x += precipBoundX;
                } else if (RpointN->y > this->aabbMax.y) {
                    RpointN->y -= precipBoundY;
                    RpointNold->y -= precipBoundY;
                } else if (RpointN->y < this->aabbMin.y) {
                    RpointN->y += precipBoundY;
                    RpointNold->y += precipBoundY;
                }
            } else {
                if (this->RainPointsInf[i].status > CT_TURNON) {
                    continue;
                }

                if (steadyPrecip == 0) {
                    for (int j = 0; j < 2; ++j) {
                        if (rType != j && this->NumOfType[j] < this->DesiredNumOfType[j]) {
                            this->NumOfType[rType]--;
                            rType = static_cast<RainType>(j);
                            this->RainPointsInf[i].type = rType;
                            this->NumOfType[rType]++;
                        }
                    }
                }

                if (this->RainPointsInf[i].type != INACTIVE) {
                    if (this->RainPointsInf[i].status == CT_ACTIVE) {
                        this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer].z += precipBoundZ;
                        this->RainPoints[i].NormalizedPoint[this->OldSwapBuffer].z += precipBoundZ;
                    }
                    this->RainPointsInf[i].status = CT_INACTIVE;
                    continue;
                }

                if (this->RainPointsInf[i].status == CT_ACTIVE) {
                    this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer].z = precipAheadZ + bRandom(precipBoundZ);
                    this->RainPointsInf[i].status = CT_TURNON;
                    continue;
                }

                if (this->NoRain != 0 && this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer].x <= RainAheadCut) {
                    this->NumOfType[rType]--;
                    this->RainPointsInf[i].type = INACTIVE;
                    this->NumOfType[INACTIVE]++;
                    this->RainPointsInf[i].status = CT_TURNON;
                }
            }
            ++i;
        } while (i < this->NumRainPoints);
    }
}

int windAngType = 4;
float windAng = 0.0f;
float swayMax = 1.5f;
float wspeed = 45.0f;

bVector3 windAxis;

// UNSOLVED
void CreateWindRotMatrix(eView *view, bMatrix4 *windrot, int offset, bMatrix4 *l2w) {
    static int index = 0;
    bMatrix4 local2world(*l2w);
    float sway = bSin(bDegToAng(windAng + offset)) * swayMax;

    bIdentity(windrot);
    windAxis = bVector3(1.0f, 0.0f, 0.0f);

    if (view->Precipitation != nullptr) {
        bNormalize(&windAxis, view->Precipitation->GetWind());
    }

    local2world.v1.x *= -1.0f;
    local2world.v0.y *= -1.0f;
    local2world.v3.x = 0.0f;
    local2world.v3.y = 0.0f;
    local2world.v3.z = 0.0f;
    local2world.v3.w = 1.0f;
    eMulVector(&windAxis, &local2world, &windAxis);
    eCreateAxisRotationMatrix(windrot, windAxis, bDegToAng(sway));
    eRotateZ(windrot, windrot, bDegToAng(sway));
}

float maxWindEffect = 25.0f;

ParameterAccessorBlendByDistance WindAccessor[2] = {"Wind", "Wind"};

float PrevailingMult = 0.01f;

void Rain::Wind(float time) {
    static ChangingStatus windState = CHANGE;
    static float changetime = 0.0f;
    int idIndex = this->MyView->ID + EVIEW_NONE;

    if (WindAccessor[idIndex].IsValid()) {
        float x = 0.0f;
        float y = 0.0f;
        GetXYviewCar(this->MyView, &x, &y);
        WindAccessor[idIndex].CaptureData(x, y, 50.0f);
        WindAccessor[idIndex].GetDataFloat(0);
        float angleValue = WindAccessor[idIndex].GetDataFloat(1);
        bAngle angle = bDegToAng(angleValue);
        this->PrevailingWindSpeed.x = bSin(angle);
        this->PrevailingWindSpeed.y = bCos(angle);
        this->PrevailingWindSpeed.z = 0.0f;
    }

    this->PrevailingWindSpeed *= PrevailingMult;
    eMulVector(&this->PrevailingWindSpeed, &this->world2localrot, &this->PrevailingWindSpeed);

    if (windState == CHANGE) {
        this->windTime = 0.0f;
        windState = CHANGING;
        this->DesiredWindTime = bRandom(5.0f) + 1.0f;
        this->DesiredwindSpeed.x = (bRandom(0.05f) - 0.025f) * maxWindEffect;
        this->DesiredwindSpeed.z = 0.0f;
        this->DesiredwindSpeed.y = (bRandom(0.05f) - 0.025f) * maxWindEffect;
        this->DesiredwindSpeed += this->PrevailingWindSpeed;
    } else if (windState < CHANGING) {
        if (windState == STEADY) {
            this->windTime += time;
            if (this->DesiredWindTime <= this->windTime) {
                windState = CHANGE;
            }
        }
    } else if (windState == CHANGING) {
        bVector3 delta;
        bSub(&delta, &this->DesiredwindSpeed, &this->windSpeed);
        changetime += time;
        this->windSpeed.x += delta.x * time;
        this->windSpeed.y += delta.y * time;
        if (changetime >= 1.0f) {
            changetime = 0.0f;
            windState = STEADY;
        }
    }
}

// STRIPPED
void Rain::Reset() {}

float FrustrumFactor = 0.0001f;

float precipBoundD = 10.0f;

// STRIPPED
void Rain::Seed(RainPointsDef *rainpoints, bVector3 *CameraPosition, bVector3 *CameraDirection) {}

void Rain::SetRainIntensity(float percent) {
    if (precipDEBUG != 0) {
        intensity = precipPERCENT;
    } else {
        intensity = percent;
    }

    DesiredNumOfType[1] = fptoui((1.0f - intensity) * 400.0f);
    DesiredActive = 400 - DesiredNumOfType[1];
    float sumPercent = 0.0f;
    for (int j = 0; j < 1; ++j) {
        sumPercent += Percentages[j];
    }
    Percentages[1] = 1.0f - sumPercent;
}

// STRIPPED
void Rain::Change(RainType type, float percent) {
    uint32 *desired_base = this->DesiredNumOfType;
    uint32 *desired_num = desired_base + type;
    *desired_num = fptoui(percent * static_cast<float>(DesiredActive));
    Percentages[type] = percent;
}

float rw = 0.2f;
float rh = 0.2f;
float rl = 0.2f;
unsigned int RainCube = 0;
float px0 = -1.0f;
float py0 = -1.0f;
float px1 = 1.0f;
float py1 = -1.0f;
float px2 = 1.0f;
float py2 = 1.0f;
float px3 = -1.0f;
float py3 = 1.0f;

void Rain::Render() {
    if (PrecipitationEnable == 0 || precipRENDER == 0 || TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
        return;
    }

    bMatrix4 Rmat;
    bVector3 CamDirWORLD;
    bVector3 CamPosWORLD;
    bVector3 *CameraDirection;
    bVector3 *CameraPosition;
    bMatrix4 *local_world;
    Camera *view_camera;
    uint32 NumRainPointsLocal;
    uint32 *NumOfTypes;
    int j;

    if (this->renderCount != 0) {
        bIdentity(&Rmat);
        view_camera = this->MyView->pCamera;
        CameraDirection = view_camera->GetDirection();
        CameraPosition = view_camera->GetPosition();
        CamDirWORLD = *CameraDirection;
        CamPosWORLD = *CameraPosition;
        CameraDirection = &CamDirWORLD;
        eMulVector(&CamDirWORLD, &this->world2localrot, CameraDirection);

        local_world = eFrameMallocMatrix(1);
        if (local_world != nullptr) {
            *local_world = this->local2world;

            NumRainPointsLocal = this->NumRainPoints;
            NumOfTypes = this->NumOfType;
            for (j = 0; j < NUMTYPES - 1; ++j) {
                uint32 NumOfTypeLocal = NumOfTypes[j];
                if (NumOfTypeLocal != 0) {
                    if (NumRainPointsLocal == 0) {
                        continue;
                    }

                    uint32 i;
                    for (i = 0; i < NumRainPointsLocal; ++i) {
                        RainType rType = static_cast<RainType>(this->RainPointsInf[i].type);
                        if (rType == j && this->RainPointsInf[i].status == 0) {
                            bVector3 downV;
                            bVector3 acrossV;
                            bSub(&downV, &this->RainPoints[i].NormalizedPoint[this->OldSwapBuffer],
                                 &this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer]);
                            bNormalize(&downV, &downV);
                            bCross(&acrossV, CameraDirection, &downV);
                            acrossV *= this->precipRadius[j].x;
                            downV *= this->precipRadius[j].y + this->LenModifier;
                            bAdd(&this->PRECIPpoly[0].Vertices[0], &acrossV, &downV);
                            bSub(&this->PRECIPpoly[0].Vertices[0],
                                 &this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer],
                                 &this->PRECIPpoly[0].Vertices[0]);
                            bSub(&this->PRECIPpoly[0].Vertices[1], &acrossV, &downV);
                            bAdd(&this->PRECIPpoly[0].Vertices[1],
                                 &this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer],
                                 &this->PRECIPpoly[0].Vertices[1]);
                            bSub(&this->PRECIPpoly[0].Vertices[3], &downV, &acrossV);
                            bAdd(&this->PRECIPpoly[0].Vertices[3],
                                 &this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer],
                                 &this->PRECIPpoly[0].Vertices[3]);
                            bAdd(&this->PRECIPpoly[0].Vertices[2], &downV, &acrossV);
                            bAdd(&this->PRECIPpoly[0].Vertices[2],
                                 &this->RainPoints[i].NormalizedPoint[this->NewSwapBuffer],
                                 &this->PRECIPpoly[0].Vertices[2]);

                            this->MyView->Render(&this->PRECIPpoly[0], this->texture_info[j], local_world, 0, 2.0f);
                        }
                    }
                }
            }
        }
    }
}

OnScreenRain::OnScreenRain() {
    this->NumOnScreen = 0;
    int i = 0;
    do {
        this->Points[i].x = 0.0f;
        this->Points[i].y = 0.0f;
        this->Points[i].timer = 0.0f;
        this->Points[i].Maxtimer = bRandom(5.0f) + 0.3f;
        this->Points[i].DripSpeed = static_cast<float>(bRandom(0.1f) + 0.02);
        this->Points[i].Size = bRandom(1.0f);

        int shape = i + 3;
        int minimumShape = -1;
        if (minimumShape < i) {
            shape = i;
        }
        this->Points[i].DripShape = i + (shape >> 2) * -4;
        ++i;
    } while (i < MAX_OS_RAINPOINTS);
}

float DripSpeed = 0.2f;
float SpeedMod = 0.0001f;
float DropShapeSpeedChange = 0.0025f;
int OverRide = 0;

void OnScreenRain::Update(eView *view) {
    float time = WorldTimeElapsed;
    bVector3 camera_world_velocity;
    bCopy(&camera_world_velocity, view->pCamera->GetVelocityPosition());
    float camera_speed = bLength(&camera_world_velocity) * SpeedMod;

    if (view->Precipitation->NoRain == 0) {
        if (static_cast<int>(eGetCurrentViewMode()) < 3) {
            this->NumOnScreen = view->Precipitation->GetRainIntensity() != 0.0f ? 20 : 0;
        } else {
            this->NumOnScreen = view->Precipitation->GetRainIntensity() != 0.0f ? 10 : 0;
        }
    } else {
        this->NumOnScreen = 0;
    }

    int num_on_screen = this->NumOnScreen;
    for (int i = 0; i < num_on_screen; ++i) {
        OnScreenRainPointsDef *point = &this->Points[i];
        if (OverRide == 0) {
            point->timer -= time;
        }

        if (point->timer <= 0.0f) {
            point->timer = point->Maxtimer;
            point->x = bRandom(1.0f);
            point->y = bRandom(1.0f);
        } else {
            point->y += point->DripSpeed * time * DripSpeed;
            if (point->y > 1.0f || point->x < 0.0f) {
                point->timer = 0.0f;
            } else if (point->x > 1.0f) {
                point->timer = 0.0f;
            } else {
                bVector3 sv(point->x - 0.5f, point->y - 0.1f, 0.0f);
                bNormalize(&sv, &sv);
                sv *= camera_speed;
                point->x += sv.x;
            }

            if (DropShapeSpeedChange < camera_speed) {
                int shape = point->DripShape + 1;
                int next_shape = point->DripShape + 4;
                if (shape > -1) {
                    next_shape = shape;
                }
                point->DripShape = shape + (next_shape >> 2) * -4;
            }
        }
    }
}

void OnScreenRain::GetData(int index, float *x, float *y, float *decay, float *size, int *dripShape) {
    *x = this->Points[index].x;
    *y = this->Points[index].y;
    *decay = this->Points[index].timer / this->Points[index].Maxtimer;
    *size = this->Points[index].Size;
    if (dripShape != nullptr) {
        *dripShape = this->Points[index].DripShape;
    }
}

float RAINbias = 0.0f;
float SNOWbias = 0.0f;
float SLEETbias = 0.0f;
float HAILbias = 0.0f;
float FOGbias = 0.0f;

static const int WatchRain = 0;

void Rain::UpdateAndRender() {
    float time = WorldTimeElapsed;

    if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
        return;
    }

    if (TheGameFlowManager.IsPaused()) {
        ScreenEffectDef SE_def;
        SE_def.r = static_cast<float>(this->fogR);
        SE_def.g = static_cast<float>(this->fogG);
        SE_def.b = static_cast<float>(this->fogB);
        SE_def.a = 128.0f;
        SE_def.intensity = this->CloudIntensity;
        SE_def.UpdateFnc = nullptr;
        this->MyView->ScreenEffects->AddScreenEffect(SE_TINT, &SE_def, 1, SEC_FRAME);
        this->Render();
        return;
    }

    this->OSrain.Update(this->MyView);
    windAng += time * wspeed;
    if (windAng > 360.0f) {
        windAng = 0.0f;
    }

    if (PrecipitationEnable == 0) {
        this->inOverpass = 0;
        this->inTunnel = 0;
        uint32 inTunnel = static_cast<uint32>(AmIinATunnelSlow(this->MyView, 1));
        this->inTunnel = static_cast<int>(inTunnel);
        if ((inTunnel & 2) != 0) {
            this->inTunnel = 0;
            this->inOverpass = 1;
        }
        if (this->inTunnel != 0) {
            this->inTunnel = 1;
        }
        this->RoadDampness = 0.0f;
        this->intensity = 0.0f;
        return;
    }

    if (precipDEBUG == 0) {
        if (TheGameFlowManager.GetState() != GAMEFLOW_STATE_RACING) {
            this->DesiredRoadDampness = 1.0f;
            return;
        }
        float desiredRoadDampness = BaseDampness;
        if (this->intensity == 0.0f) {
            desiredRoadDampness = 0.0f;
        }
        this->DesiredRoadDampness = desiredRoadDampness;
    } else {
        this->RoadDampness = BaseDampness;
    }

    float roadDampness = this->RoadDampness;
    float roadDampnessDelta = this->DesiredRoadDampness - roadDampness;
    if (roadDampness < this->DesiredRoadDampness) {
        this->RoadDampness = roadDampness + roadDampnessDelta * 0.005f;
    } else {
        roadDampness += roadDampnessDelta * 0.01f;
        this->RoadDampness = roadDampness;
        if (roadDampness < 0.05f) {
            this->RoadDampness = 0.0f;
        }
    }

    if (RainAccessor.IsValid() && CloudAccessor.IsValid()) {
        float x = 0.0f;
        float y = 0.0f;
        GetXYviewCar(this->MyView, &x, &y);
        this->DesiredIntensity = GetDesiredRainIntensity(x, y);
        float desiredCloudyness = this->DesiredIntensity;
        if (desiredCloudyness <= 0.0f) {
            desiredCloudyness = GetDesiredCloudyness(x, y);
        }
        this->DesiredCloudyness = desiredCloudyness;
        if (static_cast<int>(eGetCurrentViewMode()) > 2) {
            this->DesiredIntensity *= 0.125f;
        }
    }

    if (time == 0.0f) {
        time = 0.001f;
    }

    this->intensity +=
        (this->DesiredIntensity - this->intensity) / ((1.0f / time) * twkRainRateOfChange);
    if (this->DesiredIntensity == 0.0f && this->intensity < 0.01f) {
        this->intensity = 0.0f;
    }
    if (this->intensity > 1.0f) {
        this->intensity = 1.0f;
    }

    this->CloudIntensity +=
        (this->DesiredCloudyness - this->CloudIntensity) / ((1.0f / time) * twkCloudsRateOfChange);
    if (this->DesiredCloudyness == 0.0f && this->CloudIntensity < twkCloudsMinAmount + 0.01f) {
        this->CloudIntensity = twkCloudsMinAmount;
    }
    if (this->CloudIntensity > 1.0f) {
        this->CloudIntensity = 1.0f;
    }

    this->SetRainIntensity(this->intensity);
    this->percentPrecip[RAIN] = 1.0f;
    this->Update();
    this->Render();

    int inTunnel = AmIinATunnel(this->MyView, 1);
    if (inTunnel == 0) {
        this->IsValidRainCurtainPos = CT_INACTIVE;
        return;
    }

    if (this->IsValidRainCurtainPos != CT_ACTIVE) {
        if (this->IsValidRainCurtainPos < CT_TURNON) {
            if (this->IsValidRainCurtainPos == CT_INACTIVE) {
                this->IsValidRainCurtainPos = CT_TURNON;
                RainPointsDef *rainpoints = this->CurtainRainPoints;
                this->FindCurtains();
                this->FindCurtain();
                int i = MAXCURTAINRAINPOINTS - 1;
                do {
                    this->SeedCurtainXZ(rainpoints);
                    --i;
                    ++rainpoints;
                } while (i > -1);
            }
        } else if (this->IsValidRainCurtainPos != CT_TURNON) {
            if (this->IsValidRainCurtainPos == CT_OVERIDE) {
                this->IsValidRainCurtainPos = CT_ACTIVE;
            }
        } else {
            this->IsValidRainCurtainPos = CT_ACTIVE;
        }
    }

    this->FindCurtain();
    this->UpdateAndRenderCurtain();
}

ParameterAccessorBlendByDistance FogAccessor[2] = {"Normal Fog", "Normal Fog"};
ParameterAccessorBlendByDistance RainFogAccessor[2] = {"Rain Fog", "Rain Fog"};
ParameterAccessor FogBlendDistAccessor[2] = {"Fog Blend Distance", "Fog Blend Distance"};

int FogControlOverRide = 0;
float BaseFogFalloff = 0.62f;
float BaseFogFalloffX = 0.31f;
float BaseFogFalloffY = 1.3f;

int32 BaseWeatherFogColourR = 0x4B;
int32 BaseWeatherFogColourG = 0x69;
int32 BaseWeatherFogColourB = 0x6E;

static const bool grabFogInfo = false;

// STRIPPED
int FogQuery::CalculateFogInfo(struct View *view, int InFE) {}

int AmIinATunnel(eView *view, int CheckOverPass) {
    if (view->Precipitation == nullptr) {
        return 0;
    }

    if (CheckOverPass != 0) {
        return static_cast<int>(view->Precipitation->inTunnel != 0 || view->Precipitation->inOverpass != 0);
    }

    return view->Precipitation->inTunnel;
}

int AmIinATunnelSlow(eView *view, int CheckOverPass) {}
