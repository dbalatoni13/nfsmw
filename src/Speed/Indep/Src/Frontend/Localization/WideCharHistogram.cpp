extern int DisableWideStringHistogram;

void WideCharHistogram::PlatEndianSwap() {
    bEndianSwap32(&NumEntries);
    for (int i = 0; i < NumEntries; i++) {
        bEndianSwap16(&EntryTable[i]);
    }
}

void WideCharHistogram::UnpackString(unsigned short *wide, int size, const char *packed) {
    unsigned int error = 0;
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
            wide[out] = static_cast<unsigned short>(result);
            out++;
        } while (result != 0 && (in = nextIn, out < size));
    }
}

void WideCharHistogram::PackString(char *packed, int size, const unsigned short *wide) {
    unsigned int error = 0;
    int out = 0;
    int in = 0;
    unsigned short ch;
    if (size > 0) {
        do {
            ch = wide[in];
            in++;
            if (ch > 0xFF7F) {
                ch = ch + 0x100;
            }
            if (ch < 0x80) {
                packed[out] = static_cast<char>(ch);
                out++;
            } else if (DisableWideStringHistogram == 0) {
                int numEntries = NumEntries;
                int idx = 0x80;
                if (numEntries > 0x80) {
                    unsigned short entry = EntryTable[0x80];
                    while (entry != ch && (idx++, idx < numEntries)) {
                        entry = EntryTable[idx];
                    }
                }
                if (idx == numEntries) {
                    error = 1;
                } else if (idx < 0x100) {
                    packed[out] = static_cast<char>(idx);
                    out++;
                } else {
                    int j = 0x80;
                    do {
                        unsigned short entry = EntryTable[j];
                        int idxVal = idx;
                        if (idx < 0) {
                            idxVal = idx + 0x7f;
                        }
                        if (static_cast<unsigned int>(entry) == static_cast<unsigned int>(idxVal >> 7)) {
                            packed[out] = static_cast<char>(j);
                            packed[out + 1] = static_cast<char>(idx) + static_cast<char>(entry) * -0x80 + -0x80;
                            out += 2;
                            break;
                        }
                        j++;
                    } while (j < 0x100);
                    if (j == 0x100) {
                        error = 1;
                    }
                }
            } else if (ch < 0x100) {
                packed[out] = static_cast<char>(ch);
                out++;
            }
        } while (ch != 0 && out < size);
    }
}
