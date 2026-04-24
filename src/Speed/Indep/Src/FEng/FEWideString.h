#ifndef _FEWIDESTRING
#define _FEWIDESTRING

// total size: 0x8
struct FEWideString {
    short* mpsString;              // offset 0x0, size 0x4
    unsigned long mulBufferLength; // offset 0x4, size 0x4

    FEWideString();
    FEWideString(const short* psString);
    FEWideString(const char* pcString);
    FEWideString(const FEWideString& string);
    ~FEWideString();

    bool operator==(const FEWideString& string);
    FEWideString& operator=(const FEWideString& string);
    FEWideString& operator=(const short* psString);
    FEWideString& operator=(const char* pcString);
    FEWideString& operator+=(const FEWideString& string);
    FEWideString& operator+=(const short* pString);
    FEWideString& operator+=(const char* pcString);

    inline void Associate(short* pString);
    inline short* Disaccociate();
    inline operator short*() { return mpsString; }
    inline operator const short*() const { return mpsString; }
    inline const short operator[](int i) const;
    inline short& operator[](int i);

    unsigned long Length() const;
    void SetLength(const unsigned long newLength);
    char* Convert(char* pString) const;
    short* AllocateString(const unsigned long newLength);

    inline void ConvertToSpecial(const FEWideString& string);
    void ConvertToSpecial(const char* pcString);
    void ConvertToSpecial(const unsigned char* pcString);
    void ConvertToSpecial(const short* psString);
    void ConvertFromSpecial(char* pcString);
    void ConvertFromSpecial(short* psString);
    void ConvertFromSpecial(FEWideString& string);
};

#endif
