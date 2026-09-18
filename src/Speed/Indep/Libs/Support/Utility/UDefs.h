#ifndef UDEFS_H
#define UDEFS_H

#ifdef EA_PLATFORM_PLAYSTATION2
#define PS2ALIGN16 __attribute__((aligned(16)))
#else
#define PS2ALIGN16
#endif

// TODO ifdef for platforms

#define ALIGND
#define ALIGN32POST
#define ALIGNPRE
#define ALIGNPOST
#if defined(EA_PLATFORM_XENON)
// Vector members are 16-byte aligned on Xbox 360 too (the original code reads them at those offsets).
#define ALIGNVEC __declspec(align(16))
#else
#define ALIGNVEC PS2ALIGN16
#endif
#define ALIGNDMA
#define ALIGNDMA_POST

#define BOND 1

#define SECTION(s)

#define FINAL_EVENT 1
#define FINAL_PRINT 1
#define FINAL_PROFILE 1
#define FINAL_ASSERT 1

#endif
