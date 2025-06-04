// OUTSIDE ZMISC
class RaceStarter {
public:
    static void StartSkipFERace();
};

// IN ZMISC
extern void InitializeEverything(int argc, char *argv[]);
extern void WriteFreekerBaseAddressBeacon();
extern int SkipFE;
class GameFlowManager {
public:
    void LoadFrontend();
};
extern struct GameFlowManager TheGameFlowManager;

int main(int argc, char *argv[]) {
    const float minimum_time_step = 0.0f;
    unsigned int current_tick;
    float milliseconds;
    int milliseconds_fix;

    InitializeEverything(argc, argv);
    WriteFreekerBaseAddressBeacon();
    if (SkipFE != 0) {
        RaceStarter::StartSkipFERace();
    } else {
        TheGameFlowManager.LoadFrontend();
    }
}
