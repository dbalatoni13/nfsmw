#ifndef ONLINE_FLOATQUANTIZER_HPP
#define ONLINE_FLOATQUANTIZER_HPP

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>
#include <float.h>

class FloatQuantizer {
  public:
    FloatQuantizer();
    ~FloatQuantizer();

    void Init(const char *name, float min_value, float max_value, float max_acceptable_error);
    void Init(const char *name, float min_value, float max_value, int num_bits);
    int GetNumBits() const { return m_numBits; }
    float GetMinValue() const { return m_minValue; }
    float GetMaxValue() const { return m_maxValue; }
    float GetMaxError() const { return m_maxError; }
    uint32 Pack(float value);
    float UnPack(uint32 packed_value);
    float PackAndUnPackTest(float value);
    char *GetName() { return m_name; }
    void PrintUsageReport();

  private:
    /* 0x000 */ char m_name[300];
    /* 0x12c */ int m_numBits;
    /* 0x130 */ float m_minValue;
    /* 0x134 */ float m_maxValue;
    /* 0x138 */ float m_maxError;
    /* 0x13c */ float m_increment;
    /* 0x140 */ float m_minValuePacked;
    /* 0x144 */ float m_maxValuePacked;
    /* 0x148 */ bool m_isInitialized;
};

#endif
