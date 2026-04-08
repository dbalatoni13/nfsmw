#include "Speed/Indep/Src/EAXSound/Csis.hpp"

static const char kFX_NITROUSName[] = "FX_NITROUS";
static const char kFX_PURGEName[] = "FX_PURGE";
static const char kFX_SPARKCHATTERName[] = "FX_SPARKCHATTER";
static const char kFX_SKIDName[] = "FX_SKID";
static const char kFX_HydraulicName[] = "FX_Hydraulic";
static const char kFX_HelicopterName[] = "FX_Helicopter";
static const char kFX_Hydr_BounceName[] = "FX_Hydr_Bounce";
static const char kFX_CameraName[] = "FX_Camera";
static const char kFX_UVESName[] = "FX_UVES";
static const char kFX_RadarName[] = "FX_Radar";
static const char kFX_ScrapeName[] = "FX_Scrape";

namespace Csis {
InterfaceId FX_NITROUSId = {kFX_NITROUSName, 0x3EE6, 0x1A8B};
InterfaceId FX_PURGEId = {kFX_PURGEName, 0x3EE6, 0x439C};
InterfaceId FX_SPARKCHATTERId = {kFX_SPARKCHATTERName, 0x3EE6, 0x723E};
InterfaceId FX_SKIDId = {kFX_SKIDName, 0x3EE6, 0x005F};
InterfaceId FX_HydraulicId = {kFX_HydraulicName, 0x3EE6, 0x6508};
InterfaceId FX_HelicopterId = {kFX_HelicopterName, 0x3EE6, 0x0990};
InterfaceId FX_Hydr_BounceId = {kFX_Hydr_BounceName, 0x3EE6, 0x6048};
InterfaceId FX_CameraId = {kFX_CameraName, 0x3EE6, 0x23B1};
InterfaceId FX_UVESId = {kFX_UVESName, 0x3EE6, 0x61F6};
InterfaceId FX_RadarId = {kFX_RadarName, 0x3EE6, 0x46AF};
InterfaceId FX_ScrapeId = {kFX_ScrapeName, 0x3EE6, 0x0625};

ClassHandle gFX_NITROUSHandle;
ClassHandle gFX_PURGEHandle;
ClassHandle gFX_SPARKCHATTERHandle;
ClassHandle gFX_SKIDHandle;
ClassHandle gFX_HydraulicHandle;
ClassHandle gFX_HelicopterHandle;
ClassHandle gFX_Hydr_BounceHandle;
ClassHandle gFX_CameraHandle;
ClassHandle gFX_UVESHandle;
ClassHandle gFX_RadarHandle;
ClassHandle gFX_ScrapeHandle;
} // namespace Csis
