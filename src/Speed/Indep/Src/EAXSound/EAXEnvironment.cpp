#include "Speed/Indep/Src/EAXSound/EAXEnvironment.hpp"

static const int Min_Vol_to_Play_EnvSnd = 4000; // Decl: 31

static const float TrollyMaxDistance = 80.0f; // Decl: 39

static const float BridgeMaxDistance = 380.0f; // Decl: 42

static const float gfTrainTriggerMaxDistance = 225.0f; // Decl: 47
static const float gfTrainTriggerMinDistance = 20.0f;  // Decl: 48
static const int Min_Vol_Play_TRAIN_EnvSnd = 1617;     // Decl: 49

static const int VOL_SCALE_FOR_READCAM_WINDNOISE = 14000; // Decl: 52

int g_HACKTRAINVOLUME = 0; // Decl: 73
