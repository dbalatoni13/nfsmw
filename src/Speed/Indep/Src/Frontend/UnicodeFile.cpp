#include "UnicodeFile.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"

void bEndianSwap(short* value);

UnicodeFile::UnicodeFile()
    : data_(nullptr) //
    , next_(nullptr) //
    , end_(nullptr)
{}

UnicodeFile::~UnicodeFile() {
    Unload();
}

void UnicodeFile::Unload() {
    if (data_ != nullptr) {
        bFree(data_);
        data_ = nullptr;
    }
}

short* UnicodeFile::First() {
    short* p = data_;
    if (p == nullptr) {
        return nullptr;
    }
    next_ = p;
    if (*p == static_cast<short>(0xFEFF)) {
        next_ = p + 1;
    }
    return next_;
}

void UnicodeFile::FixEndian() {
    short* p = data_;
    while (p != end_) {
        bEndianSwap(p);
        p++;
    }
}

void UnicodeFile::FixEOLs() {
    short* p = data_;
    while (p != end_) {
        if (*p == 10 || *p == 13) {
            *p = 0;
        }
        p++;
    }
}
