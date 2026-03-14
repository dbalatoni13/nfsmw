#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/FEng/FEWideString.h"

extern void GC_GetOSLanguage();
extern void SetCurrentLanguage(eLanguages lang);
extern const char *SearchForString(unsigned int hash);
extern const char *GetLocalizedString(unsigned int id);

struct FontNameInfo;
struct bPrintfLocaleInfo;
struct WideCharHistogram {
    void PackString(char *packed, int size, const unsigned short *wide);
    void UnpackString(unsigned short *wide, int size, const char *packed);
};

extern WideCharHistogram *pWideCharHistogram;
extern void bStrCpy(unsigned short *dst, const char *src);

struct LanguageInfo {
    eLanguages Language;                 // offset 0x0, size 0x4
    char *Name;                          // offset 0x4, size 0x4
    char *Filename;                      // offset 0x8, size 0x4
    char *FilenameTextOnly;              // offset 0xC, size 0x4
    FontNameInfo *pFontNameInfo;         // offset 0x10, size 0x4
    bPrintfLocaleInfo *pbPrintfLocaleInfo; // offset 0x14, size 0x4
};

extern LanguageInfo LanguageInfoTable[];

LanguageInfo *GetLanguageInfo(eLanguages language) {
    for (int i = 0; i <= 9; i++) {
        if (LanguageInfoTable[i].Language == language) {
            return &LanguageInfoTable[i];
        }
    }
    return nullptr;
}

const char *GetLanguageName(eLanguages language) {
    LanguageInfo *info = GetLanguageInfo(language);
    if (!info) {
        return "UNKNOWN";
    }
    return info->Name;
}

static eLanguages CurrentLanguage;

void LoadCurrentLanguage() {
    GC_GetOSLanguage();
    SetCurrentLanguage(CurrentLanguage);
}

eLanguages GetCurrentLanguage() {
    return CurrentLanguage;
}

struct WideCharHistogram;
extern WideCharHistogram *pWideCharHistogram;

extern void bStrCpy(unsigned short *dst, const char *src);

void PackedStringToWideString(unsigned short *wide_string, int wide_string_buffer_size, const char *packed_string) {
    if (!pWideCharHistogram) {
        bStrCpy(wide_string, packed_string);
    } else {
        pWideCharHistogram->UnpackString(wide_string, wide_string_buffer_size, packed_string);
    }
}

void WideStringToPackedString(char *packed_string, int packed_string_buffer_size, const unsigned short *wide_string) {
    pWideCharHistogram->PackString(packed_string, packed_string_buffer_size, wide_string);
}

FEWideString &FEWideString::operator=(const char *pcString) {
    short wide_string[1024];
    PackedStringToWideString(reinterpret_cast<unsigned short *>(wide_string), 0x800, pcString);
    *this = wide_string;
    return *this;
}

bool DoesStringExist(unsigned int label) {
    if (!SearchForString(label)) {
        return false;
    }
    return true;
}

const char *GetLocalizedString(unsigned int id) {
    const char *str = SearchForString(id);
    if (!str) {
        str = SearchForString(0x9bb9ccc3);
    }
    return str;
}

char *GetTranslatedString(int id) {
    return const_cast<char *>(GetLocalizedString(static_cast<unsigned int>(id)));
}

void FormatMessage(char *buf, int size, const char *fmt, __va_list_tag *args) {
    bVSPrintf(buf, fmt, reinterpret_cast<va_list *>(args));
}