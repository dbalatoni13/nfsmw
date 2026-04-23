#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/Src/Frontend/Database/FEDatabase.hpp"
#include "Speed/Indep/Src/EAXSound/EAXSOund.hpp"
#include "Speed/Indep/Src/FEng/cFEng.h"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"
#include "Speed/Indep/Src/FEng/FEWideString.h"
#include "Speed/Indep/bWare/Inc/Strings.hpp"
#include "Speed/Indep/bWare/Inc/bChunk.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"
#include "Speed/Indep/Src/Misc/GameFlow.hpp"

extern void GC_GetOSLanguage();
extern const char *GetLocalizedString(unsigned int id);
extern void bPrintfSetLocaleInfo(char decimal, char group, char group_len);
extern void LoadLanguageResources(bool load_global, bool load_frontend, bool load_ingame, bool blocking);

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

struct WideCharHistogram {
    void PackString(char *packed, int size, const unsigned short *wide);
    void UnpackString(unsigned short *wide, int size, const char *packed);
    void PlatEndianSwap();

  protected:
    int NumEntries;                       // offset 0x0, size 0x4
    unsigned short EntryTable[3072];      // offset 0x4, size 0x1800
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

struct StringRecord {
    unsigned int Hash;            // offset 0x0, size 0x4
    unsigned char *PackedString;  // offset 0x4, size 0x4
};

static unsigned int NumStringRecords;
static unsigned char *PackedStringTable;
static StringRecord *RecordTable;

extern cFrontendDatabase *FEDatabase;
eLanguages GetCurrentLanguage();

void LanguageHasChanged(eLanguages new_language) {
    EAXSound::ChangeLanguage(new_language);
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

void SetCurrentLanguage(eLanguages new_language);

void LoadCurrentLanguage() {
    GC_GetOSLanguage();
    SetCurrentLanguage(CurrentLanguage);
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
                bPrintfSetLocaleInfo(locInfo.decimal_char, locInfo.group_char, langInfo->pbPrintfLocaleInfo->group_len);
            }
            LanguageHasChanged(CurrentLanguage);
        }
        if (FEDatabase && !FEDatabase->GetUserProfile(0)->IsProfileNamed()) {
            FEDatabase->GetUserProfile(0)->SetProfileName(nullptr, true);
        }
        MemoryCard::LoadLocale(CurrentLanguage);
    }
}

eLanguages GetCurrentLanguage() {
    return CurrentLanguage;
}

struct WideCharHistogram;

void WideToCharString(char *dest, unsigned int destlen, const short *src) {
    if (!dest) {
        return;
    }
    if (!src) {
        return;
    }
    unsigned int bytes = 0;
    unsigned short ch = *reinterpret_cast<const unsigned short *>(src);
    if (ch != 0) {
        destlen = destlen - 1;
        if (bytes < destlen) {
            do {
                if (ch < 0x100) {
                    bytes++;
                    *dest = reinterpret_cast<const char *>(src)[1];
                    src++;
                    dest++;
                } else {
                    src++;
                }
                ch = *reinterpret_cast<const unsigned short *>(src);
            } while (ch != 0 && bytes < destlen);
        }
    }
    *dest = 0;
}

static const char *SearchForString(unsigned int string_label) {
    if (!RecordTable) {
        return nullptr;
    }
    unsigned int top = NumStringRecords - 1;
    unsigned int bot = 0;
    while (true) {
        unsigned int mid = (bot + top) >> 1;
        unsigned int hash = RecordTable[mid].Hash;
        if (hash == string_label) {
            return reinterpret_cast<const char *>(RecordTable[mid].PackedString);
        }
        if (top - bot < 3) {
            if (RecordTable[bot].Hash == string_label) {
                return reinterpret_cast<const char *>(RecordTable[bot].PackedString);
            }
            if (RecordTable[top].Hash == string_label) {
                return reinterpret_cast<const char *>(RecordTable[top].PackedString);
            }
            break;
        }
        if (mid == bot) {
            return nullptr;
        }
        if (hash > string_label) {
            top = mid;
        }
        if (hash < string_label) {
            bot = mid;
        }
    }
    return nullptr;
}

int UnloaderLanguage(bChunk *chunk) {
    if (chunk->GetID() == 0x39000) {
        RecordTable = nullptr;
        PackedStringTable = nullptr;
        pWideCharHistogram = nullptr;
        NumStringRecords = 0;
        return 1;
    }
    return 0;
}

int LoaderLanguage(bChunk *chunk) {
    if (chunk->GetID() == 0x39000) {
        LanguageChunkHeader *header = reinterpret_cast<LanguageChunkHeader *>(chunk->GetData());
        header->PlatEndianSwap();
        RecordTable = reinterpret_cast<StringRecord *>(reinterpret_cast<char *>(header) + header->StringRecordTablePos);
        PackedStringTable = reinterpret_cast<unsigned char *>(header) + header->StringTablePos;
        pWideCharHistogram = reinterpret_cast<WideCharHistogram *>(reinterpret_cast<char *>(header) + header->HistogramTablePos);
        NumStringRecords = header->NumStringRecords;
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

void FormatMessage(char *buf, int size, const char *fmt, va_list *args) {
    bVSPrintf(buf, fmt, args);
}

void GetLocalizedString(char *buffer, unsigned int bufsize, unsigned int string_label) {
    char *str = const_cast<char *>(GetLocalizedString(string_label));
    bStrNCpy(buffer, str, static_cast<int>(bufsize));
}

bool GetLocalizedWideString(short *wide_string, int wide_string_buffer_size, unsigned int string_label) {
    const char *str = SearchForString(string_label);
    if (str) {
        PackedStringToWideString(reinterpret_cast<unsigned short *>(wide_string), wide_string_buffer_size, str);
        return true;
    }
    return false;
}

const char *GetLocalizedPercentSign() {
    const char *szPercentUnit = "%";
    eLanguages currLang = GetCurrentLanguage();
    if (currLang == eLANGUAGE_DANISH || currLang == eLANGUAGE_FINNISH || currLang == eLANGUAGE_FRENCH || currLang == eLANGUAGE_GERMAN || currLang == eLANGUAGE_SWEDISH) {
        szPercentUnit = " %";
    }
    return szPercentUnit;
}

struct FontSizeInfo {
    unsigned int Hash; // offset 0x0
    int Size;          // offset 0x4
};

extern FontSizeInfo FontSizeInfoTable[9];
extern int LanguageMemoryPoolNumber;
extern void *pLanguageMemoryPoolMemory;
extern int LanguageMemoryPoolSize;
extern bool IsKorea();
extern int bGetFreeMemoryPoolNum();
extern void bInitMemoryPool(int pool, void *mem, int size, const char *name);
extern void eLoadStreamingTexturePack(const char *filename, void (*callback)(void *), void *param, int flags);
extern void eWaitForStreamingTexturePackLoading(const char *name);

struct VMFile;
extern ResourceFile *pLanguageResourceFile;
extern VMFile *pLanguageResourceFile_VM;
extern void UnloadFileFromVirtualMemory(VMFile *vm_file);
extern VMFile *LoadFileIntoVirtualMemory(const char *filename, bool compressed, bool use_trackstreampool_as_temp);
extern void WaitForResourceLoadingComplete();

void InitLocalization() {
    LanguageInfo *info;
    if (IsKorea()) {
        info = GetLanguageInfo(eLANGUAGE_KOREAN);
    } else {
        info = GetLanguageInfo(eLANGUAGE_ENGLISH);
    }
    unsigned int *fonts = info->pFontNameInfo->GlobalFonts;
    int total_font_size = 0;
    int n = 0;
    while (fonts[n] != 0) {
        int font_size = -1;
        for (int i = 0; i < 9; i++) {
            if (FontSizeInfoTable[i].Hash == fonts[n]) {
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
        pLanguageMemoryPoolMemory = bMalloc(LanguageMemoryPoolSize, 0);
        bInitMemoryPool(LanguageMemoryPoolNumber, pLanguageMemoryPoolMemory,
                        LanguageMemoryPoolSize, "LanguageMemoryPool");
    }
    eLoadStreamingTexturePack("LANGUAGES\\LANGUAGETEXTURES.BIN", nullptr, nullptr, 0);
    eWaitForStreamingTexturePackLoading("LANGUAGES\\LANGUAGETEXTURES.BIN");
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
            {} // empty anonymous block (DWARF)
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
            eLoadStreamingTexture(info->pFontNameInfo->GlobalFonts, 8, static_cast<void (*)(void*)>(nullptr), static_cast<void*>(nullptr), LanguageMemoryPoolNumber);
            info->pFontNameInfo->GlobalFontsLoaded = 1;
        }
    }
    if (load_frontend && !info->pFontNameInfo->FrontendFontsLoaded) {
        eLoadStreamingTexture(info->pFontNameInfo->FrontendFonts, 8, static_cast<void (*)(void*)>(nullptr), static_cast<void*>(nullptr), LanguageMemoryPoolNumber);
        info->pFontNameInfo->FrontendFontsLoaded = 1;
    }
    if (load_ingame && !info->pFontNameInfo->InGameFontsLoaded) {
        eLoadStreamingTexture(info->pFontNameInfo->InGameFonts, 8, static_cast<void (*)(void*)>(nullptr), static_cast<void*>(nullptr), LanguageMemoryPoolNumber);
        info->pFontNameInfo->InGameFontsLoaded = 1;
    }
    if (blocking) {
        eWaitForStreamingTexturePackLoading("LANGUAGES\\LANGUAGETEXTURES.BIN");
    }
}
