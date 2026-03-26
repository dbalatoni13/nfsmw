#ifndef MAIN_STUBS_H
#define MAIN_STUBS_H

#ifdef EA_PRAGMA_ONCE_SUPPORTED
#pragma once
#endif

inline void dbattrib(float, float, unsigned int, float, float, unsigned int);
inline void dbattrib(int, int, unsigned int, int, int, unsigned int);

inline void dbattrib(float min1, float max1, unsigned int attr1, float min2, float max2, unsigned int attr2) {}
inline void dbattrib(int min1, int max1, unsigned int attr1, int min2, int max2, unsigned int attr2) {}

#endif
