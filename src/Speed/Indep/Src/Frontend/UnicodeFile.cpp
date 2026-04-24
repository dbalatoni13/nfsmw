#include "UnicodeFile.hpp"

#include "Speed/Indep/bWare/Inc/bWare.hpp"

void bEndianSwap(short* value);
extern void* bGetFile(const char* filename, int* size, int flags);

UnicodeFile::UnicodeFile()
    : data_(nullptr) //
    , next_(nullptr) //
    , end_(nullptr)
{}

UnicodeFile::~UnicodeFile() {
    Unload();
}

bool UnicodeFile::Load(const char* filename) {
    int size;
    data_ = static_cast<short*>(bGetFile(filename, &size, 0));
    next_ = nullptr;
    if (data_ != nullptr) {
        end_ = data_ + size / 2;
        if (*data_ == static_cast<short>(0xFFFE)) {
            FixEndian();
        }
        FixEOLs();
        *(end_ - 1) = 0;
    }
    return data_ != nullptr;
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

short* UnicodeFile::Next() {
    if (data_ == nullptr || next_ == nullptr) {
        return nullptr;
    }
    if (next_ != end_) {
        while (*next_ != 0) {
            next_++;
            if (next_ == end_) {
                goto done;
            }
        }
        if (next_ != end_) {
            while (*next_ == 0) {
                next_++;
                if (next_ == end_) {
                    goto done;
                }
            }
            if (next_ != end_) {
                return next_;
            }
        }
    }
done:
    next_ = nullptr;
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

void UnicodeFile::LineWrap(int maxCharacters) {
    short* p = First();
    while (p != nullptr) {
        int count = 0;
        short* lastSpace = nullptr;
        while (*p != 0) {
            count++;
            if (count > 1 && *p == 0x20 && *(p - 1) != 0x20) {
                lastSpace = p;
            }
            if (count >= maxCharacters && lastSpace != nullptr) {
                while (*lastSpace == 0x20) {
                    *lastSpace = 0;
                    lastSpace++;
                }
                count = (p - lastSpace);
                lastSpace = nullptr;
            }
            p++;
        }
        p = Next();
    }
}
