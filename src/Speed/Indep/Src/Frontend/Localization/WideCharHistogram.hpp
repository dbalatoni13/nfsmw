#ifndef WIDE_CHAR_HISTOGRAM_HPP
#define WIDE_CHAR_HISTOGRAM_HPP

#include <types.h>

#define MAX_WIDE_CHAR_HISTOGRAM_ENTRIES 3072 // :9

class WideCharHistogram {
  public:
    WideCharHistogram();
    void PlatEndianSwap();
    bool PackString(char *string, int string_size, const uint16 *wide_string);
    bool UnpackString(uint16 *wide_string, int wide_string_size, const char *string);
    void AddEntry(uint16 wide_char);

  protected:
    int32 NumEntries;
    uint16 EntryTable[MAX_WIDE_CHAR_HISTOGRAM_ENTRIES];
};

// File: speed/indep/src/frontend/localization/WideCharHistogram.hpp
// total size: 0x41808
// Decl: speed/indep/src/frontend/localization/WideCharHistogram.hpp:42
class WideCharHistogramBuilder : public WideCharHistogram {
  public:
    WideCharHistogramBuilder() {} // Decl: speed/indep/src/frontend/localization/WideCharHistogram.hpp:44

    void AddEntry(uint16 wide_char); // Decl: speed/indep/src/frontend/localization/WideCharHistogram.hpp:46

    void BuildTable(); // Decl: speed/indep/src/frontend/localization/WideCharHistogram.hpp:48

  private:
    int32 TotalFrequency;        // offset 0x1804, size 0x4, Decl: speed/indep/src/frontend/localization/WideCharHistogram.hpp:57
    int32 FrequencyTable[65536]; // offset 0x1808, size 0x40000, Decl: speed/indep/src/frontend/localization/WideCharHistogram.hpp:58
};

extern WideCharHistogram *pWideCharHistogram;

#endif
