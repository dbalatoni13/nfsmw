#ifndef _VERSION_H
#define _VERSION_H

#define VERSION_USA 0
#define VERSION_PAL 1

#if VERSION == VERSION_USA
#define VERSION_SELECT(USA, PAL) (USA)
#elif VERSION == VERSION_PAL
#define VERSION_SELECT(USA, PAL) (PAL)
#endif

#define REFRESH_RATE (60.0f)

#endif
