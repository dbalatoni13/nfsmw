#ifndef SUPPORT_UTILITY_UTLFASTVECTOR_H
#define SUPPORT_UTILITY_UTLFASTVECTOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#include "Speed/Indep/Libs/Support/Utility/FastMem.h"
#include "Speed/Indep/Libs/Support/Utility/UTLVector.h"

namespace UTL {

template <typename T, int Alignment = 16> class FastVector : public Vector<T, Alignment> {
  public:
    FastVector() {}

    ~FastVector() override {
        Vector<T, Alignment>::clear();
    }

  protected:
    typename Vector<T, Alignment>::pointer AllocVectorSpace(std::size_t num, unsigned int alignment) override {
        return static_cast<typename Vector<T, Alignment>::pointer>(
            gFastMem.Alloc(num * sizeof(T), nullptr));
    }

    void FreeVectorSpace(typename Vector<T, Alignment>::pointer buffer, std::size_t num) override {
        gFastMem.Free(buffer, num * sizeof(T), nullptr);
    }
};

}; // namespace UTL

#endif
