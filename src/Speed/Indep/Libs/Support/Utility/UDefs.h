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
#define ALIGNVEC PS2ALIGN16
#define ALIGNDMA
#define ALIGNDMA_POST

#define BOND 1

#define SECTION(s)

#define FINAL_EVENT 1
#define FINAL_PRINT 1
#define FINAL_PROFILE 1
#define FINAL_ASSERT 1

#endif
