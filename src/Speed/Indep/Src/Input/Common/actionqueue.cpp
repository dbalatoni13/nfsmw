#include "Speed/Indep/Src/Input/ActionQueue.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/controller.h"
#include "Speed/Indep/Src/Input/Action.h"
#include "Speed/Indep/Src/Input/ActionData.h"
#include "Speed/Indep/Src/Input/ActionRef.h"
#include "Speed/Indep/Src/Input/IOModule.h"
#include "Speed/Indep/Src/Input/InputDefParser.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Main/AttribSupport.h"
#include "Speed/Indep/Src/Misc/Joylog.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"
#include "Speed/Indep/Tools/AttribSys/Runtime/Common/AttribPrivate.h"

static const int PrintActionQueue = 0;

bool ActionQueue::sInJoylogFrame;
Timer ActionQueue::mLastAnyActionTime;

ActionQueue::List ActionQueue::_mTable;

// const ActionRef ActionQueue::operator[](int i) {

// }

bool ActionQueue::IsConnected() const {
    if (this->mPort < 0) {
        return false;
    }
    return this->mConnected;
}

void ActionQueue::IO_SetConnected(bool plugged) {
    if (this->mPort < 0) {
        return;
    }
    if (this->mConnected == plugged) {
        return;
    }
    if (plugged) {
        ActionData a(ACTION_PLUGGED, 0.0f, this->mPort);

        this->ReceiveAction(a);
    } else {
        ActionData a(ACTION_UNPLUGGED, 0.0f, this->mPort);

        this->ReceiveAction(a);
    }
}

void ActionQueue::IO_Flush() {
    if (this->mPort < 0) {
        return;
    }

    ActionData a(ACTION_FLUSH, 0.0f, this->mPort);

    this->ReceiveAction(a);
}

void ActionQueue::Init(int port, unsigned int config) {
    if (port == this->mPort && config == this->mConfig) {
        return;
    }
    this->mConfig = config;
    this->mPort = port;

    if (this->mMappings != nullptr) {
        delete this->mMappings;
        this->mMappings = nullptr;
    }

    if (port >= 0) {
        InputDevice *device = IOModule::GetIOModule().GetDevice(port);
        if (device != nullptr) {
            const Attrib::Collection *spec = Attrib::FindCollection(Attrib::Gen::controller::ClassKey(), this->mConfig);
            if (spec != nullptr) {
                this->mMappings = new InputMapping(device, spec);
            }
        }
    }

    this->OnActivationChange();
}

void ActionQueue::SetPort(int port) {
    this->Init(port, this->mConfig);
}

void ActionQueue::SetConfig(unsigned int config, const char *queue_name) {
    this->mQueueName = queue_name;
    this->Init(this->mPort, config);
}

bool ActionQueue::IsEnabled() const {
    return (this->mState < AQS_ENABLED) ? false : true;
}

void ActionQueue::Enable(bool b) {
    if (b) {
        if (this->mState == AQS_DISABLED) {
            this->mState = AQS_ENABLED;
            this->Flush();
        }
    } else {
        if (this->mState != AQS_DISABLED) {
            this->mState = AQS_DISABLED;
            this->Flush();
        }
    }

    this->OnActivationChange();
}

// unfinished
void ActionQueue::FetchCurrentValues(InputDevice *device) {
    _STL::list<InputMapEntry> &e = this->mMappings->GetEntries();

    for (_STL::list<InputMapEntry>::iterator iter = e.begin(); iter != e.end(); iter++) {
        if (iter->DeviceScalarIndex != 0) {
            InputMapEntry &entry = *iter;
            DeviceScalar *button = device->GetDeviceScalar(0);

            // UMath::Ramp(button->GetValue(), button->GetValue(), button->GetValue());
        }
    }
}

ActionQueue *ActionQueue::FindActionQueue(int id) {
    for (ActionQueue *const *iter = _mTable.begin(); iter != _mTable.end(); iter++) {
        ActionQueue *q = *iter;

        if (q->mUniqueID == id) {
            return q;
        }
    }

    return nullptr;
}

// ActionQueue::FindActionQueue(unsigned int config, int port)

int ActionQueue::AssignUniqueID() {
    int id = 0;
    while (ActionQueue::FindActionQueue(id) != nullptr) {
        id += 1;
    }

    return id;
}

// unfinished
void ActionQueue::IO_UpdateFromDevice() {
    if (this->mMappings == nullptr || this->mState == AQS_DISABLED || this->mPort < 0) return;

    InputDevice *device = IOModule::GetIOModule().GetDevice(this->mPort);
    ActionData ad = ActionData(0, 0.0f, 0);

    this->FetchCurrentValues(device);

    _STL::list<InputMapEntry> &e = this->mMappings->GetEntries();
    for (_STL::list<InputMapEntry>::iterator iaction = e.begin(); iaction != e.end(); iaction++) {
        InputMapEntry &entry = *iaction;
        if (entry.HasChanged()) {
            iaction++;
            continue;
        }
    }
}

void ActionQueue::OnActivationChange() {
    bool is_active;
    bool was_active = false;

    if (this->IsEnabled() && this->IsConnected()) {
        is_active = this->IsActive();
        if (is_active) {
            was_active = true;
        }
    }
    if (was_active != (this->mActivationTime.IsSet() != 0)) {
        if (was_active) {
            this->mActivationTime = RealTimer;
        } else {
            this->mActivationTime.ResetLow();
        }
    }
}

bool ActionQueue::IsEmpty() {
    return this->fQueue.size() == 0;
}

// unfinished
bool ActionQueue::ReceiveAction(ActionData &action) {
    if (action.Slot() != this->mPort) {
        return false;
    }
    if (action.ID() != ACTION_UNPLUGGED) {
        this->mLastAnyActionTime = RealTimer;
    }
    Joylog::AddData(this->mUniqueID, 8, JOYLOG_CHANNEL_JOYEVENTS);
    Joylog::AddData(action.ID(), 8, JOYLOG_CHANNEL_JOYEVENTS);
    Joylog::AddData(action.Data(), JOYLOG_CHANNEL_JOYEVENTS);

    if (action.ID() == ACTION_UNPLUGGED) {
        this->Flush();
        this->mConnected = false;
        this->OnActivationChange();
    } else if (action.ID() == ACTION_PLUGGED) {
        this->Flush();
        this->mConnected = true;
        this->OnActivationChange();
    } else if (action.ID() == ACTION_FLUSH) {
        this->Flush();
    }
    this->mActionTime = RealTimer;

    this->fQueue.size(); // dwarf nonsense
    this->fQueue.enqueue(action);

    return true;
}

void ActionQueue::PopAction() {
    if (this->fQueue.size() > 0) {
        this->fQueue.dequeue();
    }
}

void ActionQueue::Flush() {
    this->fQueue.reset();

    if (this->mMappings != nullptr) {
        _STL::list<InputMapEntry> &e = this->mMappings->GetEntries();
        for (_STL::list<InputMapEntry>::iterator iter = e.begin(); iter != e.end(); iter++) {
            InputMapEntry &entry = *iter;
            entry.PreviousValue = -1.0;
            entry.CurrentValue = -1.0;
        }
    }
}

const ActionRef ActionQueue::GetAction() {
    if (this->fQueue.size() > 0) {
        ActionData *a = &this->fQueue.tail();
        return ActionRef(a);
    } else {
        return ActionRef(nullptr);
    }
}

void ActionQueue::BeginJoylogFrame() {
    sInJoylogFrame = true;
    if (Joylog::IsReplaying()) {
        while (true) {
            int queue_id = Joylog::GetSignedData(0x8, JOYLOG_CHANNEL_JOYEVENTS);
            if (queue_id == -1) return;

            ActionQueue *q = ActionQueue::FindActionQueue(queue_id);
            int action_id = Joylog::GetData(0x8, JOYLOG_CHANNEL_JOYEVENTS);
            float data = Joylog::GetData(JOYLOG_CHANNEL_JOYEVENTS);
            ActionData a = ActionData(action_id, data, q->mPort);
            q->ReceiveAction(a);
        }
    }
}

void ActionQueue::EndJoylogFrame() {
    if (!Joylog::IsReplaying()) {
        Joylog::AddData(-1, 0x8, JOYLOG_CHANNEL_JOYEVENTS);
    }
    sInJoylogFrame = false;
}
