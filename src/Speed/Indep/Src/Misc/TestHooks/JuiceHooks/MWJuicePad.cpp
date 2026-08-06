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

void MWJuicePad::Initialize() {
    int segment = 0;
    do {
        int button = 0;
        do {
            mPadState[segment].buttonState[button] = false;
            mTrackSegments[segment].buttonState[button] = false;
            button++;
        } while (button < 26);
        segment++;
    } while (segment < 2);

    FEActionMapping[0] = FRONTENDACTION_UP;
    BEActionMapping[6] = GAMEACTION_SHIFTUP;
    BEActionMapping[7] = GAMEACTION_SHIFTDOWN;
    BEActionMapping[10] = CAMERAACTION_LOOKBACK;
    FEActionMapping[1] = FRONTENDACTION_DOWN;
    FEActionMapping[2] = FRONTENDACTION_LEFT;
    FEActionMapping[3] = FRONTENDACTION_RIGHT;
    FEActionMapping[4] = FRONTENDACTION_ACCEPT;
    FEActionMapping[5] = FRONTENDACTION_CANCEL;
    BEActionMapping[11] = GAMEACTION_GAMEBREAKER;
    FEActionMapping[6] = FRONTENDACTION_BUTTON5;
    FEActionMapping[7] = FRONTENDACTION_BUTTON4;
    FEActionMapping[8] = FRONTENDACTION_LTRIGGER;
    FEActionMapping[9] = FRONTENDACTION_RTRIGGER;
    FEActionMapping[10] = FRONTENDACTION_BUTTON0;
    FEActionMapping[11] = FRONTENDACTION_BUTTON1;
    FEActionMapping[12] = FRONTENDACTION_BUTTON2;
    FEActionMapping[13] = FRONTENDACTION_BUTTON3;
    FEActionMapping[14] = FRONTENDACTION_START;
    FEActionMapping[15] = FRONTENDACTION_CANCEL_ALT;
    BEActionMapping[0] = HUDACTION_ENGAGE_EVENT;
    BEActionMapping[1] = HUDACTION_PAD_DOWN;
    BEActionMapping[2] = HUDACTION_PAD_LEFT;
    BEActionMapping[3] = HUDACTION_PAD_RIGHT;
    BEActionMapping[4] = GAMEACTION_HANDBRAKE;
    BEActionMapping[5] = GAMEACTION_NOS;
    mButtonPressedLastFrame = false;
    FEActionMapping[16] = NULL_ACTION;
    FEActionMapping[17] = NULL_ACTION;
    FEActionMapping[18] = NULL_ACTION;
    FEActionMapping[19] = NULL_ACTION;
    FEActionMapping[20] = NULL_ACTION;
    FEActionMapping[21] = NULL_ACTION;
    FEActionMapping[22] = NULL_ACTION;
    FEActionMapping[23] = NULL_ACTION;
    FEActionMapping[24] = NULL_ACTION;
    FEActionMapping[25] = NULL_ACTION;
    BEActionMapping[25] = GAMEACTION_BRAKE;
    BEActionMapping[14] = HUDACTION_PAUSEREQUEST;
    BEActionMapping[15] = GAMEACTION_RESET;
    BEActionMapping[16] = CAMERAACTION_CHANGE;
    BEActionMapping[18] = GAMEACTION_STEERLEFT;
    BEActionMapping[19] = GAMEACTION_STEERRIGHT;
    BEActionMapping[22] = GAMEACTION_TURNLEFT;
    BEActionMapping[23] = GAMEACTION_TURNRIGHT;
    BEActionMapping[24] = GAMEACTION_GAS;
    BEActionMapping[8] = GAMEACTION_BRAKE;
    BEActionMapping[9] = GAMEACTION_GAS;
    BEActionMapping[12] = NULL_ACTION;
    BEActionMapping[13] = NULL_ACTION;
    BEActionMapping[17] = NULL_ACTION;
    BEActionMapping[20] = NULL_ACTION;
    BEActionMapping[21] = NULL_ACTION;
}

void MWJuicePad::PressButton(int port, MWJuicePadButtonType buttonType) {
    mButtonPressedLastFrame = true;
    mPadState[port].buttonState[buttonType] = true;
    if (cFEng::Get()->FindPackageWithControl() != nullptr) {
        SimulateFEButton(port, FEActionMapping[buttonType], 1.0f);
        return;
    }
    if (gMoviePlayer != nullptr) {
        SimulateFEButton(port, FEActionMapping[buttonType], 1.0f);
        return;
    }
    ActionID buttonID = BEActionMapping[buttonType];
    if (INIS::Exists() && buttonType == JUICE_ACCEPT) {
        buttonID = HUDACTION_SKIPNIS;
    }
    SimulateBEButton(port, buttonID, 1.0f);
}

void MWJuicePad::ReleaseAllButtons(MWJPadReleaseType type) {
    if (mButtonPressedLastFrame || type == JUICE_FORCED) {
        mButtonPressedLastFrame = false;
        int port = 0;
        do {
            int nextPort = port + 1;
            MWJuicePadState *state = mPadState + port;
            ActionID *action = BEActionMapping;
            int button = 0x19;
            do {
                if (state->buttonState[0] == true) {
                    state->buttonState[0] = false;
                    goto simulate;
                } else if (type == JUICE_FORCED) {
                    state->buttonState[0] = false;
                    goto simulate;
                }
                goto advance;
            simulate:
                if (cFEng::Get()->FindPackageWithControl() != nullptr) {
                    SimulateFEButton(port, action[-0x1a], 0.0f);
                } else {
                    SimulateBEButton(port, *action, 0.0f);
                }
            advance:
                action++;
                button--;
                state = reinterpret_cast<MWJuicePadState *>(state->buttonState + 4);
            } while (button >= 0);
            port = nextPort;
        } while (port < 2);
    }
}

void MWJuicePad::PollInput() {
    if (cFEng::Get()->FindPackageWithControl() != nullptr && mInputQueue != nullptr) {
        while (!mInputQueue->IsEmpty()) {
            ActionRef action = mInputQueue->GetAction();
            if (action.Data() > 0.0f && mInputQueue->IsConnected()) {
                switch (action.ID()) {
                case 0x19:
                    Juice::JuicePad::Instance()->SetButton(255.0f, 1);
                    break;
                case 0x1a:
                    Juice::JuicePad::Instance()->SetButton(255.0f, 2);
                    break;
                case 0x1b:
                    Juice::JuicePad::Instance()->SetButton(255.0f, 3);
                    break;
                case 0x1c:
                    Juice::JuicePad::Instance()->SetButton(255.0f, 4);
                    break;
                case 0x21:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 5);
                    break;
                case 0x22:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 8);
                    break;
                case 0x24:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 15);
                    break;
                case 0x25:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 16);
                    break;
                case 0x26:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 13);
                    break;
                case 0x27:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 11);
                    break;
                case 0x28:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 14);
                    break;
                case 0x29:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 7);
                    break;
                case 0x2a:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 6);
                    break;
                case 0x2b:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 9);
                    break;
                case 0x2c:
                    Juice::JuicePad::Instance()->SetButton(action.Data(), 12);
                    break;
                }
            }
            mInputQueue->PopAction();
        }
    }
}

void MWJuicePad::ResetGamePad() {
    mResetSegmentPresses = true;
}

void MWJuicePad::ReleaseSegmentPresses() {
    if (mResetSegmentPresses) {
        mResetSegmentPresses = false;
        for (int port = 0; port < 2; port++) {
            MWJuicePadState *segment = mTrackSegments + port;
            ActionID *action = BEActionMapping;
            int button = 0x19;
            bool *buttonState = segment->buttonState;
            do {
                if (*buttonState == true) {
                    *buttonState = false;
                    if (cFEng::Get()->FindPackageWithControl() != nullptr) {
                        SimulateFEButton(port, action[-0x1a], 0.0f);
                    } else {
                        SimulateBEButton(port, *action, 0.0f);
                    }
                }
                buttonState++;
                action++;
                button--;
            } while (button >= 0);
        }
    }
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

void MWJuicePad::TrackButtonPress(int actionId, float data, int port) {
    if (cFEng::Get()->FindPackageWithControl() != nullptr) {
        if (mIsInBE) {
            ResetTrackedPresses();
            mIsInBE = false;
        }
        TrackFEPresses(actionId, data, port);
        return;
    }
    if (!mIsInBE) {
        ResetTrackedPresses();
        mIsInBE = true;
    }
    TrackBEPresses(actionId, data, port);
}
