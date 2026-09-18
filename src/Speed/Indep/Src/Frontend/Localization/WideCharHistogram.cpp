#include "Speed/Indep/Src/Frontend/Localization/WideCharHistogram.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

WideCharHistogram *pWideCharHistogram = nullptr;

int DisableWideStringHistogram = 0;


void WideCharHistogram::PlatEndianSwap() {
    bPlatEndianSwap(&NumEntries);
    for (int i = 0; i < NumEntries; i++) {
        bPlatEndianSwap(&EntryTable[i]);
    }
}

bool WideCharHistogram::PackString(char *string, int string_size, const uint16 *wide_string) {
    bool error = false;
    int string_pos = 0;
    int wide_string_pos = 0;

    if (string_pos < string_size) {
        do {
            uint16 wide_char = wide_string[wide_string_pos];
            wide_string_pos++;

            if (wide_char > 0xFF7F) {
                wide_char = wide_char + 0x100;
            }

            if (wide_char < 0x80) {
                string[string_pos] = static_cast<char>(wide_char);
                string_pos++;
            } else if (DisableWideStringHistogram) {
                if (wide_char < 0x100) {
                    string[string_pos] = static_cast<char>(wide_char);
                    string_pos++;
                }
            } else {
                int histogram_index = 0x80;

                if (histogram_index < NumEntries && EntryTable[histogram_index] != wide_char) {
                    do {
                        histogram_index++;
                    } while (histogram_index < NumEntries && EntryTable[histogram_index] != wide_char);
                }

                if (histogram_index == NumEntries) {
                    error = true;
                } else if (histogram_index < 0x100) {
                    string[string_pos] = static_cast<char>(histogram_index);
                    string_pos++;
                } else {
                    int index128 = 0x80;

                    do {
                        uint16 entry = EntryTable[index128];
                        if (entry == histogram_index / 0x80) {
                            string[string_pos++] = static_cast<char>(index128);
                            // el segundo byte va por puntero: el objetivo materializa la direccion
                            // (add+stb) en vez de usar stbx, y sin casts a char (no hay extsb)
                            char *dest = &string[string_pos++];
                            *dest = histogram_index - entry * 0x80 - 0x80;
                            break;
                        }
                        index128++;
                    } while (index128 < 0x100);

                    if (index128 == 0x100) {
                        error = true;
                    }
                }
            }

            if (wide_char == 0) {
                break;
            }
        } while (string_pos < string_size);
    }

    return !error;
}

bool WideCharHistogram::UnpackString(uint16 *wide_string, int wide_string_size, const char *string) {
    bool error = false;
    int wide_string_pos = 0;
    int string_pos = 0;

    if (wide_string_pos < wide_string_size) {
        do {
            uint16 wide_char = static_cast<unsigned char>(string[string_pos]);
            string_pos++;

            if ((wide_char & 0x80) != 0 && DisableWideStringHistogram == 0) {
                uint16 index = EntryTable[wide_char];

                if (index > 0x7f) {
                    wide_char = index;
                } else if (index != 0) {
                    unsigned char c2 = string[string_pos];
                    string_pos++;

                    if ((c2 & 0x80) == 0) {
                        wide_char = 0;
                    } else {
                        wide_char = EntryTable[index * 0x80 + c2 - 0x80];
                    }
                }

                if (wide_char == 0) {
                    error = true;
                    wide_char = 0x5f;
                }
            }

            wide_string[wide_string_pos] = wide_char;
            wide_string_pos++;

            if (wide_char == 0) {
                break;
            }
        } while (wide_string_pos < wide_string_size);
    }

    return !error;
}
