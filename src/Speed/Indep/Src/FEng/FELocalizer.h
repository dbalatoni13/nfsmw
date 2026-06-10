#ifndef FELOCALIZER_H_
#define FELOCALIZER_H_

#include <types.h>
#include "Speed/Indep/Src/FEng/FEList.h"

// File: speed/indep/src/feng/FELocalizer.h
// total size: 0x1C
// Decl: speed/indep/src/feng/FELocalizer.h:26
class FELocalString {
  public:
    FELocalString() {} // Decl: speed/indep/src/feng/FELocalizer.h:28
    ~FELocalString() {}

    void SetLabelName(const char *pString);

    void SetPlatform(const char *pString);

    void SetComment(const char *pString);

    void SetFlags(const char *pString);

    void SetSize();

    char *pLabelName;         // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:37
    char *pPlatform;          // offset 0x4, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:38
    char *pComment;           // offset 0x8, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:39
    char *pFlags;             // offset 0xC, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:40
    char *pMax_Size;          // offset 0x10, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:41
    u32 LabelHash;            // offset 0x14, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:43
    FEWideString *pLanguages; // offset 0x18, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:44
};

// total size: 0x28
// Decl: speed/indep/src/feng/FELocalizer.h:59
class FELocalizer {
  private:
    u32 NumLangs;               // offset 0x0, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:61
    FEList Languages;           // offset 0x4, size 0x10, Decl: speed/indep/src/feng/FELocalizer.h:62
    bool bKeepLabels;           // offset 0x14, size 0x1, Decl: speed/indep/src/feng/FELocalizer.h:63
    u32 StringCount;            // offset 0x18, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:65
    u32 StringAlloc;            // offset 0x1C, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:66
    bool bListSorted;           // offset 0x20, size 0x1, Decl: speed/indep/src/feng/FELocalizer.h:67
    FELocalString *pStringList; // offset 0x24, size 0x4, Decl: speed/indep/src/feng/FELocalizer.h:69

    bool ReadString(short *pDest, char *pBuffer, u32 &StartAt, u32 Length);

    bool ReadNextString(short *pDest, char *pBuffer, u32 &StartAt, u32 Length);

    void WideToEscapeString(char *pDest, const FEWideString &Src);

    void WideToEscapeString(char *pDest, const short *pSrc);

  public:
    FELocalizer(); // Decl: speed/indep/src/feng/FELocalizer.h:82
    ~FELocalizer();

    void ReleaseAll(); // Decl: speed/indep/src/feng/FELocalizer.h:84

    void SetNumLanguages(u32 Count);

    u32 GetNumLanguages() const {}

    void SetLanguageName(const char *pName, u32 LangIndex);

    const char *GetLanguageName(u32 LangIndex) const;

    void AllocStrings(u32 Count);

    u32 GetNumStrings() const {}

    u32 GetNumAlloc() const {}

    FEWideString *GetString(const char *pLabelName, u32 LangIndex) const;

    FEWideString *GetString(u32 LabelHash, u32 LangIndex) const;

    FELocalString *GetStringByIndex(u32 Index) const {}

    u32 GetLabelIndex(const char *pLabelName) const;

    u32 GetLabelIndex(u32 LabelHash) const;

    void SetString(const char *pLabelName, u32 LangIndex, FEWideString &String);

    bool SetString(u32 LabelHash, u32 LangIndex, FEWideString &String);

    bool SetFlags(u32 LabelHash, char *flags);

    bool SetSize(u32 LabelHash, char *size);

    bool SetComments(u32 LabelHash, char *comments);

    bool ReadLocaleData(void *pData, u32 Size);

    u32 WriteLocaleData(void *pDest, u32 BufSize);
};

#endif
