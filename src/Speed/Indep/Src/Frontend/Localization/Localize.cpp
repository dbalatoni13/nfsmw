#include "Speed/Indep/Src/Frontend/MemoryCard/MemoryCard.hpp"
#include "Speed/Indep/bWare/Inc/bPrintf.hpp"

extern void GC_GetOSLanguage();
extern void SetCurrentLanguage(eLanguages lang);
extern const char *SearchForString(unsigned int hash);
extern const char *GetLocalizedString(unsigned int id);

static eLanguages CurrentLanguage;

eLanguages GetCurrentLanguage() {
    return CurrentLanguage;
}

void LoadCurrentLanguage() {
    GC_GetOSLanguage();
    SetCurrentLanguage(CurrentLanguage);
}

bool DoesStringExist(unsigned int hash) {
    return SearchForString(hash) != 0;
}

char *GetTranslatedString(int id) {
    return const_cast<char *>(GetLocalizedString(static_cast<unsigned int>(id)));
}

void FormatMessage(char *buf, int size, const char *fmt, va_list *args) {
    bVSPrintf(buf, fmt, args);
}