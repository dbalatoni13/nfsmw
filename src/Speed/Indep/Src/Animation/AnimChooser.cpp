#include "AnimChooser.hpp"
#include "Speed/Indep/bWare/Inc/Strings.hpp"

CAnimChooser TheAnimChooser;

static unsigned int WAM_START_TRIGGER = bStringHash("WAM_START_TRIGGER");
static unsigned int WAM_STOP_TRIGGER = bStringHash("WAM_STOP_TRIGGER");
static unsigned int WAM_FIRST_FRAME = bStringHash("WAM_FIRST_FRAME");
static unsigned int WAM_LAST_FRAME = bStringHash("WAM_LAST_FRAME");
static unsigned int WAM_SOUND_TRIGGER_START = bStringHash("WAM_SOUND_TRIGGER_START");
static unsigned int WAM_SOUND_TRIGGER_STOP = bStringHash("WAM_SOUND_TRIGGER_STOP");
static unsigned int WAM_NIS_GENERIC_CONTROL_MSG = bStringHash("WAM_NIS_GENERIC_CONTROL_MSG");
static unsigned int WAM_FWD_REV_TRACK_CONTROL_MSG = bStringHash("WAM_FWD_REV_TRACK_CONTROL_MSG");

CAnimChooser::CAnimChooser() {}

CAnimChooser::~CAnimChooser() {}
