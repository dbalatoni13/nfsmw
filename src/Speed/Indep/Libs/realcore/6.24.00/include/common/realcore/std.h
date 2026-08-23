#ifndef REALCORE_COMMON_STD_H
#define REALCORE_COMMON_STD_H

void MEM_clear(void *dest, int count);
void MEM_copy(void *dest, const void *source, int count);
void MEM_fill(void *dest, unsigned int val, int count);

#endif
