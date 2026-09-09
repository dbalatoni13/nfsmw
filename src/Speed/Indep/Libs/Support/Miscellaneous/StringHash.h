#ifndef SUPPORT_MISC_STRINGHASH_H
#define SUPPORT_MISC_STRINGHASH_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

unsigned int stringhash32(const char *k);

inline unsigned int stringhash(const char *k) {
    return stringhash32(k);
}

#endif
