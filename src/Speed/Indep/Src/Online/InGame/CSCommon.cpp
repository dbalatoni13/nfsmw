#include "CSCommon.hpp"
#include "Speed/Indep/Src/Misc/Timer.hpp"

extern int DoScreenPrintf;
extern int ShowMiscOnlineDiagnostics;
extern volatile float PreviousCpuFrameRate;
extern volatile float PreviousGpuFrameRate;
void ScreenPrintf(int x, int y, const char *format, ...);

void CSCommon::DumpBytes(const char *raw_data, int num_bytes) {
    for (int i = 0; i < num_bytes; i++) {
    }
}

void CSCommon::CarOverlayDiagnostics() {}

void CSCommon::ShowDiagnostics() {
    if (DoScreenPrintf && ShowMiscOnlineDiagnostics) {
        ScreenPrintf(-100, 120, "WorldTimeSeconds: %f", WorldTimeSeconds);
        int fps = static_cast<int>((PreviousCpuFrameRate < PreviousGpuFrameRate
                                        ? PreviousCpuFrameRate
                                        : PreviousGpuFrameRate) +
                                   0.5f);
        ScreenPrintf(280, -235, "%d", fps);
    }
    CarOverlayDiagnostics();
}
