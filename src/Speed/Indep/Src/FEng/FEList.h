#ifndef FENG_FELIST_H
#define FENG_FELIST_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0xC
class FEMinNode {
  public:
  protected:
    FEMinNode *next; // offset 0x0, size 0x4
    FEMinNode *prev; // offset 0x4, size 0x4
};

// total size: 0x10
class FEMinList {
  public:
  private:
    unsigned int numElements; // offset 0x0, size 0x4
  protected:
    FEMinNode *head; // offset 0x4, size 0x4
    FEMinNode *tail; // offset 0x8, size 0x4
};

#endif
