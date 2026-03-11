#ifndef _CANIMENTITYCREATIONCONTEXT
#define _CANIMENTITYCREATIONCONTEXT

struct CAnimEntityCreationContext {
    static bool mIsRaceStart;

    static void SetRaceStartContext(bool enable) { mIsRaceStart = enable; }
};

#endif // _CANIMENTITYCREATIONCONTEXT
