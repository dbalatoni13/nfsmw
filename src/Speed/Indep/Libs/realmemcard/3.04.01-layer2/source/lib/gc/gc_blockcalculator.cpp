#include "../../../include/common/realmemcard/gc_blockcalculator.h"

namespace Realmc {

BlockCalculatorImp::BlockCalculatorImp() {
    this->mBlockSize = 0;
    this->Clear();
}

void BlockCalculatorImp::Init(GCDriver *driver, unsigned int blockSize) {
    this->Clear();
    this->mDriver = driver;
    this->mBlockSize = blockSize;
}

void BlockCalculatorImp::Clear() {
    this->mBlockCount = 0;
    this->mFileCount = 0;
    this->mFileInfoSet = false;
}

void BlockCalculatorImp::SetFileInfo(const CardID &cardID, const FileInfo &fileInfo) {
    this->mBlockCount += this->mDriver->GetFileBlocks(cardID, &fileInfo, nullptr);
    this->mFileInfoSet = true;
    this->mFileCount += 1;
}

} // namespace Realmc
