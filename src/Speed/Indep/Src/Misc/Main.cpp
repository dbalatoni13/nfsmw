// OUTSIDE ZMISC //

extern int ExitTheGameFlag;
extern int frames_elapsed;
extern int loop_ticker;

float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks);          /* extern */
extern unsigned int bGetTicker();
////

// IN ZMISC //
extern void InitializeEverything(int argc, char *argv[]);

extern void WriteFreekerBaseAddressBeacon();

extern void MainLoop(float hardware_ms);

class RaceStarter {
public:
    static void StartSkipFERace();
};
extern int SkipFE;

class GameFlowManager {
public:
    void LoadFrontend();
};
extern struct GameFlowManager TheGameFlowManager;

class Timer {};
class Scheduler {
public:
    void Synchronize(Timer to);
};
extern Timer RealTimer;
extern Scheduler * fgScheduler;

extern bool twkDumpProfileMarks;
////

int main(int argc, char *argv[]) {
    unsigned int current_tick;
    float milliseconds;
    float temp_f30;
    float temp_f31;

    InitializeEverything(argc, argv);
    WriteFreekerBaseAddressBeacon();
    if (SkipFE != 0) {
        RaceStarter::StartSkipFERace();
    } else {
        TheGameFlowManager.LoadFrontend();
    }
    frames_elapsed = 1;
    loop_ticker = bGetTicker();
    fgScheduler->Synchronize(RealTimer);
    if ((int) ExitTheGameFlag == 0) {
        temp_f30 = 0.0f;
        temp_f31 = 0.25f;
        while (ExitTheGameFlag == 0) {
            current_tick = bGetTicker();
            milliseconds = bGetTickerDifference(loop_ticker, current_tick);
            if ((current_tick != (int) loop_ticker) && !(milliseconds > temp_f30)) {
                loop_ticker = current_tick;
            } else {
                if (!(milliseconds <= temp_f31)) {
                    const float minimum_time_step = 32000.0f;
                    loop_ticker = current_tick;
                    if (!(milliseconds <= minimum_time_step)) {
                        milliseconds = minimum_time_step;
                    }
                    MainLoop(milliseconds);
                    if (twkDumpProfileMarks != 0) {
                        twkDumpProfileMarks = 0;
                    }
                }
            }
        }
    }
    return 0;
}
