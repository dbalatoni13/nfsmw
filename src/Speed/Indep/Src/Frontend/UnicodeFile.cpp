#include "UnicodeFile.hpp"

#include "Speed/Indep/Src/FEng/FETypes.h"
#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

UnicodeFile::UnicodeFile()
    : data_(nullptr), //
      next_(nullptr), //
      end_(nullptr) {}

UnicodeFile::~UnicodeFile() {
    Unload();
}

bool UnicodeFile::Load(const char *filename) {
    int32 size;
    data_ = static_cast<i16 *>(bGetFile(filename, &size, 0));
    next_ = nullptr;
    if (data_ != nullptr) {
        end_ = data_ + size / 2;
        if (*data_ == static_cast<i16>(0xFFFE)) {
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

i16 *UnicodeFile::First() {
    if (data_ == nullptr) {
        return nullptr;
    }
    next_ = data_;
    if (*data_ == static_cast<i16>(0xFEFF)) {
        next_ = data_ + 1;
    }
    return next_;
}

// UNSOLVED
i16 *UnicodeFile::Next() {
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
    for (i16 *p = data_; p != end_; p++) {
        bEndianSwap(p);
    }
}

void UnicodeFile::FixEOLs() {
    for (i16 *p = data_; p != end_; p++) {
        if (*p == 10 || *p == 13) {
            *p = 0;
        }
    }
}

void UnicodeFile::LineWrap(int maxCharacters) {
    i16 *p = First();
    while (p != nullptr) {
        int count = 0;
        i16 *lastSpace = nullptr;
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
