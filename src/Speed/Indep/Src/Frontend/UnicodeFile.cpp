#include "UnicodeFile.hpp"

#include "Speed/Indep/Src/Misc/bFile.hpp"
#include "Speed/Indep/bWare/Inc/bWare.hpp"

UnicodeFile::UnicodeFile() {
    data_ = 0;
    next_ = 0;
    end_ = 0;
}

UnicodeFile::~UnicodeFile() { Unload(); }

bool UnicodeFile::Load(const char *filename) {
    int size;
    data_ = (short *)bGetFile(filename, &size, 0);
    next_ = 0;
    if (data_ != 0) {
        end_ = data_ + size / 2;
        if (data_[0] == -2) {
            FixEndian();
        }
        FixEOLs();
        end_[-1] = 0;
    }
    return data_ != 0;
}

void UnicodeFile::Unload() {
    if (data_ != 0) {
        bFree(data_);
        data_ = 0;
    }
}

short *UnicodeFile::First() {
    if (data_ == 0) {
        return 0;
    }
    next_ = data_;
    if (*next_ == -257) {
        next_ = next_ + 1;
    }
    return next_;
}

short *UnicodeFile::Next() {
    if (data_ == 0 || next_ == 0) {
        return 0;
    }
    if (next_ != end_) {
        while (next_ != end_ && *next_ != 0) {
            next_ = next_ + 1;
        }
        while (next_ != end_ && *next_ == 0) {
            next_ = next_ + 1;
        }
    }
    if (next_ == end_) {
        next_ = 0;
    }
    return next_;
}

void UnicodeFile::FixEndian() {
    for (short *p = data_; p != end_; p++) {
        bEndianSwap16(p);
    }
}

void UnicodeFile::FixEOLs() {
    for (short *p = data_; p != end_; p++) {
        if (*p == 0xA || *p == 0xD) {
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
