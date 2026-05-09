#ifndef FRONTEND_LOCALIZATION_LOCALIZE_H
#define FRONTEND_LOCALIZATION_LOCALIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include <types.h>

enum eLanguages {
    eLANGUAGE_NONE = -1,
    eLANGUAGE_FIRST = 0,
    eLANGUAGE_ENGLISH = 0,
    eLANGUAGE_FRENCH = 1,
    eLANGUAGE_GERMAN = 2,
    eLANGUAGE_ITALIAN = 3,
    eLANGUAGE_SPANISH = 4,
    eLANGUAGE_DUTCH = 5,
    eLANGUAGE_SWEDISH = 6,
    eLANGUAGE_DANISH = 7,
    eLANGUAGE_KOREAN = 8,
    eLANGUAGE_CHINESE = 9,
    eLANGUAGE_JAPANESE = 10,
    eLANGUAGE_THAI = 11,
    eLANGUAGE_POLISH = 12,
    eLANGUAGE_FINNISH = 13,
    eLANGUAGE_LARGEST = 14,
    eLANGUAGE_LABELS = 15,
    eLANGUAGE_MAX = 16,
};

char *GetLocalizedString(uint32 string_label);
void GetLocalizedString(char *buffer, uint32 bufsize, uint32 string_label);

char *GetTranslatedString(int label_hash);
void LoadCurrentLanguage();
void InitLocalization();
eLanguages GetCurrentLanguage();

const bool GetLocalizedWideString(int16 *wide_string, int wide_string_buffer_size, uint32 string_label);

enum eLanguages GetCurrentLanguage();
void WideToCharString(char *dest /* r3 */, unsigned int destlen /* r4 */, const short *src /* r5 */);

// Range: 0x8015C8F0 -> 0x8015C944
void PackedStringToWideString(unsigned short *wide_string /* r0 */, int wide_string_buffer_size /* r9 */, const char *packed_string /* r6 */);

// Range: 0x8015C944 -> 0x8015C980
void WideStringToPackedString(char *packed_string /* r11 */, int packed_string_buffer_size /* r0 */, const unsigned short *wide_string /* r6 */);
char *GetLanguageName(eLanguages language /* r3 */);
const char *GetLocalizedPercentSign();
bool DoesStringExist(uint32 label);

#endif
