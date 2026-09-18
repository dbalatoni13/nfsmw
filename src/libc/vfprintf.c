/*
 * vfprintf.c de la libc de SN ProDG: es el de newlib con la salida reescrita
 * sobre `_vfwrite' (buffer estatico de 128 B volcado con `write') y con
 * separador de millares (`%'d' -> `add_separators').
 *
 * El fichero se compila DOS VECES: tal cual (coma flotante) y desde
 * `vfprintf_1.c' con INTEGER_ONLY definido, que es de donde sale
 * `_vfiprintf_r'.  Las dos copias comparten `_vfwrite'.
 */

#ifdef INTEGER_ONLY
#define _VFPRINTF_R _vfiprintf_r
#else
#define _VFPRINTF_R _vfprintf_r
#define FLOATING_POINT
#endif

#include <stdarg.h>

typedef unsigned int size_t;
typedef long long quad_t;
typedef unsigned long long u_quad_t;
typedef unsigned long u_long;
typedef unsigned int wchar_t;

struct __sbuf {
    unsigned char *_base;
    int _size;
};

struct __sFILE {
    unsigned char *_p; /* 0x00 */
    int _r; /* 0x04 */
    int _w; /* 0x08 */
    short _flags; /* 0x0c */
    short _file; /* 0x0e */
    struct __sbuf _bf; /* 0x10 */
    int _lbfsize; /* 0x18 */
    void *_cookie; /* 0x1c */
    int (*_read)(); /* 0x20 */
    int (*_write)(); /* 0x24 */
    int (*_seek)(); /* 0x28 */
    int (*_close)(); /* 0x2c */
    struct __sbuf _ub; /* 0x30 */
    unsigned char *_up; /* 0x38 */
    int _ur; /* 0x3c */
    unsigned char _ubuf[3]; /* 0x40 */
    unsigned char _nbuf[1]; /* 0x43 */
    struct __sbuf _lb; /* 0x44 */
    int _blksize; /* 0x4c */
    int _offset; /* 0x50 */
    void *_data2; /* 0x54 */
    int _flags2; /* 0x58 */
    struct _reent *_data; /* 0x5c */
};

typedef struct __sFILE FILE;

#define __SSTR 0x0200

extern void *memcpy(void *, const void *, size_t);
extern void *memchr(const void *, int, size_t);
extern size_t strlen(const char *);
extern int write(int, const void *, int);
extern int _mbtowc_r(struct _reent *, wchar_t *, const char *, size_t, int *);
extern struct _reent *_impure_ptr;
extern int __mb_cur_max;
extern char *alloca();

#define _REENT _impure_ptr
#define MB_CUR_MAX __mb_cur_max

/* vfprintf.c se compila dos veces (aqui y desde vfprintf_1.c, sin coma
 * flotante) y cada copia tiene sus propios estaticos. */
static char pch[128];
static int cumulative_written = 0;
static char *pch_pointer = pch;

static int _vfwrite(int fd, const char *buf, unsigned int len, int flush)
{
    unsigned int i;

    if (flush == 1) {
        int done = write(fd, pch, cumulative_written);
        cumulative_written = 0;
        pch_pointer = pch;
        return done;
    }

    for (i = 0; i < len; i++) {
        int done;

        *pch_pointer = buf[i];
        pch_pointer++;
        cumulative_written++;
        if (cumulative_written > 127) {
            done = write(fd, pch, cumulative_written);
            pch_pointer = pch;
            cumulative_written = 0;
            if (done == 0)
                return 0;
        }
    }

    return i;
}

#ifndef INTEGER_ONLY
extern int _vfiprintf_r();
int _vfprintf_r(struct _reent *, FILE *, const char *, va_list);
char *add_separators(char *, char *, char *, int *);

int vfprintf(FILE *fp, const char *fmt0, va_list ap)
{
    const char *p;

    for (p = fmt0; *p != '\0'; p++) {
        if (*p == '%' && p[1] != '\0') {
            p++;
            while (*p <= '@' && p[1] != '\0')
                p++;
            switch (*p) {
            case 'E':
            case 'G':
            case 'L':
            case 'e':
            case 'f':
            case 'g':
                return _vfprintf_r(fp->_data, fp, fmt0, ap);
            }
        }
    }

    return _vfiprintf_r(fp->_data, fp, fmt0, ap);
}
#else
extern char *add_separators(char *, char *, char *, int *);
#endif /* !INTEGER_ONLY */

/*
 * Actual printf innards.
 */

#ifdef FLOATING_POINT
extern double sn_log10(double);
extern void *localeconv(void);
extern char *fftoa(double, int, int, int, char *);

#define MAXEXP 308
#define MAXFRACT 39
#define MAXEXPLEN 8
#define BUF (MAXEXP + MAXFRACT + 1) /* + decimal point */
#else
#define BUF 40
#endif

#define ALT 0x001 /* alternate form */
#define HEXPREFIX 0x002 /* add 0x or 0X prefix */
#define LADJUST 0x004 /* left adjustment */
#define LONGDBL 0x008 /* long double */
#define LONGINT 0x010 /* long integer */
#define QUADINT 0x020 /* quad integer */
#define SHORTINT 0x040 /* short integer */
#define ZEROPAD 0x080 /* zero (as opposed to blank) pad */
#define FPT 0x100 /* Floating point number */
#define GROUPING 0x200 /* thousands separators */

#define to_digit(c) ((c) - '0')
#define is_digit(c) ((unsigned)to_digit(c) <= 9)
#define to_char(n) ((n) + '0')

#define PADSIZE 16 /* pad chunk size */
static const char blanks[PADSIZE]
    = { ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ',
          ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' };
static const char zeroes[PADSIZE]
    = { '0', '0', '0', '0', '0', '0', '0', '0',
          '0', '0', '0', '0', '0', '0', '0', '0' };

/* The SN libc object keeps each expansion as a separate control-flow region.
 * PRINT y PAD van como bloques `{ }' sueltos, que es como los escribe newlib:
 * el `do { } while (0)' anade un nivel de bucle y con el cambia la prioridad
 * (n_refs por loop_depth) de todo lo que hay dentro.  Ver r67-libc.md. */
#define PRINT(ptr, len)                            \
    {                                              \
        if (fp->_flags & __SSTR) {                 \
            if (fp->_w > (len)) {                  \
                memcpy(fp->_p, (ptr), (len));      \
                fp->_p += (len);                   \
                fp->_w -= (len);                   \
            } else {                               \
                memcpy(fp->_p, (ptr), fp->_w);     \
                fp->_p += fp->_w;                  \
                fp->_w = 0;                        \
            }                                      \
        } else {                                   \
            if (fp->_flags == 0 && fp->_file == 0) \
                fp->_file = 1;                     \
            _vfwrite(fp->_file, (ptr), (len), 0);  \
        }                                          \
    }

#define PAD(howmany, with)            \
    {                                 \
        if ((n = (howmany)) > 0) {    \
            while (n > PADSIZE) {     \
                PRINT(with, PADSIZE); \
                n -= PADSIZE;         \
            }                         \
            PRINT(with, n);           \
        }                             \
    }

#define FLUSH()                               \
    do {                                      \
        if (!(fp->_flags & __SSTR))           \
            _vfwrite(fp->_file, 0, 0, 1);     \
    } while (0)

/*
 * To extend shorts properly, we need both signed and unsigned
 * argument extraction methods.
 */
#define SARG()                                                \
    (flags & QUADINT ? va_arg(ap, quad_t)                     \
                     : flags & LONGINT ? (quad_t)va_arg(ap, long) \
                                       : flags & SHORTINT     \
                     ? (quad_t)(short)va_arg(ap, int)         \
                     : (quad_t)va_arg(ap, int))
#define UARG()                                                     \
    (flags & QUADINT ? va_arg(ap, u_quad_t)                        \
                     : flags & LONGINT ? (u_quad_t)va_arg(ap, u_long) \
                                       : flags & SHORTINT          \
                     ? (u_quad_t)(unsigned short)va_arg(ap, int)   \
                     : (u_quad_t)va_arg(ap, unsigned int))

int _VFPRINTF_R(struct _reent *data, FILE *fp0, const char *fmt0, va_list ap0)
{

    register FILE *fp = fp0;
    register __typeof__(ap0) ap = ap0;
#ifdef FLOATING_POINT
    register const char *fmt; /* format string */
#else
    register const char *fmt = fmt0; /* format string */
#endif
    register int ch; /* character from fmt */
    register int n, m; /* handy integers (short term usage) */
    register char *cp; /* handy char pointer (short term usage) */
    register int flags; /* flags as above */
    register int ret; /* return value accumulator */
    register int width; /* width from format (%8d), or 0 */
    register int prec; /* precision from format (%.3d), or -1 */
    char sign; /* sign prefix (' ', '+', '-', or \0) */
    wchar_t wc;
    int state;

#ifdef FLOATING_POINT
    void *decimal_point = localeconv();
    double _fpvalue; /* floating point arguments %[eEfgG] */
    int trim;
    /* El objetivo reserva 8 B de marco delante de `buf' (buf en 0x10, no en
     * 0x8) para un objeto de 8 B en memoria y muerto.  `volatile' es lo unico
     * que lo reproduce: un array <= 8 B sin volatile se va a pseudo y
     * desaparece, y tomar su direccion en un `if (0)' tampoco lo reserva. */
    volatile char expstr[MAXEXPLEN]; /* buffer for exponent string */
#endif
    u_quad_t _uquad; /* integer arguments %[diouxX] */
    enum { OCT, DEC, HEX } base; /* base for [diouxX] conversion */
    register int dprec; /* a copy of prec if [diouxX], 0 otherwise */
    int realsz; /* field size expanded by dprec */
    int size; /* size of converted field or string */
    register char *number_end;
    const char *xdigs; /* digits for [xX] conversion */
    char buf[BUF]; /* space for %c, %[diouxX], %[eEfgG] */
    char ox[2]; /* space for 0x hex-prefix */

    state = 0;
#ifdef FLOATING_POINT
    fmt = fmt0;
#endif
    ret = 0;

    /*
     * Scan the format for conversions (`%' character).
     */
    for (;;) {
        cp = (char *)fmt;
        while ((n = _mbtowc_r(_REENT, &wc, fmt, MB_CUR_MAX, &state)) > 0) {
            fmt += n;
            if (wc == '%') {
                fmt--;
                break;
            }
        }
        if ((m = fmt - cp) != 0) {
            PRINT(cp, m);
            ret += m;
        }
        if (n <= 0)
            goto done;
        if (fp->_w <= 0 && (fp->_flags & __SSTR))
            goto error;
        width = 0;
        fmt++; /* skip over '%' */
        sign = width;

        flags = 0;
        dprec = 0;

        prec = -1;
    nextch:
        ch = *fmt++;
    reswitch:
        switch (ch) {
        case ' ':
            /*
             * ``If the space and + flags both appear, the space
             * flag will be ignored.''
             *	-- ANSI X3J11
             */
            if (sign)
                goto nextch;
            /* FALLTHROUGH */
        case '+':
            sign = ch;
            goto nextch;
        case '#':
            flags |= ALT;
            goto nextch;
        case '\'':
            flags |= GROUPING;
            goto nextch;
        case '*':
            /*
             * ``A negative field width argument is taken as a
             * - flag followed by a positive field width.''
             *	-- ANSI X3J11
             * They don't exclude field widths read from args.
             */
            if ((width = va_arg(ap, int)) >= 0)
                goto nextch;
            width = -width;
            /* FALLTHROUGH */
        case '-':
            flags |= LADJUST;
            goto nextch;
        case '.':
            ch = *fmt++;
            if (ch == '*') {
                n = va_arg(ap, int);
                prec = n < 0 ? -1 : n;
                goto nextch;
            }
            n = 0;
            while (is_digit(ch)) {
                n = 10 * n + to_digit(ch);
                ch = *fmt++;
            }
            prec = n;
            if (prec < -1)
                prec = -1;
            goto reswitch;
        case '0':
            /*
             * ``Note that 0 is taken as a flag, not as the
             * beginning of a field width.''
             *	-- ANSI X3J11
             */
            flags |= ZEROPAD;
            goto nextch;
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            n = 0;
            do {
                n = 10 * n + to_digit(ch);
                ch = *fmt++;
            } while (is_digit(ch));
            width = n;
            goto reswitch;
#ifdef FLOATING_POINT
        case 'L':
            flags |= LONGDBL;
            goto nextch;
#endif
        case 'h':
            flags |= SHORTINT;
            goto nextch;
        case 'l':
            if (*fmt == 'l') {
                fmt++;
                flags |= QUADINT;
            } else {
                flags |= LONGINT;
            }
            goto nextch;
        case 'q':
            flags |= QUADINT;
            goto nextch;
        case 'c':
            *(cp = buf) = va_arg(ap, int);
            size = 1;
            sign = '\0';
            break;
        case 'D':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'd':
        case 'i':
            _uquad = SARG();
            if ((quad_t)_uquad < 0) {
                _uquad = -_uquad;
                sign = '-';
            }
            base = DEC;
            goto number;
#ifdef FLOATING_POINT
        case 'e':
        case 'E':
        case 'f':
        case 'g':
        case 'G':
            trim = 0;
            if (prec == -1)
                prec = 6;
            _fpvalue = va_arg(ap, double);
            if (ch == 'g' || ch == 'G') {
                double expt;

                if (_fpvalue != 0.0)
                    expt = sn_log10(_fpvalue < 0.0 ? -_fpvalue : _fpvalue);
                else
                    expt = 1.0;
                if (expt < -4.0 || expt >= prec) {
                    ch = ch == 'g' ? 'e' : 'E';
                } else
                    ch = 'f';
                trim = 1;
            }
            if (flags & ALT) {
                if (prec == 0)
                    prec = 1;
                trim = 0;
            }
            cp = fftoa(_fpvalue, prec, (char)ch, trim, &sign);
            size = strlen(cp);
            if (flags & GROUPING)
                cp = add_separators(cp, cp, cp + size, &size);
            break;
#endif /* FLOATING_POINT */
        case 'n':
            if (flags & QUADINT)
                *va_arg(ap, quad_t *) = ret;
            else if (flags & LONGINT)
                *va_arg(ap, long *) = ret;
            else if (flags & SHORTINT)
                *va_arg(ap, short *) = ret;
            else
                *va_arg(ap, int *) = ret;
            continue; /* no output */
        case 'O':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'o':
            _uquad = UARG();
            base = OCT;
            goto nosign;
        case 'p':
            /*
             * ``The argument shall be a pointer to void.  The
             * value of the pointer is converted to a sequence
             * of printable characters, in an implementation-
             * defined manner.''
             *	-- ANSI X3J11
             */
            /* NOSTRICT */
            _uquad = (u_long)va_arg(ap, void *);
            base = HEX;
            xdigs = "0123456789abcdef";
            flags |= HEXPREFIX;
            ch = 'x';
            goto nosign;
        case 's':
            if ((cp = va_arg(ap, char *)) == 0)
                cp = "(null)";
            if (prec >= 0) {
                /*
                 * can't use strlen; can only look for the
                 * NUL in the first `prec' characters, and
                 * strlen() will go further.
                 */
                char *p = memchr(cp, 0, prec);

                if (p != 0) {
                    size = p - cp;
                    if (size > prec)
                        size = prec;
                } else
                    size = prec;
            } else
                size = strlen(cp);
            sign = '\0';
            break;
        case 'U':
            flags |= LONGINT;
            /*FALLTHROUGH*/
        case 'u':
            _uquad = UARG();
            base = DEC;
            goto nosign;
        case 'X':
            xdigs = "0123456789ABCDEF";
            goto hex;
        case 'x':
            xdigs = "0123456789abcdef";
        hex:
            _uquad = UARG();
            base = HEX;
            /* leading 0x/X only if non-zero */
            if (flags & ALT && _uquad != 0)
                flags |= HEXPREFIX;

            /* unsigned conversions */
        nosign:
            sign = '\0';
            /*
             * ``... diouXx conversions ... if a precision is
             * specified, the 0 flag will be ignored.''
             *	-- ANSI X3J11
             */
        number:
            if ((dprec = prec) >= 0)
                flags &= ~ZEROPAD;

            /*
             * ``The result of converting a zero value with an
             * explicit precision of zero is no characters.''
             *	-- ANSI X3J11
             */
            cp = buf + BUF;
            number_end = cp;
            if (_uquad != 0 || dprec != 0) {
                /*
                 * Unsigned mod is hard, and unsigned mod
                 * by a constant is easier than that by
                 * a variable; hence this switch.
                 */
                switch (base) {
                case OCT:
                    do {
                        *--cp = to_char(_uquad & 7);
                        _uquad >>= 3;
                    } while (_uquad);
                    /* handle octal leading 0 */
                    if (flags & ALT && *cp != '0')
                        *--cp = '0';
                    break;

                case DEC:
                    /* many numbers are 1 digit */
                    while (_uquad >= 10) {
                        *--cp = to_char(_uquad % 10);
                        _uquad /= 10;
                    }
                    *--cp = to_char(_uquad);
                    break;

                case HEX:
                    do {
                        *--cp = xdigs[_uquad & 15];
                        _uquad >>= 4;
                    } while (_uquad);
                    break;

                default:
                    cp = "bug in vfprintf: bad base";
                    size = strlen(cp);
                    goto skipsize;
                }
            }
            if (base == DEC && (flags & GROUPING))
                cp = add_separators(buf, cp, number_end, &size);
            else
                size = buf + BUF - cp;
        skipsize:
            break;
        default: /* "%?" prints ?, unless ? is NUL */
            if (ch == '\0')
                goto done;
            /* pretend it was %c with argument ch */
            cp = buf;
            *cp = ch;
            size = 1;
            sign = '\0';
            break;
        }

        /*
         * All reasonable formats wind up here.  At this point, `cp'
         * points to a string which (if not flags&LADJUST) should be
         * padded out to `width' places.  If flags&ZEROPAD, it should
         * first be prefixed by any sign or other prefix; otherwise,
         * it should be blank padded before the prefix is emitted.
         * After any left-hand padding and prefixing, emit zeroes
         * required by a decimal [diouxX] precision, then print the
         * string proper, then emit zeroes required by any leftover
         * floating precision; finally, if LADJUST, pad with blanks.
         *
         * Compute actual size, so we know how much to pad.
         * size excludes decimal prec; realsz includes it.
         */
        /* Keep the original SN register lifetimes across the output macros. */
#ifndef FLOATING_POINT
#endif
        realsz = dprec > size ? dprec : size;
        if (sign)
            realsz++;
        else if (flags & HEXPREFIX)
            realsz += 2;

        /* right-adjusting blank padding */
        if ((flags & (LADJUST | ZEROPAD)) == 0)
            PAD(width - realsz, blanks);

        /* Los dos escritores de prefijo comparten la actualizacion final del
         * buffer.  El `goto' cruzado NO es adorno: es lo unico que reproduce el
         * empalme de colas del original.  MEDIDO Y NEGATIVO (r65): la forma
         * limpia de newlib --`if (sign) PRINT(&sign, 1); else if (flags &
         * HEXPREFIX) { ox[0] = '0'; ox[1] = ch; PRINT(ox, 2); }'-- compila
         * 16 B DE MAS porque GCC 2.95 no cruza esas dos colas; cuatro variantes
         * (con y sin barrera sobre `ch', leyendo _flags antes o no) dan los
         * mismos 16 B.  Las cuatro barreras de dentro son IRREDUCIBLES por el
         * mismo reparto global de la cabecera de la funcion. */
        {
            register unsigned char *prefix_p;
            register int prefix_w;
            unsigned char *prefix_old_p;
            int prefix_old_w;
            if (sign) {
                if (fp->_flags & __SSTR) {
                    if (fp->_w > 1) {
                        memcpy(fp->_p, &sign, 1);
                        prefix_p = fp->_p + 1;
                        prefix_w = fp->_w - 1;
                        goto prefix_store;
                    } else {
                        memcpy(fp->_p, &sign, fp->_w);
                        prefix_old_p = fp->_p;
                        prefix_old_w = fp->_w;
                        fp->_w = 0;
                        goto prefix_advance;
                    }
                } else {
                    if (fp->_flags == 0 && fp->_file == 0)
                        fp->_file = 1;
                    _vfwrite(fp->_file, &sign, 1, 0);
                }
                goto prefix_finished;
            } else if (flags & HEXPREFIX) {
                register unsigned int prefix_flags = (unsigned short)fp->_flags;
                ox[0] = '0';
                ox[1] = ch;
                if (prefix_flags & __SSTR) {
                    if (fp->_w > 2) {
                        memcpy(fp->_p, ox, 2);
                        prefix_p = fp->_p + 2;
                        prefix_w = fp->_w - 2;
                    prefix_store:
                        fp->_p = prefix_p;
                        fp->_w = prefix_w;
                        goto prefix_finished;
                    } else {
                        memcpy(fp->_p, ox, fp->_w);
                        prefix_old_p = fp->_p;
                        prefix_old_w = fp->_w;
                        fp->_w = 0;
                    prefix_advance:
                        fp->_p = prefix_old_p + prefix_old_w;
                        goto prefix_finished;
                    }
                } else {
                    if (fp->_flags == 0 && fp->_file == 0)
                        fp->_file = 1;
                    _vfwrite(fp->_file, ox, 2, 0);
                }
            prefix_finished:;
            }
        }

        /* right-adjusting zero padding */
        if ((flags & (LADJUST | ZEROPAD)) == ZEROPAD)
            PAD(width - realsz, zeroes);

        /* leading zeroes from decimal precision */
        PAD(dprec - size, zeroes);

        /* the string or number proper */
        if (fp->_flags & __SSTR) {
            if (fp->_w > size) {
                memcpy(fp->_p, cp, size);
                fp->_p += size;
                fp->_w -= size;
            } else {
                memcpy(fp->_p, cp, fp->_w);
                fp->_p += fp->_w;
                fp->_w = 0;
            }
        } else {
            if (fp->_flags == 0 && fp->_file == 0)
                fp->_file = 1;
            _vfwrite(fp->_file, cp, size, 0);
        }

        /* left-adjusting padding (always blank) */
        if (flags & LADJUST)
            PAD(width - realsz, blanks);

        /* finally, adjust ret */
        ret += width > realsz ? width : realsz;
    }
done:
    FLUSH();
error:
    return ret;
}

#ifndef INTEGER_ONLY
char *add_separators(char *dst, char *start, char *end, int *lenp)
{
    int n = end - start + 10;
    char *buf = alloca(end - start + 10);
    char *p = buf + n - 1;
    char *q;
    register char *mark;
    /* IRREDUCIBLES (r65): PERMUTACION pura.  El original reparte state=r7,
     * mark=r8, count=r12, p=r6; sin los pines nos sale state=r9, mark=r3,
     * count=r6, p=r8 -- las mismas instrucciones con otros registros.  MEDIDO Y
     * NEGATIVO: seis formas de declaracion (mark/state/count en tres ordenes,
     * con y sin `register', con `int state = 0' y con `mark = end' suelto)
     * dan EL MISMO objeto equivocado.  El orden de la fuente no llega a
     * allocno_compare.
     * r67: siguen igual sobre la fuente de hoy: sin `mark' 15 filas, sin
     * `state' 17 (el cambio de PRINT/PAD no toca esta funcion). */
    register int state;
    register int count = 0;

    if (dst == 0 || start == 0 || end == 0)
        return 0;

    state = 0;
    for (q = start, mark = end; q <= end && state != 2; q++) {
        switch (*q) {
        case '.':
            mark = q;
            if (state == 0)
                state = 1;
            break;
        case 'e':
        case 'E':
            state = 2;
            break;
        }
    }

    if (state == 2)
        return start;

    for (q = end; q > start; q--) {
        *p-- = *q;
        if (q <= mark) {
            if (count % 3 == 0 && count != 0) {
                *p-- = ',';
                n++;
            }
            count++;
        }
    }
    /* The original keeps the scan state live through the insertion loop. */
    *p = *q;
    n -= 10;
    memcpy(dst, p, n);
    *lenp = n;
    return dst;
}
#endif /* !INTEGER_ONLY */
