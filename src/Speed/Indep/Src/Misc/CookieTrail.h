#ifndef COOKIETRAIL_H
#define COOKIETRAIL_H

// total size: 0x810
// Decl: 9
template <typename T, int U> class CookieTrail {
  public:
    CookieTrail() : mCount(0), mLast(-1), mCapacity(U) {}

    int Capacity() {
        return this->mCapacity;
    }

    T &Oldest() {
        if (this->mCount < this->mCapacity) {
            return this->mData[0];
        }
        int next = this->mLast + 1;
        return this->mData[next - (next / this->mCapacity) * this->mCapacity];
    }

    const T &Newest() const {
        return this->mData[this->mLast];
    }

    T &Newest() {
        return this->mData[this->mLast];
    }

    T &NthOldest(int n) {
        return this->mData[this->mCount < this->mCapacity ? (n % this->mCount) : ((this->mLast + 1 + n) % this->mCapacity)];
    }

    int Count() const {
        return this->mCount;
    }

    void Clear() {
        this->mCount = 0;
        this->mLast = -1;
    }

    void AddNew(const T &t) {
        this->mLast = (this->mLast + 1) % this->mCapacity;
        if (this->mCount < this->mCapacity) {
            this->mCount++;
        }
        this->mData[this->mLast] = t;
    }

  private:
    int mCount;          // offset 0x0, size 0x4
    int mLast;           // offset 0x4, size 0x4
    const int mCapacity; // offset 0x8, size 0x4
    int pad;             // offset 0xC, size 0x4
    T mData[U];          // offset 0x10, size 0x800
};

#endif
