#ifndef BWARE_BMEMORY_H
#define BWARE_BMEMORY_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

unsigned int GetVirtualMemoryAllocParams();
int bGetMemoryPoolNum(const char *memory_pool_name /* r30 */);

#endif
