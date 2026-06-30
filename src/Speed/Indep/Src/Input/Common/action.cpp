#include "Speed/Indep/Src/Input/Action.h"
#include "Speed/Indep/Libs/Support/Utility/UStringToNumber.h"
#include "Speed/Indep/Src/Input/ActionData.h"
#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Misc/Timer.hpp"

// enum name to string
StringToNumberEntry GameActionToStringTable[] = {
    { NULL_ACTION, const_cast<char *>("NULL_ACTION") },
    { GAMEACTION_STEERLEFT, const_cast<char *>("GAMEACTION_STEERLEFT") },
    { GAMEACTION_STEERRIGHT, const_cast<char *>("GAMEACTION_STEERRIGHT") },
    { GAMEACTION_TURNLEFT, const_cast<char *>("GAMEACTION_TURNLEFT") },
    { GAMEACTION_TURNRIGHT, const_cast<char *>("GAMEACTION_TURNRIGHT") },
    { GAMEACTION_GAS, const_cast<char *>("GAMEACTION_GAS") },
    { GAMEACTION_BRAKE, const_cast<char *>("GAMEACTION_BRAKE") },
    { GAMEACTION_HANDBRAKE, const_cast<char *>("GAMEACTION_HANDBRAKE") },
    { GAMEACTION_GAMEBREAKER, const_cast<char *>("GAMEACTION_GAMEBREAKER") },
    { GAMEACTION_SHIFTUP, const_cast<char *>("GAMEACTION_SHIFTUP") },
    { GAMEACTION_SHIFTDOWN, const_cast<char *>("GAMEACTION_SHIFTDOWN") },
    { GAMEACTION_NOS, const_cast<char *>("GAMEACTION_NOS") },
    { GAMEACTION_RESET, const_cast<char *>("GAMEACTION_RESET") },
    { GAMEACTION_FORWARD, const_cast<char *>("GAMEACTION_FORWARD") },
    { GAMEACTION_BACK, const_cast<char *>("GAMEACTION_BACK") },
    { GAMEACTION_JUMP, const_cast<char *>("GAMEACTION_JUMP") },
    { GAMEACTION_DEBUGHUMAN1, const_cast<char *>("GAMEACTION_DEBUGHUMAN1") },
    { GAMEACTION_DEBUGHUMAN2, const_cast<char *>("GAMEACTION_DEBUGHUMAN2") },
    { GAMEACTION_DEBUGHUMAN3, const_cast<char *>("GAMEACTION_DEBUGHUMAN3") },
    { GAMEACTION_DEBUGHUMAN4, const_cast<char *>("GAMEACTION_DEBUGHUMAN4") },
    { CAMERAACTION_CHANGE, const_cast<char *>("CAMERAACTION_CHANGE") },
    { CAMERAACTION_DEBUG, const_cast<char *>("CAMERAACTION_DEBUG") },
    { CAMERAACTION_ENABLE_ICE, const_cast<char *>("CAMERAACTION_ENABLE_ICE") },
    { CAMERAACTION_LOOKBACK, const_cast<char *>("CAMERAACTION_LOOKBACK") },
    { CAMERAACTION_PULLBACK, const_cast<char *>("CAMERAACTION_PULLBACK") },
    { FRONTENDACTION_UP, const_cast<char *>("FRONTENDACTION_UP") },
    { FRONTENDACTION_DOWN, const_cast<char *>("FRONTENDACTION_DOWN") },
    { FRONTENDACTION_LEFT, const_cast<char *>("FRONTENDACTION_LEFT") },
    { FRONTENDACTION_RIGHT, const_cast<char *>("FRONTENDACTION_RIGHT") },
    { FRONTENDACTION_RUP, const_cast<char *>("FRONTENDACTION_RUP") },
    { FRONTENDACTION_RDOWN, const_cast<char *>("FRONTENDACTION_RDOWN") },
    { FRONTENDACTION_RLEFT, const_cast<char *>("FRONTENDACTION_RLEFT") },
    { FRONTENDACTION_RRIGHT, const_cast<char *>("FRONTENDACTION_RRIGHT") },
    { FRONTENDACTION_ACCEPT, const_cast<char *>("FRONTENDACTION_ACCEPT") },
    { FRONTENDACTION_CANCEL, const_cast<char *>("FRONTENDACTION_CANCEL") },
    { FRONTENDACTION_CANCEL_ALT, const_cast<char *>("FRONTENDACTION_CANCEL_ALT") },
    { FRONTENDACTION_START, const_cast<char *>("FRONTENDACTION_START") },
    { FRONTENDACTION_BUTTON0, const_cast<char *>("FRONTENDACTION_BUTTON0") },
    { FRONTENDACTION_BUTTON1, const_cast<char *>("FRONTENDACTION_BUTTON1") },
    { FRONTENDACTION_BUTTON2, const_cast<char *>("FRONTENDACTION_BUTTON2") },
    { FRONTENDACTION_BUTTON3, const_cast<char *>("FRONTENDACTION_BUTTON3") },
    { FRONTENDACTION_BUTTON4, const_cast<char *>("FRONTENDACTION_BUTTON4") },
    { FRONTENDACTION_BUTTON5, const_cast<char *>("FRONTENDACTION_BUTTON5") },
    { FRONTENDACTION_LTRIGGER, const_cast<char *>("FRONTENDACTION_LTRIGGER") },
    { FRONTENDACTION_RTRIGGER, const_cast<char *>("FRONTENDACTION_RTRIGGER") },
    { HUDACTION_PAUSEREQUEST, const_cast<char *>("HUDACTION_PAUSEREQUEST") },
    { HUDACTION_ENGAGE_EVENT, const_cast<char *>("HUDACTION_ENGAGE_EVENT") },
    { HUDACTION_PAD_LEFT, const_cast<char *>("HUDACTION_PAD_LEFT") },
    { HUDACTION_PAD_DOWN, const_cast<char *>("HUDACTION_PAD_DOWN") },
    { HUDACTION_PAD_RIGHT, const_cast<char *>("HUDACTION_PAD_RIGHT") },
    { HUDACTION_SKIPNIS, const_cast<char *>("HUDACTION_SKIPNIS") },
    { HUDACTION_NEXTSONG, const_cast<char *>("HUDACTION_NEXTSONG") },
    { VOIPACTION_PUSHTOTALK, const_cast<char *>("VOIPACTION_PUSHTOTALK") },
    { DEBUGACTION_MOVE_UP, const_cast<char *>("DEBUGACTION_MOVE_UP") },
    { DEBUGACTION_MOVE_DOWN, const_cast<char *>("DEBUGACTION_MOVE_DOWN") },
    { DEBUGACTION_MOVE_LEFT, const_cast<char *>("DEBUGACTION_MOVE_LEFT") },
    { DEBUGACTION_MOVE_RIGHT, const_cast<char *>("DEBUGACTION_MOVE_RIGHT") },
    { DEBUGACTION_MOVE_FORWARD, const_cast<char *>("DEBUGACTION_MOVE_FORWARD") },
    { DEBUGACTION_MOVE_BACK, const_cast<char *>("DEBUGACTION_MOVE_BACK") },
    { DEBUGACTION_MOVE_D_LEFT, const_cast<char *>("DEBUGACTION_MOVE_D_LEFT") },
    { DEBUGACTION_MOVE_D_RIGHT, const_cast<char *>("DEBUGACTION_MOVE_D_RIGHT") },
    { DEBUGACTION_MOVE_D_FORWARD, const_cast<char *>("DEBUGACTION_MOVE_D_FORWARD") },
    { DEBUGACTION_MOVE_D_BACK, const_cast<char *>("DEBUGACTION_MOVE_D_BACK") },
    { DEBUGACTION_LOOK_UP, const_cast<char *>("DEBUGACTION_LOOK_UP") },
    { DEBUGACTION_LOOK_DOWN, const_cast<char *>("DEBUGACTION_LOOK_DOWN") },
    { DEBUGACTION_LOOK_LEFT, const_cast<char *>("DEBUGACTION_LOOK_LEFT") },
    { DEBUGACTION_LOOK_RIGHT, const_cast<char *>("DEBUGACTION_LOOK_RIGHT") },
    { DEBUGACTION_LOOK_D_UP, const_cast<char *>("DEBUGACTION_LOOK_D_UP") },
    { DEBUGACTION_LOOK_D_DOWN, const_cast<char *>("DEBUGACTION_LOOK_D_DOWN") },
    { DEBUGACTION_LOOK_D_LEFT, const_cast<char *>("DEBUGACTION_LOOK_D_LEFT") },
    { DEBUGACTION_LOOK_D_RIGHT, const_cast<char *>("DEBUGACTION_LOOK_D_RIGHT") },
    { DEBUGACTION_TURBO, const_cast<char *>("DEBUGACTION_TURBO") },
    { DEBUGACTION_SUPER_TURBO, const_cast<char *>("DEBUGACTION_SUPER_TURBO") },
    { DEBUGACTION_DROPCAR, const_cast<char *>("DEBUGACTION_DROPCAR") },
    { DEBUGACTION_TOGGLEAI, const_cast<char *>("DEBUGACTION_TOGGLEAI") },
    { DEBUGACTION_TOGGLESIMSTEP, const_cast<char *>("DEBUGACTION_TOGGLESIMSTEP") },
    { DEBUGACTION_SIMSTEP, const_cast<char *>("DEBUGACTION_SIMSTEP") },
    { DEBUGACTION_SCREENSHOT, const_cast<char *>("DEBUGACTION_SCREENSHOT") },
    { DEBUGACTION_STOPRECORDPATH, const_cast<char *>("DEBUGACTION_STOPRECORDPATH") },
    { DEBUGACTION_TOGGLECARCOLOUR, const_cast<char *>("DEBUGACTION_TOGGLECARCOLOUR") },
    { DEBUGACTION_TOGGLEDEMOCAMERAS, const_cast<char *>("DEBUGACTION_TOGGLEDEMOCAMERAS") },
    { ICEACTION_GRAB_LEFT, const_cast<char *>("ICEACTION_GRAB_LEFT") },
    { ICEACTION_GRAB_RIGHT, const_cast<char *>("ICEACTION_GRAB_RIGHT") },
    { ICEACTION_LEFT, const_cast<char *>("ICEACTION_LEFT") },
    { ICEACTION_RIGHT, const_cast<char *>("ICEACTION_RIGHT") },
    { ICEACTION_FAST_LEFT, const_cast<char *>("ICEACTION_FAST_LEFT") },
    { ICEACTION_FAST_RIGHT, const_cast<char *>("ICEACTION_FAST_RIGHT") },
    { ICEACTION_UP, const_cast<char *>("ICEACTION_UP") },
    { ICEACTION_DOWN, const_cast<char *>("ICEACTION_DOWN") },
    { ICEACTION_HELP, const_cast<char *>("ICEACTION_HELP") },
    { ICEACTION_UNDO, const_cast<char *>("ICEACTION_UNDO") },
    { ICEACTION_PLAY, const_cast<char *>("ICEACTION_PLAY") },
    { ICEACTION_HIDE, const_cast<char *>("ICEACTION_HIDE") },
    { ICEACTION_SCREENSHOT, const_cast<char *>("ICEACTION_SCREENSHOT") },
    { ICEACTION_SCREENSHOT_STREAM, const_cast<char *>("ICEACTION_SCREENSHOT_STREAM") },
    { ICEACTION_INSERT, const_cast<char *>("ICEACTION_INSERT") },
    { ICEACTION_CUT, const_cast<char *>("ICEACTION_CUT") },
    { ICEACTION_COPY, const_cast<char *>("ICEACTION_COPY") },
    { ICEACTION_PASTE, const_cast<char *>("ICEACTION_PASTE") },
    { ICEACTION_SELECT, const_cast<char *>("ICEACTION_SELECT") },
    { ICEACTION_CANCEL, const_cast<char *>("ICEACTION_CANCEL") },
    { ICEACTION_ALT_1, const_cast<char *>("ICEACTION_ALT_1") },
    { ICEACTION_CLIP_IN, const_cast<char *>("ICEACTION_CLIP_IN") },
    { ICEACTION_CLIP_OUT, const_cast<char *>("ICEACTION_CLIP_OUT") },
    { ICEACTION_SIMSPEED_INC, const_cast<char *>("ICEACTION_SIMSPEED_INC") },
    { ICEACTION_SIMSPEED_DEC, const_cast<char *>("ICEACTION_SIMSPEED_DEC") },
    { ICEACTION_DUTCH_LEFT, const_cast<char *>("ICEACTION_DUTCH_LEFT") },
    { ICEACTION_DUTCH_RIGHT, const_cast<char *>("ICEACTION_DUTCH_RIGHT") },
    { ICEACTION_ZOOM_IN, const_cast<char *>("ICEACTION_ZOOM_IN") },
    { ICEACTION_ZOOM_OUT, const_cast<char *>("ICEACTION_ZOOM_OUT") },
    { ICEACTION_LETTERBOX_IN, const_cast<char *>("ICEACTION_LETTERBOX_IN") },
    { ICEACTION_LETTERBOX_OUT, const_cast<char *>("ICEACTION_LETTERBOX_OUT") },
    { ICEACTION_BUBBLE_IN, const_cast<char *>("ICEACTION_BUBBLE_IN") },
    { ICEACTION_BUBBLE_OUT, const_cast<char *>("ICEACTION_BUBBLE_OUT") },
    { ICEACTION_BUBBLE_UP, const_cast<char *>("ICEACTION_BUBBLE_UP") },
    { ICEACTION_BUBBLE_DOWN, const_cast<char *>("ICEACTION_BUBBLE_DOWN") },
    { ICEACTION_BUBBLE_TILT_UP, const_cast<char *>("ICEACTION_BUBBLE_TILT_UP") },
    { ICEACTION_BUBBLE_TILT_DOWN, const_cast<char *>("ICEACTION_BUBBLE_TILT_DOWN") },
    { ICEACTION_BUBBLE_SPIN_LEFT, const_cast<char *>("ICEACTION_BUBBLE_SPIN_LEFT") },
    { ICEACTION_BUBBLE_SPIN_RIGHT, const_cast<char *>("ICEACTION_BUBBLE_SPIN_RIGHT") },
    { ICEACTION_BUBBLE_LEFT, const_cast<char *>("ICEACTION_BUBBLE_LEFT") },
    { ICEACTION_BUBBLE_RIGHT, const_cast<char *>("ICEACTION_BUBBLE_RIGHT") },
    { ICEACTION_BUBBLE_BACK, const_cast<char *>("ICEACTION_BUBBLE_BACK") },
    { ICEACTION_BUBBLE_FORTH, const_cast<char *>("ICEACTION_BUBBLE_FORTH") },
    { ICEACTION_SHAKE_MAG_DEC, const_cast<char *>("ICEACTION_SHAKE_MAG_DEC") },
    { ICEACTION_SHAKE_MAG_INC, const_cast<char *>("ICEACTION_SHAKE_MAG_INC") },
    { ICEACTION_SHAKE_FRQ_DEC, const_cast<char *>("ICEACTION_SHAKE_FRQ_DEC") },
    { ICEACTION_SHAKE_FRQ_INC, const_cast<char *>("ICEACTION_SHAKE_FRQ_INC") },
    { ICEACTION_SHUTTLE_LEFT, const_cast<char *>("ICEACTION_SHUTTLE_LEFT") },
    { ICEACTION_SHUTTLE_RIGHT, const_cast<char *>("ICEACTION_SHUTTLE_RIGHT") },
    { ICEACTION_SHUTTLE_UP, const_cast<char *>("ICEACTION_SHUTTLE_UP") },
    { ICEACTION_SHUTTLE_DOWN, const_cast<char *>("ICEACTION_SHUTTLE_DOWN") },
    { ICEACTION_SHUTTLE_SPEED_DEC, const_cast<char *>("ICEACTION_SHUTTLE_SPEED_DEC") },
    { ICEACTION_SHUTTLE_SPEED_INC, const_cast<char *>("ICEACTION_SHUTTLE_SPEED_INC") },
    { ACTION_EXITAPPLICATION, const_cast<char *>("ACTION_EXITAPPLICATION") },
    { ACTION_PLUGGED, const_cast<char *>("ACTION_PLUGGED") },
    { ACTION_UNPLUGGED, const_cast<char *>("ACTION_UNPLUGGED") },
    { ACTION_FLUSH, const_cast<char *>("ACTION_FLUSH") },
    { MAX_ACTIONID, const_cast<char *>("MAX_ACTIONID") },
    { 0, nullptr }
};

ActionRef::ActionRef(ActionData *p) {
    this->actiondata = p;
}

StringToNumber GameActionConverter(GameActionToStringTable);

ActionData::ActionData(int id, float data, int slot) {
    this->id = id;
    this->data = data;
    this->slot = slot;
}

char *getActionIdString(ActionID actionID) {
    char *ais = GameActionConverter.ConvertNumberToString(actionID);

    return ais;
}

ActionQueue::ActionQueue(bool required) {
    this->mRequired = required;
    this->mPort = -1;
    this->mMappings = nullptr;
    this->mConfig = 0;
    this->mActionTime = Timer();
    this->mActivationTime = Timer();
    this->mState = AQS_ENABLED;
    this->mUniqueID = this->AssignUniqueID();
    this->mQueueName = "Unnamed";
    this->mConnected = false;

    this->fQueue.reset();
}

ActionQueue::ActionQueue(int port, unsigned int config, const char *queue_name, bool required) {
    this->mRequired = required;
    this->mPort = -1;
    this->mMappings = nullptr;
    this->mConfig = 0;
    this->mActionTime = Timer();
    this->mActivationTime = Timer();
    this->mState = AQS_ENABLED;
    this->mUniqueID = this->AssignUniqueID();
    this->mConnected = false;

    this->fQueue.reset();
    this->mPort = port;

    this->SetConfig(config, queue_name);
}

ActionQueue::~ActionQueue() {
    if (this->mMappings != nullptr) {
        delete this->mMappings;
    }
}
