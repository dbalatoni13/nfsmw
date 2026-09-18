#ifndef LOCALIZE_HPP
#define LOCALIZE_HPP

#include <types.h>

#define LONGEST_LANGUAGE_STRING 1024    // :91
#define STRING_CATEGORY_GLOBAL "Global" // :94
#define STRING_CATEGORY_ONLINE "Online" // :95
#define STRING_CATEGORY_DEMO "Demo"     // :96
#define MAX_STRING_CATEGORY_LENGTH 16   // :132

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
void WideToCharString(char *dest, uint32 destlen, const int16 *src);

// Range: 0x8015C8F0 -> 0x8015C944
void PackedStringToWideString(uint16 *wide_string, int wide_string_buffer_size, const char *packed_string);

// Range: 0x8015C944 -> 0x8015C980
void WideStringToPackedString(char *packed_string, int packed_string_buffer_size, const uint16 *wide_string);
char *GetLanguageName(eLanguages language);
const char *GetLocalizedPercentSign();
bool DoesStringExist(uint32 label);

#endif
