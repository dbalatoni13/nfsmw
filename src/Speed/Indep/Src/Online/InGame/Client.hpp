#ifndef ONLINE_INGAME_CLIENT_HPP
#define ONLINE_INGAME_CLIENT_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct ByteCounter {
    ByteCounter(int LengthOfSecond)
        : m_lengthOfSecond(LengthOfSecond) //
        , m_time(0) //
        , m_bytes(0) //
        , m_bps(0) {}
    ~ByteCounter() {}

    void Increment(int now, int bytes) {
        int delta = now - m_time;
        RollCounter(delta);
        m_bytes += bytes;
    }

    int GetBytesPerSecond(int now) {
        int delta = now - m_time;
        RollCounter(delta);
        return m_bps;
    }

  private:
    void RollCounter(int delta) {
        if (delta >= m_lengthOfSecond) {
            m_bps = m_bytes * m_lengthOfSecond / delta;
            m_time += delta;
            m_bytes = 0;
        }
    }

    int m_lengthOfSecond;
    int m_time;
    int m_bytes;
    int m_bps;
};

#endif
