#include "Speed/Indep/Src/Misc/TestHooks/JuiceHooks/JuiceHooks.h"

int bStrCmp(const char *a, const char *b);

MWJuicePad *MWJuicePad::mInstance;

MWJuicePad *MWJuicePad::Instance() {
    if (mInstance == nullptr) {
        mInstance = new ("MWJuicePad", 0) MWJuicePad();
    }
    return mInstance;
}

MWJuicePad::MWJuicePad() {
    mInputQueue = new ActionQueue(0, 0x82D21520, "juiceinput", false);
    mInputQueue->Enable(true);
    mIsLastFrame = false;
    mIsInBE = false;
    mResetSegmentPresses = false;
}

void MWJuicePad::ResetGamePad() {
    mResetSegmentPresses = true;
}

void MWJuicePad::SetIsLastFrame(bool val) {
    mIsLastFrame = val;
}

bool MWJuicePad::IsLastJoyFrame() {
    return mIsLastFrame;
}

ActionQueue *MWJuicePad::GetActionQueue(const char *qName, int port) {
    const ActionQueue::List *actionQueues = &ActionQueue::GetList();
    ActionQueue::List::const_iterator iter = actionQueues->begin();
    while (iter != actionQueues->end()) {
        ActionQueue *queue = *iter;
        if (bStrCmp(queue->GetRawName(), qName) == 0 && queue->GetPort() == port) {
            return queue;
        }
        iter++;
    }
    return nullptr;
}

void MWJuicePad::SimulateFEButton(int port, int buttonID, float buttonData) {
    ActionQueue *queue = GetActionQueue("FEngHud", 0);
    if (queue != nullptr) {
        ActionData action(buttonID, buttonData, port);
        queue->ReceiveAction(action);
    }
}

void MWJuicePad::SimulateBEButton(int port, int buttonID, float buttonData) {
    ActionQueue *queue = nullptr;
    if (buttonID >= 0x2D && buttonID < 0x34) {
        queue = GetActionQueue("InputPlayer", 0);
    } else if (buttonID >= 1 && buttonID < 0x14) {
        queue = GetActionQueue("InputPlayer", 0);
    }
    if (queue != nullptr) {
        ActionData action(buttonID, buttonData, port);
        queue->ReceiveAction(action);
    }
}

void MWJuicePad::TrackFEPresses(int actionID, float data, int port) {
    bool press_state = false;
    if (data > 0.0f) {
        press_state = true;
    }
    MWJuicePadButtonType button = JUICE_MAX_BUTTON;
    switch (actionID) {
    case FRONTENDACTION_UP:
        button = JUICE_UP;
        break;
    case FRONTENDACTION_DOWN:
        button = JUICE_DOWN;
        break;
    case FRONTENDACTION_LEFT:
        button = JUICE_LEFT;
        break;
    case FRONTENDACTION_RIGHT:
        button = JUICE_RIGHT;
        break;
    case FRONTENDACTION_ACCEPT:
        button = JUICE_ACCEPT;
        break;
    case FRONTENDACTION_CANCEL:
        button = JUICE_CANCEL;
        break;
    case FRONTENDACTION_BUTTON5:
        button = JUICE_Y;
        break;
    case FRONTENDACTION_BUTTON4:
        button = JUICE_X;
        break;
    case FRONTENDACTION_LTRIGGER:
        button = JUICE_L1;
        break;
    case FRONTENDACTION_RTRIGGER:
        button = JUICE_R1;
        break;
    case FRONTENDACTION_BUTTON0:
        button = JUICE_WHITE;
        break;
    case FRONTENDACTION_BUTTON1:
        button = JUICE_BLACK;
        break;
    case FRONTENDACTION_BUTTON2:
        button = JUICE_LSTICKBUTTON;
        break;
    case FRONTENDACTION_BUTTON3:
        button = JUICE_RSTICKBUTTON;
        break;
    case FRONTENDACTION_START:
        button = JUICE_START;
        break;
    case FRONTENDACTION_CANCEL_ALT:
        button = JUICE_BACK;
        break;
    default:
        break;
    }
    if (button != JUICE_MAX_BUTTON) {
        mTrackSegments[port].buttonState[button] = press_state;
    }
}

void MWJuicePad::TrackBEPresses(int actionID, float data, int port) {
    bool press_state = false;
    if (data > 0.0f) {
        press_state = true;
    }
    MWJuicePadButtonType button = JUICE_MAX_BUTTON;
    switch (actionID) {
    case HUDACTION_ENGAGE_EVENT:
        button = JUICE_UP;
        break;
    case HUDACTION_PAD_DOWN:
        button = JUICE_DOWN;
        break;
    case HUDACTION_PAD_LEFT:
        button = JUICE_LEFT;
        break;
    case HUDACTION_PAD_RIGHT:
        button = JUICE_RIGHT;
        break;
    case GAMEACTION_HANDBRAKE:
        button = JUICE_ACCEPT;
        break;
    case GAMEACTION_NOS:
        button = JUICE_CANCEL;
        break;
    case GAMEACTION_SHIFTUP:
        button = JUICE_Y;
        break;
    case GAMEACTION_SHIFTDOWN:
        button = JUICE_X;
        break;
    case GAMEACTION_BRAKE:
        button = JUICE_L1;
        break;
    case GAMEACTION_GAS:
        button = JUICE_R1;
        break;
    case CAMERAACTION_LOOKBACK:
        button = JUICE_WHITE;
        break;
    case GAMEACTION_GAMEBREAKER:
        button = JUICE_BLACK;
        break;
    case HUDACTION_PAUSEREQUEST:
        button = JUICE_START;
        break;
    case GAMEACTION_RESET:
        button = JUICE_BACK;
        break;
    case CAMERAACTION_CHANGE:
        button = JUICE_R3;
        break;
    case GAMEACTION_STEERLEFT:
        button = JUICE_LEFTANALOG_LEFT;
        break;
    case GAMEACTION_STEERRIGHT:
        button = JUICE_LEFTANALOG_RIGHT;
        break;
    case GAMEACTION_TURNLEFT:
        button = JUICE_RIGHTANALOG_LEFT;
        break;
    case GAMEACTION_TURNRIGHT:
        button = JUICE_RIGHTANALOG_RIGHT;
        break;
    default:
        break;
    }
    if (button != JUICE_MAX_BUTTON) {
        mTrackSegments[port].buttonState[button] = press_state;
    }
}

void MWJuicePad::ResetTrackedPresses() {
    for (int port = 0; port < 2; port++) {
        for (int button = 0; button < JUICE_MAX_BUTTON; button++) {
            mTrackSegments[port].buttonState[button] = false;
        }
    }
}
