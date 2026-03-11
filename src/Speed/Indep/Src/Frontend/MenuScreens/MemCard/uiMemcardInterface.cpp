#include "Speed/Indep/Src/Frontend/MenuScreens/MemCard/uiMemcardInterface.hpp"

unsigned int MemcardGetCurrentUIOperation() {
    return gMemcardSetup.mOp & 0xf0;
}
