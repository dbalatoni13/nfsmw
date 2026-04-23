#ifndef FRONTEND_LOCALIZATION_LOCALIZE_H
#define FRONTEND_LOCALIZATION_LOCALIZE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

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

const char *GetLocalizedString(unsigned int string_label);
char *GetTranslatedString(int label_hash);
void LoadCurrentLanguage();
void InitLocalization();
eLanguages GetCurrentLanguage();

#endif
