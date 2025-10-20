// OUTSIDE ZMISC //

#include "Speed/Indep/Src/Misc/Timer.hpp"
extern int ExitTheGameFlag;
extern int frames_elapsed;
extern int loop_ticker;

float bGetTickerDifference(unsigned int start_ticks, unsigned int end_ticks); /* extern */
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

class Scheduler {
  public:
    void Synchronize(Timer to);
    static Scheduler *fgScheduler;
};
Timer RealTimer;

extern bool twkDumpProfileMarks;
////

#ifndef EA_PLATFORM_XENON
int main(int argc, char** argv)
#else
int MainThreadFunction(int argc, char** argv)
#endif
{
    #ifdef EA_PLATFORM_WINDOWS
    // PC version wants there to only be one instance at a time
    if (CheckProcessCount("speed.exe", 1)) // this function name is complete bs that I made up for now -Toru
        _exit(0);
    #endif
    
    InitializeEverything(argc, argv);
    
    WriteFreekerBaseAddressBeacon();
    
    if (SkipFE)
        RaceStarter::StartSkipFERace();
    else
        TheGameFlowManager.LoadFrontend();
    
    frames_elapsed = 1;
    
    loop_ticker = bGetTicker();
    Scheduler::fgScheduler->Synchronize(RealTimer);
    
    while ( !ExitTheGameFlag )
    {
        const float minumum_time_step = 0.25f;
        
        unsigned int current_tick = bGetTicker();
        float milliseconds = bGetTickerDifference(loop_ticker, current_tick);
        int milliseconds_fix;
        
        if (current_tick != loop_ticker && milliseconds <= 0.0f)
        {
            loop_ticker = current_tick;
        }
        else if ( milliseconds > minumum_time_step )
        {
            loop_ticker = current_tick;
            if ( milliseconds > 32000.0f )
                milliseconds = 32000.0f;
            
            milliseconds_fix = (int)(milliseconds * 65536.0f); // this is used on other platforms
            
            MainLoop(milliseconds);
            
            if ( twkDumpProfileMarks )
                twkDumpProfileMarks = false;
        }
    }
    
    return 0;
}
