#ifndef MISC_BFILE_H
#define MISC_BFILE_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

void *bGetFile(const char *filename, int *size, int allocation_params);
void bFileFlushCachedFiles();
#endif
