/*
 * tix.h --
 *
 *	This is the standard header file for all tix C code. It
 *	defines many macros and utility functions to make it easier to
 *	write TCL commands and TK widgets in C. No more needs to write
 *	2000 line functions!
 *
 * Copyright (c) 1996, Expert Interface Technologies
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef _TIX_H_
#define _TIX_H_

#ifndef TIX_VERSION
#define TIX_VERSION	"4.1"
#endif
#define TIX_PATCHLEVEL	"4.1.0"
#define TIX_PATCH_LEVEL TIX_PATCHLEVEL

#define TIX_RELEASE     "4.1.0.005"

#ifndef RC_INVOKED

#ifndef _TCL
#include <tcl.h>
#endif

#ifndef _TK
#include <tk.h>
#endif


#if defined(__WIN32__) || defined(_WIN32) || defined (__BORLAND) || defined(_Windows)
#ifndef _WINDOWS
#define _WINDOWS
#endif
#endif

#ifdef BUILD_tix
# undef TCL_STORAGE_CLASS
# define TCL_STORAGE_CLASS DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif

#if (TCL_MAJOR_VERSION > 7)
#  define TCL_7_5_OR_LATER
#else
#  if ((TCL_MAJOR_VERSION == 7) && (TCL_MINOR_VERSION >= 5))
#    define TCL_7_5_OR_LATER
#  endif
#endif


#if (TK_MAJOR_VERSION > 4)
#  define TK_4_1_OR_LATER
#else
#  if ((TK_MAJOR_VERSION == 4) && (TK_MINOR_VERSION >= 1))
#    define TK_4_1_OR_LATER
#  endif
#endif /* TK_MAJOR_VERSION ... */

#if (TK_MAJOR_VERSION >= 8)
#  define TK_8_0_OR_LATER
#endif

#ifdef TK_4_1_OR_LATER
    /* TK 4.1 or later */
#  define Tix_FreeProc Tcl_FreeProc

#else
    /* TK 4.0 */
#  define Tix_FreeProc Tk_FreeProc

    /* These portable features were not defined in previous versions of
     * TK but are used in Tix. Let's define them here.
     */
#  define TkPutImage(a, b, c, d, e, f, g, h, i, j, k, l) \
	XPutImage(c, d, e, f, g, h, i, j, k, l)

#  define TkStringToKeysym XStringToKeysym 

#endif /* TK_4_1_OR_LATER */


#define TIX_STDIN_ALWAYS	0
#define TIX_STDIN_OPTIONAL	1
#define TIX_STDIN_NONE		2

typedef struct {
    char *name;			/* Name of command. */
    int (*cmdProc) _ANSI_ARGS_((ClientData clientData, Tcl_Interp *interp,
				int argc, char **argv));
				/* Command procedure. */
} Tix_TclCmd;


/*----------------------------------------------------------------------
 *
 *
 * 			SUB-COMMAND HANDLING
 *
 *
 *----------------------------------------------------------------------
 */
typedef int (*Tix_CmdProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char ** argv));
typedef int (*Tix_SubCmdProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char ** argv));
typedef int (*Tix_CheckArgvProc) _ANSI_ARGS_((ClientData clientData,
	Tcl_Interp *interp, int argc, char ** argv));

typedef struct _Tix_CmdInfo {
    int		numSubCmds;
    int		minargc;
    int		maxargc;
    char      * info;
} Tix_CmdInfo;

typedef struct _Tix_SubCmdInfo {
    int			namelen;
    char      	      * name;
    int			minargc;
    int			maxargc;
    Tix_SubCmdProc 	proc;
    char      	      * info;
    Tix_CheckArgvProc   checkArgvProc;
} Tix_SubCmdInfo;

/*
 * Tix_ArraySize --
 *
 *	Find out the number of elements inside a C array. The argument "x"
 * must be a valid C array. Pointers don't work.
 */
#define Tix_ArraySize(x) (sizeof(x) / sizeof(x[0]))

/*
 * This is used for Tix_CmdInfo.maxargc and Tix_SubCmdInfo.maxargc,
 * indicating that this command takes a variable number of arguments.
 */
#define TIX_VAR_ARGS	       -1

/*
 * TIX_DEFAULT_LEN --
 *
 * Use this for Tix_SubCmdInfo.namelen and Tix_ExecSubCmds() will try to
 * determine the length of the subcommand name for you.
 */
#define TIX_DEFAULT_LEN	       -1

/*
 * TIX_DEFAULT_SUB_CMD --
 *
 * Use this for Tix_SubCmdInfo.name. This will match any subcommand name,
 * including the empty string, when Tix_ExecSubCmds() finds a subcommand
 * to execute.
 */
#define TIX_DEFAULT_SUBCMD	0

/*
 * TIX_DECLARE_CMD --
 *
 * This is just a handy macro to declare a C function to use as a
 * command function.
 */
#define TIX_DECLARE_CMD(func) \
    int func _ANSI_ARGS_((ClientData clientData,\
	Tcl_Interp *interp, int argc, char ** argv))

/*
 * TIX_DECLARE_SUBCMD --
 *
 * This is just a handy macro to declare a C function to use as a
 * sub command function.
 */
#define TIX_DECLARE_SUBCMD(func) \
    int func _ANSI_ARGS_((ClientData clientData,\
	Tcl_Interp *interp, int argc, char ** argv))

/*
 * TIX_DEFINE_CMD --
 *
 * This is just a handy macro to define a C function to use as a
 * command function.
 */
#define TIX_DEFINE_CMD(func) \
int func(clientData, interp, argc, argv) \
    ClientData clientData;	/* Main window associated with 	\
				 * interpreter. */		\
    Tcl_Interp *interp;		/* Current interpreter. */	\
    int argc;			/* Number of arguments. */	\
    char **argv;		/* Argument strings. */


/*----------------------------------------------------------------------
 * Link-list functions --
 *
 *	These functions makes it easy to use link lists in C code.
 *
 *----------------------------------------------------------------------
 */
typedef struct Tix_ListInfo {
    int nextOffset;		/* offset of the "next" pointer in a list
				 * item */
    int prevOffset;		/* offset of the "next" pointer in a list
				 * item */
} Tix_ListInfo;


/* Singly-linked list */
typedef struct Tix_LinkList {
    int numItems;		/* number of items in this list */
    char * head;		/* (general pointer) head of the list */
    char * tail;		/* (general pointer) tail of the list */
} Tix_LinkList;

typedef struct Tix_ListIterator {
    char * last;
    char * curr;
    unsigned int started : 1;   /* True if the search operation has
				 * already started for this list */
    unsigned int deleted : 1;	/* True if a delete operation has been
				 * performed on the current item (in this
				 * case the curr pointer has already been
				 * adjusted
				 */
} Tix_ListIterator;

#define Tix_IsLinkListEmpty(list)  ((list.numItems) == 0)
#define TIX_UNIQUE 1
#define TIX_UNDEFINED -1

/*----------------------------------------------------------------------
 * General Single Link List --
 *
 *	The next pointer can be anywhere inside a link.
 *----------------------------------------------------------------------
 */

EXTERN void 		Tix_LinkListInit _ANSI_ARGS_((Tix_LinkList * lPtr));
EXTERN void		Tix_LinkListAppend _ANSI_ARGS_((Tix_ListInfo * infoPtr,
			    Tix_LinkList * lPtr, char * itemPtr, int flags));
EXTERN void		Tix_LinkListStart _ANSI_ARGS_((Tix_ListInfo * infoPtr,
			    Tix_LinkList * lPtr, Tix_ListIterator * liPtr));
EXTERN void		Tix_LinkListNext _ANSI_ARGS_((Tix_ListInfo * infoPtr,
			    Tix_LinkList * lPtr, Tix_ListIterator * liPtr));
EXTERN void		Tix_LinkListDelete _ANSI_ARGS_((Tix_ListInfo * infoPtr,
			    Tix_LinkList * lPtr, Tix_ListIterator * liPtr));
EXTERN int		Tix_LinkListDeleteRange _ANSI_ARGS_((
			    Tix_ListInfo * infoPtr, Tix_LinkList * lPtr,
			    char * fromPtr, char * toPtr,
			    Tix_ListIterator * liPtr));
EXTERN int		Tix_LinkListFind _ANSI_ARGS_((
			    Tix_ListInfo * infoPtr, Tix_LinkList * lPtr,
			    char * itemPtr, Tix_ListIterator * liPtr));
EXTERN int		Tix_LinkListFindAndDelete _ANSI_ARGS_((
			    Tix_ListInfo * infoPtr, Tix_LinkList * lPtr,
			    char * itemPtr, Tix_ListIterator * liPtr));
EXTERN void		Tix_LinkListInsert _ANSI_ARGS_((
			    Tix_ListInfo * infoPtr,
			    Tix_LinkList * lPtr, char * itemPtr,
			    Tix_ListIterator * liPtr));
EXTERN void		Tix_LinkListIteratorInit _ANSI_ARGS_((
			    Tix_ListIterator * liPtr));

#define Tix_LinkListDone(liPtr) ((liPtr)->curr == NULL)


/*----------------------------------------------------------------------
 * Simple Single Link List --
 *
 *	The next pointer is always offset 0 in the link structure.
 *----------------------------------------------------------------------
 */

EXTERN void 		Tix_SimpleListInit _ANSI_ARGS_((Tix_LinkList * lPtr));
EXTERN void		Tix_SimpleListAppend _ANSI_ARGS_((
			    Tix_LinkList * lPtr, char * itemPtr, int flags));
EXTERN void		Tix_SimpleListStart _ANSI_ARGS_((
			    Tix_LinkList * lPtr, Tix_ListIterator * liPtr));
EXTERN void		Tix_SimpleListNext _ANSI_ARGS_((
			    Tix_LinkList * lPtr, Tix_ListIterator * liPtr));
EXTERN void		Tix_SimpleListDelete _ANSI_ARGS_((
			    Tix_LinkList * lPtr, Tix_ListIterator * liPtr));
EXTERN int		Tix_SimpleListDeleteRange _ANSI_ARGS_((
			    Tix_LinkList * lPtr,
			    char * fromPtr, char * toPtr,
			    Tix_ListIterator * liPtr));
EXTERN int		Tix_SimpleListFind _ANSI_ARGS_((
			    Tix_LinkList * lPtr,
			    char * itemPtr, Tix_ListIterator * liPtr));
EXTERN int		Tix_SimpleListFindAndDelete _ANSI_ARGS_((
			    Tix_LinkList * lPtr, char * itemPtr,
			    Tix_ListIterator * liPtr));
EXTERN void		Tix_SimpleListInsert _ANSI_ARGS_((
			    Tix_LinkList * lPtr, char * itemPtr,
			    Tix_ListIterator * liPtr));
EXTERN void		Tix_SimpleListIteratorInit _ANSI_ARGS_((
			    Tix_ListIterator * liPtr));

#define Tix_SimpleListDone(liPtr) ((liPtr)->curr == NULL)

/*----------------------------------------------------------------------
 *
 *
 *
 *  			CUSTOM CONFIG OPTIONS
 *
 *
 *----------------------------------------------------------------------
 */
#define TIX_RELIEF_RAISED	1
#define TIX_RELIEF_FLAT		2
#define TIX_RELIEF_SUNKEN	4
#define TIX_RELIEF_GROOVE	8
#define TIX_RELIEF_RIDGE	16
#define TIX_RELIEF_SOLID	32

typedef int Tix_Relief;

extern Tk_CustomOption tixConfigItemType;
extern Tk_CustomOption tixConfigItemStyle;
extern Tk_CustomOption tixConfigRelief;

/*
 * C functions exported by Tix
 */

EXTERN int		Tix_ArgcError _ANSI_ARGS_((Tcl_Interp *interp, 
			    int argc, char ** argv, int prefixCount,
			    char *message));
EXTERN void		Tix_CreateCommands _ANSI_ARGS_((
			    Tcl_Interp *interp, Tix_TclCmd *commands,
			    ClientData clientData,
			    Tcl_CmdDeleteProc *deleteProc));
EXTERN Tk_Window	Tix_CreateSubWindow _ANSI_ARGS_((
			    Tcl_Interp * interp, Tk_Window tkwin,
			    char * subPath));
EXTERN int		Tix_DefinePixmap _ANSI_ARGS_((
			    Tcl_Interp * interp, Tk_Uid name, char **data));
EXTERN void		Tix_DrawAnchorLines _ANSI_ARGS_((
			    Display *display, Drawable drawable,
			    GC gc, int x, int y, int w, int h));
EXTERN int		Tix_EvalArgv _ANSI_ARGS_((
    			    Tcl_Interp * interp, int argc, char ** argv));
EXTERN int 		Tix_ExistMethod _ANSI_ARGS_((Tcl_Interp *interp,
			    char *context, char *method));
EXTERN void		Tix_Exit _ANSI_ARGS_((Tcl_Interp * interp, int code));
EXTERN Pixmap		Tix_GetRenderBuffer _ANSI_ARGS_((Display *display,
			    Drawable d, int width, int height, int depth));

#ifdef TCL_VARARGS
/*
 * The TCL_VARARGS macro is only defined in Tcl 7.5 or later
 */
EXTERN int		Tix_GlobalVarEval _ANSI_ARGS_(
			    TCL_VARARGS(Tcl_Interp *,interp));
#else
EXTERN int		Tix_GlobalVarEval _ANSI_ARGS_(
			    VARARGS(Tcl_Interp *interp));
#endif

EXTERN int		Tix_HandleSubCmds _ANSI_ARGS_((
			    Tix_CmdInfo * cmdInfo,
			    Tix_SubCmdInfo * subCmdInfo,
			    ClientData clientData, Tcl_Interp *interp,
			    int argc, char **argv));
EXTERN int 		Tix_Init _ANSI_ARGS_((Tcl_Interp *interp));

EXTERN int		Tix_LoadTclLibrary _ANSI_ARGS_((
			    Tcl_Interp *interp, char *envName,
			    char *tclName, char *initFile,
			    char *defDir, char * appName));
EXTERN void 		Tix_OpenStdin _ANSI_ARGS_((Tcl_Interp *interp));
EXTERN void 		Tix_SetArgv _ANSI_ARGS_((Tcl_Interp *interp, 
			    int argc, char **argv));
EXTERN void		Tix_SetRcFileName _ANSI_ARGS_((
			    Tcl_Interp * interp,  char * rcFileName));


/*
 * Commands exported by Tix
 *
 */

extern TIX_DECLARE_CMD(Tix_CallMethodCmd);
extern TIX_DECLARE_CMD(Tix_ChainMethodCmd);
extern TIX_DECLARE_CMD(Tix_ClassCmd);
extern TIX_DECLARE_CMD(Tix_DoWhenIdleCmd);
extern TIX_DECLARE_CMD(Tix_DoWhenMappedCmd);
extern TIX_DECLARE_CMD(Tix_FalseCmd);
extern TIX_DECLARE_CMD(Tix_FileCmd);
extern TIX_DECLARE_CMD(Tix_FlushXCmd);
extern TIX_DECLARE_CMD(Tix_FormCmd);
extern TIX_DECLARE_CMD(Tix_GridCmd);
extern TIX_DECLARE_CMD(Tix_GeometryRequestCmd);
extern TIX_DECLARE_CMD(Tix_Get3DBorderCmd);
extern TIX_DECLARE_CMD(Tix_GetBooleanCmd);
extern TIX_DECLARE_CMD(Tix_GetIntCmd);
extern TIX_DECLARE_CMD(Tix_GetMethodCmd);
extern TIX_DECLARE_CMD(Tix_HListCmd);
extern TIX_DECLARE_CMD(Tix_HandleOptionsCmd);
extern TIX_DECLARE_CMD(Tix_InputOnlyCmd);
extern TIX_DECLARE_CMD(Tix_ItemStyleCmd);
extern TIX_DECLARE_CMD(Tix_ManageGeometryCmd);
extern TIX_DECLARE_CMD(Tix_MapWindowCmd);
extern TIX_DECLARE_CMD(Tix_MoveResizeWindowCmd);
extern TIX_DECLARE_CMD(Tix_NoteBookFrameCmd);
extern TIX_DECLARE_CMD(Tix_RaiseWindowCmd);
extern TIX_DECLARE_CMD(Tix_ShellInputCmd);
extern TIX_DECLARE_CMD(Tix_StringSubCmd);
extern TIX_DECLARE_CMD(Tix_StrEqCmd);
extern TIX_DECLARE_CMD(Tix_TListCmd);
extern TIX_DECLARE_CMD(Tix_TmpLineCmd);
extern TIX_DECLARE_CMD(Tix_TrueCmd);
extern TIX_DECLARE_CMD(Tix_UnmapWindowCmd);
extern TIX_DECLARE_CMD(Tix_MwmCmd);
extern TIX_DECLARE_CMD(Tix_CreateWidgetCmd);

#define SET_RECORD(interp, record, var, value) \
	Tcl_SetVar2(interp, record, var, value, TCL_GLOBAL_ONLY)

#define GET_RECORD(interp, record, var) \
	Tcl_GetVar2(interp, record, var, TCL_GLOBAL_ONLY)


#define TIX_HASHKEY(k) ((sizeof(k)>sizeof(int))?((char*)&(k)):((char*)(k)))

/*----------------------------------------------------------------------
 * Compatibility section
 *----------------------------------------------------------------------	*/

EXTERN char * 		tixStrDup _ANSI_ARGS_((
			    CONST char * s));

#ifdef _WINDOWS
#ifndef NO_STRCASECMP
#define NO_STRCASECMP 1
#endif
#endif

#if defined(NO_STRCASECMP)
#  ifndef strcasecmp
#    define strcasecmp tixStrCaseCmp
#  endif
extern int		tixStrCaseCmp _ANSI_ARGS_((CONST char *s1,
			    CONST char *s2));
#endif


#ifdef __cplusplus
}
#endif

#undef TCL_STORAGE_CLASS
#define TCL_STORAGE_CLASS DLLIMPORT

#endif /* RC_INVOKED */
#endif /* _TIX_H_ */
