#include "System.h"

#include "FnAnimFactory.h"
#include "MemoryPoolManager.h"
#include "eagl4AnimBank.h"
#include "eagl4supportdlopen.h"

#include <new>

namespace EAGL4Anim {

namespace {

struct MemoryPoolManagerAccessor : MemoryPoolManager {
    static MemoryPoolManager *&GetDefaultMemoryManager() {
        return gDefaultMemoryManager;
    }

    static MemoryPoolManager *&GetMemoryManager() {
        return gMemoryManager;
    }
};

} // namespace

void Initializer::InitInternal(size_t memorySize, bool) {
    MemoryPoolManagerAccessor::GetDefaultMemoryManager() =
        new (EAGL4Internal::EAGL4Malloc(sizeof(MemoryPoolManager), nullptr)) MemoryPoolManager;
    MemoryPoolManagerAccessor::GetMemoryManager() = MemoryPoolManagerAccessor::GetDefaultMemoryManager();
    MemoryPoolManagerAccessor::GetDefaultMemoryManager()->InitAux(static_cast<unsigned int>(memorySize));

    FnAnimFactory::mpFactory = reinterpret_cast<FnAnimFactory *>(EAGL4Internal::EAGL4Malloc(1, nullptr));
    EAGL4::DynamicLoader::gConsPool.AddType(EAGL4::DynamicLoader::AnimBankType, AnimBank::Constructor, AnimBank::Destructor);
}

}; // namespace EAGL4Anim
