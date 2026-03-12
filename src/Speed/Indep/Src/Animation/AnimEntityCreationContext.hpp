#ifndef _CANIMENTITYCREATIONCONTEXT
#define _CANIMENTITYCREATIONCONTEXT

class CAnimEntityCreationContext {
  private:
    static bool mIsRaceStart;

  public:
    static void SetRaceStartContext(bool enable) { mIsRaceStart = enable; }
    static bool IsRaceStartContext() { return mIsRaceStart; }
};

#endif // _CANIMENTITYCREATIONCONTEXT
