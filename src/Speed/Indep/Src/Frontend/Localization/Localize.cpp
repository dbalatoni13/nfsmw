#include "Speed/Indep/Src/Frontend/Localization/Localize.hpp"
#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/Frontend/FEngInterfaces/FEngInterface.hpp"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"
#include "Speed/Indep/Src/Misc/BuildRegion.hpp"
#include "Speed/Indep/Src/Frontend/Localization/WideCharHistogram.hpp"
#include "Speed/GameCube/Src/G.hpp"


struct FontNameInfo {
    unsigned int GlobalFonts[8];   // offset 0x0
    unsigned int InGameFonts[8];   // offset 0x20
    unsigned int FrontendFonts[8]; // offset 0x40
    int GlobalFontsLoaded;         // offset 0x60
    int InGameFontsLoaded;         // offset 0x64
    int FrontendFontsLoaded;       // offset 0x68
};
struct LanguageChunkHeader {
    int HistogramTablePos;    // offset 0x0, size 0x4
    int NumStringRecords;     // offset 0x4, size 0x4
    int StringRecordTablePos; // offset 0x8, size 0x4
    int StringTablePos;       // offset 0xC, size 0x4

    void PlatEndianSwap() {
        bPlatEndianSwap(&HistogramTablePos);
        bPlatEndianSwap(&NumStringRecords);
        bPlatEndianSwap(&StringRecordTablePos);
        bPlatEndianSwap(&StringTablePos);
    }
};

struct LanguageInfo {
    eLanguages Language;                   // offset 0x0, size 0x4
    char *Name;                            // offset 0x4, size 0x4
    char *Filename;                        // offset 0x8, size 0x4
    char *FilenameTextOnly;                // offset 0xC, size 0x4
    FontNameInfo *pFontNameInfo;           // offset 0x10, size 0x4
    bPrintfLocaleInfo *pbPrintfLocaleInfo; // offset 0x14, size 0x4
};


struct StringRecord {
    uint32 Hash;         // offset 0x0, size 0x4
    uint8 *PackedString; // offset 0x4, size 0x4
};


struct FontSizeInfo {
    unsigned int Hash; // offset 0x0
    int Size;          // offset 0x4
};

int LanguageMemoryPoolNumber = (int)0xFFFFFFFF;
void *pLanguageMemoryPoolMemory = nullptr;
int LanguageMemoryPoolSize = 0;

FontSizeInfo FontSizeInfoTable[9] = {
    {0x545570C6U, 0x0},
    {0x0920075CU, 0x10000},
    {0x5B9D84DBU, 0x10000},
    {0x9583AA1AU, 0x40000},
    {0xA87927BEU, 0x10000},
    {0xAB6215C1U, 0x10000},
    {0xBBBA71C2U, 0x4000},
    {0xDCA5485AU, 0x40000},
    {0x833A8678U, 0x80000},
};

FontNameInfo EuropeanFontNameInfo = {
    {0x545570C6U, 0xA87927BEU, 0x5B9D84DBU, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
    {0x01424892U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
    {0xAB6215C1U, 0x0920075CU, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U, 0x00000000U},
    0,
    0,
    0,
};

bPrintfLocaleInfo EnglishLocaleInfo = {'.', ',', 3};
bPrintfLocaleInfo FrenchLocaleInfo = {',', ' ', 3};
bPrintfLocaleInfo GermanLocaleInfo = {',', '.', 3};
bPrintfLocaleInfo ItalianLocaleInfo = {',', '.', 3};
bPrintfLocaleInfo SpanishLocaleInfo = {',', ' ', 0};
bPrintfLocaleInfo DutchLocaleInfo = {',', '.', 3};
bPrintfLocaleInfo SwedishLocaleInfo = {',', ' ', 3};
bPrintfLocaleInfo PolishLocaleInfo = {',', ' ', 3};
bPrintfLocaleInfo FinnishLocaleInfo = {',', ' ', 3};
bPrintfLocaleInfo DanishLocaleInfo = {',', '.', 3};

LanguageInfo LanguageInfoTable[] = {
    {eLANGUAGE_ENGLISH, "ENGLISH", "LANGUAGES\\ENGLISH.BIN", "LANGUAGES\\ENGLISH_TEXTONLY.BIN", &EuropeanFontNameInfo, &EnglishLocaleInfo},
    {eLANGUAGE_FRENCH, "FRENCH", "LANGUAGES\\FRENCH.BIN", "LANGUAGES\\FRENCH_TEXTONLY.BIN", &EuropeanFontNameInfo, &FrenchLocaleInfo},
    {eLANGUAGE_GERMAN, "GERMAN", "LANGUAGES\\GERMAN.BIN", "LANGUAGES\\GERMAN_TEXTONLY.BIN", &EuropeanFontNameInfo, &GermanLocaleInfo},
    {eLANGUAGE_ITALIAN, "ITALIAN", "LANGUAGES\\ITALIAN.BIN", "LANGUAGES\\ITALIAN_TEXTONLY.BIN", &EuropeanFontNameInfo, &ItalianLocaleInfo},
    {eLANGUAGE_SPANISH, "SPANISH", "LANGUAGES\\SPANISH.BIN", "LANGUAGES\\SPANISH_TEXTONLY.BIN", &EuropeanFontNameInfo, &SpanishLocaleInfo},
    {eLANGUAGE_DUTCH, "DUTCH", "LANGUAGES\\DUTCH.BIN", "LANGUAGES\\DUTCH_TEXTONLY.BIN", &EuropeanFontNameInfo, &DutchLocaleInfo},
    {eLANGUAGE_SWEDISH, "SWEDISH", "LANGUAGES\\SWEDISH.BIN", "LANGUAGES\\SWEDISH_TEXTONLY.BIN", &EuropeanFontNameInfo, &SwedishLocaleInfo},
    {eLANGUAGE_POLISH, "POLISH", "LANGUAGES\\POLISH.BIN", "LANGUAGES\\POLISH_TEXTONLY.BIN", &EuropeanFontNameInfo, &PolishLocaleInfo},
    {eLANGUAGE_FINNISH, "FINNISH", "LANGUAGES\\FINNISH.BIN", "LANGUAGES\\FINNISH_TEXTONLY.BIN", &EuropeanFontNameInfo, &FinnishLocaleInfo},
    {eLANGUAGE_DANISH, "DANISH", "LANGUAGES\\DANISH.BIN", "LANGUAGES\\DANISH_TEXTONLY.BIN", &EuropeanFontNameInfo, &DanishLocaleInfo},
};

struct VMFile;
static eLanguages CurrentLanguage = eLANGUAGE_NONE;
ResourceFile *pLanguageResourceFile = nullptr;
VMFile *pLanguageResourceFile_VM = nullptr;
static unsigned int NumStringRecords = 0;
static unsigned char *PackedStringTable = nullptr;
static StringRecord *RecordTable = nullptr;

extern cFrontendDatabase *FEDatabase;

LanguageInfo *GetLanguageInfo(eLanguages language) {
    for (int i = 0; i <= 9; i++) {
        if (LanguageInfoTable[i].Language == language) {
            return &LanguageInfoTable[i];
        }
    }
    return nullptr;
}

char *GetLanguageName(eLanguages language) {
    LanguageInfo *info = GetLanguageInfo(language);
    if (info) {
        return info->Name;
    }
    return "UNKNOWN";
}

const char *GetLocalizedPercentSign() {
    const char *szPercentUnit = "%";
    eLanguages currLang = GetCurrentLanguage();
    if (currLang == eLANGUAGE_DANISH || currLang == eLANGUAGE_FINNISH || currLang == eLANGUAGE_FRENCH || currLang == eLANGUAGE_GERMAN ||
        currLang == eLANGUAGE_SWEDISH) {
        szPercentUnit = " %";
    }
    return szPercentUnit;
}

void InitLocalization() {
    LanguageInfo *info;
    if (BuildRegion::IsKorea()) {
        info = GetLanguageInfo(eLANGUAGE_KOREAN);
    } else {
        info = GetLanguageInfo(eLANGUAGE_ENGLISH);
    }
    int total_font_size = 0;
    int n = 0;
    while (info->pFontNameInfo->GlobalFonts[n] != 0) {
        int font_size = -1;
        for (int i = 0; i < 9; i++) {
            if (FontSizeInfoTable[i].Hash == info->pFontNameInfo->GlobalFonts[n]) {
                font_size = FontSizeInfoTable[i].Size;
                break;
            }
        }
        if (font_size >= 0) {
            total_font_size = total_font_size + font_size;
        }
        n++;
    }
    LanguageMemoryPoolSize = (total_font_size + 0x4080) & 0xFFFFFFC0;
    if (LanguageMemoryPoolNumber != 0) {
        LanguageMemoryPoolNumber = bGetFreeMemoryPoolNum();
        pLanguageMemoryPoolMemory = bMalloc(LanguageMemoryPoolSize, "LanguageMemoryPool", 0, 0);
        bInitMemoryPool(LanguageMemoryPoolNumber, pLanguageMemoryPoolMemory, LanguageMemoryPoolSize, "LanguageMemoryPool");
    }
    eLoadStreamingTexturePack("LANGUAGES\\LANGUAGETEXTURES.BIN");
    eWaitForStreamingTexturePackLoading("LANGUAGES\\LANGUAGETEXTURES.BIN");
}

void LanguageHasChanged(eLanguages new_language) {
    if (FEDatabase) {
        eLanguages lang = GetCurrentLanguage();
        if (lang != eLANGUAGE_ENGLISH) {
            FEDatabase->GetGameplaySettings()->SpeedoUnits = 1;
        } else {
            FEDatabase->GetGameplaySettings()->SpeedoUnits = 0;
        }
    }
    cFEng::Get()->MakeLoadedPackagesDirty();
}

void LoadLanguageResources(bool load_global, bool load_frontend, bool load_ingame, bool blocking) {
    LanguageInfo *info = GetLanguageInfo(CurrentLanguage);
    if (!load_global) {
        UnloadResourceFile(pLanguageResourceFile);
        pLanguageResourceFile = nullptr;
        UnloadFileFromVirtualMemory(pLanguageResourceFile_VM);
        pLanguageResourceFile_VM = nullptr;
        if (info->pFontNameInfo->GlobalFontsLoaded) {
            eUnloadStreamingTexture(info->pFontNameInfo->GlobalFonts, 8);
            info->pFontNameInfo->GlobalFontsLoaded = 0;
        }
    }
    if (!load_frontend && info->pFontNameInfo->FrontendFontsLoaded) {
        eUnloadStreamingTexture(info->pFontNameInfo->FrontendFonts, 8);
        info->pFontNameInfo->FrontendFontsLoaded = 0;
    }
    if (!load_ingame && info->pFontNameInfo->InGameFontsLoaded) {
        eUnloadStreamingTexture(info->pFontNameInfo->InGameFonts, 8);
        info->pFontNameInfo->InGameFontsLoaded = 0;
    }
    if (load_global) {
        if (!pLanguageResourceFile) {
            pLanguageResourceFile_VM = LoadFileIntoVirtualMemory(info->FilenameTextOnly, false, false);
            {
            } // empty anonymous block (DWARF)
            int pool = 0;
            pLanguageResourceFile = CreateResourceFile(info->Filename, static_cast<ResourceFileType>(7), 0, 0, 0);
            int file_size = bFileSize(info->Filename);
            if (bLargestMalloc(LanguageMemoryPoolNumber) >= file_size + 0x80) {
                pool = LanguageMemoryPoolNumber;
            }
            pLanguageResourceFile->SetAllocationParams((pool & 0xF) | 0x2000, info->Filename);
            pLanguageResourceFile->BeginLoading();
            if (blocking) {
                WaitForResourceLoadingComplete();
            }
        }
        if (load_global && !info->pFontNameInfo->GlobalFontsLoaded) {
            eLoadStreamingTexture(info->pFontNameInfo->GlobalFonts, 8, static_cast<void (*)(void *)>(nullptr), static_cast<void *>(nullptr),
                                  LanguageMemoryPoolNumber);
            info->pFontNameInfo->GlobalFontsLoaded = 1;
        }
    }
    if (load_frontend && !info->pFontNameInfo->FrontendFontsLoaded) {
        eLoadStreamingTexture(info->pFontNameInfo->FrontendFonts, 8, static_cast<void (*)(void *)>(nullptr), static_cast<void *>(nullptr),
                              LanguageMemoryPoolNumber);
        info->pFontNameInfo->FrontendFontsLoaded = 1;
    }
    if (load_ingame && !info->pFontNameInfo->InGameFontsLoaded) {
        eLoadStreamingTexture(info->pFontNameInfo->InGameFonts, 8, static_cast<void (*)(void *)>(nullptr), static_cast<void *>(nullptr),
                              LanguageMemoryPoolNumber);
        info->pFontNameInfo->InGameFontsLoaded = 1;
    }
    if (blocking) {
        eWaitForStreamingTexturePackLoading("LANGUAGES\\LANGUAGETEXTURES.BIN");
    }
}

void SetCurrentLanguage(eLanguages new_language) {
    if (new_language != CurrentLanguage) {
        if (CurrentLanguage != static_cast<eLanguages>(-1)) {
            LoadLanguageResources(false, false, false, false);
        }
        CurrentLanguage = new_language;
        if (new_language != static_cast<eLanguages>(-1)) {
            LoadLanguageResources(true, TheGameFlowManager.IsInFrontend(), TheGameFlowManager.IsInGame(), true);
            LanguageInfo *langInfo = GetLanguageInfo(CurrentLanguage);
            if (langInfo) {
                bPrintfLocaleInfo locInfo = *langInfo->pbPrintfLocaleInfo;
                bPrintfSetLocaleInfo(locInfo.decimal_char, locInfo.group_char, locInfo.group_len);
            }
            LanguageHasChanged(CurrentLanguage);
        }
        if (FEDatabase && !FEDatabase->GetUserProfile(0)->IsProfileNamed()) {
            FEDatabase->GetUserProfile(0)->SetProfileName(nullptr, true);
        }
        MemoryCard::LoadLocale(CurrentLanguage);
    }
}

void LoadCurrentLanguage() {
#ifdef EA_PLATFORM_GAMECUBE
    eLanguages ngc_language = GC_GetOSLanguage();
    SetCurrentLanguage(ngc_language);
#elif EA_PLATFORM_PS2
    if (BuildRegion::IsAmerica()) {
        SetCurrentLanguage(eLANGUAGE_ENGLISH);
    } else {
        eLanguages new_language = PS2_GetOSLanguage();
        SetCurrentLanguage(new_language);
    }
#endif
}

eLanguages GetCurrentLanguage() {
    return CurrentLanguage;
};

void WideToCharString(char *dest, uint32 destlen, const int16 *src) {
    if (dest == nullptr) {
        return;
    }
    if (src == nullptr) {
        return;
    }

    bool error;
    uint32 bytes = 0;

    while (*src != 0 && bytes < destlen - 1) {
        if (*reinterpret_cast<const uint16 *>(src) < 0x100) {
            bytes++;
            *dest = *src;
            src++;
            dest++;
        } else {
            src++;
        }
    }

    *dest = 0;
}

void PackedStringToWideString(uint16 *wide_string, int wide_string_buffer_size, const char *packed_string) {
    if (!pWideCharHistogram) {
        bStrCpy(wide_string, packed_string);
    } else {
        pWideCharHistogram->UnpackString(wide_string, wide_string_buffer_size, packed_string);
    }
}

void WideStringToPackedString(char *packed_string, int packed_string_buffer_size, const unsigned short *wide_string) {
    pWideCharHistogram->PackString(packed_string, packed_string_buffer_size, wide_string);
}

// TODO: why is this here????
FEWideString &FEWideString::operator=(const char *pcString) {
    short wide_string[1024];
    PackedStringToWideString(reinterpret_cast<unsigned short *>(wide_string), 0x800, pcString);
    *this = wide_string;
    return *this;
}

static const uint8 *SearchForString(uint32 string_label) {
    if (!RecordTable) {
        return nullptr;
    }
    unsigned int top;
    unsigned int bot;
    int step;
    bot = 0;
    top = NumStringRecords - 1;
    while (true) {
        unsigned int mid = (bot + top) >> 1;
        if (RecordTable[mid].Hash == string_label) {
            return RecordTable[mid].PackedString;
        }
        if (top - bot < 3) {
            if (RecordTable[bot].Hash == string_label) {
                return RecordTable[bot].PackedString;
            }
            if (RecordTable[top].Hash == string_label) {
                return RecordTable[top].PackedString;
            }
        }
        if (mid == bot) {
            break;
        }
        if (RecordTable[mid].Hash > string_label) {
            top = mid;
        }
        if (RecordTable[mid].Hash < string_label) {
            bot = mid;
        }
    }
    return nullptr;
}

bool DoesStringExist(uint32 label) {
    if (!SearchForString(label)) {
        return false;
    }
    return true;
}

char *GetLocalizedString(uint32 id) {
    const uint8 *str = SearchForString(id);
    if (!str) {
        str = SearchForString(0x9bb9ccc3);
    }
    return reinterpret_cast<char *>(const_cast<uint8 *>(str));
}

void GetLocalizedString(char *buffer, uint32 bufsize, uint32 string_label) {
    char *str = const_cast<char *>(GetLocalizedString(string_label));
    bStrNCpy(buffer, str, static_cast<int>(bufsize));
}

char *GetTranslatedString(int id) {
    return const_cast<char *>(GetLocalizedString(static_cast<unsigned int>(id)));
}

const bool GetLocalizedWideString(int16 *wide_string, int wide_string_buffer_size, uint32 string_label) {
    const uint8 *str = SearchForString(string_label);
    if (str) {
        PackedStringToWideString(reinterpret_cast<uint16 *>(wide_string), wide_string_buffer_size, reinterpret_cast<const char *>(str));
        return true;
    }
    return false;
}

int LoaderLanguage(bChunk *chunk) {
    if (chunk->GetID() == 0x39000) {
        LanguageChunkHeader *header = reinterpret_cast<LanguageChunkHeader *>(chunk->GetData());
        header->PlatEndianSwap();
        NumStringRecords = header->NumStringRecords;
        pWideCharHistogram = reinterpret_cast<WideCharHistogram *>(reinterpret_cast<char *>(header) + header->HistogramTablePos);
        RecordTable = reinterpret_cast<StringRecord *>(reinterpret_cast<char *>(header) + header->StringRecordTablePos);
        PackedStringTable = reinterpret_cast<unsigned char *>(header) + header->StringTablePos;
        pWideCharHistogram->PlatEndianSwap();
        for (unsigned int i = 0; i < NumStringRecords; i++) {
            bPlatEndianSwap(reinterpret_cast<unsigned int *>(&RecordTable[i]));
            unsigned int offset = reinterpret_cast<unsigned int *>(&RecordTable[i])[1];
            bPlatEndianSwap(&offset);
            reinterpret_cast<unsigned int *>(&RecordTable[i])[1] = reinterpret_cast<unsigned int>(PackedStringTable) + offset;
        }
        return 1;
    }
    return 0;
}

int UnloaderLanguage(bChunk *chunk) {
    if (chunk->GetID() == 0x39000) {
        NumStringRecords = 0;
        PackedStringTable = nullptr;
        RecordTable = nullptr;
        pWideCharHistogram = nullptr;
        return 1;
    }
    return 0;
}
