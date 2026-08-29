#ifndef REALMEMCARD_GC_BLOCKCALCULATOR_H
#define REALMEMCARD_GC_BLOCKCALCULATOR_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

#pragma interface

#include "gc_driver.h"

namespace Realmc {

class BlockCalculator {
  public:
    BlockCalculator() {}
    virtual void Clear() = 0;
    virtual unsigned int GetResult() = 0;
    virtual void SetFileInfo(const CardID &cardID, const FileInfo &fileInfo) = 0;
    virtual ~BlockCalculator() {}
};

class BlockCalculatorImp : public BlockCalculator {
  public:
    BlockCalculatorImp();
    void Init(GCDriver *driver, unsigned int blockSize);
    virtual void Clear();
    virtual void SetFileInfo(const CardID &cardID, const FileInfo &fileInfo);
    virtual ~BlockCalculatorImp() {}
    virtual unsigned int GetResult() {
        return this->mBlockCount;
    }

  private:
    GCDriver *mDriver;
    unsigned int mBlockCount;
    unsigned int mFileCount;
    unsigned int mBlockSize;
    bool mFileInfoSet;
};

} // namespace Realmc

#endif
