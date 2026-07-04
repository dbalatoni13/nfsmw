/* ---------------------------------------------------------------------------

  libsn.h: SN Systems NGC support library
  $Date: 15/04/03 18:03 $ by $Author: Tom $
  $Revision: 12 $ 
  $NoKeywords: $

  (c) Copyright 2001-2003 SN Systems Ltd.

  File serving
  Profiling
  snPause



  ---------------------------------------------------------------------------- */

#ifndef _LIBSN_H
#define _LIBSN_H

#ifdef __cplusplus
extern "C" {
#endif

extern unsigned int __SN_Libsn_version[];	/* Declared as array so that compiler long refs this symbol */

/* Example
	printf("libsn version = %d\n",__SN_Libsn_version[0]);
*/


/*============ PROFILER STUFF ====================*/
#ifndef	UINT32
#define UINT32 unsigned int
#define NEEDUNDEFUINT32
#endif
/* values for full speed NGC */
#define NGCCPUSPEED		486000000
#define NGCBUSSPEED		162000000
/* interval values to pass to snProfInit() and snProfSetInterval() */
#define	_1KHZ	((NGCBUSSPEED/4)/1000)
#define	_2KHZ	((NGCBUSSPEED/4)/2000)
#define	_4KHZ	((NGCBUSSPEED/4)/4000)
#define	_10KHZ	((NGCBUSSPEED/4)/10000)
#define	_20KHZ	((NGCBUSSPEED/4)/20000)

extern int snProfInit(UINT32 rate, void* buffer, int buffersize);

/* and it's return values */
#define SN_PRF_SUCCESS          0	/* operation succeeded */
#define SN_PRF_BUFFSIZE			1	/* The buffer is too small or too large */

extern	void	snProfSetInterval(UINT32 interval);
extern	void	snProfSetRange(UINT32 profmask, void *startpc, void *endpc);
extern	int		snProfSetFlagValue(UINT32 value);
extern	int		snProfSetFlags(UINT32 flags);
extern	int		snProfClrFlags(UINT32 flags);

#ifdef NEEDUNDEFUINT32				/* only undefine it if *we* defined it */
#undef UINT32
#endif

/* These macros stop the debug-stub hooking these exceptions */
#define SN_LEAVE_DSI_FOR_OS()	long	SN_DSI = 0;
#define SN_LEAVE_ISI_FOR_OS()	long	SN_ISI = 0;
#define SN_LEAVE_ALIGNMENT_FOR_OS()	long	SN_ALIGNMENT = 0;

/* This forces the debugger to intercept the FP exception (and so the FPE isn't passed to the OS) */
#define SN_HOOK_FPE()	long	SN_FPE = 1;

/* This disables single char buffered TTY */
#define SN_DISABLE_BUFFERED_TTY()	long	SN_BUFFERED_TTY = 0;

/* returns 1 if running on TDEV */
extern int snIsSNTDEV(void);
#define IsDevLink snIsSNTDEV	/* For compatibility with old code */

/* Example
	if (snIsSNTDEV()==1)
		fh = PCopen("test.bin",O_RDONLY,0);
	else
		DVDOpen("test.bin",&fileInfo);
*/

/* The snPause call inserts a breakpoint.
   When the NGC CPU attempts to execute it, the code will stop 
   executing and the debugger will show a stopped state. Selecting Run from 
   the debugger will continue the code execution again.

   The snPollHost call should be inserted into the main game loop.
   It polls the Host PC, allowing debug communications to be maintained.
   It is only needed when interrupts are disabled for long periods of time.

   The snFlushTtyBuffer call can be used to flush the single char TTY buffer.
   It isn't needed on GDEV or DDH hardware.
*/
#ifdef __GNUC__
#define snPause() asm(" .long 0x00000001")
#define snPollHost() if(snIsSNTDEV()) asm(" .long 0x00000002")
#define snFlushTtyBuffer() if(snIsSNTDEV()) asm(" .long 0x00000004")
#else
#ifdef _WIN32
#define snPause()
#define snPollHost()
#define snFlushTtyBuffer()
#else
#define snPause() asm{ opword 0x00000001}
#define snPollHost() if(snIsSNTDEV()) asm{ opword 0x00000002}
#define snFlushTtyBuffer() if(snIsSNTDEV()) asm{ opcode 0x00000004}
#endif
#endif

/*===============================*/
/*=== Optional user functions ===*/
/*===============================*/

/* Called after OSInit() in crt0.
   Use to initialise Virtual Memory.
 */
extern void __init_vm(void);

/* Called after OSInit() & __init_vm() in crt0.
   Called before __init_user() in crt0.
   Use to do initialisation before global ctors are called.
 */
extern void __premain(int argc, char** argv);

/*===============================*/
/*==== file serving support =====*/
/*===============================*/

#define SN_ERR	-1

extern int PCinit(void);
/* This function initializes file serving. You should call this before attempting to
transfer files. */

extern int PCcreat(const char *name, int perms);
/* create file. 
   name     = name of file it attempts to create on the host.
   perms    = supplied for future enhancement; currently ignored and
              should be 0.

   Returns -1 = error, otherwise returns a file handle which can be used in
   subsequent file operations. */

extern int PCopen(const char *name, int flags, int perms);
/* open file.
   name     = name of file it attempts to open on the host, which must exist.
   flags    = 0 - read/only access
              1 - write/only access
              2 - read/write access
              Alternatively, you can specify #include <sys\fcntl.h> to obtain the
              following default values:
                O_RDONLY - read/only access
                O_WRONLY - write/only access
                O_RDWR   - read/write access
   perms    = supplied for future enhancement; currently ignored and should
              be 0.

  Returns -1 = error; otherwise returns a file handle which can be used in
  subsequent file operations. */

extern int PCclose(int fd);
/* Closes the specified file handle, freeing it for re-use. Returns 0 if the
   closure was successful or -1 if not. */

extern int PCread(int fd, void *buf, unsigned int n);
/* Read n data bytes to buf from the current seek position of the
   specified file handle fd.

   Returns the number of bytes actually read, which will be fewer than n if there was
   an error reading the file or the end of the file was reached.*/

extern int PCwrite(int fd, void *buf, unsigned int n);
/* write n data bytes from buf to the current seek position of the
   specified file handle fd.

   You can use the pre-opened handle of -1 to write to the Target Manager's
   message window. This is how SN System's implementation of printf works. To
   write a string to the TTY pane, specify the start address of the string and the
   number of characters to write.

   Note that some libc.a functions (printf,fprintf etc.) have been set up to
   use PCwrite, so that printf output will go directly to the TTY pane and
   fprintf output can go to any output file on the PC.

   Note: Do not include the string's terminating zero in this count.
   
   Returns the number of bytes actually written, which will be fewer than the n
   parameter if there was an error writing the file. */

extern int PClseek(int fd, long offset, int origin); 
/* This function attempts to move the current seek position of the specified file
   handle.

   offset = the position to move to, relative to the value indicated by origin:
            0 - start of the file
            1 - current file pointer
            2 - end of the file
            Thus, to move to the end of the file, call PClseek (fd, 0,2) and to move to
            100 bytes after the beginning use PClseek (fd, 100,0).
            Alternatively, you can specify #include <stdio.h> to obtain the following
            default values:
              SEEK_SET - start of the file
              SEEK_CUR - current file pointer
              SEEK_END - end of the file */

#ifdef __cplusplus
}
#endif

#endif /* _LIBSN_H_ */
