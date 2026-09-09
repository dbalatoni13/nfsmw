#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Src/Input/IFeedBack.h"
#include "Speed/Indep/Src/Input/InputDevice.h"
#include "Speed/Indep/Src/Input/SteeringWheelDevice.h"
#include "Speed/Indep/Src/Sim/SimEffect.h"


class GameDevice : public InputDevice, public IFeedback {
private:
    Sim::Effect* mEffect; // offset 0x34, size 0x4
    DeviceScalar fPS2DeviceScalars[37]; // offset 0x38, size 0x250
    float fPS2PrevValues[37]; // offset 0x288, size 0x94
    float fPS2CurrentValues[37]; // offset 0x31C, size 0x94
    int mNumScalars; // offset 0x3B0, size 0x4
    SteeringWheelDevice * mWheelDevice; // offset 0x3B4, size 0x4

public:
    USE_FASTALLOC(GameDevice)

    static struct InputDevice * Construct(int port); // Decl: 197

    bool IsWheel() override; // Decl: 211

    struct UTL::COM::IUnknown * GetInterfaces() override; // Decl: 213

    struct UTL::COM::IUnknown * GetSecondaryDevice() override; // Decl: 214

private:
    static int mCount; // size: 0x4, address: 0x8041E4B0, Decl: 734

public:
        void Initialize() override; // Decl: 735

        bool IsConnected() override; // Decl: 755

        void StartVibration() override; // Decl: 775

        void StopVibration() override; // Decl: 783

        void PollDevice() override; // Decl: 795

        int GetNumDeviceScalar() override; // Decl: 898

        GameDevice(int deviceIndex) ; // Decl: 903

        ~GameDevice() override; // Decl: 921

        void PauseEffects() override; // Decl: 934

        void ResumeEffects() override; // Decl: 949

        void ResetEffects() override; // Decl: 961

        void BeginUpdate() override; // Decl: 980

        void EndUpdate() override; // Decl: 1003

        void UpdateRoadNoise(bool front, const struct SimSurface & surface, float speed) override; // Decl: 1011

        void UpdateTireSkid(bool front, const struct SimSurface & surface, float speed) override; // Decl: 1043

        void UpdateTireSlip(bool front, const struct SimSurface & surface, float speed) override; // Decl: 1077

        void UpdateRPM(float powerband, float overrev, float throttle) override; // Decl: 1107

        void UpdateShiftPotential(enum ShiftPotential potential) override; // Decl: 1116

        void UpdateEngineBlown(bool blown) override; // Decl: 1207

        void UpdateNOS(bool engaged, float NOSLevel) override; // Decl: 1247

        void UpdateShifting(bool shifting) override; // Decl: 1286

        void ReportCollision(const COLLISION_INFO & cinfo, bool iamA) override; // Decl: 1320
};
