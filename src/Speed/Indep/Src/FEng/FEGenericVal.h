#ifndef FENG_FEGENERICVAL_H
#define FENG_FEGENERICVAL_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

// total size: 0x10
class FEGenericVal {
  public:
    inline operator unsigned char*() const { return reinterpret_cast<unsigned char*>(const_cast<unsigned long*>(Data)); }
    inline operator long*() const { return reinterpret_cast<long*>(const_cast<unsigned long*>(Data)); }
  private:
    unsigned long Data[4]; // offset 0x0, size 0x10
};

#endif
