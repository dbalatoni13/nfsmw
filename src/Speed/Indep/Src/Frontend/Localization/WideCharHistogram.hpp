#ifndef FRONTEND_LOCALIZATION_WIDECHARHISTOGRAM_H
#define FRONTEND_LOCALIZATION_WIDECHARHISTOGRAM_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

class WideCharHistogram {
  public:
    WideCharHistogram();
    void PlatEndianSwap();
    bool PackString(char *string, int string_size, const uint16 *wide_string);
    bool UnpackString(uint16 *wide_string, int wide_string_size, const char *string);
    void AddEntry(uint16 wide_char);

  protected:
    int32 NumEntries;
    uint16 EntryTable[3072];
};

class WideCharHistogramBuilder : WideCharHistogram {
  public:
    WideCharHistogramBuilder() {};
    void AddEntry(uint16 wide_char);
    void BuildTable();

  private:
    int32 TotalFrequency;
    int32 FrequencyTable[65536];
};

extern WideCharHistogram *pWideCharHistogram;

#endif
