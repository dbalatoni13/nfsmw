//Very basic wchar.h

#ifndef __WCHAR_SUPPORT_H_
#define __WCHAR_SUPPORT_H_

#ifdef __cplusplus
extern "C" {
#endif

#define __need_ptrdiff_t
#define __need_wint_t
#define __need_wchar_t
#define __need_size_t
#include <stddef.h>

#include <stdio.h>

wchar_t* wcscpy(wchar_t *, const wchar_t *);
long	wcstol(const wchar_t *, const wchar_t **, int);
int 	wtoi(const wchar_t*);

#if defined (__APG__) || defined (__SNC__)
extern wchar_t *wcscpy (wchar_t *__dest,
			const wchar_t *__src);
/* Copy no more than N wide-characters of SRC to DEST.  */
extern wchar_t *wcsncpy (wchar_t *__dest,
			 const wchar_t *__src, size_t __n);

/* Append SRC onto DEST.  */
extern wchar_t *wcscat (wchar_t *__dest,
			const wchar_t *__src);
/* Append no more than N wide-characters of SRC onto DEST.  */
extern wchar_t *wcsncat (wchar_t *__dest,
			 const wchar_t *__src, size_t __n);
#else
extern wchar_t *wcscpy (wchar_t *__restrict __dest,
			const wchar_t *__restrict __src);
/* Copy no more than N wide-characters of SRC to DEST.  */
extern wchar_t *wcsncpy (wchar_t *__restrict __dest,
			 const wchar_t *__restrict __src, size_t __n);

/* Append SRC onto DEST.  */
extern wchar_t *wcscat (wchar_t *__restrict __dest,
			const wchar_t *__restrict __src);
/* Append no more than N wide-characters of SRC onto DEST.  */
extern wchar_t *wcsncat (wchar_t *__restrict __dest,
			 const wchar_t *__restrict __src, size_t __n);
#endif
/* Compare S1 and S2.  */
extern int wcscmp (const wchar_t *__s1, const wchar_t *__s2);
/* Compare N wide-characters of S1 and S2.  */
extern int wcsncmp (const wchar_t *__s1, const wchar_t *__s2, size_t __n);

/* Find the first occurrence of WC in WCS.  */
extern wchar_t *wcschr (const wchar_t *__wcs, wchar_t __wc);
/* Find the last occurrence of WC in WCS.  */
extern wchar_t *wcsrchr (const wchar_t *__wcs, wchar_t __wc);

/* Search N wide characters of S for C.  */
extern wchar_t *wmemchr (const wchar_t *__s, wchar_t __c, size_t __n);
/* Return the number of wide characters in S.  */
extern size_t wcslen (const wchar_t *__s);

extern int vfwprintf (FILE *, const wchar_t *, __gnuc_va_list);
extern int wprintf (const wchar_t *, ...);
extern int swprintf (wchar_t*, const wchar_t *, ...);
extern int fwprintf (FILE *, const wchar_t *, ...);
/* Write formatted output to stdout from argument list ARG.  */
extern int vwprintf (const wchar_t *, ...);
extern int vswprintf (wchar_t*, const wchar_t*, __gnuc_va_list);

extern int snwprintf (wchar_t*, size_t, const wchar_t *, ...);
extern int _snwprintf (wchar_t*, size_t, const wchar_t *, ...);


/* Return the number of wide characters in S, but at most MAXLEN.  */
extern size_t wcsnlen (const wchar_t *, size_t);

/* Compare N wide characters of S1 and S2.  */
extern int wmemcmp (const wchar_t *, const wchar_t *, size_t);

/* Copy N wide characters of SRC to DEST.  */
extern wchar_t *wmemcpy (wchar_t *, const wchar_t *, size_t);

/* Copy N wide characters of SRC to DEST, guaranteeing
   correct behavior for overlapping strings.  */
extern wchar_t *wmemmove (wchar_t *, const wchar_t*, size_t);

/* Set N wide characters of S to C.  */
extern wchar_t *wmemset (wchar_t *, wchar_t, size_t);

/* Copy N wide characters of SRC to DEST and return pointer to following
   wide character.  */
extern wchar_t *wmempcpy (wchar_t *, const wchar_t *, size_t);



//4 byte wchar version (check if Least Sig 2bytes are whitespace, and if rest of value is zero)
#define __ISWSPACE(Ch) (( ((Ch & 0xFF)>='\t' && (Ch & 0xFF) <= '\r') || (Ch & 0xFF)==' ' ) && ((Ch & 0xFFFFFF00) == 0))
#define __ISWALPHA(Ch) (( ((Ch & 0xFF)>='a' && (Ch & 0xFF) <= 'z') || ((Ch & 0xFF)>='A' && (Ch & 0xFF) <= 'Z') ) && ((Ch & 0xFFFFFF00) == 0))
#define __ISWUPPER(Ch) (( (Ch & 0xFF)>='A' && (Ch & 0xFF) <= 'Z') && ((Ch & 0xFFFFFF00) == 0))
#define __ISWDIGIT(Ch) (( (Ch & 0xFF)>='0' && (Ch & 0xFF) <= '9') && ((Ch & 0xFFFFFF00) == 0))
#define __ISWLOWER(Ch) (( (Ch & 0xFF)>='a' && (Ch & 0xFF) <= 'z') && ((Ch & 0xFFFFFF00) == 0))


/* Return the length of the initial segmet of WCS which
   consists entirely of wide characters not in REJECT.  */
extern size_t wcscspn (const wchar_t *, const wchar_t *);
/* Return the length of the initial segmet of WCS which
   consists entirely of wide characters in  ACCEPT.  */
extern size_t wcsspn (const wchar_t *, const wchar_t *);
/* Find the first occurrence in WCS of any character in ACCEPT.  */
extern wchar_t *wcspbrk (const wchar_t *, const wchar_t *);
/* Find the first occurrence of NEEDLE in HAYSTACK.  */
extern wchar_t *wcsstr (const wchar_t *, const wchar_t *);

extern int towlower (int);


#ifdef __USE_XOPEN
/* Another name for `wcsstr' from XPG4.  */
extern wchar_t *wcswcs (const wchar_t *__haystack, const wchar_t *__needle);
#endif

/* Divide WCS into tokens separated by characters in DELIM.  */


wchar_t *wcstok_r( register wchar_t *strToken, register const wchar_t *strDelimit, char** lasts );
wchar_t *wcstok( wchar_t *strToken, const wchar_t *strDelimit );


/*extern wchar_t *wcstok (register wchar_t *,
			register const wchar_t *);*/

#ifdef __cplusplus
}
#endif

#endif
