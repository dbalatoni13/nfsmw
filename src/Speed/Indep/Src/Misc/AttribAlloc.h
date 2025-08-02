#ifndef MISC_ATTRIB_ALLOC_H
#define MISC_ATTRIB_ALLOC_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

struct AttribAlloc {
    static void Free(void *ptr, unsigned int bytes, const char *name) {}
};

#endif
