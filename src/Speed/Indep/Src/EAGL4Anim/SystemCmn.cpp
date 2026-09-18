#include "System.h"

#include "eagl4AnimBank.h"
#include "eagl4supportconspool.h"
#include "eagl4supportdlopen.h"
#include "FnAnimFactory.h"
#include "MemoryPoolManager.h"

namespace EAGL4Anim {

void Initializer::InitInternal(size_t memorySize, bool enableStats) {
    MemoryPoolManager::Startup();
    MemoryPoolManager::Init(memorySize);

    FnAnimFactory::mpFactory = new FnAnimFactory;

    EAGL4::DynamicLoader::gConsPool.AddType("AnimationBank", AnimBank::Constructor, AnimBank::Destructor);
}

}; // namespace EAGL4Anim
