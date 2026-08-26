#ifndef REALMEMCARD_UTILITIES_H
#define REALMEMCARD_UTILITIES_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

namespace RealmcUtils {

unsigned int Crc32(const void *buf, int bufsize);
void Ascii2Unicode(wchar_t *dst, const char *src);
int Wildcard(char *str, char *pattern);

} // namespace RealmcUtils

#endif
