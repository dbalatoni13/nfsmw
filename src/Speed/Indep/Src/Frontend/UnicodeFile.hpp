#ifndef _UNICODEFILE_HPP_
#define _UNICODEFILE_HPP_

#include <types.h>

// File: speed/indep/src/frontend/UnicodeFile.hpp
// total size: 0xC
// Decl: speed/indep/src/frontend/UnicodeFile.hpp:5
class UnicodeFile {
  public:
    UnicodeFile(); // Decl: speed/indep/src/frontend/UnicodeFile.hpp:7
    UnicodeFile(const char *filename);
    ~UnicodeFile(); // Decl: speed/indep/src/frontend/UnicodeFile.hpp:9

    bool Load(const char *filename); // Decl: speed/indep/src/frontend/UnicodeFile.hpp:11
    void Unload();                   // Decl: speed/indep/src/frontend/UnicodeFile.hpp:12

    void LineWrap(int maxCharacters); // Decl: speed/indep/src/frontend/UnicodeFile.hpp:14

    i16 *First(); // Decl: speed/indep/src/frontend/UnicodeFile.hpp:16
    i16 *Next();  // Decl: speed/indep/src/frontend/UnicodeFile.hpp:17

  private:
    i16 *data_; // offset 0x0, size 0x4, Decl: speed/indep/src/frontend/UnicodeFile.hpp:20
    i16 *next_; // offset 0x4, size 0x4, Decl: speed/indep/src/frontend/UnicodeFile.hpp:21
    i16 *end_;  // offset 0x8, size 0x4, Decl: speed/indep/src/frontend/UnicodeFile.hpp:22

    void FixEndian(); // Decl: speed/indep/src/frontend/UnicodeFile.hpp:24
    void FixEOLs();   // Decl: speed/indep/src/frontend/UnicodeFile.hpp:25
};

#endif
