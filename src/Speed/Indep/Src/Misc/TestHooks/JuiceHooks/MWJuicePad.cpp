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
