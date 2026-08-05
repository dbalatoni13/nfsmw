#include "Speed/Indep/Src/World/Rain.hpp"
#include "Speed/Indep/Src/Ecstasy/EcstasyData.hpp"
#include "Speed/Indep/Src/Ecstasy/eMath.hpp"
#include "Speed/Indep/Src/World/ParameterMaps.hpp"
#include "Speed/Indep/Src/Ecstasy/Texture.hpp"
#include "Speed/Indep/Src/World/WeatherMan.hpp"
#include "Speed/Indep/Src/World/TrackPath.hpp"
#include "Speed/Indep/Src/Camera/CameraMover.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

extern uint32 numCopsActive;
extern float TUNHEIGHT;
extern float WorldTimeElapsed;
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
        bSubScaled(&EntryPosition, &EntryPosition, this->MyView->pCamera->GetDirection(), 8.0f);
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
    float f = bRandom(10.0f);
    bVector3 Along;

    bSeedCurtainXPS2(&rainpoints->NormalizedPoint[this->NewSwapBuffer],
                     &Along,
                     &this->RainCurtainPos[0],
                     &this->RainCurtainPos[1],
                     &this->outvex,
                     distX,
                     f);
    rainpoints->NormalizedPoint[this->NewSwapBuffer].z -= distZ;
    bCopy(&rainpoints->NormalizedPoint[this->OldSwapBuffer],
          &rainpoints->NormalizedPoint[this->NewSwapBuffer]);
}

void Rain::SeedCurtainX(RainPointsDef *rainpoints) {
    float distX = bRandom(this->CurtainLength);
    float f = bRandom(10.0f);
    bVector3 Along;

    bSeedCurtainXPS2(&rainpoints->NormalizedPoint[this->NewSwapBuffer],
                     &Along,
                     &this->RainCurtainPos[0],
                     &this->RainCurtainPos[1],
                     &this->outvex,
                     distX,
                     f);
    bCopy(&rainpoints->NormalizedPoint[this->OldSwapBuffer], &rainpoints->NormalizedPoint[this->NewSwapBuffer]);
}

static const int showCurtain = 0;
static const float curtainWidthMod = 2.0f;

static float zspeed = 0.0f;

// STRIPPED
void Rain::UpdateAndRenderCurtain() {}

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

// STRIPPED
void Rain::Update() {}

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

    bScalePS2(&this->PrevailingWindSpeed, &this->PrevailingWindSpeed, PrevailingMult);
    eMulVector(&this->PrevailingWindSpeed, &this->world2localrot, &this->PrevailingWindSpeed);

    if (windState == CHANGE) {
        this->windTime = 0.0f;
        windState = CHANGING;
        this->DesiredWindTime = bRandom(5.0f) + 1.0f;
        this->DesiredwindSpeed.x = (bRandom(0.05f) - 0.025f) * maxWindEffect;
        this->DesiredwindSpeed.z = 0.0f;
        this->DesiredwindSpeed.y = (bRandom(0.05f) - 0.025f) * maxWindEffect;
        bAddPS2(&this->DesiredwindSpeed, &this->DesiredwindSpeed, &this->PrevailingWindSpeed);
    } else if (windState < CHANGING) {
        if (windState == STEADY) {
            this->windTime += time;
            if (this->DesiredWindTime <= this->windTime) {
                windState = CHANGE;
            }
        }
    } else if (windState == CHANGING) {
        bVector3 delta;
        bSubPS2(&delta, &this->DesiredwindSpeed, &this->windSpeed);
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

void Rain::Render() {}

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
    float camera_speed = bLengthPS2(&camera_world_velocity) * SpeedMod;

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
                bNormalizeScalePS2(&sv, &sv, camera_speed);
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

void Rain::UpdateAndRender() {}

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
