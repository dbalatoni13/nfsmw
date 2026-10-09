#ifndef __SPEECHFLOW_H
#define __SPEECHFLOW_H 1 // Decl: 15

class EAXCop;

namespace Speech {

// total size: 0x10
// Decl: 29
class SpeechFlow {
  public:
    SpeechFlow();
    virtual ~SpeechFlow();

    virtual void Update() = 0;                 // Decl: 34
    virtual bool IsTransitionable() = 0;       // Decl: 35
    virtual void ChangeStateTo(int new_state); // TODO not present in A124?
    // Decl: 37
    virtual int GetState() {
        return this->mState;
    }
    // Decl: 38
    virtual void Reset() {
        this->ChangeStateTo(-1);
        this->mBusy = 0;
    }
    // Decl: 39
    virtual bool IsBusy() {
        return this->mBusy != 0;
    }

    virtual void OnCopAdded(EAXCop *cop);
    virtual void OnCopRemoved(EAXCop *cop);

    int mState;     // offset 0x0, size 0x4, Decl: 43
    int mLastState; // offset 0x4, size 0x4, Decl: 44
    int mBusy;      // offset 0x8, size 0x4, Decl: 45

  private:
    SpeechFlow(const SpeechFlow &);
    const SpeechFlow &operator=(const SpeechFlow &);
};

}; // namespace Speech

#endif
