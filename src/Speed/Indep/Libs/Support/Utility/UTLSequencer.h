#ifndef UTLSEQUENCER_H
#define UTLSEQUENCER_H

namespace UTL {

// total size: 0x4
template <typename T> class RepeatSequencer {
  public:
    RepeatSequencer(const T &t) : mT(t) {}

    const T &operator()() {
        return mT;
    }

  private:
    const T &mT; // offset 0x0, size 0x4
};

}; // namespace UTL

#endif
