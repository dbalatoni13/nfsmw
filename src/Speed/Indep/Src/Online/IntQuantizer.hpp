#ifndef ONLINE_INTQUANTIZER_HPP
#define ONLINE_INTQUANTIZER_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class IntQuantizer {
  public:
    IntQuantizer();
    ~IntQuantizer();

    void Init(const char *name, int min_value, int max_value);
    int GetNumBits() const { return m_numBits; }
    int GetMinValue() const { return m_minValue; }
    int GetMaxValue() const { return m_maxValue; }
    uint32 Pack(int value);
    int UnPack(uint32 packed_value);
    int PackAndUnPackTest(int value);
    char *GetName() { return m_name; }

  private:
    /* 0x000 */ char m_name[300];
    /* 0x12c */ int m_numBits;
    /* 0x130 */ int m_minValue;
    /* 0x134 */ int m_maxValue;
    /* 0x138 */ int m_minValuePacked;
    /* 0x13c */ int m_maxValuePacked;
    /* 0x140 */ bool m_isInitialized;
};

#endif
