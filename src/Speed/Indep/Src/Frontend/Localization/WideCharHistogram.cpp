#include "Speed/Indep/Src/Frontend/Localization/WideCharHistogram.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

extern int DisableWideStringHistogram;

WideCharHistogram *pWideCharHistogram;

void WideCharHistogram::PlatEndianSwap() {
    bPlatEndianSwap(&NumEntries);
    for (int n = 0; n < NumEntries; n++) {
        bPlatEndianSwap(&EntryTable[n]);
    }
}

// UNSOLVED
bool WideCharHistogram::PackString(char *packed, int size, const uint16 *wide) {
    bool error = false;
    int string_pos = 0;
    int wide_string_pos = 0;
    uint16 ch;
    if (size > 0) {
        do {
            ch = wide[wide_string_pos];
            wide_string_pos++;
            if (ch > 0xFF7F) {
                ch = ch + 0x100;
            }
            if (ch < 0x80) {
                packed[string_pos] = static_cast<char>(ch);
                string_pos++;
            } else if (DisableWideStringHistogram == 0) {
                int numEntries = NumEntries;
                int idx = 0x80;
                if (numEntries > 0x80) {
                    uint16 entry = EntryTable[0x80];
                    while (entry != ch && (idx++, idx < numEntries)) {
                        entry = EntryTable[idx];
                    }
                }
                if (idx == numEntries) {
                    error = 1;
                } else if (idx < 0x100) {
                    packed[string_pos] = static_cast<char>(idx);
                    string_pos++;
                } else {
                    int j = 0x80;
                    do {
                        uint16 entry = EntryTable[j];
                        int idxVal = idx;
                        if (idx < 0) {
                            idxVal = idx + 0x7f;
                        }
                        if (static_cast<unsigned int>(entry) == static_cast<unsigned int>(idxVal >> 7)) {
                            packed[string_pos] = static_cast<char>(j);
                            packed[string_pos + 1] = static_cast<char>(idx) + static_cast<char>(entry) * -0x80 + -0x80;
                            string_pos += 2;
                            break;
                        }
                        j++;
                    } while (j < 0x100);
                    if (j == 0x100) {
                        error = 1;
                    }
                }
            } else if (ch < 0x100) {
                packed[string_pos] = static_cast<char>(ch);
                string_pos++;
            }
        } while (ch != 0 && string_pos < size);
    }
}

// UNSOLVED
bool WideCharHistogram::UnpackString(uint16 *wide, int size, const char *packed) {
    bool error = false;
    int out = 0;
    if (size > 0) {
        bool histEnabled = DisableWideStringHistogram == 0;
        int in = 0;
        unsigned int result;
        int nextIn;
        do {
            unsigned int ch = static_cast<unsigned int>(static_cast<unsigned char>(packed[in]));
            nextIn = in + 1;
            result = ch;
            if ((static_cast<unsigned char>(packed[in]) & 0x80) != 0 && histEnabled) {
                unsigned int lookup = static_cast<unsigned int>(EntryTable[ch]);
                result = lookup;
                if (lookup < 0x80 && lookup != 0) {
                    unsigned char nextByte = static_cast<unsigned char>(packed[nextIn]);
                    nextIn = in + 2;
                    result = 0;
                    if ((nextByte & 0x80) != 0) {
                        result = static_cast<unsigned int>(EntryTable[lookup * 0x80 + static_cast<unsigned int>(nextByte) - 0x80]);
                    }
                }
                if (result == 0) {
                    error = 1;
                    result = 0x5f;
                }
            }
            wide[out] = static_cast<uint16>(result);
            out++;
        } while (result != 0 && (in = nextIn, out < size));
    }
}
