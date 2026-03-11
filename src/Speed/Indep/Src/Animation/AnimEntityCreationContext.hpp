#ifndef _CANIMENTITYCREATIONCONTEXT
#define _CANIMENTITYCREATIONCONTEXT

struct CAnimEntityCreationContext {
    static int mIsRaceStart;

    static void SetRaceStartContext(bool enable) { mIsRaceStart = enable; }
};

#endif // _CANIMENTITYCREATIONCONTEXT
