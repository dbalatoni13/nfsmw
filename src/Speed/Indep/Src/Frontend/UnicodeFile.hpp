#ifndef _UNICODEFILE_HPP_
#define _UNICODEFILE_HPP_

#include "Speed/Indep/Src/FEng/FETypes.h"
#include <types.h>

// total size: 0xC
// Decl: 5
class UnicodeFile {
  public:
    UnicodeFile(); // Decl: 7
    UnicodeFile(const char *filename);
    ~UnicodeFile(); // Decl: 9

    bool Load(const char *filename); // Decl: 11
    void Unload();                   // Decl: 12

    void LineWrap(int maxCharacters); // Decl: 14

    i16 *First(); // Decl: 16
    i16 *Next();  // Decl: 17

  private:
    i16 *data_; // offset 0x0, size 0x4, Decl: 20
    i16 *next_; // offset 0x4, size 0x4, Decl: 21
    i16 *end_;  // offset 0x8, size 0x4, Decl: 22

    void FixEndian(); // Decl: 24
    void FixEOLs();   // Decl: 25
};

#endif
