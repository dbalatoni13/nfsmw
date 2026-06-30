#include "Speed/Indep/Src/Input/InputDefParser.h"
#include "Speed/Indep/Src/Generated/AttribSys/Classes/controller.h"
#include "Speed/Indep/Src/Input/Action.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Misc/MWAttribUserTypes.h"
#include "Speed/Indep/Src/Misc/attribuserinclude.h"
#include "Speed/Indep/Tools/AttribSys/Runtime/AttribSys.h"

InputMapping::InputMapping(InputDevice *device, const Attrib::Collection *spec) {
    if (spec == nullptr) return;

    const char *actionName;
    Attrib::Gen::controller atr(spec, 0, nullptr);

    for (int actionid = 0; actionid < MAX_ACTIONID; actionid++) {
        actionName = getActionIdString((ActionID)actionid);
        if (actionName == nullptr) continue;

        Attrib::Attribute attribute = atr.Get(Attrib::StringToKey(actionName));
        unsigned int numattribs = attribute.GetLength();

        for (int control = 0; control < numattribs; control++) {
            const ControllerDataRecord &button = attribute.Get<ControllerDataRecord>(control);
            const Attrib::StringKey &buttonName = button.mDeviceID;
            int buttonIndex;

            if (buttonName.IsEmpty()) {
                continue;
            }
            buttonIndex = -1;
            for (int j = 0; j < device->GetNumDeviceScalar(); j++) {
                DeviceScalar *scalar = device->GetDeviceScalar(j);
                if (scalar != nullptr) {
                    if (scalar->GetDeviceScalarName().GetValue() == buttonName.GetHash32()) {
                        buttonIndex = j;
                        break;
                    }
                }
            }

            InputMapEntry tableEntry;
            if (buttonIndex != -1) {
                tableEntry.Action = (ActionID)actionid;
                tableEntry.UpdateType = button.mUpdateType;
                tableEntry.LowerDZ = button.mLowerDZ;
                tableEntry.UpperDZ = button.mUpperDZ;
                tableEntry.DeviceScalarIndex = buttonIndex;
                tableEntry.PreviousValue = -1.0f;
                tableEntry.CurrentValue = -1.0f;
                this->mEntries.push_back(tableEntry);
            }
        }
    }
}

InputMapping::~InputMapping() {}
